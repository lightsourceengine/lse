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

#include "lse_env.h"

#include "lse_font_store.h"
#include "lse_gamepad.h"
#include "lse_graphics.h"
#include "lse_keyboard.h"
#include "lse_object.h"
#include "lse_string.h"
#include "lse_video.h"
#include "lse_window.h"

#define i_tag gamepads
#define i_val lse_gamepad_ptr
#define i_valdrop cvec_gamepads_value_drop
#define i_opt c_no_clone | c_no_cmp | c_is_fwd
#include "stc/cvec.h"

#define i_tag windows
#define i_val lse_window_ptr
#define i_valdrop cvec_windows_value_drop
#define i_opt c_no_clone | c_no_cmp | c_is_fwd
#include "stc/cvec.h"

#define i_tag mappings
#define i_key lse_string_ptr
#define i_keyraw crawstr
#define i_keyto lse_string_keyto
#define i_keyfrom assert
#define i_hash crawstr_hash
#define i_cmp crawstr_cmp
#define i_eq crawstr_eq
#define i_val cmap_mappings_entry
#define i_valdrop cmap_mappings_entry_drop
#define i_opt c_no_clone | c_no_cmp | c_is_fwd
#include "stc/cmap.h"

static void default_on_gamepad_status(lse_gamepad* gamepad, bool connected, void* user_data);
static void
default_on_gamepad_button(int32_t id, lse_keyspace keyspace, int32_t key, bool pressed, bool repeat, void* user_data);
static void
default_on_gamepad_axis_motion(int32_t id, lse_keyspace keyspace, int32_t axis, float axis_value, void* user_data);
static void
default_on_gamepad_hat_motion(int32_t id, lse_keyspace keyspace, int32_t hat, int32_t value, void* user_data);
static void default_on_keyboard_button(lse_keyspace keyspace, int32_t key, bool pressed, bool repeat, void* user_data);
static lse_gamepad* find_gamepad_by_instance_id(lse_env* env, int32_t instance_id);
static void lse_env_load_mappings_sync(lse_env* env);
static void update_gamepad_mapping(lse_env* env, const char* uuid);

static void constructor(lse_object* object, void* arg) {
  lse_env* env = (lse_env*)object;

  env->state = LSE_ENV_STATE_INIT;
  env->gamepads = cvec_gamepads_init();
  env->windows = cvec_windows_init();
  env->mappings = cmap_mappings_init();
  env->fonts = lse_new(lse_font_store, env);

  lse_env_reset_event_callbacks(env);
}

static void destructor(lse_object* object) {
  lse_env* env = (lse_env*)object;

  assert(lse_env_is_destroyed(env));

  lse_unref(env->video);
  lse_unref(env->keyboard);
  lse_unref(env->fonts);
  cvec_gamepads_drop(&env->gamepads);
  cvec_windows_drop(&env->windows);
  cmap_mappings_drop(&env->mappings);
}

LSE_API lse_env* LSE_CDECL lse_env_new() {
  return lse_new(lse_env, NULL);
}

