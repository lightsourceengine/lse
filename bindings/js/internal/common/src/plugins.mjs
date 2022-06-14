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

import { terser } from 'rollup-plugin-terser'
import replace from 'rollup-plugin-re'
import babel from '@rollup/plugin-babel'
import resolve from '@rollup/plugin-node-resolve'
import autoExternal from 'rollup-plugin-auto-external'
import copy from 'rollup-plugin-copy'
import commonjs from '@rollup/plugin-commonjs'
import alias from '@rollup/plugin-alias'

// forward plugins so leaf config files have less importing to do
export { alias, autoExternal, babel, commonjs, replace, resolve }

// es module output template for a rollup config
export const esm = (file) => ({
  file,
  format: 'esm',
  preferConst: true
})

// rollup warning helper: always error on warnings
export const onwarn = (warning, warn) => {
  warn(warning)
  throw Error(warning.message)
}

// rollup plugin: format source
export const beautify = () => terser({
  compress: false,
  mangle: false,
  ecma: 2020,
  output: {
    quote_style: 1,
    semicolons: false,
    beautify: true,
    indent_level: 2
    // preamble: preamble.replace("${version}", getPublishingVersion())
  }
})

// rollup plugin: minify source
export const minify = (options = {}) => terser({
  mangle: {
    module: true,
    reserved: options.reserved,
    // property name mangling does not produce working code!
    properties: false
  },
  // Disable compress, as the defaults (inlining in particular) cause performance problems in v8.
  compress: false,
  output: {
    quote_style: 1,
    semicolons: true
    // preamble
  },
  ecma: 2020,
  module: true
})

// Replaces process.env.NODE_ENV with 'production' string.
export const nodeEnv = () => replace({
  sourceMap: false,
  replaces: {
    'process.env.NODE_ENV': JSON.stringify('production')
  }
})

export const runtimePackage = ({ src = 'package.json', dest = 'runtime', fields, once = true }) => {
  const transform = (contents, filename) => {
    const pkg = JSON.parse(contents.toString())
    const rt = {
      name: pkg.name,
      type: pkg.type,
      version: pkg.version,
      ...fields
    }
    return JSON.stringify(rt, null, 2)
  }

  return copy({
    copyOnce: !!once,
    targets: [
      { src, dest, transform }
    ]
  })
}
