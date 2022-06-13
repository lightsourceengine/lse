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

#include "lse_video.h"

#include "lse_env.h"
#include "lse_gamepad.h"
#include "lse_object.h"
#include "lse_types.h"

// TODO: many methods here will crashed when called on a destroyed instance. need destroy checks.

typedef struct lse_sdl_video lse_sdl_video;

struct lse_sdl_video {
  lse_env* env;
  bool is_attached;
  bool is_destroyed;
};

static void constructor(lse_object* object, void* arg) {
  lse_sdl_video* self = (lse_sdl_video*)object;

  self->env = arg;
  lse_ref(self->env);
}

static void destructor(lse_object* object) {
  lse_sdl_video* self = (lse_sdl_video*)object;

  lse_unref(self->env);
}

static lse_status attach(lse_video* video) {
  if (lse_video_is_destroyed(video)) {
    return LSE_ERR_EOL;
  }

  lse_sdl_video* self = (lse_sdl_video*)video;
  lse_sdl* sdl = lse_get_sdl(self);

  if (self->is_attached) {
    return LSE_OK;
  }

  if (sdl->SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    LSE_LOG_SDL_ERROR(sdl, "SDL_Init");
    return LSE_STATUS_SUBSYSTEM_ERROR;
  }

  self->is_attached = true;

  return LSE_OK;
}

static void detach(lse_video* video) {
  if (lse_video_is_destroyed(video)) {
    return;
  }

  lse_sdl_video* self = (lse_sdl_video*)video;

  if (!self->is_attached) {
    return;
  }

  // TODO: in order to "transition" to another app, we need to fully quit SDL. this is only true
  //  for RPI's broadcom dispmax. May want different behavior on other platforms, like desktop environments.
  lse_get_sdl(self)->SDL_Quit();
  self->is_attached = false;
}

static bool is_attached(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return self->is_attached;
}

static void destroy(lse_video* video) {
  if (lse_video_is_destroyed(video)) {
    return;
  }

  lse_sdl_video* self = (lse_sdl_video*)video;

  if (self->is_attached) {
    lse_video_detach(video);
  }

  self->is_destroyed = true;
  self->is_attached = false;
  lse_unref(self->env);
  self->env = NULL;
}

static bool is_destroyed(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return !self || self->is_destroyed;
}

static const char* get_driver(lse_video* video, int32_t driver_index) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return self->env->sdl.SDL_GetVideoDriver(driver_index);
}

static int32_t get_driver_count(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return self->env->sdl.SDL_GetNumVideoDrivers();
}

static int32_t get_renderer_count(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return self->env->sdl.SDL_GetNumRenderDrivers();
}

static bool get_renderer_info(lse_video* video, int32_t renderer_index, lse_renderer_info* info) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  SDL_RendererInfo sdl_info;

  if (!info || self->env->sdl.SDL_GetRenderDriverInfo(renderer_index, &sdl_info) != 0) {
    return false;
  }

  *info = (lse_renderer_info){ .name = sdl_info.name,
                               .has_hardware_acceleration = (sdl_info.flags & SDL_RENDERER_ACCELERATED) != 0,
                               .has_vsync = (sdl_info.flags & SDL_RENDERER_PRESENTVSYNC) != 0,
                               .has_render_to_texture = (sdl_info.flags & SDL_RENDERER_TARGETTEXTURE) != 0,
                               .max_texture_width = sdl_info.max_texture_width,
                               .max_texture_height = sdl_info.max_texture_height };

  return true;
}

static bool find_closest_display_mode(
    lse_video* video,
    int32_t display_index,
    int32_t desired_width,
    int32_t desired_height,
    lse_display_mode* out) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  SDL_DisplayMode desired = {
    .w = desired_width, .h = desired_height, .refresh_rate = 0, .format = 0, .driverdata = NULL
  };
  SDL_DisplayMode result;

  if (self->env->sdl.SDL_GetClosestDisplayMode(display_index, &desired, &result) == NULL) {
    return false;
  }

  *out = (lse_display_mode){ .width = result.w, .height = result.h, .refresh_rate = result.refresh_rate };

  return true;
}

