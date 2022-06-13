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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LSE_API
#define LSE_CDECL

// ////////////////////////////////////////////////////////////////////////////
// enums and constants
// ////////////////////////////////////////////////////////////////////////////

typedef enum lse_status {
  /* Success */

  LSE_OK = 0,

  /* General Errors */

  LSE_ERR_GENERIC = 1,
  LSE_ERR_FILE_NOT_FOUND = 2,
  LSE_ERR_OUT_OF_MEMORY = 3,
  LSE_ERR_ILLEGAL_ARGUMENT = 4,
  LSE_ERR_EOL = 5,
  LSE_ERR_ALREADY_CONFIGURED = 6,
  LSE_ERR_FILE_READ = 7,

  /* SDL Library Errors */

  LSE_ERR_SDL_UNAVAILABLE = 101,
  LSE_ERR_SDL_MIXER_UNAVAILABLE = 102,

  /* Resource Errors */

  LSE_ERR_RES_SVG_PARSE = 201,
  LSE_ERR_RES_FONT_KEY = 202,
  LSE_ERR_RES_FONT_UNAVAILABLE = 203,
  LSE_ERR_UNSUPPORTED_IMAGE_FORMAT = 204,

  /* Style Errors */

  LSE_ERR_STYLE_PROPERTY = 301,
  LSE_ERR_STYLE_VALUE = 302,

  /* Script Errors */
  LSE_ERR_JS_SELF = 401,
  LSE_ERR_JS_WRAP = 402,
  LSE_ERR_JS_CLASS = 403,
  LSE_ERR_JS_REF = 404,
  LSE_ERR_JS_EXPORT = 405,
  LSE_ERR_JS_OUT_OF_MEMORY = 406,
  LSE_ERR_JS_ARGUMENT_COUNT = 407,
  LSE_ERR_JS_EXPECTED_FUNCTION = 408,
  LSE_ERR_JS_CLASS_UNKNOWN = 409,

  /* Scene Graph Node Errors */
  LSE_ERR_NODE_REMOVE_CHILD = 501,
  LSE_ERR_NODE_APPEND_CHILD = 502,
  LSE_ERR_NODE_INSERT_CHILD = 503,

  /* Graphics Errors */
  LSE_ERR_GFX_CREATE_WINDOW = 601,
  LSE_ERR_GFX_CREATE_RENDERER = 602,

  /* Uncategorized */
  LSE_STATUS_SUBSYSTEM_ERROR = 901,
} lse_status;

typedef enum lse_log_level {
  LSE_LOG_LEVEL_OFF = 0,
  LSE_LOG_LEVEL_ERROR = 1,
  LSE_LOG_LEVEL_INFO = 2,
  LSE_LOG_LEVEL_DEBUG = 3,
  LSE_LOG_LEVEL_TRACE = 4
} lse_log_level;

typedef enum lse_type_id {
  lse_none_type = 0,
  lse_env_type = 1,
  lse_window_type = 2,
  lse_style_type = 3,
  lse_root_node_type = 4,
  lse_box_node_type = 5,
  lse_image_node_type = 6,
  lse_text_node_type = 7,
  lse_string_type = 8,
  lse_keyboard_type = 9,
  lse_gamepad_type = 10,
  lse_font_type = 11,
  lse_image_type = 12,
  lse_array_type = 13,
} lse_type_id;

typedef enum lse_interface_type_id {
  lse_none_interface_type = 0,
  lse_node_interface_type = 1,
  lse_keyboard_interface_type = 2,
  lse_gamepad_interface_type = 3,
} lse_interface_type_id;

typedef enum lse_window_flag {
  LSE_WINDOW_FLAG_VSYNC = 1 << 0,
  LSE_WINDOW_FLAG_FULLSCREEN = 1 << 1,
  LSE_WINDOW_FLAG_FULLSCREEN_DESKTOP = 1 << 2,
} lse_window_flag;

typedef enum lse_env_state {
  LSE_ENV_STATE_INIT = 0,
  LSE_ENV_STATE_DONE = 1,
  LSE_ENV_STATE_ERROR = 2,
  LSE_ENV_STATE_RUNNING = 3,
  LSE_ENV_STATE_PAUSED = 4
} lse_env_state;

typedef enum lse_keyspace {
  LSE_KEYSPACE_MAPPED = 0,
  LSE_KEYSPACE_HARDWARE = 1,
  LSE_KEYSPACE_SCANCODE = 2,
} lse_keyspace;

typedef enum lse_key {
  LSE_KEY_UNKNOWN = -1,
  LSE_KEY_A = 0,
  LSE_KEY_B = 1,
  LSE_KEY_X = 2,
  LSE_KEY_Y = 3,
  LSE_KEY_BACK = 4,
  LSE_KEY_GUIDE = 5,
  LSE_KEY_START = 6,
  LSE_KEY_LEFT_STICK = 7,
  LSE_KEY_RIGHT_STICK = 8,
  LSE_KEY_LEFT_SHOULDER = 9,
  LSE_KEY_RIGHT_SHOULDER = 10,
  LSE_KEY_DPAD_UP = 11,
  LSE_KEY_DPAD_DOWN = 12,
  LSE_KEY_DPAD_LEFT = 13,
  LSE_KEY_DPAD_RIGHT = 14
} lse_key;
#define k_lse_key_min 0
#define k_lse_key_max 14

typedef enum lse_analog_key {
  LSE_ANALOG_KEY_UNKNOWN = -1,
  LSE_ANALOG_KEY_LEFT_STICK_X = 1000,
  LSE_ANALOG_KEY_LEFT_STICK_Y = 1001,
  LSE_ANALOG_KEY_RIGHT_STICK_X = 1002,
  LSE_ANALOG_KEY_RIGHT_STICK_Y = 1003,
  LSE_ANALOG_KEY_LEFT_TRIGGER = 1004,
  LSE_ANALOG_KEY_RIGHT_TRIGGER = 1005
} lse_analog_key;
#define k_lse_analog_key_min 1000
#define k_lse_analog_key_max 1005

typedef enum lse_hat {
  LSE_HAT_CENTERED = 0,
  LSE_HAT_UP = 0x01,
  LSE_HAT_RIGHT = 0x02,
  LSE_HAT_DOWN = 0x04,
  LSE_HAT_LEFT = 0x08,
  LSE_HAT_RIGHTUP = (LSE_HAT_RIGHT | LSE_HAT_UP),
  LSE_HAT_RIGHTDOWN = (LSE_HAT_RIGHT | LSE_HAT_DOWN),
  LSE_HAT_LEFTUP = (LSE_HAT_LEFT | LSE_HAT_UP),
  LSE_HAT_LEFTDOWN = (LSE_HAT_LEFT | LSE_HAT_DOWN),
} lse_hat;

