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

#include "lse_env.h"
#include "lse_object.h"
#include "lse_types.h"
#include <assert.h>

typedef struct lse_sdl_keyboard lse_sdl_keyboard;

struct lse_sdl_keyboard {
  lse_env* env;
};

static void constructor(lse_object* object, void* arg) {
  lse_env* env = arg;
  lse_sdl_keyboard* self = (lse_sdl_keyboard*)object;

  self->env = env;
  lse_ref(env);
}

static void destructor(lse_object* object) {
  lse_sdl_keyboard* self = (lse_sdl_keyboard*)object;

  assert(self->env == NULL);

  lse_unref(self->env);
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
  // NOTE: connected/destroyed mean the same thing for this object
  return lse_keyboard_is_destroyed(keyboard);
}

static bool get_scancode_state(lse_keyboard* keyboard, int32_t scancode) {
  lse_sdl_keyboard* self = (lse_sdl_keyboard*)keyboard;
  int32_t len;
  const Uint8* state;

  if (lse_keyboard_is_destroyed(keyboard)) {
    return false;
  }

  len = 0;
  state = lse_get_sdl(self)->SDL_GetKeyboardState(&len);

  return (state && (int32_t)scancode > 0 && (int32_t)scancode < len && state[scancode] != 0);
}

static void destroy(lse_keyboard* keyboard) {
  if (lse_keyboard_is_destroyed(keyboard)) {
    return;
  }

  lse_sdl_keyboard* self = (lse_sdl_keyboard*)keyboard;

  lse_unref(self->env);
  self->env = NULL;
}

static bool is_destroyed(lse_keyboard* keyboard) {
  lse_sdl_keyboard* self = (lse_sdl_keyboard*)keyboard;

  return !self || !self->env;
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_KEYBOARD_VTABLE();
LSE_TYPE_INFO(lse_sdl_keyboard_type, lse_sdl_keyboard, lse_keyboard_interface_type, &vtable);
