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
#define KEEPALIVE_TIMEOUT_MS        500000
#define LONGKEY_TIME                1000
#define INIT_FAILURE_TIMEOUT_MS     (3000) //3s
#define KEYBOARD_REBOOT_DELAY_MS_LONG 600
#define KEYBOARD_REBOOT_DELAY_MS_SHORT 100

#ifdef PPK_TYPE_HANDSPRING
#define KEYBOARD_ID0 0xF9
#define KEYBOARD_ID1 0xFB
#else
#define KEYBOARD_ID0 0xFA
#define KEYBOARD_ID1 0xFD
#endif

typedef enum{
    K_STATE_0_START,
    K_STATE_1_OFF,
    K_STATE_2_ON,
    K_STATE_3_DCD_RESPONDED,
    K_STATE_4_RTS_LOW,
    K_STATE_5_RTS_HIGH,
    K_STATE_6_ID_RECEIVED,
}KEYBOARD_BOOT_STATE;

void key_loop(void); // executed every 10ms
int readKeyVal(void);
void RstAllPins(void);
void SetAllPins(void);

#endif
