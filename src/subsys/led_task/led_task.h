/*
 * Copyright (c) 2024 Xinming Chen
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUBSYS_LED_TASK_LED_TASK_H
#define SUBSYS_LED_TASK_LED_TASK_H

#include "stdbool.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* EVENTS */
#define LED_UPDATE_EVENT          (1<<0)

/* LED color */
#define LED_RED GPIO_Pin_13  // GPIOB
#define LED_GREEN  GPIO_Pin_23  // GPIOB
#define LED_BLUE   GPIO_Pin_12  // GPIOB
#define BLUE_BLINK_OFF_THRESHOLD_MS 30000  // 30s

typedef enum{
    BLULETOOTH_OFF,
    BLULETOOTH_PAIRING,
    BLULETOOTH_RECONNECTING,
    BLULETOOTH_CONNECTED_1,
    BLULETOOTH_CONNECTED_2,
    BLULETOOTH_CONNECTED_3,
    BLULETOOTH_CONNECTED_4
}BluetoothState;

typedef enum{
    LED_OFF,
    LED_CONSTANT_ON,
    LED_FLASH,
    LED_ALT_FLASH_1,
    LED_ALT_FLASH_2,
    LED_BLINK_1,
    LED_BLINK_2,
    LED_BLINK_3,
    LED_BLINK_4
}LedMode;

void led_task_init(void);

void update_led_state(void);

// LED behavior:
// Blue LED: Bluetooth state / pairing.
//   - Alternating blink with Yellow LED: Pairing
//   - constant blink: reconnecting
//   - blink once: connected to host 1
//   - blink twice: connected to host 2
//   - blink three times: connected to host 3
// Green LED: Capslock
//   - constant on: Capslock on
// Red LED: Numlock / battery state
//   - constant on: Numlock on (overrides the battery indicator display)
//   - blink once: 20% battery
//   - blink twice: 15% battery
//   - blink three times: 10% battery
// Blue+Green+Red LED constant on: Keyboard initializing.

void set_led_num(bool s);
void set_led_cap(bool s);
void set_led_scr(bool s);

void set_keyboard_init(bool is_init);
void set_bluetooth_indicator(BluetoothState state);

bool get_led_num();

#ifdef __cplusplus
}
#endif

#endif /* SUBSYS_LED_TASK_LED_TASK_H */
