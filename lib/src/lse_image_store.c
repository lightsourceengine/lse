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

#include "lse_image_store.h"

#include "lse_env.h"
#include "lse_event.h"
#include "lse_image.h"
#include "lse_memory.h"
#include "lse_object.h"
#include "lse_string.h"
#include "lse_util.h"
#include "nanosvg.h"
#include "nanosvgrast.h"
#include "stb_image.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define i_tag images
#define i_key lse_string_ptr
#define i_keyraw crawstr
#define i_keyto lse_string_keyto
#define i_keyfrom assert
#define i_hash crawstr_hash
#define i_cmp crawstr_cmp
#define i_eq crawstr_eq
#define i_val image_resource
#define i_valdrop image_resource_drop
#define i_opt c_no_cmp | c_no_clone
#include "stc/cmap.h"

//
// types
//

typedef struct load_image_context load_image_context;

struct lse_image_store {
  lse_env* env;
  cmap_images images;
  lse_image_observers observers;
};

struct load_image_context {
  lse_image* image;

  lse_string* uri;
  lse_color* pixels;
  lse_image_pixels_free pixels_free;
  int32_t width;
  int32_t height;
  lse_color_format format;
  lse_status status;
};

//
// constants
//

#define NUM_CHANNELS 4
#define SVG_DATA_URI_PREFIX "data:image/svg+xml,"
const char* LOAD_IMAGE_TASK_NAME = "load image task";
static const char* k_svg_headers[] = { "<?", "<!", "<svg" };

//
// private functions
//

static void load_image_worker(void* user_data);
static void load_image_complete(lse_env* env, void* user_data);
static load_image_context* load_image_context_init(lse_image* image);
static void load_image_context_free(void* user_data);
static lse_status stb_load_image(FILE* fp, load_image_context*);
static lse_status svg_load_image(NSVGimage* svg, load_image_context* context);
static lse_status svg_load_image_from_file(FILE* fp, load_image_context* context);
static lse_status svg_load_image_from_memory(const char* xml, load_image_context* context);
static bool is_stb(FILE* fp);
static bool is_svg(FILE* fp);
static void stb_pixels_free(lse_color* pixels);
static void svg_pixels_free(lse_color* pixels);

// @override
static void constructor(lse_object* object, void* arg) {
  lse_image_store* self = (lse_image_store*)object;

  self->env = arg;
  lse_ref(self->env);

  self->images = cmap_images_init();
  self->observers = lse_image_observers_init();
}

// @override
static void destructor(lse_object* object) {
  lse_image_store* self = (lse_image_store*)object;

  // using a strict assert because the owner should tightly control the store's lifecycle
  assert(cmap_images_empty(self->images));
  assert(self->env == NULL);

  lse_unref(self->env);
  cmap_images_drop(&self->images);

  lse_image_observers_drop(&self->observers);
}

// @public
void lse_image_store_destroy(lse_image_store* store) {
  if (lse_image_store_is_destroyed(store)) {
    return;
  }

  lse_thread_pool_task* task;

  c_foreach(it, cmap_images, store->images) {
    task = it.ref->second.task;

    if (task) {
      lse_env_cancel_thread_pool_task(store->env, task);
    }

    lse_image_observers_dispatch_event(
        &store->observers,
        &(lse_image_event){
            .image = it.ref->second.image,
            .state = LSE_RESOURCE_STATE_DONE,
        });
  }

  cmap_images_clear(&store->images);
  lse_image_observers_clear(&store->observers);

  lse_unref(store->env);
  store->env = NULL;
}

// @public
bool lse_image_store_is_destroyed(lse_image_store* store) {
  return (!store || !store->env);
}

// @public
lse_status lse_image_store_attach(lse_image_store* store) {
  return LSE_OK;
}

// @public
void lse_image_store_detach(lse_image_store* store) {
  //  c_foreach (it, cmap_images, store->images) {
  //    lse_image_set_texture(it.ref->second.image, NULL);
  // TODO: free pixels
  // TODO: move image to a detached state
  //  }
}

