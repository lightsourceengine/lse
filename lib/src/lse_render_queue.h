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

#include "lse_color.h"
#include "lse_types.h"

#include "stc/forward.h"

typedef struct lse_render_command lse_render_command;
typedef struct lse_render_queue lse_render_queue;

typedef enum lse_render_command_type {
  LSE_RCT_BEGIN = 0,
  LSE_RCT_ROUNDED_RECT = 1,
  LSE_RCT_FILL_RECT = 2,
  LSE_RCT_DRAW_IMAGE = 3,
  LSE_RCT_STROKE_RECT = 4,
  LSE_RCT_DRAW_TEXT = 5,
} lse_render_command_type;

struct lse_render_command {
  lse_render_command_type type;

  lse_rect_f* rect;
  lse_rect* src_rect;
  lse_color fill_color;
  lse_color stroke_color;
  lse_image* image;
  lse_border_radius* corners;
  lse_border_rect* edges;
  float stroke_width;
  lse_string* text;
  lse_text_style* text_style;
};

forward_cvec(cvec_render_commands, lse_render_command);

struct lse_render_queue {
  cvec_render_commands commands;
};

lse_render_queue lse_render_queue_init();
void lse_render_queue_drop(lse_render_queue* q);

void lse_render_queue_push(lse_render_queue* q, lse_render_command* command);
void lse_render_queue_clear(lse_render_queue* q);
size_t lse_render_queue_size(lse_render_queue* q);
lse_render_command* lse_render_queue_get(lse_render_queue* q, size_t i);
