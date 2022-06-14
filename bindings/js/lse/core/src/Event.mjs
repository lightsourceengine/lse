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

import {
  onAnalogMotion,
  onGamepadAxis,
  onGamepadButtonDown,
  onGamepadButtonUp,
  onGamepadConnected,
  onGamepadDisconnected,
  onGamepadHat,
  onKeyboardButtonDown,
  onKeyboardButtonUp,
  onKeyDown,
  onKeyUp
} from './EventSymbols.mjs'
import { now } from './util.mjs'

const kStateInit = -1
const kStateStopPropagation = 1
const kStateStopImmediatePropagation = 2

export class Event {
  #state = kStateInit

  constructor ($type, target, timestamp = now()) {
    this.$type = $type
    this.target = target
    this.currentTarget = target
    this.timestamp = timestamp
  }

  get type () {
    return Symbol.keyFor(this.$type)
  }

  stopPropagation () {
    if (this.cancelable) {
      this.#state = kStateStopPropagation
    }
  }

  stopImmediatePropagation () {
    if (this.cancelable) {
      this.#state = kStateStopImmediatePropagation
    }
  }

  get cancelable () {
    return this.#state >= 0
  }

  hasStopPropagation () {
    return this.#state === kStateStopPropagation
  }

  hasStopImmediatePropagation () {
    return this.#state === kStateStopImmediatePropagation
  }
}

export class GamepadEvent extends Event {
  gamepad = null

  constructor (target, gamepad, connected) {
    super(connected ? onGamepadConnected : onGamepadDisconnected, target)
    this.gamepad = gamepad
  }
}

export class GamepadHatEvent extends Event {
  gamepad = null
  hat = 0
  value = 0

  constructor (target, gamepad, hat, value) {
    super(onGamepadHat, target)
    this.gamepad = gamepad
    this.hat = hat
    this.value = value
  }
}

export class GamepadAxisEvent extends Event {
  gamepad = null
  axis = 0
  value = 0

  constructor (target, gamepad, axis, value) {
    super(onGamepadAxis, target)
    this.gamepad = gamepad
    this.axis = axis
    this.value = value
  }
}

export class GamepadButtonEvent extends Event {
  gamepad = null
  button = -1
  pressed = false
  repeat = false

  constructor (target, gamepad, button, pressed, repeat) {
    super(pressed ? onGamepadButtonDown : onGamepadButtonUp, target)
    this.gamepad = gamepad
    this.button = button
    this.pressed = pressed
    this.repeat = repeat
  }
}

export class KeyboardButtonEvent extends Event {
  keyboard = null
  scancode = -1
  pressed = false
  repeat = false

  constructor (target, keyboard, scancode, pressed, repeat) {
    super(pressed ? onKeyboardButtonDown : onKeyboardButtonUp, target)
    this.keyboard = keyboard
    this.scancode = scancode
    this.pressed = pressed
    this.repeat = repeat
  }
}

export class KeyEvent extends Event {
  device = null
  key = -1
  pressed = false
  repeat = false

  constructor (target, device, key, pressed, repeat) {
    super(pressed ? onKeyDown : onKeyUp, target)
    this.device = device
    this.key = key
    this.pressed = pressed
    this.repeat = repeat
  }
}

export class AnalogKeyEvent extends Event {
  device = null
  analogKey = 0
  value = 0

  constructor (target, device, analogKey, value) {
    super(onAnalogMotion, target)
    this.device = device
    this.analogKey = analogKey
    this.value = value
  }
}

export class FocusEvent extends Event {
  relatedTarget = null

  constructor ($type, target, relatedTarget) {
    super($type, target)
    this.relatedTarget = relatedTarget
  }
}
