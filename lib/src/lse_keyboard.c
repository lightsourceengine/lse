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

LSE_API int32_t LSE_CDECL lse_keyboard_get_id(lse_keyboard* keyboard) {
  return LSE_KEYBOARD_V(keyboard, get_id);
}

LSE_API const char* LSE_CDECL lse_keyboard_get_name(lse_keyboard* keyboard) {
  return LSE_KEYBOARD_V(keyboard, get_name);
}

LSE_API const char* LSE_CDECL lse_keyboard_get_uuid(lse_keyboard* keyboard) {
  return LSE_KEYBOARD_V(keyboard, get_uuid);
}

LSE_API bool LSE_CDECL lse_keyboard_is_connected(lse_keyboard* keyboard) {
  return LSE_KEYBOARD_V(keyboard, is_connected);
}

LSE_API bool LSE_CDECL lse_keyboard_is_destroyed(lse_keyboard* keyboard) {
  return LSE_KEYBOARD_V(keyboard, is_destroyed);
}

LSE_API bool LSE_CDECL lse_keyboard_get_scancode_state(lse_keyboard* keyboard, int32_t scancode) {
  return LSE_KEYBOARD_A(keyboard, get_scancode_state, scancode);
}
