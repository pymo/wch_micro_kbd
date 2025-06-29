/*
 * keyscan_ultrathin.h
 *
 *  Created on: Apr 26, 2025
 *      Author: chen_xin_ming
 */

#ifndef __KEY_SCAN_ULTRATHIN_H
#define __KEY_SCAN_ULTRATHIN_H

#include <stdint.h>

#define KEYSCAN_NOT_READY 0xff

void InitScanPins(void);
uint8_t ScanKeyUltraThin(uint8_t *current_key_map);

#endif // __KEY_SCAN_ULTRATHIN_H
