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

// TODO: get enums from native layer
export const LSE_RESOURCE_STATE_INIT = 0
export const LSE_RESOURCE_STATE_LOADING = 1
export const LSE_RESOURCE_STATE_READY = 2
export const LSE_RESOURCE_STATE_ERROR = 3
export const LSE_RESOURCE_STATE_DETACHED = 4
export const LSE_RESOURCE_STATE_DONE = 5

export const toString = (state) => {
  switch (state) {
    case LSE_RESOURCE_STATE_INIT:
      return 'init'
    case LSE_RESOURCE_STATE_LOADING:
      return 'loading'
    case LSE_RESOURCE_STATE_READY:
      return 'ready'
    case LSE_RESOURCE_STATE_ERROR:
      return 'error'
    case LSE_RESOURCE_STATE_DETACHED:
      return 'detached'
    case LSE_RESOURCE_STATE_DONE:
      return 'done'
    default:
      return 'unknown'
  }
}
