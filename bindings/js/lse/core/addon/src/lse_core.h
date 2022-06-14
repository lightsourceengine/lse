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

#include <lse.h>
#include <node_api.h>
#include <stc/ccommon.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef lse_thread_pool_task* lse_thread_pool_task_ptr;

typedef struct {
  napi_env env;
  napi_value namespace_object;
} lse_namespace;

napi_value lse_core_class_new(napi_env env, uint8_t type, lse_object* object, napi_value* argv, size_t argc);
napi_value lse_core_class_new_with_constructor(napi_env env, napi_value constructor, lse_object* object);
napi_value lse_core_class_new_with_object(napi_env env, uint8_t type, lse_object* object);
napi_value lse_core_class_new_with_object_ref(napi_env env, uint8_t type, lse_object* object);
napi_value lse_core_class_new_with_args(napi_env env, uint8_t type, napi_value* argv, size_t argc);

bool lse_core_class_instanceof(napi_env env, uint8_t type, napi_value instance);

void lse_core_class_bind(napi_env env, lse_string* type_name, napi_value class_value);

void lse_core_class_cleanup_hook(void* js_env);
void lse_core_env_cleanup_hook(void* js_env);

void lse_core_env_init(napi_env env, napi_value exports);
void lse_core_style_init(napi_env env, napi_value exports);
void lse_core_window_init(napi_env env, napi_value exports);

void lse_core_exports(napi_env env, napi_value exports);

bool lse_core_get_cb_info(
    napi_env env,
    napi_callback_info info,
    void** self,
    uint8_t self_type,
    bool is_interface,
    napi_value* args,
    size_t arg_count);

napi_value lse_core_throw_error(napi_env env, lse_status status);
#define LSE_CORE_CHECK_STATUS(env, status)                                                                             \
  if ((status) != LSE_OK) {                                                                                            \
    return lse_core_throw_error(env, status);                                                                          \
  }
#define LSE_CORE_CHECK_EXPR(env, expr, status)                                                                         \
  if (!(expr)) {                                                                                                       \
    return lse_core_throw_error(env, status);                                                                          \
  }

napi_value lse_core_wrap(napi_env env, napi_value js_object, void* native_object, napi_finalize finalize_cb);
lse_object* lse_core_unwrap(napi_env env, napi_value js_object);

lse_string* lse_core_unbox_s(napi_env env, napi_value string_value);
napi_value lse_core_box_s(napi_env env, lse_string* string);

lse_namespace lse_namespace_init(napi_env env, napi_value exports, const char* name);
void lse_add_function(lse_namespace* ns, const char* name, napi_callback func);

void lse_core_thread_pool_install(lse_env* env, napi_env js_env);
void lse_core_register_yoga_listeners();

int32_t lse_core_unbox_scancode(napi_env env, napi_value value);
lse_key lse_core_unbox_key(napi_env env, napi_value value);
lse_analog_key lse_core_unbox_analog_key(napi_env env, napi_value value);

typedef struct lse_function {
  const char* name;
  napi_callback binding;
} lse_function;

void lse_core_export_namespace(napi_env env, napi_value exports, const char* ns, lse_function* funcs, size_t len);

// ////////////////////////////////////////////////////////////////////////////
// env constants
// ////////////////////////////////////////////////////////////////////////////

#define JS_NAMESPACE_ENV "$env"

