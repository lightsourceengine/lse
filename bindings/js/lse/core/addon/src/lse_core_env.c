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

#include "lse_core.h"
#include "napix.h"
#include <lse_object.h>
#include <lse_style_meta.h>
#include <lse_types.h>
#include <stc/ccommon.h>

static napi_ref s_gamepad_status_ref = NULL;
static napi_ref s_gamepad_button_ref = NULL;
static napi_ref s_keyboard_button_ref = NULL;
static napi_ref s_gamepad_axis_motion_ref = NULL;
static napi_ref s_gamepad_hat_motion_ref = NULL;

static napi_value renderer_info_to_js(napi_env env, int32_t renderer_index, lse_renderer_info* info);
static napi_value display_mode_to_js(napi_env env, lse_display_mode* mode);
static void gamepad_status_callback(lse_gamepad* gamepad, bool connected, void* user_data);
static void
gamepad_button_callback(int32_t id, lse_keyspace keyspace, int32_t key, bool pressed, bool repeat, void* user_data);
static void keyboard_button_callback(lse_keyspace keyspace, int32_t key, bool pressed, bool repeat, void* user_data);
static void
gamepad_axis_motion_callback(int32_t id, lse_keyspace keyspace, int32_t axis, float axis_value, void* user_data);
static void gamepad_hat_motion_callback(int32_t id, lse_keyspace keyspace, int32_t hat, int32_t value, void* user_data);
static bool resolve_event_data_ref(napi_env env, napi_value callback, napi_ref* ref);
static int32_t get_instance_id(lse_object* device);
static void clear_event_refs(napi_env env);

JS_CALLBACK(new_instance) {
  JS_FUNCTION_SIG_NO_ARGS()
  lse_env* instance = lse_env_new();

  // TODO: check result
  lse_core_thread_pool_install(instance, env);

  return lse_core_class_new_with_object(env, lse_env_type, (lse_object*)instance);
}

JS_CALLBACK(configure) {
  JS_METHOD_SIG(lse_env, 1)

  lse_settings settings = lse_settings_init();
  napi_value mock = napix_get_own_property(env, argv[0], "mock");
  napi_value sdl = napix_get_own_property(env, argv[0], "sdl");
  napi_value sdl_mixer = napix_get_own_property(env, argv[0], "sdl_mixer");

  if (napix_type_of(env, mock) == napi_object) {
    settings.mock_settings.enabled = napix_obj_get_boolean(env, mock, "enabled", false);
  }

  if (napix_type_of(env, sdl) == napi_object) {
    settings.sdl_settings.enabled = napix_obj_get_boolean(env, sdl, "enabled", true);
  }

  if (napix_type_of(env, sdl_mixer) == napi_object) {
    settings.sdl_mixer_settings.enabled = napix_obj_get_boolean(env, sdl_mixer, "enabled", true);
  }

  lse_status status = lse_env_configure(self, &settings);

  LSE_CORE_CHECK_STATUS(env, status);

  return JS_UNDEFINED;
}

JS_CALLBACK(destroy) {
  JS_METHOD_SIG_NO_ARGS(lse_env)

  lse_env_destroy(self);

  return JS_UNDEFINED;
}

JS_CALLBACK(get_video_drivers) {
  JS_METHOD_SIG_NO_ARGS(lse_env)

  int32_t count = lse_env_get_video_driver_count(self);
  napi_value out = napix_create_array(env, count);
  napi_value string_value;

  for (int32_t i = 0; i < count; i++) {
    string_value = napix_create_string(env, lse_env_get_video_driver(self, i));

    if (!string_value) {
      return JS_UNDEFINED;
    }

    napi_set_element(env, out, i, string_value);
  }

  return out;
}

JS_CALLBACK(get_renderers) {
  JS_METHOD_SIG_NO_ARGS(lse_env)

  int32_t count = lse_env_get_video_driver_count(self);
  napi_value out = napix_create_array(env, count);
  lse_renderer_info renderer_info;
  napi_value renderer_info_value;

  for (int32_t i = 0; i < count; i++) {
    if (!lse_env_get_renderer_info(self, i, &renderer_info)) {
      return JS_UNDEFINED;
    }

    renderer_info_value = renderer_info_to_js(env, i, &renderer_info);

    if (!renderer_info_value) {
      return JS_UNDEFINED;
    }

    napi_set_element(env, out, i, renderer_info_value);
  }

  return out;
}

JS_CALLBACK(get_display_count) {
  JS_METHOD_SIG_NO_ARGS(lse_env)

  return napix_create_int32(env, lse_env_get_display_count(self));
}

