{
  "targets": [
    {
      "target_name": "stb_image",
      "type": "static_library",
      "lse:license": [ "LICENSE" ],
      "include_dirs": [
        "repo"
      ],
      "sources": [
        "src/stb_image.c"
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
          "repo"
        ]
      }
    }
  ]
}