typedef enum lse_scancode {
  LSE_SCANCODE_UNKNOWN = -1,
  LSE_SCANCODE_A = 4,
  LSE_SCANCODE_B = 5,
  LSE_SCANCODE_C = 6,
  LSE_SCANCODE_D = 7,
  LSE_SCANCODE_E = 8,
  LSE_SCANCODE_F = 9,
  LSE_SCANCODE_G = 10,
  LSE_SCANCODE_H = 11,
  LSE_SCANCODE_I = 12,
  LSE_SCANCODE_J = 13,
  LSE_SCANCODE_K = 14,
  LSE_SCANCODE_L = 15,
  LSE_SCANCODE_M = 16,
  LSE_SCANCODE_N = 17,
  LSE_SCANCODE_O = 18,
  LSE_SCANCODE_P = 19,
  LSE_SCANCODE_Q = 20,
  LSE_SCANCODE_R = 21,
  LSE_SCANCODE_S = 22,
  LSE_SCANCODE_T = 23,
  LSE_SCANCODE_U = 24,
  LSE_SCANCODE_V = 25,
  LSE_SCANCODE_W = 26,
  LSE_SCANCODE_X = 27,
  LSE_SCANCODE_Y = 28,
  LSE_SCANCODE_Z = 29,
  LSE_SCANCODE_NUM_1 = 30,
  LSE_SCANCODE_NUM_2 = 31,
  LSE_SCANCODE_NUM_3 = 32,
  LSE_SCANCODE_NUM_4 = 33,
  LSE_SCANCODE_NUM_5 = 34,
  LSE_SCANCODE_NUM_6 = 35,
  LSE_SCANCODE_NUM_7 = 36,
  LSE_SCANCODE_NUM_8 = 37,
  LSE_SCANCODE_NUM_9 = 38,
  LSE_SCANCODE_NUM_0 = 39,
  LSE_SCANCODE_RETURN = 40,
  LSE_SCANCODE_ESCAPE = 41,
  LSE_SCANCODE_BACKSPACE = 42,
  LSE_SCANCODE_TAB = 43,
  LSE_SCANCODE_SPACE = 44,
  LSE_SCANCODE_MINUS = 45,
  LSE_SCANCODE_EQUALS = 46,
  LSE_SCANCODE_LEFTBRACKET = 47,
  LSE_SCANCODE_RIGHTBRACKET = 48,
  LSE_SCANCODE_BACKSLASH = 49,
  LSE_SCANCODE_NONUSHASH = 50,
  LSE_SCANCODE_SEMICOLON = 51,
  LSE_SCANCODE_APOSTROPHE = 52,
  LSE_SCANCODE_GRAVE = 53,
  LSE_SCANCODE_COMMA = 54,
  LSE_SCANCODE_PERIOD = 55,
  LSE_SCANCODE_SLASH = 56,
  LSE_SCANCODE_CAPSLOCK = 57,
  LSE_SCANCODE_F1 = 58,
  LSE_SCANCODE_F2 = 59,
  LSE_SCANCODE_F3 = 60,
  LSE_SCANCODE_F4 = 61,
  LSE_SCANCODE_F5 = 62,
  LSE_SCANCODE_F6 = 63,
  LSE_SCANCODE_F7 = 64,
  LSE_SCANCODE_F8 = 65,
  LSE_SCANCODE_F9 = 66,
  LSE_SCANCODE_F10 = 67,
  LSE_SCANCODE_F11 = 68,
  LSE_SCANCODE_F12 = 69,
  LSE_SCANCODE_PRINTSCREEN = 70,
  LSE_SCANCODE_SCROLLLOCK = 71,
  LSE_SCANCODE_PAUSE = 72,
  LSE_SCANCODE_INSERT = 73,
  LSE_SCANCODE_HOME = 74,
  LSE_SCANCODE_PAGEUP = 75,
  LSE_SCANCODE_DELETE = 76,
  LSE_SCANCODE_END = 77,
  LSE_SCANCODE_PAGEDOWN = 78,
  LSE_SCANCODE_RIGHT = 79,
  LSE_SCANCODE_LEFT = 80,
  LSE_SCANCODE_DOWN = 81,
  LSE_SCANCODE_UP = 82,
  LSE_SCANCODE_NUMLOCKCLEAR = 83,
  LSE_SCANCODE_KP_DIVIDE = 84,
  LSE_SCANCODE_KP_MULTIPLY = 85,
  LSE_SCANCODE_KP_MINUS = 86,
  LSE_SCANCODE_KP_PLUS = 87,
  LSE_SCANCODE_KP_ENTER = 88,
  LSE_SCANCODE_KP_1 = 89,
  LSE_SCANCODE_KP_2 = 90,
  LSE_SCANCODE_KP_3 = 91,
  LSE_SCANCODE_KP_4 = 92,
  LSE_SCANCODE_KP_5 = 93,
  LSE_SCANCODE_KP_6 = 94,
  LSE_SCANCODE_KP_7 = 95,
  LSE_SCANCODE_KP_8 = 96,
  LSE_SCANCODE_KP_9 = 97,
  LSE_SCANCODE_KP_0 = 98,
  LSE_SCANCODE_KP_PERIOD = 99,
  LSE_SCANCODE_NONUSBACKSLASH = 100,
  LSE_SCANCODE_APPLICATION = 101,
  LSE_SCANCODE_POWER = 102,
  LSE_SCANCODE_KP_EQUALS = 103,
  LSE_SCANCODE_F13 = 104,
  LSE_SCANCODE_F14 = 105,
  LSE_SCANCODE_F15 = 106,
  LSE_SCANCODE_F16 = 107,
  LSE_SCANCODE_F17 = 108,
  LSE_SCANCODE_F18 = 109,
  LSE_SCANCODE_F19 = 110,
  LSE_SCANCODE_F20 = 111,
  LSE_SCANCODE_F21 = 112,
  LSE_SCANCODE_F22 = 113,
  LSE_SCANCODE_F23 = 114,
  LSE_SCANCODE_F24 = 115,
  LSE_SCANCODE_EXECUTE = 116,
  LSE_SCANCODE_HELP = 117,
  LSE_SCANCODE_MENU = 118,
  LSE_SCANCODE_SELECT = 119,
  LSE_SCANCODE_STOP = 120,
  LSE_SCANCODE_AGAIN = 121,
  LSE_SCANCODE_UNDO = 122,
  LSE_SCANCODE_CUT = 123,
  LSE_SCANCODE_COPY = 124,
  LSE_SCANCODE_PASTE = 125,
  LSE_SCANCODE_FIND = 126,
  LSE_SCANCODE_MUTE = 127,
  LSE_SCANCODE_VOLUMEUP = 128,
  LSE_SCANCODE_VOLUMEDOWN = 129,
  LSE_SCANCODE_KP_COMMA = 133,
  LSE_SCANCODE_KP_EQUALSAS400 = 134,
  LSE_SCANCODE_INTERNATIONAL1 = 135,
  LSE_SCANCODE_INTERNATIONAL2 = 136,
  LSE_SCANCODE_INTERNATIONAL3 = 137,
  LSE_SCANCODE_INTERNATIONAL4 = 138,
  LSE_SCANCODE_INTERNATIONAL5 = 139,
  LSE_SCANCODE_INTERNATIONAL6 = 140,
  LSE_SCANCODE_INTERNATIONAL7 = 141,
  LSE_SCANCODE_INTERNATIONAL8 = 142,
  LSE_SCANCODE_INTERNATIONAL9 = 143,
  LSE_SCANCODE_LANG1 = 144,
  LSE_SCANCODE_LANG2 = 145,
  LSE_SCANCODE_LANG3 = 146,
  LSE_SCANCODE_LANG4 = 147,
  LSE_SCANCODE_LANG5 = 148,
  LSE_SCANCODE_LANG6 = 149,
  LSE_SCANCODE_LANG7 = 150,
  LSE_SCANCODE_LANG8 = 151,
  LSE_SCANCODE_LANG9 = 152,
  LSE_SCANCODE_ALTERASE = 153,
  LSE_SCANCODE_SYSREQ = 154,
  LSE_SCANCODE_CANCEL = 155,
  LSE_SCANCODE_CLEAR = 156,
  LSE_SCANCODE_PRIOR = 157,
  LSE_SCANCODE_RETURN2 = 158,
  LSE_SCANCODE_SEPARATOR = 159,
  LSE_SCANCODE_OUT = 160,
  LSE_SCANCODE_OPER = 161,
  LSE_SCANCODE_CLEARAGAIN = 162,
  LSE_SCANCODE_CRSEL = 163,
  LSE_SCANCODE_EXSEL = 164,
  LSE_SCANCODE_KP_00 = 176,
  LSE_SCANCODE_KP_000 = 177,
  LSE_SCANCODE_THOUSANDSSEPARATOR = 178,
  LSE_SCANCODE_DECIMALSEPARATOR = 179,
  LSE_SCANCODE_CURRENCYUNIT = 180,
  LSE_SCANCODE_CURRENCYSUBUNIT = 181,
  LSE_SCANCODE_KP_LEFTPAREN = 182,
  LSE_SCANCODE_KP_RIGHTPAREN = 183,
  LSE_SCANCODE_KP_LEFTBRACE = 184,
  LSE_SCANCODE_KP_RIGHTBRACE = 185,
  LSE_SCANCODE_KP_TAB = 186,
  LSE_SCANCODE_KP_BACKSPACE = 187,
  LSE_SCANCODE_KP_A = 188,
  LSE_SCANCODE_KP_B = 189,
  LSE_SCANCODE_KP_C = 190,
  LSE_SCANCODE_KP_D = 191,
  LSE_SCANCODE_KP_E = 192,
  LSE_SCANCODE_KP_F = 193,
  LSE_SCANCODE_KP_XOR = 194,
  LSE_SCANCODE_KP_POWER = 195,
  LSE_SCANCODE_KP_PERCENT = 196,
  LSE_SCANCODE_KP_LESS = 197,
  LSE_SCANCODE_KP_GREATER = 198,
  LSE_SCANCODE_KP_AMPERSAND = 199,
  LSE_SCANCODE_KP_DBLAMPERSAND = 200,
  LSE_SCANCODE_KP_VERTICALBAR = 201,
  LSE_SCANCODE_KP_DBLVERTICALBAR = 202,
  LSE_SCANCODE_KP_COLON = 203,
  LSE_SCANCODE_KP_HASH = 204,
  LSE_SCANCODE_KP_SPACE = 205,
  LSE_SCANCODE_KP_AT = 206,
  LSE_SCANCODE_KP_EXCLAM = 207,
  LSE_SCANCODE_KP_MEMSTORE = 208,
  LSE_SCANCODE_KP_MEMRECALL = 209,
  LSE_SCANCODE_KP_MEMCLEAR = 210,
  LSE_SCANCODE_KP_MEMADD = 211,
  LSE_SCANCODE_KP_MEMSUBTRACT = 212,
  LSE_SCANCODE_KP_MEMMULTIPLY = 213,
  LSE_SCANCODE_KP_MEMDIVIDE = 214,
  LSE_SCANCODE_KP_PLUSMINUS = 215,
  LSE_SCANCODE_KP_CLEAR = 216,
  LSE_SCANCODE_KP_CLEARENTRY = 217,
  LSE_SCANCODE_KP_BINARY = 218,
  LSE_SCANCODE_KP_OCTAL = 219,
  LSE_SCANCODE_KP_DECIMAL = 220,
  LSE_SCANCODE_KP_HEXADECIMAL = 221,
  LSE_SCANCODE_LCTRL = 224,
  LSE_SCANCODE_LSHIFT = 225,
  LSE_SCANCODE_LALT = 226,
  LSE_SCANCODE_LGUI = 227,
  LSE_SCANCODE_RCTRL = 228,
  LSE_SCANCODE_RSHIFT = 229,
  LSE_SCANCODE_RALT = 230,
  LSE_SCANCODE_RGUI = 231,
  LSE_SCANCODE_MODE = 257,
  LSE_SCANCODE_AUDIONEXT = 258,
  LSE_SCANCODE_AUDIOPREV = 259,
  LSE_SCANCODE_AUDIOSTOP = 260,
  LSE_SCANCODE_AUDIOPLAY = 261,
  LSE_SCANCODE_AUDIOMUTE = 262,
  LSE_SCANCODE_MEDIASELECT = 263,
  LSE_SCANCODE_WWW = 264,
  LSE_SCANCODE_MAIL = 265,
  LSE_SCANCODE_CALCULATOR = 266,
  LSE_SCANCODE_COMPUTER = 267,
  LSE_SCANCODE_AC_SEARCH = 268,
  LSE_SCANCODE_AC_HOME = 269,
  LSE_SCANCODE_AC_BACK = 270,
  LSE_SCANCODE_AC_FORWARD = 271,
  LSE_SCANCODE_AC_STOP = 272,
  LSE_SCANCODE_AC_REFRESH = 273,
  LSE_SCANCODE_AC_BOOKMARKS = 274,
  LSE_SCANCODE_BRIGHTNESSDOWN = 275,
  LSE_SCANCODE_BRIGHTNESSUP = 276,
  LSE_SCANCODE_DISPLAYSWITCH = 277,
  LSE_SCANCODE_KBDILLUMTOGGLE = 278,
  LSE_SCANCODE_KBDILLUMDOWN = 279,
  LSE_SCANCODE_KBDILLUMUP = 280,
  LSE_SCANCODE_EJECT = 281,
  LSE_SCANCODE_SLEEP = 282,
  LSE_SCANCODE_APP1 = 283,
  LSE_SCANCODE_APP2 = 284,
  LSE_SCANCODE_AUDIOREWIND = 285,
  LSE_SCANCODE_AUDIOFASTFORWARD = 286
} lse_scancode;

