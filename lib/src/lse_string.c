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

#include "lse_string.h"

#include <lse.h>
#include "lse_object.h"
#include <memory.h>
#include <stddef.h>

/* optimal memory: based on malloc_usable_size() sequence: 24, 40, 56, ... */
#define lse_string_opt_mem(cap) ((((offsetof(lse_string_rep_t, str) + (cap) + 8) >> 4) << 4) + 8)

struct lse_string {};

typedef struct {
  char* str;
  size_t size;
  char fill;
} string_info;

typedef struct {
  size_t size;
  char str[];
} lse_string_rep_t;

static lse_string* kEmptyString = NULL;

static void constructor(lse_object* object, void* arg) {
  string_info* info = (string_info*)arg;
  lse_string_rep_t* rep = (lse_string_rep_t*)object;

  if (info->size > 0) {
    if (info->fill) {
      memset(rep->str, info->fill, info->size);
    } else {
      memmove(rep->str, info->str, info->size);
    }
  }

  rep->str[rep->size = info->size] = '\0';
}

static void destructor(lse_object* object) {
}

static lse_string* new_from_info(string_info* info) {
  return (lse_string*)lse_object_new_with_size(lse_string_type, lse_string_opt_mem(info->size), info);
}

static lse_string* get_empty_string() {
  if (!kEmptyString) {
    kEmptyString = new_from_info(&(string_info){ .str = "", .fill = 0, .size = 0 });
  }

  lse_ref(kEmptyString);

  return kEmptyString;
}

static lse_string* lse_string_new_internal(string_info* info) {
  return info->size == 0 ? get_empty_string() : new_from_info(info);
}

LSE_API lse_string* LSE_CDECL lse_string_new(const char* str) {
  if (!str) {
    return NULL;
  }

  return lse_string_new_internal(&(string_info){ .str = (char*)str, .fill = 0, .size = strlen(str) });
}

LSE_API lse_string* LSE_CDECL lse_string_new_with_size(char fill, size_t n) {
  return lse_string_new_internal(&(string_info){ .str = NULL, .fill = fill, .size = n });
}

LSE_API lse_string* LSE_CDECL lse_string_new_empty() {
  return get_empty_string();
}

LSE_API const char* LSE_CDECL lse_string_as_cstring(lse_string* str) {
  return str ? ((lse_string_rep_t*)str)->str : "";
}

LSE_API char* LSE_CDECL lse_string_as_data(lse_string* str) {
  return ((lse_string_rep_t*)str)->str;
}

LSE_API size_t LSE_CDECL lse_string_size(lse_string* str) {
  return ((lse_string_rep_t*)str)->size;
}

LSE_API bool LSE_CDECL lse_string_empty(lse_string* str) {
  return !str || ((lse_string_rep_t*)str)->size == 0;
}

LSE_API int32_t LSE_CDECL lse_string_cmp(lse_string* s1, lse_string* s2) {
  return strcmp(((lse_string_rep_t*)s1)->str, ((lse_string_rep_t*)s2)->str);
}

LSE_API int32_t LSE_CDECL lse_string_case_cmp(lse_string* s1, lse_string* s2) {
  int32_t ret;
  const char* c1 = ((lse_string_rep_t*)s1)->str;
  const char* c2 = ((lse_string_rep_t*)s2)->str;

  while ((ret = tolower(*c1++) - tolower(*c2)) == 0 && *c2++) {
  }

  return ret;
}

LSE_TYPE_INFO(lse_string_type, lse_string, lse_none_interface_type, NULL);
