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

#include "assert.h"
#include "lse_image_store.h"
#include "lse_object.h"
#include "lse_style.h"
#include "lse_style_meta.h"
#include "lse_window.h"

#define YGNodeStyleSetBorderAuto(...) assert(false && "border property was not validated")
#define YGNodeStyleSetBorderPercent(...) assert(false && "border property was not validated")
#define YGNodeStyleSetPositionAuto(...) assert(false && "position property was not validated")
#define YGNodeStyleSetMaxWidthAuto(...) assert(false && "maxWidth property was not validated")
#define YGNodeStyleSetMaxHeightAuto(...) assert(false && "maxHeight property was not validated")
#define YGNodeStyleSetMinWidthAuto(...) assert(false && "minWidth property was not validated")
#define YGNodeStyleSetMinHeightAuto(...) assert(false && "minHeight property was not validated")
#define YGNodeStyleSetFlexAuto(...) assert(false && "flex property was not validated")
#define YGNodeStyleSetFlexPercent(...) assert(false && "flex property was not validated")
#define YGNodeStyleSetFlexGrowAuto(...) assert(false && "flexGrow property was not validated")
#define YGNodeStyleSetFlexGrowPercent(...) assert(false && "flexGrow property was not validated")
#define YGNodeStyleSetFlexShrinkAuto(...) assert(false && "flexShrink property was not validated")
#define YGNodeStyleSetFlexShrinkPercent(...) assert(false && "flexShrink property was not validated")
#define YGNodeStyleSetPaddingAuto(...) assert(false && "flexShrink property was not validated")

#define YGStyleSetEnum(YG_PROP_NAME, YG_TYPE, NODE, PROP)                                                              \
  YGNodeStyleSet##YG_PROP_NAME((NODE)->yg_node, (YG_TYPE)lse_style_get_enum((NODE)->style, (PROP)))

#define YGStyleSetEdge(YG_PROP_NAME, EDGE, NODE, CONTEXT, PROP)                                                        \
  {                                                                                                                    \
    const lse_style_value* numeric = lse_style_get_numeric((NODE)->style, (PROP));                                     \
    switch (numeric->unit) {                                                                                           \
      case LSE_STYLE_UNIT_PERCENT:                                                                                     \
        YGNodeStyleSet##YG_PROP_NAME##Percent((NODE)->yg_node, (EDGE), numeric->value);                                \
        break;                                                                                                         \
      case LSE_STYLE_UNIT_AUTO:                                                                                        \
        YGNodeStyleSet##YG_PROP_NAME##Auto((NODE)->yg_node, (EDGE));                                                   \
        break;                                                                                                         \
      default:                                                                                                         \
        YGNodeStyleSet##YG_PROP_NAME((NODE)->yg_node, (EDGE), lse_style_resolve_numeric(numeric, (CONTEXT)));          \
    }                                                                                                                  \
  }

#define YGStyleSet(YG_PROP_NAME, NODE, CONTEXT, PROP)                                                                  \
  {                                                                                                                    \
    const lse_style_value* numeric = lse_style_get_numeric((NODE)->style, (PROP));                                     \
    switch (numeric->unit) {                                                                                           \
      case LSE_STYLE_UNIT_PERCENT:                                                                                     \
        YGNodeStyleSet##YG_PROP_NAME##Percent((NODE)->yg_node, numeric->value);                                        \
        break;                                                                                                         \
      case LSE_STYLE_UNIT_AUTO:                                                                                        \
        YGNodeStyleSet##YG_PROP_NAME##Auto((NODE)->yg_node);                                                           \
        break;                                                                                                         \
      default:                                                                                                         \
        YGNodeStyleSet##YG_PROP_NAME((NODE)->yg_node, lse_style_resolve_numeric(numeric, (CONTEXT)));                  \
    }                                                                                                                  \
  }

static uint32_t get_index(YGNodeRef parent, YGNodeRef child) {
  uint32_t size = YGNodeGetChildCount(parent);

  for (uint32_t i = 0; i < size; i++) {
    if (YGNodeGetChild(parent, i) == child) {
      return i;
    }
  }

  return UINT32_MAX;
}

