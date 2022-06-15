{
  "targets": [
    {
      "target_name": "yoga",
      "type": "static_library",
      "include_dirs": [
        "repo-local/yoga"
      ],
      "lse:source_dirs": [
      ],
      "lse:license": "repo-local/LICENSE",
      "sources": [
        "repo-local/yoga/event/event.cpp",
        "repo-local/yoga/internal/experiments.cpp",
        "repo-local/yoga/log.cpp",
        "repo-local/yoga/Utils.cpp",
        "repo-local/yoga/YGConfig.cpp",
        "repo-local/yoga/YGEnums.cpp",
        "repo-local/yoga/YGLayout.cpp",
        "repo-local/yoga/YGNode.cpp",
        "repo-local/yoga/YGNodePrint.cpp",
        "repo-local/yoga/YGStyle.cpp",
        "repo-local/yoga/YGValue.cpp",
        "repo-local/yoga/Yoga.cpp"
      ],
      "defines": [
        "YG_ENABLE_NODE_LAYOUT_EVENT=1",
        "YOGA_EXPORT="
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "repo-local/yoga"
        ]
      }
    }
  ]
}
