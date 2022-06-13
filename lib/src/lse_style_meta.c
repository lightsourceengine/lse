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

// ////////////////////////////////////////////////////////////////////////////
// AUTO-GENERATED - DO NOT EDIT
// clang-format off
// ////////////////////////////////////////////////////////////////////////////

#include "lse_style_meta.h"

#include <stdbool.h>
#include "stc/ccommon.h"

#include "lse_string.h"

static uint64_t mask(uint64_t bitWidth, uint64_t offset);
#define is_any(IGNORE) true
#define is_gte0(NUMERIC) ((NUMERIC)->value >= 0)
static bool is_xanchor(const lse_style_value* numeric);
static bool is_yanchor(const lse_style_value* numeric);
static bool is_opacity(const lse_style_value* numeric);
#define has_unit(UNIT, FLAGS) (((UNIT) & (FLAGS)) != 0)

#define F(UNIT_VALUE) (1 << LSE_STYLE_UNIT_ ## UNIT_VALUE)
#define UNIT_MASK_ANCHOR F(ANCHOR)
#define UNIT_MASK_AUTO F(AUTO)
#define UNIT_MASK_PX F(PX)
#define UNIT_MASK_PT F(PT)
#define UNIT_MASK_PERCENT F(PERCENT)
#define UNIT_MASK_PIXELISH (F(PX) | F(PT) | F(VMIN) | F(VMAX) | F(VW) | F(VH) | F(REM))

typedef struct {
  bool layout;
  lse_style_property_type type;
  const char** enum_values;
  int32_t enum_values_size;
  uint64_t enum_offset;
  uint64_t enum_bit_width;
} property_info;

typedef struct {
  const char* postfix;
  lse_style_unit unit;
} unit_postfix_entry;

static const char* k_str_align[] = {
  "auto",
  "flex-start",
  "center",
  "flex-end",
  "stretch",
  "baseline",
  "space-between",
  "space-around",
};


static const char* k_str_display[] = {
  "flex",
  "none",
};


static const char* k_str_flex_direction[] = {
  "column",
  "column-reverse",
  "row",
  "row-reverse",
};


static const char* k_str_overflow[] = {
  "visible",
  "hidden",
  "scroll",
};


static const char* k_str_overflow_wrap[] = {
  "normal",
  "break-word",
};


static const char* k_str_position[] = {
  "static",
  "relative",
  "absolute",
};


static const char* k_str_wrap[] = {
  "no-wrap",
  "wrap",
  "wrap-reverse",
};


static const char* k_str_justify[] = {
  "flex-start",
  "center",
  "flex-end",
  "space-between",
  "space-around",
  "space-evenly",
};


static const char* k_str_font_style[] = {
  "normal",
  "italic",
  "oblique",
};


static const char* k_str_font_weight[] = {
  "normal",
  "bold",
};


static const char* k_str_text_overflow[] = {
  "none",
  "clip",
  "ellipsis",
};


static const char* k_str_text_align[] = {
  "left",
  "center",
  "right",
};


static const char* k_str_object_fit[] = {
  "fill",
  "contain",
  "cover",
  "none",
  "scale-down",
};


static const char* k_str_background_clip[] = {
  "border-box",
  "padding-box",
  "content-box",
};


static const char* k_str_background_size[] = {
  "none",
  "cover",
  "contain",
};


static const char* k_str_background_repeat[] = {
  "off",
  "xy",
  "x",
  "y",
};


static const char* k_str_text_transform[] = {
  "none",
  "uppercase",
  "lowercase",
};


static const char* k_str_white_space[] = {
  "normal",
  "pre",
  "no-wrap",
};


static const char* k_str_font_kerning[] = {
  "normal",
  "none",
};


static const char* k_str_anchor[] = {
  "top",
  "right",
  "bottom",
  "left",
  "center",
};


static const char* k_str_unit[] = {
  "undefined",
  "px",
  "pt",
  "percent",
  "auto",
  "anchor",
  "rem",
  "vmin",
  "vmax",
  "vw",
  "vh",
  "deg",
  "rad",
  "grad",
  "turn",
};

