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

#include <assert.h>

#include "lse_env.h"
#include "lse_object.h"
#include "lse_types.h"

typedef struct lse_mock_video lse_mock_video;

struct lse_mock_video {
  lse_env* env;
  bool is_attached;
  bool is_destroyed;
  lse_display_mode display_mode;
  lse_renderer_info renderer_info;
};

static void constructor(lse_object* object, void* arg) {
  lse_mock_video* mock_video = (lse_mock_video*)object;

  mock_video->display_mode = (lse_display_mode){
    .width = 1280,
    .height = 720,
    .refresh_rate = 60,
  };
  mock_video->renderer_info = (lse_renderer_info){
    .name = "mock renderer",
    .has_hardware_acceleration = true,
    .has_vsync = true,
    .has_render_to_texture = true,
    .max_texture_width = 0xFFFF,
    .max_texture_height = 0xFFFF,
  };
  mock_video->env = arg;
  lse_ref(mock_video->env);
}

static void destructor(lse_object* object) {
  lse_mock_video* self = (lse_mock_video*)object;

  assert(self->is_destroyed);

  lse_unref(self->env);
}

static lse_status attach(lse_video* video) {
  if (lse_video_is_destroyed(video)) {
    return LSE_ERR_EOL;
  }

  lse_mock_video* self = (lse_mock_video*)video;

  if (self->is_attached) {
    return LSE_OK;
  }

  self->is_attached = true;

  return LSE_OK;
}

static void detach(lse_video* video) {
  lse_mock_video* self = (lse_mock_video*)video;

  self->is_attached = false;
}

static bool is_attached(lse_video* video) {
  lse_mock_video* self = (lse_mock_video*)video;

  return self->is_attached;
}

static void destroy(lse_video* video) {
  if (lse_video_is_destroyed(video)) {
    return;
  }

  lse_mock_video* self = (lse_mock_video*)video;

  if (self->is_attached) {
    lse_video_detach(video);
  }

  self->is_destroyed = true;
  self->is_attached = false;
  lse_unref(self->env);
  self->env = NULL;
}

static bool is_destroyed(lse_video* video) {
  lse_mock_video* self = (lse_mock_video*)video;

  return !self || self->is_destroyed;
}

static const char* get_driver(lse_video* video, int32_t driver_index) {
  return driver_index == 0 ? "default" : NULL;
}

static int32_t get_driver_count(lse_video* video) {
  return 1;
}

static int32_t get_renderer_count(lse_video* video) {
  return 1;
}

static bool get_renderer_info(lse_video* video, int32_t renderer_index, lse_renderer_info* info) {
  lse_mock_video* mock_video = (lse_mock_video*)video;

  if (!info || renderer_index != 0) {
    return false;
  }

  *info = mock_video->renderer_info;

  return true;
}

static bool find_closest_display_mode(
    lse_video* video,
    int32_t display_index,
    int32_t desired_width,
    int32_t desired_height,
    lse_display_mode* out) {
  lse_mock_video* mock_video = (lse_mock_video*)video;

  if (display_index != 0) {
    return false;
  }

  *out = mock_video->display_mode;

  return true;
}

static bool
get_display_mode(lse_video* video, int32_t display_index, int32_t display_mode_index, lse_display_mode* out) {
  lse_mock_video* mock_video = (lse_mock_video*)video;

  if (mock_video && display_index == 0 && display_mode_index == 0 && out) {
    *out = mock_video->display_mode;

    return true;
  }

  return false;
}

static int32_t get_display_mode_count(lse_video* video, int32_t display_index) {
  return display_index == 0 ? 1 : -1;
}

static const char* get_display_name(lse_video* video, int32_t display_index) {
  return display_index == 0 ? "mock_display" : NULL;
}

static bool get_current_display_mode(lse_video* video, int32_t display_index, lse_display_mode* mode) {
  lse_mock_video* mock_video = (lse_mock_video*)video;

  if (mode && display_index == 0) {
    *mode = mock_video->display_mode;
    return true;
  }

  return false;
}

static bool get_desktop_display_mode(lse_video* video, int32_t display_index, lse_display_mode* mode) {
  lse_mock_video* mock_video = (lse_mock_video*)video;

  if (mode && display_index == 0) {
    *mode = mock_video->display_mode;
    return true;
  }

  return false;
}

static int32_t get_display_count(lse_video* video) {
  return 1;
}

static bool process_events(lse_video* video) {
  return true;
}

static lse_graphics_container* create_graphics_container(lse_video* video) {
  lse_mock_video* mock_video = (lse_mock_video*)video;

  return (lse_graphics_container*)lse_object_new(lse_mock_graphics_container_type, mock_video->env);
}

static lse_keyboard* create_keyboard(lse_video* video) {
  return (lse_keyboard*)lse_object_new(lse_mock_keyboard_type, NULL);
}

static lse_gamepad* create_gamepad(lse_video* video, int32_t index) {
  return index == 0 ? (lse_gamepad*)lse_object_new(lse_mock_gamepad_type, NULL) : NULL;
}

static size_t get_gamepad_count(lse_video* video) {
  return 1;
}

static int32_t get_gamepad_instance_id_from_index(lse_video* video, int32_t index) {
  return -1;
}

static int32_t load_mappings_from_file(lse_video* video, const char* filename) {
  return 0;
}

static lse_string* get_mapping(lse_video* video, const char* uuid) {
  return NULL;
}

static bool set_mapping(lse_video* video, const char* uuid, lse_string* mapping) {
  lse_unref(mapping);
  return false;
}

static const char* get_platform_name(lse_video* video) {
  return "Mock";
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_VIDEO_VTABLE();
LSE_TYPE_INFO(lse_mock_video_type, lse_mock_video, lse_video_interface_type, &vtable);
