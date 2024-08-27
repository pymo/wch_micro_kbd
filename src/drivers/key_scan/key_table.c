/*
 * key_table.c
 *
 *  Created on: Aug 9, 2024
 *      Author: chen_
 */
#include "key_table.h"
#include "USB/hid.h"

uint8_t fn_key_table[256] = {0};

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

#ifdef KEYBOARD_TYPE_G750
const uint8_t key8_table[] = //G750 scan codes
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
    fn_key_table[HID_KEY_SPACE] = HID_KEY_PLAY;
    fn_key_table[HID_KEY_U] = HID_KEY_MODE_USB;
    fn_key_table[HID_KEY_A] = HID_KEY_MODE_BLE_CHAN_1;
    fn_key_table[HID_KEY_S] = HID_KEY_MODE_BLE_CHAN_2;
    fn_key_table[HID_KEY_D] = HID_KEY_MODE_BLE_CHAN_3;
    fn_key_table[HID_KEY_F] = HID_KEY_MODE_BLE_CHAN_4;
}

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

void init_fn_key_table(){
    fn_key_table[HID_KEY_TAB] = HID_KEY_ESCAPE;
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_VOL_INC;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_VOL_DEC;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_BRIGHTNESS_DEC;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_BRIGHTNESS_INC;
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
    fn_key_table[HID_KEY_GUI_LEFT] = HID_KEY_WWW_HOME;
    fn_key_table[HID_KEY_SPACE] = HID_KEY_PLAY;
    fn_key_table[HID_KEY_COMMA] = HID_KEY_PREVIOUS;
    fn_key_table[HID_KEY_PERIOD] = HID_KEY_NEXT;
    fn_key_table[HID_KEY_INSERT] = HID_KEY_PRINT_SCREEN;
    fn_key_table[HID_KEY_U] = HID_KEY_MODE_USB;
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