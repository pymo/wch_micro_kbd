/*
 * key_table.c
 *
 *  Created on: Aug 9, 2024
 *      Author: chen_
 */
#include "key_table.h"
#include "USB/hid.h"

uint8_t fn_key_table[256] = {HID_KEY_NONE};

// Maps the specially-defined codes to the standard consumer HID keycode.
// The amount and order must be exactly the same as the special codes in key_codes.h
uint16_t consumer_key_table[] = {
        HID_USAGE_CONSUMER_EMAIL, // HID_KEY_EMAIL
        HID_USAGE_CONSUMER_CALCULATOR, // HID_KEY_CALCULATOR
        HID_USAGE_CONSUMER_VOLUME_DEC, // HID_KEY_VOL_DEC
        HID_USAGE_CONSUMER_VOLUME_INC, // HID_KEY_VOL_INC
        HID_USAGE_CONSUMER_BRIGHTNESS_DEC, // HID_KEY_BRIGHTNESS_DEC
        HID_USAGE_CONSUMER_BRIGHTNESS_INC, // HID_KEY_BRIGHTNESS_INC
        HID_USAGE_CONSUMER_PLAYPAUSE, // HID_KEY_PLAY
        HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK, // HID_KEY_PREVIOUS
        HID_USAGE_CONSUMER_SCAN_NEXT_TRACK, // HID_KEY_NEXT
        HID_USAGE_CONSUMER_WWW_HOME, // HID_KEY_WWW_HOME
        HID_USAGE_CONSUMER_WWW_BACK // HID_KEY_WWW_BACK
};

// How does the key8_table variable work:
// Whenever the MCU receives a scan code from the keyboard, it looks up key8_table[scan_code] to get the HID code.
// e.g. if you want the scancode 28 to map to `A`, the 29th element (because it starts with 0) in this array should be HID_KEY_A.

// Special note: on PPK, '1''s raw scan_code is 0, but we want to use 0 to represent no key in the array,
// so we artificially remap '1' scancode to 27. You can see the code in keyscan.c, void ScanKeyPPK() function.

/********************************   G750 keycodes   ********************************/

#ifdef KEYBOARD_TYPE_G750

