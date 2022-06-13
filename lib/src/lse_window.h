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

#pragma once

#include "lse_types.h"

//
// public api
//

lse_env* lse_window_get_env(lse_window* window);

lse_image_store* lse_window_get_image_store(lse_window* window);
void lse_window_destroy(lse_window* window);
void lse_window_present(lse_window* window);

const lse_style_context* lse_window_get_style_context(lse_window* window);
void lse_window_dispatch_root_font_size_change(lse_window* window, float root_font_size);

void lse_window_destroy_render_object(lse_window* window, lse_render_object* render_object);
