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

#include <lse_image_store.h>

#include <lse_object.h>
#include <lse_test.h>

//
// types
//

struct lse_test_fixture {
  lse_env* env;
  lse_image_store* store;
  lse_image* image_1;
  lse_image* image_2;
};

//
// constants
//

#define IMAGE_FILE_PNG "assets/640x480.png"
#define IMAGE_FILE_PNG_WIDTH 640
#define IMAGE_FILE_PNG_HEIGHT 480
#define IMAGE_FILE_SVG "assets/300x300.svg"
#define IMAGE_FILE_SVG_WIDTH 300
#define IMAGE_FILE_SVG_HEIGHT 300
#define IMAGE_FILE_BAD_FORMAT "assets/not_an_image.txt"
#define IMAGE_FILE_DOES_NOT_EXIST "does_not_exist"

BEFORE_EACH(lse_image_store) {
  fixture->env = lse_env_new();
  fixture->store = (lse_image_store*)lse_object_new(lse_image_store_type, fixture->env);
  fixture->env = lse_test_env_new();
}

AFTER_EACH(lse_image_store) {
  lse_image_store_release_image(fixture->store, fixture->image_1);
  lse_image_store_release_image(fixture->store, fixture->image_2);

  lse_image_store_destroy(fixture->store);
  lse_unref(fixture->store);

  lse_test_env_drop(fixture->env);
}

TEST_CASE(lse_image_store_acquire_1, "should return new image from png file") {
  fixture->image_1 =
      lse_image_store_acquire_image(fixture->store, lse_string_new(IMAGE_FILE_PNG), LSE_IMAGE_STORE_SYNC);

  munit_assert_not_null(fixture->image_1);
  munit_assert_int32(lse_image_get_state(fixture->image_1), ==, LSE_RESOURCE_STATE_READY);
  munit_assert_int32(lse_image_get_width(fixture->image_1), ==, IMAGE_FILE_PNG_WIDTH);
  munit_assert_int32(lse_image_get_height(fixture->image_1), ==, IMAGE_FILE_PNG_HEIGHT);
}

TEST_CASE(lse_image_store_acquire_2, "should return new image from svg file") {
  fixture->image_1 =
      lse_image_store_acquire_image(fixture->store, lse_string_new(IMAGE_FILE_SVG), LSE_IMAGE_STORE_SYNC);

  munit_assert_not_null(fixture->image_1);
  munit_assert_int32(lse_image_get_state(fixture->image_1), ==, LSE_RESOURCE_STATE_READY);
  munit_assert_int32(lse_image_get_width(fixture->image_1), ==, IMAGE_FILE_SVG_WIDTH);
  munit_assert_int32(lse_image_get_height(fixture->image_1), ==, IMAGE_FILE_SVG_HEIGHT);
}

TEST_CASE(lse_image_store_acquire_3, "should return image with error for invalid image file") {
  fixture->image_1 =
      lse_image_store_acquire_image(fixture->store, lse_string_new(IMAGE_FILE_BAD_FORMAT), LSE_IMAGE_STORE_SYNC);

  munit_assert_int32(lse_image_get_state(fixture->image_1), ==, LSE_RESOURCE_STATE_ERROR);
}

TEST_CASE(lse_image_store_acquire_4, "should return image with error for file not found") {
  fixture->image_1 =
      lse_image_store_acquire_image(fixture->store, lse_string_new(IMAGE_FILE_DOES_NOT_EXIST), LSE_IMAGE_STORE_SYNC);

  munit_assert_int32(lse_image_get_state(fixture->image_1), ==, LSE_RESOURCE_STATE_ERROR);
}

TEST_CASE(lse_image_store_acquire_5, "should return stored image if uri has already been loaded") {
  fixture->image_1 =
      lse_image_store_acquire_image(fixture->store, lse_string_new(IMAGE_FILE_PNG), LSE_IMAGE_STORE_SYNC);
  fixture->image_2 =
      lse_image_store_acquire_image(fixture->store, lse_string_new(IMAGE_FILE_PNG), LSE_IMAGE_STORE_SYNC);

  munit_assert_not_null(fixture->image_1);
  munit_assert_not_null(fixture->image_2);
  munit_assert_ptr_equal(fixture->image_1, fixture->image_2);
}

TEST_CASE(lse_image_store_release_1, "should remove image from store") {
  fixture->image_1 =
      lse_image_store_acquire_image(fixture->store, lse_string_new(IMAGE_FILE_PNG), LSE_IMAGE_STORE_SYNC);

  munit_assert_not_null(fixture->image_1);

  fixture->image_1 = lse_image_store_release_image(fixture->store, fixture->image_1);

  munit_assert_null(fixture->image_1);
  munit_assert_null(lse_image_store_get_image(fixture->store, IMAGE_FILE_PNG));
}

TEST_CASE(lse_image_store_release_2, "should be no-op for null image") {
  munit_assert_null(lse_image_store_release_image(fixture->store, NULL));
}