static unit_postfix_entry k_post_fix_map [] = {
  { .postfix = "px", .unit = 1 },
  { .postfix = "pt", .unit = 2 },
  { .postfix = "%", .unit = 3 },
  { .postfix = "rem", .unit = 6 },
  { .postfix = "vmin", .unit = 7 },
  { .postfix = "vmax", .unit = 8 },
  { .postfix = "vw", .unit = 9 },
  { .postfix = "vh", .unit = 10 },
  { .postfix = "deg", .unit = 11 },
  { .postfix = "rad", .unit = 12 },
  { .postfix = "grad", .unit = 13 },
  { .postfix = "turn", .unit = 14 },
};

static const property_info k_property_info[] = {
  /* alignItems */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_align, .enum_values_size = 8, .enum_bit_width = 3, .enum_offset = 0, },
  /* alignContent */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_align, .enum_values_size = 8, .enum_bit_width = 3, .enum_offset = 3, },
  /* alignSelf */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_align, .enum_values_size = 8, .enum_bit_width = 3, .enum_offset = 6, },
  /* border */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* borderBottom */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* borderLeft */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* borderRight */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* borderTop */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* bottom */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* display */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_display, .enum_values_size = 2, .enum_bit_width = 1, .enum_offset = 9, },
  /* flex */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* flexBasis */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* flexDirection */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_flex_direction, .enum_values_size = 4, .enum_bit_width = 2, .enum_offset = 10, },
  /* flexGrow */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* flexShrink */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* flexWrap */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_wrap, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 12, },
  /* height */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* justifyContent */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_justify, .enum_values_size = 6, .enum_bit_width = 3, .enum_offset = 14, },
  /* left */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* margin */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* marginBottom */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* marginLeft */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* marginRight */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* marginTop */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* maxHeight */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* maxWidth */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* minHeight */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* minWidth */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* overflow */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_overflow, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 17, },
  /* padding */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* paddingBottom */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* paddingLeft */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* paddingRight */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* paddingTop */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* position */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = true, .enum_values = k_str_position, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 19, },
  /* right */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* top */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* width */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = true, .enum_values_size = 0, },
  /* backgroundColor */
  { .type = LSE_STYLE_PROPERTY_TYPE_COLOR, .layout = false, .enum_values_size = 0, },
  /* backgroundHeight */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* backgroundImage */
  { .type = LSE_STYLE_PROPERTY_TYPE_STRING, .layout = false, .enum_values_size = 0, },
  /* backgroundPositionX */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* backgroundPositionY */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* backgroundSize */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_background_size, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 21, },
  /* backgroundWidth */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* borderColor */
  { .type = LSE_STYLE_PROPERTY_TYPE_COLOR, .layout = false, .enum_values_size = 0, },
  /* borderRadius */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* borderRadiusTopLeft */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* borderRadiusTopRight */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* borderRadiusBottomLeft */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* borderRadiusBottomRight */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* color */
  { .type = LSE_STYLE_PROPERTY_TYPE_COLOR, .layout = false, .enum_values_size = 0, },
  /* filter */
  { .type = LSE_STYLE_PROPERTY_TYPE_FILTER, .layout = false, .enum_values_size = 0, },
  /* fontFamily */
  { .type = LSE_STYLE_PROPERTY_TYPE_STRING, .layout = false, .enum_values_size = 0, },
  /* fontKerning */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_font_kerning, .enum_values_size = 2, .enum_bit_width = 1, .enum_offset = 23, },
  /* fontSize */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* fontStyle */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_font_style, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 24, },
  /* fontWeight */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_font_weight, .enum_values_size = 2, .enum_bit_width = 1, .enum_offset = 26, },
  /* maxLines */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* objectFit */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_object_fit, .enum_values_size = 5, .enum_bit_width = 3, .enum_offset = 27, },
  /* objectPositionX */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* objectPositionY */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* opacity */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* overflowWrap */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_overflow_wrap, .enum_values_size = 2, .enum_bit_width = 1, .enum_offset = 30, },
  /* textAlign */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_text_align, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 31, },
  /* textOverflow */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_text_overflow, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 33, },
  /* textTransform */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_text_transform, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 35, },
  /* transform */
  { .type = LSE_STYLE_PROPERTY_TYPE_TRANSFORM, .layout = false, .enum_values_size = 0, },
  /* transformOriginX */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* transformOriginY */
  { .type = LSE_STYLE_PROPERTY_TYPE_NUMBER, .layout = false, .enum_values_size = 0, },
  /* whiteSpace */
  { .type = LSE_STYLE_PROPERTY_TYPE_ENUM, .layout = false, .enum_values = k_str_white_space, .enum_values_size = 3, .enum_bit_width = 2, .enum_offset = 37, },
};

