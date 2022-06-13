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

#include "lse_array.h"

#include "lse_object.h"
#include <string.h>

struct lse_array {
  size_t length;
  size_t item_size;
  void (*item_drop)(void*);
};

static lse_array* k_empty_array = NULL;

static uint8_t* get_data(lse_array* self);

void constructor(lse_object* object, void* arg) {
  lse_array* self = (lse_array*)object;
  lse_array* spec = arg;

  *self = *spec;
}

void destructor(lse_object* object) {
  lse_array* self = (lse_array*)object;
  size_t length = self->length;
  lse_array_item_drop drop = self->item_drop;

  if (drop) {
    for (size_t i = 0; i < length; i++) {
      drop(lse_array_at(self, i));
    }
  }
}

LSE_API lse_array* lse_array_new(size_t length, size_t item_size, lse_array_item_drop drop) {
  if (length == 0 || item_size == 0) {
    return lse_array_new_empty();
  }

  return (lse_array*)lse_object_new_with_size(
      lse_array_type,
      sizeof(lse_array) + (length * item_size),
      &(lse_array){
          .item_size = item_size,
          .item_drop = drop,
          .length = length,
      });
}

LSE_API lse_array* lse_array_new_empty() {
  if (!k_empty_array) {
    k_empty_array = (lse_array*)lse_object_new_with_size(
        lse_array_type,
        sizeof(lse_array),
        &(lse_array){
            .item_size = 0,
            .item_drop = 0,
            .length = 0,
        });
  }

  lse_ref(k_empty_array);
  return k_empty_array;
}

LSE_API size_t lse_array_get_length(lse_array* array) {
  return array->length;
}

LSE_API void* lse_array_at(lse_array* array, size_t i) {
  return get_data(array) + (i * array->item_size);
}

LSE_API void* lse_array_get(lse_array* array, size_t i) {
  return i < array->length ? lse_array_at(array, i) : NULL;
}

LSE_API bool lse_array_set(lse_array* array, size_t i, void* item) {
  void* slot = lse_array_get(array, i);

  if (slot) {
    memcpy(slot, item, array->item_size);
    return true;
  }

  return false;
}

static uint8_t* get_data(lse_array* self) {
  return (uint8_t*)(self + 1);
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

const lse_type_info k_lse_array_type_info = (lse_type_info){ .type = lse_array_type,
                                                             .name = "lse_array",
                                                             .size = 0,
                                                             .constructor = constructor,
                                                             .destructor = destructor,
                                                             .interface = NULL,
                                                             .interface_type = (lse_none_interface_type) };
