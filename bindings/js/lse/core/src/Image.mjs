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

import { nsimage } from './addon.mjs'
import { toString } from './ResourceState.mjs'

const { getWidth, getHeight, getState } = nsimage

export class Image {
  postConstruct () {
    // TODO: implement callbacks from native layer, $setCallback(this, () => {})
  }

  get width () {
    return getWidth(this)
  }

  get height () {
    return getHeight(this)
  }

  get state () {
    return toString(getState(this))
  }
}
