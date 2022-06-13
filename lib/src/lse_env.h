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

#include "lse.h"
#include "lse_sdl.h"
#include "lse_types.h"
#include "stc/forward.h"

forward_cvec(cvec_gamepads, lse_gamepad_ptr);
forward_cvec(cvec_windows, lse_window_ptr);

typedef struct cmap_mappings_entry {
  lse_string* uuid;
  lse_string* user_mapping;
  lse_string* intrinsic_mapping;
} cmap_mappings_entry;

forward_cmap(cmap_mappings, lse_string_ptr, cmap_mappings_entry);

struct lse_env {
  lse_env_state state;
  bool was_quit_requested;

  lse_sdl sdl;
  lse_video* video;
  lse_keyboard* keyboard;
  lse_font_store* fonts;
  cvec_gamepads gamepads;
  cvec_windows windows;
  cmap_mappings mappings;

  lse_gamepad_status_callback on_gamepad_status;
  void* on_gamepad_status_data;
  lse_gamepad_button_callback on_gamepad_button;
  void* on_gamepad_button_data;
  lse_gamepad_axis_motion_callback on_gamepad_axis_motion;
  void* on_gamepad_axis_motion_data;
  lse_gamepad_hat_motion_callback on_gamepad_hat_motion;
  void* on_gamepad_hat_motion_data;
  lse_keyboard_button_callback on_keyboard_button;
  void* on_keyboard_button_data;

  lse_thread_pool* pool;
  lse_thread_pool_free pool_free;
  lse_thread_pool_queue pool_queue;
  lse_thread_pool_cancel pool_cancel;
};

void lse_env_on_gamepad_connected(lse_env* env, int32_t index);
void lse_env_on_gamepad_disconnected(lse_env* env, int32_t instance_id);
void cmap_mappings_entry_drop(cmap_mappings_entry* entry);
void cvec_windows_value_drop(lse_window_ptr* window);
void cvec_gamepads_value_drop(lse_gamepad_ptr* gamepad);

lse_thread_pool_task* lse_env_add_thread_pool_task(
    lse_env* env,
    const char* resource_name,
    lse_thread_pool_work_callback work_callback,
    lse_thread_pool_complete_callback complete_callback,
    void* user_data,
    lse_thread_pool_user_data_finalize user_data_finalize);

void lse_env_cancel_thread_pool_task(lse_env* env, lse_thread_pool_task* task);

// helpers for classes using sdl
#define lse_get_sdl(INSTANCE) (&(INSTANCE)->env->sdl)
#define lse_get_sdl_from_base(INSTANCE) (&(INSTANCE)->base.env->sdl)
