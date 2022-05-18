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

#include <stdbool.h>
#include <stdint.h>

typedef struct nctx_* nctx;

nctx nctx_new();
void nctx_delete(nctx ctx);

bool nctx_set_surface(nctx ctx, uint8_t* surface, int32_t width, int32_t height, int32_t stride);
bool nctx_begin_path(nctx ctx);
void nctx_close_path(nctx ctx);

void nctx_move_to(nctx ctx, float x, float y);
void nctx_line_to(nctx ctx, float x, float y);
void nctx_cubic_bezier_to(nctx ctx, float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);

void nctx_fill(nctx ctx);
void nctx_stroke(nctx ctx);
void nctx_render_shape(nctx ctx);

void nctx_rounded_rect(
    nctx ctx,
    float x,
    float y,
    float width,
    float height,
    float top_left,
    float top_right,
    float bottom_left,
    float bottom_right);

void nctx_reset_style(nctx ctx);
void nctx_set_fill_color(nctx ctx, uint32_t color);
void nctx_set_fill_opacity(nctx ctx, float opacity);
void nctx_set_stroke_opacity(nctx ctx, float opacity);
void nctx_set_stroke_color(nctx ctx, uint32_t color);
void nctx_set_stroke_width(nctx ctx, float color);

void nctx_render(nctx ctx, uint8_t* data, int32_t width, int32_t height, int32_t stride);
