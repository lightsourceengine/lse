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

#include "lse_style.h"

#include "lse_color.h"
#include "lse_font.h"
#include "lse_node.h"
#include "lse_object.h"
#include "lse_string.h"
#include "lse_style_meta.h"
#include "lse_text.h"
#include "lse_util.h"
#include "Yoga.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  union {
    lse_object* object;
    uint32_t color;
    lse_style_value numeric;
    int32_t enum_value;
  } u;
} style_property;

#define i_tag properties
#define i_key int32_t
#define i_val style_property
#define i_opt c_no_clone
#include "stc/cmap.h"

#define i_tag color_keyword_table
#define i_key crawstr
#define i_val lse_color
#define i_hash crawstr_hash
#define i_cmp crawstr_cmp
#define i_opt c_no_clone
#include "stc/cmap.h"

#define i_tag properties
#define i_key uint8_t
#define i_opt c_no_clone
#include "stc/cset.h"

#define CASE_COMPUTE_PX_PT(VALUE, CONTEXT)                                                                             \
  case LSE_STYLE_UNIT_PX:                                                                                              \
    return (VALUE)->value;                                                                                             \
  case LSE_STYLE_UNIT_PT:                                                                                              \
    return (VALUE)->value * LSE_PT_SCALE_FACTOR

#define CASE_COMPUTE_REM(VALUE, CONTEXT)                                                                               \
  case LSE_STYLE_UNIT_REM:                                                                                             \
    return (VALUE)->value * (CONTEXT)->root_font_size_px

#define CASE_COMPUTE_VW_VH_VMIN_VMAX(VALUE, CONTEXT)                                                                   \
  case LSE_STYLE_UNIT_VMIN:                                                                                            \
    return ((VALUE)->value / 100.f) * (CONTEXT)->view_min;                                                             \
  case LSE_STYLE_UNIT_VMAX:                                                                                            \
    return ((VALUE)->value / 100.f) * (CONTEXT)->view_max;                                                             \
  case LSE_STYLE_UNIT_VW:                                                                                              \
    return ((VALUE)->value / 100.f) * (CONTEXT)->view_width;                                                           \
  case LSE_STYLE_UNIT_VH:                                                                                              \
    return ((VALUE)->value / 100.f) * (CONTEXT)->view_height

#define COORDINATE_AXIS_X 1
#define COORDINATE_AXIS_Y 2

struct lse_style {
  lse_node* node;
  lse_style* parent;
  cmap_properties properties;
  bool is_locked;
  bool is_class;
};

typedef struct {
  const char* name;
  lse_color color;
} css_color_value;

