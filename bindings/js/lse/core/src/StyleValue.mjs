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

export class StyleValue {
  static UNDEFINED = new StyleValue(NaN, 0)

  constructor (value, unit) {
    // TODO: ctor(string), ctor(number(, px)), ctor(value, unit)
    this[0] = value
    this[1] = unit
  }

  get value () {
    // TODO: to string, depending on unit
    return this[0]
  }

  get unit () {
    // TODO: to string
    return this[1]
  }

  isUndefined () {
    return this[1] === 0
  }
}
