/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "key_task.h"
#include "config.h"
#include "device_config.h"
#include "HAL/HAL.h"

uint8_t key_task_id = 0;
uint32_t gpio_b_interrupt_count = 0;
uint32_t last_gpio_b_interrupt_count = 0;

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
        key_loop();
        tmos_start_task(key_task_id, KEY_SCAN_EVENT, MS1_TO_SYSTEM_TIME(10));
        return (events ^ KEY_SCAN_EVENT);
    }


    return 0;
}

void keyscan_task_init(void)
{
    key_task_id = TMOS_ProcessEventRegister(key_event);
    tmos_start_task(key_task_id, KEY_SCAN_EVENT, MS1_TO_SYSTEM_TIME(10) );
}

__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler(void){
    gpio_b_interrupt_count++;
   // SetSysClock(CLK_SOURCE_PLL_60MHz);

    /* Wait for flash to stabilize */
    DelayUs(150);
    GPIOB_ClearITFlagBit(GPIO_Pin_4);
}
