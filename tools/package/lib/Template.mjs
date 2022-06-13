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

import Handlebars from 'handlebars'
import { createReadStream } from 'fs'
import { createInterface } from 'readline'
import { readFile, writeFile, stat } from 'fs/promises'
import { join } from 'path'
import { EOL } from 'os'
import { resolveTemplatePath } from './paths.mjs'

const fileOptions = { encoding: 'utf8' }
const camelToSnakeCase = str => str.replace(/[A-Z]/g, letter => `_${letter.toLowerCase()}`).toUpperCase()
const undash = str => str.replaceAll('-', '_')

export const Template = async (base, target, data) => {
  const contents = await readFile(resolveTemplatePath(base), fileOptions)
  const template = Handlebars.compile(contents, { noEscape: true })
  const targetStat = await stat(target)

  if (targetStat.isDirectory()) {
    await writeFile(join(target, base), template(data), fileOptions)
  } else if (targetStat.isFile()) {
    const targetFile = join(target)
    const rl = createInterface({
      input: createReadStream(targetFile),
      crlfDelay: Infinity
    })
    const begin = []
    const end = []
    let lines = begin

    for await (const line of rl) {
      lines?.push(line)

      if (line.startsWith('// BEGIN AUTO-GENERATED SECTION')) {
        lines = null
      } else if (line.startsWith('// END AUTO-GENERATED SECTION')) {
        lines = end
        lines.push(line)
      }
    }

    const replacement = begin.join(EOL) + EOL + template(data) + EOL + end.join(EOL) + EOL

    await writeFile(targetFile, replacement, fileOptions)
  } else {
    throw Error('unknown')
  }
}

Handlebars.registerHelper({
  toEnumName: (str) => 'lse_style_' + undash(str),
  toEnumValue: (name, value) => undash(`LSE_STYLE_${name}_${value}`).toUpperCase(),
  toPropertyEnumValue: (str) => 'LSE_SP_' + camelToSnakeCase(str),
  toPropertyTypeEnum: (str) => 'LSE_STYLE_PROPERTY_TYPE_' + str.toUpperCase(),
  toUnitEnum: (str) => 'LSE_STYLE_UNIT_' + str.toUpperCase(),
  toUpperCase: (str) => str.toUpperCase(),
  toBoolean: (value) => (!!value).toString(),
  countKeys: (value) => Object.keys(value).length.toString(),
  undash: (str) => undash(str),
  if_eq: function (a, b, opts) {
    if (a === b) {
      return opts.fn(this)
    } else {
      return opts.inverse(this)
    }
  },
  isFunctionName: function (name) {
    return !name.startsWith('$')
  }
})
