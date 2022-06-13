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

#include "lse_types.h"
#include <stdint.h>

struct lse_rect {
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

struct lse_rect_f {
  float x;
  float y;
  float width;
  float height;
};

struct lse_border_rect {
  float top;
  float right;
  float bottom;
  float left;
};

struct lse_border_radius {
  float top_left;
  float top_right;
  float bottom_left;
  float bottom_right;
};

struct lse_size {
  float width;
  float height;
};

struct lse_point_f {
  float x;
  float y;
};

struct lse_surface {
  int32_t width;
  int32_t height;
  void* buffer;
  int32_t pitch;
};

struct lse_glyph_surface {
  int32_t x_offset;
  int32_t y_offset;
  lse_surface surface;
};

void lse_clip_image_rect(
    const lse_rect_f* box,
    const lse_rect_f* image_rect,
    float natural_image_width,
    float natural_image_height,
    lse_rect_f* out_rect,
    lse_rect* out_src);

bool lse_rect_is_empty(const lse_rect* rect);

lse_rect lse_rect_intersect(const lse_rect* a, const lse_rect* b);
