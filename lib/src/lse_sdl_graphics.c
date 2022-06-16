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

#include "lse_graphics.h"

typedef struct SDL_Texture* sdl_texture_ptr;
#define i_tag image_cache
#define i_key lse_image_ptr
#define i_val sdl_texture_ptr
#define i_opt c_no_clone
#include <stc/cmap.h>

#include "lse_color.h"
#include "lse_env.h"
#include "lse_font.h"
#include "lse_image.h"
#include "lse_memory.h"
#include "lse_object.h"
#include "lse_rect.h"
#include "lse_sdl.h"
#include "lse_text.h"
#include "lse_util.h"
#include "nanoctx.h"

typedef struct lse_sdl_graphics lse_sdl_graphics;
typedef struct sdl_render_object sdl_render_object;

struct lse_sdl_graphics {
  lse_graphics_base base;
  SDL_Renderer* renderer;
  SDL_Texture* fill_texture;
  bool use_float_rects;
  cmap_image_cache image_cache;
};

struct sdl_render_object {
  SDL_Texture* texture;
  lse_image* image;
  lse_rect_f rect;
  SDL_Rect src_rect;
  bool has_src_rect;
  bool can_tint;
};

static lse_render_object* sdl_render_object_drop(sdl_render_object* current, lse_sdl_graphics* sdl_graphics);
static sdl_render_object* sdl_render_object_init(sdl_render_object* current, lse_sdl_graphics* sdl_graphics);
static sdl_render_object* sdl_render_object_init_as_target(
    sdl_render_object* current,
    lse_sdl_graphics* sdl_graphics,
    int32_t texture_access,
    int32_t width,
    int32_t height);

static lse_render_object*
create_fill_rect_render_object(lse_graphics* graphics, lse_render_command* command, sdl_render_object* sro);
static lse_render_object*
create_draw_image_render_object(lse_graphics* graphics, lse_render_command* command, sdl_render_object* sro);
static lse_render_object*
create_rounded_rect_render_object(lse_graphics* graphics, lse_render_command* command, sdl_render_object* sro);
static lse_render_object*
create_draw_text_render_object(lse_graphics* graphics, lse_render_command* command, sdl_render_object* sro);

static void render_fill_rect(lse_sdl_graphics* self, lse_render_command* command);
static void render_stroke_rect(lse_sdl_graphics* self, lse_render_command* command);
static void render_texture(lse_sdl_graphics* self, lse_render_command* command);

static bool sw_render_rounded_rect(lse_sdl_graphics* self, lse_render_command* command, SDL_Texture* target);
static bool sw_render_text(
    lse_sdl_graphics* self,
    lse_render_command* command,
    SDL_Texture* target,
    int32_t target_width,
    int32_t target_height);
static void
sw_render_text_line(lse_surface* dest, float x, float y, lse_text_style* text_style, lse_text_line_info* metrics);
static void sw_copy_alpha_buffer(int32_t x, int32_t y, const lse_glyph_surface* glyph, const lse_surface* dest);

static void cmap_image_cache_release(lse_sdl* sdl, cmap_image_cache_value* entry, bool destroy_texture);
static SDL_Texture* get_texture(lse_sdl_graphics* self, lse_image* image);

static SDL_Texture*
ensure_texture(lse_sdl_graphics* self, SDL_Texture* texture, int32_t access, int32_t width, int32_t height);
static bool has_texture_format(lse_sdl* sdl, SDL_Renderer* renderer, Uint32 desired_format);

#define INITIAL_IMAGE_CACHE_CAPACITY 32
#define LSE_TEXTURE_FORMAT LSE_COLOR_FORMAT_RGBA
#define LSE_SDL_TEXTURE_FORMAT SDL_PIXELFORMAT_RGBA32
const static SDL_FPoint k_empty_sdl_fpoint = { 0 };
const static SDL_Point k_empty_sdl_point = { 0 };

#define MAKE_NSVG_RGB(R, G, B)                                                                                         \
  (((unsigned int)(R)) | ((unsigned int)(G) << 8) | ((unsigned int)(B) << 16) | (0xFF << 24))

// @override
static void constructor(lse_object* object, void* arg) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)object;
  lse_graphics* graphics = (lse_graphics*)object;

  lse_graphics_base_constructor(graphics, arg);
  self->image_cache = cmap_image_cache_with_capacity(INITIAL_IMAGE_CACHE_CAPACITY);
}

