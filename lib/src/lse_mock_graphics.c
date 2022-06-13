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

#include "lse_graphics.h"

#include "lse_object.h"

typedef struct lse_mock_graphics lse_mock_graphics;

struct lse_mock_graphics {
  lse_graphics_base base;
  bool is_configured;
};

// @override
static void constructor(lse_object* object, void* arg) {
  lse_graphics* self = (lse_graphics*)object;

  lse_graphics_base_constructor(self, arg);
}

// @override
static void destructor(lse_object* object) {
  lse_graphics* self = (lse_graphics*)object;

  lse_graphics_base_destructor(self);
}

// @override
static lse_status configure(lse_graphics* graphics, void* arg) {
  lse_mock_graphics* self = (lse_mock_graphics*)graphics;

  if (lse_graphics_is_destroyed(graphics)) {
    return LSE_ERR_EOL;
  }

  if (self->is_configured) {
    return LSE_ERR_ALREADY_CONFIGURED;
  }

  self->is_configured = true;

  return LSE_OK;
}

// @override
static void destroy(lse_graphics* graphics) {
  if (lse_graphics_is_destroyed(graphics)) {
    return;
  }

  lse_graphics_base_destroy(graphics);
}

// @override
static bool is_destroyed(lse_graphics* graphics) {
  return lse_graphics_base_is_destroyed(graphics);
}

// @override
static void begin(lse_graphics* graphics) {
}

// @override
static void end(lse_graphics* graphics) {
  lse_graphics_base_end(graphics);
}

// @override
static void push_state(lse_graphics* graphics) {
  lse_graphics_base_push_state(graphics);
}

// @override
static void pop_state(lse_graphics* graphics) {
  lse_graphics_base_pop_state(graphics);
}

// @override
static const lse_matrix* get_matrix(lse_graphics* graphics) {
  return lse_graphics_base_get_matrix(graphics);
}

// @override
static void set_matrix(lse_graphics* graphics, lse_matrix* matrix) {
  lse_graphics_base_set_matrix(graphics, matrix);
}

// @override
static void set_opacity(lse_graphics* graphics, float opacity) {
  lse_graphics_base_set_opacity(graphics, opacity);
}

// @override
static void set_clip_rect(lse_graphics* graphics, lse_rect_f* rect) {
  lse_graphics_base_set_clip_rect(graphics, rect);
}

// @override
static void clear(lse_graphics* graphics, lse_color color) {
}

// @override
static void remove_image(lse_graphics* graphics, lse_image* image) {
}

// @override
static void draw_render_object(lse_graphics* graphics, lse_render_object* render_object, lse_color color) {
}

// @override
static lse_render_object* destroy_render_object(lse_graphics* graphics, lse_render_object* render_object) {
  return NULL;
}

// @override
static lse_render_object*
end_queue(lse_graphics* graphics, int32_t width, int32_t height, lse_render_object* render_object) {
  return NULL;
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_GRAPHICS_VTABLE();
LSE_TYPE_INFO(lse_mock_graphics_type, lse_mock_graphics, lse_graphics_interface_type, &vtable);
