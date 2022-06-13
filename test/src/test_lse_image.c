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

#include <lse_image.h>

#include <lse_object.h>
#include <lse_test.h>

//
// types
//

struct lse_test_fixture {
  lse_image* image;
};

//
// constants
//

const char* TEST_URI = "assets/640x480.png";
const int32_t TEST_IMAGE_WIDTH = 100;
const int32_t TEST_IMAGE_HEIGHT = 200;
const lse_color_format TEST_IMAGE_FORMAT = LSE_COLOR_FORMAT_BGRA;

//
// static data
//

static bool s_callback_called = false;
static void* s_callback_observer = NULL;
static lse_image* s_callback_image = NULL;
static lse_resource_state s_callback_state = LSE_RESOURCE_STATE_INIT;
static void* s_mock_observer = (void*)(intptr_t)1;

//
// private functions
//

static void image_event_callback(const lse_image_event* e, void* observer);

BEFORE_EACH(lse_image) {
}

AFTER_EACH(lse_image) {
  s_callback_called = false;
  s_callback_observer = NULL;
  s_callback_image = NULL;
  s_callback_state = LSE_RESOURCE_STATE_INIT;

  lse_unref(fixture->image);
}

TEST_CASE(lse_image_constructor_1, "should create new image in the INIT state") {
  fixture->image = lse_new(lse_image, lse_string_new(TEST_URI));

  munit_assert_not_null(fixture->image);
  munit_assert_string_equal(lse_string_as_cstring(lse_image_get_uri(fixture->image)), TEST_URI);
  munit_assert_int32(lse_image_get_state(fixture->image), ==, LSE_RESOURCE_STATE_INIT);
  munit_assert_int32(lse_image_get_width(fixture->image), ==, 0);
  munit_assert_int32(lse_image_get_height(fixture->image), ==, 0);
  munit_assert_int32(lse_image_get_format(fixture->image), ==, LSE_COLOR_FORMAT_UNKNOWN);
}

TEST_CASE(lse_image_set_loading_1, "should move image to LOADING state") {
  fixture->image = lse_new(lse_image, lse_string_new(TEST_URI));

  lse_image_add_observer(fixture->image, s_mock_observer, &image_event_callback);
  lse_image_set_loading(fixture->image);
  lse_image_remove_observer(fixture->image, s_mock_observer);

  munit_assert_int32(lse_image_get_state(fixture->image), ==, LSE_RESOURCE_STATE_LOADING);
  munit_assert_false(s_callback_called);
}

TEST_CASE(lse_image_set_ready_1, "should move image to READY state") {
  fixture->image = lse_new(lse_image, lse_string_new(TEST_URI));

  lse_image_add_observer(fixture->image, s_mock_observer, &image_event_callback);
  // TODO: send pixels?
  lse_image_set_ready(fixture->image, NULL, NULL, TEST_IMAGE_WIDTH, TEST_IMAGE_HEIGHT, TEST_IMAGE_FORMAT);
  lse_image_remove_observer(fixture->image, s_mock_observer);

  munit_assert_int32(lse_image_get_state(fixture->image), ==, LSE_RESOURCE_STATE_READY);
  munit_assert_int32(lse_image_get_width(fixture->image), ==, TEST_IMAGE_WIDTH);
  munit_assert_int32(lse_image_get_height(fixture->image), ==, TEST_IMAGE_HEIGHT);
  munit_assert_int32(lse_image_get_format(fixture->image), ==, TEST_IMAGE_FORMAT);

  munit_assert_true(s_callback_called);
  munit_assert_ptr_equal(s_callback_observer, s_mock_observer);
  munit_assert_ptr_equal(s_callback_image, fixture->image);
  munit_assert_int32(s_callback_state, ==, LSE_RESOURCE_STATE_READY);
}

TEST_CASE(lse_image_set_error_1, "should move image to READY state") {
  fixture->image = lse_new(lse_image, lse_string_new(TEST_URI));

  lse_image_add_observer(fixture->image, s_mock_observer, &image_event_callback);
  lse_image_set_error(fixture->image);
  lse_image_remove_observer(fixture->image, s_mock_observer);

  munit_assert_int32(lse_image_get_state(fixture->image), ==, LSE_RESOURCE_STATE_ERROR);
  munit_assert_true(s_callback_called);
  munit_assert_ptr_equal(s_callback_observer, s_mock_observer);
  munit_assert_ptr_equal(s_callback_image, fixture->image);
  munit_assert_int32(s_callback_state, ==, LSE_RESOURCE_STATE_ERROR);
}

// @private
static void image_event_callback(const lse_image_event* e, void* observer) {
  s_callback_called = true;
  s_callback_observer = observer;
  s_callback_image = e->image;
  s_callback_state = e->state;
}