// @override
static void destructor(lse_object* object) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)object;
  lse_graphics* graphics = (lse_graphics*)object;

  lse_graphics_base_destructor(graphics);
  cmap_image_cache_drop(&self->image_cache);
}

// @override
static lse_status configure(lse_graphics* graphics, void* arg) {
  if (lse_graphics_is_destroyed(graphics)) {
    return LSE_ERR_EOL;
  }

  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  lse_sdl* sdl = lse_get_sdl_from_base(self);
  SDL_Window* window = arg;
  SDL_Renderer* renderer = NULL;
  SDL_Texture* fill_texture = NULL;

  if (self->renderer) {
    return LSE_ERR_ALREADY_CONFIGURED;
  }

  //
  // create renderer
  //

  renderer = sdl->SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

  if (!renderer) {
    LSE_LOG_SDL_ERROR(sdl, "SDL_CreateRenderer");
    goto ERROR;
  }

  sdl->SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  //
  // check the texture format
  //

  if (!has_texture_format(sdl, renderer, LSE_SDL_TEXTURE_FORMAT)) {
    LSE_LOG_ERROR("renderer does not support texture format: %s", sdl->SDL_GetPixelFormatName(LSE_SDL_TEXTURE_FORMAT));
    goto ERROR;
  }

  LSE_LOG_INFO("texture format: %s", sdl->SDL_GetPixelFormatName(LSE_SDL_TEXTURE_FORMAT));

  //
  // get actual renderable area
  //

  if (sdl->SDL_GetRendererOutputSize(renderer, &self->base.width, &self->base.height) != 0) {
    LSE_LOG_SDL_ERROR(sdl, "SDL_GetRendererOutputSize");
    goto ERROR;
  }

  //
  // SDL fill rect does not support rotation. The workaround is to create a 1x1 texture filled with white. The
  // fill color can be set with a texture tint color and the texture can be scaled and rotated with RenderCopyEx.
  //

  self->renderer = renderer;

  fill_texture = ensure_texture(self, NULL, SDL_TEXTUREACCESS_STATIC, 1, 1);

  if (!fill_texture) {
    LSE_LOG_SDL_ERROR(sdl, "SDL_CreateTexture");
    goto ERROR;
  }

  if (sdl->SDL_UpdateTexture(fill_texture, NULL, (uint8_t*)&(uint32_t){ 0xFFFFFFFF }, 4) != 0) {
    LSE_LOG_SDL_ERROR(sdl, "SDL_UpdateTexture");
    goto ERROR;
  }

  self->fill_texture = fill_texture;
  self->use_float_rects = sdl->SDL_RenderCopyF != NULL;

  return LSE_OK;

ERROR:
  if (fill_texture) {
    sdl->SDL_DestroyTexture(fill_texture);
  }

  if (renderer) {
    sdl->SDL_DestroyRenderer(renderer);
    self->renderer = NULL;
  }

  return LSE_ERR_GFX_CREATE_RENDERER;
}

// @override
static void destroy(lse_graphics* graphics) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  lse_sdl* sdl = lse_get_sdl_from_base(self);

  if (lse_graphics_is_destroyed(graphics)) {
    return;
  }

  c_foreach(i, cmap_image_cache, self->image_cache) {
    cmap_image_cache_release(sdl, i.ref, self->renderer != NULL);
  }
  cmap_image_cache_clear(&self->image_cache);

  if (self->renderer) {
    sdl->SDL_DestroyTexture(self->fill_texture);
    self->fill_texture = NULL;

    sdl->SDL_DestroyRenderer(self->renderer);
    self->renderer = NULL;
  }

  lse_graphics_base_destroy(graphics);
}

// @override
static bool is_destroyed(lse_graphics* graphics) {
  return lse_graphics_base_is_destroyed(graphics);
}

// @override
static void push_state(lse_graphics* graphics) {
  lse_graphics_base_push_state(graphics);
}

