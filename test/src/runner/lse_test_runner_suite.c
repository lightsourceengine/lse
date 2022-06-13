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

// ////////////////////////////////////////////////////////////////////////////
// AUTO-GENERATED - DO NOT EDIT
// clang-format off
// ////////////////////////////////////////////////////////////////////////////

#include <lse_test.h>

extern void* lse_array_before_each(const MunitParameter params[], void* user_data);
extern void lse_array_after_each(void* fixture);
extern const char* test_lse_array_new_1_description;
extern MunitResult test_lse_array_new_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_array_new_2_description;
extern MunitResult test_lse_array_new_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_array_new_empty_1_description;
extern MunitResult test_lse_array_new_empty_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_array_at_1_description;
extern MunitResult test_lse_array_at_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_array_get_1_description;
extern MunitResult test_lse_array_get_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_array_get_2_description;
extern MunitResult test_lse_array_get_2(const MunitParameter params[], void* fixture);

extern void* lse_env_before_each(const MunitParameter params[], void* user_data);
extern void lse_env_after_each(void* fixture);
extern const char* test_lse_env_get_video_driver_1_description;
extern MunitResult test_lse_env_get_video_driver_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_video_driver_2_description;
extern MunitResult test_lse_env_get_video_driver_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_video_driver_count_1_description;
extern MunitResult test_lse_env_get_video_driver_count_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_display_name_1_description;
extern MunitResult test_lse_env_get_display_name_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_display_name_2_description;
extern MunitResult test_lse_env_get_display_name_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_current_display_mode_1_description;
extern MunitResult test_lse_env_get_current_display_mode_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_current_display_mode_2_description;
extern MunitResult test_lse_env_get_current_display_mode_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_desktop_display_mode_1_description;
extern MunitResult test_lse_env_get_desktop_display_mode_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_desktop_display_mode_2_description;
extern MunitResult test_lse_env_get_desktop_display_mode_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_display_count_1_description;
extern MunitResult test_lse_env_get_display_count_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_display_mode_1_description;
extern MunitResult test_lse_env_get_display_mode_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_display_mode_2_description;
extern MunitResult test_lse_env_get_display_mode_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_display_mode_3_description;
extern MunitResult test_lse_env_get_display_mode_3(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_display_mode_count_1_description;
extern MunitResult test_lse_env_get_display_mode_count_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_display_mode_count_2_description;
extern MunitResult test_lse_env_get_display_mode_count_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_renderer_count_1_description;
extern MunitResult test_lse_env_get_renderer_count_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_renderer_info_1_description;
extern MunitResult test_lse_env_get_renderer_info_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_get_renderer_info_2_description;
extern MunitResult test_lse_env_get_renderer_info_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_env_find_closest_display_mode_1_description;
extern MunitResult test_lse_env_find_closest_display_mode_1(const MunitParameter params[], void* fixture);

extern void* lse_event_observers_before_each(const MunitParameter params[], void* user_data);
extern void lse_event_observers_after_each(void* fixture);
extern const char* test_lse_event_observers_add_1_description;
extern MunitResult test_lse_event_observers_add_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_event_observers_remove_1_description;
extern MunitResult test_lse_event_observers_remove_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_event_observers_dispatch_1_description;
extern MunitResult test_lse_event_observers_dispatch_1(const MunitParameter params[], void* fixture);

extern void* lse_font_before_each(const MunitParameter params[], void* user_data);
extern void lse_font_after_each(void* fixture);
extern const char* test_lse_font_constructor_1_description;
extern MunitResult test_lse_font_constructor_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_set_ready_1_description;
extern MunitResult test_lse_font_set_ready_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_set_error_1_description;
extern MunitResult test_lse_font_set_error_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_set_loading_1_description;
extern MunitResult test_lse_font_set_loading_1(const MunitParameter params[], void* fixture);

extern void* lse_font_store_before_each(const MunitParameter params[], void* user_data);
extern void lse_font_store_after_each(void* fixture);
extern const char* test_lse_font_store_acquire_font_1_description;
extern MunitResult test_lse_font_store_acquire_font_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_store_acquire_font_2_description;
extern MunitResult test_lse_font_store_acquire_font_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_store_release_font_1_description;
extern MunitResult test_lse_font_store_release_font_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_store_release_font_2_description;
extern MunitResult test_lse_font_store_release_font_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_store_add_font_1_description;
extern MunitResult test_lse_font_store_add_font_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_store_add_font_2_description;
extern MunitResult test_lse_font_store_add_font_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_font_store_add_font_3_description;
extern MunitResult test_lse_font_store_add_font_3(const MunitParameter params[], void* fixture);

extern void* lse_image_before_each(const MunitParameter params[], void* user_data);
extern void lse_image_after_each(void* fixture);
extern const char* test_lse_image_constructor_1_description;
extern MunitResult test_lse_image_constructor_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_set_loading_1_description;
extern MunitResult test_lse_image_set_loading_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_set_ready_1_description;
extern MunitResult test_lse_image_set_ready_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_set_error_1_description;
extern MunitResult test_lse_image_set_error_1(const MunitParameter params[], void* fixture);

extern void* lse_image_store_before_each(const MunitParameter params[], void* user_data);
extern void lse_image_store_after_each(void* fixture);
extern const char* test_lse_image_store_acquire_1_description;
extern MunitResult test_lse_image_store_acquire_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_store_acquire_2_description;
extern MunitResult test_lse_image_store_acquire_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_store_acquire_3_description;
extern MunitResult test_lse_image_store_acquire_3(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_store_acquire_4_description;
extern MunitResult test_lse_image_store_acquire_4(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_store_acquire_5_description;
extern MunitResult test_lse_image_store_acquire_5(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_store_release_1_description;
extern MunitResult test_lse_image_store_release_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_image_store_release_2_description;
extern MunitResult test_lse_image_store_release_2(const MunitParameter params[], void* fixture);

extern void* lse_node_before_each(const MunitParameter params[], void* user_data);
extern void lse_node_after_each(void* fixture);
extern const char* test_lse_node_get_parent_1_description;
extern MunitResult test_lse_node_get_parent_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_node_get_child_count_1_description;
extern MunitResult test_lse_node_get_child_count_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_node_get_child_at_1_description;
extern MunitResult test_lse_node_get_child_at_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_node_append_1_description;
extern MunitResult test_lse_node_append_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_node_insert_before_1_description;
extern MunitResult test_lse_node_insert_before_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_node_remove_child_1_description;
extern MunitResult test_lse_node_remove_child_1(const MunitParameter params[], void* fixture);

extern void* lse_object_before_each(const MunitParameter params[], void* user_data);
extern void lse_object_after_each(void* fixture);
extern const char* test_lse_object_new_1_description;
extern MunitResult test_lse_object_new_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_object_new_2_description;
extern MunitResult test_lse_object_new_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_object_ref_1_description;
extern MunitResult test_lse_object_ref_1(const MunitParameter params[], void* fixture);

extern void* lse_string_before_each(const MunitParameter params[], void* user_data);
extern void lse_string_after_each(void* fixture);
extern const char* test_lse_string_new_1_description;
extern MunitResult test_lse_string_new_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_string_new_2_description;
extern MunitResult test_lse_string_new_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_string_new_3_description;
extern MunitResult test_lse_string_new_3(const MunitParameter params[], void* fixture);
extern const char* test_lse_string_new_with_size_1_description;
extern MunitResult test_lse_string_new_with_size_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_string_new_with_size_2_description;
extern MunitResult test_lse_string_new_with_size_2(const MunitParameter params[], void* fixture);

extern void* lse_style_before_each(const MunitParameter params[], void* user_data);
extern void lse_style_after_each(void* fixture);
extern const char* test_lse_style_new_1_description;
extern MunitResult test_lse_style_new_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_from_string_1_description;
extern MunitResult test_lse_style_from_string_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_from_string_2_description;
extern MunitResult test_lse_style_from_string_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_from_string_3_description;
extern MunitResult test_lse_style_from_string_3(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_from_string_4_description;
extern MunitResult test_lse_style_from_string_4(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_from_string_5_description;
extern MunitResult test_lse_style_from_string_5(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_set_object_1_description;
extern MunitResult test_lse_style_set_object_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_transform_new_1_description;
extern MunitResult test_lse_style_transform_new_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_transform_new_2_description;
extern MunitResult test_lse_style_transform_new_2(const MunitParameter params[], void* fixture);



extern const char* test_lse_style_meta_set_enum_1_description;
extern MunitResult test_lse_style_meta_set_enum_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_meta_set_enum_2_description;
extern MunitResult test_lse_style_meta_set_enum_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_meta_set_enum_3_description;
extern MunitResult test_lse_style_meta_set_enum_3(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_meta_enum_to_string_1_description;
extern MunitResult test_lse_style_meta_enum_to_string_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_meta_enum_to_string_2_description;
extern MunitResult test_lse_style_meta_enum_to_string_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_meta_enum_to_string_3_description;
extern MunitResult test_lse_style_meta_enum_to_string_3(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_meta_from_string_1_description;
extern MunitResult test_lse_style_meta_from_string_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_meta_from_string_2_description;
extern MunitResult test_lse_style_meta_from_string_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_style_meta_from_string_3_description;
extern MunitResult test_lse_style_meta_from_string_3(const MunitParameter params[], void* fixture);

extern void* lse_text_before_each(const MunitParameter params[], void* user_data);
extern void lse_text_after_each(void* fixture);
extern const char* test_lse_text_measure_1_description;
extern MunitResult test_lse_text_measure_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_text_measure_2_description;
extern MunitResult test_lse_text_measure_2(const MunitParameter params[], void* fixture);
extern const char* test_lse_text_measure_3_description;
extern MunitResult test_lse_text_measure_3(const MunitParameter params[], void* fixture);

extern void* lse_window_before_each(const MunitParameter params[], void* user_data);
extern void lse_window_after_each(void* fixture);
extern const char* test_lse_window_get_root_description;
extern MunitResult test_lse_window_get_root(const MunitParameter params[], void* fixture);
extern const char* test_lse_window_reset_1_description;
extern MunitResult test_lse_window_reset_1(const MunitParameter params[], void* fixture);
extern const char* test_lse_window_reset_2_description;
extern MunitResult test_lse_window_reset_2(const MunitParameter params[], void* fixture);

#define STRINGIFY(SYM) #SYM

MunitSuite lse_test_runner_suite_init() {
  MunitSuite* suites = (MunitSuite*)calloc(14 + 1, sizeof(MunitSuite));
  size_t suites_push_index = 0;

  lse_test_info tests_0 [] = {
      { .name = STRINGIFY(test_lse_array_new_1), .desc = test_lse_array_new_1_description, .test = test_lse_array_new_1 },
      { .name = STRINGIFY(test_lse_array_new_2), .desc = test_lse_array_new_2_description, .test = test_lse_array_new_2 },
      { .name = STRINGIFY(test_lse_array_new_empty_1), .desc = test_lse_array_new_empty_1_description, .test = test_lse_array_new_empty_1 },
      { .name = STRINGIFY(test_lse_array_at_1), .desc = test_lse_array_at_1_description, .test = test_lse_array_at_1 },
      { .name = STRINGIFY(test_lse_array_get_1), .desc = test_lse_array_get_1_description, .test = test_lse_array_get_1 },
      { .name = STRINGIFY(test_lse_array_get_2), .desc = test_lse_array_get_2_description, .test = test_lse_array_get_2 },
  };
  MunitTestSetup tests_0_before_each = &lse_array_before_each;
  MunitTestTearDown tests_0_after_each = &lse_array_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_0, sizeof(tests_0) / sizeof(tests_0[0]), tests_0_before_each, tests_0_after_each);

  lse_test_info tests_1 [] = {
      { .name = STRINGIFY(test_lse_env_get_video_driver_1), .desc = test_lse_env_get_video_driver_1_description, .test = test_lse_env_get_video_driver_1 },
      { .name = STRINGIFY(test_lse_env_get_video_driver_2), .desc = test_lse_env_get_video_driver_2_description, .test = test_lse_env_get_video_driver_2 },
      { .name = STRINGIFY(test_lse_env_get_video_driver_count_1), .desc = test_lse_env_get_video_driver_count_1_description, .test = test_lse_env_get_video_driver_count_1 },
      { .name = STRINGIFY(test_lse_env_get_display_name_1), .desc = test_lse_env_get_display_name_1_description, .test = test_lse_env_get_display_name_1 },
      { .name = STRINGIFY(test_lse_env_get_display_name_2), .desc = test_lse_env_get_display_name_2_description, .test = test_lse_env_get_display_name_2 },
      { .name = STRINGIFY(test_lse_env_get_current_display_mode_1), .desc = test_lse_env_get_current_display_mode_1_description, .test = test_lse_env_get_current_display_mode_1 },
      { .name = STRINGIFY(test_lse_env_get_current_display_mode_2), .desc = test_lse_env_get_current_display_mode_2_description, .test = test_lse_env_get_current_display_mode_2 },
      { .name = STRINGIFY(test_lse_env_get_desktop_display_mode_1), .desc = test_lse_env_get_desktop_display_mode_1_description, .test = test_lse_env_get_desktop_display_mode_1 },
      { .name = STRINGIFY(test_lse_env_get_desktop_display_mode_2), .desc = test_lse_env_get_desktop_display_mode_2_description, .test = test_lse_env_get_desktop_display_mode_2 },
      { .name = STRINGIFY(test_lse_env_get_display_count_1), .desc = test_lse_env_get_display_count_1_description, .test = test_lse_env_get_display_count_1 },
      { .name = STRINGIFY(test_lse_env_get_display_mode_1), .desc = test_lse_env_get_display_mode_1_description, .test = test_lse_env_get_display_mode_1 },
      { .name = STRINGIFY(test_lse_env_get_display_mode_2), .desc = test_lse_env_get_display_mode_2_description, .test = test_lse_env_get_display_mode_2 },
      { .name = STRINGIFY(test_lse_env_get_display_mode_3), .desc = test_lse_env_get_display_mode_3_description, .test = test_lse_env_get_display_mode_3 },
      { .name = STRINGIFY(test_lse_env_get_display_mode_count_1), .desc = test_lse_env_get_display_mode_count_1_description, .test = test_lse_env_get_display_mode_count_1 },
      { .name = STRINGIFY(test_lse_env_get_display_mode_count_2), .desc = test_lse_env_get_display_mode_count_2_description, .test = test_lse_env_get_display_mode_count_2 },
      { .name = STRINGIFY(test_lse_env_get_renderer_count_1), .desc = test_lse_env_get_renderer_count_1_description, .test = test_lse_env_get_renderer_count_1 },
      { .name = STRINGIFY(test_lse_env_get_renderer_info_1), .desc = test_lse_env_get_renderer_info_1_description, .test = test_lse_env_get_renderer_info_1 },
      { .name = STRINGIFY(test_lse_env_get_renderer_info_2), .desc = test_lse_env_get_renderer_info_2_description, .test = test_lse_env_get_renderer_info_2 },
      { .name = STRINGIFY(test_lse_env_find_closest_display_mode_1), .desc = test_lse_env_find_closest_display_mode_1_description, .test = test_lse_env_find_closest_display_mode_1 },
  };
  MunitTestSetup tests_1_before_each = &lse_env_before_each;
  MunitTestTearDown tests_1_after_each = &lse_env_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_1, sizeof(tests_1) / sizeof(tests_1[0]), tests_1_before_each, tests_1_after_each);

  lse_test_info tests_2 [] = {
      { .name = STRINGIFY(test_lse_event_observers_add_1), .desc = test_lse_event_observers_add_1_description, .test = test_lse_event_observers_add_1 },
      { .name = STRINGIFY(test_lse_event_observers_remove_1), .desc = test_lse_event_observers_remove_1_description, .test = test_lse_event_observers_remove_1 },
      { .name = STRINGIFY(test_lse_event_observers_dispatch_1), .desc = test_lse_event_observers_dispatch_1_description, .test = test_lse_event_observers_dispatch_1 },
  };
  MunitTestSetup tests_2_before_each = &lse_event_observers_before_each;
  MunitTestTearDown tests_2_after_each = &lse_event_observers_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_2, sizeof(tests_2) / sizeof(tests_2[0]), tests_2_before_each, tests_2_after_each);

  lse_test_info tests_3 [] = {
      { .name = STRINGIFY(test_lse_font_constructor_1), .desc = test_lse_font_constructor_1_description, .test = test_lse_font_constructor_1 },
      { .name = STRINGIFY(test_lse_font_set_ready_1), .desc = test_lse_font_set_ready_1_description, .test = test_lse_font_set_ready_1 },
      { .name = STRINGIFY(test_lse_font_set_error_1), .desc = test_lse_font_set_error_1_description, .test = test_lse_font_set_error_1 },
      { .name = STRINGIFY(test_lse_font_set_loading_1), .desc = test_lse_font_set_loading_1_description, .test = test_lse_font_set_loading_1 },
  };
  MunitTestSetup tests_3_before_each = &lse_font_before_each;
  MunitTestTearDown tests_3_after_each = &lse_font_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_3, sizeof(tests_3) / sizeof(tests_3[0]), tests_3_before_each, tests_3_after_each);

  lse_test_info tests_4 [] = {
      { .name = STRINGIFY(test_lse_font_store_acquire_font_1), .desc = test_lse_font_store_acquire_font_1_description, .test = test_lse_font_store_acquire_font_1 },
      { .name = STRINGIFY(test_lse_font_store_acquire_font_2), .desc = test_lse_font_store_acquire_font_2_description, .test = test_lse_font_store_acquire_font_2 },
      { .name = STRINGIFY(test_lse_font_store_release_font_1), .desc = test_lse_font_store_release_font_1_description, .test = test_lse_font_store_release_font_1 },
      { .name = STRINGIFY(test_lse_font_store_release_font_2), .desc = test_lse_font_store_release_font_2_description, .test = test_lse_font_store_release_font_2 },
      { .name = STRINGIFY(test_lse_font_store_add_font_1), .desc = test_lse_font_store_add_font_1_description, .test = test_lse_font_store_add_font_1 },
      { .name = STRINGIFY(test_lse_font_store_add_font_2), .desc = test_lse_font_store_add_font_2_description, .test = test_lse_font_store_add_font_2 },
      { .name = STRINGIFY(test_lse_font_store_add_font_3), .desc = test_lse_font_store_add_font_3_description, .test = test_lse_font_store_add_font_3 },
  };
  MunitTestSetup tests_4_before_each = &lse_font_store_before_each;
  MunitTestTearDown tests_4_after_each = &lse_font_store_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_4, sizeof(tests_4) / sizeof(tests_4[0]), tests_4_before_each, tests_4_after_each);

  lse_test_info tests_5 [] = {
      { .name = STRINGIFY(test_lse_image_constructor_1), .desc = test_lse_image_constructor_1_description, .test = test_lse_image_constructor_1 },
      { .name = STRINGIFY(test_lse_image_set_loading_1), .desc = test_lse_image_set_loading_1_description, .test = test_lse_image_set_loading_1 },
      { .name = STRINGIFY(test_lse_image_set_ready_1), .desc = test_lse_image_set_ready_1_description, .test = test_lse_image_set_ready_1 },
      { .name = STRINGIFY(test_lse_image_set_error_1), .desc = test_lse_image_set_error_1_description, .test = test_lse_image_set_error_1 },
  };
  MunitTestSetup tests_5_before_each = &lse_image_before_each;
  MunitTestTearDown tests_5_after_each = &lse_image_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_5, sizeof(tests_5) / sizeof(tests_5[0]), tests_5_before_each, tests_5_after_each);

  lse_test_info tests_6 [] = {
      { .name = STRINGIFY(test_lse_image_store_acquire_1), .desc = test_lse_image_store_acquire_1_description, .test = test_lse_image_store_acquire_1 },
      { .name = STRINGIFY(test_lse_image_store_acquire_2), .desc = test_lse_image_store_acquire_2_description, .test = test_lse_image_store_acquire_2 },
      { .name = STRINGIFY(test_lse_image_store_acquire_3), .desc = test_lse_image_store_acquire_3_description, .test = test_lse_image_store_acquire_3 },
      { .name = STRINGIFY(test_lse_image_store_acquire_4), .desc = test_lse_image_store_acquire_4_description, .test = test_lse_image_store_acquire_4 },
      { .name = STRINGIFY(test_lse_image_store_acquire_5), .desc = test_lse_image_store_acquire_5_description, .test = test_lse_image_store_acquire_5 },
      { .name = STRINGIFY(test_lse_image_store_release_1), .desc = test_lse_image_store_release_1_description, .test = test_lse_image_store_release_1 },
      { .name = STRINGIFY(test_lse_image_store_release_2), .desc = test_lse_image_store_release_2_description, .test = test_lse_image_store_release_2 },
  };
  MunitTestSetup tests_6_before_each = &lse_image_store_before_each;
  MunitTestTearDown tests_6_after_each = &lse_image_store_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_6, sizeof(tests_6) / sizeof(tests_6[0]), tests_6_before_each, tests_6_after_each);

  lse_test_info tests_7 [] = {
      { .name = STRINGIFY(test_lse_node_get_parent_1), .desc = test_lse_node_get_parent_1_description, .test = test_lse_node_get_parent_1 },
      { .name = STRINGIFY(test_lse_node_get_child_count_1), .desc = test_lse_node_get_child_count_1_description, .test = test_lse_node_get_child_count_1 },
      { .name = STRINGIFY(test_lse_node_get_child_at_1), .desc = test_lse_node_get_child_at_1_description, .test = test_lse_node_get_child_at_1 },
      { .name = STRINGIFY(test_lse_node_append_1), .desc = test_lse_node_append_1_description, .test = test_lse_node_append_1 },
      { .name = STRINGIFY(test_lse_node_insert_before_1), .desc = test_lse_node_insert_before_1_description, .test = test_lse_node_insert_before_1 },
      { .name = STRINGIFY(test_lse_node_remove_child_1), .desc = test_lse_node_remove_child_1_description, .test = test_lse_node_remove_child_1 },
  };
  MunitTestSetup tests_7_before_each = &lse_node_before_each;
  MunitTestTearDown tests_7_after_each = &lse_node_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_7, sizeof(tests_7) / sizeof(tests_7[0]), tests_7_before_each, tests_7_after_each);

  lse_test_info tests_8 [] = {
      { .name = STRINGIFY(test_lse_object_new_1), .desc = test_lse_object_new_1_description, .test = test_lse_object_new_1 },
      { .name = STRINGIFY(test_lse_object_new_2), .desc = test_lse_object_new_2_description, .test = test_lse_object_new_2 },
      { .name = STRINGIFY(test_lse_object_ref_1), .desc = test_lse_object_ref_1_description, .test = test_lse_object_ref_1 },
  };
  MunitTestSetup tests_8_before_each = &lse_object_before_each;
  MunitTestTearDown tests_8_after_each = &lse_object_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_8, sizeof(tests_8) / sizeof(tests_8[0]), tests_8_before_each, tests_8_after_each);

  lse_test_info tests_9 [] = {
      { .name = STRINGIFY(test_lse_string_new_1), .desc = test_lse_string_new_1_description, .test = test_lse_string_new_1 },
      { .name = STRINGIFY(test_lse_string_new_2), .desc = test_lse_string_new_2_description, .test = test_lse_string_new_2 },
      { .name = STRINGIFY(test_lse_string_new_3), .desc = test_lse_string_new_3_description, .test = test_lse_string_new_3 },
      { .name = STRINGIFY(test_lse_string_new_with_size_1), .desc = test_lse_string_new_with_size_1_description, .test = test_lse_string_new_with_size_1 },
      { .name = STRINGIFY(test_lse_string_new_with_size_2), .desc = test_lse_string_new_with_size_2_description, .test = test_lse_string_new_with_size_2 },
  };
  MunitTestSetup tests_9_before_each = &lse_string_before_each;
  MunitTestTearDown tests_9_after_each = &lse_string_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_9, sizeof(tests_9) / sizeof(tests_9[0]), tests_9_before_each, tests_9_after_each);

  lse_test_info tests_10 [] = {
      { .name = STRINGIFY(test_lse_style_new_1), .desc = test_lse_style_new_1_description, .test = test_lse_style_new_1 },
      { .name = STRINGIFY(test_lse_style_from_string_1), .desc = test_lse_style_from_string_1_description, .test = test_lse_style_from_string_1 },
      { .name = STRINGIFY(test_lse_style_from_string_2), .desc = test_lse_style_from_string_2_description, .test = test_lse_style_from_string_2 },
      { .name = STRINGIFY(test_lse_style_from_string_3), .desc = test_lse_style_from_string_3_description, .test = test_lse_style_from_string_3 },
      { .name = STRINGIFY(test_lse_style_from_string_4), .desc = test_lse_style_from_string_4_description, .test = test_lse_style_from_string_4 },
      { .name = STRINGIFY(test_lse_style_from_string_5), .desc = test_lse_style_from_string_5_description, .test = test_lse_style_from_string_5 },
      { .name = STRINGIFY(test_lse_style_set_object_1), .desc = test_lse_style_set_object_1_description, .test = test_lse_style_set_object_1 },
      { .name = STRINGIFY(test_lse_style_transform_new_1), .desc = test_lse_style_transform_new_1_description, .test = test_lse_style_transform_new_1 },
      { .name = STRINGIFY(test_lse_style_transform_new_2), .desc = test_lse_style_transform_new_2_description, .test = test_lse_style_transform_new_2 },
  };
  MunitTestSetup tests_10_before_each = &lse_style_before_each;
  MunitTestTearDown tests_10_after_each = &lse_style_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_10, sizeof(tests_10) / sizeof(tests_10[0]), tests_10_before_each, tests_10_after_each);

  lse_test_info tests_11 [] = {
      { .name = STRINGIFY(test_lse_style_meta_set_enum_1), .desc = test_lse_style_meta_set_enum_1_description, .test = test_lse_style_meta_set_enum_1 },
      { .name = STRINGIFY(test_lse_style_meta_set_enum_2), .desc = test_lse_style_meta_set_enum_2_description, .test = test_lse_style_meta_set_enum_2 },
      { .name = STRINGIFY(test_lse_style_meta_set_enum_3), .desc = test_lse_style_meta_set_enum_3_description, .test = test_lse_style_meta_set_enum_3 },
      { .name = STRINGIFY(test_lse_style_meta_enum_to_string_1), .desc = test_lse_style_meta_enum_to_string_1_description, .test = test_lse_style_meta_enum_to_string_1 },
      { .name = STRINGIFY(test_lse_style_meta_enum_to_string_2), .desc = test_lse_style_meta_enum_to_string_2_description, .test = test_lse_style_meta_enum_to_string_2 },
      { .name = STRINGIFY(test_lse_style_meta_enum_to_string_3), .desc = test_lse_style_meta_enum_to_string_3_description, .test = test_lse_style_meta_enum_to_string_3 },
      { .name = STRINGIFY(test_lse_style_meta_from_string_1), .desc = test_lse_style_meta_from_string_1_description, .test = test_lse_style_meta_from_string_1 },
      { .name = STRINGIFY(test_lse_style_meta_from_string_2), .desc = test_lse_style_meta_from_string_2_description, .test = test_lse_style_meta_from_string_2 },
      { .name = STRINGIFY(test_lse_style_meta_from_string_3), .desc = test_lse_style_meta_from_string_3_description, .test = test_lse_style_meta_from_string_3 },
  };
  MunitTestSetup tests_11_before_each = NULL;
  MunitTestTearDown tests_11_after_each = NULL;

  suites[suites_push_index++] = lse_test_suite_init(tests_11, sizeof(tests_11) / sizeof(tests_11[0]), tests_11_before_each, tests_11_after_each);

  lse_test_info tests_12 [] = {
      { .name = STRINGIFY(test_lse_text_measure_1), .desc = test_lse_text_measure_1_description, .test = test_lse_text_measure_1 },
      { .name = STRINGIFY(test_lse_text_measure_2), .desc = test_lse_text_measure_2_description, .test = test_lse_text_measure_2 },
      { .name = STRINGIFY(test_lse_text_measure_3), .desc = test_lse_text_measure_3_description, .test = test_lse_text_measure_3 },
  };
  MunitTestSetup tests_12_before_each = &lse_text_before_each;
  MunitTestTearDown tests_12_after_each = &lse_text_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_12, sizeof(tests_12) / sizeof(tests_12[0]), tests_12_before_each, tests_12_after_each);

  lse_test_info tests_13 [] = {
      { .name = STRINGIFY(test_lse_window_get_root), .desc = test_lse_window_get_root_description, .test = test_lse_window_get_root },
      { .name = STRINGIFY(test_lse_window_reset_1), .desc = test_lse_window_reset_1_description, .test = test_lse_window_reset_1 },
      { .name = STRINGIFY(test_lse_window_reset_2), .desc = test_lse_window_reset_2_description, .test = test_lse_window_reset_2 },
  };
  MunitTestSetup tests_13_before_each = &lse_window_before_each;
  MunitTestTearDown tests_13_after_each = &lse_window_after_each;

  suites[suites_push_index++] = lse_test_suite_init(tests_13, sizeof(tests_13) / sizeof(tests_13[0]), tests_13_before_each, tests_13_after_each);

  return (MunitSuite) {
      .prefix = "",
      .tests = NULL,
      .suites = suites,
      .iterations = 1,
      .options = MUNIT_SUITE_OPTION_NONE
  };
}

void lse_test_runner_suite_free(MunitSuite* suite) {
  for (size_t i = 0; suite->suites[i].tests != NULL; i++) {
    lse_test_suite_free(&suite->suites[i]);
  }

  free(suite->suites);
  suite->suites = NULL;
}