#define JS_ENV_NEW_INSTANCE "$newInstance"
#define JS_ENV_CONFIGURE "$configure"
#define JS_ENV_UPDATE "$update"
#define JS_ENV_DESTROY "$destroy"
#define JS_ENV_ADD_WINDOW "$addWindow"
#define JS_ENV_REMOVE_WINDOW "$removeWindow"
#define JS_ENV_GET_DISPLAY_NAME "$getDisplayName"
#define JS_ENV_GET_CURRENT_DISPLAY_MODE "$getCurrentDisplayMode"
#define JS_ENV_GET_DESKTOP_DISPLAY_MODE "$getDesktopDisplayMode"
#define JS_ENV_GET_DISPLAY_COUNT "$getDisplayCount"
#define JS_ENV_GET_VIDEO_DRIVERS "$getVideoDrivers"
#define JS_ENV_GET_RENDERERS "$getRenderers"
#define JS_ENV_GET_DISPLAY_MODES "$getDisplayModes"
#define JS_ENV_GET_FIND_CLOSEST_DISPLAY_MODE "$findClosestDisplayMode"
#define JS_ENV_REGISTER_EVENT_CALLBACKS "$registerEventCallbacks"
#define JS_ENV_RESET_EVENT_CALLBACKS "$resetEventCallbacks"
#define JS_ENV_GET_KEYBOARD "$getKeyboard"
#define JS_ENV_GET_GAMEPADS "$getGamepads"
#define JS_ENV_GET_MAPPING "$getMapping"
#define JS_ENV_SET_MAPPING "$setMapping"
#define JS_ENV_RESET_MAPPING "$resetMapping"
#define JS_ENV_ADD_FONT "$addFont"
#define JS_ENV_IS_CONFIGURED "$isConfigured"
#define JS_ENV_IS_RUNNING "$isRunning"
#define JS_ENV_IS_DESTROYED "$isDestroyed"
#define JS_ENV_IS_ERROR "$isError"

// ////////////////////////////////////////////////////////////////////////////
// window constants
// ////////////////////////////////////////////////////////////////////////////

#define JS_NAMESPACE_WINDOW "$window"

#define JS_WINDOW_CONFIGURE "$configure"
#define JS_WINDOW_GET_ROOT "$getRoot"
#define JS_WINDOW_SET_TITLE "$setTitle"
#define JS_WINDOW_GET_WIDTH "$getWidth"
#define JS_WINDOW_GET_HEIGHT "$getHeight"
#define JS_WINDOW_GET_REFRESH_RATE "$getRefreshRate"
#define JS_WINDOW_GET_FLAGS "$getFlags"
#define JS_WINDOW_CREATE_NODE "$createNode"
#define JS_WINDOW_ADD_IMAGE "$addImage"

// ////////////////////////////////////////////////////////////////////////////
// style constants
// ////////////////////////////////////////////////////////////////////////////

#define JS_NAMESPACE_STYLE "$style"

#define JS_STYLE_GET "$get"
#define JS_STYLE_SET "$set"
#define JS_STYLE_RESET "$reset"
#define JS_STYLE_LOCK "$lock"
#define JS_STYLE_SET_PARENT "$setParent"
#define JS_STYLE_RESOLVE_TRANSLATE_VALUE "$resolveTranslateValue"
#define JS_STYLE_CREATE_CLASS "$createClass"

#define JS_COMMON_POST_CONSTRUCT "$postWrap"

// ////////////////////////////////////////////////////////////////////////////
// macros
// ////////////////////////////////////////////////////////////////////////////

#define STRINGIFY(ID) (#ID)

#define JS_METHOD_SIG(TYPE, REQUIRED_ARGC)                                                                             \
  TYPE* self;                                                                                                          \
  napi_value __args[(REQUIRED_ARGC) + 1];                                                                              \
  napi_value* argv = &__args[1];                                                                                       \
  if (!lse_core_get_cb_info(env, info, (void**)&self, TYPE##_type, false, __args, (REQUIRED_ARGC) + 1)) {              \
    return JS_UNDEFINED;                                                                                               \
  }

#define JS_METHOD_SIG_NO_ARGS(TYPE)                                                                                    \
  TYPE* self;                                                                                                          \
  napi_value __args[1];                                                                                                \
  if (!lse_core_get_cb_info(env, info, (void**)&self, TYPE##_type, false, __args, 1)) {                                \
    return JS_UNDEFINED;                                                                                               \
  }

#define JS_FUNCTION_SIG(REQUIRED_ARGC)                                                                                 \
  napi_value argv[REQUIRED_ARGC];                                                                                      \
  if (!lse_core_get_cb_info(env, info, NULL, 0, false, argv, REQUIRED_ARGC)) {                                         \
    return JS_UNDEFINED;                                                                                               \
  }

#define JS_FUNCTION_SIG_NO_ARGS()                                                                                      \
  if (!lse_core_get_cb_info(env, info, NULL, 0, false, NULL, 0)) {                                                     \
    return JS_UNDEFINED;                                                                                               \
  }

#ifdef __cplusplus
} // end extern "C"
#endif
