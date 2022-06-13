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

struct lse_test_fixture {
  lse_array* array;
};

typedef struct {
  int32_t value;
} array_item;

BEFORE_EACH(lse_array) {
}

AFTER_EACH(lse_array) {
  lse_unref(fixture->array);
}

TEST_CASE(lse_array_new_1, "should create new array") {
  size_t array_length = 10;
  size_t i;

  fixture->array = lse_array_new(array_length, sizeof(array_item), NULL);

  munit_assert_not_null(fixture->array);
  munit_assert_size(lse_array_get_length(fixture->array), ==, array_length);

  for (i = 0; i < array_length; i++) {
    munit_assert_int32(((array_item*)lse_array_at(fixture->array, i))->value, ==, 0);
  }
}

TEST_CASE(lse_array_new_2, "should create empty array when length is 0") {
  fixture->array = lse_array_new(0, 0, NULL);

  munit_assert_not_null(fixture->array);
  munit_assert_size(lse_array_get_length(fixture->array), ==, 0);
}

TEST_CASE(lse_array_new_empty_1, "should create empty array") {
  fixture->array = lse_array_new_empty();

  munit_assert_not_null(fixture->array);
  munit_assert_size(lse_array_get_length(fixture->array), ==, 0);
}

TEST_CASE(lse_array_at_1, "should get item at index") {
  int32_t value = 100;
  array_item* item;

  fixture->array = lse_array_new(1, sizeof(array_item), NULL);
  lse_array_set(fixture->array, 0, &(array_item){ value });

  item = lse_array_at(fixture->array, 0);

  munit_assert_not_null(item);
  munit_assert_int32(item->value, ==, 100);
}

TEST_CASE(lse_array_get_1, "should get item at index") {
  int32_t value = 100;
  array_item* item;

  fixture->array = lse_array_new(1, sizeof(array_item), NULL);
  lse_array_set(fixture->array, 0, &(array_item){ value });

  item = lse_array_get(fixture->array, 0);

  munit_assert_not_null(item);
  munit_assert_int32(item->value, ==, 100);
}

TEST_CASE(lse_array_get_2, "should return NULL for index out of range") {
  int32_t value = 100;
  array_item* item;

  fixture->array = lse_array_new(1, sizeof(array_item), NULL);
  lse_array_set(fixture->array, 0, &(array_item){ value });

  item = lse_array_get(fixture->array, 100);

  munit_assert_null(item);
}
