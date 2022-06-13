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

#include "lse_env.h"
#include "lse_object.h"
#include "lse_types.h"
#include <assert.h>

#define GAMEPAD_GUID_SIZE 33

typedef struct lse_sdl_gamepad lse_sdl_gamepad;

struct lse_sdl_gamepad {
  lse_env* env;
  int32_t instance_id;
  char uuid[GAMEPAD_GUID_SIZE];
  SDL_GameController* game_controller;
  SDL_Joystick* joystick;
};

static void constructor(lse_object* object, void* arg) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)object;
  lse_gamepad_arg gamepad_arg = arg;
  lse_env* env = gamepad_arg->env;
  int32_t index = gamepad_arg->index;
  lse_sdl* sdl = &self->env->sdl;
  SDL_JoystickGUID joystick_guid;

  self->instance_id = -1;

  // TODO: return from constructor?
  if (sdl->SDL_IsGameController(index) == SDL_TRUE) {
    self->game_controller = sdl->SDL_GameControllerOpen(index);

    if (!self->game_controller) {
      LSE_LOG_ERROR("$s(%i): %s", SDL_STRINGIFY_ARG(SDL_GameControllerOpen), sdl->SDL_GetError());
      return;
    }

    self->joystick = sdl->SDL_GameControllerGetJoystick(self->game_controller);

    if (!self->joystick) {
      sdl->SDL_GameControllerClose(self->game_controller);
      LSE_LOG_ERROR("$s(%i): %s", SDL_STRINGIFY_ARG(SDL_GameControllerGetJoystick), sdl->SDL_GetError());
      return;
    }
  } else {
    self->joystick = sdl->SDL_JoystickOpen(index);

    if (!self->joystick) {
      LSE_LOG_ERROR("$s(%i): %s", SDL_STRINGIFY_ARG(SDL_JoystickOpen), sdl->SDL_GetError());
      return;
    }
  }

  self->instance_id = sdl->SDL_JoystickInstanceID(self->joystick);

  joystick_guid = sdl->SDL_JoystickGetGUID(self->joystick);
  // TODO: return "zero GUID" on failure

  sdl->SDL_JoystickGetGUIDString(joystick_guid, self->uuid, GAMEPAD_GUID_SIZE);

  self->env = env;
  lse_ref(env);
}

static void destructor(lse_object* object) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)object;

  assert(self->joystick == NULL);
  assert(self->game_controller == NULL);

  lse_unref(self->env);
}

static void destroy(lse_gamepad* gamepad) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;
  lse_sdl* sdl = &self->env->sdl;

  if (self->game_controller) {
    sdl->SDL_GameControllerClose(self->game_controller);
  } else if (self->joystick) {
    sdl->SDL_JoystickClose(self->joystick);
  }

  self->game_controller = NULL;
  self->joystick = NULL;
  self->instance_id = -1;
  self->uuid[0] = '\0';
  lse_unref(self->env);
  self->env = NULL;
}

static int32_t get_id(lse_gamepad* gamepad) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->instance_id;
}

static const char* get_name(lse_gamepad* gamepad) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;
  lse_sdl* sdl = lse_get_sdl(self);
  const char* name;

  if (self->game_controller) {
    name = sdl->SDL_GameControllerName(self->game_controller);
  } else if (self->joystick) {
    name = sdl->SDL_JoystickName(self->joystick);
  } else {
    name = NULL;
  }

  return name ? name : "";
}

static const char* get_uuid(lse_gamepad* gamepad) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->uuid;
}

static bool is_connected(lse_gamepad* gamepad) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->game_controller || self->joystick;
}

static bool get_button_state(lse_gamepad* gamepad, int32_t button) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->joystick && lse_get_sdl(self)->SDL_JoystickGetButton(self->joystick, button) > 0;
}

static float get_axis_state(lse_gamepad* gamepad, int32_t axis) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  if (!self->joystick) {
    return 0;
  }

  return lse_sdl_axis_value_to_float(lse_get_sdl(self)->SDL_JoystickGetAxis(self->joystick, axis));
}

static lse_hat get_hat_state(lse_gamepad* gamepad, int32_t hat_index) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->joystick ? (lse_hat)(lse_get_sdl(self)->SDL_JoystickGetHat(self->joystick, hat_index))
                        : LSE_HAT_CENTERED;
}

static float get_analog_state(lse_gamepad* gamepad, lse_analog_key analog_key) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;
  Sint16 value;

  if (self->game_controller) {
    value = lse_get_sdl(self)->SDL_GameControllerGetAxis(self->game_controller, (SDL_GameControllerAxis)analog_key);
  } else {
    value = 0;
  }

  return lse_sdl_axis_value_to_float(value);
}

static bool get_key_state(lse_gamepad* gamepad, lse_key key) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->game_controller &&
         lse_get_sdl(self)->SDL_GameControllerGetButton(self->game_controller, (SDL_GameControllerButton)key);
}

static int32_t get_button_count(lse_gamepad* gamepad) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->joystick && lse_get_sdl(self)->SDL_JoystickNumButtons(self->joystick);
}

static int32_t get_axis_count(lse_gamepad* gamepad) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->joystick && lse_get_sdl(self)->SDL_JoystickNumAxes(self->joystick);
}

static int32_t get_hat_count(lse_gamepad* gamepad) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;

  return self->joystick && lse_get_sdl(self)->SDL_JoystickNumHats(self->joystick);
}

static void enable_mapping(lse_gamepad* gamepad, int32_t index) {
  lse_sdl_gamepad* self = (lse_sdl_gamepad*)gamepad;
  lse_sdl* sdl = &self->env->sdl;

  if (self->game_controller || !sdl->SDL_IsGameController(index)) {
    return;
  }

  sdl->SDL_JoystickClose(self->joystick);

  self->game_controller = sdl->SDL_GameControllerOpen(index);
  // TODO: check value

  self->joystick = sdl->SDL_GameControllerGetJoystick(self->game_controller);
  // TODO: check value
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_GAMEPAD_VTABLE();
LSE_TYPE_INFO(lse_sdl_gamepad_type, lse_sdl_gamepad, lse_gamepad_interface_type, &vtable);
