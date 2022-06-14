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

import Reconciler from '@internal/react-reconciler'
import { TextElement } from './TextElement.mjs'
import { BoxElement } from './BoxElement.mjs'
import { ImageElement } from './ImageElement.mjs'
import { performance } from 'perf_hooks'

const kElementNameToElementClass = {
  box: BoxElement,
  img: ImageElement,
  text: TextElement
}

const errorUnknownElementType = (type) => {
  throw Error(`Invalid react element type '${type}'.`)
}

export const createReconciler = () => {
  // TODO: may not be in sync with latest 18.1.0 reconciler api (new focus api, new async rendering, etc)
  return Reconciler({
    now: performance.now,
    supportsMutation: true,
    supportsHydration: false,
    supportsPersistence: false,
    isPrimaryRenderer: true,

    createInstance (type, props, rootContainerInstance, _currentHostContext, workInProgress) {
      const ElementClass = kElementNameToElementClass[type] || errorUnknownElementType(type)

      return new ElementClass(rootContainerInstance.node.window.createNode(type), props)
    },

    createTextInstance (text, rootContainerInstance, internalInstanceHandle) {
      return text
    },

    finalizeInitialChildren (element, type, props, rootContainerInstance, hostContext) {
      // return true will force commitMount to be called on this element where autoFocus is applied
      return !!props.autoFocus
    },

    getPublicInstance (inst) {
      return inst
    },

    prepareForCommit (containerInfo) {
      return null
    },

    prepareUpdate (wordElement, type, oldProps, newProps) {
      return true
    },

    resetAfterCommit (containerInfo) {
      // noop
    },

    resetTextContent (wordElement) {
      // noop
    },

    getRootHostContext (instance) {
      return instance
    },

    getChildHostContext (instance) {
      return instance
    },

    shouldSetTextContent (type, props) {
      return type === 'text'
    },

    appendChild (parentInstance, child) {
      parentInstance.appendChild(child)
    },

    appendInitialChild (parentInstance, child) {
      parentInstance.appendChild(child)
    },

    appendChildToContainer (parentInstance, child) {
      parentInstance.appendChild(child)
    },

    removeChild (parentInstance, child) {
      parentInstance.removeChild(child)
    },

    removeChildFromContainer (parentInstance, child) {
      parentInstance.removeChild(child)
    },

    insertBefore (parentInstance, child, beforeChild) {
      parentInstance.insertBefore(child, beforeChild)
    },

    insertInContainerBefore (parentInstance, child, beforeChild) {
      parentInstance.insertBefore(child, beforeChild)
    },

    commitUpdate (instance, updatePayload, type, oldProps, newProps) {
      if (instance.updateProps && oldProps !== newProps) {
        instance.updateProps(oldProps, newProps)
      }
    },

    // note: only called if finalizeInitialChildren returns true
    commitMount (instance, type, newProps) {
      if (newProps.autoFocus) {
        instance.node.focus()
      }
    },

    commitTextUpdate (textInstance, oldText, newText) {
      // noop
    },

    shouldDeprioritizeSubtree (type, nextProps) {
      // noop
    },

    clearContainer (instance) {
      instance.node.clear()
    },

    detachDeletedInstance (instance) {
      instance.node.destroy()
    },

    supportsMicrotasks: true,
    scheduleMicrotask: queueMicrotask
  })
}
