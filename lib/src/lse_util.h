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

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef NAN
static const uint32_t __nan = 0x7fc00000;
#define NAN (*(const float*)__nan)
#endif

#define LSE_UNDEFINED NAN
#define PI_F 3.1415926535897932384626433832795f

#define lse_min(a, b) (((a) < (b)) ? (a) : (b))
#define lse_max(a, b) (((a) > (b)) ? (a) : (b))
#define lse_snap_to_pixel_grid_i(VALUE) (int32_t) lse_snap_to_pixel_grid_f((VALUE))

typedef enum {
  LSE_ROUND = 0,
  LSE_ROUND_CEIL = 1,
  LSE_ROUND_FLOOR = 2
} lse_round;

bool lse_is_undefined(float value);

bool lse_equals_f(float a, float b);

float lse_snap_to_pixel_grid(float value, lse_round rounding);

float lse_snap_to_pixel_grid_f(float value);

float lse_clamp_f(float value, float lo, float hi);

const char* lse_ensure_string(const char* str);
