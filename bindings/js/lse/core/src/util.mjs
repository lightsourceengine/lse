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

import { performance } from 'perf_hooks'

export const emptyArray = Object.freeze([])

export const setFlag = (flags, bit, value) => value ? (flags | (1 << bit)) : (flags & ~(1 << bit))

export const getFlag = (flags, bit) => (flags & (1 << bit)) !== 0

export const isPlainObject = (obj) => (typeof obj === 'object' && obj?.constructor === Object)

export const clamp = (val, min, max) => val > max ? max : val < min ? min : val

export const invertMap = (map) => new Map(Array.from(map, entry => entry.reverse()))

export const { now } = performance

export const arrayAtPolyfill = () => {
  if (!Array.prototype.at) {
    // eslint-disable-next-line no-extend-native
    Object.defineProperty(Array.prototype, 'at', {
      value: function (n) {
        n = n >>> 0
        n < 0 && (n += this.length)
        if (n >= 0 && n < this.length) return this[n]
      },
      writable: true,
      enumerable: false,
      configurable: true
    })
  }
}

export const illegalArgumentError = (name, value) => {
  throw Error(`illegal argument: ${name}: ${value}`)
}