JS_CALLBACK(get_display_name) {
  JS_METHOD_SIG(lse_env, 1)

  int32_t display_index = napix_unbox_i(env, argv[0], -1);

  return napix_create_string(env, lse_env_get_display_name(self, display_index));
}

JS_CALLBACK(find_closest_display_mode) {
  JS_METHOD_SIG(lse_env, 3)

  int32_t display_index = napix_unbox_i(env, argv[0], -1);
  int32_t desired_width = napix_unbox_i(env, argv[1], 0);
  int32_t desired_height = napix_unbox_i(env, argv[2], 0);
  lse_display_mode mode;

  if (!lse_env_find_closest_display_mode(self, display_index, desired_width, desired_height, &mode)) {
    return JS_UNDEFINED;
  }

  return display_mode_to_js(env, &mode);
}

JS_CALLBACK(get_current_display_mode) {
  JS_METHOD_SIG(lse_env, 1)

  int32_t display_index = napix_unbox_i(env, argv[0], -1);
  lse_display_mode mode;

  if (lse_env_get_current_display_mode(self, display_index, &mode)) {
    return display_mode_to_js(env, &mode);
  }

  return JS_UNDEFINED;
}

JS_CALLBACK(get_desktop_display_mode) {
  JS_METHOD_SIG(lse_env, 1)

  int32_t display_index = napix_unbox_i(env, argv[0], -1);
  lse_display_mode mode;

  if (lse_env_get_desktop_display_mode(self, display_index, &mode)) {
    return display_mode_to_js(env, &mode);
  }

  return JS_UNDEFINED;
}

JS_CALLBACK(get_display_modes) {
  JS_METHOD_SIG(lse_env, 1)

  int32_t display_index = napix_unbox_i(env, argv[0], -1);
  int32_t count = lse_env_get_display_mode_count(self, display_index);
  napi_value out = napix_create_array(env, count);
  napi_value mode_value;
  lse_display_mode mode;

  for (int32_t i = 0; i < count; i++) {
    if (!lse_env_get_display_mode(self, display_index, i, &mode)) {
      return JS_UNDEFINED;
    }

    mode_value = display_mode_to_js(env, &mode);

    if (!mode_value) {
      return JS_UNDEFINED;
    }

    napi_set_element(env, out, i, mode_value);
  }

  return out;
}

JS_CALLBACK(update) {
  JS_METHOD_SIG_NO_ARGS(lse_env)

  lse_env_update(self);

  return napix_get_boolean(env, !lse_env_has_quit_request(self));
}

JS_CALLBACK(add_window) {
  JS_METHOD_SIG_NO_ARGS(lse_env)
  return lse_core_class_new_with_object(env, lse_window_type, (lse_object*)lse_env_add_window(self));
}

JS_CALLBACK(remove_window) {
  JS_METHOD_SIG(lse_env, 1)

  lse_window* window = (lse_window*)lse_core_unwrap(env, argv[0]);
  bool result;

  if (window) {
    result = lse_env_remove_window(self, window);
  } else {
    result = false;
  }

  return napix_get_boolean(env, result);
}

JS_CALLBACK(register_event_callbacks) {
  JS_METHOD_SIG(lse_env, 1)

  napi_value callback_map = argv[0];
  napi_value callback;

  if ((callback = napix_get_own_property(env, callback_map, "gamepad_status"))) {
    if (!resolve_event_data_ref(env, callback, &s_gamepad_status_ref)) {
      return JS_UNDEFINED;
    }

    lse_env_set_gamepad_status_callback(self, &gamepad_status_callback, env);
  }

  if ((callback = napix_get_own_property(env, callback_map, "gamepad_button"))) {
    if (!resolve_event_data_ref(env, callback, &s_gamepad_button_ref)) {
      return JS_UNDEFINED;
    }

    lse_env_set_gamepad_button_callback(self, &gamepad_button_callback, env);
  }

  if ((callback = napix_get_own_property(env, callback_map, "keyboard_button"))) {
    if (!resolve_event_data_ref(env, callback, &s_keyboard_button_ref)) {
      return JS_UNDEFINED;
    }

    lse_env_set_keyboard_button_callback(self, &keyboard_button_callback, env);
  }

  if ((callback = napix_get_own_property(env, callback_map, "gamepad_hat_motion"))) {
    if (!resolve_event_data_ref(env, callback, &s_gamepad_hat_motion_ref)) {
      return JS_UNDEFINED;
    }

    lse_env_set_gamepad_hat_motion_callback(self, &gamepad_hat_motion_callback, env);
  }

  if ((callback = napix_get_own_property(env, callback_map, "gamepad_axis_motion"))) {
    if (!resolve_event_data_ref(env, callback, &s_gamepad_axis_motion_ref)) {
      return JS_UNDEFINED;
    }

    lse_env_set_gamepad_axis_motion_callback(self, &gamepad_axis_motion_callback, env);
  }

  return JS_UNDEFINED;
}