static css_color_value kColorKeywords[] = {
  // from: https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#Color_keywords
  { "aliceblue", LSE_MAKE_COLOR(0xF0, 0xF8, 0xFF, 0xFF) },
  { "antiquewhite", LSE_MAKE_COLOR(0xFA, 0xEB, 0xD7, 0xFF) },
  { "aqua", LSE_MAKE_COLOR(0x00, 0xFF, 0xFF, 0xFF) },
  { "aquamarine", LSE_MAKE_COLOR(0x7F, 0xFF, 0xD4, 0xFF) },
  { "azure", LSE_MAKE_COLOR(0xF0, 0xFF, 0xFF, 0xFF) },
  { "beige", LSE_MAKE_COLOR(0xF5, 0xF5, 0xDC, 0xFF) },
  { "bisque", LSE_MAKE_COLOR(0xFF, 0xE4, 0xC4, 0xFF) },
  { "black", LSE_MAKE_COLOR(0x00, 0x00, 0x00, 0xFF) },
  { "blanchedalmond", LSE_MAKE_COLOR(0xFF, 0xEB, 0xCD, 0xFF) },
  { "blue", LSE_MAKE_COLOR(0x00, 0x00, 0xFF, 0xFF) },
  { "blueviolet", LSE_MAKE_COLOR(0x8A, 0x2B, 0xE2, 0xFF) },
  { "brown", LSE_MAKE_COLOR(0xA5, 0x2A, 0x2A, 0xFF) },
  { "burlywood", LSE_MAKE_COLOR(0xDE, 0xB8, 0x87, 0xFF) },
  { "cadetblue", LSE_MAKE_COLOR(0x5F, 0x9E, 0xA0, 0xFF) },
  { "chartreuse", LSE_MAKE_COLOR(0x7F, 0xFF, 0x00, 0xFF) },
  { "chocolate", LSE_MAKE_COLOR(0xD2, 0x69, 0x1E, 0xFF) },
  { "coral", LSE_MAKE_COLOR(0xFF, 0x7F, 0x50, 0xFF) },
  { "cornflowerblue", LSE_MAKE_COLOR(0x64, 0x95, 0xED, 0xFF) },
  { "cornsilk", LSE_MAKE_COLOR(0xFF, 0xF8, 0xDC, 0xFF) },
  { "crimson", LSE_MAKE_COLOR(0xDC, 0x14, 0x3C, 0xFF) },
  { "cyan", LSE_MAKE_COLOR(0x00, 0xFF, 0xFF, 0xFF) },
  { "darkblue", LSE_MAKE_COLOR(0x00, 0x00, 0x8B, 0xFF) },
  { "darkcyan", LSE_MAKE_COLOR(0x00, 0x8B, 0x8B, 0xFF) },
  { "darkgoldenrod", LSE_MAKE_COLOR(0xB8, 0x86, 0x0B, 0xFF) },
  { "darkgray", LSE_MAKE_COLOR(0xA9, 0xA9, 0xA9, 0xFF) },
  { "darkgreen", LSE_MAKE_COLOR(0x00, 0x64, 0x00, 0xFF) },
  { "darkgrey", LSE_MAKE_COLOR(0xA9, 0xA9, 0xA9, 0xFF) },
  { "darkkhaki", LSE_MAKE_COLOR(0xBD, 0xB7, 0x6B, 0xFF) },
  { "darkmagenta", LSE_MAKE_COLOR(0x8B, 0x00, 0x8B, 0xFF) },
  { "darkolivegreen", LSE_MAKE_COLOR(0x55, 0x6B, 0x2F, 0xFF) },
  { "darkorange", LSE_MAKE_COLOR(0xFF, 0x8C, 0x00, 0xFF) },
  { "darkorchid", LSE_MAKE_COLOR(0x99, 0x32, 0xCC, 0xFF) },
  { "darkred", LSE_MAKE_COLOR(0x8B, 0x00, 0x00, 0xFF) },
  { "darksalmon", LSE_MAKE_COLOR(0xE9, 0x96, 0x7A, 0xFF) },
  { "darkseagreen", LSE_MAKE_COLOR(0x8F, 0xBC, 0x8F, 0xFF) },
  { "darkslateblue", LSE_MAKE_COLOR(0x48, 0x3D, 0x8B, 0xFF) },
  { "darkslategray", LSE_MAKE_COLOR(0x2F, 0x4F, 0x4F, 0xFF) },
  { "darkslategrey", LSE_MAKE_COLOR(0x2F, 0x4F, 0x4F, 0xFF) },
  { "darkturquoise", LSE_MAKE_COLOR(0x00, 0xCE, 0xD1, 0xFF) },
  { "darkviolet", LSE_MAKE_COLOR(0x94, 0x00, 0xD3, 0xFF) },
  { "deeppink", LSE_MAKE_COLOR(0xFF, 0x14, 0x93, 0xFF) },
  { "deepskyblue", LSE_MAKE_COLOR(0x00, 0xBF, 0xFF, 0xFF) },
  { "dimgray", LSE_MAKE_COLOR(0x69, 0x69, 0x69, 0xFF) },
  { "dimgrey", LSE_MAKE_COLOR(0x69, 0x69, 0x69, 0xFF) },
  { "dodgerblue", LSE_MAKE_COLOR(0x1E, 0x90, 0xFF, 0xFF) },
  { "firebrick", LSE_MAKE_COLOR(0xB2, 0x22, 0x22, 0xFF) },
  { "floralwhite", LSE_MAKE_COLOR(0xFF, 0xFA, 0xF0, 0xFF) },
  { "forestgreen", LSE_MAKE_COLOR(0x22, 0x8B, 0x22, 0xFF) },
  { "fuchsia", LSE_MAKE_COLOR(0xFF, 0x00, 0xFF, 0xFF) },
  { "gainsboro", LSE_MAKE_COLOR(0xDC, 0xDC, 0xDC, 0xFF) },
  { "ghostwhite", LSE_MAKE_COLOR(0xF8, 0xF8, 0xFF, 0xFF) },
  { "gold", LSE_MAKE_COLOR(0xFF, 0xD7, 0x00, 0xFF) },
  { "goldenrod", LSE_MAKE_COLOR(0xDA, 0xA5, 0x20, 0xFF) },
  { "gray", LSE_MAKE_COLOR(0x80, 0x80, 0x80, 0xFF) },
  { "green", LSE_MAKE_COLOR(0x00, 0x80, 0x00, 0xFF) },
  { "greenyellow", LSE_MAKE_COLOR(0xAD, 0xFF, 0x2F, 0xFF) },
  { "grey", LSE_MAKE_COLOR(0x80, 0x80, 0x80, 0xFF) },
  { "honeydew", LSE_MAKE_COLOR(0xF0, 0xFF, 0xF0, 0xFF) },
  { "hotpink", LSE_MAKE_COLOR(0xFF, 0x69, 0xB4, 0xFF) },
  { "indianred", LSE_MAKE_COLOR(0xCD, 0x5C, 0x5C, 0xFF) },
  { "indigo", LSE_MAKE_COLOR(0x4B, 0x00, 0x82, 0xFF) },
  { "ivory", LSE_MAKE_COLOR(0xFF, 0xFF, 0xF0, 0xFF) },
  { "khaki", LSE_MAKE_COLOR(0xF0, 0xE6, 0x8C, 0xFF) },
  { "lavender", LSE_MAKE_COLOR(0xE6, 0xE6, 0xFA, 0xFF) },
  { "lavenderblush", LSE_MAKE_COLOR(0xFF, 0xF0, 0xF5, 0xFF) },
  { "lawngreen", LSE_MAKE_COLOR(0x7C, 0xFC, 0x00, 0xFF) },
  { "lemonchiffon", LSE_MAKE_COLOR(0xFF, 0xFA, 0xCD, 0xFF) },
  { "lightblue", LSE_MAKE_COLOR(0xAD, 0xD8, 0xE6, 0xFF) },
  { "lightcoral", LSE_MAKE_COLOR(0xF0, 0x80, 0x80, 0xFF) },
  { "lightcyan", LSE_MAKE_COLOR(0xE0, 0xFF, 0xFF, 0xFF) },
  { "lightgoldenrodyellow", LSE_MAKE_COLOR(0xFA, 0xFA, 0xD2, 0xFF) },
  { "lightgray", LSE_MAKE_COLOR(0xD3, 0xD3, 0xD3, 0xFF) },
  { "lightgreen", LSE_MAKE_COLOR(0x90, 0xEE, 0x90, 0xFF) },
  { "lightgrey", LSE_MAKE_COLOR(0xD3, 0xD3, 0xD3, 0xFF) },
  { "lightpink", LSE_MAKE_COLOR(0xFF, 0xB6, 0xC1, 0xFF) },
  { "lightsalmon", LSE_MAKE_COLOR(0xFF, 0xA0, 0x7A, 0xFF) },
  { "lightseagreen", LSE_MAKE_COLOR(0x20, 0xB2, 0xAA, 0xFF) },
  { "lightskyblue", LSE_MAKE_COLOR(0x87, 0xCE, 0xFA, 0xFF) },
  { "lightslategray", LSE_MAKE_COLOR(0x77, 0x88, 0x99, 0xFF) },
  { "lightslategrey", LSE_MAKE_COLOR(0x77, 0x88, 0x99, 0xFF) },
  { "lightsteelblue", LSE_MAKE_COLOR(0xB0, 0xC4, 0xDE, 0xFF) },
  { "lightyellow", LSE_MAKE_COLOR(0xFF, 0xFF, 0xE0, 0xFF) },
  { "lime", LSE_MAKE_COLOR(0x00, 0xFF, 0x00, 0xFF) },
  { "limegreen", LSE_MAKE_COLOR(0x32, 0xCD, 0x32, 0xFF) },
  { "linen", LSE_MAKE_COLOR(0xFA, 0xF0, 0xE6, 0xFF) },
  { "magenta", LSE_MAKE_COLOR(0xFF, 0x00, 0xFF, 0xFF) },
  { "maroon", LSE_MAKE_COLOR(0x80, 0x00, 0x00, 0xFF) },
  { "mediumaquamarine", LSE_MAKE_COLOR(0x66, 0xCD, 0xAA, 0xFF) },
  { "mediumblue", LSE_MAKE_COLOR(0x00, 0x00, 0xCD, 0xFF) },
  { "mediumorchid", LSE_MAKE_COLOR(0xBA, 0x55, 0xD3, 0xFF) },
  { "mediumpurple", LSE_MAKE_COLOR(0x93, 0x70, 0xDB, 0xFF) },
  { "mediumseagreen", LSE_MAKE_COLOR(0x3C, 0xB3, 0x71, 0xFF) },
  { "mediumslateblue", LSE_MAKE_COLOR(0x7B, 0x68, 0xEE, 0xFF) },
  { "mediumspringgreen", LSE_MAKE_COLOR(0x00, 0xFA, 0x9A, 0xFF) },
  { "mediumturquoise", LSE_MAKE_COLOR(0x48, 0xD1, 0xCC, 0xFF) },
  { "mediumvioletred", LSE_MAKE_COLOR(0xC7, 0x15, 0x85, 0xFF) },
  { "midnightblue", LSE_MAKE_COLOR(0x19, 0x19, 0x70, 0xFF) },
  { "mintcream", LSE_MAKE_COLOR(0xF5, 0xFF, 0xFA, 0xFF) },
  { "mistyrose", LSE_MAKE_COLOR(0xFF, 0xE4, 0xE1, 0xFF) },
  { "moccasin", LSE_MAKE_COLOR(0xFF, 0xE4, 0xB5, 0xFF) },
  { "navajowhite", LSE_MAKE_COLOR(0xFF, 0xDE, 0xAD, 0xFF) },
  { "navy", LSE_MAKE_COLOR(0x00, 0x00, 0x80, 0xFF) },
  { "oldlace", LSE_MAKE_COLOR(0xFD, 0xF5, 0xE6, 0xFF) },
  { "olive", LSE_MAKE_COLOR(0x80, 0x80, 0x00, 0xFF) },
  { "olivedrab", LSE_MAKE_COLOR(0x6B, 0x8E, 0x23, 0xFF) },
  { "orange", LSE_MAKE_COLOR(0xFF, 0xA5, 0x00, 0xFF) },
  { "orangered", LSE_MAKE_COLOR(0xFF, 0x45, 0x00, 0xFF) },
  { "orchid", LSE_MAKE_COLOR(0xDA, 0x70, 0xD6, 0xFF) },
  { "palegoldenrod", LSE_MAKE_COLOR(0xEE, 0xE8, 0xAA, 0xFF) },
  { "palegreen", LSE_MAKE_COLOR(0x98, 0xFB, 0x98, 0xFF) },
  { "paleturquoise", LSE_MAKE_COLOR(0xAF, 0xEE, 0xEE, 0xFF) },
  { "palevioletred", LSE_MAKE_COLOR(0xDB, 0x70, 0x93, 0xFF) },
  { "papayawhip", LSE_MAKE_COLOR(0xFF, 0xEF, 0xD5, 0xFF) },
  { "peachpuff", LSE_MAKE_COLOR(0xFF, 0xDA, 0xB9, 0xFF) },
  { "peru", LSE_MAKE_COLOR(0xCD, 0x85, 0x3F, 0xFF) },
  { "pink", LSE_MAKE_COLOR(0xFF, 0xC0, 0xCB, 0xFF) },
  { "plum", LSE_MAKE_COLOR(0xDD, 0xA0, 0xDD, 0xFF) },
  { "powderblue", LSE_MAKE_COLOR(0xB0, 0xE0, 0xE6, 0xFF) },
  { "purple", LSE_MAKE_COLOR(0x80, 0x00, 0x80, 0xFF) },
  { "red", LSE_MAKE_COLOR(0xFF, 0x00, 0x00, 0xFF) },
  { "rosybrown", LSE_MAKE_COLOR(0xBC, 0x8F, 0x8F, 0xFF) },
  { "royalblue", LSE_MAKE_COLOR(0x41, 0x69, 0xE1, 0xFF) },
  { "saddlebrown", LSE_MAKE_COLOR(0x8B, 0x45, 0x13, 0xFF) },
  { "salmon", LSE_MAKE_COLOR(0xFA, 0x80, 0x72, 0xFF) },
  { "sandybrown", LSE_MAKE_COLOR(0xF4, 0xA4, 0x60, 0xFF) },
  { "seagreen", LSE_MAKE_COLOR(0x2E, 0x8B, 0x57, 0xFF) },
  { "seashell", LSE_MAKE_COLOR(0xFF, 0xF5, 0xEE, 0xFF) },
  { "sienna", LSE_MAKE_COLOR(0xA0, 0x52, 0x2D, 0xFF) },
  { "silver", LSE_MAKE_COLOR(0xC0, 0xC0, 0xC0, 0xFF) },
  { "skyblue", LSE_MAKE_COLOR(0x87, 0xCE, 0xEB, 0xFF) },
  { "slateblue", LSE_MAKE_COLOR(0x6A, 0x5A, 0xCD, 0xFF) },
  { "slategray", LSE_MAKE_COLOR(0x70, 0x80, 0x90, 0xFF) },
  { "slategrey", LSE_MAKE_COLOR(0x70, 0x80, 0x90, 0xFF) },
  { "snow", LSE_MAKE_COLOR(0xFF, 0xFA, 0xFA, 0xFF) },
  { "springgreen", LSE_MAKE_COLOR(0x00, 0xFF, 0x7F, 0xFF) },
  { "steelblue", LSE_MAKE_COLOR(0x46, 0x82, 0xB4, 0xFF) },
  { "tan", LSE_MAKE_COLOR(0xD2, 0xB4, 0x8C, 0xFF) },
  { "teal", LSE_MAKE_COLOR(0x00, 0x80, 0x80, 0xFF) },
  { "thistle", LSE_MAKE_COLOR(0xD8, 0xBF, 0xD8, 0xFF) },
  { "tomato", LSE_MAKE_COLOR(0xFF, 0x63, 0x47, 0xFF) },
  { "turquoise", LSE_MAKE_COLOR(0x40, 0xE0, 0xD0, 0xFF) },
  { "violet", LSE_MAKE_COLOR(0xEE, 0x82, 0xEE, 0xFF) },
  { "wheat", LSE_MAKE_COLOR(0xF5, 0xDE, 0xB3, 0xFF) },
  { "white", LSE_MAKE_COLOR(0xFF, 0xFF, 0xFF, 0xFF) },
  { "whitesmoke", LSE_MAKE_COLOR(0xF5, 0xF5, 0xF5, 0xFF) },
  { "yellow", LSE_MAKE_COLOR(0xFF, 0xFF, 0x00, 0xFF) },
  { "yellowgreen", LSE_MAKE_COLOR(0x9A, 0xCD, 0x32, 0xFF) },
  { "rebeccapurple", LSE_MAKE_COLOR(0x66, 0x33, 0x99, 0xFF) },
  { "transparent", { 0 } },
};
static cmap_color_keyword_table k_color_keyword_table = _cmap_inits;
static cset_properties k_defined_properties = _cmap_inits;
static const lse_style_value k_undefined_value = { .unit = LSE_STYLE_UNIT_UNDEFINED, .value = LSE_UNDEFINED };
static lse_style* k_empty_style = NULL;
#define LSE_PI_F (float)3.141592653589793238462643383279502884L
static const float k_deg_to_rad = (LSE_PI_F) / 180.f;
static const float k_grad_to_rad = (LSE_PI_F)/ 200.f;
static const float k_turn_to_rad = (LSE_PI_F) * 2.f;

