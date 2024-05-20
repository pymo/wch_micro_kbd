/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __KEY_SCAN_H
#define __KEY_SCAN_H

#include <stdbool.h>
#include <stdint.h>

#define DCD_PIN                     (GPIO_Pin_11)  // GPIOA
#define RTS_PIN                     (GPIO_Pin_12)  // GPIOA
#define VCC_PIN                     (GPIO_Pin_15)  // GPIOB

#define LONGKEY_TIME                800

void keyInit(void);
int readKeyVal(void);
void RstAllPins(void);
void SetAllPins(void);

#endif
