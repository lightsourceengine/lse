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

void lse_gamepad_destroy(lse_gamepad* gamepad) {
  if (gamepad) {
    LSE_GAMEPAD_V(gamepad, destroy);
  }
}

LSE_API int32_t LSE_CDECL lse_gamepad_get_id(lse_gamepad* gamepad) {
  return LSE_GAMEPAD_V(gamepad, get_id);
}

LSE_API const char* LSE_CDECL lse_gamepad_get_name(lse_gamepad* gamepad) {
  return LSE_GAMEPAD_V(gamepad, get_name);
}

LSE_API const char* LSE_CDECL lse_gamepad_get_uuid(lse_gamepad* gamepad) {
  return LSE_GAMEPAD_V(gamepad, get_uuid);
}

LSE_API bool LSE_CDECL lse_gamepad_is_connected(lse_gamepad* gamepad) {
  return LSE_GAMEPAD_V(gamepad, is_connected);
}

LSE_API bool LSE_CDECL lse_gamepad_get_button_state(lse_gamepad* gamepad, int32_t button) {
  return LSE_GAMEPAD_A(gamepad, get_button_state, button);
}

LSE_API float LSE_CDECL lse_gamepad_get_axis_state(lse_gamepad* gamepad, int32_t axis_index) {
  return LSE_GAMEPAD_A(gamepad, get_axis_state, axis_index);
}

LSE_API lse_hat LSE_CDECL lse_gamepad_get_hat_state(lse_gamepad* gamepad, int32_t hat_index) {
  return LSE_GAMEPAD_A(gamepad, get_hat_state, hat_index);
}

LSE_API float LSE_CDECL lse_gamepad_get_analog_state(lse_gamepad* gamepad, lse_analog_key analog_key) {
  return LSE_GAMEPAD_A(gamepad, get_analog_state, analog_key);
}

LSE_API bool LSE_CDECL lse_gamepad_get_key_state(lse_gamepad* gamepad, lse_key key) {
  return LSE_GAMEPAD_A(gamepad, get_key_state, key);
}

LSE_API int32_t LSE_CDECL lse_gamepad_get_button_count(lse_gamepad* gamepad) {
  return LSE_GAMEPAD_V(gamepad, get_button_count);
}

LSE_API int32_t LSE_CDECL lse_gamepad_get_axis_count(lse_gamepad* gamepad) {
  return LSE_GAMEPAD_V(gamepad, get_axis_count);
}

LSE_API int32_t LSE_CDECL lse_gamepad_get_hat_count(lse_gamepad* gamepad) {
  return LSE_GAMEPAD_V(gamepad, get_hat_count);
}