bool lse_style_meta_is_style_property(int32_t value) {
  return value >= 0 && value < 71;
}

lse_style_property_type lse_style_meta_get_property_type(lse_style_property prop) {
  return k_property_info[prop].type;
}

bool lse_style_meta_is_layout_property(lse_style_property prop) {
  return k_property_info[prop].layout;
}

bool lse_style_meta_is_compatible_unit(lse_style_property prop, lse_style_unit unit) {
  return false;
}

bool lse_style_meta_is_enum_value(lse_style_property prop, int32_t value) {
  return k_property_info[prop].type == LSE_STYLE_PROPERTY_TYPE_ENUM
    && value >= 0 && value < k_property_info[prop].enum_values_size;
}

bool lse_style_meta_is_valid_numeric_value(lse_style_property prop, const lse_style_value* numeric) {
  switch (prop) {
    case LSE_SP_BORDER:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BORDER_BOTTOM:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BORDER_LEFT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BORDER_RIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BORDER_TOP:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BOTTOM:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_FLEX:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_FLEX_BASIS:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_gte0(numeric);
    case LSE_SP_FLEX_GROW:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_FLEX_SHRINK:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_HEIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_gte0(numeric);
    case LSE_SP_LEFT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_MARGIN:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_any(numeric);
    case LSE_SP_MARGIN_BOTTOM:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_any(numeric);
    case LSE_SP_MARGIN_LEFT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_any(numeric);
    case LSE_SP_MARGIN_RIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_any(numeric);
    case LSE_SP_MARGIN_TOP:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_any(numeric);
    case LSE_SP_MAX_HEIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_gte0(numeric);
    case LSE_SP_MAX_WIDTH:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_gte0(numeric);
    case LSE_SP_MIN_HEIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_gte0(numeric);
    case LSE_SP_MIN_WIDTH:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_gte0(numeric);
    case LSE_SP_PADDING:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_PADDING_BOTTOM:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_PADDING_LEFT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_PADDING_RIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_PADDING_TOP:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_RIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_TOP:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT) && is_any(numeric);
    case LSE_SP_WIDTH:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_gte0(numeric);
    case LSE_SP_BACKGROUND_HEIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_gte0(numeric);
    case LSE_SP_BACKGROUND_POSITION_X:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO | UNIT_MASK_ANCHOR) && is_xanchor(numeric);
    case LSE_SP_BACKGROUND_POSITION_Y:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO | UNIT_MASK_ANCHOR) && is_yanchor(numeric);
    case LSE_SP_BACKGROUND_WIDTH:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO) && is_gte0(numeric);
    case LSE_SP_BORDER_RADIUS:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BORDER_RADIUS_TOP_LEFT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BORDER_RADIUS_TOP_RIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BORDER_RADIUS_BOTTOM_LEFT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_BORDER_RADIUS_BOTTOM_RIGHT:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_FONT_SIZE:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH) && is_gte0(numeric);
    case LSE_SP_MAX_LINES:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PX) && is_gte0(numeric);
    case LSE_SP_OBJECT_POSITION_X:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO | UNIT_MASK_ANCHOR) && is_xanchor(numeric);
    case LSE_SP_OBJECT_POSITION_Y:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO | UNIT_MASK_ANCHOR) && is_yanchor(numeric);
    case LSE_SP_OPACITY:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PX | UNIT_MASK_PERCENT) && is_opacity(numeric);
    case LSE_SP_TRANSFORM_ORIGIN_X:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO | UNIT_MASK_ANCHOR) && is_xanchor(numeric);
    case LSE_SP_TRANSFORM_ORIGIN_Y:
      return has_unit(numeric->unit, 0 | UNIT_MASK_PIXELISH | UNIT_MASK_PERCENT | UNIT_MASK_AUTO | UNIT_MASK_ANCHOR) && is_yanchor(numeric);
    default:
      return false;
  };
}

