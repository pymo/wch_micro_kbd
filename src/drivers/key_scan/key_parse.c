/*
 * Copyright (c) 2022 zerosensei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "key_parse.h"
#include "key_table.h"
#include "CH58x_common.h"
#include "led_task/led_task.h"

bool key_fn_flag;

void raw_to_hid_keycode(uint8_t *index, uint8_t *keyVal, uint8_t len)
{
    for (int i = 0, idx = 0; i < len; i++)
    {
        if (index[i]>key8_table_size){
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
    for (int i = new_len; i < len; i++) {
        buf[i] = 0;
    }

    return new_len;
}

void ModifierKeyHandler(uint8_t *hid_keycodes, uint8_t *keys, uint8_t nums)
{
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
        default:
            break;
        }
    }
}

// If Fn+key is mapped to another key, replace it here.
// The result key may not be an simple HID keycode, it's OK, we will replace it in key_special.c
void FnKeyHandler(uint8_t *hid_keycodes, uint8_t nums){
    if (!key_fn_flag) return;
    for (uint8_t i = 0; i < nums; ++i)
    {
        uint8_t layer2_key = fn_key_table[hid_keycodes[i]];
        if (layer2_key!=0){
            hid_keycodes[i] = layer2_key;
        }
    }
}

void NumlockKeyHandler(uint8_t *hid_keycodes, uint8_t nums){
    if (!get_led_num()) return;
    for (uint8_t i = 0; i < nums; ++i)
    {
        switch(hid_keycodes[i]){
        case HID_KEY_M:
            hid_keycodes[i] = HID_KEY_KEYPAD_0;
            break;
        case HID_KEY_J:
            hid_keycodes[i] = HID_KEY_KEYPAD_1;
            break;
        case HID_KEY_K:
            hid_keycodes[i] = HID_KEY_KEYPAD_2;
            break;
        case HID_KEY_L:
            hid_keycodes[i] = HID_KEY_KEYPAD_3;
            break;
        case HID_KEY_U:
            hid_keycodes[i] = HID_KEY_KEYPAD_4;
            break;
        case HID_KEY_I:
            hid_keycodes[i] = HID_KEY_KEYPAD_5;
            break;
        case HID_KEY_O:
            hid_keycodes[i] = HID_KEY_KEYPAD_6;
            break;
        case HID_KEY_7:
            hid_keycodes[i] = HID_KEY_KEYPAD_7;
            break;
        case HID_KEY_8:
            hid_keycodes[i] = HID_KEY_KEYPAD_8;
            break;
        case HID_KEY_9:
            hid_keycodes[i] = HID_KEY_KEYPAD_9;
            break;
        case HID_KEY_PERIOD:
            hid_keycodes[i] = HID_KEY_KEYPAD_DECIMAL;
            break;
        case HID_KEY_SLASH:
            hid_keycodes[i] = HID_KEY_KEYPAD_DIVIDE;
            break;
        case HID_KEY_SEMICOLON:
            hid_keycodes[i] = HID_KEY_KEYPAD_ADD;
            break;
        case HID_KEY_P:
            hid_keycodes[i] = HID_KEY_KEYPAD_SUBTRACT;
            break;
        case HID_KEY_0:
            hid_keycodes[i] = HID_KEY_KEYPAD_MULTIPLY;
            break;
        }
    }
}

bool is_fn_pressed(uint8_t *hid_keycodes, uint8_t num){
    for (uint8_t i = 0; i < num; ++i)
    {
        if (hid_keycodes[i] == HID_KEY_FN){
            hid_keycodes[i] = 0;
            return true;
        }
    }
    return false;
}

int key_parse(uint8_t *raw_key_codes, uint8_t num, uint8_t hid_key8[8], uint8_t hid_key16[16])
{
    uint8_t current_key[120] = {0};
    static uint8_t last_key_8[8] = {0};
    static uint8_t last_key_16[16] = {0};
    uint8_t key8_num = 0, key16_num = 0;

    raw_to_hid_keycode(raw_key_codes, current_key, num);

    // Set key_fn_flag to true when fn is pressed. Set key_fn_flag to false when all keys are released.
    // This is to handle the case where user release the Fn key first, then the other key later. (Assume user's intent is to release all keys)
    if(is_fn_pressed(current_key, num))
        key_fn_flag = true;
    if(num==0)
        key_fn_flag = false;

    FnKeyHandler(current_key, num);
#ifdef ENABLE_NUMLOCK
    NumlockKeyHandler(current_key, num);
#endif
    ModifierKeyHandler(current_key, hid_key8, num);

    uint8_t remain_num = CompactIntegers(current_key, num);


    /* 对比上次传输的键值，若此次仍存在，则使用上次的传输方式 */
    for(int key_idx = 0; key_idx < remain_num; key_idx++) {
        for(int key_8_idx = 2; key_8_idx < 8; key_8_idx++) {
            if(current_key[key_idx] && 
                    (last_key_8[key_8_idx] == current_key[key_idx])) {
                hid_key8[key8_num + 2] = current_key[key_idx];
                current_key[key_idx] = 0;
                key8_num++;
            }

            struct key16_type key16_temp = key8_to_key16(current_key[key_idx]);
            if(current_key[key_idx] && 
                    (last_key_16[key16_temp.index] & key16_temp.val)) {
                hid_key16[key16_temp.index] |= key16_temp.val;
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

        hid_key8[i] = current_key[j];
        current_key[j] = 0;
        key8_num++;
        j++;
    }

    remain_num = CompactIntegers(current_key, remain_num);

    for(int i = 0; i < remain_num; i++){
        struct key16_type key16_temp = key8_to_key16(current_key[i]);
        hid_key16[key16_temp.index] |= key16_temp.val;
        current_key[i] = 0;
        key16_num++;
    }

    remain_num = CompactIntegers(current_key, remain_num);

    memcpy(last_key_8, hid_key8, 8);
    memcpy(last_key_16, hid_key16, 16);

    if(remain_num)
        return -1;
    
    return 0;
}
