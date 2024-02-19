/*
 * Copyright (c) 2022 zerosensei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "key_parse.h"
#include "key_table.h"
#include "CH58x_common.h"

bool key_fn_flag = false;

void keymap_to_keybuf8(uint8_t *index, uint8_t *keyVal, uint8_t len)
{
    for (int i = 0, idx = 0; i < len; i++)
    {
        if (!key8_table[index[i]])
            continue;
        keyVal[2 + idx++] = key8_table[index[i]];
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

bool hotkeydeal(uint8_t *index, uint8_t *keys, uint8_t nums)
{
    bool ret = false;
    for (int i = 0; i < nums; i++)
    {
        if (index[i] == 5) // shift-l
        {
            keys[0] |= key8_table[index[i]];
            index[i] = 0;
            ret = true;
        }
        if (index[i] == 82) // shift-r
        {
            keys[0] |= key8_table[index[i]];
            index[i] = 0;
            ret = true;
        }
        if (index[i] == 6) // ctrl-l
        {
            keys[0] |= key8_table[index[i]];
            index[i] = 0;
            ret = true;
        }
        if (index[i] == 13) // winl
        {
            keys[0] |= key8_table[index[i]];
            index[i] = 0;
            ret = true;
        }
        if (index[i] == 20) // altl
        {
            keys[0] |= key8_table[index[i]];
            index[i] = 0;
            ret = true;
        }
        if (index[i] == 34) // altr
        {
            keys[0] |= key8_table[index[i]];
            index[i] = 0;
            ret = true;
        }
        if (index[i] == 55) // winr
        {
            keys[0] |= key8_table[index[i]];
            index[i] = 0;
            ret = true;
        }
        if (index[i] == 62) // ctrr
        {
            keys[0] |= key8_table[index[i]];
            index[i] = 0;
            ret = true;
        }

        if (index[i] == 41) // Fn
        {
            // keys[0] = key8_table[index[i]];
            key_fn_flag = true;
            index[i] = 0;
        }
    }

    return ret;
}

int key_parse(uint8_t *key_map, uint8_t num, uint8_t key8[8], uint8_t key16[16])
{
    uint8_t current_key[120] = {0};
    static uint8_t last_key_8[8] = {0};
    static uint8_t last_key_16[16] = {0};
    uint8_t key8_num = 0, key16_num = 0;

    hotkeydeal(key_map, key8, num);
    keymap_to_keybuf8(key_map, current_key, num);

    uint8_t remain_num = CompactIntegers(current_key, sizeof(current_key));

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