static void insert_child_at(YGNodeRef parent, YGNodeRef child, uint32_t index) {
  if (YGNodeGetParent(child) != NULL || index == UINT32_MAX) {
    return;
  }

  YGNodeInsertChild(parent, child, index);

  // for set parent
  lse_object_ref(YGNodeGetContext(parent));
  // for adding to child list
  lse_object_ref(YGNodeGetContext(child));
}

void lse_node_base_constructor(lse_node* node, lse_window* window) {
  lse_node_base* base = lse_node_get_base(node);

  base->yg_node = YGNodeNew();
  YGNodeSetContext(base->yg_node, node);

  base->window = window;
  lse_ref(window);
}

void lse_node_base_destructor(lse_node* node) {
  lse_node_base* base = lse_node_get_base(node);

  assert(base->yg_node == NULL);
  assert(base->window == NULL);
  assert(base->style == NULL);
}

// TODO: pass in enum or style_value?
bool lse_node_base_style_property_change(lse_node* node, lse_style_property prop) {
  lse_node_base* base = lse_node_get_base(node);
  const lse_style_context* context = lse_window_get_style_context(base->window);

  switch (prop) {
    // non-layout properties
    // composite-only
    case LSE_SP_TRANSFORM_ORIGIN_X:
    case LSE_SP_TRANSFORM_ORIGIN_Y:
    case LSE_SP_OPACITY:
    case LSE_SP_TRANSFORM:
      //    case LSE_SP_Z_ORDER:
      //      // TODO: addChild(), removeChild()
      //      // TODO: check if captured in paint
      //      // TODO: transform could cause a re-paint
      //      lse_node_request_composite(node);
      //      break;
      // layout properties
    case LSE_SP_ALIGN_ITEMS:
      YGStyleSetEnum(AlignItems, YGAlign, base, prop);
      break;
    case LSE_SP_ALIGN_CONTENT:
      YGStyleSetEnum(AlignContent, YGAlign, base, prop);
      break;
    case LSE_SP_ALIGN_SELF:
      YGStyleSetEnum(AlignSelf, YGAlign, base, prop);
      break;
    case LSE_SP_BORDER:
      YGStyleSetEdge(Border, YGEdgeAll, base, context, prop);
      break;
    case LSE_SP_BORDER_BOTTOM:
      YGStyleSetEdge(Border, YGEdgeBottom, base, context, prop);
      break;
    case LSE_SP_BORDER_LEFT:
      YGStyleSetEdge(Border, YGEdgeLeft, base, context, prop);
      break;
    case LSE_SP_BORDER_RIGHT:
      YGStyleSetEdge(Border, YGEdgeRight, base, context, prop);
      break;
    case LSE_SP_BORDER_TOP:
      YGStyleSetEdge(Border, YGEdgeTop, base, context, prop);
      break;
    case LSE_SP_BOTTOM:
      YGStyleSetEdge(Position, YGEdgeBottom, base, context, prop);
      break;
    case LSE_SP_DISPLAY:
      YGStyleSetEnum(Display, YGDisplay, base, prop);
      break;
    case LSE_SP_FLEX:
      YGStyleSet(Flex, base, context, prop);
      break;
    case LSE_SP_FLEX_BASIS:
      YGStyleSet(FlexBasis, base, context, prop);
      break;
    case LSE_SP_FLEX_DIRECTION:
      YGStyleSetEnum(FlexDirection, YGFlexDirection, base, prop);
      break;
    case LSE_SP_FLEX_GROW:
      YGStyleSet(FlexGrow, base, context, prop);
      break;
    case LSE_SP_FLEX_SHRINK:
      YGStyleSet(FlexShrink, base, context, prop);
      break;
    case LSE_SP_FLEX_WRAP:
      YGStyleSetEnum(FlexWrap, YGWrap, base, prop);
      break;
    case LSE_SP_HEIGHT:
      YGStyleSet(Height, base, context, prop);
      break;
    case LSE_SP_JUSTIFY_CONTENT:
      YGStyleSetEnum(JustifyContent, YGJustify, base, prop);
      break;
    case LSE_SP_LEFT:
      YGStyleSetEdge(Position, YGEdgeLeft, base, context, prop);
      break;
    case LSE_SP_MARGIN:
      YGStyleSetEdge(Margin, YGEdgeAll, base, context, prop);
      break;
    case LSE_SP_MARGIN_BOTTOM:
      YGStyleSetEdge(Margin, YGEdgeBottom, base, context, prop);
      break;
    case LSE_SP_MARGIN_LEFT:
      YGStyleSetEdge(Margin, YGEdgeLeft, base, context, prop);
      break;
    case LSE_SP_MARGIN_RIGHT:
      YGStyleSetEdge(Margin, YGEdgeRight, base, context, prop);
      break;
    case LSE_SP_MARGIN_TOP:
      YGStyleSetEdge(Margin, YGEdgeTop, base, context, prop);
      break;
    case LSE_SP_MAX_HEIGHT:
      YGStyleSet(MaxHeight, base, context, prop);
      break;
    case LSE_SP_MAX_WIDTH:
      YGStyleSet(MaxWidth, base, context, prop);
      break;
    case LSE_SP_MIN_HEIGHT:
      YGStyleSet(MinHeight, base, context, prop);
      break;
    case LSE_SP_MIN_WIDTH:
      YGStyleSet(MinWidth, base, context, prop);
      break;
    case LSE_SP_OVERFLOW:
      YGStyleSetEnum(Overflow, YGOverflow, base, prop);
      break;
    case LSE_SP_PADDING:
      YGStyleSetEdge(Padding, YGEdgeAll, base, context, prop);
      break;
    case LSE_SP_PADDING_BOTTOM:
      YGStyleSetEdge(Padding, YGEdgeBottom, base, context, prop);
      break;
    case LSE_SP_PADDING_LEFT:
      YGStyleSetEdge(Padding, YGEdgeLeft, base, context, prop);
      break;
    case LSE_SP_PADDING_RIGHT:
      YGStyleSetEdge(Padding, YGEdgeRight, base, context, prop);
      break;
    case LSE_SP_PADDING_TOP:
      YGStyleSetEdge(Padding, YGEdgeTop, base, context, prop);
      break;
    case LSE_SP_POSITION:
      YGStyleSetEnum(PositionType, YGPositionType, base, prop);
      break;
    case LSE_SP_RIGHT:
      YGStyleSetEdge(Position, YGEdgeRight, base, context, prop);
      break;
    case LSE_SP_TOP:
      YGStyleSetEdge(Position, YGEdgeTop, base, context, prop);
      break;
    case LSE_SP_WIDTH:
      YGStyleSet(Width, base, context, prop);
      break;
    default:
      return false;
  }

  // TODO: comes through yoga layout event
  //  lse_node_set_flag(node, LSE_NODE_FLAG_BOUNDS);
  //  lse_node_request_style_resolve(node);

  return true;
}

