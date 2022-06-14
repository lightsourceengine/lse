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

#include <node_api.h>

#ifdef __cplusplus
extern "C" {
#endif

void napix_throw_error(napi_env env, const char* code, const char* message);
bool napix_is_exception_pending(napi_env env);

bool napix_read_string(napi_env env, napi_value string_value, char* target, size_t target_len);
bool napix_as_boolean(napi_env env, napi_value value, bool fallback);

int32_t napix_unbox_i(napi_env env, napi_value value, int32_t fallback);
uint32_t napix_unbox_u(napi_env env, napi_value value, uint32_t fallback);
float napix_unbox_f(napi_env env, napi_value value, float fallback);

napi_valuetype napix_type_of(napi_env env, napi_value value);
bool napix_is_nullish(napi_env env, napi_value value);

napi_value napix_create_int32(napi_env env, int32_t value);
napi_value napix_create_float(napi_env env, float value);
napi_value napix_create_uint32(napi_env env, uint32_t value);
napi_value napix_create_string(napi_env env, const char* utf8);
napi_value napix_create_object(napi_env env);
napi_value napix_create_array(napi_env env, size_t length);
napi_value napix_get_boolean(napi_env env, bool value);
napi_value napix_get_null(napi_env env);
napi_value napix_get_undefined(napi_env env);

napi_value napix_get_own_property(napi_env env, napi_value object, const char* name);

napi_value napix_get_element(napi_env env, napi_value object, uint32_t index);
uint32_t napix_get_element_u(napi_env env, napi_value value, uint32_t index, uint32_t fallback);
int32_t napix_get_element_i(napi_env env, napi_value value, uint32_t index, int32_t fallback);
float napix_get_element_f(napi_env env, napi_value value, uint32_t index, float fallback);

bool napix_call_ref_function(napi_env env, napi_ref function, napi_value* argv, size_t argc, napi_value* result);

bool napix_safe_wrap(napi_env env, napi_value js_object, void* native_object, napi_finalize finalize_cb);

bool napix_obj_get_boolean(napi_env env, napi_value obj, const char* name, bool fallback);
int32_t napix_obj_get_i(napi_env env, napi_value obj, const char* name, int32_t fallback);
bool napix_obj_read_cstring(napi_env env, napi_value obj, const char* name, char* target, size_t target_size);

#define JS_CALLBACK(FUNC) static napi_value FUNC(napi_env env, napi_callback_info info)

#define JS_UNDEFINED ((napi_value)NULL)

#ifdef __cplusplus
} // end extern "C"
#endif
