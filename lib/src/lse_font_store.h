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
#include "lse_types.h"

#define LSE_FONT_STORE_ASYNC true
#define LSE_FONT_STORE_SYNC false

/**
 * Destroy a font store object.
 *
 * Performs internal cleanup, removing all fonts. The lse_object is NOT cleaned up here, as the owner will
 * dereference. No new fonts can be added after destroy.
 */
void lse_font_store_destroy(lse_font_store* store);

/**
 * Has the font store been destroyed?
 *
 * In the destroyed state, adding fonts will fail and retrieving fonts will return NULL.
 */
bool lse_font_store_is_destroyed(lse_font_store* store);

/**
 * Acquire a font for use in the scene graph.
 *
 * The function will prefer an exact match of family/style/weight. If a match is not available, another font in
 * the family will be chosen, if available.
 *
 * The returned font object will be ref'd and callers should use lse_font_store_release_font() to dereference.
 *
 * @return font object if a match is found; otherwise, NULL
 */
lse_font* lse_font_store_acquire_font(
    lse_font_store* store,
    const char* family,
    lse_style_font_style style,
    lse_style_font_weight weight);

/**
 * Release fonts that were acquired through lse_font_store_acquire_font().
 */
void lse_font_store_release_font(lse_font_store* store, lse_font* font);

/**
 * Add a font from file.
 *
 * The font will be accessible through the family/style/weight key.
 *
 * The function is intended to work asynchronously. Add listeners to the font to know the result of success or error.
 * If running synchronously, listeners will be dispatch BEFORE this function returns.
 *
 * @return error code. use lse_font_get() to access the font object.
 */
lse_status lse_font_store_add_font(
    lse_font_store* store,
    lse_string* uri,
    int32_t index,
    lse_string* family,
    lse_style_font_style style,
    lse_style_font_weight weight,
    bool async);

/**
 * Get a font by exact family/style/weight key.
 *
 * The returned font does not have a reference. If the caller wants to hold the font, add a reference with lse_ref().
 *
 * @return NULL if font not found; otherwise, lse_font with matching key
 */
lse_font* lse_font_store_get_font(
    lse_font_store* store,
    const char* family,
    lse_style_font_style style,
    lse_style_font_weight weight);

//
// cmap_font_table
//

typedef struct font_resource font_resource;
typedef struct font_table_entry font_table_entry;

struct font_resource {
  lse_font* font;
  lse_thread_pool_task* task;
};

struct font_table_entry {
  lse_string* family;
  font_resource fonts[k_lse_style_font_style_count][k_lse_style_font_weight_count];
};

static font_table_entry font_table_entry_init(lse_string* family);
static void font_table_entry_drop(font_table_entry* entry);