static bool parse_color_hex(const char* str, uint32_t* color);
static void init_color_keyword_table();
static void clear_properties(lse_style* style);
static void cleanup_before_erase(cmap_properties_value* entry);
static void gather_defined_properties(lse_style* style, cset_properties* defined);
static float compute_object_coordinate(
    lse_style* style,
    const lse_style_context* context,
    lse_style_property coordinate_property,
    int32_t coordinate_axis,
    float box_length,
    float object_length,
    float default_percent);
static float compute_background_size_dimension(
    lse_style* style,
    const lse_style_context* context,
    lse_style_property property,
    float dimension,
    float auto_dimension);
static float
resolve_numeric_or_default(const lse_style_value* numeric, const lse_style_context* context, float fallback);
static float compute_rotate(lse_style_value value);
static float compute_translate_axis(lse_style_value value, const lse_style_context* context, float box_bound);
static float compute_scale(lse_style_value value);
static float
compute_transform_origin(const lse_style_value* numeric, const lse_style_context* context, float box_bound);

static void constructor(lse_object* object, void* arg) {
  lse_style* style = (lse_style*)object;

  // TODO: set capacity?
  style->properties = cmap_properties_init();
}

static void destructor(lse_object* object) {
  lse_style* style = (lse_style*)object;

  lse_unref(style->parent);

  clear_properties(style);
  cmap_properties_drop(&style->properties);
}

LSE_API lse_style* LSE_CDECL lse_style_new() {
  return (lse_style*)lse_object_new(lse_style_type, NULL);
}

