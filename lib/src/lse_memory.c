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

#include "lse_memory.h"

#include <lse.h>

void* lse_malloc(size_t size) {
  void* mem = malloc(size);

  if (!mem) {
    LSE_LOG_ERROR("for alloc: %lu bytes", size);
    exit(1);
  }

  return mem;
}

void* lse_calloc(size_t count, size_t size) {
  void* mem = calloc(count, size);

  if (!mem) {
    LSE_LOG_ERROR("for alloc: %lu bytes", size);
    exit(1);
  }

  return mem;
}

void* lse_realloc(void* mem, size_t new_size) {
  void* new_mem = realloc(mem, new_size);

  if (!new_mem) {
    LSE_LOG_ERROR("for alloc: %lu bytes", new_size);
    exit(1);
  }

  return new_mem;
}
