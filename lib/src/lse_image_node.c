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
#include "lse_style.h"
#include "lse_window.h"

typedef struct lse_image_node lse_image_node;

struct lse_image_node {
  lse_node_base base;
  lse_image* image;
};

static YGSize
measure(YGNodeRef nodeRef, float width, YGMeasureMode width_mode, float height, YGMeasureMode height_mode);
static void on_image_event(const lse_image_event* e, void* observer);

// @override
static void constructor(lse_object* obj, void* arg) {
  lse_node* node = (lse_node*)obj;
  lse_node_base* base = lse_node_get_base(node);

  lse_node_base_constructor(node, (lse_window*)arg);

  YGNodeSetMeasureFunc(base->yg_node, &measure);
  lse_node_set_flag(node, LSE_NODE_FLAG_LEAF);
}

// @override
static void destructor(lse_object* node) {
  lse_image_node* self = (lse_image_node*)node;

  assert(self->image == NULL);
  lse_node_base_destructor((lse_node*)node);
}

// @override
static void destroy(lse_node* node) {
  if (lse_node_is_destroyed(node)) {
    return;
  }

  lse_image_node* self = (lse_image_node*)node;

  self->image = lse_node_base_image_release(node, self->image);
  lse_node_base_destroy(node);
}

// @override
static void on_composite(lse_node* node, lse_graphics* graphics) {
  lse_node_base* base = lse_node_get_base(node);

  if (!base->surface) {
    return;
  }

  lse_graphics_draw_render_object(
      graphics,
      base->surface,
      // TODO: send filter
      LSE_COLOR_MAKE(255, 255, 255, 255));
}

// @override
static void on_paint(lse_node* node, lse_graphics* graphics) {
  lse_image_node* self = (lse_image_node*)node;
  lse_node_base* base = lse_node_get_base(node);
  lse_style* style = lse_node_get_style_or_empty(node);
  const lse_style_context* context = lse_window_get_style_context(base->window);

  lse_rect_f box = lse_node_get_box_at(node, 0, 0);
  lse_rect_f image_rect;
  lse_rect_f clipped_rect;
  lse_rect clipped_src_rect;
  lse_border_rect edges;

  if (!lse_graphics_begin_queue(graphics)) {
    return;
  }

  if (lse_style_has_property(style, LSE_SP_BACKGROUND_COLOR)) {
    lse_graphics_queue_fill_rect(graphics, &box, lse_style_get_color_t(style, LSE_SP_BACKGROUND_COLOR));
  }

  if (lse_image_can_render(self->image)) {
    lse_style_compute_object_fit(style, context, &box, self->image, &image_rect);

    lse_clip_image_rect(
        &box,
        &image_rect,
        (float)lse_image_get_width(self->image),
        (float)lse_image_get_height(self->image),
        &clipped_rect,
        &clipped_src_rect);

    lse_graphics_queue_draw_image(graphics, &clipped_rect, self->image, &clipped_src_rect);
  }

  if (lse_style_has_property(style, LSE_SP_BORDER_COLOR) && lse_style_has_border_layout(node)) {
    edges = lse_style_get_border_edges(node);

    lse_graphics_queue_stroke_rect(graphics, &box, &edges, lse_style_get_color_t(style, LSE_SP_BORDER_COLOR));
  }

  base->surface = lse_graphics_end_queue(graphics, box.width, box.height, base->surface);

  lse_node_request_composite(node);
}

// @override
static void on_style_resolve(lse_node* node) {
  if (lse_node_has_flag(node, LSE_NODE_FLAG_COLOR | LSE_NODE_FLAG_BOUNDS)) {
    lse_node_unset_flag(node, LSE_NODE_FLAG_BOUNDS | LSE_NODE_FLAG_COLOR);
    lse_node_request_paint(node);
  }
}

// @override
static void on_style_property_change(lse_node* node, lse_style_property property) {
  switch (property) {
    case LSE_SP_OBJECT_FIT:
    case LSE_SP_OBJECT_POSITION_X:
    case LSE_SP_OBJECT_POSITION_Y:
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_BOUNDS);
      break;
    case LSE_SP_BACKGROUND_COLOR:
    case LSE_SP_BORDER_COLOR:
      // TODO: case LSE_SP_FILTER:
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_COLOR);
      break;
    default:
      lse_node_base_style_property_change(node, property);
      break;
  }
}

// @private
static YGSize
measure(YGNodeRef nodeRef, float width, YGMeasureMode width_mode, float height, YGMeasureMode height_mode) {
  lse_image_node* image_node = YGNodeGetContext(nodeRef);

  if (!image_node->image || lse_image_get_state(image_node->image) != LSE_RESOURCE_STATE_READY) {
    // TODO: check not ready
    return (YGSize){ .width = 0, .height = 0 };
  }

  return (YGSize){ .width = (float)lse_image_get_width(image_node->image),
                   .height = (float)lse_image_get_height(image_node->image) };
}

// @public
LSE_API bool LSE_CDECL lse_node_set_src(lse_node* node, lse_string* uri) {
  lse_image_node* image_node = (lse_image_node*)node;
  lse_image* image = image_node->image;

  if (image) {
    if (lse_string_cmp(uri, lse_image_get_uri(image))) {
      lse_unref(uri);
      return true;
    } else {
      lse_node_base_image_release(node, image);
    }
  }

  image_node->image = lse_node_base_image_acquire(node, uri, &on_image_event);

  return image_node->image != NULL;
}

// @public
LSE_API lse_string* LSE_CDECL lse_node_get_src(lse_node* node) {
  lse_image_node* image_node = (lse_image_node*)node;
  lse_image* image = image_node->image;

  return image ? lse_image_get_uri(image) : NULL;
}

static void on_image_event(const lse_image_event* e, void* observer) {
  YGNodeMarkDirty(lse_node_get_base(observer)->yg_node);
  // TODO: send message to scripting layer (support for onload event)
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_NODE_VTABLE();
LSE_TYPE_INFO(lse_image_node_type, lse_image_node, lse_node_interface_type, &vtable);
