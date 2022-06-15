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

#include "lse_font.h"

#include "lse_event.h"
#include "lse_object.h"
#include "lse_rect.h"
#include "lse_style.h"

#include <freetype/freetype.h>
#include <freetype/ftadvanc.h>
#include <freetype/ftglyph.h>
#include <ctype.h>

struct lse_font {
  lse_font_info info;
  lse_resource_state state;
  lse_font_observers observers;

  FT_Face face;
  uint8_t* face_memory;
  size_t face_memory_size;

  float font_size;
  float ascent;
  float line_height;

  bool has_kerning;
};

static void constructor(lse_object* object, void* arg) {
  lse_font* self = (lse_font*)object;
  lse_font_info* info = arg;

  self->info = *info;
  self->state = LSE_RESOURCE_STATE_INIT;
  self->observers = lse_font_observers_init();
}

static void destructor(lse_object* object) {
  lse_font* self = (lse_font*)object;

  lse_font_observers_drop(&self->observers);
}

void lse_font_destroy(lse_font* font) {
  if (lse_font_is_destroyed(font)) {
    return;
  }

  lse_font_observers_clear(&font->observers);

  if (font->face) {
    FT_Done_Face(font->face);
    font->face = NULL;
  }

  if (font->face_memory) {
    free(font->face_memory);
    font->face_memory = NULL;
    font->face_memory_size = 0;
  }

  font->state = LSE_RESOURCE_STATE_DONE;
}

LSE_API lse_string* LSE_CDECL lse_font_get_uri(lse_font* font) {
  return font->info.uri;
}

LSE_API int32_t LSE_CDECL lse_font_get_index(lse_font* font) {
  return font->info.index;
}

LSE_API lse_string* LSE_CDECL lse_font_get_family(lse_font* font) {
  return font->info.family;
}

LSE_API lse_style_font_style LSE_CDECL lse_font_get_style(lse_font* font) {
  return font->info.style;
}

LSE_API lse_style_font_weight LSE_CDECL lse_font_get_weight(lse_font* font) {
  return font->info.weight;
}

LSE_API lse_resource_state LSE_CDECL lse_font_get_state(lse_font* font) {
  return font->state;
}

LSE_API bool LSE_CDECL lse_font_is_destroyed(lse_font* font) {
  return (!font || font->state == LSE_RESOURCE_STATE_DONE);
}

LSE_API void LSE_CDECL
lse_font_add_observer(lse_font* font, void* observer, lse_font_event_callback observer_callback) {
  if (lse_font_is_destroyed(font)) {
    return;
  }

  lse_font_observers_add(&font->observers, observer, observer_callback);
}

LSE_API void LSE_CDECL lse_font_remove_observer(lse_font* font, void* observer) {
  lse_font_observers_remove(&font->observers, observer);
}

void lse_font_set_ready(lse_font* font, FT_Face face, uint8_t* face_memory, size_t face_memory_size) {
  if (lse_font_is_destroyed(font)) {
    return;
  }

  // TODO: face can be NULL in test environment!
  font->state = LSE_RESOURCE_STATE_READY;
  font->face = face;
  font->has_kerning = face ? FT_HAS_KERNING(face) : false;
  font->face_memory = face_memory;
  font->face_memory_size = face_memory_size;

  lse_font_observers_dispatch_event(&font->observers, &(lse_font_event){ .state = font->state, .font = font });
}

void lse_font_set_error(lse_font* font) {
  if (lse_font_is_destroyed(font)) {
    return;
  }

  font->state = LSE_RESOURCE_STATE_ERROR;

  lse_font_observers_dispatch_event(&font->observers, &(lse_font_event){ .state = font->state, .font = font });
}

void lse_font_set_loading(lse_font* font) {
  if (lse_font_is_destroyed(font)) {
    return;
  }

  font->state = LSE_RESOURCE_STATE_LOADING;
}

bool lse_font_is_ready(lse_font* font) {
  return font && font->state == LSE_RESOURCE_STATE_READY;
}

