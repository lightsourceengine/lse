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

import { fileURLToPath, pathToFileURL } from 'url'
import { basename, extname, isAbsolute, join, dirname } from 'path'
import { transformAsync, loadOptionsAsync } from '@babel/core'

/**
 * Light Source Engine Loader with Babel Support
 *
 * The loader is not a general purpose babel loader. The loader assumes transpilation from module format to
 * module format. The primary use case is converting JSX to JS equivalent code. Any needs outside of module ->
 * module transforms should use a different loader.
 *
 * The developer provides the babel configuration. The developer is responsible for ensuring that their presets and
 * plugins are limited to module -> module transformations.
 *
 * The loader is not intended to be used with Light Source Engine Runtime (lse). The transpilation process with babel
 * is expensive in terms of memory and cpu, even on high-end systems. It will hurt the user experience. Apps
 * requiring transpilation should do that step at package time.
 *
 * @ignore
 */

const babelConfigFilenames = new Set([
  '.babelrc.js',
  '.babelrc.mjs',
  'babel.config.js',
  'babel.config.mjs'
])

export const resolve = async (specifier, context, defaultResolve) => {
  if (extname(specifier) === '.jsx') {
    let url

    if (specifier.startsWith('file:')) {
      url = specifier
    } else if (isAbsolute(specifier)) {
      url = pathToFileURL(specifier).href
    } else if (specifier.startsWith('.')) {
      let parentPath

      if (context.parentURL) {
        parentPath = dirname(fileURLToPath(context.parentURL))
      } else {
        parentPath = process.cwd()
      }

      url = pathToFileURL(join(parentPath, specifier)).href
    } else {
      throw Error(`Could not resolve specifier: '${specifier}'`)
    }

    return {
      url,
      format: 'module'
    }
  }

  return defaultResolve(specifier, context, defaultResolve)
}

export const load = async (url, context, defaultLoad) => {
  const result = await defaultLoad(url, context, defaultLoad)
  const { source, format } = result

  // ignore non-module code
  if (!source || format !== 'module') {
    return result
  }

  const filename = fileURLToPath(url)

  // do not transpile the babel config files
  if (babelConfigFilenames.has(basename(filename))) {
    return result
  }

  // load the configuration for this file
  const options = await loadOptionsAsync({
    sourceType: format || 'module',
    // XXX: should the project root be configurable (env var)?
    root: process.cwd(),
    rootMode: 'root',
    filename,
    configFile: true
  })

  // if no plugins exist, skip an expensive no-op transpilation.
  //
  // for a general purpose loader, this check is not a safe assumption. since the loader is scoped to module -> module
  // in a NodeJS environment, transpilation can be safely skipped.
  //
  // the config files allow file filtering, which appears to happen in loadOptionsAsync. however, if a file is excluded,
  // options are still returned. other than presets and plugins being empty, there is no other indication that the
  // file should be excluded.
  // if (!options || (!options.presets.length && !options.plugins.length)) {
  //   return result
  // }

  // transpile
  const transformed = await transformAsync(source, {
    ...options,
    filename,
    ast: false,
    sourceMaps: false
  })

  if (!transformed || !transformed.code) {
    console.log(`not transpiled: ${filename}`)
  }

  return {
    source: transformed?.code ?? source,
    format: 'module'
  }
}