LSE_API lse_style* LSE_CDECL lse_style_new_class() {
  lse_style* result = lse_style_new();

  result->is_class = true;

  return result;
}

LSE_API bool LSE_CDECL lse_style_is_class(lse_style* style) {
  return style->is_class;
}

LSE_API bool LSE_CDECL lse_style_set_parent(lse_style* style, lse_style* parent) {
  if (parent && !parent->is_class) {
    return false;
  }

  lse_style* old_parent = style->parent;
  lse_style* new_parent = parent;

  if (style->node) {
    cset_properties_clear(&k_defined_properties);

    if (new_parent) {
      gather_defined_properties(new_parent, &k_defined_properties);
    }

    if (old_parent) {
      gather_defined_properties(old_parent, &k_defined_properties);
    }

    c_foreach(i, cmap_properties, style->properties) {
      cset_properties_erase(&k_defined_properties, i.ref->first);
    }
  }

  lse_unref(old_parent);
  lse_ref(new_parent);
  style->parent = new_parent;

  if (style->node) {
    c_foreach(i, cset_properties, k_defined_properties) {
      lse_node_on_style_property_change(style->node, *i.ref);
    }
  }

  return true;
}

LSE_API lse_style* lse_style_get_parent(lse_style* style) {
  return style->parent;
}

LSE_API lse_status LSE_CDECL lse_style_lock(lse_style* style) {
  style->is_locked = true;

  return LSE_OK;
}

LSE_API bool LSE_CDECL lse_style_is_locked(lse_style* style) {
  return style->is_locked;
}

LSE_API bool LSE_CDECL lse_style_has_property(lse_style* style, lse_style_property prop) {
  if (cmap_properties_find(&style->properties, prop).ref != cmap_properties_end(&style->properties).ref) {
    return true;
  } else if (style->parent) {
    return lse_style_has_property(style->parent, prop);
  }

  return false;
}

LSE_API bool LSE_CDECL lse_style_unset(lse_style* style, lse_style_property prop) {
  cmap_properties_iter it = cmap_properties_find(&style->properties, prop);

  if (it.ref != cmap_properties_end(&style->properties).ref) {
    if (style->node) {
      lse_node_on_style_property_change(style->node, prop);
    }

    cleanup_before_erase(it.ref);
    cmap_properties_erase_at(&style->properties, it);

    return true;
  }

  return false;
}

LSE_API bool LSE_CDECL lse_style_reset(lse_style* style) {
  if (style->is_locked) {
    return false;
  }

  if (style->node) {
    c_foreach(i, cmap_properties, style->properties) {
      lse_node_on_style_property_change(style->node, i.ref->first);
    }
  }

  cmap_properties_clear(&style->properties);

  return true;
}

LSE_API bool LSE_CDECL lse_style_from_string(lse_style* style, lse_style_property prop, lse_string* str) {
  if (!str) {
    return false;
  }

  switch (lse_style_meta_get_property_type(prop)) {
    case LSE_STYLE_PROPERTY_TYPE_NUMBER: {
      lse_style_value numeric;
      return lse_style_parse_numeric(str, &numeric) && lse_style_set_numeric(style, prop, &numeric);
    }
    case LSE_STYLE_PROPERTY_TYPE_ENUM: {
      int32_t enum_value = lse_style_meta_enum_from_string(prop, lse_string_as_cstring(str));

      lse_unref(str);

      return lse_style_set_enum(style, prop, enum_value);
    }
    case LSE_STYLE_PROPERTY_TYPE_COLOR: {
      uint32_t color;
      return lse_style_parse_color(str, &color) && lse_style_set_color(style, prop, color);
    }
    case LSE_STYLE_PROPERTY_TYPE_STRING:
      return lse_style_set_object(style, prop, (lse_object*)str);
    default:
      lse_unref(str);
      return false;
  }
}

LSE_API bool LSE_CDECL lse_style_set_numeric(lse_style* style, lse_style_property prop, const lse_style_value* value) {
  if (lse_style_is_locked(style)) {
    return false;
  }

  bool is_numeric = lse_style_meta_get_property_type(prop) == LSE_STYLE_PROPERTY_TYPE_NUMBER;

  if (is_numeric) {
    // TODO: validate
    cmap_properties_insert_or_assign(&style->properties, prop, (style_property){ .u.numeric = *value });

    if (style->node) {
      lse_node_on_style_property_change(style->node, prop);
    }
  }

  // TODO: same value?
  // TODO: update node

  return is_numeric;
}

LSE_API bool LSE_CDECL lse_style_set_enum(lse_style* style, lse_style_property prop, int32_t value) {
  if (lse_style_is_locked(style)) {
    return false;
  }

  bool is_enum = lse_style_meta_get_property_type(prop) == LSE_STYLE_PROPERTY_TYPE_ENUM &&
                 lse_style_meta_is_enum_value(prop, value);

  if (is_enum) {
    cmap_properties_insert_or_assign(&style->properties, prop, (style_property){ .u.enum_value = value });

    if (style->node) {
      lse_node_on_style_property_change(style->node, prop);
    }
  }

  // TODO: same value?
  // TODO: update node

  return is_enum;
}

LSE_API bool LSE_CDECL lse_style_set_object(lse_style* style, lse_style_property prop, lse_object* value) {
  lse_style_property_type prop_type;
  cmap_properties_result result;

  if (lse_style_is_locked(style)) {
    prop_type = LSE_STYLE_PROPERTY_TYPE_UNKNOWN;
  } else {
    prop_type = lse_style_meta_get_property_type(prop);
  }

  switch (prop_type) {
    case LSE_STYLE_PROPERTY_TYPE_STRING:
      // TODO: validate
      // TODO: same value?
      // TODO: update node
      // TODO: unref if set
      // TODO: empty string?
      result = cmap_properties_insert_or_assign(
          &style->properties,
          prop,
          (style_property){
              .u.object = value,
          });
      break;
    case LSE_STYLE_PROPERTY_TYPE_FILTER:
      // TODO: validate
      result = cmap_properties_insert_or_assign(
          &style->properties,
          prop,
          (style_property){
              .u.object = value,
          });
      break;
    case LSE_STYLE_PROPERTY_TYPE_TRANSFORM:
      // TODO: validate
      result = cmap_properties_insert_or_assign(
          &style->properties,
          prop,
          (style_property){
              .u.object = value,
          });
      break;
    default:
      result = (cmap_properties_result){ .inserted = false, .ref = NULL };
      break;
  }

  if (result.inserted) {
    if (style->node) {
      lse_node_on_style_property_change(style->node, prop);
    }
    return true;
  } else {
    lse_object_unref(value);
    return false;
  }
}

LSE_API bool LSE_CDECL lse_style_set_color(lse_style* style, lse_style_property prop, uint32_t value) {
  if (lse_style_is_locked(style)) {
    return false;
  }

  bool is_color = lse_style_meta_get_property_type(prop) == LSE_STYLE_PROPERTY_TYPE_COLOR;

  if (is_color) {
    // TODO: validate
    cmap_properties_insert_or_assign(&style->properties, prop, (style_property){ .u.color = value });
    if (style->node) {
      lse_node_on_style_property_change(style->node, prop);
    }
  }

  // TODO: same value?
  // TODO: update node

  return is_color;
}

