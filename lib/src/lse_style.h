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
#include "lse_matrix.h"
#include "lse_rect.h"
#include "lse_types.h"
#include "lse_util.h"

struct lse_style_context {
  float view_width;
  float view_height;
  float view_min;
  float view_max;
  float root_font_size_px;
};

#define LSE_DEFAULT_FONT_SIZE_PX 16.0f
#define LSE_PT_SCALE_FACTOR 1.333333f

void lse_style_bind_node(lse_style* style, lse_node* node);

lse_style* lse_style_get_empty();

bool lse_style_parse_color(lse_string* str, uint32_t* color);

bool lse_style_parse_numeric(lse_string* str, lse_style_value* numeric);

float lse_style_resolve_numeric(const lse_style_value* value, const lse_style_context* context);

float lse_style_resolve_opacity(lse_style* style);

lse_border_radius lse_style_compute_border_radius(lse_style* style, const lse_style_context* context);

bool lse_style_has_border_layout(lse_node* node);

lse_border_rect lse_style_get_border_edges(lse_node* node);

bool lse_style_has_transform(lse_style* style);

float lse_style_compute_border_only(lse_style* style, const lse_style_context* context);

float lse_style_compute_font_size(lse_style* style, const lse_style_context* context);

int32_t lse_style_compute_max_lines(lse_style* style, const lse_style_context* context);

lse_matrix lse_style_compute_transform(lse_style* style, const lse_style_context* context, const lse_rect_f* box);

lse_point_f
lse_style_compute_transform_origin(lse_style* style, const lse_style_context* context, const lse_rect_f* box);

float lse_style_compute_font_size_from_root(lse_style* style, const lse_style_context* context);

void lse_style_compute_object_fit(
    lse_style* style,
    const lse_style_context* context,
    lse_rect_f* box,
    lse_image* image,
    lse_rect_f* out);

void lse_style_compute_background_fit(
    lse_style* style,
    const lse_style_context* context,
    lse_rect_f* box,
    lse_image* image,
    lse_rect_f* out);

void lse_style_update_dynamic_units(lse_node* node, bool view_units, bool rem_units);

lse_text_style lse_style_as_text_style(
    lse_style* style,
    const lse_style_context* context,
    lse_font* font,
    const lse_size* content_box);