// @override
static void pop_state(lse_graphics* graphics) {
  bool had_clip_rect;
  const lse_graphics_state* current;
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  lse_sdl* sdl = lse_get_sdl_from_base(self);

  had_clip_rect = lse_graphics_base_get_state(graphics)->has_clip_rect;
  lse_graphics_base_pop_state(graphics);

  if (had_clip_rect) {
    current = lse_graphics_base_get_state(graphics);
    sdl->SDL_RenderSetClipRect(self->renderer, current ? (const SDL_Rect*)&current->clip_rect : NULL);
  }
}

// @override
static const lse_matrix* get_matrix(lse_graphics* graphics) {
  return lse_graphics_base_get_matrix(graphics);
}

// @override
static void set_matrix(lse_graphics* graphics, lse_matrix* matrix) {
  lse_graphics_base_set_matrix(graphics, matrix);
}

// @override
static void set_opacity(lse_graphics* graphics, float opacity) {
  lse_graphics_base_set_opacity(graphics, opacity);
}

// @override
static void set_clip_rect(lse_graphics* graphics, lse_rect_f* rect) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  lse_sdl* sdl = lse_get_sdl_from_base(self);
  const lse_rect* clip_rect;

  lse_graphics_base_set_clip_rect(graphics, rect);
  clip_rect = lse_graphics_base_get_clip_rect(graphics);

  sdl->SDL_RenderSetClipRect(self->renderer, (const SDL_Rect*)clip_rect);
}

// @override
static void begin(lse_graphics* graphics) {
}

// @override
static void end(lse_graphics* graphics) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  lse_sdl* sdl = lse_get_sdl_from_base(self);

  sdl->SDL_RenderPresent(self->renderer);

  lse_graphics_base_end(graphics);
}

// @override
static void clear(lse_graphics* graphics, lse_color color) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  lse_sdl* sdl = lse_get_sdl_from_base(self);

  sdl->SDL_SetRenderDrawColor(self->renderer, color.comp.r, color.comp.g, color.comp.b, color.comp.a);
  sdl->SDL_RenderClear(self->renderer);
}

// @override
static void remove_image(lse_graphics* graphics, lse_image* image) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  cmap_image_cache_value* value = cmap_image_cache_get_mut(&self->image_cache, image);

  if (value) {
    cmap_image_cache_release(lse_get_sdl_from_base(self), value, true);
    cmap_image_cache_erase(&self->image_cache, image);
  }
}

// @private
static lse_color_format to_color_format(Uint32 pixel_format) {
  switch (pixel_format) {
    case SDL_PIXELFORMAT_RGBA8888:
      return LSE_COLOR_FORMAT_RGBA;
    case SDL_PIXELFORMAT_ARGB8888:
      return LSE_COLOR_FORMAT_ARGB;
    case SDL_PIXELFORMAT_BGRA8888:
      return LSE_COLOR_FORMAT_BGRA;
    case SDL_PIXELFORMAT_RGBA32:
      return LSE_COLOR_FORMAT_RGBA;
    default:
      return LSE_COLOR_FORMAT_UNKNOWN;
  }
}

// @private
static bool has_texture_format(lse_sdl* sdl, SDL_Renderer* renderer, Uint32 desired_format) {
  SDL_RendererInfo renderer_info;

  if (sdl->SDL_GetRendererInfo(renderer, &renderer_info) == 0) {
    for (Uint32 t = 0; t < renderer_info.num_texture_formats; t++) {
      if (renderer_info.texture_formats[t] == desired_format) {
        return true;
      }
    }
  }

  return false;
}

