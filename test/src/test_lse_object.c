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

#include <lse_object.h>

#include <lse_test.h>

struct lse_test_fixture {
  lse_object* object;
};

BEFORE_EACH(lse_object) {
}

AFTER_EACH(lse_object) {
  lse_unref(fixture->object);
}

TEST_CASE(lse_object_new_1, "should succeed") {
  fixture->object = lse_object_new(lse_style_type, NULL);

  munit_assert_not_null(fixture->object);
  munit_assert_int(lse_object_get_ref_count(fixture->object), ==, 1);
  munit_assert_int(lse_object_get_type(fixture->object), ==, lse_style_type);
  munit_assert_string_equal(lse_object_get_type_name(fixture->object), "lse_style");
}

TEST_CASE(lse_object_new_2, "should return null for invalid type") {
  fixture->object = lse_object_new(0, NULL);

  munit_assert_null(fixture->object);
}

TEST_CASE(lse_object_ref_1, "should increment count") {
  fixture->object = lse_object_new(lse_style_type, NULL);
  uint32_t result = lse_object_ref(fixture->object);

  munit_assert_uint32(result, ==, 2);
  munit_assert_uint32(lse_object_get_ref_count(fixture->object), ==, 2);
}
