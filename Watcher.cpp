#include "Watcher.h"

using namespace std;

void FolderWatcher::subscribe(const Observer& observer) {
    observers.push_back(observer);
}

void FolderWatcher::startWatching(const std::string& folderPath) {
    std::thread([this, folderPath]() {
        std::filesystem::path pathToWatch(folderPath);
        std::filesystem::directory_iterator iterator(pathToWatch);
        std::vector<std::filesystem::path> currentFiles;

        for (const auto& entry : iterator) {
            currentFiles.push_back(entry.path());
        }

        while (true) {
            std::vector<std::filesystem::path> newFiles;

            for (const auto& entry : std::filesystem::directory_iterator(pathToWatch)) {
                newFiles.push_back(entry.path());
            }

            // Detectar cambios (nuevos, eliminados, modificados)
            for (const auto& newFile : newFiles) {
                if (std::find(currentFiles.begin(), currentFiles.end(), newFile) == currentFiles.end()) {
                    // Nuevo archivo
                    lastChange = {newFile.filename().string(), FileChangeInfo::ADDED};
                    notifyObservers(lastChange);
                }
            }

            for (const auto& currentFile : currentFiles) {
                if (std::find(newFiles.begin(), newFiles.end(), currentFile) == newFiles.end()) {
                    // Archivo eliminado
                    lastChange = {currentFile.filename().string(), FileChangeInfo::DELETED};
                    notifyObservers(lastChange);
                }
            }

            // Actualizar la lista de archivos actuales
            currentFiles = newFiles;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach(); // Separar el hilo del watcher del hilo principal
}

void FolderWatcher::notifyObservers(const FileChangeInfo& changeInfo) {
    for (const auto& observer : observers) {
        observer(changeInfo);
    }
}
void Observer::update() {
    cout << "Se detectó un cambio en la carpeta.\n";
}