typedef enum lse_window_mode {
  LSE_WINDOW_MODE_FULLSCREEN = 0,
  LSE_WINDOW_MODE_FULLSCREEN_EXCLUSIVE = 1,
  LSE_WINDOW_MODE_WINDOWED = 2,
} lse_window_mode;

typedef enum lse_window_fit {
  LSE_WINDOW_FIT_CLOSEST = 0,
  LSE_WINDOW_FIT_EXACT = 1,
} lse_window_fit;

typedef enum {
  LSE_RESOURCE_STATE_INIT = 0,
  LSE_RESOURCE_STATE_LOADING = 1,
  LSE_RESOURCE_STATE_READY = 2,
  LSE_RESOURCE_STATE_ERROR = 3,
  LSE_RESOURCE_STATE_DETACHED = 4,
  LSE_RESOURCE_STATE_DONE = 5,
} lse_resource_state;

// BEGIN AUTO-GENERATED SECTION ///////////////////////////////////////////////
// clang-format off

typedef enum {
  LSE_SP_ALIGN_ITEMS = 0,
  LSE_SP_ALIGN_CONTENT = 1,
  LSE_SP_ALIGN_SELF = 2,
  LSE_SP_BORDER = 3,
  LSE_SP_BORDER_BOTTOM = 4,
  LSE_SP_BORDER_LEFT = 5,
  LSE_SP_BORDER_RIGHT = 6,
  LSE_SP_BORDER_TOP = 7,
  LSE_SP_BOTTOM = 8,
  LSE_SP_DISPLAY = 9,
  LSE_SP_FLEX = 10,
  LSE_SP_FLEX_BASIS = 11,
  LSE_SP_FLEX_DIRECTION = 12,
  LSE_SP_FLEX_GROW = 13,
  LSE_SP_FLEX_SHRINK = 14,
  LSE_SP_FLEX_WRAP = 15,
  LSE_SP_HEIGHT = 16,
  LSE_SP_JUSTIFY_CONTENT = 17,
  LSE_SP_LEFT = 18,
  LSE_SP_MARGIN = 19,
  LSE_SP_MARGIN_BOTTOM = 20,
  LSE_SP_MARGIN_LEFT = 21,
  LSE_SP_MARGIN_RIGHT = 22,
  LSE_SP_MARGIN_TOP = 23,
  LSE_SP_MAX_HEIGHT = 24,
  LSE_SP_MAX_WIDTH = 25,
  LSE_SP_MIN_HEIGHT = 26,
  LSE_SP_MIN_WIDTH = 27,
  LSE_SP_OVERFLOW = 28,
  LSE_SP_PADDING = 29,
  LSE_SP_PADDING_BOTTOM = 30,
  LSE_SP_PADDING_LEFT = 31,
  LSE_SP_PADDING_RIGHT = 32,
  LSE_SP_PADDING_TOP = 33,
  LSE_SP_POSITION = 34,
  LSE_SP_RIGHT = 35,
  LSE_SP_TOP = 36,
  LSE_SP_WIDTH = 37,
  LSE_SP_BACKGROUND_COLOR = 38,
  LSE_SP_BACKGROUND_HEIGHT = 39,
  LSE_SP_BACKGROUND_IMAGE = 40,
  LSE_SP_BACKGROUND_POSITION_X = 41,
  LSE_SP_BACKGROUND_POSITION_Y = 42,
  LSE_SP_BACKGROUND_SIZE = 43,
  LSE_SP_BACKGROUND_WIDTH = 44,
  LSE_SP_BORDER_COLOR = 45,
  LSE_SP_BORDER_RADIUS = 46,
  LSE_SP_BORDER_RADIUS_TOP_LEFT = 47,
  LSE_SP_BORDER_RADIUS_TOP_RIGHT = 48,
  LSE_SP_BORDER_RADIUS_BOTTOM_LEFT = 49,
  LSE_SP_BORDER_RADIUS_BOTTOM_RIGHT = 50,
  LSE_SP_COLOR = 51,
  LSE_SP_FILTER = 52,
  LSE_SP_FONT_FAMILY = 53,
  LSE_SP_FONT_KERNING = 54,
  LSE_SP_FONT_SIZE = 55,
  LSE_SP_FONT_STYLE = 56,
  LSE_SP_FONT_WEIGHT = 57,
  LSE_SP_MAX_LINES = 58,
  LSE_SP_OBJECT_FIT = 59,
  LSE_SP_OBJECT_POSITION_X = 60,
  LSE_SP_OBJECT_POSITION_Y = 61,
  LSE_SP_OPACITY = 62,
  LSE_SP_OVERFLOW_WRAP = 63,
  LSE_SP_TEXT_ALIGN = 64,
  LSE_SP_TEXT_OVERFLOW = 65,
  LSE_SP_TEXT_TRANSFORM = 66,
  LSE_SP_TRANSFORM = 67,
  LSE_SP_TRANSFORM_ORIGIN_X = 68,
  LSE_SP_TRANSFORM_ORIGIN_Y = 69,
  LSE_SP_WHITE_SPACE = 70,
} lse_style_property;

