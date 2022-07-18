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

import cl from 'command-line-args'
import {
  installEntryPoint,
  parallel,
  installGameControllerDb,
  installIntrinsicModules,
  installSDL,
  installNode,
  installMeta,
  compile,
  createPackage,
  createContext,
  stripBinary,
  compressBinary
} from '../lib/create-runtime-lib.mjs'

const runtimeType = rt => {
  if (!['node', 'veil'].includes(rt)) {
    throw Error(`Error: invalid --js-runtime value of '${rt}'. expected 'node' or 'veil'`)
  }

  return rt
}

const CL_ARG_DEFS = [
  { name: 'recipe', type: String, required: true },
  { name: 'skip-compile', type: Boolean },
  { name: 'preserve-staging', type: Boolean },
  { name: 'js-runtime', type: runtimeType, defaultValue: 'node' },
  { name: 'download-cache', type: String, defaultValue: '' }
]

const main = async () => {
  const ctx = await createContext(cl(CL_ARG_DEFS, { camelCase: true }))

  await parallel(
    installEntryPoint(ctx)
      .then(() => installMeta(ctx))
      .then(() => ctx.status.entryPoint.succeed()),
    installGameControllerDb(ctx)
      .then(() => ctx.status.assets.succeed()),
    installSDL(ctx)
      .then(() => ctx.status.lib.succeed()),
    installNode(ctx)
      .then(() => stripBinary(ctx, ctx.status.node, 'node'))
      .then(() => compressBinary(ctx, ctx.status.node, 'node'))
      .then(() => ctx.status.node.succeed()),
    compile(ctx)
      .then(() => installIntrinsicModules(ctx))
      .then(() => stripBinary(ctx, ctx.status.compile, 'core'))
      .then(() => compressBinary(ctx, ctx.status.compile, 'core'))
      .then(() => ctx.status.compile.succeed())
  )

  await createPackage(ctx).then(() => ctx.status.package.succeed())
}

await main()
