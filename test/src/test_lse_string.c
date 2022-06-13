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

#include <lse_test.h>
#include <string.h>

struct lse_test_fixture {
  lse_string* string;
};

BEFORE_EACH(lse_string) {
}

AFTER_EACH(lse_string) {
  lse_unref(fixture->string);
}

TEST_CASE(lse_string_new_1, "should create new string") {
  const char* value = "test";
  fixture->string = lse_string_new(value);

  munit_assert_not_null(fixture->string);
  munit_assert_size(lse_string_size(fixture->string), ==, strlen(value));
  munit_assert_string_equal(lse_string_as_cstring(fixture->string), value);
}

TEST_CASE(lse_string_new_2, "should return null for empty string or NULL") {
  fixture->string = lse_string_new(NULL);
  munit_assert_null(fixture->string);
}

TEST_CASE(lse_string_new_3, "should return empty string for empty cstring") {
  fixture->string = lse_string_new("");

  munit_assert_not_null(fixture->string);
  munit_assert_size(lse_string_size(fixture->string), ==, 0);
  munit_assert_string_equal(lse_string_as_cstring(fixture->string), "");
}

TEST_CASE(lse_string_new_with_size_1, "should create new string filled with X") {
  const char* value = "XXXX";
  fixture->string = lse_string_new_with_size('X', 4);

  munit_assert_not_null(fixture->string);
  munit_assert_size(lse_string_size(fixture->string), ==, strlen(value));
  munit_assert_string_equal(lse_string_as_cstring(fixture->string), value);
}

TEST_CASE(lse_string_new_with_size_2, "should return empty string for size == 0") {
  fixture->string = lse_string_new_with_size('X', 0);

  munit_assert_not_null(fixture->string);
  munit_assert_string_equal(lse_string_as_cstring(fixture->string), "");
  munit_assert_size(lse_string_size(fixture->string), ==, 0);
}
