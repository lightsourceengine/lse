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

import { join, dirname } from 'path'
import { createRequire } from 'module'
import { fileURLToPath } from 'url'
import { emptyArray } from './util.mjs'

const { url } = import.meta
const require = createRequire(url)
const lseCorePath = dirname(fileURLToPath(url))

/*
 * Optimized bindings implementation for the standalone lse runtime.
 *
 * The bindings npm package works, but performs a lot of file I/O. In standalone mode, this function will be in
 * $PACKAGE/share/intrinsic/@lse/core/index.mjs. The file path can be retrieved from import.meta.url. The @lse/core
 * addon is located at $PACKAGE/share/intrinsic/@lse/core/$BUILD_TYPE/lse-core.node. The environment can specify
 * a build type of 'release' or 'debug' via LSE_BUILD_TYPE env variable. In summary, lse-core.node is resolved from
 * import.meta.url in standalone mode.
 *
 * bindings try paths are supported. The main use case is for processing LSE_CORE_PATH, an environment variable
 * with a custom path for lse-core.node. The caller needs to build these paths.
 */
const bindings = (opts) => {
  // if try paths are provided, try to load from them first.
  for (const t of opts.try ?? emptyArray) {
    if (t.at(-1) === 'bindings') {
      t.splice(-1, 1, opts.bindings)
    }

    const tryPath = join(...t)

    try {
      return require(tryPath)
    } catch (e) {
      console.warn(`bindings: '${e.message}' - ${tryPath}`)
    }
  }

  const buildType = (process.env.LSE_BUILD_TYPE ?? 'release').toLowerCase()

  if (buildType !== 'release' && buildType !== 'debug') {
    throw Error(`Unsupported bindings build type: ${buildType}`)
  }

  return require(join(lseCorePath, buildType, opts.bindings))
}

export default bindings
