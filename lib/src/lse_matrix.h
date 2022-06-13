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

typedef struct lse_matrix {
  float a, b, x; // row 1
  float c, d, y; // row 2
  //    0, 0, 1  // row 3
} lse_matrix;

const lse_matrix k_identity;

lse_matrix lse_matrix_init();
lse_matrix lse_matrix_init_rotate(float angle_rad);
lse_matrix lse_matrix_init_scale(float sx, float sy);
lse_matrix lse_matrix_init_translate(float tx, float ty);

float lse_matrix_get_axis_angle(const lse_matrix* m);
float lse_matrix_get_scale_x(const lse_matrix* m);
float lse_matrix_get_scale_y(const lse_matrix* m);
float lse_matrix_get_translate_x(const lse_matrix* m);
float lse_matrix_get_translate_y(const lse_matrix* m);
lse_matrix* lse_matrix_multiply(const lse_matrix* a, const lse_matrix* b, lse_matrix* out);
void lse_matrix_multiply_point(
    const lse_matrix* m,
    float point_x,
    float point_y,
    float* out_point_x,
    float* out_point_y);
