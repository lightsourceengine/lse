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

#include "lse_color.h"
#include "lse_types.h"
#include <lse.h>

typedef void (*lse_image_pixels_free)(lse_color*);

/**
 * Move image to the LOADING state.
 */
void lse_image_set_loading(lse_image* image);

/**
 * Set image data and move image to the READY state.
 */
void lse_image_set_ready(
    lse_image* image,
    lse_color* pixels,
    lse_image_pixels_free pixels_free,
    int32_t width,
    int32_t height,
    lse_color_format format);

/**
 * Move image to the ERROR state.
 */
void lse_image_set_error(lse_image* image);

void lse_image_destroy(lse_image* image);

lse_color* lse_image_get_pixels(lse_image* image);

void lse_image_release_pixels(lse_image* image);

bool lse_image_is_ready(lse_image* image);

bool lse_image_can_render(lse_image* image);
