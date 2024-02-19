#include "CH58x_common.h"
#include "USB_kbd.h"
#include "config.h"
#include "HAL.h"
#include "device_config.h"

uint8_t USBTaskID = INVALID_TASK_ID;

static void USB_ProcessTMOSMsg(tmos_event_hdr_t *pMsg)
{

    switch (pMsg->event) {
    case KEY_MESSAGE: {
        SendMSG_t *msg = (SendMSG_t *)pMsg;
        msg->hdr.status?tmos_set_event(USBTaskID, USB_IN_EVT):0;
        break;
    }
    default:
        break;
    }
}

uint16_t USB_ProcessEvent(uint8 task_id, uint16 events)
{

    if (events & SYS_EVENT_MSG) {
        uint8 *pMsg;

        if ((pMsg = tmos_msg_receive(USBTaskID)) != NULL) {
            USB_ProcessTMOSMsg((tmos_event_hdr_t *) pMsg);
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & USB_IN_EVT){
        if(DevEPn_IN_Deal())
            tmos_start_task(USBTaskID, USB_IN_EVT, MS1_TO_SYSTEM_TIME(100));
        return (events ^ USB_IN_EVT);
    }

    return 0;
}

void USB_Task_Init(void)
{
    USBTaskID = TMOS_ProcessEventRegister(USB_ProcessEvent);
    USB_Init();

    /* just for test */
//    tmos_start_task(USBTaskID, USB_TEST_EVT, MS1_TO_SYSTEM_TIME(1000));
}

void USB_Task_Uinit(void)
{
    USBTaskID = 0;
    usb_disable();
}
