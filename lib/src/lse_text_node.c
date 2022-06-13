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

#include "lse_font.h"
#include "lse_graphics.h"
#include "lse_object.h"
#include "lse_style.h"
#include "lse_text.h"
#include "lse_window.h"

typedef struct lse_text_node lse_text_node;

struct lse_text_node {
  lse_node_base base;
  lse_string* text;
  lse_font* font;
};

static YGSize
measure(YGNodeRef nodeRef, float width, YGMeasureMode width_mode, float height, YGMeasureMode height_mode);
static bool sync_font_with_style(lse_node* node);
static void on_font_event(const lse_font_event* e, void* observer);

// @override
static void constructor(lse_object* obj, void* arg) {
  lse_node* node = (lse_node*)obj;
  lse_node_base* base = lse_node_get_base(node);

  lse_node_base_constructor(node, (lse_window*)arg);

  YGNodeSetNodeType(base->yg_node, YGNodeTypeText);
  YGNodeSetMeasureFunc(base->yg_node, &measure);

  lse_node_set_flag(node, LSE_NODE_FLAG_LEAF);
}

// @override
static void destructor(lse_object* obj) {
  lse_node* node = (lse_node*)obj;
  lse_text_node* self = (lse_text_node*)obj;

  lse_node_base_destructor(node);
  lse_unref(self->text);
}

// @override
static void destroy(lse_node* node) {
  if (lse_node_is_destroyed(node)) {
    return;
  }

  lse_text_node* self = (lse_text_node*)node;

  if (self->font) {
    lse_font_remove_observer(self->font, node);
    lse_env_release_font(lse_window_get_env(self->base.window), self->font);
    self->font = NULL;
  }

  lse_node_base_destroy(node);
}

// @override
static void on_composite(lse_node* node, lse_graphics* graphics) {
  lse_node_base* base = lse_node_get_base(node);

  if (!base->surface) {
    return;
  }

  lse_style* style = lse_node_get_style_or_empty(node);

  lse_graphics_draw_render_object(graphics, base->surface, (lse_color)lse_style_get_color(style, LSE_SP_COLOR));
}

// @override
static void on_paint(lse_node* node, lse_graphics* graphics) {
  lse_text_node* self = (lse_text_node*)node;
  lse_node_base* base = lse_node_get_base(node);
  lse_style* style = lse_node_get_style_or_empty(node);
  const lse_style_context* context = lse_window_get_style_context(base->window);
  lse_rect_f box = lse_node_get_box_at(node, 0, 0);
  lse_text_style text_style = lse_style_as_text_style(
      style,
      context,
      self->font,
      &(lse_size){
          .width = box.width,
          .height = box.height,
      });
  // lse_border_rect edges;

  if (!lse_graphics_begin_queue(graphics)) {
    return;
  }

  //  if (lse_style_has_property(style, LSE_SP_BACKGROUND_COLOR)) {
  //    lse_graphics_queue_fill_rect(graphics, &box, (lse_color)lse_style_get_color(style, LSE_SP_BACKGROUND_COLOR));
  //  }

  if (lse_font_is_ready(self->font)) {
    lse_graphics_queue_draw_text(graphics, &box, &text_style, self->text);
  }

  //  if (lse_style_has_border_layout(node)) {
  //    edges = lse_style_get_border_edges(node);
  //
  //    lse_graphics_queue_stroke_rect(graphics, &box, &edges, (lse_color)lse_style_get_color(style,
  //    LSE_SP_BORDER_COLOR));
  //  }

  base->surface = lse_graphics_end_queue(graphics, box.width, box.height, base->surface);

  lse_node_request_composite(node);
}

// @override
static void on_style_resolve(lse_node* node) {
  lse_text_node* self = (lse_text_node*)node;
  lse_node_base* base = lse_node_get_base(node);

  if (lse_node_has_flag(node, LSE_NODE_FLAG_FONT)) {
    lse_node_unset_flag(node, LSE_NODE_FLAG_FONT);

    if (sync_font_with_style(node)) {
      if (self->font && lse_font_get_state(self->font) != LSE_RESOURCE_STATE_LOADING) {
        YGNodeMarkDirty(base->yg_node);
        lse_node_request_paint(node);
      }
    }
  }

  if (lse_node_has_flag(node, LSE_NODE_FLAG_BOUNDS | LSE_NODE_FLAG_TEXT | LSE_NODE_FLAG_COLOR)) {
    lse_node_unset_flag(node, LSE_NODE_FLAG_BOUNDS | LSE_NODE_FLAG_TEXT | LSE_NODE_FLAG_COLOR);
    lse_node_request_paint(node);
  }
}

