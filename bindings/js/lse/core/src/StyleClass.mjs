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

import { Style } from './Style.mjs'

export const FOCUS_WITHIN_PSEUDO_CLASS_NAME = ':focus-within'
export const FOCUS_WITHIN_PSEUDO_CLASS_ID = Symbol.for(FOCUS_WITHIN_PSEUDO_CLASS_NAME)

export const FOCUS_WITHOUT_PSEUDO_CLASS_NAME = ':focus-without'
export const FOCUS_WITHOUT_PSEUDO_CLASS_ID = Symbol.for(FOCUS_WITHOUT_PSEUDO_CLASS_NAME)

export class StyleClass extends Style {
  [FOCUS_WITHIN_PSEUDO_CLASS_ID] = null;
  [FOCUS_WITHOUT_PSEUDO_CLASS_ID] = null
}
