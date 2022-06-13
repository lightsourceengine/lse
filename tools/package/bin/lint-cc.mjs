#!/usr/bin/env node

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

import { spawnClangFormat } from 'clang-format'
import glob from 'glob'
import { PROJECT_ROOT } from '@internal/common/paths'

// TODO: move to package.json?
const patterns = [
  'external/stb/src/*.[hc]',
  'external/STC/src/*.[hc]',
  'external/nanosvg/include/*.[hc]',
  'external/nanosvg/src/*.[hc]',
  'lib/src/*.[hc]',
  'lib/inc/*.[hc]',
  'test/src/*.[hc]',
  'bindings/js/lse/core/addon/src/*.[hc]'
]
const args = [
  // accept --fix and --verbose to have arg compatibility with the js linter
  process.argv.includes('--fix') ? '-i' : '-n',
  process.argv.includes('--verbose') ? '--verbose' : '',
  // must treat warnings as error, or the spawn will be successful for incorrectly formatted files
  '--Werror'
]
const done = (err) => err ? process.exit(1) : console.log('Done.')
let files = []

process.chdir(PROJECT_ROOT)

console.log('> lint-cc')
patterns.forEach(pattern => { files = files.concat(glob.sync(pattern)) })
console.log(`Checking ${files.length} files.`)

spawnClangFormat([...args.filter(Boolean), ...files], done, 'inherit')
