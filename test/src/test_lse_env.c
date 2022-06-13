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

#include <lse_env.h>

#include <lse_object.h>
#include <lse_test.h>

struct lse_test_fixture {
  lse_env* env;
};

static void assert_display_mode(
    lse_display_mode* mode,
    int32_t expected_width,
    int32_t expected_height,
    int32_t expected_refresh_rate) {
  munit_assert_int32(mode->width, ==, expected_width);
  munit_assert_int32(mode->height, ==, expected_height);
  munit_assert_int32(mode->refresh_rate, ==, expected_refresh_rate);
}

BEFORE_EACH(lse_env) {
  fixture->env = lse_test_env_new();
}

AFTER_EACH(lse_env) {
  lse_test_env_drop(fixture->env);
}

// TODO: move to window?
// TEST_CASE(lse_env_create_node_from_tag_1, "should create node from tag") {
//  lse_node node;
//  lse_object node_obj;
//
//  node = lse_env_create_node_from_tag(fixture->env, LSE_NODE_TAG_IMAGE);
//  node_obj = lse_node_as_object(node);
//
//  munit_assert_not_null(node);
//  munit_assert_uint8(lse_object_get_type(node_obj), ==, lse_image_node_type);
//  munit_assert_uint8(lse_object_get_interface_type(node_obj), ==, lse_node_interface_type);
//  lse_node_unref(node);
//
//  node = lse_env_create_node_from_tag(fixture->env, LSE_NODE_TAG_BOX);
//  node_obj = lse_node_as_object(node);
//
//  munit_assert_not_null(node);
//  munit_assert_uint8(lse_object_get_type(node_obj), ==, lse_box_node_type);
//  munit_assert_uint8(lse_object_get_interface_type(node_obj), ==, lse_node_interface_type);
//  lse_node_unref(node);
//
//  node = lse_env_create_node_from_tag(fixture->env, LSE_NODE_TAG_TEXT);
//  node_obj = lse_node_as_object(node);
//
//  munit_assert_not_null(node);
//  munit_assert_uint8(lse_object_get_type(node_obj), ==, lse_text_node_type);
//  munit_assert_uint8(lse_object_get_interface_type(node_obj), ==, lse_node_interface_type);
//  lse_node_unref(node);
//
//  return MUNIT_OK;
//}
//
// TEST_CASE(lse_env_create_node_from_tag_2, "should return null for invalid or unsupported tag") {
//  munit_assert_null(lse_env_create_node_from_tag(fixture->env, "invalid"));
//  munit_assert_null(lse_env_create_node_from_tag(fixture->env, ""));
//  munit_assert_null(lse_env_create_node_from_tag(fixture->env, NULL));
//
//  return MUNIT_OK;
//}

TEST_CASE(lse_env_get_video_driver_1, "should get mock video driver name") {
  munit_assert_string_equal(lse_env_get_video_driver(fixture->env, 0), "default");
}

TEST_CASE(lse_env_get_video_driver_2, "should return NULL for invalid index") {
  munit_assert_null(lse_env_get_video_driver(fixture->env, -1));
}

TEST_CASE(lse_env_get_video_driver_count_1, "should get mock video driver count") {
  munit_assert_int32(lse_env_get_video_driver_count(fixture->env), ==, 1);
}

TEST_CASE(lse_env_get_display_name_1, "should get mock display name") {
  munit_assert_string_equal(lse_env_get_display_name(fixture->env, 0), "mock_display");
}

TEST_CASE(lse_env_get_display_name_2, "should return NULL for invalid index") {
  munit_assert_null(lse_env_get_display_name(fixture->env, -1));
}

TEST_CASE(lse_env_get_current_display_mode_1, "should get current display mode from mock display") {
  lse_display_mode mode;

  munit_assert_true(lse_env_get_current_display_mode(fixture->env, 0, &mode));
  assert_display_mode(&mode, 1280, 720, 60);
}

TEST_CASE(lse_env_get_current_display_mode_2, "should return false for invalid index") {
  lse_display_mode mode;

  munit_assert_false(lse_env_get_current_display_mode(fixture->env, -1, &mode));
}

TEST_CASE(lse_env_get_desktop_display_mode_1, "should get desktop display mode from mock display") {
  lse_display_mode mode;

  munit_assert_true(lse_env_get_desktop_display_mode(fixture->env, 0, &mode));
  assert_display_mode(&mode, 1280, 720, 60);
}

TEST_CASE(lse_env_get_desktop_display_mode_2, "should return false for invalid index") {
  lse_display_mode mode;

  munit_assert_false(lse_env_get_desktop_display_mode(fixture->env, -1, &mode));
}

TEST_CASE(lse_env_get_display_count_1, "should return mock display count") {
  munit_assert_int32(lse_env_get_display_count(fixture->env), ==, 1);
}

TEST_CASE(lse_env_get_display_mode_1, "should get mock display mode") {
  lse_display_mode mode;

  munit_assert_true(lse_env_get_display_mode(fixture->env, 0, 0, &mode));
  assert_display_mode(&mode, 1280, 720, 60);
}

TEST_CASE(lse_env_get_display_mode_2, "should return false for invalid display index") {
  lse_display_mode mode;

  munit_assert_false(lse_env_get_display_mode(fixture->env, -1, 0, &mode));
}

TEST_CASE(lse_env_get_display_mode_3, "should return false for invalid index") {
  lse_display_mode mode;

  munit_assert_false(lse_env_get_display_mode(fixture->env, 0, -1, &mode));
}

TEST_CASE(lse_env_get_display_mode_count_1, "should get mock display mode count") {
  munit_assert_int32(lse_env_get_display_mode_count(fixture->env, 0), ==, 1);
}

TEST_CASE(lse_env_get_display_mode_count_2, "should return -1 for invalid index") {
  munit_assert_int32(lse_env_get_display_mode_count(fixture->env, -1), ==, -1);
}

TEST_CASE(lse_env_get_renderer_count_1, "should return mock renderer count") {
  munit_assert_int32(lse_env_get_renderer_count(fixture->env), ==, 1);
}

TEST_CASE(lse_env_get_renderer_info_1, "should return mock renderer info") {
  lse_renderer_info info;

  munit_assert_true(lse_env_get_renderer_info(fixture->env, 0, &info));
  munit_assert_string_equal(info.name, "mock renderer");
  munit_assert_true(info.has_hardware_acceleration);
  munit_assert_true(info.has_vsync);
  munit_assert_true(info.has_render_to_texture);
  munit_assert_int32(info.max_texture_width, ==, 0xFFFF);
  munit_assert_int32(info.max_texture_height, ==, 0xFFFF);
}

TEST_CASE(lse_env_get_renderer_info_2, "should return false for invalid index") {
  lse_renderer_info info;

  munit_assert_false(lse_env_get_renderer_info(fixture->env, -1, &info));
}

TEST_CASE(lse_env_find_closest_display_mode_1, "should ...") {
  lse_display_mode mode;

  // mock implementation always returns same display mode!
  munit_assert_true(lse_env_find_closest_display_mode(fixture->env, 0, 1280, 720, &mode));
  assert_display_mode(&mode, 1280, 720, 60);
}