LSE_API lse_status LSE_CDECL lse_env_configure(lse_env* env, const lse_settings* settings) {
  if (lse_env_is_destroyed(env) || lse_env_is_configured(env)) {
    return LSE_ERR_GENERIC;
  }

  lse_status status;
  lse_video* video = NULL;
  uint8_t video_type;
  size_t i;
  size_t count;
  lse_settings default_settings;
  lse_gamepad* gamepad;

  //
  // load default settings, if necessary
  //

  if (!settings) {
    default_settings = lse_settings_init();
    settings = &default_settings;
  }

  // TODO: add validation when more settings are added

  //
  // select video backend from settings
  //

  if (settings->mock_settings.enabled) {
    video_type = lse_mock_video_type;
  } else if (settings->sdl_settings.enabled) {
    status = lse_sdl_load(&env->sdl);

    if (status != LSE_OK) {
      goto INIT_ERR;
    }

    video_type = lse_sdl_video_type;
  } else {
    status = LSE_ERR_GENERIC;
    goto INIT_ERR;
  }

  video = (lse_video*)lse_object_new(video_type, env);

  if (!video) {
    status = LSE_ERR_OUT_OF_MEMORY;
    goto INIT_ERR;
  }

  status = lse_video_attach(video);

  if (status != LSE_OK) {
    goto INIT_ERR;
  }

  //
  // load gamepads and mappings
  //

  lse_env_load_mappings_sync(env);

  // Note: this list would get automatically get populated by joystick added events. across SDL versions, I have not
  // found that added events are not reliably dispatched at start up. so, get all the gamepads here. this also has
  // the benefit of having gamepads available immediately after init().
  count = lse_video_get_gamepad_count(video);

  for (i = 0; i < count; i++) {
    gamepad = lse_video_create_gamepad(video, (int32_t)i);

    if (gamepad) {
      cvec_gamepads_push_back(&env->gamepads, gamepad);
    } else {
      LSE_LOG_ERROR("Cannot create gamepad at index %i", (int32_t)i);
    }
  }

  //
  // load keyboard (ok if system does not have one)
  //

  env->keyboard = lse_video_create_keyboard(video);

  if (!env->keyboard) {
    status = LSE_ERR_OUT_OF_MEMORY;
    goto INIT_ERR;
  }

  //
  // enable the environment
  //

  env->video = video;
  env->state = LSE_ENV_STATE_RUNNING;

  return LSE_OK;

  //
  // error handling
  //

INIT_ERR:

  cvec_gamepads_clear(&env->gamepads);

  if (video) {
    lse_video_destroy(video);
    lse_object_unref((lse_object*)video);
  }

  env->state = LSE_ENV_STATE_ERROR;

  return status;
}

LSE_API void LSE_CDECL lse_env_destroy(lse_env* env) {
  if (lse_env_is_destroyed(env)) {
    return;
  }

  lse_font_store_destroy(env->fonts);
  cvec_windows_clear(&env->windows);
  cvec_gamepads_clear(&env->gamepads);

  if (env->keyboard) {
    lse_keyboard_destroy(env->keyboard);
  }

  if (env->video) {
    lse_video_destroy(env->video);
  }

  if (env->pool) {
    env->pool_free(env->pool);
    env->pool = NULL;
  }

  lse_sdl_unload(&env->sdl);

  env->state = LSE_ENV_STATE_DONE;
}

LSE_API bool LSE_CDECL lse_env_is_configured(lse_env* env) {
  return env->state == LSE_ENV_STATE_RUNNING || env->state == LSE_ENV_STATE_PAUSED;
}

LSE_API bool LSE_CDECL lse_env_is_running(lse_env* env) {
  return env->state == LSE_ENV_STATE_RUNNING;
}

LSE_API bool LSE_CDECL lse_env_is_destroyed(lse_env* env) {
  return !env || env->state == LSE_ENV_STATE_DONE;
}

LSE_API bool LSE_CDECL lse_env_is_error(lse_env* env) {
  return env->state == LSE_ENV_STATE_ERROR;
}

LSE_API bool LSE_CDECL lse_env_has_quit_request(lse_env* env) {
  return env->was_quit_requested;
}

LSE_API void LSE_CDECL
lse_env_set_gamepad_status_callback(lse_env* env, lse_gamepad_status_callback callback, void* data) {
  env->on_gamepad_status = callback;
  env->on_gamepad_status_data = data;
}

LSE_API void LSE_CDECL
lse_env_set_gamepad_button_callback(lse_env* env, lse_gamepad_button_callback callback, void* data) {
  env->on_gamepad_button = callback;
  env->on_gamepad_button_data = data;
}

LSE_API void LSE_CDECL
lse_env_set_gamepad_axis_motion_callback(lse_env* env, lse_gamepad_axis_motion_callback callback, void* data) {
  env->on_gamepad_axis_motion = callback;
  env->on_gamepad_axis_motion_data = data;
}

