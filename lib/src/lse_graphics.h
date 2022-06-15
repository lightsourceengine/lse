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

#include <lse.h>
#include "lse_color.h"
#include "lse_matrix.h"
#include "lse_rect.h"
#include "lse_render_queue.h"
#include "lse_types.h"
#include "nanoctx.h"
#include <stc/forward.h>

//
// vtable
//

typedef struct lse_graphics_interface lse_graphics_interface;

struct lse_graphics_interface {
  lse_status (*configure)(lse_graphics*, void*);
  void (*destroy)(lse_graphics*);
  bool (*is_destroyed)(lse_graphics*);

  void (*begin)(lse_graphics*);
  void (*end)(lse_graphics*);

  void (*clear)(lse_graphics*, lse_color);
  void (*draw_render_object)(lse_graphics*, lse_render_object*, lse_color);
  lse_render_object* (*destroy_render_object)(lse_graphics*, lse_render_object*);

  lse_render_object* (*end_queue)(lse_graphics*, int32_t, int32_t, lse_render_object*);

  void (*push_state)(lse_graphics*);
  void (*pop_state)(lse_graphics*);
  const lse_matrix* (*get_matrix)(lse_graphics*);
  void (*set_matrix)(lse_graphics*, lse_matrix* matrix);
  void (*set_opacity)(lse_graphics*, float opacity);
  void (*set_clip_rect)(lse_graphics*, lse_rect_f* rect);

  void (*remove_image)(lse_graphics*, lse_image*);
};

#define LSE_GRAPHICS_VTABLE(CLASS)                                                                                     \
  static lse_graphics_interface vtable = {                                                                             \
    .configure = configure,                                                                                            \
    .destroy = destroy,                                                                                                \
    .is_destroyed = is_destroyed,                                                                                      \
    .draw_render_object = draw_render_object,                                                                          \
    .destroy_render_object = destroy_render_object,                                                                    \
    .end_queue = end_queue,                                                                                            \
    .begin = begin,                                                                                                    \
    .end = end,                                                                                                        \
    .clear = clear,                                                                                                    \
    .remove_image = remove_image,                                                                                      \
    .push_state = push_state,                                                                                          \
    .pop_state = pop_state,                                                                                            \
    .get_matrix = get_matrix,                                                                                          \
    .set_matrix = set_matrix,                                                                                          \
    .set_opacity = set_opacity,                                                                                        \
    .set_clip_rect = set_clip_rect,                                                                                    \
  };

//
// types
//

typedef struct lse_graphics_state lse_graphics_state;
typedef struct lse_graphics_base lse_graphics_base;

forward_cstack(cstack_graphics_state, lse_graphics_state);

struct lse_graphics_state {
  lse_rect clip_rect;
  bool has_clip_rect;
  lse_matrix matrix;
  float opacity;
};

struct lse_graphics_base {
  lse_env* env;
  cstack_graphics_state state;
  nctx ctx;
  int32_t width;
  int32_t height;
  lse_render_queue render_queue;
};

//
// public c api
//

#define LSE_GRAPHICS_V(INSTANCE, METHOD)                                                                               \
  ((lse_graphics_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE))
#define LSE_GRAPHICS_A(INSTANCE, METHOD, ...)                                                                          \
  ((lse_graphics_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE), __VA_ARGS__)

#define lse_graphics_configure(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), configure, __VA_ARGS__)
#define lse_graphics_destroy(INSTANCE) LSE_GRAPHICS_V((INSTANCE), destroy)
#define lse_graphics_is_destroyed(INSTANCE) LSE_GRAPHICS_V((INSTANCE), is_destroyed)
#define lse_graphics_clear(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), clear, __VA_ARGS__)
#define lse_graphics_remove_image(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), remove_image, __VA_ARGS__)
#define lse_graphics_begin(INSTANCE) LSE_GRAPHICS_V((INSTANCE), begin)
#define lse_graphics_end(INSTANCE) LSE_GRAPHICS_V((INSTANCE), end)
#define lse_graphics_push_state(INSTANCE) LSE_GRAPHICS_V((INSTANCE), push_state)
#define lse_graphics_pop_state(INSTANCE) LSE_GRAPHICS_V((INSTANCE), pop_state)
#define lse_graphics_get_matrix(INSTANCE) LSE_GRAPHICS_V((INSTANCE), get_matrix)
#define lse_graphics_set_matrix(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), set_matrix, __VA_ARGS__)
#define lse_graphics_set_opacity(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), set_opacity, __VA_ARGS__)
#define lse_graphics_set_clip_rect(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), set_clip_rect, __VA_ARGS__)
#define lse_graphics_draw_render_object(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), draw_render_object, __VA_ARGS__)
#define lse_graphics_get_render_object_offset(INSTANCE, ...)                                                           \
  LSE_GRAPHICS_A((INSTANCE), get_render_object_offset, __VA_ARGS__)
#define lse_graphics_destroy_render_object(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), destroy_render_object, __VA_ARGS__)
#define lse_graphics_end_queue(INSTANCE, ...) LSE_GRAPHICS_A((INSTANCE), end_queue, __VA_ARGS__)

void lse_graphics_reset_state(lse_graphics* graphics);
int32_t lse_graphics_get_width(lse_graphics* graphics);
int32_t lse_graphics_get_height(lse_graphics* graphics);

bool lse_graphics_begin_queue(lse_graphics* graphics);
void lse_graphics_queue_stroke_rect(
    lse_graphics* graphics,
    lse_rect_f* rect,
    lse_border_rect* edges,
    lse_color stroke_color);
void lse_graphics_queue_fill_rect(lse_graphics* graphics, lse_rect_f* rect, lse_color fill_color);
void lse_graphics_queue_rounded_rect(
    lse_graphics* graphics,
    lse_rect_f* rect,
    lse_border_radius* corners,
    lse_color fill_color,
    float stroke_width,
    lse_color stroke_color);
void lse_graphics_queue_draw_image(lse_graphics* graphics, lse_rect_f* rect, lse_image* image, lse_rect* src_rect);
// TODO: add text
void lse_graphics_queue_draw_text(lse_graphics* graphics, lse_rect_f* rect, lse_text_style* style, lse_string* text);

//
// protected base class api
//

void lse_graphics_base_constructor(lse_graphics* graphics, void* arg);
void lse_graphics_base_destructor(lse_graphics* graphics);
void lse_graphics_base_destroy(lse_graphics* graphics);
bool lse_graphics_base_is_destroyed(lse_graphics* graphics);
void lse_graphics_base_end(lse_graphics* graphics);
void lse_graphics_base_push_state(lse_graphics* graphics);
void lse_graphics_base_pop_state(lse_graphics* graphics);
const lse_graphics_state* lse_graphics_base_get_state(lse_graphics* graphics);
const lse_matrix* lse_graphics_base_get_matrix(lse_graphics* graphics);
void lse_graphics_base_set_matrix(lse_graphics* graphics, lse_matrix* matrix);
void lse_graphics_base_set_opacity(lse_graphics* graphics, float opacity);
void lse_graphics_base_set_clip_rect(lse_graphics* graphics, lse_rect_f* rect);
const lse_rect* lse_graphics_base_get_clip_rect(lse_graphics* graphics);

/**
 * Get the base data pointer of the object.
 *
 * In C the address of a struct equals the address of the first field. In order for this cast to work, the base data
 * struct MUST be the first field of the struct.
 */
#define lse_graphics_get_base(GRAPHICS) ((lse_graphics_base*)(GRAPHICS))
