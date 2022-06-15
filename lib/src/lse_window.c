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

#include "lse_window.h"

#include "lse_env.h"
#include "lse_graphics.h"
#include "lse_graphics_container.h"
#include "lse_image_store.h"
#include "lse_node.h"
#include "lse_object.h"
#include "lse_string.h"
#include "lse_style.h"
#include "lse_video.h"
#include <string.h>

struct lse_window {
  lse_env* env;

  lse_node* root;

  lse_string* title;
  int32_t width;
  int32_t height;
  int32_t refresh_rate;
  uint32_t flags;

  lse_graphics_container* graphics_container;
  lse_image_store* image_store;

  lse_style_context style_context;
};

static void on_image_removed(const lse_image_event* e, void* observer) {
  lse_window* window = observer;
  lse_graphics_container* container = window->graphics_container;
  lse_graphics* graphics = container ? lse_graphics_container_get_base(container)->graphics : NULL;

  if (!lse_graphics_is_destroyed(graphics)) {
    lse_graphics_remove_image(graphics, e->image);
  }
}

static void constructor(lse_object* object, void* arg) {
  lse_window* self = (lse_window*)object;

  self->env = arg;
  lse_ref(self->env);

  self->root = (lse_node*)lse_object_new(lse_root_node_type, self);
  self->style_context.root_font_size_px = LSE_DEFAULT_FONT_SIZE_PX;

  self->title = lse_string_new_empty();

  self->image_store = lse_new(lse_image_store, arg);
  lse_image_store_add_observer(self->image_store, self, &on_image_removed);
}

static void destructor(lse_object* object) {
  lse_window* self = (lse_window*)object;

  assert(lse_window_is_destroyed(self));

  lse_unref(self->image_store);
  lse_unref(self->graphics_container);
  lse_unref(self->title);
  lse_unref(self->root);
  lse_unref(self->env);
}

lse_env* lse_window_get_env(lse_window* window) {
  return window->env;
}

lse_image_store* lse_window_get_image_store(lse_window* window) {
  return window->image_store;
}

void lse_window_destroy(lse_window* window) {
  if (lse_window_is_destroyed(window)) {
    return;
  }

  // TODO: this destroys the native scene graph, but not the js one. needs a fix.
  lse_node_destroy(window->root);

  lse_image_store_remove_observer(window->image_store, window);
  lse_image_store_destroy(window->image_store);

  if (window->graphics_container) {
    lse_graphics_container_destroy(window->graphics_container);
    lse_unref(window->graphics_container);
    window->graphics_container = NULL;
  }

  lse_unref(window->title);
  window->title = NULL;

  lse_unref(window->env);
  window->env = NULL;

  window->width = window->height = window->refresh_rate = 0;
  window->flags = 0;
}

LSE_API bool LSE_CDECL lse_window_is_destroyed(lse_window* window) {
  return !window || !window->env;
}

LSE_API lse_status LSE_CDECL lse_window_configure(lse_window* window, const lse_window_settings* settings) {
  lse_status status;
  lse_graphics_container* container;

  if (lse_window_is_destroyed(window)) {
    return LSE_ERR_EOL;
  }

  if (window->graphics_container) {
    return LSE_ERR_ALREADY_CONFIGURED;
  }

  // TODO: should settings size options be resolved here, rather than in container?

  container = lse_video_create_graphics_container(window->env->video);

  if (!container) {
    return LSE_ERR_OUT_OF_MEMORY;
  }

  status = lse_graphics_container_configure(container, settings);

  if (status != LSE_OK) {
    lse_graphics_container_destroy(container);
    lse_unref(container);
    return status;
  }

  window->width = lse_graphics_container_get_width(container);
  window->height = lse_graphics_container_get_height(container);
  window->graphics_container = container;

  lse_image_store_attach(window->image_store);

  window->style_context.view_width = (float)window->width;
  window->style_context.view_height = (float)window->height;
  window->style_context.view_max = lse_max(window->style_context.view_width, window->style_context.view_height);
  window->style_context.view_min = lse_min(window->style_context.view_width, window->style_context.view_height);

  lse_style_update_dynamic_units(window->root, true, false);

  return status;
}

LSE_API lse_node* LSE_CDECL lse_window_get_root(lse_window* window) {
  return window->root;
}

LSE_API void LSE_CDECL lse_window_set_title(lse_window* window, lse_string* title) {
  lse_graphics_container* graphics_container = window->graphics_container;

  lse_unref(window->title);
  // caller must pass a string with a ref
  window->title = title ? title : lse_string_new_empty();

  if (!graphics_container) {
    return;
  }

  lse_graphics_container_set_title(graphics_container, title);
}

LSE_API int32_t LSE_CDECL lse_window_get_width(lse_window* window) {
  return window->width;
}

LSE_API int32_t LSE_CDECL lse_window_get_height(lse_window* window) {
  return window->height;
}

LSE_API int32_t LSE_CDECL lse_window_get_refresh_rate(lse_window* window) {
  return window->refresh_rate;
}

LSE_API uint32_t LSE_CDECL lse_window_get_flags(lse_window* window) {
  return window->flags;
}

LSE_API lse_node* LSE_CDECL lse_window_create_node_from_tag(lse_window* window, const char* tag) {
  uint8_t type = lse_none_type;

  if (tag) {
    if (c_strncasecmp(LSE_NODE_TAG_BOX, tag, strlen(LSE_NODE_TAG_BOX)) == 0) {
      type = lse_box_node_type;
    } else if (c_strncasecmp(LSE_NODE_TAG_IMAGE, tag, SIZE_MAX) == 0) {
      type = lse_image_node_type;
    } else if (c_strncasecmp(LSE_NODE_TAG_TEXT, tag, SIZE_MAX) == 0) {
      type = lse_text_node_type;
    }
  }

  return type != lse_none_type ? (lse_node*)lse_object_new(type, window) : NULL;
}

void lse_window_present(lse_window* window) {
  lse_graphics* graphics;
  lse_graphics_container* graphics_container = window->graphics_container;

  if (!graphics_container) {
    return;
  }

  graphics = lse_graphics_container_begin_frame(graphics_container);

  if (!graphics) {
    return;
  }

  lse_root_node_update(window->root, graphics, (float)window->width, (float)window->height);

  lse_graphics_container_end_frame(graphics_container);
}

const lse_style_context* lse_window_get_style_context(lse_window* window) {
  return &window->style_context;
}

void lse_window_dispatch_root_font_size_change(lse_window* window, float root_font_size) {
  if (!lse_equals_f(window->style_context.root_font_size_px, root_font_size)) {
    lse_style_update_dynamic_units(window->root, false, true);
  }
}

void lse_window_destroy_render_object(lse_window* window, lse_render_object* render_object) {
  if (!render_object) {
    return;
  }

  lse_graphics_container* container = window->graphics_container;
  lse_graphics* graphics = container ? lse_graphics_container_get_base(container)->graphics : NULL;

  if (graphics) {
    lse_graphics_destroy_render_object(graphics, render_object);
  }
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_TYPE_INFO(lse_window_type, lse_window, lse_none_interface_type, NULL);
