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

import { nsnode } from './addon.mjs'
import { emptyArray, getFlag, setFlag } from './util.mjs'
import { EventEmitter } from './EventEmitter.mjs'
import { FocusEvent } from './Event.mjs'
import { onBlur, onFocus, onFocusIn, onFocusOut } from './EventSymbols.mjs'
import { FOCUS_WITHIN_PSEUDO_CLASS_ID, FOCUS_WITHOUT_PSEUDO_CLASS_ID } from './StyleClass.mjs'

const { appendChild, insertBefore, removeChild, getStyle, destroy, getX, getY, getWidth, getHeight } = nsnode

const kFlagHidden = 1
const kFlagFocusable = 2

export class Node extends EventEmitter {
  waypoint = null

  #children = emptyArray
  #style = null
  #styleClass = null
  #parent = null
  #flags = 0
  #window = null

  constructor (window) {
    super()
    this.#window = window
    this.isLeaf() || (this.#children = [])
  }

  appendChild (child) {
    if (appendChild(this, child)) {
      this.#children.push(child)
      child.#parent = this
    }
  }

  insertBefore (child, before) {
    const children = this.#children

    if (insertBefore(this, child, before)) {
      children.splice(children.indexOf(before), 0, child)
      child.#parent = this
    }
  }

  removeChild (child) {
    const children = this.#children
    const { activeNode } = this.#window

    if (!removeChild(this, child)) {
      return null
    }

    children.splice(children.indexOf(child), 1)
    child.#parent = null

    if (activeNode) {
      if (activeNode === child) {
        child.blur()
      } else {
        let walker = activeNode.#parent

        while (walker) {
          if (walker === child) {
            child.blur()
            break
          }
          walker = walker.#parent
        }
      }
    }

    return child
  }

  destroy () {
    if (this.isDestroyed()) {
      return
    }

    let child

    while ((child = this.#children[0])) {
      child.destroy()
    }

    if (this.#parent) {
      this.#parent.removeChild(this)
      this.#parent = null
    }

    destroy(this)

    this.#window = null
    this.#style = null
  }

  get childNodes () {
    return this.#children
  }

  get firstChild () {
    return this.#children[0] ?? null
  }

  get lastChild () {
    return this.#children.at(-1) ?? null
  }

  get window () {
    return this.#window
  }

  hasChildNodes () {
    return !!this.#children.length
  }

  isLeaf () {
    return false
  }

  isDestroyed () {
    return !this.#window
  }

  clear () {
    let child

    while ((child = this.#children[0])) {
      child.destroy()
    }
  }

  focus () {
    if (!this.focusable || !this.#isConnectedToRoot() || this.hasFocus()) {
      return
    }

    const window = this.#window
    const old = window.activeNode

    window.$setActiveNode(this)

    // remove the :focus-within pseudo class. if one was not added, this is a no-op
    old?.#clearPseudoClassRecursive()

    // dispatch the :focus-within and :focus-without pseudo classes
    // this is a trivial implementation of pseudo classes that will not scale. a better design will be needed
    // to add more pseudos.
    this.#dispatchFocusWithin()
    this.#dispatchFocusWithout()

    old?.#clearFocusWithin(this)

    if (old) {
      old.emitEvent(new FocusEvent(onBlur, old, this))
      old.$bubble(new FocusEvent(onFocusOut, old, this))
    }

    this.emitEvent(new FocusEvent(onFocus, this, old))
    this.$bubble(new FocusEvent(onFocusIn, this, old))
  }

  blur () {
    const window = this.#window

    if (this === window.activeNode) {
      window.$setActiveNode(null)

      // remove the :focus-within pseudo class.
      this.#clearPseudoClassRecursive()

      this.emitEvent(new FocusEvent(onBlur, this, null))
      this.$bubble(new FocusEvent(onFocusOut, this, null))
    }
  }

  hasFocus () {
    return this === this.#window.activeNode
  }

  get parent () {
    return this.#parent
  }

  get style () {
    return this.#style ?? (this.#style = getStyle(this))
  }

  get styleClass () {
    return this.#styleClass
  }

  set styleClass (styleClass) {
    if (this.#hasParentFocus()) {
      this.style.$setParent(styleClass[FOCUS_WITHOUT_PSEUDO_CLASS_ID])
    } else if (this.hasFocus() || this.#window.activeNode?.#hasParentFocusOf(this)) {
      this.style.$setParent(styleClass[FOCUS_WITHIN_PSEUDO_CLASS_ID])
    } else {
      this.#styleClass = this.style.$setParent(styleClass)
    }
  }

  // note: equivalent to offsetLeft, offsetTop, offsetWidth, offsetHeight

  get x () {
    return getX(this)
  }

  get y () {
    return getY(this)
  }

  get width () {
    return getWidth(this)
  }

  get height () {
    return getHeight(this)
  }

  get hidden () {
    return getFlag(this.#flags, kFlagHidden)
  }

  set hidden (isHidden) {
    this.#flags = setFlag(this.#flags, kFlagHidden, isHidden)
  }

  get focusable () {
    return getFlag(this.#flags, kFlagFocusable)
  }

  set focusable (isFocusable) {
    this.#flags = setFlag(this.#flags, kFlagFocusable, isFocusable)
  }

  $bubble (event) {
    let walker = this

    while (walker) {
      walker.emitEvent(event)

      if (event.hasStopPropagation()) {
        return
      }

      walker = walker.#parent
    }
  }

  #isConnectedToRoot () {
    let walker = this
    const root = this.#window.root

    while (walker) {
      if (walker === root) {
        return true
      }

      walker = walker.#parent
    }

    return false
  }

  #dispatchFocusWithin () {
    let walker = this

    while (walker) {
      const focusWithin = walker.#styleClass?.[FOCUS_WITHIN_PSEUDO_CLASS_ID]

      focusWithin && walker.#style?.$setParent(focusWithin)

      walker = walker.#parent
    }
  }

  #dispatchFocusWithout () {
    this.#children.forEach(child => {
      const focusWithout = child.#styleClass?.[FOCUS_WITHOUT_PSEUDO_CLASS_ID]
      focusWithout && child.#style?.$setParent(focusWithout)
      child.#dispatchFocusWithout()
    })
  }

  #clearPseudoClassRecursive () {
    this.#children.forEach(child => child.#clearPseudoClassRecursive())
    this.#style?.$setParent(this.#styleClass)
  }

  #hasParentFocus () {
    let { parent } = this

    while (parent) {
      if (parent.hasFocus()) {
        return true
      }
      parent = parent.#parent
    }

    return false
  }

  #hasParentFocusOf (of) {
    let { parent } = this

    while (parent) {
      if (parent === of) {
        return parent.hasFocus()
      }
      parent = parent.#parent
    }
  }

  #clearFocusWithin (focused) {
    let walker = focused
    const ancestors = []

    while (walker) {
      ancestors.push(walker)
      walker = walker.#parent
    }

    walker = this

    while (walker) {
      if (!ancestors.includes(walker)) {
        walker.#style?.$setParent(walker.#styleClass)
      }
      walker = walker.#parent
    }
  }
}
