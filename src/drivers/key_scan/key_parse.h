/*
 * Copyright (c) 2022 zerosensei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_KEY_SCAN_KEY_PARSE_H
#define DRIVERS_KEY_SCAN_KEY_PARSE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct key16_type {
    uint8_t index;
    uint8_t val;
};

struct KeyWithLayer {
    uint8_t key;
    uint8_t layer;
};

int key_parse(uint8_t *raw_key_codes, uint8_t num, uint8_t hid_key8[8], uint8_t hid_key16[16]);
uint8_t CompactIntegers(uint8_t *buf, uint8_t len);

#endif /* DRIVERS_KEY_SCAN_KEY_PARSE_H */
