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

import bindings from 'bindings'
import { dirname } from 'path'

const LSE_CORE_ADDON = 'lse-core.node'

const generateTryPaths = () => {
  let { LSE_CORE_PATH } = process.env

  if (!LSE_CORE_PATH) {
    return
  }

  if (LSE_CORE_PATH.endsWith(LSE_CORE_ADDON)) {
    LSE_CORE_PATH = dirname(LSE_CORE_PATH)
  }

  return [
    [LSE_CORE_PATH, 'bindings']
  ]
}

export const loadAddon = () => {
  const opts = {
    bindings: LSE_CORE_ADDON,
    try: generateTryPaths()
  }

  try {
    return bindings(opts)
  } catch (e) {
    console.error(`Failed to load ${opts.bindings}: ${e.message}`)
    process.exit(1)
  }
}

export const {
  $env,
  nsfont,
  nsgamepad,
  nsimage,
  nskeyboard,
  nsglobal,
  nsnode,
  $style,
  $window
} = loadAddon()