#ifdef KEYBOARD_LAYOUT_ISO
//G750 UK/ES scan codes
const uint8_t key8_table[] =
{
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_FN,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_GUI_LEFT,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_TAB,       HID_KEY_GRAVE,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_SPACE,      HID_KEY_SHIFT_LEFT,HID_KEY_NONE,         HID_KEY_CONTROL_LEFT,HID_KEY_Q,         HID_KEY_1,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_Z,         HID_KEY_S,            HID_KEY_A,           HID_KEY_W,         HID_KEY_2,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_C,          HID_KEY_X,         HID_KEY_D,            HID_KEY_E,           HID_KEY_4,         HID_KEY_3,         HID_KEY_NONE,
HID_KEY_ARROW_UP,  HID_KEY_NONE,       HID_KEY_V,         HID_KEY_F,            HID_KEY_T,           HID_KEY_R,         HID_KEY_5,         HID_KEY_ARROW_RIGHT,
HID_KEY_NONE,      HID_KEY_N,          HID_KEY_B,         HID_KEY_H,            HID_KEY_G,           HID_KEY_Y,         HID_KEY_6,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_M,         HID_KEY_J,            HID_KEY_U,           HID_KEY_7,         HID_KEY_8,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_COMMA,      HID_KEY_K,         HID_KEY_I,            HID_KEY_O,           HID_KEY_0,         HID_KEY_9,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_PERIOD,     HID_KEY_SLASH,     HID_KEY_L,            HID_KEY_SEMICOLON,   HID_KEY_P,         HID_KEY_MINUS,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_EUROPE_1,  HID_KEY_NONE,         HID_KEY_BRACKET_LEFT,HID_KEY_EQUAL,     HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_CAPS_LOCK, HID_KEY_SHIFT_RIGHT,HID_KEY_RETURN,    HID_KEY_BRACKET_RIGHT,HID_KEY_SPACE,       HID_KEY_BACKSLASH, HID_KEY_ARROW_LEFT,HID_KEY_NONE,
HID_KEY_ARROW_DOWN,HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_DELETE,    HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_ALT_RIGHT, HID_KEY_BACKSPACE,  HID_KEY_NONE,      HID_KEY_CONTROL_RIGHT,HID_KEY_EUROPE_2
};
#elif KEYBOARD_LAYOUT_G740
//G740 Palm variant scan codes
const uint8_t key8_table[] =
{
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_FN,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_GUI_LEFT,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_TAB,       HID_KEY_GRAVE,     HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_SPACE,      HID_KEY_SHIFT_LEFT,HID_KEY_NONE,         HID_KEY_CONTROL_LEFT,HID_KEY_Q,         HID_KEY_1,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_Z,         HID_KEY_S,            HID_KEY_A,           HID_KEY_W,         HID_KEY_2,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_C,          HID_KEY_X,         HID_KEY_D,            HID_KEY_E,           HID_KEY_4,         HID_KEY_3,         HID_KEY_NONE,
HID_KEY_SHIFT_RIGHT, HID_KEY_NONE,       HID_KEY_V,         HID_KEY_F,            HID_KEY_T,           HID_KEY_R,         HID_KEY_5,         HID_KEY_ARROW_DOWN,
HID_KEY_NONE,        HID_KEY_N,          HID_KEY_B,         HID_KEY_H,            HID_KEY_G,           HID_KEY_Y,         HID_KEY_6,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_M,         HID_KEY_J,            HID_KEY_U,           HID_KEY_7,         HID_KEY_8,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_COMMA,      HID_KEY_K,         HID_KEY_I,            HID_KEY_O,           HID_KEY_0,         HID_KEY_9,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_PERIOD,     HID_KEY_SLASH,     HID_KEY_L,            HID_KEY_SEMICOLON,   HID_KEY_P,         HID_KEY_MINUS,     HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_APOSTROPHE,HID_KEY_NONE,         HID_KEY_BRACKET_LEFT,HID_KEY_EQUAL,     HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_CAPS_LOCK,   HID_KEY_ARROW_UP,   HID_KEY_RETURN,    HID_KEY_BRACKET_RIGHT,HID_KEY_SPACE,       HID_KEY_BACKSLASH, HID_KEY_ARROW_LEFT,HID_KEY_NONE,
HID_KEY_ARROW_RIGHT, HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_DELETE,    HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_PRINT_SCREEN,HID_KEY_BACKSPACE,  HID_KEY_NONE,      HID_KEY_APPLICATION,  HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_ALT_RIGHT
};
#else
//G750, G7L0 and G7L1 US scan codes (assume alt and Today keycaps are swapped)
const uint8_t key8_table[] =
{
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_FN,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_GUI_LEFT,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_TAB,       HID_KEY_GRAVE,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_SPACE,      HID_KEY_SHIFT_LEFT,HID_KEY_NONE,         HID_KEY_CONTROL_LEFT,HID_KEY_Q,         HID_KEY_1,         HID_KEY_NONE,
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
HID_KEY_ALT_RIGHT, HID_KEY_BACKSPACE,  HID_KEY_NONE,      HID_KEY_CONTROL_RIGHT,HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_SPACE
};
#endif

