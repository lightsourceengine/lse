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

#include "lse_node.h"

#include "lse_graphics.h"
#include "lse_image.h"
#include "lse_object.h"
#include "lse_rect.h"
#include "lse_style.h"
#include "lse_window.h"

typedef struct lse_box_node lse_box_node;

struct lse_box_node {
  lse_node_base base;
  lse_image* background_image;
};

static void set_background_image(lse_node* node, lse_string* uri);
static void on_image_event(const lse_image_event* e, void* observer);

// @override
static void constructor(lse_object* node, void* arg) {
  lse_node_base_constructor((lse_node*)node, (lse_window*)arg);
}

// @override
static void destructor(lse_object* node) {
  lse_box_node* self = (lse_box_node*)node;

  assert(self->background_image == NULL);
  lse_node_base_destructor((lse_node*)node);
}

// @override
static void destroy(lse_node* node) {
  if (lse_node_is_destroyed(node)) {
    return;
  }

  lse_box_node* self = (lse_box_node*)node;

  self->background_image = lse_node_base_image_release(node, self->background_image);
  lse_node_base_destroy(node);
}

// @override
static void on_composite(lse_node* node, lse_graphics* graphics) {
  lse_node_base* base = lse_node_get_base(node);

  if (!base->surface) {
    return;
  }

  lse_style* style = lse_node_get_style_or_empty(node);

  lse_graphics_draw_render_object(graphics, base->surface, lse_style_get_color_t(style, LSE_SP_BACKGROUND_COLOR));
}

// @override
static void on_paint(lse_node* node, lse_graphics* graphics) {
  lse_box_node* self = (lse_box_node*)node;
  lse_node_base* base = lse_node_get_base(node);
  lse_style* style = lse_node_get_style_or_empty(node);
  const lse_style_context* context = lse_window_get_style_context(base->window);

  lse_rect_f box = lse_node_get_box_at(node, 0, 0);
  lse_border_rect edges;
  lse_border_radius border_radius;
  lse_rect_f image_rect;
  lse_rect_f clipped_rect;
  lse_rect clipped_src_rect;

  if (!lse_graphics_begin_queue(graphics)) {
    return;
  }

  if (lse_style_has_property(style, LSE_SP_BORDER_RADIUS)) {
    if (lse_style_has_property(style, LSE_SP_BACKGROUND_COLOR) || lse_style_has_property(style, LSE_SP_BORDER_COLOR)) {
      border_radius = lse_style_compute_border_radius(style, context);

      lse_graphics_queue_rounded_rect(
          graphics,
          &box,
          &border_radius,
          lse_style_get_color_t(style, LSE_SP_BACKGROUND_COLOR),
          lse_style_compute_border_only(style, context),
          lse_style_get_color_t(style, LSE_SP_BORDER_COLOR));
    }
  } else {
    if (lse_style_has_property(style, LSE_SP_BACKGROUND_COLOR)) {
      lse_graphics_queue_fill_rect(graphics, &box, lse_style_get_color_t(style, LSE_SP_BACKGROUND_COLOR));
    }

    if (lse_image_can_render(self->background_image)) {
      lse_style_compute_background_fit(style, context, &box, self->background_image, &image_rect);

      lse_clip_image_rect(
          &box,
          &image_rect,
          (float)lse_image_get_width(self->background_image),
          (float)lse_image_get_height(self->background_image),
          &clipped_rect,
          &clipped_src_rect);

      lse_graphics_queue_draw_image(graphics, &clipped_rect, self->background_image, &clipped_src_rect);
    }

    if (lse_style_has_property(style, LSE_SP_BORDER_COLOR) && lse_style_has_border_layout(node)) {
      edges = lse_style_get_border_edges(node);

      lse_graphics_queue_stroke_rect(graphics, &box, &edges, lse_style_get_color_t(style, LSE_SP_BORDER_COLOR));
    }
  }

  base->surface = lse_graphics_end_queue(graphics, box.width, box.height, base->surface);

  lse_node_request_composite(node);
}

// @override
static void on_style_resolve(lse_node* node) {
  if (lse_node_has_flag(node, LSE_NODE_FLAG_BOUNDS | LSE_NODE_FLAG_SHAPE | LSE_NODE_FLAG_COLOR)) {
    lse_node_unset_flag(node, LSE_NODE_FLAG_BOUNDS | LSE_NODE_FLAG_SHAPE | LSE_NODE_FLAG_COLOR);
    lse_node_request_paint(node);
  }
}

// @override
static void on_style_property_change(lse_node* node, lse_style_property property) {
  switch (property) {
    case LSE_SP_BACKGROUND_IMAGE:
      set_background_image(node, (lse_string*)lse_style_get_object(lse_node_get_style(node), LSE_SP_BACKGROUND_IMAGE));
      break;
      //    case LSE_SP_BACKGROUND_REPEAT:
    case LSE_SP_BACKGROUND_POSITION_X:
    case LSE_SP_BACKGROUND_POSITION_Y:
    case LSE_SP_BACKGROUND_SIZE:
    case LSE_SP_BACKGROUND_WIDTH:
    case LSE_SP_BACKGROUND_HEIGHT:
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_BOUNDS);
      break;
    case LSE_SP_BORDER_RADIUS:
    case LSE_SP_BORDER_RADIUS_TOP_LEFT:
    case LSE_SP_BORDER_RADIUS_TOP_RIGHT:
    case LSE_SP_BORDER_RADIUS_BOTTOM_LEFT:
    case LSE_SP_BORDER_RADIUS_BOTTOM_RIGHT:
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_SHAPE);
      break;
    case LSE_SP_BACKGROUND_COLOR:
    case LSE_SP_BORDER_COLOR:
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_COLOR);
      break;
    default:
      lse_node_base_style_property_change(node, property);
      break;
  }
}

// @private
static void set_background_image(lse_node* node, lse_string* uri) {
  lse_box_node* self = (lse_box_node*)node;

  if (self->background_image) {
    if (lse_string_empty(uri)) {
      lse_node_base_image_release(node, self->background_image);
      self->background_image = NULL;
    } else if (lse_string_cmp(lse_image_get_uri(self->background_image), uri) != 0) {
      lse_node_base_image_release(node, self->background_image);
      lse_ref(uri);
      self->background_image = lse_node_base_image_acquire(node, uri, &on_image_event);
    }
  } else {
    if (!lse_string_empty(uri)) {
      lse_ref(uri);
      self->background_image = lse_node_base_image_acquire(node, uri, &on_image_event);
    }
  }
}

static void on_image_event(const lse_image_event* e, void* observer) {
  lse_node_request_style_resolve(observer, LSE_NODE_FLAG_BOUNDS);
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_NODE_VTABLE();
LSE_TYPE_INFO(lse_box_node_type, lse_box_node, lse_node_interface_type, &vtable);
