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

import './bootstrap.mjs'

import { Env } from './Env.mjs'
import { StyleOps } from './StyleOps.mjs'
import { Input } from './Input.mjs'
import { StyleFilter } from './StyleFilter.mjs'
import { StyleTransform } from './StyleTransform.mjs'
import { StyleValue } from './StyleValue.mjs'

export { Key } from './Key.mjs'
export { Hat } from './Hat.mjs'
export { AnalogKey } from './AnalogKey.mjs'
export { Scancode } from './Scancode.mjs'
export { waypoint } from './waypoint.mjs'

const terminate = () => {
  // TODO: notify env of exit
  process.exit(0)
}

process.on('SIGINT', terminate)
process.on('SIGTERM', terminate)

export const env = Env.new()
export const input = new Input(env)
export const window = env.$addWindow()
export const version = $LSE_VERSION

export const letThereBeLight = (config = {}) => {
  // TODO: this is sufficient for demos, but more work needs to be done with configuration options
  env.configure(config)
  window.configure(config.window)
}

// export basic API for support packages
export const __internal = {
  Style: {
    assign: StyleOps.assign,
    StyleClass: StyleOps.StyleClass,
    StyleSheet: StyleOps.StyleSheet,
    StyleFilter,
    StyleTransform,
    StyleValue
  }
}
