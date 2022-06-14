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

import { $style } from './addon.mjs'
import {
  StyleClass,
  FOCUS_WITHIN_PSEUDO_CLASS_NAME,
  FOCUS_WITHIN_PSEUDO_CLASS_ID,
  FOCUS_WITHOUT_PSEUDO_CLASS_NAME,
  FOCUS_WITHOUT_PSEUDO_CLASS_ID
} from './StyleClass.mjs'
import { Style } from './Style.mjs'
import { StyleInstance } from './StyleInstance.mjs'
import { isPlainObject } from './util.mjs'

const { $createClass, $lock, $setParent } = $style

// NOTE: pseudo classes not supported by assign!
const assign = (style, obj) => style instanceof StyleInstance && isPlainObject(obj) && doApply(style, obj)

// TODO: need native support to flatten classes efficiently
const flatten = (...args) => { throw Error('not implemented') }

const createStyleClass = (obj) => {
  const style = $createClass(StyleClass)

  if (!doApply(style, obj)) {
    return null
  }

  addPseudoClass(obj, style, FOCUS_WITHIN_PSEUDO_CLASS_NAME, FOCUS_WITHIN_PSEUDO_CLASS_ID)
  addPseudoClass(obj, style, FOCUS_WITHOUT_PSEUDO_CLASS_NAME, FOCUS_WITHOUT_PSEUDO_CLASS_ID)

  return $lock(style)
}

const createStyleSheet = (obj) => {
  const sheet = {}

  for (const [key, value] of Object.entries(obj)) {
    sheet[key] = createStyleClass(value)
  }

  return sheet
}

const doApply = (style, obj) => {
  const { prototype } = Style

  for (const prop in obj) {
    if (prop in prototype) {
      style[prop] = obj[prop]
    }
  }

  return true
}

const addPseudoClass = (raw, style, name, symbol) => {
  if (raw[name]) {
    const pseudoClass = createStyleClass(raw[name])

    if (pseudoClass) {
      style[symbol] = pseudoClass
      $setParent(pseudoClass, style)
    }
  }
}

export const StyleOps = {
  assign,
  flatten,
  StyleClass: createStyleClass,
  StyleSheet: createStyleSheet
}