LSE_API void LSE_CDECL
lse_env_set_gamepad_hat_motion_callback(lse_env* env, lse_gamepad_hat_motion_callback callback, void* data) {
  env->on_gamepad_hat_motion = callback;
  env->on_gamepad_hat_motion_data = data;
}

LSE_API void LSE_CDECL
lse_env_set_keyboard_button_callback(lse_env* env, lse_keyboard_button_callback callback, void* data) {
  env->on_keyboard_button = callback;
  env->on_keyboard_button_data = data;
}

LSE_API void LSE_CDECL lse_env_reset_event_callbacks(lse_env* env) {
  env->on_gamepad_status = &default_on_gamepad_status;
  env->on_gamepad_status_data = NULL;
  env->on_gamepad_button = &default_on_gamepad_button;
  env->on_gamepad_button_data = NULL;
  env->on_gamepad_hat_motion = &default_on_gamepad_hat_motion;
  env->on_gamepad_hat_motion_data = NULL;
  env->on_gamepad_axis_motion = &default_on_gamepad_axis_motion;
  env->on_gamepad_axis_motion_data = NULL;
  env->on_keyboard_button = &default_on_keyboard_button;
  env->on_keyboard_button_data = NULL;
}

LSE_API void LSE_CDECL lse_env_update(lse_env* env) {
  // TODO: check running?
  if (!lse_video_process_events(env->video)) {
    env->was_quit_requested = true;
    return;
  }

  c_foreach(it, cvec_windows, env->windows) {
    lse_window_present(*it.ref);
  }
}

LSE_API const char* LSE_CDECL lse_env_get_video_driver(lse_env* env, int32_t driver_index) {
  if (!lse_env_is_configured(env)) {
    return "";
  }

  return lse_video_get_driver(env->video, driver_index);
}

LSE_API int32_t LSE_CDECL lse_env_get_video_driver_count(lse_env* env) {
  if (!lse_env_is_configured(env)) {
    return 0;
  }

  return lse_video_get_driver_count(env->video);
}

LSE_API const char* LSE_CDECL lse_env_get_display_name(lse_env* env, int32_t display_index) {
  if (!lse_env_is_configured(env)) {
    return "";
  }

  return lse_video_get_display_name(env->video, display_index);
}

LSE_API bool LSE_CDECL lse_env_get_current_display_mode(lse_env* env, int32_t display_index, lse_display_mode* mode) {
  if (!lse_env_is_configured(env)) {
    return false;
  }

  return lse_video_get_current_display_mode(env->video, display_index, mode);
}

LSE_API bool LSE_CDECL lse_env_get_desktop_display_mode(lse_env* env, int32_t display_index, lse_display_mode* mode) {
  if (!lse_env_is_configured(env)) {
    return false;
  }

  return lse_video_get_desktop_display_mode(env->video, display_index, mode);
}

LSE_API int32_t LSE_CDECL lse_env_get_display_count(lse_env* env) {
  if (!lse_env_is_configured(env)) {
    return 0;
  }

  return lse_video_get_display_count(env->video);
}

LSE_API bool LSE_CDECL
lse_env_get_display_mode(lse_env* env, int32_t display_index, int32_t display_mode_index, lse_display_mode* out) {
  if (!lse_env_is_configured(env)) {
    return false;
  }

  return lse_video_get_display_mode(env->video, display_index, display_mode_index, out);
}

LSE_API int32_t LSE_CDECL lse_env_get_display_mode_count(lse_env* env, int32_t display_index) {
  if (!lse_env_is_configured(env)) {
    return 0;
  }

  return lse_video_get_display_mode_count(env->video, display_index);
}

LSE_API int32_t LSE_CDECL lse_env_get_renderer_count(lse_env* env) {
  if (!lse_env_is_configured(env)) {
    return 0;
  }

  return lse_video_get_renderer_count(env->video);
}

LSE_API bool LSE_CDECL lse_env_get_renderer_info(lse_env* env, int32_t renderer_index, lse_renderer_info* info) {
  if (!lse_env_is_configured(env)) {
    return false;
  }

  return lse_video_get_renderer_info(env->video, renderer_index, info);
}

