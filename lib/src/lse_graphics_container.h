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

#include "lse_types.h"
#include <lse.h>

//
// vtable
//

typedef struct lse_graphics_container_interface lse_graphics_container_interface;

struct lse_graphics_container_interface {
  lse_status (*configure)(lse_graphics_container*, const lse_window_settings*);
  void (*destroy)(lse_graphics_container*);
  bool (*is_destroyed)(lse_graphics_container*);
  void (*set_title)(lse_graphics_container*, lse_string*);
  lse_graphics* (*begin_frame)(lse_graphics_container*);
  void (*end_frame)(lse_graphics_container*);
};

#define LSE_GRAPHICS_CONTAINER_VTABLE()                                                                                \
  static lse_graphics_container_interface vtable = {                                                                   \
    .configure = configure,                                                                                            \
    .destroy = destroy,                                                                                                \
    .is_destroyed = is_destroyed,                                                                                      \
    .begin_frame = begin_frame,                                                                                        \
    .end_frame = end_frame,                                                                                            \
    .set_title = set_title,                                                                                            \
  };

//
// types
//

typedef struct lse_graphics_container_base lse_graphics_container_base;

struct lse_graphics_container_base {
  lse_env* env;
  lse_graphics* graphics;
};

//
// public c api
//

#define LSE_GRAPHICS_CONTAINER_V(INSTANCE, METHOD)                                                                     \
  ((lse_graphics_container_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE))
#define LSE_GRAPHICS_CONTAINER_A(INSTANCE, METHOD, ...)                                                                \
  ((lse_graphics_container_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))                               \
      ->METHOD((INSTANCE), __VA_ARGS__)

#define lse_graphics_container_configure(CONTAINER, ...) LSE_GRAPHICS_CONTAINER_A((CONTAINER), configure, __VA_ARGS__)
#define lse_graphics_container_destroy(CONTAINER) LSE_GRAPHICS_CONTAINER_V((CONTAINER), destroy)
#define lse_graphics_container_is_destroyed(CONTAINER) LSE_GRAPHICS_CONTAINER_V((CONTAINER), is_destroyed)
#define lse_graphics_container_set_title(CONTAINER, ...) LSE_GRAPHICS_CONTAINER_A((CONTAINER), set_title, __VA_ARGS__)
#define lse_graphics_container_begin_frame(CONTAINER) LSE_GRAPHICS_CONTAINER_V((CONTAINER), begin_frame)
#define lse_graphics_container_end_frame(CONTAINER) LSE_GRAPHICS_CONTAINER_V((CONTAINER), end_frame)
int32_t lse_graphics_container_get_width(lse_graphics_container* container);
int32_t lse_graphics_container_get_height(lse_graphics_container* container);

//
// protected base class api
//

void lse_graphics_container_base_constructor(lse_graphics_container* container, void* arg);
void lse_graphics_container_base_destructor(lse_graphics_container* container);
void lse_graphics_container_base_destroy(lse_graphics_container* container);

/**
 * Get the base data pointer of the object.
 *
 * In C the address of a struct equals the address of the first field. In order for this cast to work, the base data
 * struct MUST be the first field of the struct.
 */
#define lse_graphics_container_get_base(CONTAINER) ((lse_graphics_container_base*)(CONTAINER))
