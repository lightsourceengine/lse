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
#include <lse_style.h>
#include <lse_style_meta.h>
#include <lse_types.h>
#include <lse_util.h>
#include <assert.h>

static napi_value get_enum(napi_env env, lse_style* style, lse_style_property property);
static napi_value get_numeric(napi_env env, lse_style* style, lse_style_property property);
static napi_value get_string(napi_env env, lse_style* style, lse_style_property property);
static napi_value get_color(napi_env env, lse_style* style, lse_style_property property);
static bool set_from_js_number(napi_env env, lse_style* style, lse_style_property property, napi_value number);
static bool set_from_js_object(napi_env env, lse_style* style, lse_style_property property, napi_value object);
static bool set_from_js_string(napi_env env, lse_style* style, lse_style_property property, napi_value object);

static lse_object* unbox_transform_as_array(napi_env env, napi_value object);
static lse_object* unbox_filter_as_array(napi_env env, napi_value object);
static lse_object* unbox_array(napi_env env, napi_value object, lse_style_property property);

static bool unbox_style_transform(napi_env env, napi_value object, lse_style_transform* transform);
static bool unbox_style_filter(napi_env env, napi_value object, lse_style_filter* filter);
static lse_style_value unbox_style_value(napi_env env, napi_value object);

JS_CALLBACK(set) {
  JS_METHOD_SIG(lse_style, 2)

  if (lse_style_is_locked(self)) {
    return JS_UNDEFINED;
  }

  int32_t propertyArg = napix_unbox_i(env, argv[0], -1);
  lse_style_property property;
  bool was_set = false;

  if (!lse_style_meta_is_style_property(propertyArg)) {
    return lse_core_throw_error(env, LSE_ERR_STYLE_PROPERTY);
  }

  property = (lse_style_property)propertyArg;

  switch (napix_type_of(env, argv[1])) {
    case napi_string:
      was_set = set_from_js_string(env, self, property, argv[1]);
      break;
    case napi_number:
      was_set = set_from_js_number(env, self, property, argv[1]);
      break;
    case napi_object:
      was_set = set_from_js_object(env, self, property, argv[1]);
      break;
    default:
      break;
  }

  if (!was_set) {
    lse_style_unset(self, property);
  }

  return JS_UNDEFINED;
}

JS_CALLBACK(get) {
  JS_METHOD_SIG(lse_style, 1)

  int32_t propertyArg = napix_unbox_i(env, argv[0], -1);
  lse_style_property property;

  if (!lse_style_meta_is_style_property(propertyArg)) {
    return lse_core_throw_error(env, LSE_ERR_STYLE_PROPERTY);
  }

  property = (lse_style_property)propertyArg;

  switch (lse_style_meta_get_property_type(property)) {
    case LSE_STYLE_PROPERTY_TYPE_ENUM:
      return get_enum(env, self, property);
    case LSE_STYLE_PROPERTY_TYPE_NUMBER:
      return get_numeric(env, self, property);
    case LSE_STYLE_PROPERTY_TYPE_STRING:
      return get_string(env, self, property);
    case LSE_STYLE_PROPERTY_TYPE_COLOR:
      return get_color(env, self, property);
    default:
      break;
  }

  return lse_core_throw_error(env, LSE_ERR_STYLE_PROPERTY);
}

JS_CALLBACK(reset) {
  JS_METHOD_SIG_NO_ARGS(lse_style)

  lse_style_reset(self);

  return JS_UNDEFINED;
}

JS_CALLBACK(lock) {
  JS_METHOD_SIG_NO_ARGS(lse_style)

  lse_style_lock(self);

  return __args[0];
}

JS_CALLBACK(set_parent) {
  JS_METHOD_SIG(lse_style, 1);

  lse_style* parent;
  bool got_parent;

  switch (napix_type_of(env, argv[0])) {
    case napi_object:
      parent = (lse_style*)lse_core_unwrap(env, argv[0]);
      got_parent = (parent != NULL);
      break;
    case napi_null:
      parent = NULL;
      got_parent = true;
      break;
    default:
      got_parent = false;
      break;
  }

  return (got_parent && lse_style_set_parent(self, parent)) ? argv[0] : lse_core_throw_error(env, LSE_ERR_JS_CLASS);
}

JS_CALLBACK(resolve_translate_value) {
  JS_FUNCTION_SIG(1);

  //  lse_style_from_string()

  return JS_UNDEFINED;
}

JS_CALLBACK(create_class) {
  JS_FUNCTION_SIG(1);

  napi_value constructor = argv[0];

  if (napix_type_of(env, constructor) != napi_function) {
    return lse_core_throw_error(env, LSE_ERR_JS_EXPECTED_FUNCTION);
  }

  return lse_core_class_new_with_constructor(env, constructor, (lse_object*)lse_style_new_class());
}

