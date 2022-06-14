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

import { onwarn, commonjs, resolve, nodeEnv } from '@internal/common/plugins'

const intro = `import { performance } from 'perf_hooks'`

export default [
  /*
   * react-reconciler and scheduler tend to contain environment specific global references. In past releases, dealing
   * with these specific global references got painful with the node, veil or lse environments. The hacks, shims and
   * polyfills are addressed here so @lse/react can import a workable react-reconciler (via @lse/reconciler).
   *
   * Fortunately, the patches have been reduced to injecting 'performance' from node 'perf_hooks' and bundling
   * react-reconciler + scheduler into a single file. I would prefer to have this bundling here, rather than adding
   * complexity to @lse/react bundling.
   */
  {
    input: 'react-reconciler',
    onwarn,
    output: {
      file: 'dist/reconciler.mjs',
      format: 'esm',
      preferConst: true,
      interop: false,
      intro
    },
    external: ['react'],
    plugins: [
      nodeEnv(),
      resolve(),
      commonjs({
        defaultIsModuleExports: true
      })
    ]
  }
]
