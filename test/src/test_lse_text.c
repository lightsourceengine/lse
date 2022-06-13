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

#include <lse_text.h>

#include <lse_font.h>
#include <lse_style.h>
#include <lse_test.h>
#include <stc/ccommon.h>

struct lse_test_fixture {
  lse_env* env;
  lse_text_style style;
};

static float compute_width(lse_test_fixture* fixture, int32_t m_count, int32_t space_count);
static float compute_height(lse_test_fixture* fixture, int32_t expected_line_count);

BEFORE_EACH(lse_text) {
  fixture->env = lse_test_env_new();
  fixture->style = (lse_text_style){
    .font = lse_env_acquire_font(fixture->env, "test", LSE_STYLE_FONT_STYLE_NORMAL, LSE_STYLE_FONT_WEIGHT_NORMAL),
    .font_size_px = 16,
    .context_box =
        (lse_size){
            100.f,
            100.f,
        },
  };
}

AFTER_EACH(lse_text) {
  lse_env_release_font(fixture->env, fixture->style.font);
  lse_env_destroy(fixture->env);
  lse_unref(fixture->env);
}

TEST_CASE(lse_text_measure_1, "should measure single line string, ignoring leading and trailing spaces") {
  lse_size size;
  size_t i;
  lse_string* str;
  const char* words[] = {
    "mmmm  ",
    "mmmm",
    "  mmmm",
    "  mmmm  ",
  };

  for (i = 0; i < c_arraylen(words); i++) {
    str = lse_string_new(words[i]);

    lse_text_measure(str, &fixture->style, &size);
    lse_unref(str);

    munit_assert_true(lse_equals_f(size.width, compute_width(fixture, 4, 0)));
    munit_assert_true(lse_equals_f(size.height, compute_height(fixture, 1)));
  }
}

TEST_CASE(lse_text_measure_2, "should measure string with newline breaks") {
  lse_size size;
  lse_string* str = lse_string_new("mmmm\nmm");

  lse_text_measure(str, &fixture->style, &size);
  lse_unref(str);

  munit_assert_true(lse_equals_f(size.width, compute_width(fixture, 4, 0)));
  munit_assert_true(lse_equals_f(size.height, compute_height(fixture, 2)));
}

TEST_CASE(lse_text_measure_3, "should return 0 size for empty string") {
  lse_size size;
  lse_string* str = lse_string_new_empty();

  lse_text_measure(str, &fixture->style, &size);
  lse_unref(str);

  munit_assert_float(size.width, ==, 0.f);
  munit_assert_float(size.height, ==, 0.f);
}

static float compute_width(lse_test_fixture* fixture, int32_t m_count, int32_t space_count) {
  lse_font_use_font_size(fixture->style.font, fixture->style.font_size_px);

  return (lse_font_get_advance(fixture->style.font, 'm') * (float)m_count) +
         (lse_font_get_advance(fixture->style.font, ' ') * (float)space_count);
}

static float compute_height(lse_test_fixture* fixture, int32_t expected_line_count) {
  lse_font_use_font_size(fixture->style.font, fixture->style.font_size_px);

  return lse_font_get_line_height(fixture->style.font) * (float)expected_line_count;
}
