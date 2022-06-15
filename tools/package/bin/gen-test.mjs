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

import { Template } from '../lib/Template.mjs'
import { PROJECT_ROOT } from '@internal/common/paths'
import { readdirSync, readFileSync } from 'fs'
import { join } from 'path'

const testPath = (...args) => join(PROJECT_ROOT, 'test', ...args)
const tests = []
const files = readdirSync(testPath('src')).filter(name => name.match(/^test_\w+\.c$/))

for (const file of files) {
  const contents = readFileSync(testPath('src', file), 'utf8')
  const test = {
    testCases: []
  }

  contents.split(/\r?\n/).forEach(line => {
    const parts = line.split(/[,() ]+/).filter(Boolean)

    switch (parts[0]) {
      case 'TEST_CASE':
        test.testCases.push('test_' + parts[1])
        break
      case 'BEFORE_EACH':
        test.beforeEach = parts[1] + '_before_each'
        break
      case 'AFTER_EACH':
        test.afterEach = parts[1] + '_after_each'
        break
      default:
        break
    }
  })

  if (!test.testCases.length) {
    console.error(`${file} contains no tests!`)
    process.exit(1)
  }

  tests.push(test)
}

await Template('lse_test_runner_suite.c', testPath('src', 'runner'), { tests })