void lse_core_style_init(napi_env env, napi_value exports) {
  lse_namespace ns = lse_namespace_init(env, exports, JS_NAMESPACE_STYLE);

  lse_add_function(&ns, JS_STYLE_GET, &get);
  lse_add_function(&ns, JS_STYLE_SET, &set);
  lse_add_function(&ns, JS_STYLE_RESET, &reset);
  lse_add_function(&ns, JS_STYLE_LOCK, &lock);
  lse_add_function(&ns, JS_STYLE_SET_PARENT, &set_parent);
  lse_add_function(&ns, JS_STYLE_RESOLVE_TRANSLATE_VALUE, &resolve_translate_value);
  lse_add_function(&ns, JS_STYLE_CREATE_CLASS, &create_class);
}

static napi_value get_enum(napi_env env, lse_style* style, lse_style_property property) {
  int32_t enum_value = lse_style_get_enum(style, property);
  const char* string_value = lse_style_meta_enum_to_string(property, enum_value);

  if (!string_value) {
    assert(false && "get_enum_to_string");
    return JS_UNDEFINED;
  }

  return napix_create_string(env, string_value);
}

static napi_value get_numeric(napi_env env, lse_style* style, lse_style_property property) {
  const lse_style_value* numeric = lse_style_get_numeric(style, property);

  if (numeric->unit != LSE_STYLE_UNIT_UNDEFINED) {
    napi_value args[] = { napix_create_float(env, numeric->value), napix_create_int32(env, numeric->unit) };

    return lse_core_class_new_with_args(env, lse_style_value_type, args, c_arraylen(args));
  }

  return JS_UNDEFINED;
}

static napi_value get_string(napi_env env, lse_style* style, lse_style_property property) {
  lse_object* object = lse_style_get_object(style, property);

  if (object) {
    return napix_create_string(env, lse_string_as_cstring((lse_string*)object));
  }

  return JS_UNDEFINED;
}

static napi_value get_color(napi_env env, lse_style* style, lse_style_property property) {
  return napix_create_uint32(env, lse_style_get_color(style, property));
}

static bool set_from_js_number(napi_env env, lse_style* style, lse_style_property property, napi_value number) {
  uint32_t color;

  if (property == LSE_SP_BORDER_RADIUS) {
    ;
  }

  switch (lse_style_meta_get_property_type(property)) {
    case LSE_STYLE_PROPERTY_TYPE_COLOR:
      return napi_get_value_uint32(env, number, &color) == napi_ok && lse_style_set_color(style, property, color);
    case LSE_STYLE_PROPERTY_TYPE_NUMBER:
      return lse_style_set_numeric(
          style, property, &(lse_style_value){ .value = napix_unbox_f(env, number, 0), .unit = LSE_STYLE_UNIT_PX });
    default:
      return false;
  }
}

static bool set_from_js_object(napi_env env, lse_style* style, lse_style_property property, napi_value object) {
  bool is_array;
  lse_style_value style_value;

  if (lse_core_class_instanceof(env, lse_style_value_type, object)) {
    style_value = unbox_style_value(env, object);
    return lse_style_set_numeric(style, property, &style_value);
  } else if (lse_core_class_instanceof(env, lse_style_transform_type, object)) {
    return lse_style_set_object(style, property, unbox_transform_as_array(env, object));
  } else if (lse_core_class_instanceof(env, lse_style_filter_type, object)) {
    return lse_style_set_object(style, property, unbox_filter_as_array(env, object));
  } else if (napi_is_array(env, object, &is_array) == napi_ok && is_array) {
    return lse_style_set_object(style, property, unbox_array(env, object, property));
  }

  return false;
}

static bool set_from_js_string(napi_env env, lse_style* style, lse_style_property property, napi_value object) {
  return lse_style_from_string(style, property, lse_core_unbox_s(env, object));
}

static lse_style_value unbox_style_value(napi_env env, napi_value object) {
  lse_style_value result;
  int32_t raw_unit = -1;

  switch (napix_type_of(env, object)) {
    case napi_number:
      result.value = napix_unbox_f(env, object, 0);
      raw_unit = LSE_STYLE_UNIT_PX;
      break;
    case napi_string:
      if (lse_style_parse_numeric(lse_core_unbox_s(env, object), &result)) {
        raw_unit = result.unit;
      }
      break;
    case napi_object:
      if (lse_core_class_instanceof(env, lse_style_value_type, object)) {
        result.value = napix_get_element_f(env, object, 0u, LSE_UNDEFINED);
        raw_unit = napix_get_element_i(env, object, 1u, LSE_STYLE_UNIT_UNDEFINED);
      }
      break;
    default:
      break;
  }

  if (!lse_style_meta_is_unit(raw_unit)) {
    result = (lse_style_value){ 0 };
  } else {
    result.unit = raw_unit;
  }

  return result;
}