LSE_API bool LSE_CDECL lse_env_find_closest_display_mode(
    lse_env* env,
    int32_t display_index,
    int32_t desired_width,
    int32_t desired_height,
    lse_display_mode* out) {
  if (!lse_env_is_configured(env)) {
    return false;
  }

  return lse_video_find_closest_display_mode(env->video, display_index, desired_width, desired_height, out);
}

LSE_API lse_window* LSE_CDECL lse_env_add_window(lse_env* env) {
  if (lse_env_is_destroyed(env)) {
    return NULL;
  }

  lse_window* window = lse_new(lse_window, env);

  cvec_windows_push_back(&env->windows, window);

  // NOTE: add ref for caller
  lse_ref(window);

  return window;
}

LSE_API bool LSE_CDECL lse_env_remove_window(lse_env* env, lse_window* window) {
  c_foreach(it, cvec_windows, env->windows) {
    if (window == *it.ref) {
      cvec_windows_erase_at(&env->windows, it);
      return true;
    }
  }

  return false;
}

LSE_API lse_window* LSE_CDECL lse_env_get_window(lse_env* env, size_t index) {
  if (index >= cvec_rep_(&env->windows)->size) {
    return NULL;
  }

  return (lse_window*)*cvec_windows_at(&env->windows, index);
}

LSE_API size_t LSE_CDECL lse_env_get_window_count(lse_env* env) {
  return cvec_rep_(&env->windows)->size;
}

LSE_API lse_keyboard* LSE_CDECL lse_env_get_keyboard(lse_env* env) {
  return env->keyboard;
}

LSE_API lse_gamepad* LSE_CDECL lse_env_get_gamepad(lse_env* env, size_t index) {
  if (index < cvec_rep_(&env->gamepads)->size) {
    return *cvec_gamepads_at(&env->gamepads, index);
  }

  return NULL;
}

LSE_API size_t LSE_CDECL lse_env_get_gamepad_count(lse_env* env) {
  return cvec_rep_(&env->gamepads)->size;
}

LSE_API lse_string* LSE_CDECL lse_env_get_mapping(lse_env* env, const char* uuid) {
  cmap_mappings_iter i = cmap_mappings_find(&env->mappings, uuid);

  if (i.ref != cmap_mappings_end(&env->mappings).ref) {
    return i.ref->second.user_mapping;
  }

  return NULL;
}

LSE_API bool LSE_CDECL lse_env_set_mapping(lse_env* env, const char* uuid, lse_string* mapping) {
  if (!lse_env_is_configured(env)) {
    return false;
  }

  lse_video* video = env->video;
  cmap_mappings_iter i = cmap_mappings_find(&env->mappings, uuid);
  cmap_mappings_entry* entry;
  lse_string* uuid_string;
  lse_string* intrinsic;

  if (i.ref != cmap_mappings_end(&env->mappings).ref) {
    lse_video_set_mapping(video, uuid, mapping);

    entry = &i.ref->second;
    lse_unref(entry->user_mapping);
    entry->user_mapping = mapping;
  } else {
    intrinsic = lse_video_get_mapping(video, uuid);
    lse_video_set_mapping(video, uuid, mapping);
    uuid_string = lse_string_new(uuid);

    cmap_mappings_insert(
        &env->mappings,
        uuid_string,
        (cmap_mappings_entry){ .uuid = uuid_string, .user_mapping = mapping, .intrinsic_mapping = intrinsic });
  }

  update_gamepad_mapping(env, uuid);

  return true;
}