LSE_API const lse_style_value* LSE_CDECL lse_style_get_numeric(lse_style* style, lse_style_property prop) {
  cmap_properties_iter it = cmap_properties_find(&style->properties, prop);

  if (it.ref != cmap_properties_end(&style->properties).ref) {
    return &it.ref->second.u.numeric;
  } else if (style->parent) {
    return lse_style_get_numeric(style->parent, prop);
  }

  return &k_undefined_value;
}

LSE_API int32_t LSE_CDECL lse_style_get_enum(lse_style* style, lse_style_property prop) {
  cmap_properties_iter it = cmap_properties_find(&style->properties, prop);

  if (it.ref != cmap_properties_end(&style->properties).ref) {
    return it.ref->second.u.enum_value;
  } else if (style->parent) {
    return lse_style_get_enum(style->parent, prop);
  }

  return 0;
}

LSE_API uint32_t LSE_CDECL lse_style_get_color(lse_style* style, lse_style_property prop) {
  cmap_properties_iter it = cmap_properties_find(&style->properties, prop);

  if (it.ref != cmap_properties_end(&style->properties).ref) {
    return it.ref->second.u.color;
  } else if (style->parent) {
    return lse_style_get_color(style->parent, prop);
  }

  return 0;
}

LSE_API lse_object* LSE_CDECL lse_style_get_object(lse_style* style, lse_style_property prop) {
  cmap_properties_iter it = cmap_properties_find(&style->properties, prop);

  // TODO: validate object property

  if (it.ref != cmap_properties_end(&style->properties).ref) {
    return it.ref->second.u.object;
  } else if (style->parent) {
    return lse_style_get_object(style->parent, prop);
  }

  return NULL;
}

lse_style* lse_style_get_empty() {
  if (!k_empty_style) {
    k_empty_style = lse_style_new();
  }

  return k_empty_style;
}

void lse_style_bind_node(lse_style* style, lse_node* node) {
  // TODO: assert !class
  style->node = node;
}

float lse_style_resolve_opacity(lse_style* style) {
  const lse_style_value* opacity = lse_style_get_numeric(style, LSE_SP_OPACITY);

  switch (opacity ? opacity->unit : LSE_STYLE_UNIT_UNDEFINED) {
    case LSE_STYLE_UNIT_PX:
      return lse_clamp_f(opacity->value, 0.f, 1.f);
    case LSE_STYLE_UNIT_PERCENT:
      return lse_clamp_f(opacity->value / 100.f, 0.f, 1.f);
    default:
      return 1.f;
  }
}

float lse_style_resolve_numeric(const lse_style_value* numeric, const lse_style_context* context) {
  switch (numeric->unit) {
    CASE_COMPUTE_PX_PT(numeric, context);
    CASE_COMPUTE_REM(numeric, context);
    CASE_COMPUTE_VW_VH_VMIN_VMAX(numeric, context);
    case LSE_STYLE_UNIT_UNDEFINED:
      return LSE_UNDEFINED;
    default:
      // function is only for pixel-ish unit values
      assert(false && numeric->unit);
      return 0;
  }
}

static float
resolve_numeric_or_default(const lse_style_value* numeric, const lse_style_context* context, float fallback) {
  switch (numeric->unit) {
    CASE_COMPUTE_PX_PT(numeric, context);
    CASE_COMPUTE_REM(numeric, context);
    CASE_COMPUTE_VW_VH_VMIN_VMAX(numeric, context);
    default:
      return fallback;
  }
}

bool lse_style_parse_color(lse_string* str, uint32_t* color) {
  const char* cstring = lse_string_as_cstring(str);
  bool success = parse_color_hex(cstring, color);

  if (!success) {
    if (k_color_keyword_table.size == 0) {
      init_color_keyword_table();
    }

    cmap_color_keyword_table_iter it = cmap_color_keyword_table_find(&k_color_keyword_table, cstring);

    if (it.ref != cmap_color_keyword_table_end(&k_color_keyword_table).ref) {
      *color = it.ref->second.value;
      success = true;
    }
  }

  lse_unref(str);

  return success;
}

bool lse_style_parse_numeric(lse_string* str, lse_style_value* numeric) {
  const char* cstring = lse_string_as_cstring(str);
  int32_t enum_value;
  bool result = false;

  if (isalpha(cstring[0])) {
    if (c_strncasecmp(cstring, LSE_STYLE_AUTO, strlen(LSE_STYLE_AUTO)) == 0) {
      numeric->value = LSE_UNDEFINED;
      numeric->unit = LSE_STYLE_UNIT_AUTO;
      result = true;
    } else if ((enum_value = lse_style_meta_anchor_from_string(cstring)) >= 0) {
      numeric->value = (float)enum_value;
      numeric->unit = LSE_STYLE_UNIT_ANCHOR;
      result = true;
    }
  } else if (cstring[0] == '-' || cstring[0] == '.' || isdigit(cstring[0])) {
    char* end = NULL;
    float parsed = strtof(cstring, &end);

    if (end && *end == '\0') {
      numeric->value = parsed;
      numeric->unit = LSE_STYLE_UNIT_PX;
      result = true;
    } else if (end && strcmp(end, cstring) != 0) {
      enum_value = lse_style_meta_unit_from_postfix_string(end);

      if (enum_value > 0) {
        numeric->value = parsed;
        numeric->unit = enum_value;
        result = true;
      }
    }
  }

  lse_unref(str);

  return result;
}

static bool parse_color_hex(const char* str, uint32_t* color) {
#define BUFFER_SIZE 11

  char buffer[BUFFER_SIZE];
  char* end;

  if (str[0] != '#') {
    return false;
  }

  switch (strlen(str)) {
    case 4: // #RGB
      snprintf(buffer, BUFFER_SIZE, "0x%c%c%c%c%c%cFF", str[1], str[1], str[2], str[2], str[3], str[3]);
      break;
    case 5: // #RGBA
      snprintf(
          buffer, BUFFER_SIZE, "0x%c%c%c%c%c%c%c%c", str[1], str[1], str[2], str[2], str[3], str[3], str[4], str[4]);
      break;
    case 7: // #RRGGBB
      snprintf(buffer, BUFFER_SIZE, "0x%sFF", &str[1]);
      break;
    case 9: // #RRGGBBAA
      snprintf(
          buffer, BUFFER_SIZE, "0x%c%c%c%c%c%c%c%c", str[1], str[2], str[3], str[4], str[5], str[6], str[7], str[8]);
      break;
    default:
      return false;
  }

  *color = (uint32_t)strtoul(buffer, &end, 16);

  return !(strcmp(end, str) == 0 || *end != '\0');

#undef BUFFER_SIZE
}

static void cleanup_before_erase(cmap_properties_value* entry) {
  switch (lse_style_meta_get_property_type(entry->first)) {
    case LSE_STYLE_PROPERTY_TYPE_STRING:
    case LSE_STYLE_PROPERTY_TYPE_FILTER:
    case LSE_STYLE_PROPERTY_TYPE_TRANSFORM:
      lse_object_unref(entry->second.u.object);
      break;
    default:
      break;
  }
}

static void gather_defined_properties(lse_style* style, cset_properties* defined) {
  if (!style) {
    return;
  }

  c_foreach(i, cmap_properties, style->properties) {
    cset_properties_insert(defined, i.ref->first);
  }

  gather_defined_properties(style->parent, defined);
}

