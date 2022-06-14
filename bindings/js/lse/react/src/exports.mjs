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

import { createReconciler } from './reconciler.mjs'
import { env, window } from '@lse/core'
import { RootElement } from './RootElement.mjs'
import { Element } from './Element.mjs'
import { jsx } from 'react/jsx-runtime'

const LegacyRoot = 0 // from react-reconciler (constant is not exported)
let reconciler = createReconciler()
let rootElement = new RootElement(window.root)
// concurrent mode was introduced in react 18. need to do more research into how to configure concurrent mode on the
// lse side. set the container to legacy mode (synchronous rendering).
let container = reconciler.createContainer(rootElement, LegacyRoot, false, true)

export const render = (...args) => {
  const { root } = window
  let node
  let component
  let callback

  // NOTE: configuration is not strictly necessary to proceed. however, all unconfigured use cases have not been
  //       tested. for now, force the user to call letThereBeLight().
  if (!env.isConfigured()) {
    throw Error('env must be configured before rendering react components')
  }

  switch (args.length) {
    case 1:
      node = root
      component = args[0]
      break
    case 2:
      if (args[0] === window || args[0] === root) {
        node = root
        component = args[1]
      } else {
        node = root
        component = args[0]
        callback = args[1]
      }
      break
    case 3:
      node = (args[0] === window || args[0] === root) ? root : null
      component = args[1]
      callback = args[2]
      break
    default:
      break
  }

  if (!node) {
    throw Error('node must be window or window.root')
  }

  if (callback && typeof callback !== 'function') {
    throw Error('callback must be a function')
  }

  if (typeof component === 'function') {
    component = jsx(component, {})
  }

  // TODO: in 17, unbatchedUpdates was recommended by the docs. lse runs in legacy mode, so flushSync() looks like
  //       the replacement. it 'works', but i need to dive into the react source code to see if this is correct.
  reconciler.flushSync(() => reconciler.updateContainer(component, container, null, callback))
}

export const findNode = (node, component) => {
  let element

  if (component instanceof Element) {
    element = component
  } else if (component) {
    element = reconciler.findHostInstance(component)
  }

  return element?.node ?? null
}

export const version = $LSE_VERSION

window.on('before-destroy', (e) => {
  if (reconciler) {
    render(rootElement.node, null, () => { rootElement = null; container = null })
  }
})

env.on('after-destroy', (e) => {
  if (reconciler) {
    rootElement = null
    reconciler = null
    container = null

    // in past react releases, react-reconciler and scheduler would leave timers open via MessageChannel ports
    // and setTimeout. this has been fixed. now, it appears to take a couple of cycles of the node loop for
    // react to release it's timer refs. need to do some more research into react to see what is going on. lse
    // may need to do a process.exit() to close the executable faster.
  }
})
