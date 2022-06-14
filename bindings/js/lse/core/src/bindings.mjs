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

import { join, normalize } from 'path'
import { createRequire } from 'module'

const require = createRequire(import.meta.url)

/*
 * Optimized bindings implementation for the standalone lse runtime.
 *
 * The bindings npm package works, but performs a lot of IO. In standalone mode, lse-core.node will be in the
 * builtin package directory. Or, the user can supply an absolute path to lse-core.node using the LSE_CORE_PATH
 * environment variable (for development use cases). The optimization is simple: try to load from the env; otherwise
 * load from the builtin package path. Done.
 *
 * The script is only included in standalone runtimes. Given that assumption, the builtin package directory can be
 * located using the node execPath. Also, this script will only run in the @lse/core package.
 */
const bindings = (opts) => {
  // if absolute paths are provided (via LSE_CORE_PATH), try to load them first
  if (opts.try?.length) {
    for (const t of opts.try) {
      const path = normalize(join(...t.map(value => value === 'bindings' ? opts.bindings : value)))
      try {
        return require(path)
      } catch (e) {
        console.warn(`bindings: '${e.message}' - ${path}`)
      }
    }
  }

  const { env, execPath, platform } = process
  let buildType = env.LSE_BUILD_TYPE?.toLowerCase()
  let lseHome

  if (platform === 'win32') {
    // lse_package/node/node.exe -> lse_package
    lseHome = join(execPath, '..', '..')
  } else {
    // lse_package/node/bin/node -> lse_package
    lseHome = join(execPath, '..', '..', '..')
  }

  lseHome = normalize(lseHome)

  if (buildType !== 'release' || buildType !== 'debug') {
    buildType = 'release'
  }

  return require(join(lseHome, 'intrinsic', '@lse', 'core', buildType, opts.bindings))
}

export default bindings
