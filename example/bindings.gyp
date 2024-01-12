{
  "targets": [
    {
      "target_name": "folder_watcher",
      "sources": [ "main.cpp", "FolderWatcher.cpp" ],
      "include_dirs": ["<!(node -e \"require('node-addon-api').include\")"],
      "dependencies": ["<!(node -e \"require('node-addon-api').gyp\")"],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ]
    }
  ]
}