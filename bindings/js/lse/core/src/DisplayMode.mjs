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

export class DisplayMode {
  width = 0
  height = 0
  refreshRate = 0

  constructor (width, height, refreshRate) {
    this.width = width ?? 0
    this.height = height ?? 0
    this.refreshRate = refreshRate ?? 0
  }

  toString () {
    return `${this.width}x${this.height}@${this.refreshRate}hz`
  }
}
