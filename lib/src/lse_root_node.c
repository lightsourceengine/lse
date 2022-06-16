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
#include "lse_object.h"
#include "lse_style.h"
#include "lse_window.h"

typedef struct lse_root_node lse_root_node;

struct lse_root_node {
  lse_node_base base;
};

static void run_layout(lse_node* node, float width, float height);
static void run_paint(lse_node* node, lse_graphics* graphics);
static void run_composite(lse_node* node, lse_graphics* graphics);
static void resolve(lse_node* node);

// @override
static void constructor(lse_object* object, void* arg) {
  lse_node_base_constructor((lse_node*)object, (lse_window*)arg);
}

// @override
static void destructor(lse_object* object) {
  lse_node_base_destructor((lse_node*)object);
}

// @override
static void destroy(lse_node* node) {
  if (!lse_node_is_destroyed(node)) {
    lse_node_base_destroy(node);
  }
}

// @override
static void on_style_resolve(lse_node* node) {
}

// @override
static void on_paint(lse_node* node, lse_graphics* graphics) {
}

// @override
static void on_composite(lse_node* node, lse_graphics* graphics) {
  lse_style* style = lse_node_get_style(node);

  lse_graphics_clear(graphics, lse_style_get_color_t(style, LSE_SP_BACKGROUND_COLOR));
}

// @override
static void on_style_property_change(lse_node* node, lse_style_property property) {
  lse_root_node* self = (lse_root_node*)node;

  switch (property) {
    case LSE_SP_BACKGROUND_COLOR:
    case LSE_SP_OPACITY:
      lse_node_request_composite(node);
      break;
    case LSE_SP_FONT_SIZE:
      lse_window_dispatch_root_font_size_change(
          self->base.window,
          lse_style_compute_font_size_from_root(
              lse_node_get_style(node), lse_window_get_style_context(self->base.window)));
      break;
    default:
      lse_node_base_style_property_change(node, property);
      break;
  }
}

// @public
void lse_root_node_update(lse_node* node, lse_graphics* graphics, float width, float height) {
  run_layout(node, width, height);

  run_paint(node, graphics);

  lse_graphics_reset_state(graphics);
  run_composite(node, graphics);
}

// @public
void lse_root_node_inline_layout(lse_node* node) {
  lse_root_node* self = (lse_root_node*)node;
  YGNodeRef yg_node = self->base.yg_node;

  // run layout, if necessary
  if (YGNodeIsDirty(yg_node)) {
    YGNodeCalculateLayout(
        yg_node,
        (float)lse_window_get_width(self->base.window),
        (float)lse_window_get_height(self->base.window),
        YGDirectionLTR);
  }

  // flush existing style_resolve events and ones generated by the above layout calc
  if (lse_node_has_flag(node, LSE_NODE_FLAG_RESOLVE)) {
    resolve(node);
  }

  // ahh! the style_resolve events could have dirtied the layout, particularly text related properties
  if (YGNodeIsDirty(yg_node)) {
    YGNodeCalculateLayout(
        yg_node,
        (float)lse_window_get_width(self->base.window),
        (float)lse_window_get_height(self->base.window),
        YGDirectionLTR);
  }

  // in the current pipeline, layout should be settled at this point in the code. the event bubbling is complex.
  // as features are added, the above algorithm may need to change to ensure inline layout settles.
}

// @private
static void resolve(lse_node* node) {
  if (lse_node_has_flag(node, LSE_NODE_FLAG_RESOLVE)) {
    lse_node_unset_flag(node, LSE_NODE_FLAG_RESOLVE);
    lse_node_on_style_resolve(node);
  }

  uint32_t count = lse_node_get_child_count(node);

  for (uint32_t i = 0; i < count; i++) {
    resolve(lse_node_get_child_at(node, i));
  }
}

// @private
static void run_layout(lse_node* node, float width, float height) {
  YGNodeRef yg_node = lse_node_get_base(node)->yg_node;

  if (YGNodeIsDirty(yg_node)) {
    YGNodeCalculateLayout(yg_node, width, height, YGDirectionLTR);
  }

  if (lse_node_has_flag(node, LSE_NODE_FLAG_RESOLVE)) {
    resolve(node);
  }
}

// @private
static void run_paint(lse_node* node, lse_graphics* graphics) {
  if (!lse_node_is_leaf(node)) {
    uint32_t i;
    uint32_t count = lse_node_get_child_count(node);

    for (i = 0; i < count; i++) {
      run_paint(lse_node_get_child_at(node, i), graphics);
    }
  }

  if (lse_node_has_flag(node, LSE_NODE_FLAG_PAINT)) {
    lse_node_unset_flag(node, LSE_NODE_FLAG_PAINT);
    lse_node_on_paint(node, graphics);
  }
}

// @private
static void run_composite(lse_node* node, lse_graphics* graphics) {
  lse_node_base* base = lse_node_get_base(node);
  lse_style* style = lse_node_get_style_or_empty(node);
  lse_rect_f box = lse_node_get_box(node);
  bool clip = lse_style_get_enum(style, LSE_SP_OVERFLOW) == LSE_STYLE_OVERFLOW_HIDDEN;
  lse_matrix transform;

  lse_graphics_push_state(graphics);
  lse_graphics_set_opacity(graphics, lse_style_resolve_opacity(style));

  if (clip) {
    lse_graphics_set_clip_rect(graphics, &box);
  }

  transform = lse_matrix_init_translate(box.x, box.y);
  lse_graphics_set_matrix(graphics, &transform);

  if (lse_style_has_transform(style)) {
    transform = lse_style_compute_transform(style, lse_window_get_style_context(base->window), &box);
    lse_graphics_set_matrix(graphics, &transform);
  }

  // TODO: check box empty
  lse_node_on_composite(node, graphics);

  if (!lse_node_is_leaf(node)) {
    uint32_t count = lse_node_get_child_count(node);

    for (uint32_t i = 0; i < count; i++) {
      run_composite(lse_node_get_child_at(node, i), graphics);
    }
  }

  lse_graphics_pop_state(graphics);
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_NODE_VTABLE();
LSE_TYPE_INFO(lse_root_node_type, lse_root_node, lse_node_interface_type, &vtable);
