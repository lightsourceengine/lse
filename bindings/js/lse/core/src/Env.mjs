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
import { Display } from './Display.mjs'
import { EventEmitter } from './EventEmitter.mjs'
import { $EventAfterConfigure, $EventBeforeConfigure, $EventBeforeDestroy, $EventAfterDestroy } from './EventSymbols.mjs'
import { isPlainObject, illegalArgumentError } from './util.mjs'
import { performance } from 'perf_hooks'

const { now } = performance

const {
  $configure,
  $destroy,
  $update,
  $getVideoDrivers,
  $getDisplayCount,
  $getRenderers,
  $newInstance,
  $addFont,
  $isConfigured,
  $isError,
  $isDestroyed,
  $isRunning,
  $addWindow,
  $removeWindow
} = $env

export class Env extends EventEmitter {
  #mainLoopId = null
  #fonts = []
  #primaryWindow = null

  static new () {
    return $newInstance()
  }

  configure (config = {}) {
    // TODO: destroyed?
    if (this.isConfigured()) {
      throw Error('init can only be called once')
    }

    const cleanConfig = {}

    for (const key in config) {
      if (typeof config[key] === 'boolean') {
        cleanConfig[key] = { enabled: config[key] }
      } else {
        cleanConfig[key] = config[key]
      }
    }

    this.emitEvent({ $type: $EventBeforeConfigure, env: this })

    $configure(this, config)

    this.emitEvent({ $type: $EventAfterConfigure, env: this })

    let lastTick = now()

    this.#mainLoopId = setInterval(() => {
      // TODO: check init
      // TODO: reset windows

      const tick = now()

      this.#primaryWindow.$onFrame(tick, lastTick)

      lastTick = tick

      if (!$update(this)) {
        this.destroy()
      }
    }, 1000 / 60)
  }

  destroy () {
    clearInterval(this.#mainLoopId)
    this.#mainLoopId = null

    this.emitEvent({ $type: $EventBeforeDestroy, env: this })

    if (this.#primaryWindow) {
      this.#primaryWindow.$notifyBeforeDestroy()

      try {
        $removeWindow(this, this.#primaryWindow)
      } catch (e) {
        // TODO: log error?
      }

      this.#primaryWindow?.$notifyAfterDestroy()
      this.#primaryWindow = null
    }

    this.#fonts = []

    try {
      $destroy(this)
    } catch (e) {
      // TODO: log error?
    }

    this.emitEvent({ $type: $EventAfterDestroy, env: this })
  }

  get videoDrivers () {
    return $getVideoDrivers(this) || []
  }

  get renderers () {
    return $getRenderers(this) || []
  }

  get displays () {
    const count = $getDisplayCount(this)
    const result = []

    for (let i = 0; i < count; i++) {
      result.push(new Display(this, i))
    }

    return result
  }

  get fonts () {
    return [...this.#fonts]
  }

  addFont (spec) {
    // TODO: check env state -> return null if shutdown

    let font
    const { uri, index, family, style, weight } = (isPlainObject(spec) ? spec : { uri: spec })

    typeof uri === 'string' || illegalArgumentError('uri', uri)
    typeof family === 'string' || illegalArgumentError('family', family)

    this.#fonts.push(font = $addFont(this, uri, index, family, style, weight))

    return font
  }

  isConfigured () {
    return $isConfigured(this)
  }

  isRunning () {
    return $isRunning(this)
  }

  // TODO: necessary?
  isError () {
    return $isError(this)
  }

  isDestroyed () {
    return $isDestroyed(this)
  }

  $addWindow () {
    if (this.#primaryWindow) {
      throw Error('only one window per env')
    }

    return (this.#primaryWindow = $addWindow(this))
  }

  $getPrimaryWindow () {
    return this.#primaryWindow
  }
}
