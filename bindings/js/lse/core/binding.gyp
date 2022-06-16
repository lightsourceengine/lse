{
  "includes": [
    "external/freetype/build.gypi",
    "external/nanosvg/build.gypi",
    "external/stb/build.gypi",
    "external/STC/build.gypi",
    "external/yoga/build.gypi",
    "external/lse/build.gypi",
    "addon/build.gypi"
  ],
  "target_defaults": {
    "defines": [
      "NAPI_VERSION=3"
    ],
    "cflags_cc!": [
      "-std=gnu++1y"
    ],
    "cflags_cc+": [
      "-std=c++11",
      "-fvisibility=hidden",
      "-fvisibility-inlines-hidden",
      "-ffunction-sections",
      "-fdata-sections",
      "-fno-semantic-interposition",
      "-fno-threadsafe-statics",
      "-fmerge-all-constants",
      "-fno-math-errno",
      "-Wno-missing-field-initializers"
    ],
    "ldflags+": [
      "-fvisibility=hidden",
      "-fvisibility-inlines-hidden",
      "-ffunction-sections",
      "-fdata-sections",
      "-Wl,--gc-sections",
      "-flto"
    ],
    "xcode_settings": {
      "GCC_ENABLE_CPP_EXCEPTIONS": "NO",
      "CLANG_CXX_LIBRARY": "libc++",
      "MACOSX_DEPLOYMENT_TARGET": "10.10",
      "CLANG_CXX_LANGUAGE_STANDARD": "c++11",
      "DEAD_CODE_STRIPPING": "YES",
      "GCC_THREADSAFE_STATICS": "NO",
      "OTHER_CFLAGS": [
        "-fvisibility=hidden",
        "-fvisibility-inlines-hidden",
        "-ffunction-sections",
        "-fdata-sections",
        "-Wno-missing-field-initializers"
      ]
    },
    "msvs_settings": {
      "VCCLCompilerTool": {
        "ExceptionHandling": "0"
      }
    },
    "conditions": [
      [
        "OS==\"win\"",
        {
          "defines": [
            "_WIN32",
            "NOMINMAX",
            "_USE_MATH_DEFINES"
          ]
        }
      ]
    ],
    "configurations": {
      "Release": {
        "defines": [
          "NDEBUG"
        ]
      }
    }
  }
}