// @public
lse_image* lse_image_store_get_image(lse_image_store* store, const char* uri) {
  const cmap_images_value* value = cmap_images_get(&store->images, lse_ensure_string(uri));

  return value ? value->second.image : NULL;
}

// @public
lse_image* lse_image_store_acquire_image(lse_image_store* store, lse_string* uri, bool async) {
  if (lse_image_store_is_destroyed(store)) {
    return NULL;
  }

  lse_image* image;
  load_image_context* context;
  cmap_images_value* value = cmap_images_get_mut(&store->images, lse_string_as_cstring(uri));

  if (value) {
    image = value->second.image;

    value->second.usages++;
    lse_ref(image);

    lse_unref(uri);

    return image;
  }

  image = cmap_images_insert(&store->images, uri, image_resource_init(uri)).ref->second.image;
  lse_image_set_loading(image);

  context = load_image_context_init(image);

  if (async) {
    lse_env_add_thread_pool_task(
        store->env, LOAD_IMAGE_TASK_NAME, &load_image_worker, &load_image_complete, context, &load_image_context_free);
  } else {
    load_image_worker(context);
    load_image_complete(store->env, context);
  }

  return image;
}

// @public
lse_image* lse_image_store_release_image(lse_image_store* store, lse_image* image) {
  if (image) {
    cmap_images_value* value = cmap_images_get_mut(&store->images, lse_string_as_cstring(lse_image_get_uri(image)));

    if (value && value->second.image == image) {
      assert(value->second.image == image);
      assert(value->second.usages > 0);

      if (--value->second.usages <= 0) {
        lse_env_cancel_thread_pool_task(store->env, value->second.task);
        lse_image_observers_dispatch_event(
            &store->observers,
            &(lse_image_event){
                .image = value->second.image,
                .state = LSE_RESOURCE_STATE_DONE,
            });
        cmap_images_erase_entry(&store->images, value);
      }
    }

    lse_unref(image);
  }

  return NULL;
}

void lse_image_store_add_observer(lse_image_store* store, void* observer, lse_image_event_callback callback) {
  lse_image_observers_add(&store->observers, observer, callback);
}

void lse_image_store_remove_observer(lse_image_store* store, void* observer) {
  lse_image_observers_remove(&store->observers, observer);
}

// @private
static load_image_context* load_image_context_init(lse_image* image) {
  load_image_context* context = lse_calloc(1, sizeof(load_image_context));

  context->image = image;
  lse_ref(context->image);

  context->uri = lse_image_get_uri(image);
  lse_ref(context->uri);

  return context;
}

// @private
static void load_image_context_free(void* user_data) {
  load_image_context* context = user_data;

  if (context) {
    lse_unref(context->image);
    lse_unref(context->uri);
    free(context);
  }
}

// @private
static void load_image_worker(void* user_data) {
  load_image_context* context = user_data;
  const char* uri = lse_string_as_cstring(context->uri);
  FILE* fp;
  size_t len = strlen(SVG_DATA_URI_PREFIX);

  // special case plain text svg data uris.
  // TODO: need to support base64 and raster file formats in the future.
  if (strncmp(uri, SVG_DATA_URI_PREFIX, len) == 0) {
    context->status = svg_load_image_from_memory(uri + len, context);
    return;
  }

  fp = fopen(lse_string_as_cstring(context->uri), "rb");

  if (!fp) {
    context->status = LSE_ERR_FILE_NOT_FOUND;
    return;
  }

  if (is_stb(fp)) {
    context->status = stb_load_image(fp, context);
  } else if (is_svg(fp)) {
    context->status = svg_load_image_from_file(fp, context);
  } else {
    context->status = LSE_ERR_UNSUPPORTED_IMAGE_FORMAT;
  }

  fclose(fp);
}

// @private
static void load_image_complete(lse_env* env, void* user_data) {
  load_image_context* context = user_data;

  LSE_LOG_INFO("image load complete");

  // TODO: handle was_cancelled

  if (context->status == LSE_OK) {
    lse_image_set_ready(
        context->image, context->pixels, context->pixels_free, context->width, context->height, context->format);
  } else {
    lse_image_set_error(context->image);
  }
}

