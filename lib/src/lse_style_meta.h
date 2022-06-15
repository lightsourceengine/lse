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

#include <lse.h>

#define LSE_STYLE_AUTO "auto"

typedef bool (*lse_style_constraint)(lse_style_value*);

bool lse_style_meta_is_style_property(int32_t value);

lse_style_property_type lse_style_meta_get_property_type(lse_style_property prop);

bool lse_style_meta_is_layout_property(lse_style_property prop);

bool lse_style_meta_is_compatible_unit(lse_style_property prop, lse_style_unit unit);

bool lse_style_meta_is_enum_value(lse_style_property prop, int32_t value);

bool lse_style_meta_is_valid_numeric_value(lse_style_property prop, const lse_style_value* numeric);

int32_t lse_style_meta_enum_from_string(lse_style_property prop, const char* string_value);

const char* lse_style_meta_enum_to_string(lse_style_property prop, int32_t enum_value);

int32_t lse_style_meta_get_enum(lse_style_property prop, uint64_t flags);

uint64_t lse_style_meta_set_enum(lse_style_property prop, int32_t enum_value, uint64_t flags);

int32_t lse_style_meta_anchor_from_string(const char* string_value);

int32_t lse_style_meta_unit_from_postfix_string(const char* string_value);

bool lse_style_meta_is_anchor(int32_t value);

bool lse_style_meta_is_unit(int32_t value);
