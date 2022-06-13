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

#ifndef TEST_FIXTURE_TYPE
#define TEST_FIXTURE_TYPE lse_test_fixture
#endif

#pragma once

#include <lse.h>
#include <munit.h>

#define TEST_CASE(ID, DESCRIPTION)                                                                                     \
  const char* test_##ID##_description = DESCRIPTION " ";                                                               \
  static void _test_##ID(TEST_FIXTURE_TYPE* fixture);                                                                  \
  MunitResult test_##ID(const MunitParameter params[], void* fixture) {                                                \
    _test_##ID((TEST_FIXTURE_TYPE*)fixture);                                                                           \
    return MUNIT_OK;                                                                                                   \
  }                                                                                                                    \
  static void _test_##ID(TEST_FIXTURE_TYPE* fixture)
#define SKIP_TEST_CASE(ID, DESCRIPTION) static void _test_##ID(TEST_FIXTURE_TYPE* fixture)
#define BEFORE_EACH(ID)                                                                                                \
  static void _##ID##_before_each(TEST_FIXTURE_TYPE* fixture);                                                         \
  void* ID##_before_each(const MunitParameter params[], void* user_data) {                                             \
    TEST_FIXTURE_TYPE* fixture = calloc(1, sizeof(TEST_FIXTURE_TYPE));                                                 \
    _##ID##_before_each(fixture);                                                                                      \
    return fixture;                                                                                                    \
  }                                                                                                                    \
  static void _##ID##_before_each(TEST_FIXTURE_TYPE* fixture)
#define SKIP_BEFORE_EACH(ID)
#define AFTER_EACH(ID)                                                                                                 \
  static void _##ID##_after_each(TEST_FIXTURE_TYPE* fixture);                                                          \
  void ID##_after_each(void* fixture) {                                                                                \
    _##ID##_after_each((TEST_FIXTURE_TYPE*)fixture);                                                                   \
    free(fixture);                                                                                                     \
  }                                                                                                                    \
  static void _##ID##_after_each(TEST_FIXTURE_TYPE* fixture)
#define SKIP_AFTER_EACH(ID)

typedef struct TEST_FIXTURE_TYPE TEST_FIXTURE_TYPE;

typedef struct {
  const char* name;
  const char* desc;
  MunitTestFunc test;
} lse_test_info;

MunitSuite
lse_test_suite_init(lse_test_info* tests, size_t tests_count, MunitTestSetup before_each, MunitTestTearDown after_each);

void lse_test_suite_free(MunitSuite* suite);

lse_env* lse_test_env_new();
void lse_test_env_drop(lse_env* env);
