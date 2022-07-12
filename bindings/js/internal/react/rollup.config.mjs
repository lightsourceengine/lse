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

import { onwarn, autoExternal, commonjs, nodeEnv, resolve, runtimePackage } from '@internal/common/plugins'
import { PROJECT_ROOT } from '@internal/common/paths'
import { join, posix } from 'path'

const reactStandalone = (reactModule, outputFile) => ({
  input: join(PROJECT_ROOT, 'node_modules', 'react', 'cjs', `${reactModule}.production.min.js`),
  onwarn,
  output: {
    file: `runtime/${outputFile}.mjs`,
    format: 'esm',
    preferConst: true
  },
  external: ['react'],
  plugins: [
    autoExternal(),
    nodeEnv(),
    resolve(),
    commonjs(),
    runtimePackage({
      // XXX: copy uses globby. src is not a filename, but a globby pattern. globby does not support windows paths.
      src: posix.join('..', '..', '..', '..', 'node_modules', 'react', 'package.json'),
      fields: {
        type: 'module',
        exports: {
          '.': './index.mjs',
          './jsx-runtime': './jsx-runtime.mjs'
        }
      }
    })
  ]
})

export default [
  reactStandalone('react', 'index'),
  reactStandalone('react-jsx-runtime', 'jsx-runtime')
]
