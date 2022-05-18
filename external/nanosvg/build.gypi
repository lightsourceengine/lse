{
  "targets": [
    {
      "target_name": "nanosvg",
      "type": "static_library",
      "include_dirs": [
        "include",
        "repo-local/src"
      ],
      "sources": [
        "src/nanosvg.c"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "include",
          "repo-local/src"
        ]
      }
    }
  ]
}
