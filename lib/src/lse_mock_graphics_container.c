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

#include "lse_graphics_container.h"

#include "lse_object.h"

typedef struct lse_mock_graphics_container lse_mock_graphics_container;

struct lse_mock_graphics_container {
  lse_graphics_container_base base;
};

static void constructor(lse_object* object, void* arg) {
  lse_graphics_container_base_constructor((lse_graphics_container*)object, arg);
}

static void destructor(lse_object* object) {
  lse_graphics_container_base_destructor((lse_graphics_container*)object);
}

static lse_status configure(lse_graphics_container* container, const lse_window_settings* settings) {
  lse_mock_graphics_container* self = (lse_mock_graphics_container*)container;

  self->base.graphics = (lse_graphics*)lse_object_new(lse_mock_graphics_type, self->base.env);

  return LSE_OK;
}

static void destroy(lse_graphics_container* container) {
  if (lse_graphics_container_is_destroyed(container)) {
    return;
  }

  lse_graphics_container_base_destroy(container);
}

static bool is_destroyed(lse_graphics_container* container) {
  lse_mock_graphics_container* self = (lse_mock_graphics_container*)container;

  return !container || self->base.env == NULL;
}

static lse_graphics* begin_frame(lse_graphics_container* container) {
  lse_mock_graphics_container* self = (lse_mock_graphics_container*)container;

  return self->base.graphics;
}

static void end_frame(lse_graphics_container* container) {
}

static void set_title(lse_graphics_container* container, lse_string* title) {
}


// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_GRAPHICS_CONTAINER_VTABLE();
LSE_TYPE_INFO(
    lse_mock_graphics_container_type,
    lse_mock_graphics_container,
    lse_graphics_container_interface_type,
    &vtable);
