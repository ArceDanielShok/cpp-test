#include <node_api.h>
#include "Watcher.h"
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <filesystem>

using namespace std;

struct WatcherData {
    FolderWatcher folderWatcher;
    napi_ref callbackRef;
    napi_env env;
};

struct AsyncContext {
    napi_env env;
    napi_ref callbackRef;
    std::string folderPath;
};

std::vector<std::filesystem::path> GetCurrentFiles(const std::filesystem::path& directory);
void DetectFileChanges(napi_env env, napi_value jsCallback, const std::vector<std::filesystem::path>& oldFiles, const std::vector<std::filesystem::path>& newFiles);
void NotifyFileChange(napi_env env, napi_value jsCallback, const std::filesystem::path& filePath);

static int initializationCount = 0;
void PrintToConsole(napi_env env, const char* message) {
    napi_value console;
    napi_value global;
    napi_value logFunction;

    napi_get_global(env, &global);
    napi_get_named_property(env, global, "console", &console);
    napi_get_named_property(env, console, "log", &logFunction);

    napi_value logMessageValue;
    napi_create_string_utf8(env, message, NAPI_AUTO_LENGTH, &logMessageValue);

    napi_value logArgs[] = { logMessageValue };
    napi_call_function(env, console, logFunction, 1, logArgs, nullptr);
}

// Función de trabajo en segundo plano
void NotifyCallbackAsync(napi_env env, void* data) {
    AsyncContext* asyncContext = static_cast<AsyncContext*>(data);
    std::vector<std::filesystem::path> currentFiles = GetCurrentFiles(asyncContext->folderPath);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::vector<std::filesystem::path> newFiles = GetCurrentFiles(asyncContext->folderPath);

        // Llamar a la función de detección de cambios en el hilo principal
        DetectFileChanges(asyncContext->env, asyncContext->callbackRef, currentFiles, newFiles);
        currentFiles = newFiles;
    }
}

// Función de limpieza después del trabajo en segundo plano
void NotifyCallbackAsyncComplete(napi_env env, napi_status status, void* data) {
    AsyncContext* asyncContext = static_cast<AsyncContext*>(data);

    // Liberar la referencia al callback
    napi_delete_reference(env, asyncContext->callbackRef);

    // Liberar la memoria de la estructura de contexto
    delete asyncContext;
}

// Obtener la lista de archivos en un directorio
std::vector<std::filesystem::path> GetCurrentFiles(const std::filesystem::path& directory) {
    std::vector<std::filesystem::path> files;
    std::filesystem::directory_iterator iterator(directory);

    for (const auto& entry : iterator) {
        files.push_back(entry.path());
    }

    return files;
}

// Detectar cambios entre dos listas de archivos
void DetectFileChanges(napi_env env, napi_vul jsCallbackRef, const std::vector<std::filesystem::path>& oldFiles, const std::vector<std::filesystem::path>& newFiles) {
    for (const auto& newFile : newFiles) {
        if (std::find(oldFiles.begin(), oldFiles.end(), newFile) == oldFiles.end()) {
            // Nuevo archivo
            NotifyFileChange(env, jsCallbackRef, newFile);
        }
    }

    for (const auto& oldFile : oldFiles) {
        if (std::find(newFiles.begin(), newFiles.end(), oldFile) == newFiles.end()) {
            // Archivo eliminado
            NotifyFileChange(env, jsCallbackRef, oldFile);
        }
    }
}
// Notificar un cambio de archivo al callback de JavaScript
void NotifyFileChange(napi_env env, napi_value jsCallbackRef, const std::filesystem::path& filePath) {
    napi_value argv[1];
    napi_create_string_utf8(env, filePath.filename().string().c_str(), NAPI_AUTO_LENGTH, &argv[0]);

    // Llamar a la función de JavaScript con el contexto correcto
    napi_call_function(env, nullptr, jsCallbackRef, 1, argv, nullptr);
}
void CallbackFinalizer(napi_env env, void* finalize_data, void* finalize_hint) {
    WatcherData* watcherData = static_cast<WatcherData*>(finalize_data);
    napi_delete_reference(env, watcherData->callbackRef);
    delete watcherData;
}

napi_value StartWatching(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[1];
    napi_value thisArg;
    napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr);

    WatcherData* watcherData;
    napi_unwrap(env, thisArg, reinterpret_cast<void**>(&watcherData));

    // Obtener la ruta de la carpeta desde el argumento de JavaScript
    size_t strLen;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &strLen);
    std::string folderPath(strLen + 1, '\0');
    napi_get_value_string_utf8(env, argv[0], &folderPath[0], strLen + 1, nullptr);



    // Imprimir la ruta de la carpeta en la consola
    cout << folderPath << endl;

    // Configurar la ruta de la carpeta en el WatcherData
    watcherData->folderWatcher.setFolderPath(folderPath);

    // Crear la estructura de contexto para el trabajo en segundo plano
    AsyncContext* asyncContext = new AsyncContext{
    env,
    nullptr,
    folderPath
    };

    // Crear una referencia al callback para mantenerlo vivo
    napi_create_reference(env, argv[1], 1, &asyncContext->callbackRef);

    // Crear y ejecutar el trabajo en segundo plano
    napi_async_work asyncWork;
    napi_value workName;
    napi_create_string_utf8(env, "NotifyCallbackAsync", NAPI_AUTO_LENGTH, &workName);

    napi_create_async_work(
    env,
    nullptr,
    workName,
    NotifyCallbackAsync,
    NotifyCallbackAsyncComplete,
    asyncContext,
    &asyncWork
    );
    napi_queue_async_work(env, asyncWork);

    return nullptr;
}
napi_value InitModule(napi_env env, napi_value exports) {
    napi_value fn;
    napi_create_function(env, "startWatching", NAPI_AUTO_LENGTH, StartWatching, nullptr, &fn);
    napi_set_named_property(env, exports, "startWatching", fn);

    return exports;
}

napi_value Init(napi_env env, napi_value exports) {
    initializationCount++;

    // Imprimir el mensaje en la consola de Node.js
    std::string logMessage = "Init llamado " + to_string(initializationCount) + " veces.";
    PrintToConsole(env, logMessage.c_str());


    WatcherData* watcherData = new WatcherData();
    watcherData->callbackRef = nullptr;

    napi_wrap(env, exports, watcherData, CallbackFinalizer, nullptr, nullptr);

    return InitModule(env, exports);
}



NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
