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

#define LSE_KEYBOARD_ID -2
#define LSE_KEYBOARD_NAME "System Keyboard"
#define LSE_KEYBOARD_UUID "00000000000000000000000000000001"

//
// vtable
//

typedef struct lse_keyboard_interface lse_keyboard_interface;

struct lse_keyboard_interface {
  int32_t (*get_id)(lse_keyboard*);
  const char* (*get_name)(lse_keyboard*);
  const char* (*get_uuid)(lse_keyboard*);
  bool (*is_connected)(lse_keyboard*);
  bool (*get_scancode_state)(lse_keyboard*, int32_t);
  void (*destroy)(lse_keyboard*);
  bool (*is_destroyed)(lse_keyboard*);
};

#define LSE_KEYBOARD_VTABLE()                                                                                          \
  static lse_keyboard_interface vtable = {                                                                             \
    .get_id = get_id,                                                                                                  \
    .get_name = get_name,                                                                                              \
    .get_uuid = get_uuid,                                                                                              \
    .is_connected = is_connected,                                                                                      \
    .get_scancode_state = get_scancode_state,                                                                          \
    .destroy = destroy,                                                                                                \
    .is_destroyed = is_destroyed,                                                                                      \
  }

//
// public c api
//

#define LSE_KEYBOARD_V(INSTANCE, METHOD)                                                                               \
  ((lse_keyboard_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE))
#define LSE_KEYBOARD_A(INSTANCE, METHOD, ...)                                                                          \
  ((lse_keyboard_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE), __VA_ARGS__)

#define lse_keyboard_destroy(INSTANCE) LSE_KEYBOARD_V((INSTANCE), destroy)
