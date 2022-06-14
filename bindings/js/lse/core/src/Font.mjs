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

import { nsfont } from './addon.mjs'
import { toString } from './ResourceState.mjs'

const { getUri, getIndex, getFamily, getStyle, getWeight, getState } = nsfont

export class Font {
  $postConstruct () {
    // $setListener(this, (status) => {})
  }

  get uri () {
    return getUri(this)
  }

  get index () {
    return getIndex(this)
  }

  get family () {
    return getFamily(this)
  }

  get style () {
    // TODO: get strings or enums from native?
    switch (getStyle(this)) {
      case 0: return 'normal'
      case 1: return 'italic'
      case 2: return 'oblique'
      default: return 'unknown'
    }
  }

  get weight () {
    // TODO: get strings or enums from native?
    switch (getWeight(this)) {
      case 0: return 'normal'
      case 1: return 'bold'
      default: return 'unknown'
    }
  }

  get state () {
    return toString(getState(this))
  }
}
