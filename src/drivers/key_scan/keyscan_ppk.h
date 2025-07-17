/*
 * keyscan_ppk.h
 *
 *  Created on: Jun 26, 2025
 *      Author: chen_xin_ming
 */

#ifdef KEYBOARD_TYPE_PPK

#ifndef __KEY_SCAN_PPK_H
#define __KEY_SCAN_PPK_H

#include <stdint.h>
#include "key_util.h"

#define DCD_PIN                     (GPIO_Pin_14)  // GPIOB
#define RTS_PIN                     (GPIO_Pin_15)  // GPIOB
#define VCC_PIN                     (GPIO_Pin_14)  // GPIOA

#ifdef PPK_TYPE_HANDSPRING
#define KEYBOARD_ID0 0xF9
#define KEYBOARD_ID1 0xFB
#else
#define KEYBOARD_ID0 0xFA
#define KEYBOARD_ID1 0xFD
#endif

extern const uint8_t key8_table[];
extern uint8_t key8_table_size;

void InitKeyboard(KeyboardBootState* k_state, uint32_t ms_current, uint32_t ms_last_state_change);
KeyLayerType GetKeyLayer(uint8_t *raw_keycodes, uint8_t raw_keycode_len);
void PreprocessKeyList(struct KeyWithLayer* key_list, uint8_t* key_num);
uint8_t ConvertLayeredKey(struct KeyWithLayer* key_list, uint8_t key_num, uint8_t key_index);
uint8_t ScanRawKeycodes(uint8_t *raw_keys);

#endif // __KEY_SCAN_ULTRATHIN_H
#endif