typedef enum lse_style_align {
  LSE_STYLE_ALIGN_AUTO = 0,
  LSE_STYLE_ALIGN_FLEX_START = 1,
  LSE_STYLE_ALIGN_CENTER = 2,
  LSE_STYLE_ALIGN_FLEX_END = 3,
  LSE_STYLE_ALIGN_STRETCH = 4,
  LSE_STYLE_ALIGN_BASELINE = 5,
  LSE_STYLE_ALIGN_SPACE_BETWEEN = 6,
  LSE_STYLE_ALIGN_SPACE_AROUND = 7,
} lse_style_align;
#define k_lse_style_align_count 8

typedef enum lse_style_display {
  LSE_STYLE_DISPLAY_FLEX = 0,
  LSE_STYLE_DISPLAY_NONE = 1,
} lse_style_display;
#define k_lse_style_display_count 2

typedef enum lse_style_flex_direction {
  LSE_STYLE_FLEX_DIRECTION_COLUMN = 0,
  LSE_STYLE_FLEX_DIRECTION_COLUMN_REVERSE = 1,
  LSE_STYLE_FLEX_DIRECTION_ROW = 2,
  LSE_STYLE_FLEX_DIRECTION_ROW_REVERSE = 3,
} lse_style_flex_direction;
#define k_lse_style_flex_direction_count 4

typedef enum lse_style_overflow {
  LSE_STYLE_OVERFLOW_VISIBLE = 0,
  LSE_STYLE_OVERFLOW_HIDDEN = 1,
  LSE_STYLE_OVERFLOW_SCROLL = 2,
} lse_style_overflow;
#define k_lse_style_overflow_count 3

typedef enum lse_style_overflow_wrap {
  LSE_STYLE_OVERFLOW_WRAP_NORMAL = 0,
  LSE_STYLE_OVERFLOW_WRAP_BREAK_WORD = 1,
} lse_style_overflow_wrap;
#define k_lse_style_overflow_wrap_count 2

typedef enum lse_style_position {
  LSE_STYLE_POSITION_STATIC = 0,
  LSE_STYLE_POSITION_RELATIVE = 1,
  LSE_STYLE_POSITION_ABSOLUTE = 2,
} lse_style_position;
#define k_lse_style_position_count 3

typedef enum lse_style_wrap {
  LSE_STYLE_WRAP_NO_WRAP = 0,
  LSE_STYLE_WRAP_WRAP = 1,
  LSE_STYLE_WRAP_WRAP_REVERSE = 2,
} lse_style_wrap;
#define k_lse_style_wrap_count 3

typedef enum lse_style_justify {
  LSE_STYLE_JUSTIFY_FLEX_START = 0,
  LSE_STYLE_JUSTIFY_CENTER = 1,
  LSE_STYLE_JUSTIFY_FLEX_END = 2,
  LSE_STYLE_JUSTIFY_SPACE_BETWEEN = 3,
  LSE_STYLE_JUSTIFY_SPACE_AROUND = 4,
  LSE_STYLE_JUSTIFY_SPACE_EVENLY = 5,
} lse_style_justify;
#define k_lse_style_justify_count 6

typedef enum lse_style_font_style {
  LSE_STYLE_FONT_STYLE_NORMAL = 0,
  LSE_STYLE_FONT_STYLE_ITALIC = 1,
  LSE_STYLE_FONT_STYLE_OBLIQUE = 2,
} lse_style_font_style;
#define k_lse_style_font_style_count 3

typedef enum lse_style_font_weight {
  LSE_STYLE_FONT_WEIGHT_NORMAL = 0,
  LSE_STYLE_FONT_WEIGHT_BOLD = 1,
} lse_style_font_weight;
#define k_lse_style_font_weight_count 2

typedef enum lse_style_text_overflow {
  LSE_STYLE_TEXT_OVERFLOW_NONE = 0,
  LSE_STYLE_TEXT_OVERFLOW_CLIP = 1,
  LSE_STYLE_TEXT_OVERFLOW_ELLIPSIS = 2,
} lse_style_text_overflow;
#define k_lse_style_text_overflow_count 3

