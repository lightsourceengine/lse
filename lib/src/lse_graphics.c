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

#include "lse_object.h"
#include "lse_util.h"
#include <assert.h>

#define i_val lse_graphics_state
#define i_tag graphics_state
#define i_opt c_no_clone | c_no_compare | c_is_fwd
#include "stc/cstack.h"

static const lse_graphics_state k_empty_state = {
  .clip_rect = {
    .x = 0,
    .y = 0,
    .width = 0,
    .height = 0
  },
  .matrix = {
    .a = 1, .b = 0, .x = 0,
    .c = 0, .d = 1, .y = 0,
  },
  .opacity = 1
};

void lse_graphics_base_constructor(lse_graphics* graphics, void* arg) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  base->env = arg;
  lse_ref(base->env);

  base->state = cstack_graphics_state_with_capacity(32);
  base->ctx = nctx_new();
  base->render_queue = lse_render_queue_init();
}

void lse_graphics_base_destructor(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  assert(base->env == NULL);

  lse_render_queue_drop(&base->render_queue);

  cstack_graphics_state_drop(&base->state);
  lse_unref(base->env);

  // TODO: in destroy?
  nctx_delete(base->ctx);
}

void lse_graphics_base_destroy(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  cstack_graphics_state_clear(&base->state);

  lse_unref(base->env);
  base->env = NULL;

  base->width = base->height = 0;
}

bool lse_graphics_base_is_destroyed(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  return !base || base->env == NULL;
}

void lse_graphics_base_end(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  //  cstack_graphics_state_clear(&base->state);
  lse_graphics_reset_state(graphics);
}

void lse_graphics_reset_state(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  cstack_graphics_state_clear(&base->state);
  cstack_graphics_state_push(&base->state, k_empty_state);
}

void lse_graphics_base_push_state(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);
  cstack_graphics_state_value* state;
  lse_graphics_state top;

  if (base->state.size == 0) {
    cstack_graphics_state_push(&base->state, k_empty_state);
  } else {
    top = *cstack_graphics_state_top(&base->state);

    cstack_graphics_state_push(&base->state, top);
  }

  state = cstack_graphics_state_top(&base->state);
  state->has_clip_rect = false;
}

void lse_graphics_base_pop_state(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  cstack_graphics_state_pop(&base->state);
}

const lse_graphics_state* lse_graphics_base_get_state(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  if (base->state.size > 0) {
    return cstack_graphics_state_top(&base->state);
  }

  return NULL;
}

void lse_graphics_base_set_matrix(lse_graphics* graphics, lse_matrix* matrix) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);
  lse_graphics_state* top = cstack_graphics_state_top(&base->state);

  lse_matrix_multiply(&top->matrix, matrix, &top->matrix);
}

void lse_graphics_base_set_opacity(lse_graphics* graphics, float opacity) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  cstack_graphics_state_top(&base->state)->opacity *= lse_clamp_f(opacity, 0.f, 1.f);
}

void lse_graphics_base_set_clip_rect(lse_graphics* graphics, lse_rect_f* rect) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);
  cstack_graphics_state_value* state = cstack_graphics_state_top(&base->state);
  lse_rect* clip_rect = &state->clip_rect;
  lse_rect rect_i = {
    .x = (int32_t)rect->x,
    .y = (int32_t)rect->y,
    .width = (int32_t)rect->width,
    .height = (int32_t)rect->height,
  };

  if (lse_rect_is_empty(clip_rect)) {
    *clip_rect = rect_i;
  } else {
    *clip_rect = lse_rect_intersect(clip_rect, &rect_i);
  }

  state->has_clip_rect = true;
}

const lse_rect* lse_graphics_base_get_clip_rect(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  return &cstack_graphics_state_top(&base->state)->clip_rect;
}

const lse_matrix* lse_graphics_base_get_matrix(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  return &cstack_graphics_state_top(&base->state)->matrix;
}

int32_t lse_graphics_get_width(lse_graphics* graphics) {
  return lse_graphics_get_base(graphics)->width;
}

int32_t lse_graphics_get_height(lse_graphics* graphics) {
  return lse_graphics_get_base(graphics)->height;
}

bool lse_graphics_begin_queue(lse_graphics* graphics) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  lse_render_queue_clear(&base->render_queue);

  return true;
}

void lse_graphics_queue_stroke_rect(
    lse_graphics* graphics,
    lse_rect_f* rect,
    lse_border_rect* edges,
    lse_color stroke_color) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  lse_render_queue_push(
      &base->render_queue,
      &(lse_render_command){ .type = LSE_RCT_STROKE_RECT, .rect = rect, .edges = edges, .stroke_color = stroke_color });
}

void lse_graphics_queue_fill_rect(lse_graphics* graphics, lse_rect_f* rect, lse_color fill_color) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  lse_render_queue_push(
      &base->render_queue, &(lse_render_command){ .type = LSE_RCT_FILL_RECT, .rect = rect, .fill_color = fill_color });
}

void lse_graphics_queue_rounded_rect(
    lse_graphics* graphics,
    lse_rect_f* rect,
    lse_border_radius* corners,
    lse_color fill_color,
    float stroke_width,
    lse_color stroke_color) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  lse_render_queue_push(
      &base->render_queue,
      &(lse_render_command){
          .type = LSE_RCT_ROUNDED_RECT,
          .rect = rect,
          .corners = corners,
          .fill_color = fill_color,
          .stroke_color = stroke_color,
          .stroke_width = stroke_width,
      });
}

void lse_graphics_queue_draw_image(lse_graphics* graphics, lse_rect_f* rect, lse_image* image, lse_rect* src_rect) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  lse_render_queue_push(
      &base->render_queue,
      &(lse_render_command){
          .type = LSE_RCT_DRAW_IMAGE,
          .rect = rect,
          .image = image,
          .src_rect = src_rect,
      });
}

void lse_graphics_queue_draw_text(lse_graphics* graphics, lse_rect_f* rect, lse_text_style* style, lse_string* text) {
  lse_graphics_base* base = lse_graphics_get_base(graphics);

  lse_render_queue_push(
      &base->render_queue,
      &(lse_render_command){
          .type = LSE_RCT_DRAW_TEXT,
          .rect = rect,
          .text_style = style,
          .text = text,
      });
}