JS_CALLBACK(reset_event_callbacks) {
  JS_METHOD_SIG_NO_ARGS(lse_env)

  lse_env_reset_event_callbacks(self);
  clear_event_refs(env);

  return JS_UNDEFINED;
}

JS_CALLBACK(get_keyboard) {
  JS_METHOD_SIG_NO_ARGS(lse_env)
  lse_keyboard* keyboard = lse_env_get_keyboard(self);

  lse_ref(keyboard);

  return lse_core_class_new_with_object(env, lse_keyboard_type, (lse_object*)keyboard);
}

JS_CALLBACK(get_gamepads) {
  JS_METHOD_SIG_NO_ARGS(lse_env)
  size_t i;
  size_t count = lse_env_get_gamepad_count(self);
  napi_value result = napix_create_array(env, count);
  lse_gamepad* gamepad;

  for (i = 0; i < count; i++) {
    gamepad = lse_env_get_gamepad(self, i);

    lse_ref(gamepad);
    // TODO: check gamepad

    napi_set_element(env, result, i, lse_core_class_new_with_object(env, lse_gamepad_type, (lse_object*)gamepad));
  }

  return result;
}

JS_CALLBACK(get_mapping) {
  JS_METHOD_SIG(lse_env, 1)
  lse_string* uuid = lse_core_unbox_s(env, argv[0]);
  lse_string* mapping;

  if (uuid) {
    mapping = lse_env_get_mapping(self, lse_string_as_cstring(uuid));
    lse_unref(uuid);
  } else {
    mapping = NULL;
  }

  return lse_core_box_s(env, mapping);
}

JS_CALLBACK(set_mapping) {
  JS_METHOD_SIG(lse_env, 2)
  lse_string* uuid = lse_core_unbox_s(env, argv[0]);
  lse_string* mapping = lse_core_unbox_s(env, argv[1]);
  bool result;

  if (uuid && mapping) {
    result = lse_env_set_mapping(self, lse_string_as_cstring(uuid), mapping);
    lse_unref(uuid);
  } else {
    result = false;
  }

  return napix_get_boolean(env, result);
}

JS_CALLBACK(reset_mapping) {
  JS_METHOD_SIG(lse_env, 1)
  lse_string* uuid = lse_core_unbox_s(env, argv[0]);
  bool result;

  if (uuid) {
    result = lse_env_reset_mapping(self, lse_string_as_cstring(uuid));
    lse_unref(uuid);
  } else {
    result = false;
  }

  return napix_get_boolean(env, result);
}

JS_CALLBACK(add_font) {
  JS_METHOD_SIG(lse_env, 5)

  lse_status status;
  lse_font* font;
  lse_string* uri = lse_core_unbox_s(env, argv[0]);
  int32_t index = napix_unbox_i(env, argv[1], 0);
  lse_string* family = lse_core_unbox_s(env, argv[2]);
  lse_string* style_value = lse_core_unbox_s(env, argv[3]);
  lse_string* weight_value = lse_core_unbox_s(env, argv[4]);
  int32_t style;
  int32_t weight;

  assert(uri != NULL);
  assert(family != NULL);

  LSE_CORE_CHECK_EXPR(env, (uri != NULL && family != NULL), LSE_ERR_ILLEGAL_ARGUMENT);

  if (index < 0) {
    index = 0;
  }

  style = style_value ? lse_style_meta_enum_from_string(LSE_SP_FONT_STYLE, lse_string_as_cstring(style_value)) : -1;

  if (style < 0) {
    style = LSE_STYLE_FONT_STYLE_NORMAL;
  }

  weight = weight_value ? lse_style_meta_enum_from_string(LSE_SP_FONT_WEIGHT, lse_string_as_cstring(weight_value)) : -1;

  if (weight < 0) {
    weight = LSE_STYLE_FONT_WEIGHT_NORMAL;
  }

  status = lse_env_add_font(self, uri, index, family, style, weight);

  LSE_CORE_CHECK_STATUS(env, status);

  // TODO: should acquire here.. need to address font store life cycle vs js object life cycle
  font = lse_env_get_font(self, lse_string_as_cstring(family), style, weight);

  LSE_CORE_CHECK_EXPR(env, font != NULL, LSE_ERR_RES_FONT_UNAVAILABLE);
  lse_ref(font);

  return lse_core_class_new_with_object(env, lse_font_type, (lse_object*)font);
}

