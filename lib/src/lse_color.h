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

#include "lse_cfg.h"
#include <stdint.h>

#define LSE_COLOR_INIT(R, G, B, A)                                                                                     \
  {                                                                                                                    \
    { .a = A, .b = B, .g = G, .r = R }                                                                                 \
  }
#define LSE_COLOR_MAKE(R, G, B, A) (lse_color) LSE_COLOR_INIT(R, G, B, A)

typedef union lse_color {
  struct {
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
  } comp;
  uint32_t value;
} lse_color;

typedef enum lse_color_format_ {
  /* 4 channel, 32 bit, RGBA */
  LSE_COLOR_FORMAT_RGBA,
  /* 4 channel, 32 bit, ARGB */
  LSE_COLOR_FORMAT_ARGB,
  /* 4 channel, 32 bit, ABGR */
  LSE_COLOR_FORMAT_ABGR,
  /* 4 channel, 32 bit, BGRA */
  LSE_COLOR_FORMAT_BGRA,
  /* 1 channel, 8 bit, alpha */
  LSE_COLOR_FORMAT_ALPHA,
  /* unknown pixel format */
  LSE_COLOR_FORMAT_UNKNOWN
} lse_color_format;

// converts 32-bit RGBA pixels to the desired (32-bit) color format
void lse_color_to_format(lse_color* pixels, int32_t len, lse_color_format target_format);
