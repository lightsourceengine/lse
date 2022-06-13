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

#pragma once

#include "lse.h"
#include <SDL.h>

// 255.255.255 + null terminator
#define LSE_VERSION_STRING_SIZE 12

typedef struct lse_library lse_library;
typedef struct lse_sdl lse_sdl;

struct lse_library {
  char compiled_version[LSE_VERSION_STRING_SIZE];
  char linked_version[LSE_VERSION_STRING_SIZE];
  void *handle;
};

struct lse_sdl {
  // REQUIRED API
  // - all functions must be in sdl library
  // - these functions are available in at least version 2.0.4

  int(SDLCALL *SDL_Init)(Uint32);
  int(SDLCALL *SDL_InitSubSystem)(Uint32);
  void(SDLCALL *SDL_Quit)(void);
  void(SDLCALL *SDL_QuitSubSystem)(Uint32);
  Uint32(SDLCALL *SDL_WasInit)(Uint32);
  SDL_Texture *(SDLCALL *SDL_CreateTexture)(SDL_Renderer *, Uint32, int, int, int);
  void(SDLCALL *SDL_DestroyTexture)(SDL_Texture *);
  int(SDLCALL *SDL_SetTextureBlendMode)(SDL_Texture *, SDL_BlendMode);
  int(SDLCALL *SDL_SetTextureColorMod)(SDL_Texture *, Uint8, Uint8, Uint8);
  int(SDLCALL *SDL_SetTextureAlphaMod)(SDL_Texture *, Uint8);
  int(SDLCALL *SDL_LockTexture)(SDL_Texture *, const SDL_Rect *, void **, int *);
  void(SDLCALL *SDL_UnlockTexture)(SDL_Texture *);
  int(SDLCALL *SDL_QueryTexture)(SDL_Texture *, Uint32 *, int *, int *, int *);
  int(SDLCALL *SDL_UpdateTexture)(SDL_Texture *, const SDL_Rect *, const void *, int);
  const char *(SDLCALL *SDL_GetPixelFormatName)(Uint32 format);
  const char *(SDLCALL *SDL_GetVideoDriver)(int index);
  int(SDLCALL *SDL_GetNumVideoDrivers)(void);
  const char *(SDLCALL *SDL_GetCurrentVideoDriver)(void);
  int(SDLCALL *SDL_GetRenderDriverInfo)(int, SDL_RendererInfo *);
  int(SDLCALL *SDL_GetRendererInfo)(SDL_Renderer *, SDL_RendererInfo *);
  int(SDLCALL *SDL_GetNumRenderDrivers)(void);
  SDL_Renderer *(SDLCALL *SDL_CreateRenderer)(SDL_Window *, int, Uint32);
  void(SDLCALL *SDL_DestroyRenderer)(SDL_Renderer *);
  int(SDLCALL *SDL_GetRendererOutputSize)(SDL_Renderer *, int *, int *);
  int(SDLCALL *SDL_RenderCopy)(SDL_Renderer *, SDL_Texture *, const SDL_Rect *, const SDL_Rect *);
  int(SDLCALL *SDL_RenderCopyEx)(
      SDL_Renderer *,
      SDL_Texture *,
      const SDL_Rect *,
      const SDL_Rect *,
      const double,
      const SDL_Point *,
      const SDL_RendererFlip);
  int(SDLCALL *SDL_RenderFillRect)(SDL_Renderer *, const SDL_Rect *);
  int(SDLCALL *SDL_RenderFillRects)(SDL_Renderer *, const SDL_Rect *, int);
  int(SDLCALL *SDL_SetRenderDrawColor)(SDL_Renderer *, Uint8, Uint8, Uint8, Uint8);
  int(SDLCALL *SDL_SetRenderDrawBlendMode)(SDL_Renderer *, SDL_BlendMode);
  int(SDLCALL *SDL_SetRenderTarget)(SDL_Renderer *, SDL_Texture *);
  int(SDLCALL *SDL_RenderClear)(SDL_Renderer *);
  void(SDLCALL *SDL_RenderPresent)(SDL_Renderer *);
  int(SDLCALL *SDL_RenderSetClipRect)(SDL_Renderer *, const SDL_Rect *);
  int(SDLCALL *SDL_GetDesktopDisplayMode)(int, SDL_DisplayMode *);
  int(SDLCALL *SDL_GetCurrentDisplayMode)(int, SDL_DisplayMode *);
  const char *(SDLCALL *SDL_GetDisplayName)(int);
  int(SDLCALL *SDL_GetDisplayMode)(int, int, SDL_DisplayMode *);
  int(SDLCALL *SDL_GetNumDisplayModes)(int);
  int(SDLCALL *SDL_GetNumVideoDisplays)(void);
  void(SDLCALL *SDL_GetVersion)(SDL_version *);
  SDL_Joystick *(SDLCALL *SDL_JoystickOpen)(int);
  void(SDLCALL *SDL_JoystickClose)(SDL_Joystick *);
  const char *(SDLCALL *SDL_JoystickName)(SDL_Joystick *);
  SDL_JoystickID(SDLCALL *SDL_JoystickInstanceID)(SDL_Joystick *);
  Uint8(SDLCALL *SDL_JoystickGetButton)(SDL_Joystick *, int);
  int(SDLCALL *SDL_JoystickNumAxes)(SDL_Joystick *);
  int(SDLCALL *SDL_JoystickNumHats)(SDL_Joystick *);
  int(SDLCALL *SDL_JoystickNumButtons)(SDL_Joystick *);
  SDL_JoystickGUID(SDLCALL *SDL_JoystickGetGUID)(SDL_Joystick *);
  Uint8(SDLCALL *SDL_JoystickGetHat)(SDL_Joystick *, int);
  Sint16(SDLCALL *SDL_JoystickGetAxis)(SDL_Joystick *, int);
  int(SDLCALL *SDL_NumJoysticks)(void);
  void(SDLCALL *SDL_JoystickGetGUIDString)(SDL_JoystickGUID, char *, int);
  SDL_JoystickGUID(SDLCALL *SDL_JoystickGetGUIDFromString)(const char *);
  int(SDLCALL *SDL_GameControllerEventState)(int);
  int(SDLCALL *SDL_GameControllerAddMappingsFromRW)(SDL_RWops *, int);
  char *(SDLCALL *SDL_GameControllerMapping)(SDL_GameController *);
  char *(SDLCALL *SDL_GameControllerMappingForGUID)(SDL_JoystickGUID);
  SDL_bool(SDLCALL *SDL_IsGameController)(int);
  SDL_GameController *(SDLCALL *SDL_GameControllerOpen)(int);
  void(SDLCALL *SDL_GameControllerClose)(SDL_GameController *);
  SDL_Joystick *(SDLCALL *SDL_GameControllerGetJoystick)(SDL_GameController *);
  const char *(SDLCALL *SDL_GameControllerName)(SDL_GameController *);
  Sint16(SDLCALL *SDL_GameControllerGetAxis)(SDL_GameController *, SDL_GameControllerAxis);
  Uint8(SDLCALL *SDL_GameControllerGetButton)(SDL_GameController *, SDL_GameControllerButton);
  const Uint8 *(SDLCALL *SDL_GetKeyboardState)(int *);
  int(SDLCALL *SDL_PeepEvents)(SDL_Event *, int, SDL_eventaction, Uint32, Uint32);
  void(SDLCALL *SDL_PumpEvents)(void);
  SDL_Window *(SDLCALL *SDL_CreateWindow)(const char *, int, int, int, int, Uint32);
  void(SDLCALL *SDL_DestroyWindow)(SDL_Window *);
  void(SDLCALL *SDL_SetWindowTitle)(SDL_Window *, const char *);
  int(SDLCALL *SDL_GetWindowDisplayMode)(SDL_Window *, SDL_DisplayMode *);
  Uint32(SDLCALL *SDL_GetWindowFlags)(SDL_Window *);
  int(SDLCALL *SDL_ShowCursor)(int);
  SDL_RWops *(SDLCALL *SDL_RWFromFile)(const char *, const char *);
  SDL_RWops *(SDLCALL *SDL_RWFromMem)(void *, int);
  SDL_RWops *(SDLCALL *SDL_RWFromConstMem)(const void *, int);
  SDL_bool(SDLCALL *SDL_SetHint)(const char *, const char *);
  const char *(SDLCALL *SDL_GetError)(void);
  SDL_DisplayMode *(SDLCALL *SDL_GetClosestDisplayMode)(int, const SDL_DisplayMode *, SDL_DisplayMode *);
  void(SDLCALL *SDL_free)(void *);
  const char *(SDLCALL *SDL_GetPlatform)(void);

