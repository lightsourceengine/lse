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

#include <lse.h>
#include <lse_color.h>
#include <lse_test.h>
#include <lse_util.h>
#include <stc/ccommon.h>

struct lse_test_fixture {
  lse_style* style;
};

typedef struct {
  const char* cstring_value;
  lse_style_unit expected_unit;
  float expected_value;
} from_string_numeric_input;

typedef struct {
  const char* cstring_value;
  uint32_t expected_value;
} from_string_color_input;

typedef struct {
  lse_style_property property;
  const char* value;
  int32_t expected_value;
} from_string_enum_input;

typedef struct {
  lse_style_property property;
  const char* value;
} from_string_invalid_input;

static lse_object* create_transform();
static lse_object* create_filter();

BEFORE_EACH(lse_style) {
  fixture->style = lse_style_new();
}

AFTER_EACH(lse_style) {
  munit_assert_uint32(lse_get_ref_count(fixture->style), ==, 1);
  lse_unref(fixture->style);
}

TEST_CASE(lse_style_new_1, "should create new style") {
  munit_assert_not_null(fixture->style);
  munit_assert_uint32(lse_get_ref_count(fixture->style), ==, 1);
}

TEST_CASE(lse_style_from_string_1, "should parse numeric values for width property") {
  const lse_style_value* numeric;
  bool result;
  from_string_numeric_input input[] = {
    { "100", LSE_STYLE_UNIT_PX, 100.f },
    { "100px", LSE_STYLE_UNIT_PX, 100.f },
    { "-100", LSE_STYLE_UNIT_PX, -100.f },
    { "-100px", LSE_STYLE_UNIT_PX, -100.f },
    { "100%", LSE_STYLE_UNIT_PERCENT, 100.f },
    { "-100%", LSE_STYLE_UNIT_PERCENT, -100.f },
    { "auto", LSE_STYLE_UNIT_AUTO, LSE_UNDEFINED },
    { "100vw", LSE_STYLE_UNIT_VW, 100.f },
    { "100vh", LSE_STYLE_UNIT_VH, 100.f },
    { "100vmin", LSE_STYLE_UNIT_VMIN, 100.f },
    { "100vmax", LSE_STYLE_UNIT_VMAX, 100.f },
    { "100rem", LSE_STYLE_UNIT_REM, 100.f },
  };

  for (size_t i = 0; i < c_arraylen(input); i++) {
    result = lse_style_from_string(fixture->style, LSE_SP_WIDTH, lse_string_new(input[i].cstring_value));

    munit_assert_true(result);

    numeric = lse_style_get_numeric(fixture->style, LSE_SP_WIDTH);

    munit_assert_not_null(numeric);

    if (lse_is_undefined(input[i].expected_value)) {
      munit_assert_true(lse_is_undefined(numeric->value));
    } else {
      munit_assert_float(numeric->value, ==, input[i].expected_value);
    }

    munit_assert_int32(numeric->unit, ==, input[i].expected_unit);
  }
}

TEST_CASE(lse_style_from_string_2, "should parse color values for backgroundColor property") {
  bool result;
  // clang-format off
  from_string_color_input input[] = {
    { "#F00", LSE_COLOR_MAKE(255, 0, 0, 255).value },
    { "#FF0000", LSE_COLOR_MAKE(255, 0, 0, 255).value },
    { "#F00F", LSE_COLOR_MAKE(255, 0, 0, 255).value },
    { "#FF0000FF", LSE_COLOR_MAKE(255, 0, 0, 255).value },
    { "red", LSE_COLOR_MAKE(255, 0, 0, 255).value },
  };
  // clang-format on

  for (size_t i = 0; i < c_arraylen(input); i++) {
    result = lse_style_from_string(fixture->style, LSE_SP_BACKGROUND_COLOR, lse_string_new(input[i].cstring_value));

    munit_assert_true(result);
    munit_assert_uint32(lse_style_get_color(fixture->style, LSE_SP_BACKGROUND_COLOR), ==, input[i].expected_value);
  }
}

TEST_CASE(lse_style_from_string_3, "should parse enum values") {
  bool result;
  from_string_enum_input input[] = {
    { LSE_SP_ALIGN_ITEMS, "center", LSE_STYLE_ALIGN_CENTER },
    { LSE_SP_ALIGN_ITEMS, "CENTER", LSE_STYLE_ALIGN_CENTER },
  };

  for (size_t i = 0; i < c_arraylen(input); i++) {
    result = lse_style_from_string(fixture->style, input[i].property, lse_string_new(input[i].value));

    munit_assert_true(result);
    munit_assert_uint32(lse_style_get_enum(fixture->style, input[i].property), ==, input[i].expected_value);
  }
}

TEST_CASE(lse_style_from_string_4, "should set fontFamily string") {
  const char* family = "Comic Sans";
  const char* property_value;
  bool result = lse_style_from_string(fixture->style, LSE_SP_FONT_FAMILY, lse_string_new(family));

  munit_assert_true(result);
  munit_assert_true(lse_style_has_property(fixture->style, LSE_SP_FONT_FAMILY));

  property_value = lse_string_as_cstring((lse_string*)lse_style_get_object(fixture->style, LSE_SP_FONT_FAMILY));
  munit_assert_string_equal(property_value, family);
}

