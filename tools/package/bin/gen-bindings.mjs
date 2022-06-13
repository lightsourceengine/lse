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
import { BINDINGS_SCHEMA_FILE, resolveLsePackagePath } from '../lib/paths.mjs'
import { readFileSync } from 'fs'

const toNativeReturnType = (t) => {
  switch (t) {
    case 'int': return 'int32_t'
    case 'bool': return 'bool'
    case 'string': return 'lse_string*'
    case 'cstring': return 'const char*'
    case 'float': return 'float'
    case 'style_ref': return 'lse_style*'
    default: return ''
  }
}

const toNativeReturnBox = (t) => `lse_core_box_${t}`

const toUnboxArg = (t) => `lse_core_unbox_${t}`

const bindings = JSON5.parse(readFileSync(BINDINGS_SCHEMA_FILE, 'utf8'))

for (const nsValue of Object.values(bindings.namespace)) {
  for (const [fnName, fnSignature] of Object.entries(nsValue).filter(([key]) => !key.startsWith('$'))) {
    // cheesy regex to parse the function pseudo-signature
    // [0] - return value, [1] - function name, [rest] - args, if any
    const f = fnSignature.split(/[,() ]+/).filter(Boolean)
    // replace void return type with blank to make template code simpler
    const ret = f[0] === 'void' ? '' : f[0]
    const args = f.slice(2)
    const isInterface = !!nsValue.$interface
    let env
    let typeId = 'lse_none_type'

    // special case for passing napi_env to the bound C function.
    if (args[0] === 'env') {
      args.shift()
      env = true
    }

    // putting self at the beginning of the array makes the template code simpler
    if (nsValue.$self) {
      if (args[0] !== 'self') {
        args.unshift('self')
      }

      if (isInterface) {
        typeId = `${nsValue.$self}_interface_type`
      } else {
        typeId = `${nsValue.$self}_type`
      }
    }

    // expand the pseudo-types to C name counterparts. again, for simplifying template code
    nsValue[fnName] = {
      return: ret,
      nativeReturn: toNativeReturnType(ret),
      returnBox: toNativeReturnBox(ret),
      native: f[1],
      args: args.map(toUnboxArg),
      isInterface,
      typeId,
      env
    }
  }
}

await Promise.all([
  Template('lse_core_exports.c', resolveLsePackagePath('core', 'addon', 'src'), bindings)
])
