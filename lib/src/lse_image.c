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

#include "lse_image.h"

#include "lse_event.h"
#include "lse_object.h"
#include <assert.h>

struct lse_image {
  lse_resource_state state;
  lse_string* uri;
  int32_t width;
  int32_t height;
  lse_color* pixels;
  lse_image_pixels_free pixels_free;
  lse_color_format format;
  lse_image_observers observers;
};

static void constructor(lse_object* object, void* arg) {
  lse_image* self = (lse_image*)object;
  lse_string* uri = arg;

  assert(uri != NULL);
  // release fallback
  if (uri == NULL) {
    uri = lse_string_new_empty();
  }

  self->uri = uri; // take the ref from the passed in uri
  self->state = LSE_RESOURCE_STATE_INIT;
  self->format = LSE_COLOR_FORMAT_UNKNOWN;
  self->observers = lse_image_observers_init();
}

static void destructor(lse_object* object) {
  lse_image* self = (lse_image*)object;

  lse_image_observers_drop(&self->observers);

  lse_unref(self->uri);
}

void lse_image_set_ready(
    lse_image* image,
    lse_color* pixels,
    lse_image_pixels_free pixels_free,
    int32_t width,
    int32_t height,
    lse_color_format format) {
  image->state = LSE_RESOURCE_STATE_READY;
  image->pixels = pixels;
  image->pixels_free = pixels_free;
  image->width = width;
  image->height = height;
  image->format = format;

  lse_image_observers_dispatch_event(
      &image->observers,
      &(lse_image_event){
          .state = image->state,
          .image = image,
      });
}

void lse_image_set_error(lse_image* image) {
  image->state = LSE_RESOURCE_STATE_ERROR;

  lse_image_observers_dispatch_event(
      &image->observers,
      &(lse_image_event){
          .state = image->state,
          .image = image,
      });
}

void lse_image_set_loading(lse_image* image) {
  image->state = LSE_RESOURCE_STATE_LOADING;
}

void lse_image_destroy(lse_image* image) {
  image->state = LSE_RESOURCE_STATE_DONE;
  lse_image_release_pixels(image);
}

LSE_API lse_string* LSE_CDECL lse_image_get_uri(lse_image* image) {
  return image->uri;
}

LSE_API int32_t LSE_CDECL lse_image_get_width(lse_image* image) {
  return image->width;
}

LSE_API int32_t LSE_CDECL lse_image_get_height(lse_image* image) {
  return image->height;
}

LSE_API int32_t LSE_CDECL lse_image_get_format(lse_image* image) {
  return image->format;
}

LSE_API lse_resource_state LSE_CDECL lse_image_get_state(lse_image* image) {
  return image->state;
}

LSE_API void LSE_CDECL
lse_image_add_observer(lse_image* image, void* observer, lse_image_event_callback observer_callback) {
  lse_image_observers_add(&image->observers, observer, observer_callback);
}

LSE_API void LSE_CDECL lse_image_remove_observer(lse_image* image, void* observer) {
  lse_image_observers_remove(&image->observers, observer);
}

lse_color* lse_image_get_pixels(lse_image* image) {
  return image->pixels;
}

void lse_image_release_pixels(lse_image* image) {
  if (image->pixels) {
    image->pixels_free(image->pixels);
    image->pixels = NULL;
    image->pixels_free = NULL;
  }
}

bool lse_image_can_render(lse_image* image) {
  return image && image->state == LSE_RESOURCE_STATE_READY && image->width > 0 && image->height > 0;
}

bool lse_image_is_ready(lse_image* image) {
  return (image && lse_image_get_state(image) == LSE_RESOURCE_STATE_READY);
}


// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_TYPE_INFO(lse_image_type, lse_image, lse_none_interface_type, NULL);