TEST_CASE(lse_style_from_string_5, "should return false for invalid values") {
  from_string_invalid_input input[] = {
    { LSE_SP_ALIGN_ITEMS, "" },   { LSE_SP_ALIGN_ITEMS, "xxx" },       { LSE_SP_COLOR, "#" },
    { LSE_SP_COLOR, "#QIPOQII" }, { LSE_SP_COLOR, "not a css color" }, { LSE_SP_WIDTH, "100uy" },
    { LSE_SP_WIDTH, "1dasd0" },
  };

  for (size_t i = 0; i < c_arraylen(input); i++) {
    munit_assert_false(lse_style_from_string(fixture->style, input[i].property, lse_string_new(input[i].value)));
  }
}

TEST_CASE(lse_style_set_object_1, "should return true when setting transform") {
  bool result;

  result = lse_style_set_object(fixture->style, LSE_SP_TRANSFORM, create_transform());

  munit_assert_true(result);
}

SKIP_TEST_CASE(lse_style_set_object_2, "should return true when setting filter") {
  bool result;

  result = lse_style_set_object(fixture->style, LSE_SP_FILTER, create_filter());

  munit_assert_true(result);
}

TEST_CASE(lse_style_transform_new_1, "should create a new transform array from list") {
  lse_style_transform list[3];

  lse_style_transform_translate(
      (lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX },
      (lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX },
      &list[0]);
  lse_style_transform_scale(
      (lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX },
      (lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX },
      &list[1]);
  lse_style_transform_rotate((lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX }, &list[2]);

  lse_object* t = lse_style_transform_new(list, c_arraylen(list));

  munit_assert_uint8(lse_object_get_type(t), ==, lse_array_type);
  munit_assert_size(lse_array_get_length((lse_array*)t), ==, 3);
  munit_assert_int32(((lse_style_transform*)lse_array_at((lse_array*)t, 0))->op, ==, LSE_STYLE_TRANSFORM_TRANSLATE);
  munit_assert_int32(((lse_style_transform*)lse_array_at((lse_array*)t, 1))->op, ==, LSE_STYLE_TRANSFORM_SCALE);
  munit_assert_int32(((lse_style_transform*)lse_array_at((lse_array*)t, 2))->op, ==, LSE_STYLE_TRANSFORM_ROTATE);
}

TEST_CASE(lse_style_transform_new_2, "should create an empty array for length 0") {
  lse_object* t = lse_style_transform_new(NULL, 0);

  munit_assert_uint8(lse_object_get_type(t), ==, lse_array_type);
  munit_assert_size(lse_array_get_length((lse_array*)t), ==, 0);
}

SKIP_TEST_CASE(lse_style_filter_new_1, "should create a new filter array from list") {
  lse_style_filter list[3];

  lse_style_filter_flip_h(&list[0]);
  lse_style_filter_flip_v(&list[1]);
  lse_style_filter_tint(0xFFFFFFFF, &list[2]);

  lse_object* t = lse_style_filter_new(list, c_arraylen(list));

  munit_assert_uint8(lse_object_get_type(t), ==, lse_array_type);
  munit_assert_size(lse_array_get_length((lse_array*)t), ==, 3);
  munit_assert_int32(((lse_style_filter*)lse_array_at((lse_array*)t, 0))->func, ==, LSE_STYLE_FILTER_FLIP_H);
  munit_assert_int32(((lse_style_filter*)lse_array_at((lse_array*)t, 1))->func, ==, LSE_STYLE_FILTER_FLIP_V);
  munit_assert_int32(((lse_style_filter*)lse_array_at((lse_array*)t, 2))->func, ==, LSE_STYLE_FILTER_TINT);
}

SKIP_TEST_CASE(lse_style_filter_new_2, "should create an empty array for length 0") {
  lse_object* t = lse_style_filter_new(NULL, 0);

  munit_assert_uint8(lse_object_get_type(t), ==, lse_array_type);
  munit_assert_size(lse_array_get_length((lse_array*)t), ==, 0);
}

static lse_object* create_transform() {
  lse_style_transform list[3];

  lse_style_transform_translate(
      (lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX },
      (lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX },
      &list[0]);
  lse_style_transform_scale(
      (lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX },
      (lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX },
      &list[1]);
  lse_style_transform_rotate((lse_style_value){ .value = 1, .unit = LSE_STYLE_UNIT_PX }, &list[2]);

  return lse_style_transform_new(list, c_arraylen(list));
}

static lse_object* create_filter() {
  lse_style_filter list[3];

  lse_style_filter_flip_h(&list[0]);
  lse_style_filter_flip_v(&list[1]);
  lse_style_filter_tint(0xFFFFFFFF, &list[2]);

  return lse_style_filter_new(list, c_arraylen(list));
}
