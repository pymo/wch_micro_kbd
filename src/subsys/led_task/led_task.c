/*
 * Copyright (c) 2024 Xinming Chen
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "led_task/led_task.h"
#include "soc.h"
#include "HAL/ADC.h"
#include "device_config.h"
#include "BLE/hidkbd.h"
#include "PM/pm_task.h"

uint8_t led_taskid = 0;

// High level system states that LED needs to represent
static bool led_numlock_on = false;
static bool led_capslock_on = false;
static bool led_scrolllock_on = false;  // scroll lock LED is not implemented
static bool is_keyboard_init = false;
static BluetoothState bluetooth_state = BLULETOOTH_OFF;

void led_on(uint32_t pin){
    GPIOB_SetBits(pin);
}

void led_off(uint32_t pin){
    GPIOB_ResetBits(pin);
}

bool led_mode_to_state(LedMode led_mode){
    uint32 tick = TMOS_GetSystemClock();
    uint32 ms_in_cycle;
    switch (led_mode) {
        case LED_OFF:
            return false;
        case LED_CONSTANT_ON:
            return true;
        // Flash cycle: 150ms. 50ms on, 100ms off
        case LED_FLASH:
            ms_in_cycle = (tick % 240)*SYSTEM_TIME_MICROSEN/1000; // cycle is 150ms, 150ms/625us=240
            return (ms_in_cycle < 50);
        // Flash cycle: 400ms. 200ms on, 200ms off
        case LED_ALT_FLASH_1:
            ms_in_cycle = (tick % 640)*SYSTEM_TIME_MICROSEN/1000; // cycle is 400ms, 400ms/625us=640
            return (ms_in_cycle < 200);
        case LED_ALT_FLASH_2:
            ms_in_cycle = (tick % 640)*SYSTEM_TIME_MICROSEN/1000; // cycle is 400ms, 400ms/625us=640
            return (ms_in_cycle >= 200);
        // Blink cycle = 3s
        //  |  On  |  Off  |  On  |  Off  |  On  |  Off  |  On  |  Off   |
        //  | 50ms | 150ms | 50ms | 150ms | 50ms | 150ms | 50ms | 2350ms |
        case LED_BLINK_1:
            ms_in_cycle = (tick % 4800)*SYSTEM_TIME_MICROSEN/1000; // cycle is 3s, 3s/625us=4800
            return (ms_in_cycle < 50);
        case LED_BLINK_2:
            ms_in_cycle = (tick % 4800)*SYSTEM_TIME_MICROSEN/1000; // cycle is 3s, 3s/625us=4800
            return (ms_in_cycle < 50) || (ms_in_cycle >= 200 && ms_in_cycle < 250);
        case LED_BLINK_3:
            ms_in_cycle = (tick % 4800)*SYSTEM_TIME_MICROSEN/1000; // cycle is 3s, 3s/625us=4800
            return (ms_in_cycle < 50) || (ms_in_cycle >= 200 && ms_in_cycle < 250) || (ms_in_cycle >= 400 && ms_in_cycle < 450);
        case LED_BLINK_4:
            ms_in_cycle = (tick % 4800)*SYSTEM_TIME_MICROSEN/1000; // cycle is 3s, 3s/625us=4800
            return (ms_in_cycle < 50) || (ms_in_cycle >= 200 && ms_in_cycle < 250) || (ms_in_cycle >= 400 && ms_in_cycle < 450) || (ms_in_cycle >= 600 && ms_in_cycle < 650);
        default:
            break;
    }
    return false;
}

void update_led_state(){
    LedMode led_yellow_mode = LED_OFF;
    LedMode led_green_mode = LED_OFF;
    LedMode led_blue_mode = LED_OFF;

    // Don't turn on LEDs during sleep.
    if (!pm_is_in_idle()){
        if (is_keyboard_init){
            led_yellow_mode = LED_CONSTANT_ON;
            led_green_mode = LED_CONSTANT_ON;
            led_blue_mode = LED_CONSTANT_ON;
        } else {
            if (led_capslock_on) led_green_mode = LED_CONSTANT_ON;

            uint8_t percentage = GetBatteryPercentage();
            //if (led_numlock_on) led_yellow_mode = LED_CONSTANT_ON; else
            if (percentage<10) led_yellow_mode = LED_BLINK_3;
            else if (percentage<15) led_yellow_mode = LED_BLINK_2;
            else if (percentage<20) led_yellow_mode = LED_BLINK_1;

            if (device_mode == MODE_USB || bluetooth_state == BLULETOOTH_OFF)
                led_blue_mode = LED_OFF;
            else if (bluetooth_state == BLULETOOTH_RECONNECTING)
                led_blue_mode = LED_FLASH;
            else if (bluetooth_state == BLULETOOTH_PAIRING){
                led_blue_mode = LED_ALT_FLASH_1;
                led_yellow_mode = LED_ALT_FLASH_2;
            }
            else if (bluetooth_state == BLULETOOTH_CONNECTED_1)
                led_blue_mode = LED_BLINK_1;
            else if (bluetooth_state == BLULETOOTH_CONNECTED_2)
                led_blue_mode = LED_BLINK_2;
            else if (bluetooth_state == BLULETOOTH_CONNECTED_3)
                led_blue_mode = LED_BLINK_3;
            else if (bluetooth_state == BLULETOOTH_CONNECTED_4)
                led_blue_mode = LED_BLINK_4;
        }
    }
    // Sets the value of LED
    if(led_mode_to_state(led_yellow_mode)) led_on(LED_YELLOW);
    else led_off(LED_YELLOW);
    if(led_mode_to_state(led_green_mode)) led_on(LED_GREEN);
    else led_off(LED_GREEN);
    if(led_mode_to_state(led_blue_mode)) led_on(LED_BLUE);
    else led_off(LED_BLUE);
}

uint16_t led_task_event(uint8_t task_id, uint16_t event)
{
    if (event & LED_UPDATE_EVENT) {
        // Sets the value of LED here
        update_led_state();
        tmos_start_task(led_taskid, LED_UPDATE_EVENT, MS1_TO_SYSTEM_TIME(50));
        return (event ^ LED_UPDATE_EVENT); //异或的方式清除该事件运行标志，并返回未运行的事件标志
    }
    // Discard unknown events
    return 0;
}

void led_task_init(void)
{
    //键盘状态灯配置
    led_off(LED_YELLOW);
    led_off(LED_GREEN);
    led_off(LED_BLUE);
    GPIOB_ModeCfg(LED_YELLOW, GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg(LED_GREEN, GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg(LED_BLUE, GPIO_ModeOut_PP_5mA);

    led_taskid = TMOS_ProcessEventRegister(led_task_event);
    tmos_set_event(led_taskid,LED_UPDATE_EVENT);
}

// Num lock is not handled on PPK because it does not have num key area.
void set_led_num(bool s) {
    led_numlock_on = s;
}

void set_led_cap(bool s) {
    led_capslock_on = s;
}

// Screen lock is not handled because there is not enough LED.
void set_led_scr(bool s) {
    led_scrolllock_on = s;
}

void set_keyboard_init(bool is_init){
    is_keyboard_init = is_init;
}

void set_bluetooth_indicator(BluetoothState state){
    bluetooth_state = state;
}
