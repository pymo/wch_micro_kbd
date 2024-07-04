/*
 * Copyright (c) 2024 Xinming Chen
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <button_task/button_task.h>
#include "soc.h"
#include "device_config.h"
#include "mode/mode.h"

uint8_t button_taskid = 0;

// Called every 150ms
void handle_button_state(){
    static int consecutive_button_down_count = 0;
    if (!GPIOB_ReadPortPin(BUTTON_PAIR)) { // Button pressed, GPIO low
        consecutive_button_down_count++;
    } else {
        consecutive_button_down_count = 0;
    }
    if(consecutive_button_down_count*BUTTON_READ_INTERVAL_MS>BUTTON_LONG_PRESS_MS){
        PRINT("Pair button long press, clear Data Flash.\n");
        consecutive_button_down_count = 0;
        bool success = EEPROM_ERASE(0, EEPROM_MAX_SIZE);
        PRINT("Erase Data Flash %s.\n", (success==0)?"success":"fail");
        SYS_ResetExecute();
    }
}

uint16_t button_task_event(uint8_t task_id, uint16_t event)
{
    if (event & BUTTON_UPDATE_EVENT) {
        // Reads the state of button here
        handle_button_state();
        tmos_start_task(button_taskid, BUTTON_UPDATE_EVENT, MS1_TO_SYSTEM_TIME(BUTTON_READ_INTERVAL_MS));
        return (event ^ BUTTON_UPDATE_EVENT); //异或的方式清除该事件运行标志，并返回未运行的事件标志
    }
    // Discard unknown events
    return 0;
}

void button_task_init(void)
{
    GPIOB_ModeCfg(BUTTON_PAIR, GPIO_ModeIN_PU);

    button_taskid = TMOS_ProcessEventRegister(button_task_event);
    tmos_set_event(button_taskid,BUTTON_UPDATE_EVENT);
}