static void clear_properties(lse_style* style) {
  c_foreach(i, cmap_properties, style->properties) {
    cleanup_before_erase(i.ref);
  }

  cmap_properties_clear(&style->properties);
}

static void init_color_keyword_table() {
  k_color_keyword_table = cmap_color_keyword_table_init();

  for (size_t i = 0; i < c_arraylen(kColorKeywords); i++) {
    cmap_color_keyword_table_insert(&k_color_keyword_table, kColorKeywords[i].name, kColorKeywords[i].color);
  }
}

lse_border_radius lse_style_compute_border_radius(lse_style* style, const lse_style_context* context) {
  float border_radius = resolve_numeric_or_default(lse_style_get_numeric(style, LSE_SP_BORDER_RADIUS), context, 0);

  return (lse_border_radius){
    .top_left =
        resolve_numeric_or_default(lse_style_get_numeric(style, LSE_SP_BORDER_RADIUS_TOP_LEFT), context, border_radius),
    .top_right = resolve_numeric_or_default(
        lse_style_get_numeric(style, LSE_SP_BORDER_RADIUS_TOP_RIGHT), context, border_radius),
    .bottom_left = resolve_numeric_or_default(
        lse_style_get_numeric(style, LSE_SP_BORDER_RADIUS_BOTTOM_LEFT), context, border_radius),
    .bottom_right = resolve_numeric_or_default(
        lse_style_get_numeric(style, LSE_SP_BORDER_RADIUS_BOTTOM_RIGHT), context, border_radius)
  };
}

bool lse_style_has_border_layout(lse_node* node) {
  YGNodeRef yg_node = lse_node_get_base(node)->yg_node;

  return YGNodeLayoutGetBorder(yg_node, YGEdgeTop) > 0 || YGNodeLayoutGetBorder(yg_node, YGEdgeRight) > 0 ||
         YGNodeLayoutGetBorder(yg_node, YGEdgeBottom) > 0 || YGNodeLayoutGetBorder(yg_node, YGEdgeLeft) > 0;
}

lse_border_rect lse_style_get_border_edges(lse_node* node) {
  YGNodeRef yg_node = lse_node_get_base(node)->yg_node;

  return (lse_border_rect){
    .top = YGNodeLayoutGetBorder(yg_node, YGEdgeTop),
    .right = YGNodeLayoutGetBorder(yg_node, YGEdgeRight),
    .bottom = YGNodeLayoutGetBorder(yg_node, YGEdgeBottom),
    .left = YGNodeLayoutGetBorder(yg_node, YGEdgeLeft),
  };
}

bool lse_style_has_transform(lse_style* style) {
  lse_array* transform = (lse_array*)lse_style_get_object(style, LSE_SP_TRANSFORM);

  return transform && lse_array_get_length(transform) > 0;
}

float lse_style_compute_border_only(lse_style* style, const lse_style_context* context) {
  return resolve_numeric_or_default(lse_style_get_numeric(style, LSE_SP_BORDER), context, 0);
}

float lse_style_compute_font_size(lse_style* style, const lse_style_context* context) {
  return resolve_numeric_or_default(lse_style_get_numeric(style, LSE_SP_FONT_SIZE), context, LSE_DEFAULT_FONT_SIZE_PX);
}

int32_t lse_style_compute_max_lines(lse_style* style, const lse_style_context* context) {
  const lse_style_value* value = lse_style_get_numeric(style, LSE_SP_FONT_SIZE);

  if (value->unit == LSE_STYLE_UNIT_PX) {
    return (int32_t)value->value;
  }

  return 0;
}

lse_matrix lse_style_compute_transform(lse_style* style, const lse_style_context* context, const lse_rect_f* box) {
  const lse_matrix identity = lse_matrix_init();
  lse_matrix temp;
  lse_matrix result;
  lse_point_f origin = lse_style_compute_transform_origin(style, context, box);
  lse_array* transform_list = (lse_array*)lse_style_get_object(style, LSE_SP_TRANSFORM);
  size_t len = transform_list ? lse_array_get_length(transform_list) : 0;
  size_t i;
  lse_style_transform* transform;

  result = lse_matrix_init_translate(origin.x, origin.y);

  for (i = 0; i < len; i++) {
    transform = lse_array_at(transform_list, i);

    switch (transform->op) {
      case LSE_STYLE_TRANSFORM_IDENTITY:
        lse_matrix_multiply(&result, &identity, &result);
        break;
      case LSE_STYLE_TRANSFORM_TRANSLATE:
        temp = lse_matrix_init_translate(
            compute_translate_axis(lse_style_transform_get_translate_x(transform), context, box->width),
            compute_translate_axis(lse_style_transform_get_translate_y(transform), context, box->height));
        lse_matrix_multiply(&result, &temp, &result);
        break;
      case LSE_STYLE_TRANSFORM_SCALE:
        temp = lse_matrix_init_scale(
            compute_scale(lse_style_transform_get_scale_x(transform)),
            compute_scale(lse_style_transform_get_scale_y(transform)));
        lse_matrix_multiply(&result, &temp, &result);
        break;
      case LSE_STYLE_TRANSFORM_ROTATE:
        temp = lse_matrix_init_rotate(compute_rotate(lse_style_transform_get_angle(transform)));
        lse_matrix_multiply(&result, &temp, &result);
        break;
      default:
        return identity;
    }
  }

  temp = lse_matrix_init_translate(-origin.x, -origin.y);

  lse_matrix_multiply(&result, &temp, &result);

  return result;
}

lse_point_f
lse_style_compute_transform_origin(lse_style* style, const lse_style_context* context, const lse_rect_f* box) {
  return (lse_point_f){
    compute_transform_origin(lse_style_get_numeric(style, LSE_SP_TRANSFORM_ORIGIN_X), context, box->width),
    compute_transform_origin(lse_style_get_numeric(style, LSE_SP_TRANSFORM_ORIGIN_Y), context, box->height)
  };
}

float lse_style_compute_font_size_from_root(lse_style* style, const lse_style_context* context) {
  const lse_style_value* value = lse_style_get_numeric(style, LSE_SP_FONT_SIZE);

  switch (value->unit) {
    case LSE_STYLE_UNIT_REM:
      return value->value * LSE_DEFAULT_FONT_SIZE_PX;
    case LSE_STYLE_UNIT_UNDEFINED:
      return LSE_DEFAULT_FONT_SIZE_PX;
    default:
      return resolve_numeric_or_default(value, context, 0);
  }
}

