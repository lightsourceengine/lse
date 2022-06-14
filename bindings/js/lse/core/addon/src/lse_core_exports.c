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

// ////////////////////////////////////////////////////////////////////////////
// AUTO-GENERATED - DO NOT EDIT
// clang-format off
// ////////////////////////////////////////////////////////////////////////////

#include "lse_core.h"

#include "napix.h"
#include <limits.h>
#include <lse_object.h>
#include <stc/ccommon.h>

#include <lse_keyboard.h>
#include <lse_image.h>
#include <lse_font.h>
#include <lse_gamepad.h>
#include <lse_node.h>

#define lse_core_unbox_self(ENV, X) (self)
#define lse_core_unbox_napi_value(ENV, VALUE) (VALUE)
#define lse_core_unbox_int(ENV, VALUE) napix_unbox_i((ENV), (VALUE), INT_MIN)
#define lse_core_unbox_string(ENV, VALUE) lse_core_unbox_s((ENV), (VALUE))
#define lse_core_unbox_node(ENV, VALUE) (lse_node*)lse_core_unwrap((ENV), (VALUE))

#define lse_core_box_int(ENV, VALUE) napix_create_int32((ENV), (VALUE))
#define lse_core_box_float(ENV, VALUE) napix_create_float((ENV), (VALUE))
#define lse_core_box_bool(ENV, VALUE) napix_get_boolean((ENV), (VALUE))
#define lse_core_box_string(ENV, VALUE) napix_create_string((ENV), lse_string_as_cstring((VALUE)))
#define lse_core_box_cstring(ENV, VALUE) napix_create_string((ENV), (VALUE))
#define lse_core_box_style_ref(ENV, VALUE) lse_core_class_new_with_object_ref((ENV), lse_style_type, (lse_object*)(VALUE))

static napi_value nsglobal_bind(napi_env env, napi_callback_info info) {
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, NULL, lse_none_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  lse_core_class_bind(env,
    lse_core_unbox_string(env, args[0]),
    lse_core_unbox_napi_value(env, args[1])
  );

  return JS_UNDEFINED;
}

