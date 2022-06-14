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

const { StyleTransform } = __internal.Style
const { IDENTITY, TRANSLATE, SCALE, ROTATE } = StyleTransform

export const identity = () => new StyleTransform(IDENTITY)

export const translate = (x, y) => new StyleTransform(TRANSLATE, x, y)

export const translateX = (x) => translate(x, 0)

export const translateY = (y) => translate(0, y)

export const scale = (x, y) => new StyleTransform(SCALE, x, y)

export const scaleX = (x) => scale(x, 1)

export const scaleY = (y) => scale(1, y)

export const rotate = angle => new StyleTransform(ROTATE, angle)
