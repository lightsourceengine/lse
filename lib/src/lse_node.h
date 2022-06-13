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

#include "lse.h"
#include "lse_rect.h"
#include "lse_types.h"
#include "lse_util.h"
#include "Yoga.h"

//
// vtable
//

typedef struct lse_node_interface lse_node_interface;

struct lse_node_interface {
  void (*destroy)(lse_node*);
  void (*on_composite)(lse_node*, lse_graphics*);
  void (*on_paint)(lse_node*, lse_graphics*);
  void (*on_style_resolve)(lse_node*);
  void (*on_style_property_change)(lse_node*, lse_style_property);
};

#define LSE_NODE_VTABLE(CLASS)                                                                                         \
  static lse_node_interface vtable = {                                                                                 \
    .destroy = destroy,                                                                                                \
    .on_composite = on_composite,                                                                                      \
    .on_paint = on_paint,                                                                                              \
    .on_style_resolve = on_style_resolve,                                                                              \
    .on_style_property_change = on_style_property_change,                                                              \
  };

//
// types
//

typedef enum lse_node_flag {
  LSE_NODE_FLAG_LEAF = 1,

  LSE_NODE_FLAG_RESOLVE = 1 << 1,
  LSE_NODE_FLAG_PAINT = 1 << 2,
  LSE_NODE_FLAG_COMPOSITE = 1 << 3,

  LSE_NODE_FLAG_BOUNDS = 1 << 4,
  LSE_NODE_FLAG_SHAPE = 1 << 5,
  LSE_NODE_FLAG_COLOR = 1 << 6,
  LSE_NODE_FLAG_TEXT = 1 << 7,
  LSE_NODE_FLAG_FONT = 1 << 8,
} lse_node_flag;

struct lse_node_base {
  int32_t id;
  YGNodeRef yg_node; // TODO: port flex layout to C99
  lse_style* style;
  lse_window* window;

  uint32_t flags;
  lse_render_object* surface;
};

//
// public lse_node c api
//

#define LSE_NODE_V(INSTANCE, METHOD)                                                                                   \
  ((lse_node_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE))
#define LSE_NODE_A(INSTANCE, METHOD, ...)                                                                              \
  ((lse_node_interface*)lse_object_get_interface((lse_object*)(INSTANCE)))->METHOD((INSTANCE), __VA_ARGS__)

#define lse_node_destroy(INSTANCE) LSE_NODE_V((lse_node*)(INSTANCE), destroy)
#define lse_node_on_composite(INSTANCE, ...) LSE_NODE_A((lse_node*)(INSTANCE), on_composite, __VA_ARGS__)
#define lse_node_on_paint(INSTANCE, ...) LSE_NODE_A((lse_node*)(INSTANCE), on_paint, __VA_ARGS__)
#define lse_node_on_style_resolve(INSTANCE) LSE_NODE_V((lse_node*)(INSTANCE), on_style_resolve)
#define lse_node_on_style_property_change(INSTANCE, ...)                                                               \
  LSE_NODE_A((lse_node*)(INSTANCE), on_style_property_change, __VA_ARGS__)

lse_rect_f lse_node_get_box(lse_node* node);
lse_rect_f lse_node_get_box_at(lse_node* node, float x, float y);

bool lse_node_is_destroyed(lse_node* node);

//
// protected lse_node_base functions
//

void lse_node_base_constructor(lse_node* node, lse_window* window);
void lse_node_base_destructor(lse_node* node);
void lse_node_base_destroy(lse_node* node);

bool lse_node_base_style_property_change(lse_node* node, lse_style_property prop);

lse_image* lse_node_base_image_acquire(lse_node* node, lse_string* uri, lse_image_event_callback callback);
lse_image* lse_node_base_image_release(lse_node* node, lse_image* image);

// TODO: layout_type does not need to be passed
void lse_node_on_yoga_layout_event(YGNodeConstRef yg_node, int32_t layout_type);

void lse_node_request_paint(lse_node* node);
void lse_node_request_style_resolve(lse_node* node, uint32_t flag);
void lse_node_request_composite(lse_node* node);

bool lse_node_is_leaf(lse_node* node);
bool lse_node_has_flag(lse_node* node, uint32_t flag);
void lse_node_set_flag(lse_node* node, uint32_t flag);
void lse_node_unset_flag(lse_node* node, uint32_t flag);

//
// subclass specific functions
//

void lse_root_node_update(lse_node* node, lse_graphics* graphics, float width, float height);
void lse_root_node_inline_layout(lse_node* node);

//
// utility functions
//

void lse_node_traverse_pre_order(lse_node* node, void (*func)(lse_node*));

/**
 * Get the base data pointer of the object.
 *
 * In C the address of a struct equals the address of the first field. In order for this cast to work, the base data
 * struct MUST be the first field of the struct.
 */
#define lse_node_get_base(NODE) ((lse_node_base*)(NODE))
