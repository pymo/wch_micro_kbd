/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_KEY_SCAN_KEY_TABLE_H
#define DRIVERS_KEY_SCAN_KEY_TABLE_H

#include <stdint.h>
#include "key_codes.h"

#ifdef KEYBOARD_TYPE_G750
const uint8_t key8_table[] = //G750 keycodes
{
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_FN,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_GUI_LEFT,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_TAB,       HID_KEY_GRAVE,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_ALT_RIGHT,  HID_KEY_SHIFT_LEFT,HID_KEY_NONE,         HID_KEY_CONTROL_LEFT,HID_KEY_Q,         HID_KEY_1,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_Z,         HID_KEY_S,            HID_KEY_A,           HID_KEY_W,         HID_KEY_2,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_C,          HID_KEY_X,         HID_KEY_D,            HID_KEY_E,           HID_KEY_4,         HID_KEY_3,         HID_KEY_NONE,
HID_KEY_ARROW_UP,  HID_KEY_NONE,       HID_KEY_V,         HID_KEY_F,            HID_KEY_T,           HID_KEY_R,         HID_KEY_5,         HID_KEY_ARROW_RIGHT,
HID_KEY_NONE,      HID_KEY_N,          HID_KEY_B,         HID_KEY_H,            HID_KEY_G,           HID_KEY_Y,         HID_KEY_6,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_M,         HID_KEY_J,            HID_KEY_U,           HID_KEY_7,         HID_KEY_8,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_COMMA,      HID_KEY_K,         HID_KEY_I,            HID_KEY_O,           HID_KEY_0,         HID_KEY_9,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_PERIOD,     HID_KEY_SLASH,     HID_KEY_L,            HID_KEY_SEMICOLON,   HID_KEY_P,         HID_KEY_MINUS,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_APOSTROPHE,HID_KEY_NONE,         HID_KEY_BRACKET_LEFT,HID_KEY_EQUAL,     HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_CAPS_LOCK, HID_KEY_SHIFT_RIGHT,HID_KEY_RETURN,    HID_KEY_BRACKET_RIGHT,HID_KEY_SPACE,       HID_KEY_BACKSLASH, HID_KEY_ARROW_LEFT,HID_KEY_NONE,
HID_KEY_ARROW_DOWN,HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_DELETE,    HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_WWW_HOME,  HID_KEY_BACKSPACE,  HID_KEY_NONE,      HID_KEY_CONTROL_RIGHT,HID_KEY_BACKSLASH
};
#endif

#ifdef KEYBOARD_TYPE_PPK
const uint8_t key8_table[] = //ppk keycodes
{
HID_KEY_NONE,        HID_KEY_2,            HID_KEY_3,           HID_KEY_Z,          HID_KEY_4,   HID_KEY_5,    HID_KEY_6,     HID_KEY_7,
HID_KEY_GUI_LEFT,    HID_KEY_Q,            HID_KEY_W,           HID_KEY_E,          HID_KEY_R,   HID_KEY_T,    HID_KEY_Y,     HID_KEY_GRAVE,
HID_KEY_X,           HID_KEY_A,            HID_KEY_S,           HID_KEY_D,          HID_KEY_F,   HID_KEY_G,    HID_KEY_H,     HID_KEY_SPACE,
HID_KEY_CAPS_LOCK,   HID_KEY_TAB,          HID_KEY_CONTROL_LEFT,HID_KEY_1,          HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_FN,          HID_KEY_ALT_LEFT,   HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_C,   HID_KEY_V,    HID_KEY_B,     HID_KEY_N,
HID_KEY_MINUS,       HID_KEY_EQUAL,        HID_KEY_BACKSPACE,   HID_KEY_HOME,       HID_KEY_8,   HID_KEY_9,    HID_KEY_0,     HID_KEY_SPACE,
HID_KEY_BRACKET_LEFT,HID_KEY_BRACKET_RIGHT,HID_KEY_BACKSLASH,   HID_KEY_END,        HID_KEY_U,   HID_KEY_I,    HID_KEY_O,     HID_KEY_P,
HID_KEY_APOSTROPHE,  HID_KEY_RETURN,       HID_KEY_PAGE_UP,     HID_KEY_NONE,       HID_KEY_J,   HID_KEY_K,    HID_KEY_L,     HID_KEY_SEMICOLON,
HID_KEY_SLASH,       HID_KEY_ARROW_UP,     HID_KEY_PAGE_DOWN,   HID_KEY_NONE,       HID_KEY_M,   HID_KEY_COMMA,HID_KEY_PERIOD,HID_KEY_INSERT,
HID_KEY_DELETE,      HID_KEY_ARROW_LEFT,   HID_KEY_ARROW_DOWN,  HID_KEY_ARROW_RIGHT,HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_SHIFT_LEFT,  HID_KEY_SHIFT_RIGHT
};
#endif

#endif /* DRIVERS_KEY_SCAN_KEY_TABLE_H */

