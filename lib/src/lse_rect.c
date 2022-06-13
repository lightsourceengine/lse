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

#include "lse_rect.h"

#include "lse_util.h"
#include <math.h>

void lse_clip_image_rect(
    const lse_rect_f* box,
    const lse_rect_f* image_rect,
    float natural_image_width,
    float natural_image_height,
    lse_rect_f* out_rect,
    lse_rect* out_src) {
  const float ax2 = box->x + box->width;
  const float bx2 = image_rect->x + image_rect->width;
  const float ay2 = box->y + box->height;
  const float by2 = image_rect->y + image_rect->height;
  const float scaleX = natural_image_width / image_rect->width;
  const float scaleY = natural_image_height / image_rect->height;

  if (image_rect->x > box->x) {
    out_rect->x = image_rect->x;
    out_src->x = 0;
  } else {
    out_rect->x = box->x;
    out_src->x = (int32_t)lse_snap_to_pixel_grid(fabsf(box->x - image_rect->x) * scaleX, LSE_ROUND);
  }

  if (image_rect->y > box->y) {
    out_rect->y = image_rect->y;
    out_src->y = 0;
  } else {
    out_rect->y = box->y;
    out_src->y = (int32_t)lse_snap_to_pixel_grid(fabsf(box->y - image_rect->y) * scaleY, LSE_ROUND);
  }

  out_rect->width = (bx2 < ax2) ? bx2 - out_rect->x : ax2 - out_rect->x;
  out_rect->height = (by2 < ay2) ? by2 - out_rect->y : ay2 - out_rect->y;

  out_src->width = (int32_t)lse_snap_to_pixel_grid(out_rect->width * scaleX, LSE_ROUND);
  out_src->height = (int32_t)lse_snap_to_pixel_grid(out_rect->height * scaleY, LSE_ROUND);
}

bool lse_rect_is_empty(const lse_rect* rect) {
  return !rect || rect->width <= 0 || rect->height <= 0;
}

lse_rect lse_rect_intersect(const lse_rect* a, const lse_rect* b) {
  const int32_t x = (b->x > a->x) ? b->x : a->x;
  const int32_t y = (b->y > a->y) ? b->y : a->y;
  const int32_t ax2 = a->x + a->width;
  const int32_t bx2 = b->x + b->width;
  const int32_t ay2 = a->y + a->height;
  const int32_t by2 = b->y + b->height;

  return (lse_rect){
    x,
    y,
    (bx2 < ax2) ? bx2 - x : ax2 - x,
    (by2 < ay2) ? by2 - y : ay2 - y,
  };
}