// @override
static void draw_render_object(lse_graphics* graphics, lse_render_object* render_object, lse_color color) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  sdl_render_object* sro = (sdl_render_object*)render_object;
  lse_sdl* sdl = lse_get_sdl_from_base(self);
  SDL_Texture* texture;
  const lse_matrix* m = lse_graphics_get_matrix(graphics);
  lse_matrix temp;
  float angle;

  if (sro->image) {
    texture = get_texture(self, sro->image);
    if (!texture) {
      return;
    }
  } else if (sro->texture) {
    texture = sro->texture;
  } else {
    texture = self->fill_texture;
  }

  if (!sro->can_tint) {
    color.value = 0xFFFFFFFF;
  }

  sdl->SDL_SetTextureColorMod(texture, color.comp.r, color.comp.g, color.comp.b);
  sdl->SDL_SetTextureAlphaMod(texture, color.comp.a);

  angle = lse_matrix_get_axis_angle(m);

  if (!lse_equals_f(angle, 0)) {
    temp = lse_matrix_init_rotate(-angle);
    lse_matrix_multiply(m, &temp, &temp);
    m = &temp;
  }

  if (self->use_float_rects) {
    SDL_FRect dest = {
      // TODO: snap to pixel grid?
      .x = sro->rect.x + lse_matrix_get_translate_x(m),
      .y = sro->rect.y + lse_matrix_get_translate_y(m),
      .w = sro->rect.width * lse_matrix_get_scale_x(m),
      .h = sro->rect.height * lse_matrix_get_scale_y(m),
    };

    sdl->SDL_RenderCopyExF(
        self->renderer,
        texture,
        sro->has_src_rect ? &sro->src_rect : NULL,
        &dest,
        angle,
        &k_empty_sdl_fpoint,
        SDL_FLIP_NONE);
  } else {
    SDL_Rect dest = {
      // TODO: snap to pixel grid?
      .x = (int32_t)(sro->rect.x + lse_matrix_get_translate_x(m)),
      .y = (int32_t)(sro->rect.y + lse_matrix_get_translate_y(m)),
      .w = (int32_t)(sro->rect.width * lse_matrix_get_scale_x(m)),
      .h = (int32_t)(sro->rect.height * lse_matrix_get_scale_y(m)),
    };

    sdl->SDL_RenderCopyEx(
        self->renderer,
        texture,
        sro->has_src_rect ? &sro->src_rect : NULL,
        &dest,
        angle,
        &k_empty_sdl_point,
        SDL_FLIP_NONE);
  }
}

// @override
static lse_render_object* destroy_render_object(lse_graphics* graphics, lse_render_object* render_object) {
  return sdl_render_object_drop((sdl_render_object*)render_object, (lse_sdl_graphics*)graphics);
}

// @override
static lse_render_object*
end_queue(lse_graphics* graphics, int32_t width, int32_t height, lse_render_object* render_object) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  lse_sdl* sdl = lse_get_sdl_from_base(self);
  size_t size = lse_render_queue_size(&self->base.render_queue);
  lse_render_command* command;
  sdl_render_object* sro = (sdl_render_object*)render_object;

  if (!size) {
    return sdl_render_object_drop(sro, self);
  }

  if (size == 1) {
    command = lse_render_queue_get(&self->base.render_queue, 0);

    switch (command->type) {
      case LSE_RCT_FILL_RECT:
        return create_fill_rect_render_object(graphics, command, sro);
      case LSE_RCT_DRAW_IMAGE:
        return create_draw_image_render_object(graphics, command, sro);
      case LSE_RCT_ROUNDED_RECT:
        return create_rounded_rect_render_object(graphics, command, sro);
      case LSE_RCT_DRAW_TEXT:
        return create_draw_text_render_object(graphics, command, sro);
      default:
        break;
    }
  }

  sro = sdl_render_object_init_as_target(sro, self, SDL_TEXTUREACCESS_TARGET, width, height);

  if (!sro) {
    return (lse_render_object*)sro;
  }

  sro->rect = (lse_rect_f){ 0, 0, (float)width, (float)height };
  sro->has_src_rect = false;
  sro->can_tint = false;

  if (sdl->SDL_SetRenderTarget(self->renderer, sro->texture) != 0) {
    return sdl_render_object_drop(sro, self);
  }

  sdl->SDL_SetRenderDrawColor(self->renderer, 0, 0, 0, 0);
  sdl->SDL_RenderClear(self->renderer);

  for (size_t i = 0; i < size; i++) {
    command = lse_render_queue_get(&self->base.render_queue, i);

    switch (command->type) {
      case LSE_RCT_FILL_RECT:
        render_fill_rect(self, command);
        break;
      case LSE_RCT_STROKE_RECT:
        render_stroke_rect(self, command);
        break;
      case LSE_RCT_DRAW_IMAGE:
        render_texture(self, command);
      default:
        break;
    }
  }

  sdl->SDL_SetRenderTarget(self->renderer, NULL);

  return (lse_render_object*)sro;
}