JS_CALLBACK(is_configured) {
  JS_METHOD_SIG_NO_ARGS(lse_env)
  return napix_get_boolean(env, lse_env_is_configured(self));
}

JS_CALLBACK(is_running) {
  JS_METHOD_SIG_NO_ARGS(lse_env)
  return napix_get_boolean(env, lse_env_is_running(self));
}

JS_CALLBACK(is_error) {
  JS_METHOD_SIG_NO_ARGS(lse_env)
  return napix_get_boolean(env, lse_env_is_error(self));
}

JS_CALLBACK(is_destroyed) {
  JS_METHOD_SIG_NO_ARGS(lse_env)
  return napix_get_boolean(env, lse_env_is_destroyed(self));
}

void lse_core_env_cleanup_hook(void* js_env) {
  clear_event_refs((napi_env)js_env);
}

void lse_core_env_init(napi_env env, napi_value exports) {
  lse_namespace ns = lse_namespace_init(env, exports, JS_NAMESPACE_ENV);

  lse_add_function(&ns, JS_ENV_NEW_INSTANCE, &new_instance);
  lse_add_function(&ns, JS_ENV_CONFIGURE, &configure);
  lse_add_function(&ns, JS_ENV_DESTROY, &destroy);
  lse_add_function(&ns, JS_ENV_UPDATE, &update);
  lse_add_function(&ns, JS_ENV_ADD_WINDOW, &add_window);
  lse_add_function(&ns, JS_ENV_REMOVE_WINDOW, &remove_window);
  lse_add_function(&ns, JS_ENV_GET_DISPLAY_NAME, &get_display_name);
  lse_add_function(&ns, JS_ENV_GET_CURRENT_DISPLAY_MODE, &get_current_display_mode);
  lse_add_function(&ns, JS_ENV_GET_DESKTOP_DISPLAY_MODE, &get_desktop_display_mode);
  lse_add_function(&ns, JS_ENV_GET_DISPLAY_COUNT, &get_display_count);
  lse_add_function(&ns, JS_ENV_GET_VIDEO_DRIVERS, &get_video_drivers);
  lse_add_function(&ns, JS_ENV_GET_RENDERERS, &get_renderers);
  lse_add_function(&ns, JS_ENV_GET_DISPLAY_MODES, &get_display_modes);
  lse_add_function(&ns, JS_ENV_GET_FIND_CLOSEST_DISPLAY_MODE, &find_closest_display_mode);
  lse_add_function(&ns, JS_ENV_REGISTER_EVENT_CALLBACKS, &register_event_callbacks);
  lse_add_function(&ns, JS_ENV_RESET_EVENT_CALLBACKS, &reset_event_callbacks);
  lse_add_function(&ns, JS_ENV_GET_KEYBOARD, &get_keyboard);
  lse_add_function(&ns, JS_ENV_GET_GAMEPADS, &get_gamepads);
  lse_add_function(&ns, JS_ENV_GET_MAPPING, &get_mapping);
  lse_add_function(&ns, JS_ENV_SET_MAPPING, &set_mapping);
  lse_add_function(&ns, JS_ENV_RESET_MAPPING, &reset_mapping);
  lse_add_function(&ns, JS_ENV_ADD_FONT, &add_font);
  lse_add_function(&ns, JS_ENV_IS_CONFIGURED, &is_configured);
  lse_add_function(&ns, JS_ENV_IS_RUNNING, &is_running);
  lse_add_function(&ns, JS_ENV_IS_DESTROYED, &is_destroyed);
  lse_add_function(&ns, JS_ENV_IS_ERROR, &is_error);
}

static napi_value display_mode_to_js(napi_env env, lse_display_mode* mode) {
  napi_value args[] = {
    napix_create_int32(env, mode->width),
    napix_create_int32(env, mode->height),
    napix_create_int32(env, mode->refresh_rate),
  };

  return lse_core_class_new_with_args(env, lse_display_mode_type, args, c_arraylen(args));
}

