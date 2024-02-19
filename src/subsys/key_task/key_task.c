/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "key_task.h"
#include "config.h"
#include "device_config.h"
#include "BLE/hidkbd.h"
#include "USB_task/USB_kbd.h"
#include "RF_task/rf_dev.h"
#include "HAL/HAL.h"
#include "PM/pm_task.h"

uint8_t key_task_id = 0;
static uint8_t no_key_count;

int key_deal(void)
{
    int ret = readKeyVal();
    if(ret > 0) {
        no_key_count = 0;

        switch(device_mode){
        case MODE_BLE:
            pm_start_working(PM_WORKING_TIMEOUT, PM_IDLE_TIMEOUT);
            OnBoard_SendMsg(hidEmuTaskId, RF_MS_STATE_CHANGE, PM_STATE_ACTIVE, NULL);
            OnBoard_SendMsg(hidEmuTaskId, KEY_MESSAGE, 1, NULL);
            break;

        case MODE_RF24:
            pm_start_working(PM_WORKING_TIMEOUT, PM_IDLE_TIMEOUT);
            OnBoard_SendMsg(rf_dev_taskid, KEY_MESSAGE, 1, NULL);
            break;

        case MODE_USB:
            /* USB don't sleep */
            pm_start_working(PM_TIMEOUT_FOREVER, PM_TIMEOUT_FOREVER);
            OnBoard_SendMsg(USBTaskID, KEY_MESSAGE, 1, NULL);
            break;
            
        default:
            pm_start_working(PM_TIMEOUT_FOREVER, PM_TIMEOUT_FOREVER);
            break;
        }
    } else if(ret == 0) {
        /* no key */
        no_key_count++;
        
        if(no_key_count > 50) {
            no_key_count = 50;
            
            if(pm_is_in_idle())
            {
                //PRINT("keyscan pause\n");
                tmos_stop_task(key_task_id, KEY_SCAN_EVENT);
                return 0;
            }
        } 
    } else {
        //PRINT("read key error\n");
        return -1;
    }

    tmos_start_task(key_task_id, KEY_SCAN_EVENT, MS1_TO_SYSTEM_TIME(10));
    return 1;
}

static void key_tmos_msg(tmos_event_hdr_t *pMsg)
{
    switch (pMsg->event) {
      case KEY_PRESSED:
        tmos_start_task(key_task_id, KEY_SCAN_EVENT, 0);
      break;

    default:
        break;
    }
}

uint16_t key_event(uint8_t task_id, uint16_t events)
{
    if (events & SYS_EVENT_MSG) {
        tmos_event_hdr_t *pMsg;

        if ((pMsg = (tmos_event_hdr_t *)tmos_msg_receive(key_task_id)) != NULL) {
            key_tmos_msg((tmos_event_hdr_t *) pMsg);
            // Release the TMOS message
            tmos_msg_deallocate((uint8_t *)pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & KEY_SCAN_EVENT) {
        
        int ret = key_deal();

        if(ret < 0) {
            tmos_start_task(key_task_id, KEY_SCAN_EVENT, MS1_TO_SYSTEM_TIME(10));
        }
 
        return (events ^ KEY_SCAN_EVENT);
    }


    return 0;
}

void keyscan_task_init(void)
{
    key_task_id = TMOS_ProcessEventRegister(key_event);
    tmos_start_task(key_task_id, KEY_SCAN_EVENT, MS1_TO_SYSTEM_TIME(10) );
}

__HIGH_CODE
void GPIOA_IRQHandler(void){
    no_key_count = 0;
   // SetSysClock(CLK_SOURCE_PLL_60MHz);

    /* Wait for flash to stabilize */
    DelayUs(150);
    GPIOA_ClearITFlagBit(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
}
