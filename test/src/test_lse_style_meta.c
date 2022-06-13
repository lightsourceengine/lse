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

#include <lse_style_meta.h>

#include <lse_test.h>

TEST_CASE(lse_style_meta_set_enum_1, "should set all alignItems values") {
  uint64_t flags;

  for (size_t i = 0; i < 8; i++) {
    munit_assert_true(lse_style_meta_is_enum_value(LSE_SP_ALIGN_ITEMS, i));
    flags = lse_style_meta_set_enum(LSE_SP_ALIGN_ITEMS, (int32_t)i, flags);
    munit_assert_int32(lse_style_meta_get_enum(LSE_SP_ALIGN_ITEMS, flags), ==, i);
  }
}

TEST_CASE(lse_style_meta_set_enum_2, "should not change flags for non-enum property") {
  munit_assert_uint64(lse_style_meta_set_enum(LSE_SP_BACKGROUND_HEIGHT, 0, 0xFFFF), ==, 0xFFFF);
}

TEST_CASE(lse_style_meta_set_enum_3, "should not change flags for out of range enum") {
  munit_assert_uint64(lse_style_meta_set_enum(LSE_SP_TEXT_ALIGN, -1, 0xFFFF), ==, 0xFFFF);
}

TEST_CASE(lse_style_meta_enum_to_string_1, "should get string value") {
  munit_assert_string_equal(lse_style_meta_enum_to_string(LSE_SP_ALIGN_ITEMS, LSE_STYLE_ALIGN_BASELINE), "baseline");
}

TEST_CASE(lse_style_meta_enum_to_string_2, "should return NULL for non-enum property") {
  munit_assert_null(lse_style_meta_enum_to_string(LSE_SP_HEIGHT, 0));
}

TEST_CASE(lse_style_meta_enum_to_string_3, "should return NULL for invalid enum") {
  munit_assert_null(lse_style_meta_enum_to_string(LSE_SP_ALIGN_ITEMS, -1));
}

TEST_CASE(lse_style_meta_from_string_1, "should return enum value for string") {
  munit_assert_int32(lse_style_meta_enum_from_string(LSE_SP_ALIGN_ITEMS, "baseline"), ==, LSE_STYLE_ALIGN_BASELINE);
}

TEST_CASE(lse_style_meta_from_string_2, "should return -1 for invalid enum string") {
  munit_assert_int32(lse_style_meta_enum_from_string(LSE_SP_ALIGN_ITEMS, "unknown"), ==, -1);
}

TEST_CASE(lse_style_meta_from_string_3, "should return -1 for non-enum property") {
  munit_assert_int32(lse_style_meta_enum_from_string(LSE_SP_HEIGHT, "center"), ==, -1);
}