static const char* get_display_name(lse_video* video, int32_t display_index) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return self->env->sdl.SDL_GetDisplayName(display_index);
}

static bool get_current_display_mode(lse_video* video, int32_t display_index, lse_display_mode* mode) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  SDL_DisplayMode sdl_mode;

  if (!mode || self->env->sdl.SDL_GetCurrentDisplayMode(display_index, &sdl_mode)) {
    return false;
  }

  *mode = (lse_display_mode){
    .width = sdl_mode.w,
    .height = sdl_mode.h,
    .refresh_rate = sdl_mode.refresh_rate,
  };

  return true;
}

static bool get_desktop_display_mode(lse_video* video, int32_t display_index, lse_display_mode* mode) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  SDL_DisplayMode sdl_mode;

  if (!mode || self->env->sdl.SDL_GetCurrentDisplayMode(display_index, &sdl_mode)) {
    return false;
  }

  *mode = (lse_display_mode){
    .width = sdl_mode.w,
    .height = sdl_mode.h,
    .refresh_rate = sdl_mode.refresh_rate,
  };

  return true;
}

static int32_t get_display_count(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return self->env->sdl.SDL_GetNumVideoDisplays();
}

static bool
get_display_mode(lse_video* video, int32_t display_index, int32_t display_mode_index, lse_display_mode* out) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  SDL_DisplayMode sdl_display_mode;

  if (self->env->sdl.SDL_GetDisplayMode(display_index, display_mode_index, &sdl_display_mode) != 0) {
    return false;
  }

  out->width = sdl_display_mode.w;
  out->height = sdl_display_mode.h;
  out->refresh_rate = sdl_display_mode.refresh_rate;
  // TODO: add format

  return true;
}

static int32_t get_display_mode_count(lse_video* video, int32_t display_index) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return self->env->sdl.SDL_GetNumDisplayModes(display_index);
}

static bool process_events(lse_video* video) {
#define NUM_EVENTS_PER_FRAME 20
  static SDL_Event event_buffer[NUM_EVENTS_PER_FRAME];
  lse_sdl_video* self = (lse_sdl_video*)video;
  int32_t event_index = 0;
  bool result = true;
  int32_t event_count;
  SDL_Event* event;

  self->env->sdl.SDL_PumpEvents();
  event_count =
      self->env->sdl.SDL_PeepEvents(event_buffer, NUM_EVENTS_PER_FRAME, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);

  if (event_count < 0) {
    // TODO: SDL_GetError() ?
    result = false;
  }

  while (result && event_index < event_count) {
    // TODO: bail if sdl closed, window closed, etc.

    event = &event_buffer[event_index++];

    switch (event->type) {
      case SDL_QUIT:
        result = false;
        break;
      case SDL_KEYUP:
      case SDL_KEYDOWN:
        self->env->on_keyboard_button(
            LSE_KEYSPACE_SCANCODE,
            event->key.keysym.scancode,
            event->key.state == SDL_PRESSED,
            event->key.repeat != 0,
            self->env->on_keyboard_button_data);
        break;
      case SDL_JOYBUTTONUP:
      case SDL_JOYBUTTONDOWN:
        self->env->on_gamepad_button(
            event->jbutton.which,
            LSE_KEYSPACE_HARDWARE,
            event->jbutton.button,
            event->jbutton.state == SDL_PRESSED,
            false,
            self->env->on_gamepad_button_data);
        break;
      case SDL_JOYHATMOTION:
        self->env->on_gamepad_hat_motion(
            event->jhat.which,
            LSE_KEYSPACE_HARDWARE,
            event->jhat.hat,
            event->jhat.value,
            self->env->on_gamepad_hat_motion_data);
        break;
      case SDL_JOYAXISMOTION:
        self->env->on_gamepad_axis_motion(
            event->jaxis.which,
            LSE_KEYSPACE_HARDWARE,
            event->jaxis.axis,
            lse_sdl_axis_value_to_float(event->jaxis.value),
            self->env->on_gamepad_axis_motion_data);
        break;
      case SDL_JOYDEVICEADDED:
        lse_env_on_gamepad_connected(self->env, event->jdevice.which);
        break;
      case SDL_JOYDEVICEREMOVED:
        lse_env_on_gamepad_disconnected(self->env, event->jdevice.which);
        break;
      case SDL_CONTROLLERAXISMOTION:
        self->env->on_gamepad_axis_motion(
            event->caxis.which,
            LSE_KEYSPACE_MAPPED,
            event->caxis.axis,
            lse_sdl_axis_value_to_float(event->caxis.value),
            self->env->on_gamepad_axis_motion_data);
        break;
      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
        self->env->on_gamepad_button(
            event->cbutton.which,
            LSE_KEYSPACE_MAPPED,
            event->cbutton.button,
            event->cbutton.state == SDL_PRESSED,
            false,
            self->env->on_gamepad_button_data);
        break;
      default:
        break;
    }
  }

  return result;
#undef NUM_EVENTS_PER_FRAME
}

