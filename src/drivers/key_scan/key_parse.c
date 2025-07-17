/*
 * Copyright (c) 2022 zerosensei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "key_parse.h"
#include "key_util.h"
#include "CH58x_common.h"
#include "led_task/led_task.h"

#include "keyscan_ultrathin.h"
#include "keyscan_ppk.h"
#include "keyscan_g750.h"

uint8_t RawToHidKeycodes(uint8_t *raw_keycodes, struct KeyWithLayer *keyVal, uint8_t len)
{
    KeyLayerType key_layer = GetKeyLayer(raw_keycodes, len);
    uint8_t idx = 0;
    for (int i = 0; i < len; i++)
    {
        if (raw_keycodes[i]>key8_table_size){
            PRINT("raw keycode %#x out of range!\n", raw_keycodes[i]);
            continue;
        }
        if (!key8_table[raw_keycodes[i]])
            continue;
        struct KeyWithLayer temp_key;
        temp_key.key = key8_table[raw_keycodes[i]];
        temp_key.layer = key_layer;
        temp_key.sent = KEY_SENT_NOTYET;
        keyVal[idx++] = temp_key;
    }
    return idx;
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

void LayeredKeyMergeWithLast(
        struct KeyWithLayer* current_key, uint8_t current_key_num,
        struct KeyWithLayer* last_key, uint8_t last_key_num)
{
    for(int cur_key_idx = 0; cur_key_idx < current_key_num; cur_key_idx++) {
        for(int last_key_idx = 0; last_key_idx < last_key_num; last_key_idx++) {
            if (current_key[cur_key_idx].key == last_key[last_key_idx].key){
                current_key[cur_key_idx] = last_key[last_key_idx];
            }
        }
    }
}

void ModifierKeyHandler(struct KeyWithLayer *hid_keycodes, uint8_t *keys, uint8_t nums)
{
    for (uint8_t i = 0; i < nums; ++i)
    {
        switch (ConvertLayeredKey(hid_keycodes, nums, i)){
        case HID_KEY_CONTROL_LEFT:
            keys[0] |= KEYBOARD_MODIFIER_LEFTCTRL;
            hid_keycodes[i].key = 0;
            break;
        case HID_KEY_SHIFT_LEFT:
            keys[0] |= KEYBOARD_MODIFIER_LEFTSHIFT;
            hid_keycodes[i].key = 0;
            break;
        case HID_KEY_ALT_LEFT:
            keys[0] |= KEYBOARD_MODIFIER_LEFTALT;
            hid_keycodes[i].key = 0;
            break;
        case HID_KEY_GUI_LEFT:
            keys[0] |= KEYBOARD_MODIFIER_LEFTGUI;
            hid_keycodes[i].key = 0;
            break;
        case HID_KEY_CONTROL_RIGHT:
            keys[0] |= KEYBOARD_MODIFIER_RIGHTCTRL;
            hid_keycodes[i].key = 0;
            break;
        case HID_KEY_SHIFT_RIGHT:
            keys[0] |= KEYBOARD_MODIFIER_RIGHTSHIFT;
            hid_keycodes[i].key = 0;
            break;
        case HID_KEY_ALT_RIGHT:
            keys[0] |= KEYBOARD_MODIFIER_RIGHTALT;
            hid_keycodes[i].key = 0;
            break;
        case HID_KEY_GUI_RIGHT:
            keys[0] |= KEYBOARD_MODIFIER_RIGHTGUI;
            hid_keycodes[i].key = 0;
            break;
        default:
            break;
        }
    }
}

int key_parse(uint8_t *raw_key_codes, uint8_t raw_num, uint8_t hid_key8[8], uint8_t hid_key16[16])
{
    struct KeyWithLayer current_key[MAX_KEY_NUM];
    static struct KeyWithLayer last_key[MAX_KEY_NUM];
    static uint8_t last_key_num = 0;
    uint8_t key8_num = 0;
    uint8_t num = 0;

    num = RawToHidKeycodes(raw_key_codes, current_key, raw_num);
    LayeredKeyMergeWithLast(current_key, num, last_key, last_key_num);
    // Adds a Shift for the FN2 if necessary
    PreprocessKeyList(current_key, &num);
#ifdef ENABLE_NUMLOCK
    NumlockKeyHandler(current_key, num);
#endif
    ModifierKeyHandler(current_key, hid_key8, num);

    num = CompactKeyLayer(current_key, num);


    /* 对比上次传输的键值，若此次仍存在，则使用上次的传输方式 */
    for(int key_idx = 0; key_idx < num; key_idx++) {
        if (current_key[key_idx].sent==KEY_SENT_8){
            hid_key8[key8_num + 2] = ConvertLayeredKey(current_key,num,key_idx);
            key8_num++;
        } else if (current_key[key_idx].sent==KEY_SENT_16) {
            struct key16_type key16_temp = key8_to_key16(ConvertLayeredKey(current_key,num,key_idx));
            hid_key16[key16_temp.index] |= key16_temp.val;
        }
/*        for(int key_8_idx = 2; key_8_idx < 8; key_8_idx++) {
            if(current_key[key_idx].key > 0 &&
                    (last_key_8[key_8_idx].key == current_key[key_idx].key)) {
                current_key[key_idx].layer = last_key_8[key_8_idx].layer;
                hid_key8[key8_num + 2] = LayeredKeyHandler(current_key[key_idx]);
                current_key[key_idx].key = 0;
                key8_num++;
            }

            struct key16_type key16_temp = key8_to_key16(current_key[key_idx]);
            if(current_key[key_idx].key > 0 &&
                    (last_key_16[key16_temp.index] & key16_temp.val)) {
                hid_key16[key16_temp.index] |= key16_temp.val;
                current_key[key_idx] = 0;
                key16_num++;
            }
        }*/
    }

    /* 获取剩余的键值， 并加载 */

    //遍历未发送的键值，8未满则通过8发，8满则通过16发
    for(int key_idx = 0; key_idx < num; key_idx++) {
        if (current_key[key_idx].sent!=KEY_SENT_NOTYET) continue;
        if (key8_num < 6){
            hid_key8[key8_num + 2] = ConvertLayeredKey(current_key,num,key_idx);
            current_key[key_idx].sent = KEY_SENT_8;
            key8_num++;
        } else {
            struct key16_type key16_temp = key8_to_key16(ConvertLayeredKey(current_key,num,key_idx));
            hid_key16[key16_temp.index] |= key16_temp.val;
            current_key[key_idx].sent = KEY_SENT_16;
        }
    }

/*

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
*/
    last_key_num = num;
    memcpy(last_key, current_key, sizeof(struct KeyWithLayer)*num);

    return 0;
}
