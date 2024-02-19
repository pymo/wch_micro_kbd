/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __KEY_SCAN_H
#define __KEY_SCAN_H

#include <stdbool.h>
#include <stdint.h>

#define Key_S0                      (R32_PA_PIN&GPIO_Pin_0)
#define Key_S1                      (R32_PA_PIN&GPIO_Pin_1)
#define Key_S2                      (R32_PA_PIN&GPIO_Pin_2)
#define Key_S3                      (R32_PA_PIN&GPIO_Pin_3)
#define Key_S4                      (R32_PA_PIN&GPIO_Pin_4)
#define Key_S5                      (R32_PA_PIN&GPIO_Pin_5)

#define LONGKEY_TIME                800

void keyInit(void);
int readKeyVal(void);
void RstAllPins(void);
void SetAllPins(void);

#endif
