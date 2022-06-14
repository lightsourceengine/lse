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

import { autoExternal, beautify, minify, onwarn, resolve, runtimePackage } from '@internal/common/plugins'
import { injectVersion, lseVersion } from '@internal/common/version'

const lseReact = ({ input, output, plugins }) => ({
  input,
  onwarn,
  output: {
    file: output,
    format: 'esm',
    preferConst: true
  },
  external: ['react', 'react/jsx-runtime', '@lse/core', '@lse/style'],
  plugins: [
    autoExternal(),
    resolve(),
    injectVersion(),
    ...plugins
  ]
})

export default [
  // dist
  lseReact({
    input: 'src/exports.mjs',
    output: 'dist/lse-react.mjs',
    plugins: [
      beautify()
    ]
  }),
  // runtime
  lseReact({
    input: 'src/exports.mjs',
    output: 'runtime/lse-react.mjs',
    plugins: [
      minify(),
      runtimePackage({
        fields: {
          version: lseVersion,
          exports: {
            '.': './lse-react.mjs'
          }
        }
      })
    ]
  })
]