typedef enum lse_style_text_align {
  LSE_STYLE_TEXT_ALIGN_LEFT = 0,
  LSE_STYLE_TEXT_ALIGN_CENTER = 1,
  LSE_STYLE_TEXT_ALIGN_RIGHT = 2,
} lse_style_text_align;
#define k_lse_style_text_align_count 3

typedef enum lse_style_object_fit {
  LSE_STYLE_OBJECT_FIT_FILL = 0,
  LSE_STYLE_OBJECT_FIT_CONTAIN = 1,
  LSE_STYLE_OBJECT_FIT_COVER = 2,
  LSE_STYLE_OBJECT_FIT_NONE = 3,
  LSE_STYLE_OBJECT_FIT_SCALE_DOWN = 4,
} lse_style_object_fit;
#define k_lse_style_object_fit_count 5

typedef enum lse_style_background_clip {
  LSE_STYLE_BACKGROUND_CLIP_BORDER_BOX = 0,
  LSE_STYLE_BACKGROUND_CLIP_PADDING_BOX = 1,
  LSE_STYLE_BACKGROUND_CLIP_CONTENT_BOX = 2,
} lse_style_background_clip;
#define k_lse_style_background_clip_count 3

typedef enum lse_style_background_size {
  LSE_STYLE_BACKGROUND_SIZE_NONE = 0,
  LSE_STYLE_BACKGROUND_SIZE_COVER = 1,
  LSE_STYLE_BACKGROUND_SIZE_CONTAIN = 2,
} lse_style_background_size;
#define k_lse_style_background_size_count 3

typedef enum lse_style_background_repeat {
  LSE_STYLE_BACKGROUND_REPEAT_OFF = 0,
  LSE_STYLE_BACKGROUND_REPEAT_XY = 1,
  LSE_STYLE_BACKGROUND_REPEAT_X = 2,
  LSE_STYLE_BACKGROUND_REPEAT_Y = 3,
} lse_style_background_repeat;
#define k_lse_style_background_repeat_count 4

typedef enum lse_style_text_transform {
  LSE_STYLE_TEXT_TRANSFORM_NONE = 0,
  LSE_STYLE_TEXT_TRANSFORM_UPPERCASE = 1,
  LSE_STYLE_TEXT_TRANSFORM_LOWERCASE = 2,
} lse_style_text_transform;
#define k_lse_style_text_transform_count 3

typedef enum lse_style_white_space {
  LSE_STYLE_WHITE_SPACE_NORMAL = 0,
  LSE_STYLE_WHITE_SPACE_PRE = 1,
  LSE_STYLE_WHITE_SPACE_NO_WRAP = 2,
} lse_style_white_space;
#define k_lse_style_white_space_count 3

typedef enum lse_style_font_kerning {
  LSE_STYLE_FONT_KERNING_NORMAL = 0,
  LSE_STYLE_FONT_KERNING_NONE = 1,
} lse_style_font_kerning;
#define k_lse_style_font_kerning_count 2

typedef enum lse_style_anchor {
  LSE_STYLE_ANCHOR_TOP = 0,
  LSE_STYLE_ANCHOR_RIGHT = 1,
  LSE_STYLE_ANCHOR_BOTTOM = 2,
  LSE_STYLE_ANCHOR_LEFT = 3,
  LSE_STYLE_ANCHOR_CENTER = 4,
} lse_style_anchor;
#define k_lse_style_anchor_count 5

typedef enum lse_style_unit {
  LSE_STYLE_UNIT_UNDEFINED = 0,
  LSE_STYLE_UNIT_PX = 1,
  LSE_STYLE_UNIT_PT = 2,
  LSE_STYLE_UNIT_PERCENT = 3,
  LSE_STYLE_UNIT_AUTO = 4,
  LSE_STYLE_UNIT_ANCHOR = 5,
  LSE_STYLE_UNIT_REM = 6,
  LSE_STYLE_UNIT_VMIN = 7,
  LSE_STYLE_UNIT_VMAX = 8,
  LSE_STYLE_UNIT_VW = 9,
  LSE_STYLE_UNIT_VH = 10,
  LSE_STYLE_UNIT_DEG = 11,
  LSE_STYLE_UNIT_RAD = 12,
  LSE_STYLE_UNIT_GRAD = 13,
  LSE_STYLE_UNIT_TURN = 14,
} lse_style_unit;
#define k_lse_style_unit_count 15

typedef enum {
  LSE_STYLE_PROPERTY_TYPE_NUMBER = 0,
  LSE_STYLE_PROPERTY_TYPE_ENUM = 1,
  LSE_STYLE_PROPERTY_TYPE_COLOR = 2,
  LSE_STYLE_PROPERTY_TYPE_STRING = 3,
  LSE_STYLE_PROPERTY_TYPE_FILTER = 4,
  LSE_STYLE_PROPERTY_TYPE_TRANSFORM = 5,
  LSE_STYLE_PROPERTY_TYPE_UNKNOWN = -1,
} lse_style_property_type;

typedef enum {
  LSE_STYLE_TRANSFORM_IDENTITY = 0,
  LSE_STYLE_TRANSFORM_TRANSLATE = 1,
  LSE_STYLE_TRANSFORM_SCALE = 2,
  LSE_STYLE_TRANSFORM_ROTATE = 3,
} lse_style_transform_op;
#define lse_is_style_transform_op(T) ((T) >= 0 && (T) <= 3)

typedef enum {
  LSE_STYLE_FILTER_FLIP_H = 0,
  LSE_STYLE_FILTER_FLIP_V = 1,
  LSE_STYLE_FILTER_TINT = 2,
} lse_style_filter_function;
#define lse_is_style_filter_function(T) ((T) >= 0 && (T) <= 2)

// clang-format on

// END AUTO-GENERATED SECTION ///////////////////////////////////////////////

#define LSE_NODE_TAG_BOX "box"
#define LSE_NODE_TAG_IMAGE "img"
#define LSE_NODE_TAG_TEXT "text"

// ////////////////////////////////////////////////////////////////////////////
// types
// ////////////////////////////////////////////////////////////////////////////

typedef struct lse_object lse_object;
typedef struct lse_env lse_env;
typedef struct lse_window lse_window;
typedef struct lse_string lse_string;
typedef struct lse_node lse_node;
typedef struct lse_style lse_style;
typedef struct lse_keyboard lse_keyboard;
typedef struct lse_gamepad lse_gamepad;
typedef struct lse_font lse_font;
typedef struct lse_image lse_image;
typedef struct lse_array lse_array;

typedef struct lse_display_mode lse_display_mode;
typedef struct lse_renderer_info lse_renderer_info;
typedef struct lse_style_value lse_style_value;
typedef struct lse_mock_settings lse_mock_settings;
typedef struct lse_font_event lse_font_event;
typedef struct lse_image_event lse_image_event;
typedef struct lse_window_settings lse_window_settings;
typedef struct lse_settings lse_settings;
typedef struct lse_sdl_mixer_settings lse_sdl_mixer_settings;
typedef struct lse_sdl_settings lse_sdl_settings;
typedef struct lse_style_filter lse_style_filter;
typedef struct lse_style_transform lse_style_transform;

struct lse_display_mode {
  int32_t width;
  int32_t height;
  int32_t refresh_rate;
};

