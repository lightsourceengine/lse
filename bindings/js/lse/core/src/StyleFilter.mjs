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

const FLIP_H = 0
const FLIP_V = 1
const TINT = 2

export class StyleFilter {
  [0] = -1;
  [1] = 0

  constructor (type, ...args) {
    this[0] = type
    this[1] = args[0]
  }

  isFlipH () {
    return this[0] === FLIP_H
  }

  isFlipV () {
    return this[0] === FLIP_V
  }

  isTint () {
    return this[0] === TINT
  }

  get color () {
    return this[1]
  }
}