void lse_node_base_destroy(lse_node* node) {
  lse_node_base* base = lse_node_get_base(node);
  uint32_t child_count = YGNodeGetChildCount(base->yg_node);
  lse_node* parent;
  lse_node* child;

  for (uint32_t i = 0; i < child_count; i++) {
    child = lse_node_get_child_at(node, i);
    lse_node_destroy(child);
  }

  parent = lse_node_get_parent(node);

  if (parent) {
    lse_node_remove_child(node, parent);
  }

  if (base->style) {
    lse_style_bind_node(base->style, NULL);
    lse_unref(base->style);
    base->style = NULL;
  }

  lse_window_destroy_render_object(base->window, base->surface);
  base->surface = NULL;

  lse_unref(base->window);
  base->window = NULL;

  YGNodeFree(base->yg_node);
  base->yg_node = NULL;
}

lse_rect_f lse_node_get_box(lse_node* node) {
  YGNodeRef yg_node = lse_node_get_base(node)->yg_node;

  return (lse_rect_f){
    .x = YGNodeLayoutGetLeft(yg_node),
    .y = YGNodeLayoutGetTop(yg_node),
    .width = YGNodeLayoutGetWidth(yg_node),
    .height = YGNodeLayoutGetHeight(yg_node),
  };
}

lse_rect_f lse_node_get_box_at(lse_node* node, float x, float y) {
  YGNodeRef yg_node = lse_node_get_base(node)->yg_node;

  return (lse_rect_f){
    .x = x,
    .y = y,
    .width = YGNodeLayoutGetWidth(yg_node),
    .height = YGNodeLayoutGetHeight(yg_node),
  };
}

