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

#include "lse_font_store.h"

#include "freetype/freetype.h"
#include "lse_env.h"
#include "lse_font.h"
#include "lse_memory.h"
#include "lse_object.h"
#include "lse_string.h"
#include "lse_util.h"
#include <assert.h>

#define i_tag font_table
#define i_key lse_string_ptr
#define i_val font_table_entry
#define i_keyraw crawstr
#define i_keyto lse_string_keyto
#define i_keyfrom assert
#define i_hash crawstr_hash
#define i_cmp crawstr_cmp
#define i_eq crawstr_eq
#define i_valdrop font_table_entry_drop
#define i_opt c_no_clone
#include "stc/cmap.h"

typedef struct load_font_context load_font_context;

struct lse_font_store {
  lse_env* env;
  cmap_font_table font_table;
  lse_font* builtin;
  FT_Library ft_library;
};

struct load_font_context {
  // safely modifiable in MAIN thread only (anywhere but the work callback)
  lse_font* font;

  // safely modifiable in POOL thread (work callback)
  lse_font_store* font_store;
  lse_string* uri;
  uint8_t* font_data;
  size_t font_data_size;
  lse_status status;
};

//
// constants
//

static const char* LOAD_FONT_TASK_NAME = "load font task";
extern const uint8_t ROBOTO_REGULAR_LATIN[];
extern const size_t ROBOTO_REGULAR_LATIN_SIZE;
extern const int32_t ROBOTO_REGULAR_LATIN_INDEX;

//
// private functions
//

static void load_font_async(void* user_data);
static void load_font_complete(lse_env* env, void* user_data);
static load_font_context* load_font_context_new(lse_font_store* store, lse_font* font, lse_string* uri);
static void load_font_context_finalize(void* user_data);
static void load_default_font(lse_font_store* store);

// @override
static void constructor(lse_object* object, void* arg) {
  lse_env* env = arg;
  lse_font_store* self = (lse_font_store*)object;
  FT_Error e;

  self->font_table = cmap_font_table_init();

  e = FT_Init_FreeType(&self->ft_library);

  if (e == FT_Err_Ok) {
    self->env = env;
    lse_ref(self->env);
  } else {
    LSE_LOG_ERROR("FT: %s", FT_Error_String(e));
  }

  load_default_font(self);
}

// @override
static void destructor(lse_object* object) {
  lse_font_store* self = (lse_font_store*)object;

  // using a strict assert because the owner should tightly control the store's lifecycle
  assert(cmap_font_table_empty(self->font_table));
  assert(self->env == NULL);

  lse_unref(self->env);
  cmap_font_table_drop(&self->font_table);

  if (self->ft_library) {
    FT_Done_FreeType(self->ft_library);
  }
}

// @public
lse_font* lse_font_store_acquire_font(
    lse_font_store* store,
    const char* family,
    lse_style_font_style style,
    lse_style_font_weight weight) {
  cmap_font_table_value* value = cmap_font_table_get_mut(&store->font_table, family);
  font_resource* resource;

  if (value) {
    resource = &value->second.fonts[style][weight];

    // exact match
    if (resource->font) {
      lse_ref(resource->font);
      return resource->font;
    }

    // find any other font in this family
    for (int32_t s = 0; s < k_lse_style_font_style_count; s++) {
      for (int32_t w = 0; w < k_lse_style_font_weight_count; w++) {
        resource = &value->second.fonts[s][w];

        if (resource->font) {
          lse_ref(resource->font);
          return resource->font;
        }
      }
    }
  }

  // fallback to builtin, if available
  if (store->builtin) {
    lse_ref(store->builtin);
    return store->builtin;
  }

  return NULL;
}

// @public
void lse_font_store_release_font(lse_font_store* store, lse_font* font) {
  // since removing fonts is not supported right now, unref is sufficient.
  lse_unref(font);
}

// @public
lse_status lse_font_store_add_font(
    lse_font_store* store,
    lse_string* uri,
    int32_t index,
    lse_string* family,
    lse_style_font_style style,
    lse_style_font_weight weight,
    bool async) {
  if (lse_font_store_is_destroyed(store)) {
    lse_unref(uri);
    lse_unref(family);

    return LSE_ERR_EOL;
  }

  cmap_font_table_iter i = cmap_font_table_find(&store->font_table, lse_string_as_cstring(family));
  font_resource* resource;
  load_font_context* context;
  cmap_font_table_result insert_result;

  if (i.ref == cmap_font_table_end(&store->font_table).ref) {
    insert_result = cmap_font_table_insert(&store->font_table, family, font_table_entry_init(family));
    resource = &insert_result.ref->second.fonts[style][weight];
  } else {
    resource = &i.ref->second.fonts[style][weight];

    if (resource->font) {
      lse_unref(uri);
      lse_unref(family);

      return LSE_ERR_RES_FONT_KEY;
    }
  }

  lse_font_info info = {
    .uri = uri,
    .index = index,
    .family = family,
    .style = style,
    .weight = weight,
  };

  resource->font = lse_new(lse_font, &info);
  lse_font_set_loading(resource->font);

  context = load_font_context_new(store, resource->font, uri);

  if (async) {
    resource->task = lse_env_add_thread_pool_task(
        store->env, LOAD_FONT_TASK_NAME, &load_font_async, &load_font_complete, context, &load_font_context_finalize);
  } else {
    load_font_async(context);
    load_font_complete(store->env, context);
    load_font_context_finalize(context);
  }

  return LSE_OK;
}

