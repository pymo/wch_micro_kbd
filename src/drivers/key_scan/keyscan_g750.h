/*
 * keyscan_ppk.h
 *
 *  Created on: Jun 26, 2025
 *      Author: chen_xin_ming
 */

#ifdef KEYBOARD_TYPE_G750

#ifndef __KEY_SCAN_G750_H
#define __KEY_SCAN_G750_H

#include <stdint.h>
#include "key_util.h"

#define VCC_PIN                     (GPIO_Pin_14)  // GPIOA

extern const uint8_t key8_table[];
extern uint8_t key8_table_size;

void InitKeyboard(KeyboardBootState* k_state, uint32_t ms_current, uint32_t ms_last_state_change);
KeyLayerType GetKeyLayer(uint8_t *raw_keycodes, uint8_t raw_keycode_len);
void PreprocessKeyList(struct KeyWithLayer* key_list, uint8_t* key_num);
uint8_t ConvertLayeredKey(struct KeyWithLayer* key_list, uint8_t key_num, uint8_t key_index);
uint8_t ScanRawKeycodes(uint8_t *raw_keys);

#endif // __KEY_SCAN_G750_H
#endif