// @private
static void render_fill_rect(lse_sdl_graphics* self, lse_render_command* command) {
  lse_sdl* sdl = lse_get_sdl_from_base(self);

  sdl->SDL_SetRenderDrawColor(
      self->renderer,
      command->fill_color.comp.r,
      command->fill_color.comp.g,
      command->fill_color.comp.b,
      command->fill_color.comp.a);

  if (self->use_float_rects) {
    sdl->SDL_RenderFillRectF(
        self->renderer,
        &(SDL_FRect){
            .x = 0,
            .y = 0,
            .w = command->rect->width,
            .h = command->rect->height,
        });
  } else {
    sdl->SDL_RenderFillRect(
        self->renderer,
        &(SDL_Rect){
            .x = 0,
            .y = 0,
            .w = (int32_t)command->rect->width,
            .h = (int32_t)command->rect->height,
        });
  }
}

// private
static SDL_Texture* get_texture(lse_sdl_graphics* self, lse_image* image) {
  lse_color* pixels;
  int32_t width;
  int32_t height;
  SDL_Texture* texture;
  const cmap_image_cache_value* value = cmap_image_cache_get(&self->image_cache, image);

  if (value) {
    return value->second;
  }

  pixels = lse_image_get_pixels(image);
  width = lse_image_get_width(image);
  height = lse_image_get_height(image);

  if (!pixels || !lse_image_is_ready(image)) {
    return NULL;
  }

  texture = ensure_texture(self, NULL, SDL_TEXTUREACCESS_STATIC, width, height);

  if (!texture) {
    return NULL;
  }

  lse_color_to_format(pixels, width * height, LSE_TEXTURE_FORMAT);

  if (lse_get_sdl_from_base(self)->SDL_UpdateTexture(texture, NULL, pixels, width * 4) == 0) {
    lse_ref(image);
    cmap_image_cache_insert(&self->image_cache, image, texture);
  } else {
    lse_get_sdl_from_base(self)->SDL_DestroyTexture(texture);
  }

  lse_image_release_pixels(image);

  return texture;
}

// @private
static void cmap_image_cache_release(lse_sdl* sdl, cmap_image_cache_value* entry, bool destroy_texture) {
  lse_unref(entry->first);

  if (destroy_texture) {
    sdl->SDL_DestroyTexture(entry->second);
  }
}

// @private
static void render_texture(lse_sdl_graphics* self, lse_render_command* command) {
  lse_sdl* sdl = lse_get_sdl_from_base(self);
  SDL_Texture* texture = get_texture(self, command->image);

  if (!texture) {
    return;
  }

  // TODO: get color from filter, use opacity
  sdl->SDL_SetTextureColorMod(texture, 0xFF, 0xFF, 0xFF /*filter.tint.r, filter.tint.g, filter.tint.b*/);
  sdl->SDL_SetTextureAlphaMod(texture, 0xFF /*filter.tint.a*/);

  if (self->use_float_rects) {
    // TODO: snap to pixel grid
    sdl->SDL_RenderCopyF(self->renderer, texture, (SDL_Rect*)command->src_rect, (SDL_FRect*)command->rect);
  } else {
    // TODO: snap to pixel grid
    sdl->SDL_RenderCopy(
        self->renderer,
        texture,
        (SDL_Rect*)command->src_rect,
        &(SDL_Rect){
            (int32_t)command->rect->x,
            (int32_t)command->rect->y,
            (int32_t)command->rect->width,
            (int32_t)command->rect->height,
        });
  }
}

// @private
static void render_stroke_rect(lse_sdl_graphics* self, lse_render_command* command) {
  // TODO: rect pos?
  const float x = 0;
  const float y = 0;
  const float w = command->rect->width;
  const float h = command->rect->height;

  const float top = command->edges->top;
  const float right = command->edges->right;
  const float bottom = command->edges->bottom;
  const float left = command->edges->left;

  SDL_FRect dest[4];
  int32_t count = 0;

  if (top > 0) {
    dest[count++] = (SDL_FRect){ x, y, w, top };
  }

  if (right > 0) {
    dest[count++] = (SDL_FRect){ x + w - right, y + top, right, h - top - bottom };
  }

  if (bottom > 0) {
    dest[count++] = (SDL_FRect){ x, y + h - bottom, w, bottom };
  }

  if (left > 0) {
    dest[count++] = (SDL_FRect){ x, y + top, left, h - top - bottom };
  }

  if (count == 0) {
    return;
  }

  int32_t i;
  lse_sdl* sdl = lse_get_sdl_from_base(self);

  sdl->SDL_SetRenderDrawColor(
      self->renderer,
      command->stroke_color.comp.r,
      command->stroke_color.comp.g,
      command->stroke_color.comp.b,
      command->stroke_color.comp.a);

  if (self->use_float_rects) {
    for (i = 0; i < count; i++) {
      sdl->SDL_RenderFillRectF(self->renderer, &dest[i]);
    }
  } else {
    for (i = 0; i < count; i++) {
      sdl->SDL_RenderFillRect(
          self->renderer,
          &(SDL_Rect){
              .x = (int32_t)dest[i].x,
              .y = (int32_t)dest[i].y,
              .w = (int32_t)dest[i].w,
              .h = (int32_t)dest[i].h,
          });
    }
  }
}