static napi_value nskeyboard_getId(napi_env env, napi_callback_info info) {
  lse_keyboard* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_keyboard_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_keyboard_get_id(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nskeyboard_getUUID(napi_env env, napi_callback_info info) {
  lse_keyboard* self;
  const char* result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_keyboard_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_keyboard_get_uuid(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_cstring(env, result);
}

static napi_value nskeyboard_getName(napi_env env, napi_callback_info info) {
  lse_keyboard* self;
  const char* result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_keyboard_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_keyboard_get_name(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_cstring(env, result);
}

static napi_value nskeyboard_isConnected(napi_env env, napi_callback_info info) {
  lse_keyboard* self;
  bool result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_keyboard_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_keyboard_is_connected(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nskeyboard_getScancodeState(napi_env env, napi_callback_info info) {
  lse_keyboard* self;
  bool result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_keyboard_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_keyboard_get_scancode_state(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_scancode(env, args[1])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nsimage_getWidth(napi_env env, napi_callback_info info) {
  lse_image* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_image_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_image_get_width(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsimage_getHeight(napi_env env, napi_callback_info info) {
  lse_image* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_image_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_image_get_height(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsimage_getState(napi_env env, napi_callback_info info) {
  lse_image* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_image_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_image_get_state(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsfont_getUri(napi_env env, napi_callback_info info) {
  lse_font* self;
  lse_string* result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_font_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_font_get_uri(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_string(env, result);
}

static napi_value nsfont_getIndex(napi_env env, napi_callback_info info) {
  lse_font* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_font_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_font_get_index(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsfont_getFamily(napi_env env, napi_callback_info info) {
  lse_font* self;
  lse_string* result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_font_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_font_get_family(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_string(env, result);
}

static napi_value nsfont_getStyle(napi_env env, napi_callback_info info) {
  lse_font* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_font_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_font_get_style(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsfont_getWeight(napi_env env, napi_callback_info info) {
  lse_font* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_font_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_font_get_weight(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsfont_getState(napi_env env, napi_callback_info info) {
  lse_font* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_font_type, false, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_font_get_state(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsgamepad_getId(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_id(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsgamepad_getUUID(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  const char* result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_uuid(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_cstring(env, result);
}

static napi_value nsgamepad_getName(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  const char* result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_name(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_cstring(env, result);
}

static napi_value nsgamepad_isConnected(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  bool result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_is_connected(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nsgamepad_getButtonState(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  bool result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_button_state(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_int(env, args[1])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nsgamepad_getAxisState(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  float result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_axis_state(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_int(env, args[1])
  );

  return lse_core_box_float(env, result);
}

static napi_value nsgamepad_getHatState(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  int32_t result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_hat_state(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_int(env, args[1])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsgamepad_getAnalogState(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  float result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_analog_state(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_analog_key(env, args[1])
  );

  return lse_core_box_float(env, result);
}

static napi_value nsgamepad_getKeyState(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  bool result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_key_state(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_key(env, args[1])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nsgamepad_getButtonCount(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_button_count(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsgamepad_getAxisCount(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_axis_count(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsgamepad_getHatCount(napi_env env, napi_callback_info info) {
  lse_gamepad* self;
  int32_t result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_gamepad_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_gamepad_get_hat_count(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_int(env, result);
}

static napi_value nsnode_getX(napi_env env, napi_callback_info info) {
  lse_node* self;
  float result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_get_x(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_float(env, result);
}

static napi_value nsnode_getY(napi_env env, napi_callback_info info) {
  lse_node* self;
  float result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_get_y(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_float(env, result);
}

static napi_value nsnode_getWidth(napi_env env, napi_callback_info info) {
  lse_node* self;
  float result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_get_width(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_float(env, result);
}

static napi_value nsnode_getHeight(napi_env env, napi_callback_info info) {
  lse_node* self;
  float result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_get_height(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_float(env, result);
}

static napi_value nsnode_appendChild(napi_env env, napi_callback_info info) {
  lse_node* self;
  bool result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_append(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_node(env, args[1])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nsnode_insertChild(napi_env env, napi_callback_info info) {
  lse_node* self;
  bool result;
  napi_value args[3];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_insert_before(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_node(env, args[1]),
    lse_core_unbox_node(env, args[2])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nsnode_removeChild(napi_env env, napi_callback_info info) {
  lse_node* self;
  bool result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_remove_child(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_node(env, args[1])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nsnode_getStyle(napi_env env, napi_callback_info info) {
  lse_node* self;
  lse_style* result;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_get_style(
    lse_core_unbox_self(env, args[0])
  );

  return lse_core_box_style_ref(env, result);
}

static napi_value nsnode_destroy(napi_env env, napi_callback_info info) {
  lse_node* self;
  napi_value args[1];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  lse_node_destroy(
    lse_core_unbox_self(env, args[0])
  );

  return JS_UNDEFINED;
}

static napi_value nsnode_setSource(napi_env env, napi_callback_info info) {
  lse_node* self;
  bool result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_set_src(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_string(env, args[1])
  );

  return lse_core_box_bool(env, result);
}

static napi_value nsnode_setText(napi_env env, napi_callback_info info) {
  lse_node* self;
  bool result;
  napi_value args[2];

  if (!lse_core_get_cb_info(env, info, (void**)&self, lse_node_interface_type, true, args, c_arraylen(args))) {
    return JS_UNDEFINED;
  }

  result = lse_node_set_text(
    lse_core_unbox_self(env, args[0]),
    lse_core_unbox_string(env, args[1])
  );

  return lse_core_box_bool(env, result);
}

void lse_core_exports(napi_env env, napi_value exports) {
  lse_function nsglobal[] = {
    { "bind", &nsglobal_bind },
  };
  lse_function nskeyboard[] = {
    { "getId", &nskeyboard_getId },
    { "getUUID", &nskeyboard_getUUID },
    { "getName", &nskeyboard_getName },
    { "isConnected", &nskeyboard_isConnected },
    { "getScancodeState", &nskeyboard_getScancodeState },
  };
  lse_function nsimage[] = {
    { "getWidth", &nsimage_getWidth },
    { "getHeight", &nsimage_getHeight },
    { "getState", &nsimage_getState },
  };
  lse_function nsfont[] = {
    { "getUri", &nsfont_getUri },
    { "getIndex", &nsfont_getIndex },
    { "getFamily", &nsfont_getFamily },
    { "getStyle", &nsfont_getStyle },
    { "getWeight", &nsfont_getWeight },
    { "getState", &nsfont_getState },
  };
  lse_function nsgamepad[] = {
    { "getId", &nsgamepad_getId },
    { "getUUID", &nsgamepad_getUUID },
    { "getName", &nsgamepad_getName },
    { "isConnected", &nsgamepad_isConnected },
    { "getButtonState", &nsgamepad_getButtonState },
    { "getAxisState", &nsgamepad_getAxisState },
    { "getHatState", &nsgamepad_getHatState },
    { "getAnalogState", &nsgamepad_getAnalogState },
    { "getKeyState", &nsgamepad_getKeyState },
    { "getButtonCount", &nsgamepad_getButtonCount },
    { "getAxisCount", &nsgamepad_getAxisCount },
    { "getHatCount", &nsgamepad_getHatCount },
  };
  lse_function nsnode[] = {
    { "getX", &nsnode_getX },
    { "getY", &nsnode_getY },
    { "getWidth", &nsnode_getWidth },
    { "getHeight", &nsnode_getHeight },
    { "appendChild", &nsnode_appendChild },
    { "insertChild", &nsnode_insertChild },
    { "removeChild", &nsnode_removeChild },
    { "getStyle", &nsnode_getStyle },
    { "destroy", &nsnode_destroy },
    { "setSource", &nsnode_setSource },
    { "setText", &nsnode_setText },
  };

  lse_core_export_namespace(env, exports, "nsglobal", nsglobal, c_arraylen(nsglobal));
  lse_core_export_namespace(env, exports, "nskeyboard", nskeyboard, c_arraylen(nskeyboard));
  lse_core_export_namespace(env, exports, "nsimage", nsimage, c_arraylen(nsimage));
  lse_core_export_namespace(env, exports, "nsfont", nsfont, c_arraylen(nsfont));
  lse_core_export_namespace(env, exports, "nsgamepad", nsgamepad, c_arraylen(nsgamepad));
  lse_core_export_namespace(env, exports, "nsnode", nsnode, c_arraylen(nsnode));
}
