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

#include "lse_matrix.h"
#include <math.h>

#define PI_F 3.14159265f

const lse_matrix k_identity = {
  1, 0, 0, 0, 1, 0,
};

lse_matrix lse_matrix_init() {
  return k_identity;
}

lse_matrix lse_matrix_init_rotate(float angle_rad) {
  float s = sinf(angle_rad);
  float c = cosf(angle_rad);

  return (lse_matrix){
    c, -s, 0, s, c, 0,
  };
}

lse_matrix lse_matrix_init_scale(float sx, float sy) {
  return (lse_matrix){
    sx, 0, 0, 0, sy, 0,
  };
}

lse_matrix lse_matrix_init_translate(float tx, float ty) {
  return (lse_matrix){
    1, 0, tx, 0, 1, ty,
  };
}

float lse_matrix_get_axis_angle(const lse_matrix* m) {
  // + 180 => (-360, 0]
  // negating atan2 params flips the range [0, 360)
  return atan2f(-m->c, -m->d) / PI_F * 180.f + 180.f;
}

float lse_matrix_get_scale_x(const lse_matrix* m) {
  return sqrtf(m->a * m->a + m->c * m->c);
}

float lse_matrix_get_scale_y(const lse_matrix* m) {
  return sqrtf(m->b * m->b + m->d * m->d);
}

float lse_matrix_get_translate_x(const lse_matrix* m) {
  return m->x;
}

float lse_matrix_get_translate_y(const lse_matrix* m) {
  return m->y;
}

lse_matrix* lse_matrix_multiply(const lse_matrix* lhs, const lse_matrix* rhs, lse_matrix* out) {
  *out = (lse_matrix){
    lhs->a * rhs->a + lhs->b * rhs->c, lhs->a * rhs->b + lhs->b * rhs->d, lhs->a * rhs->x + lhs->b * rhs->y + lhs->x,
    lhs->c * rhs->a + lhs->d * rhs->c, lhs->c * rhs->b + lhs->d * rhs->d, lhs->c * rhs->x + lhs->d * rhs->y + lhs->y,
  };

  return out;
}

void lse_matrix_multiply_point(
    const lse_matrix* m,
    float point_x,
    float point_y,
    float* out_point_x,
    float* out_point_y) {
  *out_point_x = point_x * m->a + point_y * m->c;
  *out_point_y = point_x * m->b + point_y * m->d;
}
