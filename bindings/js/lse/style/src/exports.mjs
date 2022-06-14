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

import { __internal } from '@lse/core'

export { rgb, rgba, hsl, hsla } from './color-api.mjs'
export { identity, rotate, scale, scaleX, scaleY, translate, translateX, translateY } from './transform-api.mjs'
export { flipH, flipV, tint } from './filter-api.mjs'
export { absoluteFill, value } from './value-api.mjs'

export const { assign, flatten, StyleSheet, StyleClass } = __internal.Style

export const version = $LSE_VERSION