// @private
static lse_status stb_load_image(FILE* fp, load_image_context* context) {
  int32_t width;
  int32_t height;
  int32_t channels;
  stbi_uc* bytes;

  bytes = stbi_load_from_file(fp, &width, &height, &channels, NUM_CHANNELS);

  if (!bytes) {
    return LSE_ERR_UNSUPPORTED_IMAGE_FORMAT;
  }

  context->pixels = (lse_color*)bytes;
  context->pixels_free = stb_pixels_free;
  context->width = width;
  context->height = height;

  return LSE_OK;
}

// @private
static lse_status svg_load_image_from_file(FILE* fp, load_image_context* context) {
  return svg_load_image(nsvgParseFromFilePtr(fp, "px", 96), context);
}

// @private
static lse_status svg_load_image_from_memory(const char* xml, load_image_context* context) {
  lse_status status;
  size_t len;
  char* scratch = lse_malloc(strlen(xml) + 1);

  // nsvgParse modifies the input string while parsing.
  len = strlen(xml);
  memcpy(scratch, xml, len);
  scratch[len] = '\0';

  status = svg_load_image(nsvgParse(scratch, "px", 96), context);

  free(scratch);

  return status;
}

// @private
static lse_status svg_load_image(NSVGimage* svg, load_image_context* context) {
  lse_status status;
  NSVGrasterizer* rasterizer = NULL;
  uint8_t* bytes;
  int32_t width;
  int32_t height;

  if (!svg) {
    status = LSE_ERR_RES_SVG_PARSE;
    goto SVG_LOAD_IMAGE_DONE;
  }

  width = (int32_t)ceilf(svg->width);
  height = (int32_t)ceilf(svg->height);

  if (width < 1 || height < 1) {
    status = LSE_ERR_RES_SVG_PARSE;
    goto SVG_LOAD_IMAGE_DONE;
  }

  bytes = lse_malloc(width * height * NUM_CHANNELS);

  rasterizer = nsvgCreateRasterizer();

  if (!rasterizer) {
    status = LSE_ERR_GENERIC;
    goto SVG_LOAD_IMAGE_DONE;
  }

  nsvgRasterizeFull(rasterizer, svg, 0, 0, 1, 1, bytes, width, height, width * NUM_CHANNELS);

  context->pixels = (lse_color*)bytes;
  context->pixels_free = svg_pixels_free;
  context->width = width;
  context->height = height;
  status = LSE_OK;

SVG_LOAD_IMAGE_DONE:
  if (rasterizer) {
    nsvgDeleteRasterizer(rasterizer);
  }

  if (svg) {
    nsvgDelete(svg);
  }

  return status;
}

// @private
static bool is_stb(FILE* fp) {
  return stbi_info_from_file(fp, NULL, NULL, NULL) == 1;
}

// @private
static bool is_svg(FILE* fp) {
  char buffer[32];
  size_t n = fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);

  buffer[n] = '\n';
  fseek(fp, 0, SEEK_SET);

  for (size_t i = 0; i < c_arraylen(k_svg_headers); i++) {
    if (strstr(buffer, k_svg_headers[i]) != NULL) {
      return true;
    }
  }

  return false;
}

// @private
static image_resource image_resource_init(lse_string* uri) {
  return (image_resource){
    .image = lse_new(lse_image, uri),
    .usages = 1,
    .task = NULL,
  };
}

// @private
static void image_resource_drop(image_resource* resource) {
  lse_image_destroy(resource->image);
  lse_unref(resource->image);
}

// @private
static void stb_pixels_free(lse_color* pixels) {
  stbi_image_free(pixels);
}

// @private
static void svg_pixels_free(lse_color* pixels) {
  free(pixels);
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_TYPE_INFO(lse_image_store_type, lse_image_store, lse_none_interface_type, NULL);