struct lse_renderer_info {
  const char* name;
  bool has_hardware_acceleration;
  bool has_vsync;
  bool has_render_to_texture;
  int32_t max_texture_width;
  int32_t max_texture_height;
};

struct lse_style_value {
  lse_style_unit unit;
  float value;
};

struct lse_style_transform {
  lse_style_transform_op op;

  // LCD of data for all transform types. use API to access this data.
  lse_style_value a;
  lse_style_value b;
};

struct lse_style_filter {
  lse_style_filter_function func;

  // LCD of arguments for all function types. use API to access this data.
  uint32_t color;
};

struct lse_mock_settings {
  bool enabled;
};

struct lse_sdl_settings {
  bool enabled;
};

struct lse_sdl_mixer_settings {
  bool enabled;
};

struct lse_settings {
  lse_mock_settings mock_settings;
  lse_sdl_settings sdl_settings;
  lse_sdl_mixer_settings sdl_mixer_settings;
};

struct lse_window_settings {
  int32_t width;
  int32_t height;
  int32_t display_index;
  lse_window_mode mode;
  lse_window_fit fit;
};

struct lse_image_event {
  lse_image* image;
  lse_resource_state state;
};

struct lse_font_event {
  lse_font* font;
  lse_resource_state state;
};

typedef void (*lse_font_event_callback)(const lse_font_event*, void*);
typedef void (*lse_image_event_callback)(const lse_image_event*, void*);

typedef void (*lse_gamepad_status_callback)(lse_gamepad*, bool, void*);
typedef void (*lse_gamepad_button_callback)(int32_t, lse_keyspace, int32_t, bool, bool, void*);
typedef void (*lse_gamepad_axis_motion_callback)(int32_t, lse_keyspace, int32_t, float, void*);
typedef void (*lse_gamepad_hat_motion_callback)(int32_t, lse_keyspace, int32_t, int32_t, void*);
typedef void (*lse_keyboard_button_callback)(lse_keyspace, int32_t, bool, bool, void*);

typedef void (*lse_thread_pool_work_callback)(void* user_data);
typedef void (*lse_thread_pool_complete_callback)(lse_env* env, void* user_data);
typedef void (*lse_thread_pool_user_data_finalize)(void* user_data);

typedef struct lse_thread_pool lse_thread_pool;
typedef struct lse_thread_pool_task lse_thread_pool_task;

typedef lse_thread_pool_task* (*lse_thread_pool_queue)(
    lse_thread_pool*,
    const char*,
    lse_thread_pool_work_callback,
    lse_thread_pool_complete_callback,
    void*,
    lse_thread_pool_user_data_finalize);
typedef void (*lse_thread_pool_cancel)(lse_thread_pool*, lse_thread_pool_task*);
typedef void (*lse_thread_pool_free)(lse_thread_pool*);

typedef void (*lse_array_item_drop)(void*);

// ////////////////////////////////////////////////////////////////////////////
// lse_env
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_env* LSE_CDECL lse_env_new();

LSE_API lse_status LSE_CDECL lse_env_configure(lse_env* env, const lse_settings* settings);
LSE_API void LSE_CDECL lse_env_destroy(lse_env* env);

LSE_API bool LSE_CDECL lse_env_is_configured(lse_env* env);
LSE_API bool LSE_CDECL lse_env_is_running(lse_env* env);
LSE_API bool LSE_CDECL lse_env_is_destroyed(lse_env* env);
LSE_API bool LSE_CDECL lse_env_is_error(lse_env* env);

LSE_API bool LSE_CDECL lse_env_has_quit_request(lse_env* env);

LSE_API void LSE_CDECL lse_env_update(lse_env* env);

LSE_API void LSE_CDECL
lse_env_set_gamepad_status_callback(lse_env* env, lse_gamepad_status_callback callback, void* data);
LSE_API void LSE_CDECL
lse_env_set_gamepad_button_callback(lse_env* env, lse_gamepad_button_callback callback, void* data);
LSE_API void LSE_CDECL
lse_env_set_gamepad_axis_motion_callback(lse_env* env, lse_gamepad_axis_motion_callback callback, void* data);
LSE_API void LSE_CDECL
lse_env_set_gamepad_hat_motion_callback(lse_env* env, lse_gamepad_hat_motion_callback callback, void* data);
LSE_API void LSE_CDECL
lse_env_set_keyboard_button_callback(lse_env* env, lse_keyboard_button_callback callback, void* data);
LSE_API void LSE_CDECL lse_env_reset_event_callbacks(lse_env* env);

LSE_API const char* LSE_CDECL lse_env_get_video_driver(lse_env* env, int32_t driver_index);
LSE_API int32_t LSE_CDECL lse_env_get_video_driver_count(lse_env* env);
LSE_API const char* LSE_CDECL lse_env_get_display_name(lse_env* env, int32_t display_index);
LSE_API bool LSE_CDECL lse_env_get_current_display_mode(lse_env* env, int32_t display_index, lse_display_mode* mode);
LSE_API bool LSE_CDECL lse_env_get_desktop_display_mode(lse_env* env, int32_t display_index, lse_display_mode* mode);
LSE_API int32_t LSE_CDECL lse_env_get_display_count(lse_env* env);
LSE_API bool LSE_CDECL
lse_env_get_display_mode(lse_env* env, int32_t display_index, int32_t display_mode_index, lse_display_mode* out);
LSE_API int32_t LSE_CDECL lse_env_get_display_mode_count(lse_env* env, int32_t display_index);
LSE_API int32_t LSE_CDECL lse_env_get_renderer_count(lse_env* env);
LSE_API bool LSE_CDECL lse_env_get_renderer_info(lse_env* env, int32_t renderer_index, lse_renderer_info* info);
LSE_API bool LSE_CDECL lse_env_find_closest_display_mode(
    lse_env* env,
    int32_t display_index,
    int32_t desired_width,
    int32_t desired_height,
    lse_display_mode* out);

LSE_API lse_window* LSE_CDECL lse_env_add_window(lse_env* env);
LSE_API bool LSE_CDECL lse_env_remove_window(lse_env* env, lse_window* window);
LSE_API lse_window* LSE_CDECL lse_env_get_window(lse_env* env, size_t index);
LSE_API size_t LSE_CDECL lse_env_get_window_count(lse_env* env);

LSE_API lse_keyboard* LSE_CDECL lse_env_get_keyboard(lse_env* env);
LSE_API lse_gamepad* LSE_CDECL lse_env_get_gamepad(lse_env* env, size_t index);
LSE_API size_t LSE_CDECL lse_env_get_gamepad_count(lse_env* env);

LSE_API lse_string* LSE_CDECL lse_env_get_mapping(lse_env* env, const char* uuid);
LSE_API bool LSE_CDECL lse_env_set_mapping(lse_env* env, const char* uuid, lse_string* mapping);
LSE_API bool LSE_CDECL lse_env_reset_mapping(lse_env* env, const char* uuid);

LSE_API bool LSE_CDECL lse_env_set_thread_pool(
    lse_env* env,
    lse_thread_pool* context,
    lse_thread_pool_queue queue,
    lse_thread_pool_cancel cancel,
    lse_thread_pool_free thread_pool_free);

LSE_API lse_status LSE_CDECL lse_env_add_font(
    lse_env* env,
    lse_string* uri,
    int32_t index,
    lse_string* family,
    lse_style_font_style style,
    lse_style_font_weight weight);

