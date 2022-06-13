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

#include "lse_video.h"

#include "lse_object.h"
#include "stc/ccommon.h"
#include <stdio.h>

lse_string* lse_video_create_stub_mapping(lse_video* video, const char* uuid) {
  const char* platform = lse_video_get_platform_name(video);
  char buffer[64];
  int32_t written;

  // TODO: null platform?
  written = snprintf(buffer, c_arraylen(buffer), "%s,%s,platform:%s,", uuid, "Gamepad", platform);

  if (written <= 0 || written >= c_arraylen(buffer)) {
    return NULL;
  }

  return lse_string_new(buffer);
}
