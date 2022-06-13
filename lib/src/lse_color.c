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

#include "lse_color.h"

#include <assert.h>
#include <stdbool.h>

void lse_color_to_format(lse_color* pixels, int32_t len, lse_color_format target_format) {
  switch (target_format) {
    case LSE_COLOR_FORMAT_ARGB:
      // TODO: implement
      assert(false && LSE_COLOR_FORMAT_ARGB);
      break;
    case LSE_COLOR_FORMAT_BGRA:
      // TODO: implement
      assert(false && LSE_COLOR_FORMAT_BGRA);
      break;
    case LSE_COLOR_FORMAT_ABGR:
      for (int32_t i = 0; i < len; i++) {
        pixels[i].value =
            (pixels[i].comp.r << 24) | (pixels[i].comp.g << 16) | (pixels[i].comp.b << 8) | (pixels[i].comp.a << 0);
      }
      break;
    default:
      // LSE_COLOR_FORMAT_RGBA - no op
      break;
  }
}
