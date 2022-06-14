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

import { emptyObject } from './emptyObject.mjs'
import { waypoint } from '@lse/core'
import { assign } from '@lse/style'

const sym = Symbol.for

const kCommonElementProps = [
  'styleClass',
  'focusable',
  'hidden'
]

// TODO: use EventSymbols from @lse/core
const kCommonEventListeners = [
  ['onFocus', sym('onfocus')],
  ['onFocusIn', sym('onfocusin')],
  ['onFocusOut', sym('onfocusout')],
  ['onBlur', sym('onblur')],
  ['onKeyUp', sym('onkeyup')],
  ['onKeyDown', sym('onkeydown')]
]

/**
 * Base class for React elements.
 *
 * <p>React elements are specialized components that provide the React reconciler with a way to communicate
 * with a rendering system. React elements appear in jsx with first letter lowercase names, such as &lt;box/&gt;.</p>
 *
 * @memberof module:@lse/react
 * @hideconstructor
 */
class Element {
  constructor (node, props) {
    node || throwBadNodeArg()
    props || throwBadPropsArg()

    this.node = node
    this.updateProps(emptyObject, props)
  }

  /**
   * Notify element of a property change.
   *
   * @param {Object} oldProps
   * @param {Object} newProps
   */
  updateProps (oldProps, newProps) {
    const { node } = this
    const { style } = newProps

    if (oldProps.style !== style) {
      // clear all style properties before assignment, otherwise style will have properties set from the previous state.
      assign(node.style.reset(), style)
    }

    for (const name of kCommonElementProps) {
      if (oldProps[name] !== newProps[name]) {
        node[name] = newProps[name]
      }
    }

    for (const [name, event] of kCommonEventListeners) {
      if (oldProps[name] !== newProps[name]) {
        oldProps[name] && node.off(event, oldProps[name])
        newProps[name] && node.on(event, newProps[name])
      }
    }

    if (oldProps.waypoint !== newProps.waypoint) {
      this.node.waypoint = typeof newProps.waypoint === 'string' ? waypoint(newProps.waypoint) : newProps.waypoint
    }

    this.props = newProps
  }

  /**
   * Append a child element.
   *
   * @param {Element} child
   */
  appendChild (child) {
    this.node.appendChild(child.node)
  }

  /**
   * Insert a child element before another child element.
   *
   * @param {Element} child
   * @param {Element} before
   */
  insertBefore (child, before) {
    this.node.insertBefore(child.node, before.node)
  }

  /**
   * Remove a child element.
   *
   * @param {Element} child
   */
  removeChild (child) {
    child.node.destroy()
  }
}

const throwBadNodeArg = () => { throw Error('Invalid node arg passed to Element constructor.') }

const throwBadPropsArg = () => { throw Error('Invalid props arg passed to Element constructor.') }

export { Element }
