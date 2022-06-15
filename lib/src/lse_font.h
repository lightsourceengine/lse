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
#include <lse.h>

#define LSE_FROM_FLOAT_266(VALUE) (((float)(VALUE)) / 63.f)

struct FT_FaceRec_;
typedef struct lse_font_info lse_font_info;

struct lse_font_info {
  lse_string* family;
  lse_style_font_style style;
  lse_style_font_weight weight;
  lse_string* uri;
  int32_t index;
};

void lse_font_destroy(lse_font* font);

void lse_font_set_ready(lse_font* font, struct FT_FaceRec_* face, uint8_t* face_memory, size_t face_memory_size);
void lse_font_set_error(lse_font* font);
void lse_font_set_loading(lse_font* font);

bool lse_font_is_ready(lse_font* font);

bool lse_font_use_font_size(lse_font* font, float font_size);
float lse_font_get_font_size(lse_font* font);
float lse_font_get_ascent(lse_font* font);
float lse_font_get_line_height(lse_font* font);
float lse_font_get_kerning(lse_font* font, uint32_t cp1, uint32_t cp2);
float lse_font_get_advance(lse_font* font, uint32_t cp);
float lse_font_get_advance_and_kerning(lse_font* font, uint32_t codepoint, uint32_t previous);
bool lse_font_has_kerning(lse_font* font);
bool lse_font_get_glyph_surface(lse_font* font, uint32_t codepoint, lse_glyph_surface* surface);

bool lse_font_key_equals(lse_font* font, lse_string* family, lse_style_font_style style, lse_style_font_weight weight);
