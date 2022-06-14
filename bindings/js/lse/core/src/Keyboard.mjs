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

import { nskeyboard } from './addon.mjs'

const { getId, getUUID, getName, isConnected, getScancodeState } = nskeyboard

export class Keyboard {
  get id () {
    return getId(this)
  }

  get uuid () {
    return getUUID(this)
  }

  get name () {
    return getName(this)
  }

  get type () {
    return 'keyboard'
  }

  isConnected () {
    return isConnected(this)
  }

  getScancodeState (scancode) {
    return getScancodeState(this, scancode)
  }

  getKeyState (key) {
    // TODO: implement
    return false
  }

  getAnalogState (analogKey) {
    // TODO: implement
    return false
  }
}