LSE_API bool LSE_CDECL lse_env_reset_mapping(lse_env* env, const char* uuid) {
  if (!lse_env_is_configured(env)) {
    return false;
  }

  lse_video* video = env->video;
  cmap_mappings_iter i = cmap_mappings_find(&env->mappings, uuid);
  cmap_mappings_entry* entry;
  lse_string* intrinsic;

  if (i.ref == cmap_mappings_end(&env->mappings).ref) {
    return false;
  }

  entry = &i.ref->second;
  intrinsic = entry->intrinsic_mapping;

  if (!entry->intrinsic_mapping) {
    intrinsic = lse_video_create_stub_mapping(video, uuid);
  }

  assert(intrinsic);

  if (intrinsic) {
    lse_video_set_mapping(video, uuid, intrinsic);
  }

  cmap_mappings_erase_at(&env->mappings, i);

  // TODO: is it necessary to upgrade to gamecontroller on this path?
  // update_gamepad_mapping(env, uuid);

  return true;
}

LSE_API bool LSE_CDECL lse_env_set_thread_pool(
    lse_env* env,
    lse_thread_pool* context,
    lse_thread_pool_queue queue,
    lse_thread_pool_cancel cancel,
    lse_thread_pool_free thread_pool_free) {
  if (lse_env_is_destroyed(env)) {
    return false;
  }

  if (env->pool) {
    env->pool_free(env->pool);
  }

  env->pool = context;
  env->pool_queue = queue;
  env->pool_cancel = cancel;
  env->pool_free = thread_pool_free;

  return true;
}

LSE_API lse_status LSE_CDECL lse_env_add_font(
    lse_env* env,
    lse_string* uri,
    int32_t index,
    lse_string* family,
    lse_style_font_style style,
    lse_style_font_weight weight) {
  if (lse_env_is_destroyed(env)) {
    return LSE_ERR_EOL;
  }
  return lse_font_store_add_font(env->fonts, uri, index, family, style, weight, LSE_FONT_STORE_ASYNC);
}

LSE_API lse_font* LSE_CDECL
lse_env_acquire_font(lse_env* env, const char* family, lse_style_font_style style, lse_style_font_weight weight) {
  if (lse_env_is_destroyed(env)) {
    return NULL;
  }
  return lse_font_store_acquire_font(env->fonts, family, style, weight);
}

LSE_API lse_font* LSE_CDECL
lse_env_get_font(lse_env* env, const char* family, lse_style_font_style style, lse_style_font_weight weight) {
  if (lse_env_is_destroyed(env)) {
    return NULL;
  }
  return lse_font_store_get_font(env->fonts, family, style, weight);
}

LSE_API void LSE_CDECL lse_env_release_font(lse_env* env, lse_font* font) {
  if (lse_env_is_destroyed(env)) {
    return;
  }

  lse_font_store_release_font(env->fonts, font);
}

void lse_env_on_gamepad_connected(lse_env* env, int32_t index) {
  int32_t instance_id = lse_video_get_gamepad_instance_id_from_index(env->video, index);
  lse_gamepad* gamepad;

  if (instance_id < 0) {
    LSE_LOG_ERROR("no id for index = %i", index);
    return;
  }

  gamepad = find_gamepad_by_instance_id(env, instance_id);

  if (gamepad) {
    // on SDL init, the gamepads lists is created before SDL events fire. SDL may fire connected events for
    // already created/registered gamepads. so, if the gamepad already exists in env, this is a safe no-op.
    return;
  }

  gamepad = lse_video_create_gamepad(env->video, index);

  if (!gamepad) {
    LSE_LOG_ERROR("cannot connect gamepad");
    return;
  }

  if (!lse_gamepad_is_connected(gamepad)) {
    lse_unref(gamepad);
    LSE_LOG_ERROR("gamepad not connected");
    return;
  }

  cvec_gamepads_push_back(&env->gamepads, gamepad);

  env->on_gamepad_status(gamepad, true, env->on_gamepad_status_data);
}

void lse_env_on_gamepad_disconnected(lse_env* env, int32_t instance_id) {
  lse_gamepad* gamepad;

  c_foreach(i, cvec_gamepads, env->gamepads) {
    gamepad = *i.ref;

    if (lse_gamepad_get_id(gamepad) == instance_id) {
      cvec_gamepads_erase_at(&env->gamepads, i);
      // TODO: destroy may need to preserve some gamepad information, as the user may want to know which gamepad was
      // disconnected
      env->on_gamepad_status(gamepad, false, env->on_gamepad_status_data);
      return;
    }
  }

  LSE_LOG_ERROR("no gamepad for id = %i", instance_id);
}

