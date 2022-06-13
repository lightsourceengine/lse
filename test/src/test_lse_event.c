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

#include <lse_event.h>

#include <lse_test.h>

struct lse_test_fixture {
  lse_image_observers observers;
  lse_image_event event;
};

static void callback(const lse_image_event* e, void* observer);

static bool s_callback_called = false;
static const lse_image_event* s_callback_event_arg = NULL;
static void* s_callback_observer_arg = NULL;
static void* s_observer = (intptr_t*)1;

BEFORE_EACH(lse_event_observers) {
  fixture->observers = lse_image_observers_init();
  fixture->event.image = NULL;
  fixture->event.state = LSE_RESOURCE_STATE_READY;

  s_callback_called = false;
  s_callback_event_arg = NULL;
  s_callback_observer_arg = NULL;
}

AFTER_EACH(lse_event_observers) {
  lse_image_observers_drop(&fixture->observers);
}

TEST_CASE(lse_event_observers_add_1, "should add an observer") {
  lse_image_observers_add(&fixture->observers, s_observer, &callback);

  munit_assert_size(lse_image_observers_size(&fixture->observers), ==, 1);
  munit_assert_ptr_equal(fixture->observers.observers.data->observer, s_observer);
  munit_assert_ptr_equal(fixture->observers.observers.data->observer_callback, &callback);
}

TEST_CASE(lse_event_observers_remove_1, "should remove an added observer") {
  lse_image_observers_add(&fixture->observers, s_observer, &callback);
  lse_image_observers_remove(&fixture->observers, s_observer);

  munit_assert_size(lse_image_observers_size(&fixture->observers), ==, 0);
}

TEST_CASE(lse_event_observers_dispatch_1, "should send event to observers") {
  lse_image_observers_add(&fixture->observers, s_observer, &callback);
  lse_image_observers_dispatch_event(&fixture->observers, &fixture->event);

  munit_assert_true(s_callback_called);
  munit_assert_ptr_equal(s_callback_observer_arg, s_observer);
  munit_assert_ptr_equal(s_callback_event_arg, &fixture->event);
}

static void callback(const lse_image_event* e, void* observer) {
  s_callback_called = true;
  s_callback_event_arg = e;
  s_callback_observer_arg = observer;
}
