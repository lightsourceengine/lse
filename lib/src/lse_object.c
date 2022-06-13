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

#include "lse_object.h"

#include "lse_memory.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
  unsigned int type : 8;
  unsigned int ref_count : 24;
} lse_object_header;

static bool s_initialized = false;
static lse_type_info s_types[256] = { 0 };

static void register_types();

LSE_API uint8_t LSE_CDECL lse_object_get_type(lse_object* object) {
  if (object == NULL) {
    return lse_none_type;
  }

  lse_object_header* header = ((lse_object_header*)object) - 1;

  return header->type;
}

void* lse_object_get_interface(lse_object* object) {
  assert(object != NULL);

  lse_object_header* header = ((lse_object_header*)object) - 1;

  return s_types[header->type].interface;
}

uint8_t lse_object_get_interface_type(lse_object* object) {
  if (object == NULL) {
    return lse_none_interface_type;
  }

  lse_object_header* header = ((lse_object_header*)object) - 1;

  return s_types[header->type].interface_type;
}

lse_object* lse_object_new(uint8_t type, void* constructor_arg) {
  // 0 means use size in type info
  return lse_object_new_with_size(type, 0, constructor_arg);
}

lse_object* lse_object_new_with_size(uint8_t type, size_t size, void* constructor_arg) {
  if (!s_initialized) {
    register_types();
    s_initialized = true;
  }

  if (s_types[type].type == lse_none_type) {
    assert(false || "type not registered");
    return NULL;
  }

  lse_type_info* type_info = &s_types[type];
  lse_object_header* header = lse_calloc(1, size > 0 ? size : type_info->size + sizeof(lse_object_header));
  lse_object* object = (lse_object*)(header + 1);

  LSE_LOG_TRACE("alloc %s", type_info->name);

  header->type = type;
  header->ref_count = 1;
  type_info->constructor(object, constructor_arg);

  return object;
}

LSE_API uint32_t LSE_CDECL lse_object_ref(lse_object* object) {
  if (!object) {
    return 0U;
  }

  lse_object_header* header = ((lse_object_header*)object) - 1;

  return ++header->ref_count;
}

LSE_API uint32_t LSE_CDECL lse_object_get_ref_count(lse_object* object) {
  if (!object) {
    return 0U;
  }

  lse_object_header* header = ((lse_object_header*)object) - 1;

  return header->ref_count;
}

LSE_API void LSE_CDECL lse_object_unref(lse_object* object) {
  if (!object) {
    return;
  }

  lse_object_header* header = ((lse_object_header*)object) - 1;

  if (--header->ref_count == 0U) {
    lse_object_destructor destructor = s_types[header->type].destructor;

    if (destructor) {
      destructor(object);
    }

    LSE_LOG_TRACE("free %s", s_types[header->type].name);

    free(header);
  }
}

LSE_API const char* LSE_CDECL lse_object_get_type_name(lse_object* object) {
  if (!object) {
    return "null";
  }

  lse_object_header* header = ((lse_object_header*)object) - 1;

  return s_types[header->type].name;
}

static void register_types() {
#define REGISTER(CLASS)                                                                                                \
  extern const lse_type_info k_lse_##CLASS##_type_info;                                                                \
  assert(s_types[k_lse_##CLASS##_type_info.type].type == 0 && "type already registered");                              \
  s_types[k_lse_##CLASS##_type_info.type] = k_lse_##CLASS##_type_info

  REGISTER(array);
  REGISTER(box_node);
  REGISTER(env);
  REGISTER(font);
  REGISTER(font_store);
  REGISTER(image);
  REGISTER(image_node);
  REGISTER(image_store);
  REGISTER(root_node);
  REGISTER(string);
  REGISTER(style);
  REGISTER(text_node);
  REGISTER(window);

  REGISTER(sdl_gamepad);
  REGISTER(sdl_graphics);
  REGISTER(sdl_graphics_container);
  REGISTER(sdl_keyboard);
  REGISTER(sdl_video);

  REGISTER(mock_gamepad);
  REGISTER(mock_graphics);
  REGISTER(mock_graphics_container);
  REGISTER(mock_keyboard);
  REGISTER(mock_video);

#undef REGISTER
}
