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

const toSymbol = Symbol.for

export const $EventBeforeDestroy = toSymbol('before-destroy')
export const $EventAfterDestroy = toSymbol('after-destroy')

export const $EventBeforeConfigure = toSymbol('before-configure')
export const $EventAfterConfigure = toSymbol('after-configure')

export const onGamepadConnected = toSymbol('ongamepadconnected')
export const onGamepadDisconnected = toSymbol('ongamepaddisconnected')

export const onGamepadButtonDown = toSymbol('ongamepadbuttondown')
export const onGamepadButtonUp = toSymbol('ongamepadbuttonup')
export const onGamepadAxis = toSymbol('ongamepadaxis')
export const onGamepadHat = toSymbol('ongamepadhat')

export const onKeyboardButtonDown = toSymbol('onkeyboardbuttondown')
export const onKeyboardButtonUp = toSymbol('onkeyboardbuttonup')

export const onKeyDown = toSymbol('onkeydown')
export const onKeyUp = toSymbol('onkeyup')
export const onAnalogMotion = toSymbol('onanalogmotion')

export const onFocus = toSymbol('onfocus')
export const onFocusIn = toSymbol('onfocusin')
export const onFocusOut = toSymbol('onfocusout')
export const onBlur = toSymbol('onblur')