LSE_API lse_font* LSE_CDECL
lse_env_acquire_font(lse_env* env, const char* family, lse_style_font_style style, lse_style_font_weight weight);

LSE_API void LSE_CDECL lse_env_release_font(lse_env* env, lse_font* font);

LSE_API lse_font* LSE_CDECL
lse_env_get_font(lse_env* env, const char* family, lse_style_font_style style, lse_style_font_weight weight);

// ////////////////////////////////////////////////////////////////////////////
// lse_gamepad
// ////////////////////////////////////////////////////////////////////////////

LSE_API int32_t LSE_CDECL lse_gamepad_get_id(lse_gamepad* gamepad);
LSE_API const char* LSE_CDECL lse_gamepad_get_name(lse_gamepad* gamepad);
LSE_API const char* LSE_CDECL lse_gamepad_get_uuid(lse_gamepad* gamepad);
LSE_API bool LSE_CDECL lse_gamepad_is_connected(lse_gamepad* gamepad);
LSE_API bool LSE_CDECL lse_gamepad_get_button_state(lse_gamepad* gamepad, int32_t button);
LSE_API float LSE_CDECL lse_gamepad_get_axis_state(lse_gamepad* gamepad, int32_t axis_index);
LSE_API lse_hat LSE_CDECL lse_gamepad_get_hat_state(lse_gamepad* gamepad, int32_t hat_index);
LSE_API float LSE_CDECL lse_gamepad_get_analog_state(lse_gamepad* gamepad, lse_analog_key analog_key);
LSE_API bool LSE_CDECL lse_gamepad_get_key_state(lse_gamepad* gamepad, lse_key key);
LSE_API int32_t LSE_CDECL lse_gamepad_get_button_count(lse_gamepad* gamepad);
LSE_API int32_t LSE_CDECL lse_gamepad_get_axis_count(lse_gamepad* gamepad);
LSE_API int32_t LSE_CDECL lse_gamepad_get_hat_count(lse_gamepad* gamepad);

// ////////////////////////////////////////////////////////////////////////////
// lse_keyboard
// ////////////////////////////////////////////////////////////////////////////

LSE_API int32_t LSE_CDECL lse_keyboard_get_id(lse_keyboard* keyboard);
LSE_API const char* LSE_CDECL lse_keyboard_get_name(lse_keyboard* keyboard);
LSE_API const char* LSE_CDECL lse_keyboard_get_uuid(lse_keyboard* keyboard);
LSE_API bool LSE_CDECL lse_keyboard_get_scancode_state(lse_keyboard* keyboard, int32_t scancode);
LSE_API bool LSE_CDECL lse_keyboard_is_connected(lse_keyboard* keyboard);
LSE_API bool LSE_CDECL lse_keyboard_is_destroyed(lse_keyboard* keyboard);

// ////////////////////////////////////////////////////////////////////////////
// lse_node
// ////////////////////////////////////////////////////////////////////////////

LSE_API float LSE_CDECL lse_node_get_x(lse_node* node);
LSE_API float LSE_CDECL lse_node_get_y(lse_node* node);
LSE_API float LSE_CDECL lse_node_get_width(lse_node* node);
LSE_API float LSE_CDECL lse_node_get_height(lse_node* node);

LSE_API lse_node* LSE_CDECL lse_node_get_parent(lse_node* node);
LSE_API uint32_t LSE_CDECL lse_node_get_child_count(lse_node* node);
LSE_API lse_node* LSE_CDECL lse_node_get_child_at(lse_node* node, uint32_t index);

LSE_API lse_style* LSE_CDECL lse_node_get_style(lse_node* node);
LSE_API lse_style* LSE_CDECL lse_node_get_style_or_empty(lse_node* node);

LSE_API bool LSE_CDECL lse_node_append(lse_node* node, lse_node* child);
LSE_API bool LSE_CDECL lse_node_insert_before(lse_node* node, lse_node* insert, lse_node* before);
LSE_API bool LSE_CDECL lse_node_remove_child(lse_node* node, lse_node* child);

LSE_API bool LSE_CDECL lse_node_set_src(lse_node* node, lse_string* uri);
LSE_API lse_string* LSE_CDECL lse_node_get_src(lse_node* node);

LSE_API bool LSE_CDECL lse_node_set_text(lse_node* node, lse_string* text);
LSE_API lse_string* LSE_CDECL lse_node_get_text(lse_node* node);

// ////////////////////////////////////////////////////////////////////////////
// lse_window
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_status LSE_CDECL lse_window_configure(lse_window* window, const lse_window_settings* settings);
LSE_API lse_node* LSE_CDECL lse_window_get_root(lse_window* window);
LSE_API void LSE_CDECL lse_window_set_title(lse_window* window, lse_string* title);
LSE_API int32_t LSE_CDECL lse_window_get_width(lse_window* window);
LSE_API int32_t LSE_CDECL lse_window_get_height(lse_window* window);
LSE_API int32_t LSE_CDECL lse_window_get_refresh_rate(lse_window* window);
LSE_API uint32_t LSE_CDECL lse_window_get_flags(lse_window* window);
LSE_API lse_node* LSE_CDECL lse_window_create_node_from_tag(lse_window* window, const char* tag);
// LSE_API lse_node LSE_CDECL lse_env_create_box_node(lse_env env);
// LSE_API lse_node LSE_CDECL lse_env_create_image_node(lse_env env);
// LSE_API lse_node LSE_CDECL lse_env_create_text_node(lse_env env);
LSE_API bool LSE_CDECL lse_window_is_destroyed(lse_window* window);

// ////////////////////////////////////////////////////////////////////////////
// lse_style
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_style* LSE_CDECL lse_style_new();
LSE_API lse_style* LSE_CDECL lse_style_new_class();

LSE_API bool LSE_CDECL lse_style_is_class(lse_style* style);
LSE_API bool LSE_CDECL lse_style_set_parent(lse_style* style, lse_style* parent);
LSE_API lse_style* lse_style_get_parent(lse_style* style);

LSE_API lse_status LSE_CDECL lse_style_lock(lse_style* style);
LSE_API bool LSE_CDECL lse_style_is_locked(lse_style* style);
LSE_API bool LSE_CDECL lse_style_reset(lse_style* style);

LSE_API bool LSE_CDECL lse_style_has_property(lse_style* style, lse_style_property prop);
LSE_API bool LSE_CDECL lse_style_unset(lse_style* style, lse_style_property prop);

LSE_API bool LSE_CDECL lse_style_from_string(lse_style* style, lse_style_property prop, lse_string* str);

LSE_API bool LSE_CDECL lse_style_set_numeric(lse_style* style, lse_style_property prop, const lse_style_value* value);
LSE_API bool LSE_CDECL lse_style_set_enum(lse_style* style, lse_style_property prop, int32_t value);
LSE_API bool LSE_CDECL lse_style_set_object(lse_style* style, lse_style_property prop, lse_object* value);
LSE_API bool LSE_CDECL lse_style_set_color(lse_style* style, lse_style_property prop, uint32_t value);

LSE_API const lse_style_value* LSE_CDECL lse_style_get_numeric(lse_style* style, lse_style_property prop);
LSE_API int32_t LSE_CDECL lse_style_get_enum(lse_style* style, lse_style_property prop);
LSE_API uint32_t LSE_CDECL lse_style_get_color(lse_style* style, lse_style_property prop);
LSE_API lse_object* LSE_CDECL lse_style_get_object(lse_style* style, lse_style_property prop);

