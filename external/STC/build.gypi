{
  "targets": [
    {
      "target_name": "stc",
      "type": "static_library",
      "include_dirs": [
        "repo/include"
      ],
      "sources": [
        "repo/src/threads.c",
        "src/utf8.c"
      ],
      "cflags_cc+": [
        "-w"
      ],
      "xcode_settings": {
        "WARNING_CFLAGS": [
            "-w"
        ]
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
            "WarningLevel": "0"
        }
      },
      "direct_dependent_settings": {
        "include_dirs": [
          "repo/include"
        ]
      }
    }
  ]
}
