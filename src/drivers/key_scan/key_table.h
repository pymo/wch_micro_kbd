/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_KEY_SCAN_KEY_TABLE_H
#define DRIVERS_KEY_SCAN_KEY_TABLE_H

#include <stdint.h>
#include "key_codes.h"

// The maximum number of keys on the keyboard, it can be larger than the actual key number. recommend 120.
#define KEY_MAP_SIZE 120

extern uint8_t fn_key_table[];
extern uint16_t consumer_key_table[];
extern const uint8_t key8_table[];
extern const uint8_t ascii_to_hid_table[];
extern uint8_t key8_table_size;

void init_fn_key_table();
uint8_t AsciiToHidCode(uint8_t ascii);

#endif /* DRIVERS_KEY_SCAN_KEY_TABLE_H */

