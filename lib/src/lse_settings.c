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

lse_settings lse_settings_init() {
  return (lse_settings){
    .mock_settings = { .enabled = false },
    .sdl_mixer_settings = { .enabled = true },
    .sdl_settings = { .enabled = true },
  };
}

lse_settings lse_settings_mock() {
  lse_settings settings = lse_settings_init();

  settings.mock_settings.enabled = true;

  return settings;
}
