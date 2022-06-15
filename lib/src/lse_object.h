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
#include <stddef.h>

typedef void (*lse_object_constructor)(lse_object*, void*);
typedef void (*lse_object_destructor)(lse_object*);

typedef struct {
  uint8_t type;
  const char* name;
  size_t size;
  lse_object_constructor constructor;
  lse_object_destructor destructor;
  uint32_t interface_type;
  void* interface;
} lse_type_info;

// create a new object by type. returns null if alloc fails
lse_object* lse_object_new(uint8_t type, void* constructor_arg);

// create a new object with a dynamic allocation. special case (hack) for lse_string. returns null if alloc fails
lse_object* lse_object_new_with_size(uint8_t type, size_t size, void* constructor_arg);

// get an objects interface/vtable. returns null if object does not support interfaces
void* lse_object_get_interface(lse_object* object);

// get the interface/vtable type of object. lse_none_interface_type returned if object has no interface
uint8_t lse_object_get_interface_type(lse_object* object);

#define LSE_TYPE_INFO(ID, CLASS, INTERFACE_ID, INTERFACE)                                                              \
  const lse_type_info k_##CLASS##_type_info = {                                                                        \
    .type = (ID),                                                                                                      \
    .name = #CLASS,                                                                                                    \
    .size = sizeof(struct CLASS),                                                                                      \
    .constructor = constructor,                                                                                        \
    .destructor = destructor,                                                                                          \
    .interface = (INTERFACE),                                                                                          \
    .interface_type = (INTERFACE_ID),                                                                                  \
  }

#define lse_new(CLASS, ARG) (CLASS*)lse_object_new(CLASS##_type, (ARG))