static lse_object* unbox_transform_as_array(napi_env env, napi_value object) {
  lse_style_transform transform;

  if (!unbox_style_transform(env, object, &transform)) {
    return (lse_object*)lse_array_new_empty();
  }

  return (lse_object*)lse_style_transform_new(&transform, 1);
}

static lse_object* unbox_filter_as_array(napi_env env, napi_value object) {
  lse_style_filter filter;

  if (!unbox_style_filter(env, object, &filter)) {
    return (lse_object*)lse_array_new_empty();
  }

  return (lse_object*)lse_style_filter_new(&filter, 1);
}

static lse_object* unbox_array(napi_env env, napi_value object, lse_style_property property) {
  uint32_t i;
  uint32_t length;
  lse_style_property_type type;
  lse_array* result;

  if (napi_get_array_length(env, object, &length) == napi_ok && length > 0) {
    type = lse_style_meta_get_property_type(property);
  } else {
    type = LSE_STYLE_PROPERTY_TYPE_UNKNOWN;
  }

  switch (type) {
    case LSE_STYLE_PROPERTY_TYPE_FILTER:
      result = lse_array_new(length, sizeof(lse_style_filter), NULL);
      break;
    case LSE_STYLE_PROPERTY_TYPE_TRANSFORM:
      result = lse_array_new(length, sizeof(lse_style_transform), NULL);
      break;
    default:
      result = lse_array_new_empty();
      length = 0;
      break;
  }

  for (i = 0; i < length; i++) {
    bool unboxed;

    switch (type) {
      case LSE_STYLE_PROPERTY_TYPE_FILTER:
        unboxed = unbox_style_filter(env, napix_get_element(env, object, i), lse_array_at(result, i));
        break;
      case LSE_STYLE_PROPERTY_TYPE_TRANSFORM:
        unboxed = unbox_style_transform(env, napix_get_element(env, object, i), lse_array_at(result, i));
        break;
      default:
        unboxed = false;
        break;
    }

    if (!unboxed) {
      lse_unref(result);
      result = lse_array_new_empty();
      break;
    }
  }

  return (lse_object*)result;
}

static bool unbox_style_transform(napi_env env, napi_value object, lse_style_transform* transform) {
  if (!lse_core_class_instanceof(env, lse_style_transform_type, object)) {
    return false;
  }

  switch (napix_get_element_i(env, object, 0u, -1)) {
    case LSE_STYLE_TRANSFORM_IDENTITY:
      return lse_style_transform_identity(transform);
    case LSE_STYLE_TRANSFORM_TRANSLATE:
      return lse_style_transform_translate(
          unbox_style_value(env, napix_get_element(env, object, 1u)),
          unbox_style_value(env, napix_get_element(env, object, 2u)),
          transform);
    case LSE_STYLE_TRANSFORM_SCALE:
      return lse_style_transform_scale(
          unbox_style_value(env, napix_get_element(env, object, 1u)),
          unbox_style_value(env, napix_get_element(env, object, 2u)),
          transform);
    case LSE_STYLE_TRANSFORM_ROTATE:
      return lse_style_transform_rotate(unbox_style_value(env, napix_get_element(env, object, 1u)), transform);
    default:
      return false;
  }
}

static bool unbox_color(napi_env env, napi_value object, uint32_t* color) {
  switch (napix_type_of(env, object)) {
    case napi_number:
      *color = napix_unbox_u(env, object, 0);
      return true;
    case napi_string:
      return lse_style_parse_color(lse_core_unbox_s(env, object), color);
    default:
      return false;
  }
}

static bool unbox_style_filter(napi_env env, napi_value object, lse_style_filter* filter) {
  uint32_t color;

  if (!lse_core_class_instanceof(env, lse_style_filter_type, object)) {
    return false;
  }

  switch (napix_get_element_i(env, object, 0u, -1)) {
    case LSE_STYLE_FILTER_FLIP_H:
      return lse_style_filter_flip_h(filter);
    case LSE_STYLE_FILTER_FLIP_V:
      return lse_style_filter_flip_v(filter);
    case LSE_STYLE_FILTER_TINT:
      return unbox_color(env, napix_get_element(env, object, 1u), &color) && lse_style_filter_tint(color, filter);
    default:
      return false;
  }
}
