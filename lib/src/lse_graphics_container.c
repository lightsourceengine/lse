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

#include "lse_graphics.h"
#include "lse_object.h"
#include <assert.h>

void lse_graphics_container_base_constructor(lse_graphics_container* container, void* arg) {
  lse_graphics_container_base* base = lse_graphics_container_get_base(container);

  base->env = arg;
  lse_ref(base->env);
}

void lse_graphics_container_base_destructor(lse_graphics_container* container) {
  lse_graphics_container_base* base = lse_graphics_container_get_base(container);

  assert(base->env == NULL);

  lse_unref(base->graphics);
  lse_unref(base->env);
}

void lse_graphics_container_base_destroy(lse_graphics_container* container) {
  lse_graphics_container_base* base = lse_graphics_container_get_base(container);

  lse_graphics_destroy(base->graphics);
  lse_unref(base->graphics);
  base->graphics = NULL;

  lse_unref(base->env);
  base->env = NULL;
}

int32_t lse_graphics_container_get_width(lse_graphics_container* container) {
  lse_graphics_container_base* base = lse_graphics_container_get_base(container);

  return base->graphics ? lse_graphics_get_width(base->graphics) : 0;
}

int32_t lse_graphics_container_get_height(lse_graphics_container* container) {
  lse_graphics_container_base* base = lse_graphics_container_get_base(container);

  return base->graphics ? lse_graphics_get_height(base->graphics) : 0;
}