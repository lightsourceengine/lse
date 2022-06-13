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

#include <ctype.h>

static inline int c_strncasecmp(const char* s1, const char* s2, size_t nmax) {
  int ret = 0;
  while (nmax-- && (ret = tolower(*s1++) - tolower(*s2)) == 0 && *s2++)
    ;
  return ret;
}

#define lse_string_keyto(S) lse_string_as_cstring(*(S))
