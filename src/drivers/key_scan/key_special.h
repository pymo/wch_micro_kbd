/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_KEY_SCAN_KEY_SPECIAL_H
#define DRIVERS_KEY_SCAN_KEY_SPECIAL_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Special keys handler.
 * 
 * @param keys_8    Pointer to 8 bytes key.
 * 
 * @param is_long   True if it is a long keypress.
 * 
 * @return true     Remaining key values 
 * @return false    No key remaining
 */
bool special_key_handler(uint8_t *keys_8, bool is_long);


#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_KEY_SCAN_KEY_SPECIAL_H */