static void load_default_font(lse_font_store* store) {
  lse_font_info info = {
    .uri = lse_string_new_empty(),
    .index = 0,
    .family = lse_string_new("Roboto"),
    .style = LSE_STYLE_FONT_STYLE_NORMAL,
    .weight = LSE_STYLE_FONT_WEIGHT_NORMAL,
  };

  FT_Face face;
  FT_Error e = FT_New_Memory_Face(
      store->ft_library, ROBOTO_REGULAR_LATIN, (FT_Long)ROBOTO_REGULAR_LATIN_SIZE, ROBOTO_REGULAR_LATIN_INDEX, &face);

  if (e == FT_Err_Ok) {
    store->builtin = lse_new(lse_font, &info);

    lse_font_set_loading(store->builtin);
    lse_font_set_ready(store->builtin, face, NULL, 0);
  }
}

// @public
void lse_font_store_destroy(lse_font_store* store) {
  lse_thread_pool_task* task;

  if (lse_font_store_is_destroyed(store)) {
    return;
  }

  // cancel outstanding tasks here because the font table drop function does not have access to env
  c_foreach(it, cmap_font_table, store->font_table) {
    for (int32_t s = 0; s < k_lse_style_font_style_count; s++) {
      for (int32_t w = 0; w < k_lse_style_font_weight_count; w++) {
        task = it.ref->second.fonts[s][w].task;

        if (task) {
          lse_env_cancel_thread_pool_task(store->env, task);
        }
      }
    }
  }

  // clear the set here so fonts can be cleaned up and delete the set's memory in destructor
  cmap_font_table_clear(&store->font_table);

  lse_font_destroy(store->builtin);
  lse_unref(store->builtin);
  store->builtin = NULL;

  lse_unref(store->env);
  store->env = NULL;
}

// @public
bool lse_font_store_is_destroyed(lse_font_store* store) {
  return (!store || !store->env);
}

// @public
lse_font* lse_font_store_get_font(
    lse_font_store* store,
    const char* family,
    lse_style_font_style style,
    lse_style_font_weight weight) {
  const cmap_font_table_value* value = cmap_font_table_get(&store->font_table, lse_ensure_string(family));

  return value ? value->second.fonts[style][weight].font : NULL;
}

// @private
// note: caller provides family with ref
static font_table_entry font_table_entry_init(lse_string* family) {
  return (font_table_entry){
    .family = family,
    .fonts = { 0 },
  };
}

// @private
// note: method should only be called in font_store_destroy
static void font_table_entry_drop(font_table_entry* entry) {
  lse_font* font;

  for (int32_t s = 0; s < k_lse_style_font_style_count; s++) {
    for (int32_t w = 0; w < k_lse_style_font_weight_count; w++) {
      font = entry->fonts[s][w].font;

      if (font) {
        lse_font_destroy(font);
        lse_unref(font);
      }
    }
  }

  lse_unref(entry->family);
}

// @private
static void load_font_async(void* user_data) {
  load_font_context* context = user_data;
  size_t file_size;
  uint8_t* file_contents = NULL;
  FILE* fp = fopen(lse_string_as_cstring(context->uri), "rb");

  if (!fp) {
    context->status = LSE_ERR_FILE_NOT_FOUND;
    goto EXIT;
  }

  fseek(fp, 0, SEEK_END);
  file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  file_contents = lse_malloc(file_size);

  if (fread(file_contents, 1, file_size, fp) != file_size) {
    context->status = LSE_ERR_FILE_READ;
    goto EXIT;
  }

  context->font_data = file_contents;
  context->font_data_size = file_size;
  file_contents = NULL;
  context->status = LSE_OK;

EXIT:
  if (file_contents) {
    free(file_contents);
  }

  if (fp) {
    fclose(fp);
  }
}

// @private
static void load_font_complete(lse_env* env, void* user_data) {
  load_font_context* context = user_data;

  // note: the font could be destroyed by the time complete is called. the lse_font_* calls will safely no-op

  if (context->status == LSE_OK) {
    FT_Face face;
    FT_Error e = FT_New_Memory_Face(
        context->font_store->ft_library,
        context->font_data,
        (FT_Long)context->font_data_size,
        lse_font_get_index(context->font),
        &face);

    if (e == FT_Err_Ok) {
      LSE_LOG_INFO("font ready: %s", lse_string_as_cstring(context->uri));
      lse_font_set_ready(context->font, face, context->font_data, context->font_data_size);
      context->font_data = NULL;
      context->font_data_size = 0;
    } else {
      LSE_LOG_ERROR("FT error: %s", FT_Error_String(e));
      lse_font_set_error(context->font);
    }
  } else {
    LSE_LOG_ERROR("async error: %s", lse_status_string(context->status));
    lse_font_set_error(context->font);
  }
}

// @private
static load_font_context* load_font_context_new(lse_font_store* store, lse_font* font, lse_string* uri) {
  load_font_context* context = lse_calloc(1, sizeof(load_font_context));

  context->font_store = store;
  context->font = font;
  context->uri = uri;

  lse_ref(context->font_store);
  lse_ref(context->font);
  lse_ref(context->uri);

  return context;
}

// @private
static void load_font_context_finalize(void* user_data) {
  load_font_context* context = user_data;

  if (context) {
    lse_unref(context->uri);
    lse_unref(context->font);
    lse_unref(context->font_store);

    free(context->font_data);
    free(context);
  }
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_TYPE_INFO(lse_font_store_type, lse_font_store, lse_none_interface_type, NULL);