static bool sw_render_rounded_rect(lse_sdl_graphics* self, lse_render_command* command, SDL_Texture* target) {
  int32_t width = (int32_t)command->rect->width;
  int32_t height = (int32_t)command->rect->height;
  int32_t pitch = width * 4;
  uint8_t* pixels = lse_malloc(pitch * height);
  float border_adjust;
  nctx ctx = self->base.ctx;
  bool result;

  if (!nctx_set_surface(ctx, (uint8_t*)pixels, width, height, pitch)) {
    result = false;
    goto DONE;
  }

  nctx_set_fill_color(
      ctx, MAKE_NSVG_RGB(command->fill_color.comp.r, command->fill_color.comp.g, command->fill_color.comp.b));
  nctx_set_fill_opacity(ctx, (float)command->fill_color.comp.a / 255.f);

  if (command->stroke_width > 0) {
    nctx_set_stroke_color(
        ctx, MAKE_NSVG_RGB(command->stroke_color.comp.r, command->stroke_color.comp.g, command->stroke_color.comp.b));
    nctx_set_stroke_opacity(ctx, (float)command->stroke_color.comp.a / 255.f);
    nctx_set_stroke_width(ctx, command->stroke_width);
  } else {
    nctx_set_stroke_opacity(ctx, 0);
    nctx_set_stroke_width(ctx, 0);
  }

  // nanosvg draws the border on the shape edge (1/2 outside, 1/2 inside). since the rect draw fills the entire surface,
  // the borders can appear clipped (ugly). shrink the round rect down so the border correctly renders to the surface.
  border_adjust = (float)command->stroke_width / 2.f;

  nctx_rounded_rect(
      ctx,
      command->rect->x + border_adjust,
      command->rect->y + border_adjust,
      command->rect->width - (border_adjust * 2.f),
      command->rect->height - (border_adjust * 2.f),
      // TODO: border radius corner should not be more than 50% of width/height
      command->corners->top_left,
      command->corners->top_right,
      command->corners->bottom_left,
      command->corners->bottom_right);

  nctx_render_shape(ctx);
  lse_color_to_format((lse_color*)pixels, width * height, LSE_TEXTURE_FORMAT);
  result = lse_get_sdl_from_base(self)->SDL_UpdateTexture(target, NULL, pixels, pitch) == 0;

DONE:
  free(pixels);
  return result;
}

static bool sw_render_text(
    lse_sdl_graphics* self,
    lse_render_command* command,
    SDL_Texture* target,
    int32_t target_width,
    int32_t target_height) {
  lse_font* font = command->text_style->font;
  float line_height;
  float x = 0;
  float y = 0;
  bool result;
  const char* cursor = lse_string_as_cstring(command->text);
  lse_text_line_info line_metrics;
  lse_surface dest = {
    .width = target_width,
    .height = target_height,
    .buffer = lse_calloc(target_width * target_height, sizeof(lse_color)),
    .pitch = target_width * (int32_t)sizeof(lse_color),
  };
  int32_t max_lines = command->text_style->max_lines;
  int32_t line_number = 1;

  if (!dest.buffer) {
    return false;
  }

  lse_font_use_font_size(font, command->text_style->font_size_px);
  line_height = lse_font_get_line_height(font);

  while (*cursor) {
    lse_text_get_line(cursor, command->text_style, &line_metrics);

    // lines that go outside the content box should not be aligned
    if (line_metrics.width <= command->rect->width) {
      switch (command->text_style->align) {
        case LSE_STYLE_TEXT_ALIGN_CENTER:
          x = ((float)dest.width / 2.f) - (line_metrics.width / 2.f);
          break;
        case LSE_STYLE_TEXT_ALIGN_RIGHT:
          x = (float)dest.width - line_metrics.width;
          break;
        default:
          break;
      }
    }

    sw_render_text_line(&dest, x, y, command->text_style, &line_metrics);

    cursor = line_metrics.next;
    y += line_height;

    // TODO: vertical limit
    if (max_lines > 0 && max_lines == line_number++) {
      break;
    }
  }

  result = lse_get_sdl_from_base(self)->SDL_UpdateTexture(target, NULL, dest.buffer, dest.pitch) == 0;
  free(dest.buffer);

  return result;
}