// @override
static void on_style_property_change(lse_node* node, lse_style_property property) {
  switch (property) {
    case LSE_SP_FONT_FAMILY:
    case LSE_SP_FONT_STYLE:
    case LSE_SP_FONT_WEIGHT:
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_FONT);
      break;
    case LSE_SP_FONT_SIZE:
    case LSE_SP_TEXT_ALIGN:
    case LSE_SP_TEXT_OVERFLOW:
    case LSE_SP_TEXT_TRANSFORM:
    case LSE_SP_OVERFLOW_WRAP:
    case LSE_SP_MAX_LINES:
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_TEXT);
      break;
    case LSE_SP_BACKGROUND_COLOR:
    case LSE_SP_BORDER_COLOR:
    case LSE_SP_COLOR:
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
  lse_text_node* self = YGNodeGetContext(nodeRef);
  lse_text_style text_style;
  lse_size size;

  if (width_mode == YGMeasureModeUndefined) {
    width = 0;
  }

  if (height_mode == YGMeasureModeUndefined) {
    height = 0;
  }

  if (!lse_string_empty(self->text) && lse_font_is_ready(self->font)) {
    text_style = lse_style_as_text_style(
        lse_node_get_style_or_empty((lse_node*)self),
        lse_window_get_style_context(self->base.window),
        self->font,
        &(lse_size){
            .width = width,
            .height = height,
        });

    lse_text_measure(self->text, &text_style, &size);
  } else {
    size = (lse_size){ 0 };
  }

  return (YGSize){
    .width = size.width,
    .height = size.height,
  };
}

// @private
static bool sync_font_with_style(lse_node* node) {
  lse_text_node* self = (lse_text_node*)node;
  lse_env* env = lse_window_get_env(self->base.window);
  lse_style* style = lse_node_get_style_or_empty(node);
  lse_string* family = (lse_string*)lse_style_get_object(style, LSE_SP_FONT_FAMILY);
  lse_style_font_style font_style = lse_style_get_enum(style, LSE_SP_FONT_STYLE);
  lse_style_font_weight font_weight = lse_style_get_enum(style, LSE_SP_FONT_WEIGHT);
  lse_font* next_font;

  // TODO: necessary check?
  //  if (lse_font_key_equals(self->font, family, font_style, font_weight)) {
  //    return false;
  //  }

  // TODO: fallback to default font if family is null
  next_font = lse_env_acquire_font(env, lse_string_as_cstring(family), font_style, font_weight);

  if (self->font == next_font) {
    lse_env_release_font(env, next_font);
    return false;
  }

  if (self->font) {
    lse_font_remove_observer(self->font, node);
    lse_env_release_font(env, self->font);
  }

  self->font = next_font;

  if (self->font) {
    lse_font_add_observer(self->font, node, &on_font_event);
  }

  return true;
}

// @public
LSE_API bool LSE_CDECL lse_node_set_text(lse_node* node, lse_string* text) {
  lse_text_node* self = (lse_text_node*)node;

  if (!text) {
    text = lse_string_new_empty();
  }

  if (self->text && lse_string_cmp(text, self->text) == 0) {
    lse_unref(text);
  } else {
    lse_unref(self->text);
    self->text = text;
    sync_font_with_style(node);
    YGNodeMarkDirty(self->base.yg_node);
  }

  return true;
}

// @public
LSE_API lse_string* LSE_CDECL lse_node_get_text(lse_node* node) {
  lse_text_node* text_node = (lse_text_node*)node;

  return text_node->text;
}

// @private
static void on_font_event(const lse_font_event* e, void* observer) {
  if (e->state == LSE_RESOURCE_STATE_READY) {
    YGNodeMarkDirty(lse_node_get_base(observer)->yg_node);
  }
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_NODE_VTABLE();
LSE_TYPE_INFO(lse_text_node_type, lse_text_node, lse_node_interface_type, &vtable);