// if loading a gamecontroller db is exposed publicly, gamepads and mappings will need to be updated.
static void lse_env_load_mappings_sync(lse_env* env) {
  int32_t result;
  char* filename = getenv(VAR_LSE_GAMECONTROLLER_DB);

  if (!filename || filename[0] == '\0') {
    return;
  }

  result = lse_video_load_mappings_from_file(env->video, filename);

  if (result < 0) {
    LSE_LOG_ERROR("not a mapping file %s", filename);
  } else {
    LSE_LOG_INFO("found %i mappings in %s", result, filename);
  }
}

static void update_gamepad_mapping(lse_env* env, const char* uuid) {
  lse_video* video = env->video;
  int32_t count = lse_video_get_gamepad_count(video);
  int32_t instance_id;
  lse_gamepad* gamepad;

  for (int32_t i = 0; i < count; i++) {
    gamepad = NULL;
    instance_id = lse_video_get_gamepad_instance_id_from_index(video, i);

    c_foreach(iter, cvec_gamepads, env->gamepads) {
      gamepad = *iter.ref;

      if (lse_gamepad_get_id((lse_gamepad*)*iter.ref) == instance_id &&
          c_strncasecmp(uuid, lse_gamepad_get_uuid(gamepad), 33) == 0) {
        lse_gamepad_enable_mapping(gamepad, i);
        break;
      }
    }
  }
}

static void default_on_gamepad_status(lse_gamepad* gamepad, bool connected, void* user_data) {
}

static void
default_on_gamepad_button(int32_t id, lse_keyspace keyspace, int32_t key, bool pressed, bool repeat, void* user_data) {
}

static void
default_on_gamepad_axis_motion(int32_t id, lse_keyspace keyspace, int32_t axis, float axis_value, void* user_data) {
}

static void
default_on_gamepad_hat_motion(int32_t id, lse_keyspace keyspace, int32_t hat, int32_t value, void* user_data) {
}

static void default_on_keyboard_button(lse_keyspace keyspace, int32_t key, bool pressed, bool repeat, void* user_data) {
}

static lse_gamepad* find_gamepad_by_instance_id(lse_env* env, int32_t instance_id) {
  lse_gamepad* gamepad;

  c_foreach(i, cvec_gamepads, env->gamepads) {
    gamepad = *i.ref;

    if (lse_gamepad_get_id(gamepad) == instance_id) {
      return gamepad;
    }
  }

  return NULL;
}

void cmap_mappings_entry_drop(cmap_mappings_entry* entry) {
  lse_unref(entry->uuid);
  lse_unref(entry->intrinsic_mapping);
  lse_unref(entry->user_mapping);
}

void cvec_windows_value_drop(lse_window_ptr* window) {
  lse_window_destroy(*window);
  lse_unref(*window);
}

void cvec_gamepads_value_drop(lse_gamepad_ptr* gamepad) {
  lse_gamepad_destroy(*gamepad);
  lse_unref(*gamepad);
}

lse_thread_pool_task* lse_env_add_thread_pool_task(
    lse_env* env,
    const char* resource_name,
    lse_thread_pool_work_callback work_callback,
    lse_thread_pool_complete_callback complete_callback,
    void* user_data,
    lse_thread_pool_user_data_finalize user_data_finalize) {
  return env->pool_queue(env->pool, resource_name, work_callback, complete_callback, user_data, user_data_finalize);
}

void lse_env_cancel_thread_pool_task(lse_env* env, lse_thread_pool_task* task) {
  if (env->pool_cancel) {
    env->pool_cancel(env->pool, task);
  }
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_TYPE_INFO(lse_env_type, lse_env, lse_none_interface_type, NULL);
