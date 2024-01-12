{
  "targets": [
    {
      "target_name": "watcher",
       "sources": [
        "Watcher.cpp",
        "NodeWrapper.cpp",
        "Watcher.h"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "."
      ],
      "cflags_cc": ["-std=c++11"],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "conditions": [
        ["OS=='win'", {
          "defines": ["_HAS_EXCEPTIONS=0"],
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 0
            }
          }
        }]
      ]
    }
  ]
}