void lse_style_compute_object_fit(
    lse_style* style,
    const lse_style_context* context,
    lse_rect_f* box,
    lse_image* image,
    lse_rect_f* out) {
  // TODO: check box empty
  // TODO: check image has dimensions
  lse_style_object_fit object_fit = lse_style_get_enum(style, LSE_SP_OBJECT_FIT);
  float image_width = (float)lse_image_get_width(image);
  float image_height = (float)lse_image_get_height(image);
  float aspect_ratio = image_height > 0 ? image_width / image_height : 0;

  if (object_fit == LSE_STYLE_OBJECT_FIT_SCALE_DOWN) {
    if (image_width > box->width || image_height > box->height) {
      object_fit = LSE_STYLE_OBJECT_FIT_CONTAIN;
    } else {
      object_fit = LSE_STYLE_OBJECT_FIT_NONE;
    }
  }

  switch (object_fit) {
    case LSE_STYLE_OBJECT_FIT_CONTAIN:
      if (aspect_ratio > (box->width / box->height)) {
        out->width = box->width;
        out->height = box->width / aspect_ratio;
      } else {
        out->width = box->height * aspect_ratio;
        out->height = box->height;
      }
      break;
    case LSE_STYLE_OBJECT_FIT_COVER:
      if (aspect_ratio > (box->width / box->height)) {
        out->width = box->height * aspect_ratio;
        out->height = box->height;
      } else {
        out->width = box->width;
        out->height = box->width / aspect_ratio;
      }
      break;
    case LSE_STYLE_OBJECT_FIT_NONE:
      out->width = image_width;
      out->height = image_height;
      break;
    default:
      out->width = box->width;
      out->height = box->height;
      break;
  }

  out->x = box->x + compute_object_coordinate(
                        style, context, LSE_SP_OBJECT_POSITION_X, COORDINATE_AXIS_X, box->width, out->width, .5f);

  out->y = box->y + compute_object_coordinate(
                        style, context, LSE_SP_OBJECT_POSITION_Y, COORDINATE_AXIS_Y, box->height, out->height, .5f);
}

void lse_style_compute_background_fit(
    lse_style* style,
    const lse_style_context* context,
    lse_rect_f* box,
    lse_image* image,
    lse_rect_f* out) {
  float image_width = (float)lse_image_get_width(image);
  float image_height = (float)lse_image_get_height(image);
  float aspect_ratio = image_height > 0 ? image_width / image_height : 0;
  ;

  switch (lse_style_get_enum(style, LSE_SP_BACKGROUND_SIZE)) {
    case LSE_STYLE_BACKGROUND_SIZE_CONTAIN:
      if (aspect_ratio > (box->width / box->height)) {
        out->width = box->width;
        out->height = box->width / aspect_ratio;
      } else {
        out->width = box->height * aspect_ratio;
        out->height = box->height;
      }
      break;
    case LSE_STYLE_BACKGROUND_SIZE_COVER:
      if (aspect_ratio > (box->width / box->height)) {
        out->width = box->height * aspect_ratio;
        out->height = box->height;
      } else {
        out->width = box->width;
        out->height = box->width / aspect_ratio;
      }
      break;
    default:
      out->width = compute_background_size_dimension(style, context, LSE_SP_BACKGROUND_WIDTH, box->width, image_width);
      out->height =
          compute_background_size_dimension(style, context, LSE_SP_BACKGROUND_HEIGHT, box->height, image_height);
      break;
  }

  out->x = box->x + compute_object_coordinate(
                        style, context, LSE_SP_BACKGROUND_POSITION_X, COORDINATE_AXIS_X, box->width, out->width, 0);
  out->y = box->y + compute_object_coordinate(
                        style, context, LSE_SP_BACKGROUND_POSITION_Y, COORDINATE_AXIS_Y, box->height, out->height, 0);
}

static float compute_object_coordinate(
    lse_style* style,
    const lse_style_context* context,
    lse_style_property coordinate_property,
    int32_t coordinate_axis,
    float box_length,
    float object_length,
    float default_percent) {
  const lse_style_value* value = lse_style_get_numeric(style, coordinate_property);
  float percent;

  switch (value->unit) {
    CASE_COMPUTE_PX_PT(value, context);
    CASE_COMPUTE_REM(value, context);
    CASE_COMPUTE_VW_VH_VMIN_VMAX(value, context);
    case LSE_STYLE_UNIT_ANCHOR:
      switch ((int32_t)value->value) {
        case LSE_STYLE_ANCHOR_RIGHT:
          return coordinate_axis == COORDINATE_AXIS_X ? box_length - object_length : 0;
        case LSE_STYLE_ANCHOR_BOTTOM:
          return coordinate_axis == COORDINATE_AXIS_Y ? box_length - object_length : 0;
        case LSE_STYLE_ANCHOR_CENTER:
          return box_length * .5f - object_length * .5f;
        default:
          return 0;
      }
    case LSE_STYLE_UNIT_PERCENT:
      percent = value->value / 100.f;
      return (box_length * percent - object_length * percent);
    case LSE_STYLE_UNIT_UNDEFINED:
      return box_length * default_percent - object_length * default_percent;
    default:
      // unsupported unit for this property. should be caught in validation on property set.
      assert(false || value->unit);
      return 0;
  }
}

static float compute_background_size_dimension(
    lse_style* style,
    const lse_style_context* context,
    lse_style_property property,
    float dimension,
    float auto_dimension) {
  const lse_style_value* value = lse_style_get_numeric(style, property);

  switch (value->unit) {
    CASE_COMPUTE_PX_PT(value, context);
    CASE_COMPUTE_REM(value, context);
    CASE_COMPUTE_VW_VH_VMIN_VMAX(value, context);
    case LSE_STYLE_UNIT_PERCENT:
      return (value->value / 100.f) * dimension;
    case LSE_STYLE_UNIT_AUTO:
    case LSE_STYLE_UNIT_UNDEFINED:
      return auto_dimension;
    default:
      // TODO: assert?
      return 0;
  }
}

void lse_style_update_dynamic_units(lse_node* node, bool view_units, bool rem_units) {
  lse_style* style = lse_node_get_style(node);

  if (style) {
    c_foreach(it, cmap_properties, style->properties) {
      switch (lse_style_meta_get_property_type(it.ref->first)) {
        case LSE_STYLE_PROPERTY_TYPE_NUMBER:
          switch (it.ref->second.u.numeric.unit) {
            case LSE_STYLE_UNIT_VW:
            case LSE_STYLE_UNIT_VH:
            case LSE_STYLE_UNIT_VMIN:
            case LSE_STYLE_UNIT_VMAX:
              if (view_units) {
                lse_node_on_style_property_change(node, it.ref->first);
              }
              break;
            case LSE_STYLE_UNIT_REM:
              if (rem_units) {
                lse_node_on_style_property_change(node, it.ref->first);
              }
              break;
            default:
              break;
          }
          break;
        case LSE_STYLE_PROPERTY_TYPE_TRANSFORM:
        case LSE_STYLE_PROPERTY_TYPE_FILTER:
          // TODO: check if filter or transform actually have view or rem unit values inside to avoid extra draws
          lse_node_on_style_property_change(node, it.ref->first);
          break;
        default:
          break;
      }
    }
  }

  if (!lse_node_is_leaf(node)) {
    size_t i;
    size_t count = lse_node_get_child_count(node);

    for (i = 0; i < count; i++) {
      lse_style_update_dynamic_units(node, view_units, rem_units);
    }
  }
}

static YGNodeRef get_yg_node(lse_style* style) {
  assert(style != NULL);
  return style->node ? lse_node_get_base(style->node)->yg_node : NULL;
}

