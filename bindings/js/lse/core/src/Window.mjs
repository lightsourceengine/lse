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

import { $window } from './addon.mjs'
import { StyleOps } from './StyleOps.mjs'
import { EventEmitter } from './EventEmitter.mjs'
import { $EventAfterDestroy, $EventBeforeDestroy } from './EventSymbols.mjs'
import { isPlainObject, illegalArgumentError } from './util.mjs'

const { $getRoot, $configure, $setTitle, $getWidth, $getHeight, $getRefreshRate, $createNode, $addImage } = $window

export class Window extends EventEmitter {
  #title = ''
  #root = null
  #activeNode = null
  #rafCurrent = []
  #rafNextFrame = []

  $postWrap () {
    this.#root = $getRoot(this)
  }

  configure (settings = {}) {
    const { style, title } = settings

    $configure(this, settings)

    style && StyleOps.assign(this.root.style, style)
    typeof title === 'string' && (this.title = title)
  }

  createNode (tag) {
    return $createNode(this, tag)
  }

  addImage (spec) {
    if (typeof spec === 'string') {
      spec = { uri: spec }
    }

    if (!isPlainObject(spec)) {
      illegalArgumentError('spec', spec)
    }

    return $addImage(this, spec.uri)
  }

  get activeNode () {
    return this.#activeNode
  }

  get root () {
    return this.#root
  }

  get title () {
    return this.#title
  }

  set title (value) {
    $setTitle(this, value ?? '')
    this.#title = value
  }

  get width () {
    return $getWidth(this)
  }

  get height () {
    return $getHeight(this)
  }

  get refreshRate () {
    return $getRefreshRate(this)
  }

  get fullscreen () {
    return false
  }

  get fullscreenMode () {
    return ''
  }

  get vsync () {
    return false
  }

  requestAnimationFrame (callback) {
    this.#rafNextFrame.push(callback)

    return this.#rafNextFrame.at(-1)
  }

  cancelAnimationFrame (requestId) {
    // just null out the listeners, as it is cheaper than a remove. the arrays will be cleaned up at the
    // end of each frame.

    // cancel a scheduled request
    requestId < this.#rafNextFrame.length && (this.#rafNextFrame[requestId] = null)

    // cancel a request in the currently processing list.
    // use cases: 1) remove self in callback 2) remove a raf callback that has not processed this frame
    requestId < this.#rafCurrent.length && (this.#rafCurrent[requestId] = null)
  }

  $setActiveNode (node) {
    this.#activeNode = node
  }

  $notifyBeforeDestroy () {
    this.emitEvent({ $type: $EventBeforeDestroy, window: this })
  }

  $notifyAfterDestroy () {
    this.emitEvent({ $type: $EventAfterDestroy, window: this })
    this.#activeNode = null
    this.#rafNextFrame.length = 0
    this.#rafCurrent.length = 0
    this.#root.destroy()
  }

  $onFrame (tick, lastTick) {
    if (this.#rafNextFrame.length) {
      // it's the next frame, so swap current and nextFrame and notify the listeners
      const next = this.#rafCurrent

      this.#rafCurrent = this.#rafNextFrame
      this.#rafNextFrame = next

      for (const listener of this.#rafCurrent) {
        try {
          listener?.(tick, lastTick)
          // TODO: need to handle the case when this callback destroys the window
        } catch (e) {
          // TODO: exception in user callback.. log?
        }
      }

      this.#rafCurrent.length = 0
    }
  }
}
