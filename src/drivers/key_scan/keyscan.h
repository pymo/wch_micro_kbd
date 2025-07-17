/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __KEY_SCAN_H
#define __KEY_SCAN_H

#include <stdbool.h>
#include <stdint.h>

void key_loop(void); // executed every 10ms
void RstAllPins(void);
void SetAllPins(void);

#endif
