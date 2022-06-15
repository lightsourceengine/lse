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
import JSON5 from 'json5'
import { resolveLsePackagePath, resolveNativePath, STYLE_SCHEMA_FILE } from '../lib/paths.mjs'
import { readFileSync } from 'fs'

let offset = 0
const styleMetadata = JSON5.parse(readFileSync(STYLE_SCHEMA_FILE, 'utf8'))

for (const [key, values] of Object.entries(styleMetadata.types)) {
  if (Array.isArray(values)) {
    styleMetadata.types[key] = { values }
  }
}

Object.entries(styleMetadata.properties).forEach(([key, prop]) => {
  if (!prop.type) {
    prop.type = 'number'
  } else if (prop.type in styleMetadata.types) {
    prop.enumType = prop.type
    prop.type = 'enum'
  }

  // ensure layout is set and is a boolean
  prop.layout = !!prop.layout

  const values = prop.enumValues = styleMetadata.types[prop.enumType]?.values ?? []

  // fill in bit stuff
  if (values.length > 0) {
    prop.enumBitWidth = Math.ceil(Math.log2(values.length))
    prop.enumOffset = offset
    offset += prop.enumBitWidth
  }
})

await Promise.all([
  Template('Style.mjs', resolveLsePackagePath('core', 'src'), styleMetadata),
  Template('lse_style_meta.c', resolveNativePath('src'), styleMetadata),
  Template('style-enums', resolveNativePath('include', 'lse.h'), styleMetadata)
])
