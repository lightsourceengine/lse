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

import { StyleValue } from './StyleValue.mjs'

// TODO: needed? const { $resolveTranslateValue } = $style
const { UNDEFINED } = StyleValue

// TODO: sync with native enum
const IDENTITY = 0
const TRANSLATE = 1
const SCALE = 2
const ROTATE = 3

export class StyleTransform {
  static IDENTITY = IDENTITY
  static TRANSLATE = TRANSLATE
  static SCALE = SCALE
  static ROTATE = ROTATE;

  [0] = -1;
  [1] = UNDEFINED;
  [2] = UNDEFINED

  constructor (type, ...args) {
    switch (type) {
      case TRANSLATE:
      case SCALE:
        this[1] = args[0]
        this[2] = args[1]
        break
      case ROTATE:
        this[1] = args[0]
        break
      default:
        throw Error(`invalid transform type: ${type}`)
    }

    this[0] = type
  }

  get x () {
    return this[1]
  }

  get y () {
    return this[2]
  }

  get angle () {
    return this[1]
  }

  isTranslate () {
    return this[0] === TRANSLATE
  }

  isScale () {
    return this[0] === SCALE
  }

  isRotate () {
    return this[0] === ROTATE
  }
}
