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

import { $env } from './addon.mjs'
import { $EventAfterConfigure, $EventBeforeDestroy } from './EventSymbols.mjs'
import { EventEmitter } from './EventEmitter.mjs'
import {
  AnalogKeyEvent,
  GamepadEvent,
  GamepadAxisEvent,
  GamepadHatEvent,
  KeyEvent,
  GamepadButtonEvent,
  KeyboardButtonEvent
} from './Event.mjs'
import { now } from './util.mjs'
import { navigate } from './navigate.mjs'
import { Direction } from './Direction.mjs'
import { Key } from './Key.mjs'
import { Mapping } from './Mapping.mjs'
import { Scancode } from './Scancode.mjs'

const { $registerEventCallbacks, $resetEventCallbacks, $getKeyboard, $getGamepads, $getMapping, $setMapping, $resetMapping } = $env

const keyspaceToMotionEvent = [
  /* LSE_KEYSPACE_MAPPED   = 0 */ AnalogKeyEvent,
  /* LSE_KEYSPACE_HARDWARE = 1 */ GamepadAxisEvent
]

const keyspaceToButtonEvent = [
  /* LSE_KEYSPACE_MAPPED   = 0 */ KeyEvent,
  /* LSE_KEYSPACE_HARDWARE = 1 */ GamepadButtonEvent
]

export class Input extends EventEmitter {
  #env = null
  #gamepadsById = new Map()
  #keyboard = null
  #unlisten = null
  #lastActivity = now()
  #keyboardMapping = null
  #navigationMapping = null

  constructor (env) {
    super()
    this.#env = env

    this.#unlisten = [
      env.on($EventAfterConfigure, this.#onInit.bind(this)),
      env.on($EventBeforeDestroy, this.#onShutdown.bind(this))
    ]

    this.#resetNavigationMapping()
  }

  get gamepads () {
    return [...this.#gamepadsById.values()]
  }

  get keyboard () {
    return this.#keyboard
  }

  setMapping (mapping) {
    if (!(mapping instanceof Mapping)) {
      throw Error('expected Mapping instance')
    }

    if (mapping.uuid === this.#keyboard?.uuid) {
      this.#keyboardMapping = mapping
    } else {
      $setMapping(this.#env, mapping.uuid, mapping.toCsv())
    }
  }

  getMapping (uuid) {
    if (uuid === this.#keyboard?.uuid) {
      return this.#keyboardMapping
    } else {
      const csv = $getMapping(this.#env, uuid)

      return csv ? Mapping.fromCsv(csv) : undefined
    }
  }

  resetMapping (uuid) {
    if (uuid === this.#keyboard?.uuid) {
      this.#keyboardMapping = getDefaultKeyboardMapping()
    } else {
      $resetMapping(this.#env, uuid)
    }
  }

  #onInit () {
    const env = this.#env

    $registerEventCallbacks(env, {
      gamepad_status: this.#onGamepadStatus.bind(this),
      gamepad_button: this.#onGamepadButton.bind(this),
      keyboard_button: this.#onKeyboardButton.bind(this),
      gamepad_axis_motion: this.#onGamepadAxisMotion.bind(this),
      gamepad_hat_motion: this.#onGamepadHatMotion.bind(this)
    })

    this.#keyboard = $getKeyboard(env)

    this.resetMapping(this.#keyboard.uuid)

    $getGamepads(env).forEach(gamepad => {
      this.#gamepadsById.set(gamepad.id, gamepad)
      this.emitEvent(new GamepadEvent(this, gamepad, true))
    })
  }

  #onShutdown () {
    $resetEventCallbacks(this.#env)

    this.#unlisten.forEach(unlistener => unlistener())
    this.#unlisten = null
    this.#gamepadsById.clear()
    this.#keyboard = null
    this.#keyboardMapping = null
    this.#env = null
  }

  #onGamepadStatus (gamepad, connected) {
    if (connected) {
      this.#gamepadsById.set(gamepad.id, gamepad)
    } else {
      this.#gamepadsById.delete(gamepad.id)
    }

    this.emitEvent(new GamepadEvent(this, gamepad, connected))
  }

  #onKeyboardButton (keyspace, button, pressed, repeat) {
    this.#lastActivity = now()
    const { keyboard } = this

    this.#bubble(new KeyboardButtonEvent(this, keyboard, button, pressed, repeat))

    // TODO: stop prop

    const key = this.#keyboardMapping.getKeyForButton(button)

    if (key === -1) {
      return
    }

    const keyEvent = new KeyEvent(this, keyboard, key, pressed, repeat)

    if (pressed) {
      navigate(this.#env.$getPrimaryWindow().activeNode, this.#navigationMapping.get(key) ?? Direction.NONE, keyEvent)
    }

    // TODO: stop prop

    this.#bubble(keyEvent)
  }

  #onGamepadButton (id, keyspace, button, pressed, repeat) {
    this.#lastActivity = now()
    const gamepad = this.#gamepadsById.get(id)
    const ButtonEvent = keyspaceToButtonEvent[keyspace]

    // button up / down

    this.#bubble(new ButtonEvent(this, gamepad, button, pressed, repeat))
  }

  #onGamepadAxisMotion (id, keyspace, axis, value) {
    this.#lastActivity = now()
    const device = this.#gamepadsById.get(id)
    const MotionEvent = keyspaceToMotionEvent[keyspace]

    this.#bubble(new MotionEvent(this, device, axis, value))
  }

  #onGamepadHatMotion (id, keyspace, hat, value) {
    this.#lastActivity = now()
    const gamepad = this.#gamepadsById.get(id)

    this.#bubble(new GamepadHatEvent(this, gamepad, hat, value))
  }

  #bubble (event) {
    this.emitEvent(event)

    if (event.hasStopPropagation()) {
      return
    }

    const { activeNode } = this.#env.$getPrimaryWindow()

    if (activeNode) {
      event.target = activeNode
      activeNode.$bubble(event)
    }
  }

  #resetNavigationMapping () {
    this.#navigationMapping = new Map([
      [Key.DPAD_UP, Direction.UP],
      [Key.DPAD_RIGHT, Direction.RIGHT],
      [Key.DPAD_DOWN, Direction.DOWN],
      [Key.DPAD_LEFT, Direction.LEFT]
    ])
  }
}

const getDefaultKeyboardMapping = () => Mapping.forKeyboard([
  [Key.A, Scancode.Z],
  [Key.B, Scancode.X],
  [Key.X, Scancode.A],
  [Key.Y, Scancode.S],
  [Key.START, Scancode.RETURN],
  [Key.BACK, Scancode.RSHIFT],
  [Key.LEFT_SHOULDER, Scancode.Q],
  [Key.RIGHT_SHOULDER, Scancode.W],
  [Key.DPAD_UP, Scancode.UP],
  [Key.DPAD_DOWN, Scancode.DOWN],
  [Key.DPAD_LEFT, Scancode.LEFT],
  [Key.DPAD_RIGHT, Scancode.RIGHT]
])
