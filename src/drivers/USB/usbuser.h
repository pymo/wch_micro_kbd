#ifndef __USBUSER_H__
#define __USBUSER_H__

#include <stdint.h>
#include <stdbool.h>
#include "usbdesc.h"
#include "hid.h"

/* nUsb_Status Register Definitions */
#define mskBUSRESET                     (0x1<<0)
#define mskBUSSUSPEND                   (0x1<<1)
#define mskBUSRESUME                    (0x1<<2)
#define mskREMOTEWAKEUP                 (0x1<<3)
#define mskSETCONFIGURATION0CMD         (0x1<<4)
#define mskSETADDRESS                   (0x1<<5)
#define mskSETADDRESSCMD                (0x1<<6)
#define mskREMOTE_WAKEUP                (0x1<<7)
#define mskDEV_FEATURE_CMD              (0x1<<8)
#define mskSET_REPORT_FLAG              (0x1<<9)
#define mskPROTOCOL_GET_REPORT          (0x1<<10)
#define mskPROTOCOL_SET_IDLE            (0x1<<11)
#define mskPROTOCOL_ARRIVAL             (0x1<<12)
#define mskSET_REPORT_DONE              (0x1<<13)
#define mskNOT_8BYTE_ENDDING            (0x1<<14)
#define mskSETUP_OUT                    (0x1<<15)
#define mskSETUP_IN                     (0x1<<16)
#define mskINITREPEAT                   (0x1<<17)
#define mskREMOTE_WAKEUP_ACT            (0x1<<18)
#define mskSET_REPORT1_FLAG             (0x1<<18)
#define mskSET_UserOut_FLAG             (0x1<<19)


#define KEYNORMAL_ID    0
#define KEYBIT_ID       1
#define SYS_ID          2
#define CONSUME_ID      3
#define VENDOR_ID       4
#define MOUSE_ID        5


#define KB_EP           1
#define FUNC_EP         2

typedef struct
{

    volatile uint8_t    wHID_SetRptByte[16];
    volatile uint8_t    wHID_IdleTimeIf0ID;
    volatile uint8_t    wHID_Protocol;
    volatile uint8_t    wHID_SetReportFeature;
    volatile uint32_t   wHID_Setreportfeature[16];
    volatile uint32_t   wHID_Status;
}S_HID_DATA;


extern S_HID_DATA    sHID_Data;
extern volatile uint32_t wUSB_SetConfiguration;
extern uint8_t   bUSB_DeviceAddr;
extern volatile uint32_t    wUSB_Status;

bool DevEPn_IN_Deal(void);
void USB_Init(void);
void usb_disable(void);
bool is_host_sleep(void);

#endif