static lse_graphics_container* create_graphics_container(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return (lse_graphics_container*)lse_object_new(lse_sdl_graphics_container_type, self->env);
}

static lse_keyboard* create_keyboard(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return (lse_keyboard*)lse_object_new(lse_sdl_keyboard_type, self->env);
}

static lse_gamepad* create_gamepad(lse_video* video, int32_t index) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  lse_gamepad_arg_ arg = { .env = self->env, .index = index };

  return (lse_gamepad*)lse_object_new(lse_sdl_gamepad_type, &arg);
}

static size_t get_gamepad_count(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  int32_t count = self->env->sdl.SDL_NumJoysticks();

  if (count < 0) {
    // TODO: LOG SDLError
    return 0;
  }

  return (size_t)count;
}

static int32_t get_gamepad_instance_id_from_index(lse_video* video, int32_t index) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  if (self->env->sdl.SDL_JoystickGetDeviceInstanceID) {
    return self->env->sdl.SDL_JoystickGetDeviceInstanceID(index);
  } else {
    // For older versions of SDL, the only API to get the instance ID from an index
    // requires opening and closing a joystick. Since the joystick objects are ref
    // counted, the open call will not disturb any open joysticks.
    SDL_Joystick* joystick = self->env->sdl.SDL_JoystickOpen(index);
    int32_t instance_id;

    if (joystick) {
      instance_id = self->env->sdl.SDL_JoystickInstanceID(joystick);
      self->env->sdl.SDL_JoystickClose(joystick);
    } else {
      instance_id = -1;
    }

    return instance_id;
  }
}

static int32_t load_mappings_from_file(lse_video* video, const char* filename) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  SDL_RWops* rw = self->env->sdl.SDL_RWFromFile(filename, "r");

  if (!rw) {
    return -1;
  }

  return self->env->sdl.SDL_GameControllerAddMappingsFromRW(rw, SDL_TRUE);
}

static lse_string* get_mapping(lse_video* video, const char* uuid) {
  lse_sdl_video* sdl_video = (lse_sdl_video*)video;
  lse_string* result;
  SDL_JoystickGUID guid = sdl_video->env->sdl.SDL_JoystickGetGUIDFromString(uuid);
  char* mapping = sdl_video->env->sdl.SDL_GameControllerMappingForGUID(guid);

  if (mapping) {
    result = lse_string_new(mapping);
    sdl_video->env->sdl.SDL_free(mapping);
  } else {
    result = NULL;
  }

  return result;
}

static bool set_mapping(lse_video* video, const char* uuid, lse_string* mapping) {
  lse_sdl_video* self = (lse_sdl_video*)video;
  SDL_RWops* ops;
  int32_t result;

  if (!uuid || !mapping) {
    return false;
  }

  ops = self->env->sdl.SDL_RWFromConstMem(lse_string_as_cstring(mapping), (int32_t)lse_string_size(mapping));

  if (!ops) {
    return false;
  }

  result = self->env->sdl.SDL_GameControllerAddMappingsFromRW(ops, 1);

  if (result < 1) {
    return false;
  }

  return true;
}

static const char* get_platform_name(lse_video* video) {
  lse_sdl_video* self = (lse_sdl_video*)video;

  return self->env->sdl.SDL_GetPlatform();
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_VIDEO_VTABLE();
LSE_TYPE_INFO(lse_sdl_video_type, lse_sdl_video, lse_video_interface_type, &vtable);
