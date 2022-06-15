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

#include "lse_text.h"

#include "lse_font.h"
#include "lse_util.h"
#include <stc/utf8.h>
#include <ctype.h>

void lse_text_get_whitespace(const char* utf8, lse_text_whitespace* out) {
  const char* cursor = utf8;
  uint32_t codepoint;

  *out = (lse_text_whitespace){ 0 };

  while (*cursor) {
    codepoint = utf8_peek(cursor);

    if (codepoint == LSE_UNICODE_TAB || codepoint == LSE_UNICODE_SPACE) {
      out->has_space = true;
    } else if (codepoint == LSE_UNICODE_NEW_LINE) {
      // TODO: stop if pre is enabled
    } else if (codepoint > LSE_UNICODE_SPACE) {
      break;
    }

    cursor = lse_text_next(cursor);
  }

  out->next = cursor;
}

void lse_text_measure(lse_string* str, const lse_text_style* text_style, lse_size* out) {
  const char* cursor = lse_string_as_cstring(str);
  float longest = 0;
  int32_t line_count = 0;
  lse_text_line_info line_info;
  int32_t max_lines = INT32_MAX;
  float line_height;

  *out = (lse_size){ 0 };

  if (!lse_font_use_font_size(text_style->font, text_style->font_size_px)) {
    return;
  }

  line_height = lse_font_get_line_height(text_style->font);

  if (text_style->context_box.height > 0 && line_height > 0) {
    max_lines = (int32_t)ceilf(text_style->context_box.height / line_height);
  }

  if (text_style->max_lines > 0) {
    max_lines = lse_min(max_lines, text_style->max_lines);
  }

  while (*cursor) {
    lse_text_get_line(cursor, text_style, &line_info);

    if (line_info.width > longest) {
      longest = line_info.width;
    }

    cursor = line_info.next;

    if (*cursor || line_info.width > 0) {
      line_count++;
    }

    if (max_lines == line_count) {
      break;
    }
  }

  out->width = longest;
  out->height = (float)line_count * line_height;
}

void lse_text_get_line(const char* str, const lse_text_style* text_style, lse_text_line_info* out) {
  const char* cursor = str;
  uint32_t codepoint;
  uint32_t previous = 0;
  float width = 0;
  float whitespace_width;
  bool kerning_enabled = text_style->kerning_enabled;
  bool preserve_format = (text_style->white_space == LSE_STYLE_WHITE_SPACE_PRE);
  lse_font* font = text_style->font;
  lse_text_whitespace ws;
  lse_text_word_info word_metrics;

  if (preserve_format) {
    while (*cursor && *cursor != '\n') {
      codepoint = lse_text_peek(cursor, text_style);

      width += lse_font_get_advance_and_kerning(font, codepoint, previous);

      if (kerning_enabled) {
        previous = codepoint;
      }

      cursor = lse_text_next(cursor);
    }
  } else {
    // TODO: ellipsis policy
    // TODO: overflow-wrap

    // skip leading whitespace
    lse_text_get_whitespace(cursor, &ws);
    cursor = str = ws.next;
    whitespace_width = lse_font_get_advance(font, LSE_UNICODE_SPACE);

    while (*cursor) {
      lse_text_get_word(cursor, text_style, &word_metrics);

      if (width > 0) {
        if (width + whitespace_width + word_metrics.width > text_style->context_box.width) {
          break;
        }
      } else {
        if (word_metrics.width > text_style->context_box.width) {
          width = word_metrics.width;
          cursor = word_metrics.next;
          break;
        }
      }

      width += (whitespace_width + word_metrics.width);
      cursor = word_metrics.next;
    }
  }

  *out = (lse_text_line_info){
    .start = str,
    .next = cursor,
    .width = width,
  };
}

void lse_text_get_word(const char* str, const lse_text_style* style, lse_text_word_info* out) {
  const char* cursor = str;
  uint32_t codepoint;
  uint32_t previous = 0;
  float width = 0;
  bool kerning_enabled = style->kerning_enabled;
  lse_font* font = style->font;
  lse_text_whitespace ws;

  // skip leading whitespace of word
  lse_text_get_whitespace(cursor, &ws);
  cursor = str = ws.next;

  // read word
  while (*cursor) {
    codepoint = lse_text_peek(cursor, style);

    if (codepoint <= LSE_UNICODE_SPACE) {
      break;
    }

    width += lse_font_get_advance_and_kerning(font, codepoint, previous);

    if (kerning_enabled) {
      previous = codepoint;
    }

    cursor = lse_text_next(cursor);
  }

  *out = (lse_text_word_info){
    .width = width,
    .start = str,
    .next = cursor,
    .has_leading_space = ws.has_space,
  };
}

uint32_t lse_text_peek(const char* utf8, const lse_text_style* style) {
  uint32_t codepoint = utf8_peek(utf8);

  switch (style->transform) {
    case LSE_STYLE_TEXT_TRANSFORM_LOWERCASE:
      return (uint32_t)tolower((int32_t)codepoint);
    case LSE_STYLE_TEXT_TRANSFORM_UPPERCASE:
      return (uint32_t)toupper((int32_t)codepoint);
    default:
      return codepoint;
  }
}

const char* lse_text_next(const char* utf8) {
  utf8_decode_t d = { UTF8_OK, 0 };
  return (const char*)utf8_next(&d, (const uint8_t*)utf8);
}