lse_text_style lse_style_as_text_style(
    lse_style* style,
    const lse_style_context* context,
    lse_font* font,
    const lse_size* content_box) {
  return (lse_text_style){
    .font = font,
    .font_size_px = lse_style_compute_font_size(style, context),
    .max_lines = lse_style_compute_max_lines(style, context),
    .context_box = *content_box,
    .transform = lse_style_get_enum(style, LSE_SP_TEXT_TRANSFORM),
    .align = lse_style_get_enum(style, LSE_SP_TEXT_ALIGN),
    .overflow = lse_style_get_enum(style, LSE_SP_TEXT_OVERFLOW),
    .overflow_wrap = lse_style_get_enum(style, LSE_SP_OVERFLOW_WRAP),
    .white_space = lse_style_get_enum(style, LSE_SP_WHITE_SPACE),
    .kerning_enabled =
        (lse_style_get_enum(style, LSE_SP_WHITE_SPACE) == LSE_STYLE_FONT_KERNING_NORMAL) && lse_font_has_kerning(font),
  };
}

LSE_API lse_object* lse_style_transform_new(lse_style_transform* list, size_t len) {
  lse_array* result = lse_array_new(len, sizeof(lse_style_transform), NULL);

  if (len > 0) {
    memcpy(lse_array_at(result, 0), list, len * sizeof(lse_style_transform));
  }

  return (lse_object*)result;
}

LSE_API bool lse_style_transform_identity(lse_style_transform* out) {
  *out = (lse_style_transform){
    .op = LSE_STYLE_TRANSFORM_IDENTITY,
    .a = k_undefined_value,
    .b = k_undefined_value,
  };

  return true;
}

LSE_API bool lse_style_transform_translate(lse_style_value x, lse_style_value y, lse_style_transform* out) {
  // TODO: validate xy
  *out = (lse_style_transform){
    .op = LSE_STYLE_TRANSFORM_TRANSLATE,
    .a = x,
    .b = y,
  };

  return true;
}

LSE_API bool lse_style_transform_scale(lse_style_value x, lse_style_value y, lse_style_transform* out) {
  // TODO: validate xy
  *out = (lse_style_transform){
    .op = LSE_STYLE_TRANSFORM_SCALE,
    .a = x,
    .b = y,
  };

  return true;
}

LSE_API bool lse_style_transform_rotate(lse_style_value angle, lse_style_transform* out) {
  *out = (lse_style_transform){
    .op = LSE_STYLE_TRANSFORM_ROTATE,
    .a = angle,
    .b = k_undefined_value,
  };

  return true;
}

LSE_API lse_style_transform_op lse_style_transform_get_op(lse_style_transform* transform) {
  return transform->op;
}

LSE_API lse_style_value lse_style_transform_get_translate_x(lse_style_transform* transform) {
  return transform->op == LSE_STYLE_TRANSFORM_TRANSLATE ? transform->a : k_undefined_value;
}

LSE_API lse_style_value lse_style_transform_get_translate_y(lse_style_transform* transform) {
  return transform->op == LSE_STYLE_TRANSFORM_TRANSLATE ? transform->b : k_undefined_value;
}

LSE_API lse_style_value lse_style_transform_get_scale_x(lse_style_transform* transform) {
  return transform->op == LSE_STYLE_TRANSFORM_SCALE ? transform->a : k_undefined_value;
}

LSE_API lse_style_value lse_style_transform_get_scale_y(lse_style_transform* transform) {
  return transform->op == LSE_STYLE_TRANSFORM_SCALE ? transform->b : k_undefined_value;
}

LSE_API lse_style_value lse_style_transform_get_angle(lse_style_transform* transform) {
  return transform->op == LSE_STYLE_TRANSFORM_ROTATE ? transform->a : k_undefined_value;
}

LSE_API lse_object* lse_style_filter_new(lse_style_filter* list, size_t len) {
  lse_array* result = lse_array_new(len, sizeof(lse_style_filter), NULL);

  if (len > 0) {
    memcpy(lse_array_at(result, 0), list, len * sizeof(lse_style_filter));
  }

  return (lse_object*)result;
}

LSE_API bool lse_style_filter_flip_h(lse_style_filter* out) {
  *out = (lse_style_filter){
    .func = LSE_STYLE_FILTER_FLIP_H,
    .color = 0,
  };

  return true;
}

LSE_API bool lse_style_filter_flip_v(lse_style_filter* out) {
  *out = (lse_style_filter){
    .func = LSE_STYLE_FILTER_FLIP_V,
    .color = 0,
  };

  return true;
}

LSE_API bool lse_style_filter_tint(uint32_t color, lse_style_filter* out) {
  *out = (lse_style_filter){
    .func = LSE_STYLE_FILTER_TINT,
    .color = color,
  };

  return true;
}

LSE_API lse_style_filter_function lse_style_filter_get_function(lse_style_filter* filter) {
  return filter->func;
}

LSE_API uint32_t lse_style_filter_get_color(lse_style_filter* filter) {
  return filter->func == LSE_STYLE_FILTER_TINT ? filter->color : 0;
}

static float compute_rotate(lse_style_value value) {
  switch (value.unit) {
    // TODO: the parser assigns PX to raw numbers.. treat it as radian
    case LSE_STYLE_UNIT_PX:
    case LSE_STYLE_UNIT_RAD:
      return value.value;
    case LSE_STYLE_UNIT_DEG:
      return value.value * k_deg_to_rad;
    case LSE_STYLE_UNIT_GRAD:
      return value.value * k_grad_to_rad;
    case LSE_STYLE_UNIT_TURN:
      return value.value * k_turn_to_rad;
    default:
      return 0.f;
  }
}

static float compute_translate_axis(lse_style_value value, const lse_style_context* context, float box_bound) {
  switch (value.unit) {
    CASE_COMPUTE_VW_VH_VMIN_VMAX(&value, context);
    CASE_COMPUTE_REM(&value, context);
    CASE_COMPUTE_PX_PT(&value, context);
    case LSE_STYLE_UNIT_PERCENT:
      return (value.value / 100.f) * box_bound;
    default:
      return 0.f;
  }
}

static float compute_scale(lse_style_value value) {
  switch (value.unit) {
    // TODO: the parser assigns PX to raw numbers.. treat it as a scale value
    case LSE_STYLE_UNIT_PX:
      return value.value;
    case LSE_STYLE_UNIT_PERCENT:
      return value.value / 100.f;
    default:
      return 1.f;
  }
}

static float
compute_transform_origin(const lse_style_value* numeric, const lse_style_context* context, float box_bound) {
  switch (numeric ? numeric->unit : LSE_STYLE_UNIT_UNDEFINED) {
    CASE_COMPUTE_PX_PT(numeric, context);
    CASE_COMPUTE_REM(numeric, context);
    CASE_COMPUTE_VW_VH_VMIN_VMAX(numeric, context);
    case LSE_STYLE_UNIT_PERCENT:
      return (numeric->value / 100.f) * box_bound;
    case LSE_STYLE_UNIT_ANCHOR:
      switch ((int32_t)numeric->value) {
        case LSE_STYLE_ANCHOR_RIGHT:
        case LSE_STYLE_ANCHOR_BOTTOM:
          return box_bound;
        case LSE_STYLE_ANCHOR_CENTER:
          return box_bound * .5f;
        case LSE_STYLE_ANCHOR_LEFT:
        case LSE_STYLE_ANCHOR_TOP:
        default:
          return 0.f;
      }
    default:
      return box_bound * .5f;
  }
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_TYPE_INFO(lse_style_type, lse_style, lse_none_interface_type, NULL);
