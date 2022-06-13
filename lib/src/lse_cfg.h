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

// ////////////////////////////////////////////////////////////////////////////
// compile time platform detection
// ////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#define LSE_CFG_WINDOWS
#elif defined(__linux__)
#define LSE_CFG_LINUX
#elif defined(__APPLE__)
#define LSE_CFG_APPLE
#endif

// ////////////////////////////////////////////////////////////////////////////
// environment variables
// ////////////////////////////////////////////////////////////////////////////

/*
 * String passed to dlopen to load the SDL2 library.
 *
 * If environment variable is not set, a platform specific library path is used.
 */
#define VAR_LSE_SDL_LIBRARY "LSE_SDL_LIBRARY"

/*
 * String passed to dlopen to load the SDL2_mixer library.
 *
 * If environment variable is not set, a platform specific library path is used.
 */
#define VAR_LSE_SDL_MIXER_LIBRARY "LSE_SDL_MIXER_LIBRARY"

/*
 * SDL GameController mapping file name.
 *
 * If set, the (csv) mappings file will be loaded on SDL_Init() during app startup.
 */
#define VAR_LSE_GAMECONTROLLER_DB "LSE_GAMECONTROLLER_DB"

// ////////////////////////////////////////////////////////////////////////////
// environment variable defaults
// ////////////////////////////////////////////////////////////////////////////

/*
 * DEFAULT_LSE_SDL_LIBRARY
 *
 * Platform specific path of where the system SDL library would be installed.
 */
#if defined(LSE_CFG_WINDOWS)
#define DEFAULT_LSE_SDL_LIBRARY "SDL2.dll"
#elif defined(LSE_CFG_APPLE)
// Note: dlopen does not appear to understand frameworks, so the full path must be specified
#define DEFAULT_LSE_SDL_LIBRARY "/Library/Frameworks/SDL2.framework/SDL2"
#else
#define DEFAULT_LSE_SDL_LIBRARY "libSDL2-2.0.so.0"
#endif

/*
 * DEFAULT_LSE_SDL_MIXER_LIBRARY
 *
 * Platform specific path of where the system SDL_mixer library would be installed.
 */
#if defined(LSE_CFG_WINDOWS)
#define DEFAULT_LSE_SDL_MIXER_LIBRARY "SDL2_mixer.dll"
#elif defined(LSE_CFG_APPLE)
// Note: dlopen does not appear to understand frameworks, so the full path must be specified
#define DEFAULT_LSE_SDL_MIXER_LIBRARY "/Library/Frameworks/SDL2_mixer.framework/SDL2_mixer"
#else
#define DEFAULT_LSE_SDL_MIXER_LIBRARY "libSDL2_mixer-2.0.so.0"
#endif

// Endianness API

#define LSE_LITTLE_ENDIAN 0
#define LSE_BIG_ENDIAN 1

#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#define LSE_BYTE_ORDER LSE_BIG_ENDIAN
#else
#define LSE_BYTE_ORDER LSE_LITTLE_ENDIAN
#endif
