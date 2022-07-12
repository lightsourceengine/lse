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

/**
 * lse runtime module loader
 *
 * In the runtime environment, @lse and react packages are bundled with the runtime, located at
 * $PACKAGE/share/intrinsic. In node development environments, @lse and react packages are loaded from
 * node_modules. For the runtime environment, this loader functions as a browser import map, mapping @lse
 * and react specifiers to the intrinsic package directory.
 *
 * The term intrinsic is used because the term builtin is used for node packages baked in to, such as node:url. The
 * primary reason for intrinsics is so node and veil executables can be interchangeable in the lse package.
 *
 * @ignore
 */

/*
 * Resolve Note
 *
 * import.meta.url: file:///$PACKAGE/share/intrinsic/@lse/loader/index.mjs
 *
 * For URL, ../.. will bring us to intrinsic. The index.mjs is ignored. If this was a regular path, ../../..
 * would get us to intrinsic.
 */
const base = new URL('../..', import.meta.url)
const asFileUrl = (fragment) => new URL(fragment, base).href

/*
 * Import Map
 *
 * Inspired by browser import maps, but we map to a resolved { url, format } object. The url points to the
 * resolved entry point in the 'intrinsic' directory of the package. The resolved object bypasses package.json
 * exports, type, etc and the associated I/O calls to get that information.
 */
const importMap = {
  '@lse/core': { url: asFileUrl('@lse/core/index.mjs'), format: 'module' },
  '@lse/style': { url: asFileUrl('@lse/style/index.mjs'), format: 'module' },
  '@lse/react': { url: asFileUrl('@lse/react/index.mjs'), format: 'module' },
  // TODO: react needs to change back to commonjs (veil does not have a commonjs loader yet)
  react: { url: asFileUrl('react/index.mjs'), format: 'module' },
  'react/jsx-runtime': { url: asFileUrl('react/jsx-runtime.mjs'), format: 'module' }
}

export const resolve = async (specifier, context, defaultResolve) =>
  importMap[specifier] ?? defaultResolve(specifier, context, defaultResolve)
