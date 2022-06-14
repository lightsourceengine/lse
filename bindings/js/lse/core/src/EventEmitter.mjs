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

/**
 * EventEmitter
 *
 * - Removes the overhead of the builtin EventEmitter.
 * - Supports Symbol and string event names.
 * - Dispatch works the same as node's builtin EventEmitter.
 *
 * @ignore
 */
export class EventEmitter {
  /**
   * Dispatch an event.
   *
   * @param id {string|symbol} The event type.
   * @param args {...*} Arguments to be forwarded to listeners.
   */
  emit (id, ...args) {
    const sym = typeof id === 'symbol' ? id : toSymbol(id)
    const listeners = this[sym]

    if (!listeners) {
      return
    }

    const { length } = listeners
    let numRemovedDuringEmit = 0

    for (let i = 0; i < length; i++) {
      try {
        listeners[i]?.(...args)
      } catch (e) {
        // TODO: use logger
        console.error(`unhandled listener error: ${e.message}`, 'emit()')
      }

      if (!listeners[i]) {
        numRemovedDuringEmit++
      }
    }

    if (numRemovedDuringEmit) {
      if (numRemovedDuringEmit === listeners.length) {
        listeners.length = 0
      } else {
        this[sym] = listeners.filter(Boolean)
      }
    }
  }

  emitEvent (event) {
    this.emit(event.$type, event)
  }

  /**
   * Add a listener.
   *
   * @param {string|Symbol} id Event name to listen to
   * @param {function} listener Listener to register. This will be called back with an Event object argument
   * @throws Error when id is invalid (not a string, Symbol or unregistered) or listener is not a function
   */
  on (id, listener) {
    const sym = typeof id === 'symbol' ? id : toSymbol(id)
    const clistener = listener
    const self = this

    typeof listener === 'function' || throwExpectedFunction(id);

    (self[sym] ?? (self[sym] = [])).push(listener)

    return () => self.off(sym, clistener)
  }

  /**
   * Remove a listener.
   *
   * If the listener arg is not a function or a function not registered with this emitter, the call is a no-op.
   *
   * @param {string|Symbol} id Event name to listen to
   * @param {function} listener Listener to unregister
   */
  off (id, listener) {
    const listeners = this[typeof id === 'symbol' ? id : toSymbol(id)]
    let i = listeners?.length ?? 0

    while (--i) {
      if (listeners[i] === listener) {
        listeners[i] = null
      }
    }
  }

  /**
   * Checks if any listeners are registered for the given event type.
   *
   * @param id {string|symbol} The event type.
   * @returns {boolean} true if more than 0 registered listeners; otherwise, false
   */
  hasListeners (id) {
    return !!(this[typeof id === 'symbol' ? id : toSymbol(id)]?.length)
  }
}

const toSymbol = (value) => (typeof value === 'string') ? Symbol.for(value) : throwExpectedSymbolOrString()
const throwExpectedSymbolOrString = arg => { throw Error(`Expected event type to be a symbol or string. Got ${arg}`) }
const throwExpectedFunction = arg => { throw Error(`Expected listener to be a Function. Got ${arg}`) }
