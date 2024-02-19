/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc.h"
#include "HAL.h"
#include "device_config.h"
#include "mode.h"
#include "BLE/hidkbd.h"

uint8_t mode_ctrl_taskid = 0;
static uint8_t mode = 0;
uint16_t mode_ctrl_event(uint8_t task_id, uint16_t event)
{
    
    if (event & MODE_CTRL_SWITCH_EVENT) {

        Mode_Init(mode);

        /* Must return 0 */
        return 0;
    }

    return 0;
}

void mode_ctrl_init(void)
{
    mode_ctrl_taskid = TMOS_ProcessEventRegister(mode_ctrl_event);
}

void mode_select(uint8_t mod)
{
    mode = mod;

    Disconnet_conn();

    /* Wait for ble disconnet */
    tmos_start_task(mode_ctrl_taskid, MODE_CTRL_SWITCH_EVENT, 
            MS1_TO_SYSTEM_TIME(500));
}