int32_t lse_style_meta_enum_from_string(lse_style_property prop, const char* string_value) {
  int32_t i;
  int32_t len = k_property_info[prop].enum_values_size;

  if (string_value) {
    for (i = 0; i < len; i++) {
      if (c_strncasecmp(k_property_info[prop].enum_values[i], string_value, SIZE_MAX) == 0) {
        return i;
      }
    }
  }

  return -1;
}

const char* lse_style_meta_enum_to_string(lse_style_property prop, int32_t enum_value) {
  if (lse_style_meta_is_enum_value(prop, enum_value)) {
    return k_property_info[prop].enum_values[enum_value];
  }

  return NULL;
}

int32_t lse_style_meta_get_enum(lse_style_property prop, uint64_t flags) {
  if (k_property_info[prop].type == LSE_STYLE_PROPERTY_TYPE_ENUM) {
    return (int32_t)((flags & mask(k_property_info[prop].enum_bit_width, k_property_info[prop].enum_offset)) >> k_property_info[prop].enum_offset);
  }

  return -1;
}

uint64_t lse_style_meta_set_enum(lse_style_property prop, int32_t enum_value, uint64_t flags) {
  if (k_property_info[prop].type == LSE_STYLE_PROPERTY_TYPE_ENUM && lse_style_meta_is_enum_value(prop, enum_value)) {
    uint32_t mask_value = mask(k_property_info[prop].enum_bit_width, k_property_info[prop].enum_offset);

    return ((flags & ~mask_value) | ((enum_value << k_property_info[prop].enum_offset) & (mask_value)));
  }

  return flags;
}

bool lse_style_meta_is_anchor(int32_t value) {
  return value >= 0 && value < (int32_t)c_arraylen(k_str_anchor);
}

bool lse_style_meta_is_unit(int32_t value) {
  return value >= 0 && value < (int32_t)c_arraylen(k_str_unit);
}

static uint64_t mask(uint64_t bitWidth, uint64_t offset) {
  return ((1 << bitWidth) - 1) << offset;
}

static bool is_xanchor(const lse_style_value* numeric) {
  if (numeric->unit == LSE_STYLE_UNIT_ANCHOR) {
    switch ((int32_t)numeric->value) {
      case LSE_STYLE_ANCHOR_LEFT:
      case LSE_STYLE_ANCHOR_RIGHT:
      case LSE_STYLE_ANCHOR_CENTER:
        return true;
      default:
        return false;
    }
  }

  return false;
}

static bool is_yanchor(const lse_style_value* numeric) {
  if (numeric->unit == LSE_STYLE_UNIT_ANCHOR) {
    switch ((int32_t)numeric->value) {
      case LSE_STYLE_ANCHOR_TOP:
      case LSE_STYLE_ANCHOR_BOTTOM:
      case LSE_STYLE_ANCHOR_CENTER:
        return true;
      default:
        return false;
    }
  }

  return false;
}

static bool is_opacity(const lse_style_value* numeric) {
  switch (numeric->unit) {
    case LSE_STYLE_UNIT_PX:
      return numeric->value >= 0 && numeric->value <= 1;
    case LSE_STYLE_UNIT_PERCENT:
      return numeric->value >= 0 && numeric->value <= 100;
    default:
      return false;
  }
}

int32_t lse_style_meta_anchor_from_string(const char* string_value) {
  int32_t i;
  int32_t len = c_arraylen(k_str_anchor);

  if (string_value) {
    for (i = 0; i < len; i++) {
      if (c_strncasecmp(k_str_anchor[i], string_value, SIZE_MAX) == 0) {
        return i;
      }
    }
  }

  return -1;
}

int32_t lse_style_meta_unit_from_postfix_string(const char* string_value) {
  int32_t i;
  int32_t len = c_arraylen(k_post_fix_map);

  if (string_value) {
    for (i = 0; i < len; i++) {
      if (c_strncasecmp(k_post_fix_map[i].postfix, string_value, SIZE_MAX) == 0) {
        return k_post_fix_map[i].unit;
      }
    }
  }

  return -1;
}
