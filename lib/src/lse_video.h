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

#include "lse_types.h"
#include <lse.h>

//
// vtable
//

typedef struct lse_video_interface lse_video_interface;

struct lse_video_interface {
  lse_status (*attach)(lse_video*);
  void (*detach)(lse_video*);
  bool (*is_attached)(lse_video*);

  void (*destroy)(lse_video*);
  bool (*is_destroyed)(lse_video*);

  const char* (*get_driver)(lse_video*, int32_t);
  int32_t (*get_driver_count)(lse_video*);
  const char* (*get_display_name)(lse_video*, int32_t);
  bool (*get_current_display_mode)(lse_video*, int32_t, lse_display_mode*);
  bool (*get_desktop_display_mode)(lse_video*, int32_t, lse_display_mode*);
  int32_t (*get_display_count)(lse_video*);
  bool (*get_display_mode)(lse_video*, int32_t, int32_t, lse_display_mode*);
  int32_t (*get_display_mode_count)(lse_video*, int32_t);
  int32_t (*get_renderer_count)(lse_video*);
  bool (*get_renderer_info)(lse_video*, int32_t, lse_renderer_info*);
  bool (*find_closest_display_mode)(lse_video*, int32_t, int32_t, int32_t, lse_display_mode*);

  bool (*process_events)(lse_video*);

  lse_graphics_container* (*create_graphics_container)(lse_video*);

  lse_keyboard* (*create_keyboard)(lse_video*);
  lse_gamepad* (*create_gamepad)(lse_video*, int32_t);
  size_t (*get_gamepad_count)(lse_video*);
  int32_t (*get_gamepad_instance_id_from_index)(lse_video*, int32_t);
  int32_t (*load_mappings_from_file)(lse_video*, const char*);
  lse_string* (*get_mapping)(lse_video*, const char*);
  bool (*set_mapping)(lse_video*, const char*, lse_string*);
  const char* (*get_platform_name)(lse_video*);
};

#define LSE_VIDEO_VTABLE()                                                                                             \
  static lse_video_interface vtable = {                                                                                \
    .attach = attach,                                                                                                  \
    .detach = detach,                                                                                                  \
    .is_attached = is_attached,                                                                                        \
    .destroy = destroy,                                                                                                \
    .is_destroyed = is_destroyed,                                                                                      \
    .get_display_name = get_display_name,                                                                              \
    .get_current_display_mode = get_current_display_mode,                                                              \
    .get_desktop_display_mode = get_desktop_display_mode,                                                              \
    .get_display_count = get_display_count,                                                                            \
    .get_driver = get_driver,                                                                                          \
    .get_driver_count = get_driver_count,                                                                              \
    .get_display_mode = get_display_mode,                                                                              \
    .get_display_mode_count = get_display_mode_count,                                                                  \
    .get_renderer_count = get_renderer_count,                                                                          \
    .get_renderer_info = get_renderer_info,                                                                            \
    .find_closest_display_mode = find_closest_display_mode,                                                            \
    .process_events = process_events,                                                                                  \
    .create_keyboard = create_keyboard,                                                                                \
    .create_gamepad = create_gamepad,                                                                                  \
    .create_graphics_container = create_graphics_container,                                                            \
    .get_gamepad_count = get_gamepad_count,                                                                            \
    .get_gamepad_instance_id_from_index = get_gamepad_instance_id_from_index,                                          \
    .load_mappings_from_file = load_mappings_from_file,                                                                \
    .get_mapping = get_mapping,                                                                                        \
    .set_mapping = set_mapping,                                                                                        \
    .get_platform_name = get_platform_name,                                                                            \
  }

//
// public c api
//

#define LSE_VIDEO_V(INSTANCE, METHOD)                                                                                  \
  ((lse_video_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE))
#define LSE_VIDEO_A(INSTANCE, METHOD, ...)                                                                             \
  ((lse_video_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE), __VA_ARGS__)

#define lse_video_attach(VIDEO) LSE_VIDEO_V((VIDEO), attach)
#define lse_video_detach(VIDEO) LSE_VIDEO_V((VIDEO), detach)
#define lse_video_is_attached(VIDEO) LSE_VIDEO_V((VIDEO), is_attached)
#define lse_video_destroy(VIDEO) LSE_VIDEO_V((VIDEO), destroy)
#define lse_video_is_destroyed(VIDEO) LSE_VIDEO_V((VIDEO), is_destroyed)

#define lse_video_create_keyboard(VIDEO) LSE_VIDEO_V((VIDEO), create_keyboard)
#define lse_video_process_events(VIDEO) LSE_VIDEO_V((VIDEO), process_events)
#define lse_video_get_gamepad_count(VIDEO) LSE_VIDEO_V((VIDEO), get_gamepad_count)
#define lse_video_get_driver_count(VIDEO) LSE_VIDEO_V((VIDEO), get_driver_count)
#define lse_video_get_renderer_count(VIDEO) LSE_VIDEO_V((VIDEO), get_renderer_count)
#define lse_video_get_display_count(VIDEO) LSE_VIDEO_V((VIDEO), get_display_count)
#define lse_video_get_platform_name(VIDEO) LSE_VIDEO_V((VIDEO), get_platform_name)
#define lse_video_create_graphics_container(VIDEO) LSE_VIDEO_V((VIDEO), create_graphics_container)

#define lse_video_present(VIDEO, ...) LSE_VIDEO_A((VIDEO), present, __VA_ARGS__)
#define lse_video_create_gamepad(VIDEO, ...) LSE_VIDEO_A((VIDEO), create_gamepad, __VA_ARGS__)
#define lse_video_get_driver(VIDEO, ...) LSE_VIDEO_A((VIDEO), get_driver, __VA_ARGS__)
#define lse_video_get_display_name(VIDEO, ...) LSE_VIDEO_A((VIDEO), get_display_name, __VA_ARGS__)
#define lse_video_find_closest_display_mode(VIDEO, ...) LSE_VIDEO_A((VIDEO), find_closest_display_mode, __VA_ARGS__)
#define lse_video_get_renderer_info(VIDEO, ...) LSE_VIDEO_A((VIDEO), get_renderer_info, __VA_ARGS__)
#define lse_video_get_display_mode_count(VIDEO, ...) LSE_VIDEO_A((VIDEO), get_display_mode_count, __VA_ARGS__)
#define lse_video_get_display_mode(VIDEO, ...) LSE_VIDEO_A((VIDEO), get_display_mode, __VA_ARGS__)
#define lse_video_get_desktop_display_mode(VIDEO, ...) LSE_VIDEO_A((VIDEO), get_desktop_display_mode, __VA_ARGS__)
#define lse_video_get_current_display_mode(VIDEO, ...) LSE_VIDEO_A((VIDEO), get_current_display_mode, __VA_ARGS__)
#define lse_video_get_gamepad_instance_id_from_index(VIDEO, ...)                                                       \
  LSE_VIDEO_A((VIDEO), get_gamepad_instance_id_from_index, __VA_ARGS__)
#define lse_video_load_mappings_from_file(VIDEO, ...) LSE_VIDEO_A((VIDEO), load_mappings_from_file, __VA_ARGS__)
#define lse_video_get_mapping(VIDEO, ...) LSE_VIDEO_A((VIDEO), get_mapping, __VA_ARGS__)
#define lse_video_set_mapping(VIDEO, ...) LSE_VIDEO_A((VIDEO), set_mapping, __VA_ARGS__)

lse_string* lse_video_create_stub_mapping(lse_video* video, const char* uuid);
