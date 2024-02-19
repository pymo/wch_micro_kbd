#ifndef __USB_KBD_H
#define __USB_KBD_H

#include <stdint.h>
#include "USB/usbuser.h"

#define USB_IN_EVT              0x0001
#define USB_TEST_EVT            0x0008

extern uint8_t USBTaskID;

void USB_Task_Init(void);
void USB_Task_Uinit(void);

#endif
