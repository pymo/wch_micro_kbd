/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_KEY_SCAN_KEY_TABLE_H
#define DRIVERS_KEY_SCAN_KEY_TABLE_H

#include <stdint.h>
#include "key_codes.h"

// The maximum number of keys on the keyboard, it can be larger than the actual key number.
#define MAX_KEY_NUM 70

#define KEEPALIVE_TIMEOUT_MS        500000
#define LONGKEY_TIME                1000
#define INIT_FAILURE_TIMEOUT_MS     (3000) //3s
#define KEYBOARD_REBOOT_DELAY_MS_LONG 600
#define KEYBOARD_REBOOT_DELAY_MS_SHORT 100
#define KEYSCAN_NOT_READY 0xff  // return value of the scanned key number to indicate the scan is not ready yet.
#define KEY_NOT_FOUND 0xff  // return value of finding keycode in a list to represent not found.

extern uint16_t consumer_key_table[];
extern const uint8_t ascii_to_hid_table[];
extern uint32_t last_pressed;

typedef enum{
    K_STATE_0_START,
    K_STATE_1_OFF,
    K_STATE_2_ON,
    K_STATE_3_DCD_RESPONDED,
    K_STATE_4_RTS_LOW,
    K_STATE_5_RTS_HIGH,
    K_STATE_6_ID_RECEIVED,
}KeyboardBootState;

typedef enum
{
  KEY_LAYER_ORIGINAL = 0,
  KEY_LAYER_FN = 1,
  KEY_LAYER_FN2 = 2,
  KEY_LAYER_FN3 = 3,
  KEY_LAYER_SHIFT = 4
}KeyLayerType;

typedef enum
{
  KEY_SENT_NOTYET = 0,
  KEY_SENT_8 = 1,
  KEY_SENT_16 = 2
}KeySendType;

struct KeyWithLayer {
    uint8_t key;
    KeyLayerType layer;
    KeySendType sent;  // The last time this key is sent through.
};


uint8_t CompactIntegers(uint8_t *buf, uint8_t len);
uint8_t CompactKeyLayer(struct KeyWithLayer *buf, uint8_t len);
uint8_t FindKeycode(struct KeyWithLayer* key_list, uint8_t key_num, uint8_t key_code);
void keyPress(uint8_t *pbuf, uint8_t *key_num, uint8_t raw_keycode);
void keyRelease(uint8_t *pbuf, uint8_t *key_num, uint8_t raw_keycode);
void keyReset(uint8_t *pbuf, uint8_t *key_num);
uint8_t AsciiToHidCode(uint8_t ascii);

#endif /* DRIVERS_KEY_SCAN_KEY_TABLE_H */

