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

#include "lse_gamepad.h"

#include "lse_object.h"
#include "lse_types.h"

typedef struct lse_mock_gamepad lse_mock_gamepad;

struct lse_mock_gamepad {
  int32_t placeholder;
};

static void constructor(lse_object* object, void* arg) {
}

static void destructor(lse_object* object) {
}

static void destroy(lse_gamepad* gamepad) {
}

static int32_t get_id(lse_gamepad* gamepad) {
  return 0;
}

static const char* get_name(lse_gamepad* gamepad) {
  return "";
}

static const char* get_uuid(lse_gamepad* gamepad) {
  return "";
}

static bool is_connected(lse_gamepad* gamepad) {
  return false;
}

static bool get_button_state(lse_gamepad* gamepad, int32_t button) {
  return false;
}

static float get_axis_state(lse_gamepad* gamepad, int32_t axis) {
  return 0;
}

static lse_hat get_hat_state(lse_gamepad* gamepad, int32_t hat_index) {
  return LSE_HAT_CENTERED;
}

static float get_analog_state(lse_gamepad* gamepad, lse_analog_key analog_key) {
  return 0;
}

static bool get_key_state(lse_gamepad* gamepad, lse_key mapped_key) {
  return false;
}

static int32_t get_button_count(lse_gamepad* gamepad) {
  return 0;
}

static int32_t get_axis_count(lse_gamepad* gamepad) {
  return 0;
}

static int32_t get_hat_count(lse_gamepad* gamepad) {
  return 0;
}

static void enable_mapping(lse_gamepad* gamepad, int32_t index) {
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_GAMEPAD_VTABLE();
LSE_TYPE_INFO(lse_mock_gamepad_type, lse_mock_gamepad, lse_gamepad_interface_type, &vtable);
