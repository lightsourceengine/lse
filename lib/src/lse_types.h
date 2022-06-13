/*
 * Copyright (c) 2022 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <lse.h>

typedef struct lse_video lse_video;
typedef struct lse_graphics lse_graphics;
typedef struct lse_graphics_container lse_graphics_container;
typedef struct lse_image_store lse_image_store;
typedef struct lse_font_store lse_font_store;
typedef struct lse_node_base lse_node_base;
typedef struct lse_render_object lse_render_object;

typedef lse_image* lse_image_ptr;
typedef lse_window* lse_window_ptr;
typedef lse_gamepad* lse_gamepad_ptr;
typedef lse_string* lse_string_ptr;
typedef lse_node* lse_node_ptr;

typedef struct lse_size lse_size;
typedef struct lse_rect lse_rect;
typedef struct lse_rect_f lse_rect_f;
typedef struct lse_point_f lse_point_f;
typedef struct lse_border_radius lse_border_radius;
typedef struct lse_border_rect lse_border_rect;
typedef struct lse_surface lse_surface;
typedef struct lse_glyph_surface lse_glyph_surface;

typedef struct lse_style_context lse_style_context;

typedef struct lse_text_line_info lse_text_line_info;
typedef struct lse_text_whitespace lse_text_whitespace;
typedef struct lse_text_word_info lse_text_word_info;
typedef struct lse_text_style lse_text_style;

typedef enum {
  lse_display_mode_type = 128,
  lse_font_store_type = 129,
  lse_image_store_type = 130,
  lse_renderer_info_type = 131,

  lse_style_filter_type = 132,
  lse_style_transform_type = 133,
  lse_style_value_type = 134,

  lse_sdl_gamepad_type = 135,
  lse_sdl_graphics_container_type = 136,
  lse_sdl_graphics_type = 137,
  lse_sdl_keyboard_type = 138,
  lse_sdl_video_type = 139,

  lse_mock_gamepad_type = 140,
  lse_mock_graphics_container_type = 141,
  lse_mock_graphics_type = 142,
  lse_mock_keyboard_type = 143,
  lse_mock_video_type = 144,
} lse_type_id_internal;

typedef enum {
  lse_video_interface_type = 128,
  lse_graphics_interface_type = 130,
  lse_graphics_container_interface_type = 131,
} lse_interface_type_id_internal;
