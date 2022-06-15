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

#include <lse.h>

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static lse_log_level s_log_level = LSE_LOG_LEVEL_INFO;
static void print_header(lse_log_level level, const char* function, size_t line_num);

void lse_log(lse_log_level level, const char* fmt, ...) {
  va_list argp;

  print_header(level, NULL, 0);

  va_start(argp, fmt);
  vprintf(fmt, argp);
  va_end(argp);

  puts("");
}

LSE_API void LSE_CDECL lse_log_fn(lse_log_level level, const char* function, size_t line_num, const char* fmt, ...) {
  va_list argp;

  print_header(level, function, line_num);

  va_start(argp, fmt);
  vprintf(fmt, argp);
  va_end(argp);

  puts("");
}

LSE_API void LSE_CDECL lse_log_level_set(lse_log_level level) {
  s_log_level = level;
}

LSE_API lse_log_level LSE_CDECL lse_log_level_get() {
  return s_log_level;
}

LSE_API const char* LSE_CDECL lse_status_string(lse_status status) {
  switch (status) {
      /* Success */

    case LSE_OK:
      return "ok";

      /* General Errors */

    case LSE_ERR_GENERIC:
      return "generic error";
    case LSE_ERR_FILE_NOT_FOUND:
      return "file not found";
    case LSE_ERR_OUT_OF_MEMORY:
      return "out of memory";
    case LSE_ERR_ILLEGAL_ARGUMENT:
      return "illegal argument";
    case LSE_ERR_EOL:
      return "access of destroyed object";
    case LSE_ERR_ALREADY_CONFIGURED:
      return "object already configured";
    case LSE_ERR_FILE_READ:
      return "file read error";

      /* SDL Library Errors */

    case LSE_ERR_SDL_UNAVAILABLE:
      return "SDL unavailable";
    case LSE_ERR_SDL_MIXER_UNAVAILABLE:
      return "SDL Mixer unavailable";

      /* Resource Errors */

    case LSE_ERR_RES_SVG_PARSE:
      return "SVG parse error";
    case LSE_ERR_RES_FONT_KEY:
      return "duplicate font store key";
    case LSE_ERR_RES_FONT_UNAVAILABLE:
      return "font unavailable";
    case LSE_ERR_UNSUPPORTED_IMAGE_FORMAT:
      return "unsupported image format";

      /* Style Errors */

    case LSE_ERR_STYLE_PROPERTY:
      return "invalid style property";
    case LSE_ERR_STYLE_VALUE:
      return "invalid style value";

      /* Script Errors */

    case LSE_ERR_JS_SELF:
      return "js invalid self arg";
    case LSE_ERR_JS_WRAP:
      return "js native wrap error";
    case LSE_ERR_JS_CLASS:
      return "js invalid class type";
    case LSE_ERR_JS_CLASS_UNKNOWN:
      return "js unknown class type";
    case LSE_ERR_JS_REF:
      return "js native reference error";
    case LSE_ERR_JS_EXPORT:
      return "js export error";
    case LSE_ERR_JS_OUT_OF_MEMORY:
      return "js out of memory";
    case LSE_ERR_JS_ARGUMENT_COUNT:
      return "js unexpected number of args";
    case LSE_ERR_JS_EXPECTED_FUNCTION:
      return "js expected function";

    /* Scene Graph Node Errors */
    case LSE_ERR_NODE_REMOVE_CHILD:
      return "remove child error";
    case LSE_ERR_NODE_APPEND_CHILD:
      return "append child error";
    case LSE_ERR_NODE_INSERT_CHILD:
      return "insert child error";

    /* Graphics Errors */
    case LSE_ERR_GFX_CREATE_WINDOW:
      return "error creating window";
    case LSE_ERR_GFX_CREATE_RENDERER:
      return "error creating renderer";

      /* Uncategorized */

    case LSE_STATUS_SUBSYSTEM_ERROR:
      return "subsystem error";

    default:
      assert(false && "unknown error code");
      return "unknown";
  }
}

static void print_header(lse_log_level level, const char* function, size_t line_num) {
  char timestamp[32];
  const char* level_name;

  switch (level) {
    case LSE_LOG_LEVEL_INFO:
      level_name = "INFO";
      break;
    case LSE_LOG_LEVEL_ERROR:
      level_name = "ERROR";
      break;
    case LSE_LOG_LEVEL_DEBUG:
      level_name = "DEBUG";
      break;
    case LSE_LOG_LEVEL_TRACE:
      level_name = "TRACE";
      break;
    default:
      return;
  }

#ifdef LSE_LOG_LOCAL_TIME
  time_t rawtime;
  struct tm* timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
#else
  sprintf(timestamp, "%.2Lfms", (clock() * 1000.L) / CLOCKS_PER_SEC);
#endif

  if (function) {
    printf("[%s][%s][%s()#%zu] ", timestamp, level_name, function, line_num);
  } else {
    printf("[%s][%s] ", timestamp, level_name);
  }
}