  // OPTIONAL API
  // - function pointers may be null
  // - functions are in versions after 2.0.4
  SDL_JoystickID(SDLCALL *SDL_JoystickGetDeviceInstanceID)(int device_index);
  int(SDLCALL *SDL_RenderCopyF)(SDL_Renderer *, SDL_Texture *, const SDL_Rect *, const SDL_FRect *);
  int(SDLCALL *SDL_RenderCopyExF)(
      SDL_Renderer *,
      SDL_Texture *,
      const SDL_Rect *,
      const SDL_FRect *,
      const double,
      const SDL_FPoint *,
      const SDL_RendererFlip);
  int(SDLCALL *SDL_RenderFillRectsF)(SDL_Renderer *, const SDL_FRect *, int);
  int(SDLCALL *SDL_RenderFillRectF)(SDL_Renderer *, const SDL_FRect *);

  lse_library lib;
};

// sdl library loading apis

lse_status lse_sdl_load(lse_sdl *sdl);
bool lse_sdl_is_available(lse_sdl *sdl);
void lse_sdl_unload(lse_sdl *sdl);

float lse_sdl_axis_value_to_float(Sint16 axis_value);

#ifndef SDL_JOYSTICK_AXIS_MIN
#define SDL_JOYSTICK_AXIS_MIN -32768.f
#endif

#ifndef SDL_JOYSTICK_AXIS_MAX
#define SDL_JOYSTICK_AXIS_MAX 32767.f
#endif

// Note, SDL_JOYSTICK_AXIS_* defines were introduced after 2.0.4.
#define SDL_JOYSTICK_AXIS_MIN_F ((float)SDL_JOYSTICK_AXIS_MIN)
#define SDL_JOYSTICK_AXIS_MAX_F ((float)SDL_JOYSTICK_AXIS_MAX)

#define LSE_LOG_SDL_ERROR(LIB, FUNC)                                                                                   \
  if (lse_log_level_get() >= LSE_LOG_LEVEL_ERROR)                                                                      \
  lse_log_fn(LSE_LOG_LEVEL_ERROR, FUNC, 0, "SDL: %s", (LIB)->SDL_GetError())