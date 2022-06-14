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

import { arrayAtPolyfill } from './util.mjs'
import { nsglobal } from './addon.mjs'
import { Env } from './Env.mjs'
import { Window } from './Window.mjs'
import { StyleFilter } from './StyleFilter.mjs'
import { StyleInstance } from './StyleInstance.mjs'
import { StyleTransform } from './StyleTransform.mjs'
import { StyleValue } from './StyleValue.mjs'
import { RootNode } from './RootNode.mjs'
import { BoxNode } from './BoxNode.mjs'
import { ImageNode } from './ImageNode.mjs'
import { TextNode } from './TextNode.mjs'
import { DisplayMode } from './DisplayMode.mjs'
import { RendererInfo } from './RendererInfo.mjs'
import { Gamepad } from './Gamepad.mjs'
import { Keyboard } from './Keyboard.mjs'
import { Image } from './Image.mjs'
import { Font } from './Font.mjs'

const { bind } = nsglobal

const errorHandler = ({ message, stack }) => {
  message && console.log(message)
  stack && console.log(stack)

  process.exit(1)
}

process.on('uncaughtException', errorHandler)
process.on('unhandledRejection', errorHandler)

arrayAtPolyfill()

bind('lse_env', Env)
bind('lse_window', Window)
bind('lse_style', StyleInstance)
bind('lse_style_value', StyleValue)
bind('lse_root_node', RootNode)
bind('lse_box_node', BoxNode)
bind('lse_image_node', ImageNode)
bind('lse_text_node', TextNode)
bind('lse_display_mode', DisplayMode)
bind('lse_renderer_info', RendererInfo)
bind('lse_gamepad', Gamepad)
bind('lse_keyboard', Keyboard)
bind('lse_image', Image)
bind('lse_font', Font)

bind('lse_style_filter', StyleFilter)
bind('lse_style_transform', StyleTransform)
bind('lse_style_value', StyleValue)
