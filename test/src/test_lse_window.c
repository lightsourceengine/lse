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

#include <lse_window.h>

#include <lse_test.h>

struct lse_test_fixture {
  lse_env* env;
  lse_window* window;
};

BEFORE_EACH(lse_window) {
  fixture->env = lse_test_env_new();
  fixture->window = lse_env_add_window(fixture->env);
}

AFTER_EACH(lse_window) {
  lse_unref(fixture->window);
  lse_test_env_drop(fixture->env);
}

TEST_CASE(lse_window_get_root, "should get window root node") {
  lse_node* root = lse_window_get_root(fixture->window);

  munit_assert_not_null(root);
  munit_assert_uint8(lse_object_get_type((lse_object*)root), ==, lse_root_node_type);
}

TEST_CASE(lse_window_reset_1, "should return settings for configured window") {
  lse_window_settings settings;

  // TODO: add helper method for creating settings
  settings.width = 1280;
  settings.height = 720;
  settings.display_index = 0;
  settings.mode = LSE_WINDOW_MODE_FULLSCREEN;
  settings.fit = LSE_WINDOW_FIT_CLOSEST;

  lse_window_configure(fixture->window, &settings);

  // TODO: implement!
  //  munit_assert_int32(lse_window_get_width(fixture->window), ==, 1280);
  //  munit_assert_int32(lse_window_get_height(fixture->window), ==, 720);
  //  munit_assert_int32(lse_window_get_refresh_rate(fixture->window), ==, 60);
  //  munit_assert_int32(lse_window_get_flags(fixture->window), ==, 0);
}

TEST_CASE(lse_window_reset_2, "should return 0 values for un-configured window") {
  munit_assert_int32(lse_window_get_width(fixture->window), ==, 0);
  munit_assert_int32(lse_window_get_height(fixture->window), ==, 0);
  munit_assert_int32(lse_window_get_refresh_rate(fixture->window), ==, 0);
  munit_assert_int32(lse_window_get_flags(fixture->window), ==, 0);
}
