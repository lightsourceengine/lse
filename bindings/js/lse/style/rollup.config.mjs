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

import { autoExternal, babelRuntime, beautify, minify, onwarn, resolve, runtimePackage } from '@internal/common/plugins'
import { injectVersion, lseVersion } from '@internal/common/version'

const lseStyle = ({ input, output, plugins }) => ({
  input,
  onwarn,
  output: {
    file: output,
    format: 'esm',
    preferConst: true
  },
  external: ['@lse/core'],
  plugins: [
    autoExternal(),
    resolve(),
    injectVersion(),
    ...plugins
  ]
})

export default [
  lseStyle({
    input: 'src/exports.mjs',
    output: 'dist/index.mjs',
    plugins: [
      beautify()
    ]
  }),
  lseStyle({
    input: 'src/exports.mjs',
    output: 'runtime/index.mjs',
    plugins: [
      babelRuntime(),
      minify(),
      runtimePackage({
        fields: {
          version: lseVersion,
          exports: {
            '.': './index.mjs'
          }
        }
      })
    ]
  })
]