// @private
static void
sw_render_text_line(lse_surface* dest, float x, float y, lse_text_style* text_style, lse_text_line_info* metrics) {
  lse_font* font = text_style->font;
  float ascent = lse_font_get_ascent(font);
  uint32_t codepoint;
  uint32_t previous = 0;
  lse_glyph_surface glyph;
  const char* cursor = metrics->start;

  while (*cursor && cursor != metrics->next) {
    codepoint = lse_text_peek(cursor, text_style);

    x += lse_font_get_kerning(font, previous, codepoint);

    if (lse_font_get_glyph_surface(font, codepoint, &glyph)) {
      sw_copy_alpha_buffer(
          lse_snap_to_pixel_grid_i(x + (float)glyph.x_offset),
          lse_snap_to_pixel_grid_i(y + ascent - (float)glyph.y_offset),
          &glyph,
          dest);
    }

    x += lse_font_get_advance(font, codepoint);

    previous = codepoint;
    cursor = lse_text_next(cursor);
  }
}

// @private
static lse_render_object*
create_fill_rect_render_object(lse_graphics* graphics, lse_render_command* command, sdl_render_object* sro) {
  sro = sdl_render_object_init(sro, (lse_sdl_graphics*)graphics);

  if (sro) {
    sro->rect = *command->rect;
    sro->can_tint = true;
  }

  return (lse_render_object*)sro;
}

// @private
static lse_render_object*
create_draw_image_render_object(lse_graphics* graphics, lse_render_command* command, sdl_render_object* sro) {
  sro = sdl_render_object_init(sro, (lse_sdl_graphics*)graphics);

  if (sro) {
    sro->image = command->image;
    lse_ref(sro->image);
    sro->rect = *command->rect;
    sro->src_rect = *((SDL_Rect*)command->src_rect);
    sro->has_src_rect = true;
    sro->can_tint = true;
  }

  return (lse_render_object*)sro;
}

// @private
static lse_render_object*
create_rounded_rect_render_object(lse_graphics* graphics, lse_render_command* command, sdl_render_object* sro) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;

  sro = sdl_render_object_init_as_target(
      sro, self, SDL_TEXTUREACCESS_STATIC, (int32_t)command->rect->width, (int32_t)command->rect->height);

  if (!sro) {
    return (lse_render_object*)sro;
  }

  if (!sw_render_rounded_rect(self, command, sro->texture)) {
    return sdl_render_object_drop(sro, self);
  }

  sro->rect = *command->rect;
  sro->has_src_rect = false;
  sro->can_tint = false;

  return (lse_render_object*)sro;
}

static lse_render_object*
create_draw_text_render_object(lse_graphics* graphics, lse_render_command* command, sdl_render_object* sro) {
  lse_sdl_graphics* self = (lse_sdl_graphics*)graphics;
  lse_size bounds;
  int32_t width;
  int32_t height;

  lse_text_measure(command->text, command->text_style, &bounds);

  width = (int32_t)ceilf(bounds.width);
  height = (int32_t)ceilf(bounds.height);

  sro = sdl_render_object_init_as_target(sro, self, SDL_TEXTUREACCESS_STATIC, width, height);

  if (!sro) {
    return (lse_render_object*)sro;
  }

  if (!sw_render_text(self, command, sro->texture, width, height)) {
    return sdl_render_object_drop(sro, self);
  }

  switch (command->text_style->align) {
    case LSE_STYLE_TEXT_ALIGN_CENTER:
      command->rect->x += (((float)command->rect->width / 2.f) - ((float)width / 2.f));
      break;
    case LSE_STYLE_TEXT_ALIGN_RIGHT:
      command->rect->x += (float)command->rect->width - (float)width;
      break;
    default:
      break;
  }

  sro->rect = (lse_rect_f){
    .x = command->rect->x,
    .y = command->rect->y,
    .width = (float)width,
    .height = (float)height,
  };

  // TODO: may need to use src rect for overflow/content clipping
  sro->has_src_rect = false;
  sro->can_tint = true;

  return (lse_render_object*)sro;
}

