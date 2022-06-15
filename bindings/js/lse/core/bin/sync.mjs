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

import { PROJECT_ROOT } from '@internal/common/paths'
import { join, dirname, basename } from 'path'
import fse, { copy, ensureDir, emptyDir } from 'fs-extra'

/*
 * sync
 *
 * @lse/core needs the native lse and external source code to build the addon with gyp. @lse/core
 * is several layers deep in the source tree. gyp does not like to reference code outside the
 * package directory. Specifically, it does not set the path to the .o file correctly, causing the
 * .o files to be stored outside the build directory. The addon still gets built, but the .o files
 * are in inconvenient places in source root. And, node-gyp clean will not remove them.
 *
 * In addition, source code needs to be copied to the addon package when the @lse/core is bundled
 * for npm.
 *
 * This script will copy external and lse source code into the @lse/core package under external. This
 * is not checked in. That means this sync needs to be run before @lse/core is built via gyp. This step
 * only pertains to developers working on lse who are not using CMake.
 */

const { readJson } = fse
const external = join(PROJECT_ROOT, 'external')
const lse = join(PROJECT_ROOT, 'lib')
const coreExternal = join(PROJECT_ROOT, 'bindings', 'js', 'lse', 'core', 'external')

const copySource = async (gypi, dest) => {
  const from = dirname(gypi)
  const build = await readJson(gypi)
  const target = build.targets[0]

  await ensureDir(dest)
  await copy(gypi, join(dest, basename(gypi)))

  for (const dir of target.include_dirs) {
    const copyTo = join(dest, dir)

    await ensureDir(copyTo)
    await copy(join(from, dir), copyTo)
  }

  if (target['lse:source_dirs']) {
    for (const sourceDir of target['lse:source_dirs']) {
      const copyTo = join(dest, sourceDir)

      await ensureDir(copyTo)
      await copy(join(from, sourceDir), copyTo)
    }
  } else {
    for (const source of target.sources) {
      const copyTo = join(dest, source)

      await ensureDir(dirname(copyTo))
      await copy(join(from, source), copyTo)
    }
  }

  let licenses = target['lse:license']

  if (licenses) {
    if (!Array.isArray(licenses)) {
      licenses = [licenses]
    }

    for (const license of licenses) {
      await copy(join(from, license), join(dest, basename(license)))
    }
  }
}

const copyDir = async (dir, dest) => {
  await ensureDir(dest)
  await copy(dir, dest)
}

await emptyDir(coreExternal)

const copies = [
  copySource(join(external, 'freetype', 'build.gypi'), join(coreExternal, 'freetype')),
  copySource(join(external, 'nanosvg', 'build.gypi'), join(coreExternal, 'nanosvg')),
  copySource(join(external, 'stb', 'build.gypi'), join(coreExternal, 'stb')),
  copySource(join(external, 'STC', 'build.gypi'), join(coreExternal, 'STC')),
  copySource(join(external, 'yoga', 'build.gypi'), join(coreExternal, 'yoga')),
  copySource(join(lse, 'build.gypi'), join(coreExternal, 'lse')),
  copyDir(join(external, 'SDL2'), join(coreExternal, 'SDL2'))
]

await Promise.all(copies)