bool lse_font_use_font_size(lse_font* font, float font_size) {
  if (!lse_font_is_ready(font)) {
    return false;
  }

  if (lse_equals_f(font->font_size, font_size)) {
    return true;
  }

  // font_size is in pixels. scale to pt. convert to 1/64th pt.
  int32_t ft_font_size = (int32_t)(font_size * LSE_PT_SCALE_FACTOR * 64.f);

  if (FT_Set_Char_Size(font->face, ft_font_size, 0, 96, 0) == FT_Err_Ok) {
    font->font_size = font_size;
    font->ascent = LSE_FROM_FLOAT_266(font->face->size->metrics.ascender);
    font->line_height = LSE_FROM_FLOAT_266(font->face->size->metrics.height);

    return true;
  } else {
    font->font_size = 0;
    font->ascent = 0;
    font->line_height = 0;

    return false;
  }
}

float lse_font_get_font_size(lse_font* font) {
  return font->font_size;
}

float lse_font_get_ascent(lse_font* font) {
  return font->ascent;
}

float lse_font_get_line_height(lse_font* font) {
  return font->line_height;
}

float lse_font_get_kerning(lse_font* font, uint32_t cp1, uint32_t cp2) {
  FT_Vector kerning;
  uint32_t char_index_1 = FT_Get_Char_Index(font->face, cp1);
  uint32_t char_index_2 = FT_Get_Char_Index(font->face, cp2);

  if (FT_Get_Kerning(font->face, char_index_1, char_index_2, FT_KERNING_DEFAULT, &kerning) == FT_Err_Ok) {
    return LSE_FROM_FLOAT_266(kerning.x);
  }

  return 0;
}

float lse_font_get_advance(lse_font* font, uint32_t cp) {
  FT_Fixed advance1616;
  uint32_t char_index = FT_Get_Char_Index(font->face, cp);

  if (FT_Get_Advance(font->face, char_index, FT_LOAD_NO_BITMAP, &advance1616) != FT_Err_Ok) {
    return 0;
  }

  return LSE_FROM_FLOAT_266((int32_t)advance1616 >> 10);
}

float lse_font_get_advance_and_kerning(lse_font* font, uint32_t codepoint, uint32_t previous) {
  return lse_font_get_advance(font, codepoint) +
         (font->has_kerning && previous ? lse_font_get_kerning(font, previous, codepoint) : 0.f);
}

bool lse_font_has_kerning(lse_font* font) {
  return font->has_kerning;
}

bool lse_font_get_glyph_surface(lse_font* font, uint32_t codepoint, lse_glyph_surface* surface) {
  FT_Glyph glyph;
  FT_BitmapGlyph bitmap_glyph;

  if (isspace((int32_t)codepoint)) {
    return false;
  }

  if (FT_Load_Glyph(font->face, FT_Get_Char_Index(font->face, codepoint), FT_LOAD_RENDER) != FT_Err_Ok) {
    return false;
  }

  if (FT_Get_Glyph(font->face->glyph, &glyph) != FT_Err_Ok) {
    return false;
  }

  if (glyph->format != FT_GLYPH_FORMAT_BITMAP) {
    FT_Done_Glyph(glyph);
    return false;
  }

  bitmap_glyph = (FT_BitmapGlyph)glyph;

  surface->x_offset = bitmap_glyph->left;
  surface->y_offset = bitmap_glyph->top;
  surface->surface.width = (int32_t)bitmap_glyph->bitmap.width;
  surface->surface.height = (int32_t)bitmap_glyph->bitmap.rows;
  surface->surface.pitch = (int32_t)bitmap_glyph->bitmap.pitch;
  surface->surface.buffer = bitmap_glyph->bitmap.buffer;

  return true;
}

bool lse_font_key_equals(lse_font* font, lse_string* family, lse_style_font_style style, lse_style_font_weight weight) {
  if (!font || !family) {
    return !font && !family;
  }

  lse_font_info* key = &font->info;

  return (lse_string_case_cmp(key->family, family) == 0) && (key->style == style) && (key->weight == weight);
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_TYPE_INFO(lse_font_type, lse_font, lse_none_interface_type, NULL);
