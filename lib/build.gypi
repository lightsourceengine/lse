{
  "targets": [
    {
      "target_name": "lse",
      "type": "static_library",
      "include_dirs": [
        "include",
        "src",
        "../external/SDL2/repo/include"
      ],
      "dependencies": [
        "freetype",
        "stb_image",
        "nanosvg",
        "stc",
        "yoga"
      ],
      "sources": [
        "src/lse_array.c",
        "src/lse_color.c",
        "src/lse_env.c",
        "src/lse_event.c",
        "src/lse_font.c",
        "src/lse_font_store.c",
        "src/lse_gamepad.c",
        "src/lse_graphics.c",
        "src/lse_graphics_container.c",
        "src/lse_image.c",
        "src/lse_image_store.c",
        "src/lse_keyboard.c",
        "src/lse_log.c",
        "src/lse_matrix.c",
        "src/lse_memory.c",
        "src/lse_object.c",
        "src/lse_rect.c",
        "src/lse_render_queue.c",
        "src/lse_sdl.c",
        "src/lse_settings.c",
        "src/lse_string.c",
        "src/lse_style.c",
        "src/lse_style_meta.c",
        "src/lse_text.c",
        "src/lse_util.c",
        "src/lse_video.c",
        "src/lse_window.c",

        "src/lse_node.c",
        "src/lse_box_node.c",
        "src/lse_image_node.c",
        "src/lse_root_node.c",
        "src/lse_text_node.c",

        "src/lse_sdl_gamepad.c",
        "src/lse_sdl_graphics.c",
        "src/lse_sdl_graphics_container.c",
        "src/lse_sdl_keyboard.c",
        "src/lse_sdl_video.c",

        "src/lse_mock_graphics.c",
        "src/lse_mock_graphics_container.c",
        "src/lse_mock_gamepad.c",
        "src/lse_mock_keyboard.c",
        "src/lse_mock_video.c",

        "src/roboto-regular-latin-woff.c"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "include",
          "src",
          "../external/SDL2/repo/include"
        ]
      }
    }
  ]
}
