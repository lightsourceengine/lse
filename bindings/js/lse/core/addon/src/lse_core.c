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
#include <stdio.h>

static bool has_pending_exception(napi_env env);

bool lse_core_get_cb_info(
    napi_env env,
    napi_callback_info info,
    void** self,
    uint8_t self_type,
    bool is_interface,
    napi_value* args,
    size_t arg_count) {
  size_t argc = arg_count;

  if (napi_get_cb_info(env, info, &argc, args, NULL, NULL) != napi_ok || argc != arg_count) {
    lse_core_throw_error(env, LSE_ERR_JS_ARGUMENT_COUNT);
    return false;
  }

  if (self) {
    uint8_t type;
    bool result = false;

    if (napi_unwrap(env, args[0], self) == napi_ok) {
      lse_object* obj = *self;

      if (is_interface) {
        type = lse_object_get_interface_type(obj);
      } else {
        type = lse_object_get_type(obj);
      }

      result = type == self_type;
    }

    if (!result) {
      lse_core_throw_error(env, LSE_ERR_JS_SELF);
    }

    return result;
  }

  return true;
}

napi_value lse_core_throw_error(napi_env env, lse_status status) {
  if (!has_pending_exception(env)) {
    char error_code[32];

    sprintf(error_code, "E%i", status);

    napi_throw_error(env, error_code, lse_status_string(status));
  }

  return JS_UNDEFINED;
}

napi_value lse_core_wrap(napi_env env, napi_value js_object, void* native_object, napi_finalize finalize_cb) {
  if (napix_type_of(env, js_object) != napi_object ||
      napi_wrap(env, js_object, native_object, finalize_cb, NULL, NULL) != napi_ok) {
    if (finalize_cb) {
      finalize_cb(env, native_object, NULL);
    }

    return lse_core_throw_error(env, LSE_ERR_JS_WRAP);
  }

  return js_object;
}

lse_object* lse_core_unwrap(napi_env env, napi_value js_object) {
  lse_object* result;

  return (js_object && napi_unwrap(env, js_object, (void**)&result) == napi_ok) ? result : NULL;
}

lse_string* lse_core_unbox_s(napi_env env, napi_value string_value) {
  size_t len;
  lse_string* result;

  if (!string_value || napi_get_value_string_utf8(env, string_value, NULL, 0, &len) != napi_ok) {
    return NULL;
  }

  if (len == 0) {
    return lse_string_new_empty();
  }

  if (!(result = lse_string_new_with_size(' ', len))) {
    return NULL;
  }

  // + 1 for null terminator
  if (napi_get_value_string_utf8(env, string_value, lse_string_as_data(result), len + 1, NULL) != napi_ok) {
    lse_unref(result);

    return NULL;
  }

  return result;
}

napi_value lse_core_box_s(napi_env env, lse_string* string) {
  if (string == NULL) {
    return NULL;
  }

  return napix_create_string(env, lse_string_as_cstring(string));
}

lse_namespace lse_namespace_init(napi_env env, napi_value exports, const char* name) {
  napi_value namespace_object;

  if (has_pending_exception(env)) {
    namespace_object = NULL;
  } else {
    if (name[0] == '\0') {
      namespace_object = exports;
    } else {
      namespace_object = napix_create_object(env);

      if (!namespace_object || napi_set_named_property(env, exports, name, namespace_object) != napi_ok) {
        namespace_object = NULL;
        lse_core_throw_error(env, LSE_ERR_JS_EXPORT);
      }
    }
  }

  return (lse_namespace){
    .env = env,
    .namespace_object = namespace_object,
  };
}

void lse_add_function(lse_namespace* ns, const char* name, napi_callback func) {
  if (has_pending_exception(ns->env)) {
    return;
  }

  napi_env env = ns->env;
  napi_value func_value;

  if (napi_create_function(env, name, NAPI_AUTO_LENGTH, func, NULL, &func_value) != napi_ok) {
    lse_core_throw_error(env, LSE_ERR_JS_EXPORT);
    return;
  }

  if (napi_set_named_property(env, ns->namespace_object, name, func_value) != napi_ok) {
    lse_core_throw_error(env, LSE_ERR_JS_EXPORT);
    return;
  }
}

static bool has_pending_exception(napi_env env) {
  bool result;

  return napi_is_exception_pending(env, &result) != napi_ok || result;
}

int32_t lse_core_unbox_scancode(napi_env env, napi_value value) {
  return napix_unbox_i(env, value, LSE_SCANCODE_UNKNOWN);
}

lse_key lse_core_unbox_key(napi_env env, napi_value value) {
  int32_t i = napix_unbox_i(env, value, LSE_KEY_UNKNOWN);

  return (i >= k_lse_key_min && i <= k_lse_key_max) ? (lse_key)i : LSE_KEY_UNKNOWN;
}

lse_analog_key lse_core_unbox_analog_key(napi_env env, napi_value value) {
  int32_t i = napix_unbox_i(env, value, LSE_KEY_UNKNOWN);

  return (i >= k_lse_analog_key_min && i <= k_lse_analog_key_max) ? (lse_analog_key)i : LSE_ANALOG_KEY_UNKNOWN;
}

void lse_core_export_namespace(napi_env env, napi_value exports, const char* ns, lse_function* funcs, size_t len) {
  lse_namespace namespace = lse_namespace_init(env, exports, ns);

  for (size_t i = 0; i < len; i++) {
    lse_add_function(&namespace, funcs[i].name, funcs[i].binding);
  }
}
