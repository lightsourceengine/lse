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

#include "../include/nanoctx.h"

#include <stdio.h>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

typedef struct nctx_ {
  int32_t width;
  int32_t height;
  int32_t stride;
  uint8_t* surface;
  NSVGparser* parser;
  NSVGrasterizer* rasterizer;
  // color format: 0xAABBGGRR
  uint32_t fill_color;
  float fill_opacity;
  uint32_t stroke_color;
  float stroke_opacity;
  float stroke_width;
  bool is_path_closed;
} nctx_;

static void nctx_flush(nctx ctx);
static float clamp_f(float value, float lo, float hi);

static const float KAPPA = 1.f - 0.5522847493f;

nctx nctx_new() {
  nctx ctx = calloc(1, sizeof(nctx_));

  if (!ctx) {
    return NULL;
  }

  ctx->rasterizer = nsvgCreateRasterizer();

  if (!ctx->rasterizer) {
    nctx_delete(ctx);
    return NULL;
  }

  nctx_reset_style(ctx);

  return ctx;
}

void nctx_delete(nctx ctx) {
  if (ctx) {
    nsvg__deleteParser(ctx->parser);
    nsvgDeleteRasterizer(ctx->rasterizer);
    free(ctx);
  }
}

bool nctx_set_surface(nctx ctx, uint8_t* surface, int32_t width, int32_t height, int32_t stride) {
  if (!surface || width <= 0 || height <= 0) {
    return false;
  }

  ctx->surface = surface;
  ctx->width = width;
  ctx->height = height;
  ctx->stride = stride;

  return true;
}

bool nctx_begin_path(nctx ctx) {
  if (!ctx || ctx->parser || !ctx->surface) {
    return false;
  }

  NSVGparser* parser = nsvg__createParser();

  if (!parser) {
    return false;
  }

  parser->image->width = (float)ctx->width;
  parser->image->height = (float)ctx->height;
  parser->dpi = 96;
  nsvg__pushAttr(parser);

  ctx->parser = parser;
  ctx->is_path_closed = false;

  return true;
}

void nctx_close_path(nctx ctx) {
  nsvg__addPath(ctx->parser, 1);

  ctx->is_path_closed = true;
}

void nctx_move_to(nctx ctx, float x, float y) {
  nsvg__moveTo(ctx->parser, x, y);
}

void nctx_line_to(nctx ctx, float x, float y) {
  nsvg__lineTo(ctx->parser, x, y);
}

void nctx_cubic_bezier_to(nctx ctx, float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) {
  nsvg__cubicBezTo(ctx->parser, cp1x, cp1y, cp2x, cp2y, x, y);
}

void nctx_fill(nctx ctx) {
  if (!ctx->parser || !ctx->surface) {
    return;
  }

  nsvg__getAttr(ctx->parser)->hasFill = 1;
  nsvg__getAttr(ctx->parser)->fillColor = ctx->fill_color;
  nsvg__getAttr(ctx->parser)->fillOpacity = ctx->fill_opacity;
  nsvg__getAttr(ctx->parser)->hasStroke = 0;

  nsvg__addShape(ctx->parser);

  nctx_flush(ctx);
}

void nctx_stroke(nctx ctx) {
  if (!ctx->parser || !ctx->surface) {
    return;
  }

  nsvg__getAttr(ctx->parser)->hasStroke = 1;
  nsvg__getAttr(ctx->parser)->strokeColor = ctx->stroke_color;
  nsvg__getAttr(ctx->parser)->strokeWidth = ctx->stroke_width;
  nsvg__getAttr(ctx->parser)->strokeOpacity = ctx->stroke_opacity;
  nsvg__getAttr(ctx->parser)->hasFill = 0;

  nsvg__addShape(ctx->parser);

  nctx_flush(ctx);
}

void nctx_render_shape(nctx ctx) {
  if (!ctx->parser || !ctx->surface) {
    return;
  }

  NSVGattrib* attr = nsvg__getAttr(ctx->parser);

  if (ctx->stroke_width > 0 && ctx->stroke_opacity > 0) {
    attr->hasStroke = 1;
    attr->strokeColor = ctx->stroke_color;
    attr->strokeWidth = ctx->stroke_width;
    attr->strokeOpacity = ctx->stroke_opacity;
  } else {
    attr->hasStroke = 0;
  }

  if (ctx->fill_opacity > 0) {
    attr->hasFill = 1;
    attr->fillColor = ctx->fill_color;
    attr->fillOpacity = ctx->fill_opacity;
  } else {
    attr->hasFill = 0;
  }

  nsvg__addShape(ctx->parser);

  nctx_flush(ctx);
}

void nctx_rounded_rect(nctx ctx, float x, float y, float w, float h, float tl, float tr, float bl, float br) {
  if (!nctx_begin_path(ctx)) {
    return;
  }

  nctx_move_to(ctx, x + tl, y);
  nctx_line_to(ctx, x + w - tr, y);
  nctx_cubic_bezier_to(ctx, x + w - tr * KAPPA, y, x + w, y + tl * KAPPA, x + w, y + tr);
  nctx_line_to(ctx, x + w, y + h - br);
  nctx_cubic_bezier_to(ctx, x + w, y + h - br * KAPPA, x + w - tr * KAPPA, y + h, x + w - br, y + h);
  nctx_line_to(ctx, x + bl, y + h);
  nctx_cubic_bezier_to(ctx, x + bl * KAPPA, y + h, x, y + h - bl * KAPPA, x, y + h - bl);
  nctx_line_to(ctx, x, y + tl);
  nctx_cubic_bezier_to(ctx, x, y + tl * KAPPA, x + tl * KAPPA, y, x + tl, y);

  nctx_close_path(ctx);
}

void nctx_reset_style(nctx ctx) {
  ctx->fill_color = 0xFFFFFFFF;
  ctx->fill_opacity = 1.f;
  ctx->stroke_color = 0xFFFFFFFF;
  ctx->stroke_opacity = 1.f;
  ctx->stroke_width = 1;
}

void nctx_set_fill_color(nctx ctx, uint32_t color) {
  ctx->fill_color = color;
}

void nctx_set_fill_opacity(nctx ctx, float opacity) {
  ctx->fill_opacity = clamp_f(opacity, 0, 1);
}

void nctx_set_stroke_opacity(nctx ctx, float opacity) {
  ctx->stroke_opacity = clamp_f(opacity, 0, 1);
}

void nctx_set_stroke_color(nctx ctx, uint32_t color) {
  ctx->stroke_color = color;
}

void nctx_set_stroke_width(nctx ctx, float width) {
  ctx->stroke_width = width;
}

static void nctx_flush(nctx ctx) {
  if (!ctx->is_path_closed) {
    nctx_close_path(ctx);
  }

  // NOTE: shapes don't have a viewbox, so not necessary
  // nsvg__scaleToViewbox(ctx->parser, "px");

  nsvgRasterizeFull(
      ctx->rasterizer, ctx->parser->image, 0, 0, 1, 1, ctx->surface, ctx->width, ctx->height, ctx->stride);

  if (ctx->parser) {
    nsvg__deleteParser(ctx->parser);
    ctx->parser = NULL;
  }
}

static float clamp_f(float value, float lo, float hi) {
  return (value < lo) ? lo : (hi < value) ? hi : value;
}
