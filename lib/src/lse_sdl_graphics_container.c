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

#include "lse_graphics_container.h"

#include "lse_env.h"
#include "lse_graphics.h"
#include "lse_object.h"
#include <assert.h>

typedef struct lse_sdl_graphics_container lse_sdl_graphics_container;

struct lse_sdl_graphics_container {
  lse_graphics_container_base base;
  SDL_Window* window;
};

static void constructor(lse_object* object, void* arg) {
  lse_graphics_container* self = (lse_graphics_container*)object;

  lse_graphics_container_base_constructor(self, arg);
}

static void destructor(lse_object* object) {
  lse_graphics_container* self = (lse_graphics_container*)object;

  lse_graphics_container_base_destructor(self);
}

static lse_status configure(lse_graphics_container* container, const lse_window_settings* settings) {
  lse_graphics_container_base* base = lse_graphics_container_get_base(container);
  lse_sdl_graphics_container* self = (lse_sdl_graphics_container*)container;

  if (lse_graphics_container_is_destroyed(container)) {
    return LSE_ERR_EOL;
  }

  if (self->window) {
    return LSE_ERR_ALREADY_CONFIGURED;
  }

  assert(base->graphics == NULL);

  lse_sdl* sdl = lse_get_sdl_from_base(self);
  int32_t display_index = settings->display_index;
  int32_t width = settings->width;
  int32_t height = settings->height;
  Uint32 window_flags = 0;
  bool is_fullscreen = true;
  SDL_DisplayMode desired;
  SDL_DisplayMode display_mode;
  SDL_DisplayMode* result;
  lse_graphics* graphics;
  lse_status status;

  if (display_index < 0 || display_index >= sdl->SDL_GetNumVideoDisplays()) {
    display_index = 0;
  }

  switch (settings->mode) {
    case LSE_WINDOW_MODE_FULLSCREEN:
      window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
      break;
    case LSE_WINDOW_MODE_FULLSCREEN_EXCLUSIVE:
      window_flags |= SDL_WINDOW_FULLSCREEN;
      break;
    case LSE_WINDOW_MODE_WINDOWED:
    default:
      is_fullscreen = false;
      break;
  }

  if (is_fullscreen) {
    if (width == 0 && height == 0) {
      if (sdl->SDL_GetDesktopDisplayMode(display_index, &display_mode) != 0) {
        LSE_LOG_SDL_ERROR(sdl, "SDL_GetDesktopDisplayMode");
        width = 1280;
        height = 720;
      } else {
        width = display_mode.w;
        height = display_mode.h;
      }
    } else {
      result = sdl->SDL_GetClosestDisplayMode(display_index, &desired, &display_mode);

      if (!result) {
        LSE_LOG_SDL_ERROR(sdl, "SDL_GetClosestDisplayMode");
        width = 1280;
        height = 720;
      } else {
        width = result->w;
        height = result->h;
      }
    }
  } else {
    if (width == 0 && height == 0) {
      width = 960;
      height = 480;
    } else {
      // accept
      // TODO: fit to desktop bounds?
    }
  }

  self->window = sdl->SDL_CreateWindow(
      "",
      SDL_WINDOWPOS_CENTERED_DISPLAY(display_index),
      SDL_WINDOWPOS_CENTERED_DISPLAY(display_index),
      width,
      height,
      window_flags);

  if (self->window == NULL) {
    LSE_LOG_SDL_ERROR(sdl, "SDL_CreateWindow");
    return LSE_ERR_GFX_CREATE_WINDOW;
  }

  window_flags = sdl->SDL_GetWindowFlags(self->window);

  graphics = (lse_graphics*)lse_object_new(lse_sdl_graphics_type, base->env);

  if (!graphics) {
    return LSE_ERR_OUT_OF_MEMORY;
  }

  status = lse_graphics_configure(graphics, self->window);

  if (status != LSE_OK) {
    lse_graphics_destroy(graphics);
    lse_unref(graphics);
    sdl->SDL_DestroyWindow(self->window);
    self->window = NULL;

    return status;
  }

  if ((window_flags & SDL_WINDOW_FULLSCREEN) != 0) {
    sdl->SDL_ShowCursor(SDL_DISABLE);
  }

  base->graphics = graphics;

  return LSE_OK;
}

static void destroy(lse_graphics_container* container) {
  if (lse_graphics_container_is_destroyed(container)) {
    return;
  }

  lse_graphics_container_base* base = lse_graphics_container_get_base(container);
  lse_sdl_graphics_container* self = (lse_sdl_graphics_container*)container;

  if (base->graphics) {
    lse_graphics_destroy(base->graphics);
    lse_unref(base->graphics);
    base->graphics = NULL;
  }

  if (self->window) {
    lse_get_sdl_from_base(self)->SDL_DestroyWindow(self->window);
    self->window = NULL;
  }

  lse_unref(base->env);
  base->env = NULL;
}

static bool is_destroyed(lse_graphics_container* container) {
  lse_sdl_graphics_container* self = (lse_sdl_graphics_container*)container;

  return !container || (self->base.env == NULL);
}

static lse_graphics* begin_frame(lse_graphics_container* container) {
  lse_sdl_graphics_container* self = (lse_sdl_graphics_container*)container;
  lse_graphics* graphics = self->base.graphics;

  if (graphics) {
    lse_graphics_begin(graphics);
  }

  return graphics;
}

static void end_frame(lse_graphics_container* container) {
  lse_sdl_graphics_container* self = (lse_sdl_graphics_container*)container;

  if (self->base.graphics) {
    lse_graphics_end(self->base.graphics);
  }
}

static void set_title(lse_graphics_container* container, lse_string* title) {
  lse_sdl_graphics_container* self = (lse_sdl_graphics_container*)container;

  if (self->window) {
    lse_get_sdl_from_base(self)->SDL_SetWindowTitle(self->window, lse_string_as_cstring(title));
  }
}

// ////////////////////////////////////////////////////////////////////////////
// Export type information for lse_object.c:register_types().
// ////////////////////////////////////////////////////////////////////////////

LSE_GRAPHICS_CONTAINER_VTABLE();
LSE_TYPE_INFO(
    lse_sdl_graphics_container_type,
    lse_sdl_graphics_container,
    lse_graphics_container_interface_type,
    &vtable);