static napi_value renderer_info_to_js(napi_env env, int32_t renderer_index, lse_renderer_info* info) {
  napi_value args[] = {
    napix_create_int32(env, renderer_index),
    napix_create_string(env, info->name),
    napix_get_boolean(env, info->has_hardware_acceleration),
    napix_get_boolean(env, info->has_vsync),
    napix_get_boolean(env, info->has_render_to_texture),
    napix_create_int32(env, info->max_texture_width),
    napix_create_int32(env, info->max_texture_height),
  };

  return lse_core_class_new_with_args(env, lse_renderer_info_type, args, c_arraylen(args));
}

static void gamepad_status_callback(lse_gamepad* gamepad, bool connected, void* user_data) {
  // TODO: why?
  lse_ref(gamepad);

  napi_env env = user_data;
  napi_value result;
  napi_value args[] = {
    lse_core_class_new_with_object(env, lse_gamepad_type, (lse_object*)gamepad),
    napix_get_boolean(env, connected),
  };

  napix_call_ref_function(env, s_gamepad_status_ref, args, c_arraylen(args), &result);
  // TODO: throw error on failure?
}

static void
gamepad_button_callback(int32_t id, lse_keyspace keyspace, int32_t key, bool pressed, bool repeat, void* user_data) {
  napi_env env = user_data;
  napi_value result;
  // clang-format off
  napi_value args[] = {
    napix_create_int32(env, id),
    napix_create_int32(env, keyspace),
    napix_create_int32(env, key),
    napix_get_boolean(env, pressed),
    napix_get_boolean(env, repeat),
  };
  // clang-format on

  napix_call_ref_function(env, s_gamepad_button_ref, args, c_arraylen(args), &result);
  // TODO: throw error on failure?
}

static void keyboard_button_callback(lse_keyspace keyspace, int32_t key, bool pressed, bool repeat, void* user_data) {
  napi_env env = user_data;
  napi_value result;
  napi_value args[] = {
    napix_create_int32(env, keyspace),
    napix_create_int32(env, key),
    napix_get_boolean(env, pressed),
    napix_get_boolean(env, repeat),
  };

  napix_call_ref_function(env, s_keyboard_button_ref, args, c_arraylen(args), &result);
  // TODO: throw error on failure?
}

static void
gamepad_axis_motion_callback(int32_t id, lse_keyspace keyspace, int32_t axis, float axis_value, void* user_data) {
  napi_env env = user_data;
  napi_value result;
  napi_value args[] = {
    napix_create_int32(env, id),
    napix_create_int32(env, keyspace),
    napix_create_int32(env, axis),
    napix_create_float(env, axis_value),
  };

  napix_call_ref_function(env, s_gamepad_axis_motion_ref, args, c_arraylen(args), &result);
  // TODO: throw error on failure?
}

static void
gamepad_hat_motion_callback(int32_t id, lse_keyspace keyspace, int32_t hat, int32_t value, void* user_data) {
  napi_env env = user_data;
  napi_value result;
  napi_value args[] = {
    napix_create_int32(env, id),
    napix_create_int32(env, keyspace),
    napix_create_int32(env, hat),
    napix_create_int32(env, value),
  };

  napix_call_ref_function(env, s_gamepad_hat_motion_ref, args, c_arraylen(args), &result);
  // TODO: throw error on failure?
}

static bool resolve_event_data_ref(napi_env env, napi_value callback, napi_ref* ref) {
  if (napix_type_of(env, callback) != napi_function) {
    lse_core_throw_error(env, LSE_ERR_JS_EXPECTED_FUNCTION);
    return false;
  }

  if (*ref) {
    napi_delete_reference(env, *ref);
    *ref = NULL;
  }

  if (napi_create_reference(env, callback, 1, ref) != napi_ok) {
    lse_core_throw_error(env, LSE_ERR_JS_REF);
    return false;
  }

  return true;
}

static int32_t get_instance_id(lse_object* device) {
  switch (lse_object_get_interface_type(device)) {
    case lse_keyboard_interface_type:
      return lse_keyboard_get_id((lse_keyboard*)device);
    case lse_gamepad_interface_type:
      return lse_gamepad_get_id((lse_gamepad*)device);
    default:
      return -1;
  }
}

static void clear_event_refs(napi_env env) {
  // clang-format off
  napi_ref* refs[] = {
    &s_gamepad_status_ref,
    &s_gamepad_button_ref,
    &s_gamepad_axis_motion_ref,
    &s_gamepad_hat_motion_ref,
    &s_keyboard_button_ref,
  };
  // clang-format on

  for (size_t i = 0; i < c_arraylen(refs); i++) {
    if (*refs[i] != NULL) {
      napi_delete_reference(env, *refs[i]);
      *refs[i] = NULL;
    }
  }
}
