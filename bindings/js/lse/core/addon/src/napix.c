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

#include "napix.h"

bool napix_safe_wrap(napi_env env, napi_value js_object, void* native_object, napi_finalize finalize_cb) {
  if (napi_wrap(env, js_object, native_object, finalize_cb, NULL, NULL) != napi_ok) {
    if (finalize_cb) {
      finalize_cb(env, native_object, NULL);
    }

    napix_throw_error(env, "", "cannot wrap native object");

    return false;
  }

  return true;
}

napi_value napix_create_int32(napi_env env, int32_t value) {
  napi_value obj;

  return napi_create_int32(env, value, &obj) == napi_ok ? obj : NULL;
}

napi_value napix_create_float(napi_env env, float value) {
  napi_value obj;

  return napi_create_double(env, value, &obj) == napi_ok ? obj : NULL;
}

napi_value napix_create_uint32(napi_env env, uint32_t value) {
  napi_value obj;

  return napi_create_uint32(env, value, &obj) == napi_ok ? obj : NULL;
}

napi_value napix_create_string(napi_env env, const char* utf8) {
  napi_value obj;

  return napi_create_string_utf8(env, utf8 ? utf8 : "", NAPI_AUTO_LENGTH, &obj) == napi_ok ? obj : NULL;
}

napi_value napix_create_object(napi_env env) {
  napi_value obj;

  return napi_create_object(env, &obj) == napi_ok ? obj : NULL;
}

napi_value napix_create_array(napi_env env, size_t length) {
  napi_value array;
  napi_status status;

  if (length == 0) {
    status = napi_create_array(env, &array);
  } else {
    status = napi_create_array_with_length(env, length, &array);
  }

  return status == napi_ok ? array : NULL;
}

napi_value napix_get_boolean(napi_env env, bool value) {
  napi_value result;

  if (napi_get_boolean(env, value, &result) != napi_ok) {
    return NULL;
  }

  return result;
}

napi_value napix_get_null(napi_env env) {
  napi_value value;

  return napi_get_null(env, &value) == napi_ok ? value : NULL;
}

napi_value napix_get_undefined(napi_env env) {
  napi_value value;

  return napi_get_undefined(env, &value) == napi_ok ? value : NULL;
}

napi_value napix_get_own_property(napi_env env, napi_value object, const char* name) {
  bool result;
  napi_value value;

  if (napi_has_named_property(env, object, name, &result) == napi_ok && result &&
      napi_get_named_property(env, object, name, &value) == napi_ok) {
    return value;
  }

  return JS_UNDEFINED;
}

napi_value napix_get_element(napi_env env, napi_value object, uint32_t index) {
  napi_value value;

  if (napi_get_element(env, object, index, &value) != napi_ok) {
    return NULL;
  }

  return value;
}

uint32_t napix_get_element_u(napi_env env, napi_value value, uint32_t index, uint32_t fallback) {
  napi_value element;

  if (napi_get_element(env, value, index, &element) != napi_ok) {
    element = NULL;
  }

  return napix_unbox_u(env, element, fallback);
}

int32_t napix_get_element_i(napi_env env, napi_value value, uint32_t index, int32_t fallback) {
  napi_value element;

  if (napi_get_element(env, value, index, &element) != napi_ok) {
    element = NULL;
  }

  return napix_unbox_i(env, element, fallback);
}

float napix_get_element_f(napi_env env, napi_value value, uint32_t index, float fallback) {
  napi_value element;

  if (napi_get_element(env, value, index, &element) != napi_ok) {
    element = NULL;
  }

  return napix_unbox_f(env, element, fallback);
}

bool napix_call_ref_function(napi_env env, napi_ref function, napi_value* argv, size_t argc, napi_value* result) {
  napi_value function_value;

  if (!function || napi_get_reference_value(env, function, &function_value) != napi_ok) {
    return false;
  }

  if (napi_call_function(env, napix_get_undefined(env), function_value, argc, argv, result) != napi_ok) {
    return false;
  }

  return true;
}

void napix_throw_error(napi_env env, const char* code, const char* message) {
  if (!napix_is_exception_pending(env)) {
    napi_throw_error(env, code, message);
  }
}

bool napix_is_exception_pending(napi_env env) {
  bool result = false;

  return (napi_is_exception_pending(env, &result) != napi_ok) || result;
}

bool napix_read_string(napi_env env, napi_value string_value, char* target, size_t target_len) {
  size_t len;

  if (!string_value || napi_get_value_string_utf8(env, string_value, NULL, 0, &len) != napi_ok || len == 0 ||
      (len + 1) > target_len) {
    return false;
  }

  if (napi_get_value_string_utf8(env, string_value, target, len + 1, NULL) != napi_ok) {
    return false;
  }

  return true;
}

int32_t napix_unbox_i(napi_env env, napi_value value, int32_t fallback) {
  int32_t result;

  if (value && napi_get_value_int32(env, value, &result) == napi_ok) {
    return result;
  }

  return fallback;
}


uint32_t napix_unbox_u(napi_env env, napi_value value, uint32_t fallback) {
  uint32_t result;

  if (value && napi_get_value_uint32(env, value, &result) == napi_ok) {
    return result;
  }

  return fallback;
}

float napix_unbox_f(napi_env env, napi_value value, float fallback) {
  double result;

  if (value && napi_get_value_double(env, value, &result) == napi_ok) {
    return (float)result;
  }

  return (float)fallback;
}

bool napix_as_boolean(napi_env env, napi_value value, bool fallback) {
  bool result;

  if (value && napi_get_value_bool(env, value, &result) == napi_ok) {
    return result;
  }

  return fallback;
}

napi_valuetype napix_type_of(napi_env env, napi_value value) {
  napi_valuetype type;

  return (value && napi_typeof(env, value, &type) == napi_ok) ? type : napi_undefined;
}

bool napix_is_nullish(napi_env env, napi_value value) {
  switch (napix_type_of(env, value)) {
    case napi_undefined:
    case napi_null:
      return true;
    default:
      return false;
  }
}

bool napix_obj_get_boolean(napi_env env, napi_value obj, const char* name, bool fallback) {
  napi_value value = napix_get_own_property(env, obj, name);

  if (napix_type_of(env, value) == napi_boolean) {
    return napix_as_boolean(env, value, fallback);
  }

  return fallback;
}

int32_t napix_obj_get_i(napi_env env, napi_value obj, const char* name, int32_t fallback) {
  napi_value value = napix_get_own_property(env, obj, name);

  if (napix_type_of(env, value) == napi_number) {
    return napix_unbox_i(env, value, fallback);
  }

  return fallback;
}

bool napix_obj_read_cstring(napi_env env, napi_value obj, const char* name, char* target, size_t target_size) {
  napi_value value = napix_get_own_property(env, obj, name);

  if (napix_type_of(env, value) == napi_string) {
    return napix_read_string(env, value, target, target_size);
  }

  return false;
}
