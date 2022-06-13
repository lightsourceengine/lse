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

#include <lse.h>
#include <lse_test.h>

MunitSuite lse_test_runner_suite_init();
extern void lse_test_runner_suite_free(MunitSuite* suite);

int main(int argc, char* argv[]) {
  int returnCode;
  MunitSuite lse_suite = lse_test_runner_suite_init();

  lse_log_level_set(LSE_LOG_LEVEL_OFF);

  returnCode = munit_suite_main(&lse_suite, NULL, argc, argv);

  lse_test_runner_suite_free(&lse_suite);

  return returnCode;
}
