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

#include <lse_font_store.h>

#include <lse_object.h>
#include <lse_test.h>
#include <stc/ccommon.h>

//
// types
//

struct lse_test_fixture {
  lse_env* env;
  lse_font_store* store;
  lse_font* font;
};

//
// constants
//

static const char* TEST_URI_TTF = "assets/roboto.ttf";
static const char* TEST_FAMILY = "Roboto";
static const int32_t TEST_INDEX = 0;
static const lse_style_font_style TEST_STYLE = LSE_STYLE_FONT_STYLE_NORMAL;
static const lse_style_font_weight TEST_WEIGHT = LSE_STYLE_FONT_WEIGHT_NORMAL;

//
// private functions
//

static void test_add_font(lse_test_fixture* fixture, const char* uri, lse_resource_state expected_state);

BEFORE_EACH(lse_font_store) {
  fixture->env = lse_test_env_new();
  fixture->store = (lse_font_store*)lse_object_new(lse_font_store_type, fixture->env);
}

AFTER_EACH(lse_font_store) {
  lse_font_store_release_font(fixture->store, fixture->font);

  lse_font_store_destroy(fixture->store);
  lse_unref(fixture->store);

  lse_test_env_drop(fixture->env);
}

TEST_CASE(lse_font_store_acquire_font_1, "should acquire a font by family/style/weight key") {
  lse_status status = lse_font_store_add_font(
      fixture->store,
      lse_string_new(TEST_URI_TTF),
      TEST_INDEX,
      lse_string_new(TEST_FAMILY),
      TEST_STYLE,
      TEST_WEIGHT,
      LSE_FONT_STORE_SYNC);

  munit_assert_int32(status, ==, LSE_OK);

  fixture->font = lse_font_store_acquire_font(fixture->store, TEST_FAMILY, TEST_STYLE, TEST_WEIGHT);

  munit_assert_not_null(fixture->font);
  munit_assert_int32(lse_font_get_style(fixture->font), ==, TEST_STYLE);
  munit_assert_int32(lse_font_get_weight(fixture->font), ==, TEST_WEIGHT);
}

TEST_CASE(lse_font_store_acquire_font_2, "should acquire a font when family/style/weight do not exactly match") {
  lse_status status = lse_font_store_add_font(
      fixture->store,
      lse_string_new(TEST_URI_TTF),
      TEST_INDEX,
      lse_string_new(TEST_FAMILY),
      TEST_STYLE,
      TEST_WEIGHT,
      LSE_FONT_STORE_SYNC);

  munit_assert_int32(status, ==, LSE_OK);

  fixture->font =
      lse_font_store_acquire_font(fixture->store, TEST_FAMILY, LSE_STYLE_FONT_STYLE_ITALIC, LSE_STYLE_FONT_WEIGHT_BOLD);

  munit_assert_not_null(fixture->font);
  munit_assert_int32(lse_font_get_style(fixture->font), ==, TEST_STYLE);
  munit_assert_int32(lse_font_get_weight(fixture->font), ==, TEST_WEIGHT);
}

// TODO: handle default font
SKIP_TEST_CASE(lse_font_store_acquire_font_3, "should return NULL if no font can be found") {
  lse_font* font =
      lse_font_store_acquire_font(fixture->store, TEST_FAMILY, LSE_STYLE_FONT_STYLE_ITALIC, LSE_STYLE_FONT_WEIGHT_BOLD);

  munit_assert_null(font);
}

TEST_CASE(lse_font_store_release_font_1, "should release acquired font") {
  lse_status status = lse_font_store_add_font(
      fixture->store,
      lse_string_new(TEST_URI_TTF),
      TEST_INDEX,
      lse_string_new(TEST_FAMILY),
      TEST_STYLE,
      TEST_WEIGHT,
      LSE_FONT_STORE_SYNC);

  munit_assert_int32(status, ==, LSE_OK);

  lse_font* font = lse_font_store_acquire_font(fixture->store, TEST_FAMILY, TEST_STYLE, TEST_WEIGHT);

  munit_assert_not_null(font);

  lse_font_store_release_font(fixture->store, font);
}

TEST_CASE(lse_font_store_release_font_2, "should be a no-op for NULL font") {
  lse_font_store_release_font(fixture->store, NULL);
}

TEST_CASE(lse_font_store_add_font_1, "should add supported font file formats") {
  const char* uris[] = {
    TEST_URI_TTF,
    "assets/roboto.otf",
    "assets/roboto.woff",
  };

  for (size_t i = 0; i < c_arraylen(uris); i++) {
    test_add_font(fixture, uris[i], LSE_RESOURCE_STATE_READY);
  }
}

TEST_CASE(lse_font_store_add_font_2, "should return font in ERROR state for unsupported fonts") {
  const char* uris[] = {
    "assets/roboto.eot",
    "assets/roboto.svg",
    "assets/roboto.woff2",
  };

  for (size_t i = 0; i < c_arraylen(uris); i++) {
    test_add_font(fixture, uris[i], LSE_RESOURCE_STATE_ERROR);
  }
}

TEST_CASE(lse_font_store_add_font_3, "should return error status if font is already registered") {
  lse_status status;

  status = lse_font_store_add_font(
      fixture->store,
      lse_string_new(TEST_URI_TTF),
      TEST_INDEX,
      lse_string_new(TEST_FAMILY),
      TEST_STYLE,
      TEST_WEIGHT,
      LSE_FONT_STORE_SYNC);

  munit_assert_int32(status, ==, LSE_OK);

  status = lse_font_store_add_font(
      fixture->store,
      lse_string_new(TEST_URI_TTF),
      TEST_INDEX,
      lse_string_new(TEST_FAMILY),
      TEST_STYLE,
      TEST_WEIGHT,
      LSE_FONT_STORE_SYNC);

  munit_assert_int32(status, ==, LSE_ERR_RES_FONT_KEY);
}

// @private
static void test_add_font(lse_test_fixture* fixture, const char* uri, lse_resource_state expected_state) {
  const char* family = uri; // uri is just convenient to use as a unique family name
  lse_status status = lse_font_store_add_font(
      fixture->store,
      lse_string_new(uri),
      TEST_INDEX,
      lse_string_new(family),
      TEST_STYLE,
      TEST_WEIGHT,
      LSE_FONT_STORE_SYNC);

  munit_assert_int32(status, ==, LSE_OK);

  lse_font* font = lse_font_store_get_font(fixture->store, family, TEST_STYLE, TEST_WEIGHT);

  munit_assert_not_null(font);
  munit_assert_string_equal(lse_string_as_cstring(lse_font_get_family(font)), family);
  munit_assert_string_equal(lse_string_as_cstring(lse_font_get_uri(font)), uri);
  munit_assert_int32(lse_font_get_index(font), ==, TEST_INDEX);
  munit_assert_int32(lse_font_get_style(font), ==, TEST_STYLE);
  munit_assert_int32(lse_font_get_weight(font), ==, TEST_WEIGHT);
  munit_assert_int32(lse_font_get_state(font), ==, expected_state);
}