void init_fn_key_table(){
    fn_key_table[HID_KEY_TAB] = HID_KEY_ESCAPE;
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_PAGE_UP;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_PAGE_DOWN;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_HOME;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_END;
    fn_key_table[HID_KEY_DELETE] = HID_KEY_INSERT;
    fn_key_table[HID_KEY_1] = HID_KEY_F1;
    fn_key_table[HID_KEY_2] = HID_KEY_F2;
    fn_key_table[HID_KEY_3] = HID_KEY_F3;
    fn_key_table[HID_KEY_4] = HID_KEY_F4;
    fn_key_table[HID_KEY_5] = HID_KEY_F5;
    fn_key_table[HID_KEY_6] = HID_KEY_F6;
    fn_key_table[HID_KEY_7] = HID_KEY_F7;
    fn_key_table[HID_KEY_8] = HID_KEY_F8;
    fn_key_table[HID_KEY_9] = HID_KEY_F9;
    fn_key_table[HID_KEY_0] = HID_KEY_F10;
    fn_key_table[HID_KEY_MINUS] = HID_KEY_F11;
    fn_key_table[HID_KEY_EQUAL] = HID_KEY_F12;
#ifdef ENABLE_NUMLOCK
    fn_key_table[HID_KEY_CAPS_LOCK] = HID_KEY_NUM_LOCK;
#endif
    fn_key_table[HID_KEY_BACKSPACE] = HID_KEY_POWER;
    fn_key_table[HID_KEY_Q] = HID_KEY_EMAIL;
    fn_key_table[HID_KEY_R] = HID_KEY_CALCULATOR;
    fn_key_table[HID_KEY_P] = HID_KEY_PRINT_SCREEN;
    fn_key_table[HID_KEY_BRACKET_LEFT] = HID_KEY_BRIGHTNESS_DEC;
    fn_key_table[HID_KEY_BRACKET_RIGHT] = HID_KEY_BRIGHTNESS_INC;
    fn_key_table[HID_KEY_COMMA] = HID_KEY_VOL_DEC;
    fn_key_table[HID_KEY_PERIOD] = HID_KEY_VOL_INC;
    fn_key_table[HID_KEY_GUI_LEFT] = HID_KEY_WWW_HOME;
    fn_key_table[HID_KEY_SPACE] = HID_KEY_PLAY;
    fn_key_table[HID_KEY_U] = HID_KEY_MODE_USB;
    fn_key_table[HID_KEY_A] = HID_KEY_MODE_BLE_CHAN_1;
    fn_key_table[HID_KEY_S] = HID_KEY_MODE_BLE_CHAN_2;
    fn_key_table[HID_KEY_D] = HID_KEY_MODE_BLE_CHAN_3;
    fn_key_table[HID_KEY_F] = HID_KEY_MODE_BLE_CHAN_4;
}

#endif

/********************************   PPK (Palm Portable Keyboard) keycodes   ********************************/

#ifdef KEYBOARD_TYPE_PPK

