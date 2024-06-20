/*
 * Copyright (c) 2022 zerosensei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pm_task.h"
#include "device_config.h"
#include "key_scan/keyscan.h"
#include "USB_task/USB_kbd.h"
#include "led_task/led_task.h"
#include "HAL/HAL.h"
#include "HAL/ADC.h"
#include "BLE/hidkbd.h"

uint8_t pm_task_id = 0;
u8 pm_is_idle = false;

#define CONFIG_PM_DBG

#ifdef CONFIG_PM_DBG
#define PM_DBG  PRINT
#else
#define PM_DBG(...)
#endif

#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)

static void pm_tmos_msg(tmos_event_hdr_t *pMsg)
{
    switch (pMsg->event) {

    default:
        break;
    }
}

u8 pm_is_in_idle(void)
{
    return pm_is_idle;
}

void pm_start_working(int working_timeout, int idle_timeout)
{
    PM_DBG("Enter pm working\n");
    pm_is_idle = false;

    if(working_timeout >= 0) {
        if (GetChargingState()) working_timeout = PM_TIMEOUT_FOREVER;
        tmos_start_task(pm_task_id, PM_ENTER_IDLE_EVENT, MS1_TO_SYSTEM_TIME(working_timeout));

        if(idle_timeout >=0) {
            if (GetChargingState()) idle_timeout = PM_TIMEOUT_FOREVER;
            tmos_start_task(pm_task_id, PM_ENTER_STANDBY_EVENT, MS1_TO_SYSTEM_TIME(idle_timeout));
        } else {
            //PM_DBG("Warning: invalid idle_time! \n");
        }
    }
}

void pm_goto_standby(void)
{
    PM_DBG("Enter PM standby!\n");
    DelayMs(10);

    usb_disable();
    RstAllPins();
    PFIC_EnableIRQ( GPIO_B_IRQn );
    LowPower_Shutdown(0);
}

uint16_t pm_event(uint8_t task_id, uint16_t events)
{
    if (events & SYS_EVENT_MSG) {
        tmos_event_hdr_t *pMsg;

        if ((pMsg = (tmos_event_hdr_t *)tmos_msg_receive(pm_task_id)) != NULL) {
            pm_tmos_msg((tmos_event_hdr_t *) pMsg);
            // Release the TMOS message
            tmos_msg_deallocate((uint8_t *)pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }



    if(events & PM_ENTER_IDLE_EVENT) {
        PM_DBG("Enter pm idle\n");

        if(pm_is_idle==2){

        }else{
            pm_is_idle = true;
        }
        if(device_mode == MODE_BLE)
            OnBoard_SendMsg(hidEmuTaskId, RF_MS_STATE_CHANGE, PM_STATE_SLEEP, NULL);

        return (events ^ PM_ENTER_IDLE_EVENT);
    }

    if(events & PM_ENTER_STANDBY_EVENT) {
        pm_goto_standby();

        return (events ^ PM_ENTER_STANDBY_EVENT);
    }

    return 0;
}

void pm_task_init(void)
{
    pm_task_id = TMOS_ProcessEventRegister(pm_event);
    if(device_mode == MODE_USB || device_mode == MODE_TSET){
        pm_start_working(PM_TIMEOUT_FOREVER, PM_TIMEOUT_FOREVER);
    } else {
        pm_start_working(PM_WORKING_TIMEOUT, PM_IDLE_TIMEOUT);
    }
}
#else
void pm_task_init(void)
{

}
u8 pm_is_in_idle(void)
{

    return false;
}

void pm_start_working(int working_timeout, int idle_timeout)
{

}
void pm_goto_standby(void)
{

}
#endif /* HAL_SLEEP */