bool lse_node_is_destroyed(lse_node* node) {
  return !node || !lse_node_get_base(node)->yg_node;
}

lse_image* lse_node_base_image_acquire(lse_node* node, lse_string* uri, lse_image_event_callback callback) {
  lse_node_base* base = lse_node_get_base(node);
  lse_image_store* store = lse_window_get_image_store(base->window);
  lse_resource_state state;
  lse_image* image = lse_image_store_acquire_image(store, uri, LSE_IMAGE_STORE_ASYNC);

  if (!image) {
    return NULL;
  }

  lse_image_add_observer(image, node, callback);

  state = lse_image_get_state(image);

  if (state == LSE_RESOURCE_STATE_READY || state == LSE_RESOURCE_STATE_ERROR) {
    // TODO: not the right check
    if (lse_node_is_leaf(node)) {
      YGNodeMarkDirty(base->yg_node);
    } else {
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_BOUNDS);
    }
    return image;
  }

  return image;
}

lse_image* lse_node_base_image_release(lse_node* node, lse_image* image) {
  if (image) {
    lse_node_base* base = lse_node_get_base(node);
    lse_image_store* store = lse_window_get_image_store(base->window);

    lse_image_remove_observer(image, node);
    lse_image_store_release_image(store, image);

    // TODO: not the right check
    if (lse_node_is_leaf(node)) {
      YGNodeMarkDirty(base->yg_node);
    } else {
      lse_node_request_style_resolve(node, LSE_NODE_FLAG_BOUNDS);
    }
  }

  return NULL;
}

void lse_node_traverse_pre_order(lse_node* node, void (*func)(lse_node*)) {
  YGNodeRef yg_node = lse_node_get_base(node)->yg_node;
  uint32_t count = YGNodeGetChildCount(yg_node);

  func(node);

  for (uint32_t i = 0; i < count; i++) {
    lse_node_traverse_pre_order(YGNodeGetContext(YGNodeGetChild(yg_node, i)), func);
  }
}

LSE_API float LSE_CDECL lse_node_get_x(lse_node* node) {
  lse_node_base* base = lse_node_get_base(node);

  return base->yg_node ? YGNodeLayoutGetLeft(base->yg_node) : 0;
}

LSE_API float LSE_CDECL lse_node_get_y(lse_node* node) {
  lse_node_base* base = lse_node_get_base(node);

  return base->yg_node ? YGNodeLayoutGetTop(base->yg_node) : 0;
}

LSE_API float LSE_CDECL lse_node_get_width(lse_node* node) {
  lse_node_base* base = lse_node_get_base(node);

  return base->yg_node ? YGNodeLayoutGetWidth(base->yg_node) : 0;
}

LSE_API float LSE_CDECL lse_node_get_height(lse_node* node) {
  lse_node_base* base = lse_node_get_base(node);

  return base->yg_node ? YGNodeLayoutGetHeight(base->yg_node) : 0;
}

LSE_API lse_node* LSE_CDECL lse_node_get_parent(lse_node* node) {
  YGNodeRef parent = YGNodeGetParent(lse_node_get_base(node)->yg_node);

  return parent ? YGNodeGetContext(parent) : NULL;
}

LSE_API uint32_t LSE_CDECL lse_node_get_child_count(lse_node* node) {
  return YGNodeGetChildCount(lse_node_get_base(node)->yg_node);
}

LSE_API lse_node* LSE_CDECL lse_node_get_child_at(lse_node* node, uint32_t index) {
  YGNodeRef child = YGNodeGetChild(lse_node_get_base(node)->yg_node, index);

  return child ? YGNodeGetContext(child) : NULL;
}