#ifdef KEYBOARD_LAYOUT_ISO
const uint8_t key8_table[] = //German keycodes
{
HID_KEY_NONE,        HID_KEY_2,            HID_KEY_3,           HID_KEY_Z,          HID_KEY_4,   HID_KEY_5,    HID_KEY_6,     HID_KEY_7,
HID_KEY_GUI_LEFT,    HID_KEY_Q,            HID_KEY_W,           HID_KEY_E,          HID_KEY_R,   HID_KEY_T,    HID_KEY_Y,     HID_KEY_EUROPE_2,
HID_KEY_X,           HID_KEY_A,            HID_KEY_S,           HID_KEY_D,          HID_KEY_F,   HID_KEY_G,    HID_KEY_H,     HID_KEY_SPACE,
HID_KEY_CAPS_LOCK,   HID_KEY_TAB,          HID_KEY_CONTROL_LEFT,HID_KEY_1,          HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_FN,          HID_KEY_ALT_LEFT,   HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_C,   HID_KEY_V,    HID_KEY_B,     HID_KEY_N,
HID_KEY_MINUS,       HID_KEY_EQUAL,        HID_KEY_BACKSPACE,   HID_KEY_HOME,       HID_KEY_8,   HID_KEY_9,    HID_KEY_0,     HID_KEY_GRAVE,
HID_KEY_BRACKET_LEFT,HID_KEY_BRACKET_RIGHT,HID_KEY_EUROPE_1,    HID_KEY_END,        HID_KEY_U,   HID_KEY_I,    HID_KEY_O,     HID_KEY_P,
HID_KEY_APOSTROPHE,  HID_KEY_RETURN,       HID_KEY_PAGE_UP,     HID_KEY_NONE,       HID_KEY_J,   HID_KEY_K,    HID_KEY_L,     HID_KEY_SEMICOLON,
HID_KEY_SLASH,       HID_KEY_ARROW_UP,     HID_KEY_PAGE_DOWN,   HID_KEY_NONE,       HID_KEY_M,   HID_KEY_COMMA,HID_KEY_PERIOD,HID_KEY_ALT_RIGHT,
HID_KEY_DELETE,      HID_KEY_ARROW_LEFT,   HID_KEY_ARROW_DOWN,  HID_KEY_ARROW_RIGHT,HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_SHIFT_LEFT,  HID_KEY_SHIFT_RIGHT
};
#elif PPK_TYPE_JORNADA
const uint8_t key8_table[] = //Jornada US keycodes
{
HID_KEY_NONE,        HID_KEY_2,            HID_KEY_3,             HID_KEY_Z,          HID_KEY_4,   HID_KEY_5,    HID_KEY_6,     HID_KEY_7,
HID_KEY_FN,          HID_KEY_Q,            HID_KEY_W,             HID_KEY_E,          HID_KEY_R,   HID_KEY_T,    HID_KEY_Y,     HID_KEY_GRAVE,
HID_KEY_X,           HID_KEY_A,            HID_KEY_S,             HID_KEY_D,          HID_KEY_F,   HID_KEY_G,    HID_KEY_H,     HID_KEY_SPACE,
HID_KEY_CAPS_LOCK,   HID_KEY_TAB,          HID_KEY_CONTROL_LEFT,  HID_KEY_1,          HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_GUI_LEFT,      HID_KEY_ALT_LEFT,   HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_NONE,          HID_KEY_NONE,       HID_KEY_C,   HID_KEY_V,    HID_KEY_B,     HID_KEY_N,
HID_KEY_MINUS,       HID_KEY_EQUAL,        HID_KEY_BACKSPACE,     HID_KEY_VOL_INC,    HID_KEY_8,   HID_KEY_9,    HID_KEY_0,     HID_KEY_SPACE,
HID_KEY_BRACKET_LEFT,HID_KEY_BRACKET_RIGHT,HID_KEY_BACKSLASH,     HID_KEY_VOL_DEC,    HID_KEY_U,   HID_KEY_I,    HID_KEY_O,     HID_KEY_P,
HID_KEY_APOSTROPHE,  HID_KEY_RETURN,       HID_KEY_BRIGHTNESS_DEC,HID_KEY_NONE,       HID_KEY_J,   HID_KEY_K,    HID_KEY_L,     HID_KEY_SEMICOLON,
HID_KEY_SLASH,       HID_KEY_ARROW_UP,     HID_KEY_BRIGHTNESS_INC,HID_KEY_NONE,       HID_KEY_M,   HID_KEY_COMMA,HID_KEY_PERIOD,HID_KEY_INSERT,
HID_KEY_DELETE,      HID_KEY_ARROW_LEFT,   HID_KEY_ARROW_DOWN,    HID_KEY_ARROW_RIGHT,HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_SHIFT_LEFT,  HID_KEY_SHIFT_RIGHT
};
#else
const uint8_t key8_table[] = //Other PPK US keycodes
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

