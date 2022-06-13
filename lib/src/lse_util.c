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

#include "lse_util.h"

#include "Yoga.h"
#include <math.h>

// TODO: make configurable (?)
#define LSE_POINT_SCALE_FACTOR 1.f

bool lse_is_undefined(float value) {
  return YGFloatIsUndefined(value);
}

bool lse_equals_f(float a, float b) {
  if (!lse_is_undefined(a) && !lse_is_undefined(b)) {
    return fabsf(a - b) < 0.0001f;
  }
  return lse_is_undefined(a) && lse_is_undefined(b);
}

float lse_snap_to_pixel_grid(float value, lse_round rounding) {
  float scaledValue = value * LSE_POINT_SCALE_FACTOR;
  // We want to calculate `fractial` such that `floor(scaledValue) = scaledValue
  // - fractial`.
  float fractial = fmodf(scaledValue, 1.0f);
  if (fractial < 0) {
    // This branch is for handling negative numbers for `value`.
    //
    // Regarding `floor` and `ceil`. Note that for a number x, `floor(x) <= x <=
    // ceil(x)` even for negative numbers. Here are a couple of examples:
    //   - x =  2.2: floor( 2.2) =  2, ceil( 2.2) =  3
    //   - x = -2.2: floor(-2.2) = -3, ceil(-2.2) = -2
    //
    // Regarding `fmodf`. For fractional negative numbers, `fmodf` returns a
    // negative number. For example, `fmodf(-2.2) = -0.2`. However, we want
    // `fractial` to be the number such that subtracting it from `value` will
    // give us `floor(value)`. In the case of negative numbers, adding 1 to
    // `fmodf(value)` gives us this. Let's continue the example from above:
    //   - fractial = fmodf(-2.2) = -0.2
    //   - Add 1 to the fraction: fractial2 = fractial + 1 = -0.2 + 1 = 0.8
    //   - Finding the `floor`: -2.2 - fractial2 = -2.2 - 0.8 = -3
    ++fractial;
  }

  switch (rounding) {
    case LSE_ROUND_FLOOR:
      scaledValue = scaledValue - fractial;
      break;
    case LSE_ROUND_CEIL:
      scaledValue = scaledValue - fractial + 1.0f;
      break;
    default:
      scaledValue = scaledValue - fractial + (!lse_is_undefined(fractial) && (fractial > 0.4901f) ? 1.0f : 0.0f);
      break;
  }

  return lse_is_undefined(scaledValue) ? LSE_UNDEFINED : scaledValue / LSE_POINT_SCALE_FACTOR;
}

float lse_snap_to_pixel_grid_f(float value) {
  float fractial = fmodf(value, 1.0f);

  if (fractial < 0) {
    ++fractial;
  }

  return value - fractial + ((fractial > 0.4901f) ? 1.0f : 0.0f);
}

float lse_clamp_f(float value, float lo, float hi) {
  assert(!(hi < lo));
  return (value < lo) ? lo : (hi < value) ? hi : value;
}

const char* lse_ensure_string(const char* str) {
  return str ? str : "";
}