LSE_API lse_style* LSE_CDECL lse_node_get_style(lse_node* node) {
  lse_node_base* base = lse_node_get_base(node);

  if (!base->style) {
    base->style = lse_style_new();
    lse_style_bind_node(base->style, node);
  }

  return base->style;
}

LSE_API bool LSE_CDECL lse_node_append(lse_node* node, lse_node* child) {
  lse_node_base* base = lse_node_get_base(node);
  lse_node_base* child_base = lse_node_get_base(child);

  // TODO: return separate error code for each case
  if (lse_node_is_destroyed(node) || lse_node_is_leaf(node) || !child || base->window != child_base->window) {
    return false;
  }

  insert_child_at(base->yg_node, child_base->yg_node, YGNodeGetChildCount(base->yg_node));

  lse_root_node_inline_layout(lse_window_get_root(base->window));

  return true;
}

LSE_API bool LSE_CDECL lse_node_insert_before(lse_node* node, lse_node* child, lse_node* before) {
  lse_node_base* node_base = lse_node_get_base(node);
  lse_node_base* before_base = lse_node_get_base(before);
  lse_node_base* child_base = lse_node_get_base(child);

  // TODO: return separate error code for each case
  if (lse_node_is_destroyed(node) || lse_node_is_leaf(node) || !child || !before ||
      node_base->window != child_base->window) {
    return false;
  }

  uint32_t before_index = get_index(node_base->yg_node, before_base->yg_node);

  insert_child_at(node_base->yg_node, child_base->yg_node, before_index);

  lse_root_node_inline_layout(lse_window_get_root(node_base->window));

  return true;
}

LSE_API bool LSE_CDECL lse_node_remove_child(lse_node* node, lse_node* child) {
  lse_node_base* node_base = lse_node_get_base(node);
  lse_node_base* child_base = lse_node_get_base(child);

  // TODO: return separate error code for each case
  if (lse_node_is_destroyed(node) || !child || YGNodeGetParent(child_base->yg_node) != node_base->yg_node) {
    return false;
  }

  YGNodeRemoveChild(node_base->yg_node, child_base->yg_node);

  // Remove reference for the child.
  lse_unref(child);

  // Remove reference for the parent
  lse_unref(node);

  lse_root_node_inline_layout(lse_window_get_root(node_base->window));

  return true;
}

LSE_API lse_style* LSE_CDECL lse_node_get_style_or_empty(lse_node* node) {
  lse_style* style = lse_node_get_base(node)->style;

  return style ? style : lse_style_get_empty();
}

void lse_node_on_yoga_layout_event(YGNodeConstRef yg_node, int32_t layout_type) {
  lse_node* node = YGNodeGetContext((YGNodeRef)yg_node);

  lse_node_request_style_resolve(node, LSE_NODE_FLAG_BOUNDS);
}

bool lse_node_is_leaf(lse_node* node) {
  return lse_node_has_flag(node, LSE_NODE_FLAG_LEAF);
}

bool lse_node_has_flag(lse_node* node, uint32_t flag) {
  return (lse_node_get_base(node)->flags & flag);
}

void lse_node_set_flag(lse_node* node, uint32_t flag) {
  lse_node_get_base(node)->flags |= flag;
}

void lse_node_unset_flag(lse_node* node, uint32_t flag) {
  lse_node_get_base(node)->flags &= ~(flag);
}

void lse_node_request_paint(lse_node* node) {
  lse_node_set_flag(node, LSE_NODE_FLAG_PAINT);
  lse_node_set_flag(lse_window_get_root(lse_node_get_base(node)->window), LSE_NODE_FLAG_PAINT);
}

void lse_node_request_style_resolve(lse_node* node, uint32_t flag) {
  lse_node_set_flag(node, flag | LSE_NODE_FLAG_RESOLVE);
  lse_node_set_flag(lse_window_get_root(lse_node_get_base(node)->window), LSE_NODE_FLAG_RESOLVE);
}

void lse_node_request_composite(lse_node* node) {
  //  lse_node_set_flag(node, LSE_NODE_FLAG_COMPOSITE);
  lse_node_set_flag(lse_window_get_root(lse_node_get_base(node)->window), LSE_NODE_FLAG_COMPOSITE);
}
