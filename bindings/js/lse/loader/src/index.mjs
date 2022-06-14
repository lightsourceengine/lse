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
import { pathToFileURL } from 'url'

/**
 * Light Source Engine Runtime (lse) module loader.
 *
 * lse has the concept of an intrinsic package. Intrinsic packages function like builtin packages, but they
 * are not baked into the node or veil executable. Intrinsic packages are known to the system, so they are
 * not quite global packages. From an app perspective, developers can treat intrinsics as builtins. Intrinsics are
 * named differently to distinguish them from other package types.
 *
 * The known intrinsics packages are react and @lse namespaced packages.
 *
 * @ignore
 */

const intrinsics = (() => {
  const intrinsicFileURL = (...args) => {
    const { platform, execPath } = process
    let node = dirname(execPath)

    if (platform !== 'win32') {
      node = dirname(node)
    }

    return pathToFileURL(join(node, '..', 'intrinsic', ...args)).href
  }

  // TODO: duplicates information in package.json:exports for these packages (find a better way to get this info)
  return {
    '@lse/core': intrinsicFileURL('@lse', 'core', 'lse-core.mjs'),
    '@lse/style': intrinsicFileURL('@lse', 'style', 'lse-style.mjs'),
    '@lse/react': intrinsicFileURL('@lse', 'react', 'lse-react.mjs'),
    react: intrinsicFileURL('react', 'react.mjs'),
    'react/jsx-runtime': intrinsicFileURL('react', 'react-jsx-runtime.mjs')
  }
})()

export const resolve = async (specifier, context, defaultResolve) => {
  if (specifier in intrinsics) {
    return {
      url: intrinsics[specifier],
      format: 'module'
    }
  } else {
    return defaultResolve(specifier, context, defaultResolve)
  }
}
