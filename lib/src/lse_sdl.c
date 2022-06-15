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

#include "lse_sdl.h"

#include "lse_cfg.h"
#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32)
#include <windows.h>

#define lse_dlopen(NAME) LoadLibrary((NAME))
#define lse_dlclose(HANDLE) FreeLibrary((HMODULE)(HANDLE))
#define lse_dlerror() GetLastError()
#define lse_dlsym(HANDLE, SYMBOL) GetProcAddress((HMODULE)(HANDLE), (SYMBOL))
#else
#include <dlfcn.h>

#define lse_dlopen(NAME) dlopen((NAME), RTLD_LAZY)
#define lse_dlclose(HANDLE) dlclose(HANDLE)
#define lse_dlerror() dlerror()
#define lse_dlsym(HANDLE, SYMBOL) dlsym((HANDLE), (SYMBOL))
#endif

#define SDL_REQUIRED_SYMBOLS(APPLY)                                                                                    \
  APPLY(SDL_Init)                                                                                                      \
  APPLY(SDL_InitSubSystem)                                                                                             \
  APPLY(SDL_Quit)                                                                                                      \
  APPLY(SDL_QuitSubSystem)                                                                                             \
  APPLY(SDL_WasInit)                                                                                                   \
  APPLY(SDL_CreateTexture)                                                                                             \
  APPLY(SDL_DestroyTexture)                                                                                            \
  APPLY(SDL_SetTextureBlendMode)                                                                                       \
  APPLY(SDL_SetTextureColorMod)                                                                                        \
  APPLY(SDL_SetTextureAlphaMod)                                                                                        \
  APPLY(SDL_LockTexture)                                                                                               \
  APPLY(SDL_UnlockTexture)                                                                                             \
  APPLY(SDL_QueryTexture)                                                                                              \
  APPLY(SDL_UpdateTexture)                                                                                             \
  APPLY(SDL_GetPixelFormatName)                                                                                        \
  APPLY(SDL_GetVideoDriver)                                                                                            \
  APPLY(SDL_GetNumVideoDrivers)                                                                                        \
  APPLY(SDL_GetCurrentVideoDriver)                                                                                     \
  APPLY(SDL_GetRenderDriverInfo)                                                                                       \
  APPLY(SDL_GetRendererInfo)                                                                                           \
  APPLY(SDL_GetNumRenderDrivers)                                                                                       \
  APPLY(SDL_CreateRenderer)                                                                                            \
  APPLY(SDL_DestroyRenderer)                                                                                           \
  APPLY(SDL_GetRendererOutputSize)                                                                                     \
  APPLY(SDL_RenderCopy)                                                                                                \
  APPLY(SDL_RenderCopyEx)                                                                                              \
  APPLY(SDL_RenderFillRect)                                                                                            \
  APPLY(SDL_RenderFillRects)                                                                                           \
  APPLY(SDL_SetRenderDrawColor)                                                                                        \
  APPLY(SDL_SetRenderDrawBlendMode)                                                                                    \
  APPLY(SDL_SetRenderTarget)                                                                                           \
  APPLY(SDL_RenderClear)                                                                                               \
  APPLY(SDL_RenderPresent)                                                                                             \
  APPLY(SDL_RenderSetClipRect)                                                                                         \
  APPLY(SDL_GetDesktopDisplayMode)                                                                                     \
  APPLY(SDL_GetCurrentDisplayMode)                                                                                     \
  APPLY(SDL_GetDisplayName)                                                                                            \
  APPLY(SDL_GetDisplayMode)                                                                                            \
  APPLY(SDL_GetNumDisplayModes)                                                                                        \
  APPLY(SDL_GetNumVideoDisplays)                                                                                       \
  APPLY(SDL_GetVersion)                                                                                                \
  APPLY(SDL_JoystickOpen)                                                                                              \
  APPLY(SDL_JoystickClose)                                                                                             \
  APPLY(SDL_JoystickName)                                                                                              \
  APPLY(SDL_JoystickInstanceID)                                                                                        \
  APPLY(SDL_JoystickGetButton)                                                                                         \
  APPLY(SDL_JoystickNumAxes)                                                                                           \
  APPLY(SDL_JoystickNumHats)                                                                                           \
  APPLY(SDL_JoystickNumButtons)                                                                                        \
  APPLY(SDL_JoystickGetGUID)                                                                                           \
  APPLY(SDL_JoystickGetHat)                                                                                            \
  APPLY(SDL_JoystickGetAxis)                                                                                           \
  APPLY(SDL_NumJoysticks)                                                                                              \
  APPLY(SDL_JoystickGetGUIDString)                                                                                     \
  APPLY(SDL_JoystickGetGUIDFromString)                                                                                 \
  APPLY(SDL_GameControllerEventState)                                                                                  \
  APPLY(SDL_GameControllerAddMappingsFromRW)                                                                           \
  APPLY(SDL_GameControllerMapping)                                                                                     \
  APPLY(SDL_GameControllerMappingForGUID)                                                                              \
  APPLY(SDL_IsGameController)                                                                                          \
  APPLY(SDL_GameControllerOpen)                                                                                        \
  APPLY(SDL_GameControllerClose)                                                                                       \
  APPLY(SDL_GameControllerGetJoystick)                                                                                 \
  APPLY(SDL_GameControllerName)                                                                                        \
  APPLY(SDL_GameControllerGetAxis)                                                                                     \
  APPLY(SDL_GameControllerGetButton)                                                                                   \
  APPLY(SDL_GetKeyboardState)                                                                                          \
  APPLY(SDL_PeepEvents)                                                                                                \
  APPLY(SDL_PumpEvents)                                                                                                \
  APPLY(SDL_CreateWindow)                                                                                              \
  APPLY(SDL_DestroyWindow)                                                                                             \
  APPLY(SDL_SetWindowTitle)                                                                                            \
  APPLY(SDL_GetWindowDisplayMode)                                                                                      \
  APPLY(SDL_GetWindowFlags)                                                                                            \
  APPLY(SDL_ShowCursor)                                                                                                \
  APPLY(SDL_RWFromFile)                                                                                                \
  APPLY(SDL_RWFromMem)                                                                                                 \
  APPLY(SDL_RWFromConstMem)                                                                                            \
  APPLY(SDL_SetHint)                                                                                                   \
  APPLY(SDL_GetError)                                                                                                  \
  APPLY(SDL_GetClosestDisplayMode)                                                                                     \
  APPLY(SDL_free)                                                                                                      \
  APPLY(SDL_GetPlatform)

