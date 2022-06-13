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

#include <lse_font.h>

#include <lse_object.h>
#include <lse_test.h>

//
// types
//

struct lse_test_fixture {
  lse_font* font;
};

//
// constants
//

const char* TEST_FAMILY = "test";
const int32_t TEST_INDEX = 0;
const char* TEST_FONT_URI = "assets/roboto.ttf";

//
// static data
//

static bool s_callback_called = false;
static void* s_callback_observer = NULL;
static lse_font* s_callback_font = NULL;
static lse_resource_state s_callback_state = LSE_RESOURCE_STATE_INIT;
static void* s_mock_observer = (void*)(intptr_t)1;

//
// private functions
//

static lse_font_info font_info_init();
static void font_event_callback(const lse_font_event* e, void* observer);

BEFORE_EACH(lse_font) {
}

AFTER_EACH(lse_font) {
  s_callback_called = false;
  s_callback_observer = NULL;
  s_callback_font = NULL;
  s_callback_state = LSE_RESOURCE_STATE_INIT;

  lse_unref(fixture->font);
}

TEST_CASE(lse_font_constructor_1, "should create new font in the INIT state") {
  lse_font_info info = font_info_init();

  fixture->font = lse_new(lse_font, &info);

  munit_assert_not_null(fixture->font);
  munit_assert_string_equal(lse_string_as_cstring(lse_font_get_uri(fixture->font)), TEST_FONT_URI);
  munit_assert_int32(lse_font_get_index(fixture->font), ==, TEST_INDEX);
  munit_assert_string_equal(lse_string_as_cstring(lse_font_get_family(fixture->font)), TEST_FAMILY);
  munit_assert_int32(lse_font_get_style(fixture->font), ==, LSE_STYLE_FONT_STYLE_NORMAL);
  munit_assert_int32(lse_font_get_weight(fixture->font), ==, LSE_STYLE_FONT_WEIGHT_NORMAL);
  munit_assert_int32(lse_font_get_state(fixture->font), ==, LSE_RESOURCE_STATE_INIT);
}

TEST_CASE(lse_font_set_ready_1, "should move font to the READY state") {
  lse_font_info info = font_info_init();

  fixture->font = lse_new(lse_font, &info);

  lse_font_add_observer(fixture->font, s_mock_observer, &font_event_callback);
  lse_font_set_ready(fixture->font, NULL, NULL, 0);
  lse_font_remove_observer(fixture->font, s_mock_observer);

  munit_assert_int32(lse_font_get_state(fixture->font), ==, LSE_RESOURCE_STATE_READY);
  munit_assert_true(s_callback_called);
  munit_assert_ptr_equal(s_callback_observer, s_mock_observer);
  munit_assert_ptr_equal(s_callback_font, fixture->font);
  munit_assert_int32(s_callback_state, ==, LSE_RESOURCE_STATE_READY);
}

TEST_CASE(lse_font_set_error_1, "should move font to the ERROR state") {
  lse_font_info info = font_info_init();

  fixture->font = lse_new(lse_font, &info);

  lse_font_add_observer(fixture->font, s_mock_observer, &font_event_callback);
  lse_font_set_error(fixture->font);
  lse_font_remove_observer(fixture->font, s_mock_observer);

  munit_assert_int32(lse_font_get_state(fixture->font), ==, LSE_RESOURCE_STATE_ERROR);
  munit_assert_true(s_callback_called);
  munit_assert_ptr_equal(s_callback_observer, s_mock_observer);
  munit_assert_ptr_equal(s_callback_font, fixture->font);
  munit_assert_int32(s_callback_state, ==, LSE_RESOURCE_STATE_ERROR);
}

TEST_CASE(lse_font_set_loading_1, "should move font to the ERROR state") {
  lse_font_info info = font_info_init();

  fixture->font = lse_new(lse_font, &info);

  lse_font_add_observer(fixture->font, s_mock_observer, &font_event_callback);
  lse_font_set_loading(fixture->font);
  lse_font_remove_observer(fixture->font, s_mock_observer);

  munit_assert_int32(lse_font_get_state(fixture->font), ==, LSE_RESOURCE_STATE_LOADING);
  munit_assert_false(s_callback_called);
}

// @private
static lse_font_info font_info_init() {
  return (lse_font_info){
    .family = lse_string_new(TEST_FAMILY),
    .style = LSE_STYLE_FONT_STYLE_NORMAL,
    .weight = LSE_STYLE_FONT_WEIGHT_NORMAL,
    .uri = lse_string_new(TEST_FONT_URI),
    .index = TEST_INDEX,
  };
}

// @private
static void font_event_callback(const lse_font_event* e, void* observer) {
  s_callback_called = true;
  s_callback_observer = observer;
  s_callback_font = e->font;
  s_callback_state = e->state;
}
