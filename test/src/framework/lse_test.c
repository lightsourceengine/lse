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

#include "lse_test.h"

#include <stdio.h>

static const char* k_test_name_separator = " :: ";

MunitSuite lse_test_suite_init(
    lse_test_info* tests,
    size_t tests_count,
    MunitTestSetup before_each,
    MunitTestTearDown after_each) {
  MunitSuite suite = { .prefix = "",
                       .iterations = 1,
                       .options = MUNIT_SUITE_OPTION_NONE,
                       .suites = NULL,
                       .tests = calloc(tests_count + 1, sizeof(MunitTest)) };

  for (size_t i = 0; i < tests_count; i++) {
    char* buffer =
        calloc(strlen(tests[i].name) + strlen(k_test_name_separator) + strlen(tests[i].desc) + 1, sizeof(char));

    sprintf(buffer, "%s%s%s", tests[i].name, k_test_name_separator, tests[i].desc);

    suite.tests[i] = (MunitTest){ .name = buffer,
                                  .test = tests[i].test,
                                  .setup = before_each,
                                  .tear_down = after_each,
                                  .options = MUNIT_TEST_OPTION_NONE,
                                  .parameters = NULL };
  }

  suite.tests[tests_count] = (MunitTest){ 0 };

  return suite;
}

void lse_test_suite_free(MunitSuite* suite) {
  if (suite->tests) {
    for (size_t i = 0; suite->tests[i].test != NULL; i++) {
      free(suite->tests[i].name);
    }

    free(suite->tests);
  }
}

lse_env* lse_test_env_new() {
  lse_env* mock_env = lse_env_new();
  lse_settings settings = lse_settings_mock();

  lse_env_configure(mock_env, &settings);

  return mock_env;
}

void lse_test_env_drop(lse_env* env) {
  lse_env_destroy(env);
  lse_unref(env);
}
