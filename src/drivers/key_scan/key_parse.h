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

int key_parse(uint8_t *key_map, uint8_t num, uint8_t key8[8], uint8_t key16[16]);

extern bool key_fn_flag;

#endif /* DRIVERS_KEY_SCAN_KEY_PARSE_H */
