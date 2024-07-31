/*
 * Copyright (c) 2022 zerosensei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "key_parse.h"
#include "key_table.h"
#include "CH58x_common.h"

bool key_fn_flag;

void keymap_to_keybuf8(uint8_t *index, uint8_t *keyVal, uint8_t len)
{
    for (int i = 0, idx = 0; i < len; i++)
    {
        if (index[i]>sizeof(key8_table)){
            PRINT("raw keycode %#x out of range!\n", index[i]);
            continue;
        }
        //PRINT("%d -> %#x\n", index[i], key8_table[index[i]]);
        if (!key8_table[index[i]])
            continue;
        keyVal[idx++] = key8_table[index[i]];
    }
}

struct key16_type key8_to_key16(uint8_t key8)
{
    uint8_t general_code = key8 - 4;
    struct key16_type key;

    key.index = general_code / 8 + 1;
    key.val = 1 << (general_code % 8);

    return key;
}

void keybuf8_to_keybuf16(uint8_t *key8, uint8_t *key16, uint8_t len)
{
    for(int i = 0 ; i < len; i++) {
        struct key16_type temp = key8_to_key16(key8[i]);

        key16[temp.index] = temp.val;
    }
}

uint8_t CompactIntegers(uint8_t *buf, uint8_t len)
{
    uint8_t new_len = 0;

    for (int i = 0; i < len; i++) {
        if (buf[i] != 0) {
            buf[new_len] = buf[i];
            new_len++;
        }
    }

    return new_len;
}

void ModifierKeyHandler(uint8_t *hid_keycodes, uint8_t *keys, uint8_t nums)
{
    key_fn_flag = false;
    for (uint8_t i = 0; i < nums; ++i)
    {
        switch (hid_keycodes[i]){
        case HID_KEY_CONTROL_LEFT:
            keys[0] |= KEYBOARD_MODIFIER_LEFTCTRL;
            hid_keycodes[i] = 0;
            break;
        case HID_KEY_SHIFT_LEFT:
            keys[0] |= KEYBOARD_MODIFIER_LEFTSHIFT;
            hid_keycodes[i] = 0;
            break;
        case HID_KEY_ALT_LEFT:
            keys[0] |= KEYBOARD_MODIFIER_LEFTALT;
            hid_keycodes[i] = 0;
            break;
        case HID_KEY_GUI_LEFT:
            keys[0] |= KEYBOARD_MODIFIER_LEFTGUI;
            hid_keycodes[i] = 0;
            break;
        case HID_KEY_CONTROL_RIGHT:
            keys[0] |= KEYBOARD_MODIFIER_RIGHTCTRL;
            hid_keycodes[i] = 0;
            break;
        case HID_KEY_SHIFT_RIGHT:
            keys[0] |= KEYBOARD_MODIFIER_RIGHTSHIFT;
            hid_keycodes[i] = 0;
            break;
        case HID_KEY_ALT_RIGHT:
            keys[0] |= KEYBOARD_MODIFIER_RIGHTALT;
            hid_keycodes[i] = 0;
            break;
        case HID_KEY_GUI_RIGHT:
            keys[0] |= KEYBOARD_MODIFIER_RIGHTGUI;
            hid_keycodes[i] = 0;
            break;
        case HID_KEY_FN:
            key_fn_flag = true;
            hid_keycodes[i] = 0;
            break;
        default:
            break;
        }
    }
}

#ifdef KEYBOARD_TYPE_G750
// If Fn+key is mapped to another HID key (i.e. not consumer or special function), handle it here, otherwise handle it in key_special.c
void FnHidKeyHandler(uint8_t *hid_keycodes, uint8_t nums){
    if (!key_fn_flag) return;
    for (uint8_t i = 0; i < nums; ++i)
    {
        switch (hid_keycodes[i]){
        case HID_KEY_TAB:
            hid_keycodes[i] = HID_KEY_ESCAPE;
            break;
        case HID_KEY_ARROW_UP:
            hid_keycodes[i] = HID_KEY_PAGE_UP;
            break;
        case HID_KEY_ARROW_DOWN:
            hid_keycodes[i] = HID_KEY_PAGE_DOWN;
            break;
        case HID_KEY_ARROW_LEFT:
            hid_keycodes[i] = HID_KEY_HOME;
            break;
        case HID_KEY_ARROW_RIGHT:
            hid_keycodes[i] = HID_KEY_END;
            break;
        case HID_KEY_DELETE:
            hid_keycodes[i] = HID_KEY_INSERT;
            break;
        case HID_KEY_1:
            hid_keycodes[i] = HID_KEY_F1;
            break;
        case HID_KEY_2:
            hid_keycodes[i] = HID_KEY_F2;
            break;
        case HID_KEY_3:
            hid_keycodes[i] = HID_KEY_F3;
            break;
        case HID_KEY_4:
            hid_keycodes[i] = HID_KEY_F4;
            break;
        case HID_KEY_5:
            hid_keycodes[i] = HID_KEY_F5;
            break;
        case HID_KEY_6:
            hid_keycodes[i] = HID_KEY_F6;
            break;
        case HID_KEY_7:
            hid_keycodes[i] = HID_KEY_F7;
            break;
        case HID_KEY_8:
            hid_keycodes[i] = HID_KEY_F8;
            break;
        case HID_KEY_9:
            hid_keycodes[i] = HID_KEY_F9;
            break;
        case HID_KEY_0:
            hid_keycodes[i] = HID_KEY_F10;
            break;
        case HID_KEY_MINUS:
            hid_keycodes[i] = HID_KEY_F11;
            break;
        case HID_KEY_EQUAL:
            hid_keycodes[i] = HID_KEY_F12;
            break;
        default:
            break;
        }
    }
}
#endif

#ifdef KEYBOARD_TYPE_PPK
// If Fn+key is mapped to another HID key (i.e. not consumer or special function), handle it here, otherwise handle it in key_special.c
void FnHidKeyHandler(uint8_t *hid_keycodes, uint8_t nums){
    if (!key_fn_flag) return;
    for (uint8_t i = 0; i < nums; ++i)
    {
        switch (hid_keycodes[i]){
        case HID_KEY_TAB:
            hid_keycodes[i] = HID_KEY_ESCAPE;
            break;
        case HID_KEY_1:
            hid_keycodes[i] = HID_KEY_F1;
            break;
        case HID_KEY_2:
            hid_keycodes[i] = HID_KEY_F2;
            break;
        case HID_KEY_3:
            hid_keycodes[i] = HID_KEY_F3;
            break;
        case HID_KEY_4:
            hid_keycodes[i] = HID_KEY_F4;
            break;
        case HID_KEY_5:
            hid_keycodes[i] = HID_KEY_F5;
            break;
        case HID_KEY_6:
            hid_keycodes[i] = HID_KEY_F6;
            break;
        case HID_KEY_7:
            hid_keycodes[i] = HID_KEY_F7;
            break;
        case HID_KEY_8:
            hid_keycodes[i] = HID_KEY_F8;
            break;
        case HID_KEY_9:
            hid_keycodes[i] = HID_KEY_F9;
            break;
        case HID_KEY_0:
            hid_keycodes[i] = HID_KEY_F10;
            break;
        case HID_KEY_MINUS:
            hid_keycodes[i] = HID_KEY_F11;
            break;
        case HID_KEY_EQUAL:
            hid_keycodes[i] = HID_KEY_F12;
            break;
        case HID_KEY_INSERT:
            hid_keycodes[i] = HID_KEY_PRINT_SCREEN;
            break;
        default:
            break;
        }
    }
}
#endif

int key_parse(uint8_t *key_map, uint8_t num, uint8_t key8[8], uint8_t key16[16])
{
    uint8_t current_key[120] = {0};
    static uint8_t last_key_8[8] = {0};
    static uint8_t last_key_16[16] = {0};
    uint8_t key8_num = 0, key16_num = 0;

    keymap_to_keybuf8(key_map, current_key, num);
    ModifierKeyHandler(current_key, key8, num);
    FnHidKeyHandler(current_key, num);

    uint8_t remain_num = CompactIntegers(current_key, num);

    /* 对比上次传输的键值，若此次仍存在，则使用上次的传输方式 */
    for(int key_idx = 0; key_idx < remain_num; key_idx++) {
        for(int key_8_idx = 2; key_8_idx < 8; key_8_idx++) {
            if(current_key[key_idx] && 
                    (last_key_8[key_8_idx] == current_key[key_idx])) {
                key8[key8_num + 2] = current_key[key_idx];
                current_key[key_idx] = 0;
                key8_num++;
            }

            struct key16_type key16_temp = key8_to_key16(current_key[key_idx]);
            if(current_key[key_idx] && 
                    (last_key_16[key16_temp.index] & key16_temp.val)) {
                key16[key16_temp.index] |= key16_temp.val;
                current_key[key_idx] = 0;
                key16_num++;
            }
        }
    }

    /* 获取剩余的键值， 并加载 */
    remain_num = CompactIntegers(current_key, remain_num);

    for(int i = key8_num + 2, j = 0; i < 8; i++) {
        if(j > remain_num - 1)
            break;

        key8[i] = current_key[j];
        current_key[j] = 0;
        key8_num++;
        j++;
    }

    remain_num = CompactIntegers(current_key, remain_num);

    for(int i = 0; i < remain_num; i++){
        struct key16_type key16_temp = key8_to_key16(current_key[i]);
        key16[key16_temp.index] |= key16_temp.val;
        current_key[i] = 0;
        key16_num++;
    }

    remain_num = CompactIntegers(current_key, remain_num);

    memcpy(last_key_8, key8, 8);
    memcpy(last_key_16, key16, 16);

    if(remain_num)
        return -1;
    
    return 0;
}
