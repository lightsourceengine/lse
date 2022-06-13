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

import { join } from 'path'
import { PROJECT_ROOT, LSE_PACKAGE_ROOT, INTERNAL_PACKAGE_ROOT, NATIVE_ROOT } from '@internal/common/paths'

export const STYLE_SCHEMA_FILE = join(PROJECT_ROOT, 'tools', 'package', 'db', 'style.json5')
export const BINDINGS_SCHEMA_FILE = join(PROJECT_ROOT, 'tools', 'package', 'db', 'bindings.json5')

export const resolveNativePath = (...args) => join(NATIVE_ROOT, ...args)

export const resolveLsePackagePath = (...args) => {
  return args.length ? join(LSE_PACKAGE_ROOT, ...args) : LSE_PACKAGE_ROOT
}

export const resolveInternalPackagePath = (...args) => {
  return args.length ? join(INTERNAL_PACKAGE_ROOT, ...args) : INTERNAL_PACKAGE_ROOT
}

export const resolveTemplatePath = (template) => join(PROJECT_ROOT, 'tools', 'package', 'template', template + '.template')
