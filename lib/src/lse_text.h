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

#include "lse_rect.h"
#include "lse_types.h"

//
// macros
//

#define LSE_UNICODE_SPACE 0x20
#define LSE_UNICODE_TAB 0x09
#define LSE_UNICODE_NEW_LINE 0x0A

//
// types
//

struct lse_text_whitespace {
  const char* next;
  bool has_space;
};

struct lse_text_line_info {
  float width;
  const char* start;
  const char* next;
  // TODO: ellipsis flag
};

struct lse_text_style {
  lse_font* font;
  float font_size_px;
  int32_t max_lines;
  lse_size context_box;
  lse_style_text_transform transform;
  lse_style_text_align align;
  lse_style_text_overflow overflow;
  lse_style_overflow_wrap overflow_wrap;
  lse_style_white_space white_space;
  bool kerning_enabled;
};

struct lse_text_word_info {
  float width;
  const char* start;
  const char* next;
  bool has_leading_space;
};

//
// function prototypes
//

/**
 * Measures the bounding box of a text string, given a font and style settings.
 *
 * All style properties that effect text rendering are applied to the measurement, including properties and
 * circumstances that may cause line breaks (multiple lines of text).
 *
 * If the text explicitly has a linebreak character, it will be honored as a linebreak.
 *
 * @param str string to measure
 * @param font current font
 * @param style current style to layout against
 * @param context for style property resolution
 * @param line_break_limit layout width of content box used for line breaks
 * @param out must be non-null. bounding box dimensions are written here
 */
void lse_text_measure(lse_string* str, const lse_text_style* style, lse_size* out);

/**
 * Gets the beginning position, end position and length of the next line.
 *
 * line_break_limit is the layout width of the content box. If the line width exceeds this limit, the style
 * text-overflow policy is consulted. If the line can be broken, the line will be broken accordingly. If the line
 * cannot be broken, measurement will continue until the next line break or end of string. The measured line width can
 * exceed line_break_limit. The renderer will decide how to present text outside the content box.
 *
 * @param str null-terminated UTF-8 string
 * @param font current font
 * @param style current style to layout against
 * @param context for style property resolution
 * @param line_break_limit layout width of content box used for line breaks
 * @param out must be non-null. next line data is written here.
 */
void lse_text_get_line(const char* str, const lse_text_style* style, lse_text_line_info* out);

/**
 * Gets the position of the next non-whitespace character.
 *
 * If any non-zero length whitespace character are encountered between the current position and the next non-whitespace
 * character position, a space flag is set. This effectively 'compresses' consecutive whitespace characters into a
 * single space, similar to how HTML handles text content.
 *
 * If no whitespace characters are present, the cursor (out->next) remains at utf8.
 *
 * If the stream only has whitespace characters, the cursor (out->next) will point to the null terminator.
 *
 * @param str null-terminated UTF-8 string
 * @param out must be non-null. next cursor position and spacing information is written here.
 */
void lse_text_get_whitespace(const char* str, lse_text_whitespace* out);

void lse_text_get_word(const char* str, const lse_text_style* style, lse_text_word_info* out);

uint32_t lse_text_peek(const char* utf8, const lse_text_style* style);
const char* lse_text_next(const char* utf8);