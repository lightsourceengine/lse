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

import { Direction } from './Direction.mjs'

const { UP, DOWN, LEFT, RIGHT } = Direction

const horizontalTag = 'horizontal'
const verticalTag = 'vertical'

const HORIZONTAL = 1
const VERTICAL = 2
const OFFSET = [
  [],
  [0, -1, 1, 0, 0],
  [0, 0, 0, -1, 1]
]

export class FixedListWaypoint {
  constructor (tag) {
    if (tag === horizontalTag) {
      this._navigation = HORIZONTAL
      this.tag = horizontalTag
    } else if (tag === verticalTag) {
      this._navigation = VERTICAL
      this.tag = verticalTag
    } else {
      throw Error(`FixedListWaypoint expected tag ${horizontalTag} or ${verticalTag}. Got ${tag}`)
    }

    this._focalPathIndex = -1
  }

  navigate (context) {
    const { owner, direction } = context
    const offset = OFFSET[this._navigation][direction]

    if (!offset) {
      return context.pass()
    }

    if (this._focalPathIndex === -1) {
      this.sync(owner)
    }

    const nextFocalPathIndex = this._focalPathIndex + offset
    const focalPath = createFocalPath(owner)

    if (nextFocalPathIndex < 0) {
      this._focalPathIndex = 0
      return context.defer(focalPath[0])
    } else if (nextFocalPathIndex >= focalPath.length) {
      this._focalPathIndex = focalPath.length - 1
      return context.defer(focalPath[focalPath.length - 1])
    } else {
      this._focalPathIndex = nextFocalPathIndex
      return context.move(focalPath[nextFocalPathIndex])
    }
  }

  resolve (context) {
    const { owner, pending, direction } = context
    let focalPathIndex = this._focalPathIndex
    const navigation = this._navigation
    const focalPath = createFocalPath(owner)

    if (pending && !isDescendent(pending, owner)) {
      switch (direction) {
        case DOWN:
          if (navigation === VERTICAL) {
            focalPathIndex = 0
          }
          break
        case UP:
          if (navigation === VERTICAL) {
            focalPathIndex = focalPath.length - 1
          }
          break
        case RIGHT:
          if (navigation === HORIZONTAL) {
            focalPathIndex = 0
          }
          break
        case LEFT:
          if (navigation === HORIZONTAL) {
            focalPathIndex = focalPath.length - 1
          }
          break
        default:
          throw Error(`Unknown direction: ${direction}`)
      }
    }

    // TODO: out of range

    if (focalPathIndex === -1) {
      focalPathIndex = 0
    }

    return focalPath[(this._focalPathIndex = focalPathIndex)]
  }

  sync (owner) {
    let i = 0

    for (const node of createFocalPath(owner)) {
      if (hasFocusRecursive(node)) {
        this._focalPathIndex = i
        return
      }
      i++
    }

    // this._focalPathIndex = -1
  }
}

const hasFocusRecursive = (node) => {
  return node.hasFocus() || node.childNodes.some(n => hasFocusRecursive(n))
}

const findFocusable = (node) => {
  if (node.waypoint || node.focusable) {
    return node
  }

  let result

  for (const child of node.childNodes) {
    if ((result = findFocusable(child))) {
      return result
    }
  }

  return null
}

const createFocalPath = (node) => {
  const path = []
  let result

  for (const child of node.childNodes) {
    (result = findFocusable(child)) && path.push(result)
  }

  if (!path.length) {
    throw Error('Waypoint expects to be associated with a node with focusable children.')
  }

  return path
}

const isDescendent = (a, b) => {
  let walker = a

  while (walker) {
    if (b === walker.parent) {
      return true
    }

    walker = walker.parent
  }

  return false
}
