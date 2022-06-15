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

import { join, normalize } from 'path'
import { fileURLToPath } from 'url'

export const PROJECT_ROOT = normalize(join(fileURLToPath(import.meta.url), '..', '..', '..', '..', '..', '..'))
export const CONFIG_ROOT = join(PROJECT_ROOT, 'config')
export const LSE_PACKAGE_ROOT = join(PROJECT_ROOT, 'bindings', 'js', 'lse')
export const INTERNAL_PACKAGE_ROOT = join(PROJECT_ROOT, 'bindings', 'js', 'internal')
export const NATIVE_ROOT = join(PROJECT_ROOT, 'lib')
