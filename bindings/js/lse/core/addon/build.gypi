{
  "targets": [
    {
      "target_name": "lse-core",
      "dependencies": [
        "stb_image",
        "stc",
        "yoga",
        "freetype",
        "lse"
      ],
      "include_dirs": [
        "src"
      ],
      "sources": [
        "src/init.c",
        "src/napix.c",
        "src/lse_core.c",
        "src/lse_core_class.c",
        "src/lse_core_env.c",
        "src/lse_core_exports.c",
        "src/lse_core_style.c",
        "src/lse_core_thread_pool.c",
        "src/lse_core_window.c"
      ]
    }
  ]
}