void init_fn_key_table(){
    fn_key_table[HID_KEY_TAB] = HID_KEY_ESCAPE;
#ifdef PPK_TYPE_JORNADA
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_PAGE_UP;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_PAGE_DOWN;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_HOME;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_END;
#else
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_VOL_INC;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_VOL_DEC;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_BRIGHTNESS_DEC;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_BRIGHTNESS_INC;
#endif
    fn_key_table[HID_KEY_1] = HID_KEY_F1;
    fn_key_table[HID_KEY_2] = HID_KEY_F2;
    fn_key_table[HID_KEY_3] = HID_KEY_F3;
    fn_key_table[HID_KEY_4] = HID_KEY_F4;
    fn_key_table[HID_KEY_5] = HID_KEY_F5;
    fn_key_table[HID_KEY_6] = HID_KEY_F6;
    fn_key_table[HID_KEY_7] = HID_KEY_F7;
    fn_key_table[HID_KEY_8] = HID_KEY_F8;
    fn_key_table[HID_KEY_9] = HID_KEY_F9;
    fn_key_table[HID_KEY_0] = HID_KEY_F10;
    fn_key_table[HID_KEY_MINUS] = HID_KEY_F11;
    fn_key_table[HID_KEY_EQUAL] = HID_KEY_F12;
    fn_key_table[HID_KEY_GUI_LEFT] = HID_KEY_WWW_HOME;
    fn_key_table[HID_KEY_SPACE] = HID_KEY_PLAY;
    fn_key_table[HID_KEY_COMMA] = HID_KEY_PREVIOUS;
    fn_key_table[HID_KEY_PERIOD] = HID_KEY_NEXT;
    fn_key_table[HID_KEY_U] = HID_KEY_MODE_USB;
    fn_key_table[HID_KEY_A] = HID_KEY_MODE_BLE_CHAN_1;
    fn_key_table[HID_KEY_S] = HID_KEY_MODE_BLE_CHAN_2;
    fn_key_table[HID_KEY_D] = HID_KEY_MODE_BLE_CHAN_3;
    fn_key_table[HID_KEY_F] = HID_KEY_MODE_BLE_CHAN_4;
#ifdef KEYBOARD_LAYOUT_ISO
    fn_key_table[HID_KEY_ALT_RIGHT] = HID_KEY_PRINT_SCREEN;  // For German layout
    fn_key_table[HID_KEY_DELETE] = HID_KEY_INSERT;  // For German layout
#else
    fn_key_table[HID_KEY_INSERT] = HID_KEY_PRINT_SCREEN;
#endif
}
#endif
/*3   4   9   10  14
1   Up  M   U   Down    K
2   Left    N   R Fn    R Space J
5   Return  BS  P   /   =
6   .   '   O   Del -
7   ,   ;   I   Right   L
8               RShift
11  V   B   F   C   L Alt
12  Caps    L Ctrl  D   X   T
13  H   G   L Fn    L Space Y
15  Q   W   A   L Shift E
16  Cmd Tab S   Z   R
*/
#ifdef KEYBOARD_TYPE_ULTRATHIN
const uint8_t key8_table[] = //P10900U keycodes
{
HID_KEY_NONE,
HID_KEY_ARROW_UP,  HID_KEY_M,           HID_KEY_U,   HID_KEY_ARROW_DOWN,    HID_KEY_K,
HID_KEY_ARROW_LEFT,HID_KEY_N,           HID_KEY_FN2, HID_KEY_SPACE/*right*/,HID_KEY_J,
HID_KEY_RETURN,    HID_KEY_BACKSPACE,   HID_KEY_P,   HID_KEY_SLASH,         HID_KEY_EQUAL,
HID_KEY_PERIOD,    HID_KEY_APOSTROPHE,  HID_KEY_O,   HID_KEY_DELETE,        HID_KEY_MINUS,
HID_KEY_COMMA,     HID_KEY_SEMICOLON,   HID_KEY_I,   HID_KEY_ARROW_RIGHT,   HID_KEY_L,
HID_KEY_NONE,      HID_KEY_NONE,        HID_KEY_NONE,HID_KEY_SHIFT_RIGHT,   HID_KEY_NONE,
HID_KEY_V,         HID_KEY_B,           HID_KEY_F,   HID_KEY_C,             HID_KEY_ALT_LEFT,
HID_KEY_CAPS_LOCK, HID_KEY_CONTROL_LEFT,HID_KEY_D,   HID_KEY_X,             HID_KEY_T,
HID_KEY_H,         HID_KEY_G,           HID_KEY_FN,  HID_KEY_SPACE/*left*/, HID_KEY_Y,
HID_KEY_Q,         HID_KEY_W,           HID_KEY_A,   HID_KEY_SHIFT_LEFT,    HID_KEY_E,
HID_KEY_GUI_LEFT,  HID_KEY_TAB,         HID_KEY_S,   HID_KEY_Z,             HID_KEY_R
};

