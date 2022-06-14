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
#include "string.h"

static lse_window_settings parse_window_settings(napi_env env, napi_value value);

JS_CALLBACK(set_title) {
  JS_METHOD_SIG(lse_window, 1)

  napi_value title_value = argv[0];

  switch (napix_type_of(env, title_value)) {
    case napi_string:
      lse_window_set_title(self, lse_core_unbox_s(env, title_value));
      break;
    case napi_undefined:
    case napi_null:
      lse_window_set_title(self, NULL);
      break;
    default:
      break;
  }

  return JS_UNDEFINED;
}

JS_CALLBACK(configure) {
  JS_METHOD_SIG(lse_window, 1)

  lse_window_settings settings = parse_window_settings(env, argv[0]);

  lse_window_configure(self, &settings);

  return JS_UNDEFINED;
}

JS_CALLBACK(get_root) {
  JS_METHOD_SIG_NO_ARGS(lse_window)

  lse_node* root = lse_window_get_root(self);

  lse_ref(root);

  return lse_core_class_new(env, lse_root_node_type, (lse_object*)root, &__args[0], 1);
}

JS_CALLBACK(get_width) {
  JS_METHOD_SIG_NO_ARGS(lse_window)
  return napix_create_int32(env, lse_window_get_width(self));
}

JS_CALLBACK(get_height) {
  JS_METHOD_SIG_NO_ARGS(lse_window)
  return napix_create_int32(env, lse_window_get_height(self));
}

JS_CALLBACK(get_refresh_rate) {
  JS_METHOD_SIG_NO_ARGS(lse_window)
  return napix_create_int32(env, lse_window_get_refresh_rate(self));
}

JS_CALLBACK(get_flags) {
  JS_METHOD_SIG_NO_ARGS(lse_window)
  return napix_create_uint32(env, lse_window_get_flags(self));
}

JS_CALLBACK(create_node) {
  JS_METHOD_SIG(lse_window, 1)
  char tag[12];
  lse_object* instance;

  if (!napix_read_string(env, argv[0], tag, c_arraylen(tag))) {
    return JS_UNDEFINED;
  }

  instance = (lse_object*)lse_window_create_node_from_tag(self, tag);

  if (!instance) {
    return lse_core_throw_error(env, LSE_ERR_OUT_OF_MEMORY);
  }

  return lse_core_class_new(env, lse_object_get_type(instance), instance, &__args[0], 1);
}

JS_CALLBACK(add_image) {
  JS_METHOD_SIG(lse_window, 1)
  return JS_UNDEFINED;
}

void lse_core_window_init(napi_env env, napi_value exports) {
  lse_namespace ns = lse_namespace_init(env, exports, JS_NAMESPACE_WINDOW);

  lse_add_function(&ns, JS_WINDOW_CONFIGURE, &configure);
  lse_add_function(&ns, JS_WINDOW_GET_ROOT, &get_root);
  lse_add_function(&ns, JS_WINDOW_SET_TITLE, &set_title);
  lse_add_function(&ns, JS_WINDOW_GET_WIDTH, &get_width);
  lse_add_function(&ns, JS_WINDOW_GET_HEIGHT, &get_height);
  lse_add_function(&ns, JS_WINDOW_GET_REFRESH_RATE, &get_refresh_rate);
  lse_add_function(&ns, JS_WINDOW_GET_FLAGS, &get_flags);
  lse_add_function(&ns, JS_WINDOW_CREATE_NODE, &create_node);
  lse_add_function(&ns, JS_WINDOW_ADD_IMAGE, &add_image);
}

static lse_window_settings parse_window_settings(napi_env env, napi_value value) {
  char buffer[32];
  lse_window_settings settings = { 0 };

  settings.width = napix_obj_get_i(env, value, "width", 0);
  settings.height = napix_obj_get_i(env, value, "height", 0);
  settings.display_index = napix_obj_get_i(env, value, "displayIndex", 0);
  settings.mode = LSE_WINDOW_MODE_FULLSCREEN;
  settings.fit = LSE_WINDOW_FIT_CLOSEST;

  if (napix_obj_read_cstring(env, value, "mode", buffer, 32)) {
    if (strcmp("fullscreen", buffer) == 0) {
      settings.mode = LSE_WINDOW_MODE_FULLSCREEN;
    } else if (strcmp("fullscreen-exclusive", buffer) == 0) {
      settings.mode = LSE_WINDOW_MODE_FULLSCREEN_EXCLUSIVE;
    } else if (strcmp("windowed", buffer) == 0) {
      settings.mode = LSE_WINDOW_MODE_WINDOWED;
    } else {
      // TODO: warn / error ?
    }
  }

  if (napix_obj_read_cstring(env, value, "fit", buffer, 32)) {
    if (strcmp("closest", buffer) == 0) {
      settings.fit = LSE_WINDOW_FIT_CLOSEST;
    } else if (strcmp("exact", buffer) == 0) {
      settings.fit = LSE_WINDOW_FIT_EXACT;
    } else {
      // TODO: warn / error ?
    }
  }

  return settings;
}