#define SDL_OPTIONAL_SYMBOLS(APPLY)                                                                                    \
  APPLY(SDL_JoystickGetDeviceInstanceID)                                                                               \
  APPLY(SDL_RenderCopyF)                                                                                               \
  APPLY(SDL_RenderCopyExF)                                                                                             \
  APPLY(SDL_RenderFillRectsF)                                                                                          \
  APPLY(SDL_RenderFillRectF)

static void set_version_string(char* target, size_t target_size, Uint8 major, Uint8 minor, Uint8 patch);

lse_status lse_sdl_load(lse_sdl* sdl) {
  SDL_version version;
  const char* path = getenv(VAR_LSE_SDL_LIBRARY);
  void* handle;

  if (lse_sdl_is_available(sdl)) {
    return LSE_OK;
  }

  if (path == NULL || path[0] == '\0') {
    path = DEFAULT_LSE_SDL_LIBRARY;
  }

  LSE_LOG_INFO("dlopen path = %s", path);
  handle = sdl->lib.handle = lse_dlopen(path);

  if (!handle) {
    LSE_LOG_ERROR("dlopen: %s", dlerror());
    return LSE_ERR_SDL_UNAVAILABLE;
  }

#define FUNC(SYMBOL)                                                                                                   \
  if (!(sdl->SYMBOL = lse_dlsym(handle, #SYMBOL))) {                                                                   \
    goto err;                                                                                                          \
  }

  SDL_REQUIRED_SYMBOLS(FUNC)

#undef FUNC

#define FUNC(SYMBOL) sdl->SYMBOL = lse_dlsym(handle, #SYMBOL);

  SDL_OPTIONAL_SYMBOLS(FUNC)

#undef FUNC

  sdl->SDL_GetVersion(&version);

  set_version_string(sdl->lib.linked_version, LSE_VERSION_STRING_SIZE, version.major, version.minor, version.patch);

  set_version_string(
      sdl->lib.compiled_version, LSE_VERSION_STRING_SIZE, SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

  LSE_LOG_INFO("SDL version = %s (header version = %s)", sdl->lib.linked_version, sdl->lib.compiled_version);

  return LSE_OK;

err:
  lse_sdl_unload(sdl);

  return LSE_ERR_SDL_UNAVAILABLE;
}

bool lse_sdl_is_available(lse_sdl* sdl) {
  return sdl && sdl->lib.handle;
}

void lse_sdl_unload(lse_sdl* sdl) {
  if (lse_sdl_is_available(sdl)) {
    lse_dlclose(sdl->lib.handle);
    memset(sdl, 1, sizeof(lse_sdl));
  }
}

static void set_version_string(char* target, size_t target_size, Uint8 major, Uint8 minor, Uint8 patch) {
  snprintf(target, target_size, "%i.%i.%i", major, minor, patch);
}

float lse_sdl_axis_value_to_float(Sint16 axis_value) {
  float value = axis_value;
  return value < 0 ? -(value / SDL_JOYSTICK_AXIS_MIN_F) : value / SDL_JOYSTICK_AXIS_MAX_F;
}