void init_fn_key_table(){
    fn_key_table[HID_KEY_TAB] = HID_KEY_ESCAPE;
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_PAGE_UP;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_PAGE_DOWN;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_HOME;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_END;
    fn_key_table[HID_KEY_DELETE] = HID_KEY_INSERT;
    fn_key_table[HID_KEY_Q] = HID_KEY_1;
    fn_key_table[HID_KEY_W] = HID_KEY_2;
    fn_key_table[HID_KEY_E] = HID_KEY_3;
    fn_key_table[HID_KEY_R] = HID_KEY_4;
    fn_key_table[HID_KEY_T] = HID_KEY_5;
    fn_key_table[HID_KEY_Y] = HID_KEY_6;
    fn_key_table[HID_KEY_U] = HID_KEY_7;
    fn_key_table[HID_KEY_I] = HID_KEY_8;
    fn_key_table[HID_KEY_O] = HID_KEY_9;
    fn_key_table[HID_KEY_P] = HID_KEY_0;
    fn_key_table[HID_KEY_SLASH] = HID_KEY_BACKSLASH;
    fn_key_table[HID_KEY_MINUS] = HID_KEY_BRACKET_LEFT;
    fn_key_table[HID_KEY_EQUAL] = HID_KEY_BRACKET_RIGHT;
#ifdef ENABLE_NUMLOCK
    fn_key_table[HID_KEY_CAPS_LOCK] = HID_KEY_NUM_LOCK;
#endif
    fn_key_table[HID_KEY_J] = HID_KEY_WWW_HOME;
    fn_key_table[HID_KEY_K] = HID_KEY_APPLICATION;
    fn_key_table[HID_KEY_COMMA] = HID_KEY_VOL_DEC;
    fn_key_table[HID_KEY_PERIOD] = HID_KEY_VOL_INC;
    fn_key_table[HID_KEY_SPACE] = HID_KEY_PLAY;
    fn_key_table[HID_KEY_G] = HID_KEY_MODE_USB;
    fn_key_table[HID_KEY_A] = HID_KEY_MODE_BLE_CHAN_1;
    fn_key_table[HID_KEY_S] = HID_KEY_MODE_BLE_CHAN_2;
    fn_key_table[HID_KEY_D] = HID_KEY_MODE_BLE_CHAN_3;
    fn_key_table[HID_KEY_F] = HID_KEY_MODE_BLE_CHAN_4;
}

#endif

uint8_t key8_table_size = sizeof(key8_table);

uint8_t AsciiToHidCode(uint8_t ascii){
    if (ascii>='A' && ascii<='Z'){
        return ascii-'A'+HID_KEY_A;
    }
    if (ascii>='a' && ascii<='z'){
        return ascii-'a'+HID_KEY_A;
    }
    if (ascii>='1' && ascii<='9'){
        return ascii-'1'+HID_KEY_1;
    }
    if (ascii=='0'){
        return HID_KEY_0;
    }
    if (ascii=='\n'){
        return HID_KEY_RETURN;
    }
    return HID_KEY_SPACE;
}
