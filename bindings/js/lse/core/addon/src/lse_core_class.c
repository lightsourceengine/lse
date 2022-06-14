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
#include <lse_types.h>

typedef struct {
  const char* key;
  uint8_t type;
} js_type_entry;

// clang-format off
#define LSE_JS_TYPE_ENTRY(CLASS) { #CLASS, CLASS##_type }

static const js_type_entry k_js_type_map[] = {
    LSE_JS_TYPE_ENTRY(lse_box_node),
    LSE_JS_TYPE_ENTRY(lse_display_mode),
    LSE_JS_TYPE_ENTRY(lse_env),
    LSE_JS_TYPE_ENTRY(lse_font),
    LSE_JS_TYPE_ENTRY(lse_gamepad),
    LSE_JS_TYPE_ENTRY(lse_image),
    LSE_JS_TYPE_ENTRY(lse_image_node),
    LSE_JS_TYPE_ENTRY(lse_keyboard),
    LSE_JS_TYPE_ENTRY(lse_renderer_info),
    LSE_JS_TYPE_ENTRY(lse_root_node),
    LSE_JS_TYPE_ENTRY(lse_style),
    LSE_JS_TYPE_ENTRY(lse_style_value),
    LSE_JS_TYPE_ENTRY(lse_text_node),
    LSE_JS_TYPE_ENTRY(lse_window),

    LSE_JS_TYPE_ENTRY(lse_style_value),
    LSE_JS_TYPE_ENTRY(lse_style_filter),
    LSE_JS_TYPE_ENTRY(lse_style_transform),
};

// clang-format on

static napi_ref s_js_class_table[UINT8_MAX] = { 0 };

static napi_value get_class(napi_env env, uint8_t type);
static napi_value wrap_with_object(napi_env env, napi_value instance, lse_object* object);

void lse_core_class_bind(napi_env env, lse_string* type_name, napi_value class_value) {
  if (!type_name) {
    lse_core_throw_error(env, LSE_ERR_ILLEGAL_ARGUMENT);
    return;
  }

  const char* type_name_cstr = lse_string_as_cstring(type_name);

  for (size_t i = 0; i < c_arraylen(k_js_type_map); i++) {
    if (strcmp(type_name_cstr, k_js_type_map[i].key) == 0) {
      uint8_t type = k_js_type_map[i].type;

      if (napi_create_reference(env, class_value, 1, &(s_js_class_table[type])) != napi_ok) {
        lse_core_throw_error(env, LSE_ERR_JS_CLASS);
      }

      goto DONE;
    }
  }

  lse_core_throw_error(env, LSE_ERR_JS_CLASS_UNKNOWN);

DONE:
  lse_unref(type_name);
}

napi_value lse_core_class_new(napi_env env, uint8_t type, lse_object* object, napi_value* argv, size_t argc) {
  return wrap_with_object(env, lse_core_class_new_with_args(env, type, argv, argc), object);
}

napi_value lse_core_class_new_with_constructor(napi_env env, napi_value constructor, lse_object* object) {
  napi_value instance;

  if (!constructor || napi_new_instance(env, constructor, 0, NULL, &instance) != napi_ok) {
    return JS_UNDEFINED;
  }

  return wrap_with_object(env, instance, object);
}

napi_value lse_core_class_new_with_object(napi_env env, uint8_t type, lse_object* object) {
  return wrap_with_object(env, lse_core_class_new_with_args(env, type, NULL, 0), object);
}

napi_value lse_core_class_new_with_object_ref(napi_env env, uint8_t type, lse_object* object) {
  lse_ref(object);
  return wrap_with_object(env, lse_core_class_new_with_args(env, type, NULL, 0), object);
}

napi_value lse_core_class_new_with_args(napi_env env, uint8_t type, napi_value* argv, size_t argc) {
  napi_value class_value = get_class(env, type);
  napi_value instance;

  if (!class_value || napi_new_instance(env, class_value, argc, argv, &instance) != napi_ok) {
    return JS_UNDEFINED;
  }

  return instance;
}

bool lse_core_class_instanceof(napi_env env, uint8_t type, napi_value instance) {
  bool result;
  napi_value class_value = get_class(env, type);

  return class_value && napi_instanceof(env, instance, class_value, &result) == napi_ok && result;
}

void lse_core_class_cleanup_hook(void* js_env) {
  napi_env env = js_env;

  for (size_t i = 0; i < c_arraylen(s_js_class_table); i++) {
    if (s_js_class_table[i]) {
      napi_delete_reference(env, s_js_class_table[i]);
      s_js_class_table[i] = NULL;
    }
  }
}

static napi_value get_class(napi_env env, uint8_t type) {
  napi_value class_value;
  napi_ref class_ref = s_js_class_table[type];

  if (class_ref == NULL) {
    assert(false && "core class not registered");
    return JS_UNDEFINED;
  }

  if (napi_get_reference_value(env, class_ref, &class_value) != napi_ok) {
    assert(false && "where did the constructor go?");
    return JS_UNDEFINED;
  }

  return class_value;
}

static void lse_core_util_unref(napi_env env, void* data, void* hint) {
  lse_object_unref((lse_object*)data);
}

static napi_value wrap_with_object(napi_env env, napi_value instance, lse_object* object) {
  napi_value post_construct;

  if (!object || lse_object_get_type(object) == lse_none_type) {
    return lse_core_throw_error(env, LSE_ERR_OUT_OF_MEMORY);
  }

  if (!instance) {
    lse_object_unref(object);
    return JS_UNDEFINED;
  }

  if (napi_wrap(env, instance, object, &lse_core_util_unref, NULL, NULL) != napi_ok) {
    lse_object_unref(object);
    return JS_UNDEFINED;
  }

  if ((post_construct = napix_get_own_property(env, instance, JS_COMMON_POST_CONSTRUCT))) {
    napi_value return_value;

    napi_call_function(env, instance, post_construct, 0, NULL, &return_value);
  }

  return instance;
}