// ////////////////////////////////////////////////////////////////////////////
// lse_style_transform
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_object* lse_style_transform_new(lse_style_transform* list, size_t len);

LSE_API bool lse_style_transform_identity(lse_style_transform* out);
LSE_API bool lse_style_transform_translate(lse_style_value x, lse_style_value y, lse_style_transform* out);
LSE_API bool lse_style_transform_scale(lse_style_value x, lse_style_value y, lse_style_transform* out);
LSE_API bool lse_style_transform_rotate(lse_style_value angle, lse_style_transform* out);

LSE_API lse_style_transform_op lse_style_transform_get_op(lse_style_transform* transform);
LSE_API lse_style_value lse_style_transform_get_translate_x(lse_style_transform* transform);
LSE_API lse_style_value lse_style_transform_get_translate_y(lse_style_transform* transform);
LSE_API lse_style_value lse_style_transform_get_scale_x(lse_style_transform* transform);
LSE_API lse_style_value lse_style_transform_get_scale_y(lse_style_transform* transform);
LSE_API lse_style_value lse_style_transform_get_angle(lse_style_transform* transform);

// ////////////////////////////////////////////////////////////////////////////
// lse_style_filter
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_object* lse_style_filter_new(lse_style_filter* list, size_t len);

LSE_API bool lse_style_filter_flip_h(lse_style_filter* out);
LSE_API bool lse_style_filter_flip_v(lse_style_filter* out);
LSE_API bool lse_style_filter_tint(uint32_t color, lse_style_filter* out);

LSE_API lse_style_filter_function lse_style_filter_get_function(lse_style_filter* filter);
LSE_API uint32_t lse_style_filter_get_color(lse_style_filter* filter);

// ////////////////////////////////////////////////////////////////////////////
// lse_font
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_string* LSE_CDECL lse_font_get_uri(lse_font* font);
LSE_API int32_t LSE_CDECL lse_font_get_index(lse_font* font);
LSE_API lse_string* LSE_CDECL lse_font_get_family(lse_font* font);
LSE_API lse_style_font_style LSE_CDECL lse_font_get_style(lse_font* font);
LSE_API lse_style_font_weight LSE_CDECL lse_font_get_weight(lse_font* font);
LSE_API lse_resource_state LSE_CDECL lse_font_get_state(lse_font* font);
LSE_API bool LSE_CDECL lse_font_is_destroyed(lse_font* font);
LSE_API void LSE_CDECL lse_font_add_observer(lse_font* font, void* observer, lse_font_event_callback observer_callback);
LSE_API void LSE_CDECL lse_font_remove_observer(lse_font* font, void* observer);

// ////////////////////////////////////////////////////////////////////////////
// lse_image
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_string* LSE_CDECL lse_image_get_uri(lse_image* image);
LSE_API int32_t LSE_CDECL lse_image_get_width(lse_image* image);
LSE_API int32_t LSE_CDECL lse_image_get_height(lse_image* image);
LSE_API int32_t LSE_CDECL lse_image_get_format(lse_image* image);
LSE_API lse_resource_state LSE_CDECL lse_image_get_state(lse_image* image);
LSE_API void LSE_CDECL
lse_image_add_observer(lse_image* image, void* observer, lse_image_event_callback observer_callback);
LSE_API void LSE_CDECL lse_image_remove_observer(lse_image* image, void* observer);

// ////////////////////////////////////////////////////////////////////////////
// lse_string
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_string* LSE_CDECL lse_string_new(const char* str);
LSE_API lse_string* LSE_CDECL lse_string_new_with_size(char fill, size_t n);
LSE_API lse_string* LSE_CDECL lse_string_new_empty();
LSE_API const char* LSE_CDECL lse_string_as_cstring(lse_string* str);
LSE_API char* LSE_CDECL lse_string_as_data(lse_string* str);
LSE_API size_t LSE_CDECL lse_string_size(lse_string* str);
LSE_API bool LSE_CDECL lse_string_empty(lse_string* str);
LSE_API int32_t LSE_CDECL lse_string_cmp(lse_string* s1, lse_string* s2);
LSE_API int32_t LSE_CDECL lse_string_case_cmp(lse_string* s1, lse_string* s2);

// ////////////////////////////////////////////////////////////////////////////
// lse_array
// ////////////////////////////////////////////////////////////////////////////

LSE_API lse_array* lse_array_new(size_t length, size_t item_size, lse_array_item_drop drop);
LSE_API lse_array* lse_array_new_empty();
LSE_API size_t lse_array_get_length(lse_array* array);
LSE_API void* lse_array_at(lse_array* array, size_t i);
LSE_API void* lse_array_get(lse_array* array, size_t i);
LSE_API bool lse_array_set(lse_array* array, size_t i, void* item);

// ////////////////////////////////////////////////////////////////////////////
// lse_settings
// ////////////////////////////////////////////////////////////////////////////

lse_settings lse_settings_init();
lse_settings lse_settings_mock();

// ////////////////////////////////////////////////////////////////////////////
// lse_object
// ////////////////////////////////////////////////////////////////////////////

#define lse_ref(OBJECT) lse_object_ref((lse_object*)(OBJECT))
#define lse_unref(OBJECT) lse_object_unref((lse_object*)(OBJECT))
#define lse_get_ref_count(OBJECT) lse_object_get_ref_count((lse_object*)(OBJECT))

LSE_API uint32_t LSE_CDECL lse_object_ref(lse_object* object);
LSE_API void LSE_CDECL lse_object_unref(lse_object* object);
LSE_API uint32_t LSE_CDECL lse_object_get_ref_count(lse_object* object);
LSE_API uint8_t LSE_CDECL lse_object_get_type(lse_object* object);
LSE_API const char* LSE_CDECL lse_object_get_type_name(lse_object* object);

// ////////////////////////////////////////////////////////////////////////////
// logging
// ////////////////////////////////////////////////////////////////////////////

// TODO: lse_log_info(...)
LSE_API void LSE_CDECL lse_log(lse_log_level level, const char* fmt, ...);
LSE_API void LSE_CDECL lse_log_level_set(lse_log_level level);
LSE_API lse_log_level LSE_CDECL lse_log_level_get();

LSE_API void LSE_CDECL lse_log_fn(lse_log_level level, const char* function, size_t line_num, const char* fmt, ...);

#define LSE_LOG_INFO(...)                                                                                              \
  if (lse_log_level_get() >= LSE_LOG_LEVEL_INFO)                                                                       \
  lse_log_fn(LSE_LOG_LEVEL_INFO, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LSE_LOG_DEBUG(...)                                                                                             \
  if (lse_log_level_get() >= LSE_LOG_LEVEL_DEBUG)                                                                      \
  lse_log_fn(LSE_LOG_LEVEL_DEBUG, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LSE_LOG_ERROR(...)                                                                                             \
  if (lse_log_level_get() >= LSE_LOG_LEVEL_ERROR)                                                                      \
  lse_log_fn(LSE_LOG_LEVEL_ERROR, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LSE_LOG_TRACE(...)                                                                                             \
  if (lse_log_level_get() >= LSE_LOG_LEVEL_TRACE)                                                                      \
  lse_log_fn(LSE_LOG_LEVEL_TRACE, __FUNCTION__, __LINE__, __VA_ARGS__)

LSE_API const char* LSE_CDECL lse_status_string(lse_status status);

#ifdef __cplusplus
} // end extern "C"
#endif
