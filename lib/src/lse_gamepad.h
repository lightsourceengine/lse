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

//
// vtable
//

typedef struct lse_gamepad_interface lse_gamepad_interface;

struct lse_gamepad_interface {
  int32_t (*get_id)(lse_gamepad*);
  const char* (*get_name)(lse_gamepad*);
  const char* (*get_uuid)(lse_gamepad*);
  bool (*is_connected)(lse_gamepad*);
  bool (*get_button_state)(lse_gamepad*, int32_t);
  float (*get_axis_state)(lse_gamepad*, int32_t);
  lse_hat (*get_hat_state)(lse_gamepad*, int32_t);
  bool (*get_key_state)(lse_gamepad*, lse_key);
  float (*get_analog_state)(lse_gamepad*, lse_analog_key);
  int32_t (*get_button_count)(lse_gamepad*);
  int32_t (*get_axis_count)(lse_gamepad*);
  int32_t (*get_hat_count)(lse_gamepad*);
  void (*enable_mapping)(lse_gamepad*, int32_t);
  void (*destroy)(lse_gamepad*);
};

#define LSE_GAMEPAD_VTABLE()                                                                                           \
  static lse_gamepad_interface vtable = {                                                                              \
    .get_id = get_id,                                                                                                  \
    .get_name = get_name,                                                                                              \
    .get_uuid = get_uuid,                                                                                              \
    .is_connected = is_connected,                                                                                      \
    .get_button_state = get_button_state,                                                                              \
    .get_axis_state = get_axis_state,                                                                                  \
    .get_hat_state = get_hat_state,                                                                                    \
    .get_key_state = get_key_state,                                                                                    \
    .get_analog_state = get_analog_state,                                                                              \
    .get_button_count = get_button_count,                                                                              \
    .get_axis_count = get_axis_count,                                                                                  \
    .get_hat_count = get_hat_count,                                                                                    \
    .enable_mapping = enable_mapping,                                                                                  \
    .destroy = destroy,                                                                                                \
  }

//
// types
//

// constructor argument
typedef struct lse_gamepad_arg_ {
  lse_env* env;
  int32_t index;
} lse_gamepad_arg_;

typedef lse_gamepad_arg_* lse_gamepad_arg;

//
// public c api
//

#define LSE_GAMEPAD_V(INSTANCE, METHOD)                                                                                \
  ((lse_gamepad_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE))
#define LSE_GAMEPAD_A(INSTANCE, METHOD, ...)                                                                           \
  ((lse_gamepad_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE), __VA_ARGS__)

#define lse_gamepad_enable_mapping(GAMEPAD, ...) LSE_GAMEPAD_A((GAMEPAD), enable_mapping, __VA_ARGS__)
void lse_gamepad_destroy(lse_gamepad* gamepad);