// @private
static SDL_Texture*
ensure_texture(lse_sdl_graphics* self, SDL_Texture* texture, int32_t access, int32_t width, int32_t height) {
  lse_sdl* sdl = lse_get_sdl_from_base(self);
  int32_t current_access;
  int32_t current_width;
  int32_t current_height;

  if (texture) {
    // TODO: using a strict w/h criteria will create texture churn for common scene graph usages. if the texture
    // is too big, reuse it but use clip rect and src rect when rendering.
    if (sdl->SDL_QueryTexture(texture, NULL, &current_access, &current_width, &current_height) == 0 &&
        current_access == access && current_width == width && current_height == height) {
      return texture;
    }

    sdl->SDL_DestroyTexture(texture);
  }

  texture = sdl->SDL_CreateTexture(self->renderer, LSE_SDL_TEXTURE_FORMAT, access, width, height);

  if (texture) {
    sdl->SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  }

  return texture;
}

static lse_render_object* sdl_render_object_drop(sdl_render_object* current, lse_sdl_graphics* sdl_graphics) {
  if (current) {
    if (current->texture) {
      lse_get_sdl_from_base(sdl_graphics)->SDL_DestroyTexture(current->texture);
    }
    lse_unref(current->image);

    free(current);
  }

  return NULL;
}

static sdl_render_object* sdl_render_object_init(sdl_render_object* current, lse_sdl_graphics* sdl_graphics) {
  if (current) {
    if (current->texture) {
      lse_get_sdl_from_base(sdl_graphics)->SDL_DestroyTexture(current->texture);
    }
    lse_unref(current->image);

    memset(current, 0, sizeof(sdl_render_object));
  } else {
    current = lse_calloc(1, sizeof(sdl_render_object));
  }

  return current;
}

static sdl_render_object* sdl_render_object_init_as_target(
    sdl_render_object* current,
    lse_sdl_graphics* sdl_graphics,
    int32_t texture_access,
    int32_t width,
    int32_t height) {
  SDL_Texture* texture;

  assert(texture_access == SDL_TEXTUREACCESS_TARGET || texture_access == SDL_TEXTUREACCESS_STATIC);

  if (current) {
    texture = current->texture;
    current->texture = NULL;
  } else {
    texture = NULL;
  }

  texture = ensure_texture(sdl_graphics, texture, texture_access, width, height);

  if (!texture) {
    return (sdl_render_object*)sdl_render_object_drop(current, sdl_graphics);
  }

  current = sdl_render_object_init(current, sdl_graphics);
  current->texture = texture;

  return current;
}

static void sw_copy_alpha_buffer(int32_t x, int32_t y, const lse_glyph_surface* glyph, const lse_surface* dest) {
  uint8_t* src_buffer = glyph->surface.buffer;
  int32_t src_width = glyph->surface.width;
  int32_t src_height = glyph->surface.height;
  lse_color* dest_buffer = dest->buffer;
  int32_t dest_buffer_stride = dest->pitch / (int32_t)sizeof(lse_color);
  int32_t src_y = 0;
  int32_t i;

  dest_buffer += ((dest_buffer_stride * y) + x);

  while (src_y < src_height && src_y < dest->height - y) {
    for (i = 0; i < src_width; i++) {
      dest_buffer[i].value = (0xFFFFFF | (src_buffer[i] << 24));
    }

    dest_buffer += dest_buffer_stride;
    src_buffer += src_width;

    src_y++;
  }
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_GRAPHICS_VTABLE();
LSE_TYPE_INFO(lse_sdl_graphics_type, lse_sdl_graphics, lse_graphics_interface_type, &vtable);
