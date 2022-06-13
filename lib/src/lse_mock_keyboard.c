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

#include "lse_keyboard.h"

#include "lse_object.h"
#include "lse_types.h"

typedef struct lse_mock_keyboard lse_mock_keyboard;

struct lse_mock_keyboard {
  bool connected;
  bool destroyed;
};

static void constructor(lse_object* object, void* arg) {
  lse_mock_keyboard* self = (lse_mock_keyboard*)object;

  self->connected = true;
  self->destroyed = false;
}

static void destructor(lse_object* object) {
}

static int32_t get_id(lse_keyboard* keyboard) {
  return LSE_KEYBOARD_ID;
}

static const char* get_name(lse_keyboard* keyboard) {
  return LSE_KEYBOARD_NAME;
}

static const char* get_uuid(lse_keyboard* keyboard) {
  return LSE_KEYBOARD_UUID;
}

static bool is_connected(lse_keyboard* keyboard) {
  lse_mock_keyboard* self = (lse_mock_keyboard*)keyboard;

  return self->connected;
}

static bool get_scancode_state(lse_keyboard* keyboard, int32_t scancode) {
  return false;
}

static void destroy(lse_keyboard* keyboard) {
  if (lse_keyboard_is_destroyed(keyboard)) {
    return;
  }

  lse_mock_keyboard* self = (lse_mock_keyboard*)keyboard;

  self->destroyed = true;
}

static bool is_destroyed(lse_keyboard* keyboard) {
  lse_mock_keyboard* self = (lse_mock_keyboard*)keyboard;

  return !self || self->destroyed;
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_KEYBOARD_VTABLE();
LSE_TYPE_INFO(lse_mock_keyboard_type, lse_mock_keyboard, lse_keyboard_interface_type, &vtable);
