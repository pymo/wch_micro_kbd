/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbdesc.c
 * Purpose: USB Custom User Module
 * Version: V1.01
 * Date:        2013/11
 *------------------------------------------------------------------------------*/

#include "CH58x_common.h"
#include "usbdesc.h"
#include "hid.h"

/*****************************************************************************
* Description   : USB_DeviceDescriptor[]
*****************************************************************************/
const uint8_t USB_DeviceDescriptor[] =
{
    USB_DEVICE_DESC_SIZE,                   /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,             /* bDescriptorType */
    USB_WBVAL(0x0200), /* 2.00 */           /* bcdUSB */
    0x00,                                   /* bDeviceClass */
    0x00,                                   /* bDeviceSubClass */
    0x00,                                   /* bDeviceProtocol */
    USB_EP0_PACKET_SIZE,                    /* bMaxPacketSize0 */
    USB_WBVAL(USB_VID),                     /* idVendor */
    USB_WBVAL(USB_PID),                     /* idProduct */
    USB_WBVAL(USB_REV),                     /* bcdDevice */
    USB_DEVICE_STRING_MANUFACTURER,         /* iManufacturer */
    USB_DEVICE_STRING_PRODUCT,              /* iProduct */
    0X03,             /* iSerialNumber */
    0x01,                                   /* bNumConfigurations: one possible configuration*/
};


/*****************************************************************************
* Description   : HID_ReportDescriptor[]
*****************************************************************************/
const uint8_t HID_ReportDescriptor[] =
{
    HID_UsagePage(HID_USAGE_PAGE_GENERIC),      // usage page (generic desktop)
    HID_Usage(HID_USAGE_GENERIC_KEYBOARD),      // usage (keyboard)                 //Keyboard
    HID_Collection(HID_Application),            // collection (application)
    HID_UsagePage(HID_USAGE_PAGE_KEYBOARD),     // usage page (keyboard)
    HID_UsageMin(HID_USAGE_KEYBOARD_LCTRL),     // usage minimum (224)
    HID_UsageMax(HID_USAGE_KEYBOARD_RGUI),      // usage maximum (231)
    HID_LogicalMin(0),                          // logical minimum (0)
    HID_LogicalMax(1),                          // logical maximum (1)
    HID_ReportSize(1),                          // report size (1)
    HID_ReportCount(8),                         // report count (8)
    HID_Input(HID_Data|HID_Variable|HID_Absolute),  // input (data, variable, absolute)
    HID_ReportCount(1),                         // report count (1)
    HID_ReportSize(8),                          // report size (8)
    HID_Input(HID_Constant),                    // input (constant)
    HID_ReportCount(5),                         // report count (5)
    HID_ReportSize(1),                          // report size (1)
    HID_UsagePage(HID_USAGE_PAGE_LED),          // usage page(LEDs)
    HID_UsageMin(HID_USAGE_LED_NUM_LOCK),       // usage minimum (1)
    HID_UsageMax(HID_USAGE_LED_KANA),           // usage maximum (5)
    HID_Output(HID_Data|HID_Variable|HID_Absolute), // output (data, variable, absolute)
    HID_ReportCount(1),                         // report count (1)
    HID_ReportSize(3),                          // report size (3)
    HID_Output(HID_Constant),                   // output (constant)
    HID_ReportCount(6),                         // report count (6)
    HID_ReportSize(8),                          // report size (8)
    HID_LogicalMin(0),                          // logical minimum (0)
    HID_LogicalMaxS(255),                       // logical maximum (255)
    HID_UsagePage(HID_USAGE_PAGE_KEYBOARD),     // usage page (keyboard)
    HID_UsageMin(0),                            // usage minimum (0)
    //HID_UsageMaxS(HID_USAGE_KEYBOARD_APPLICATION),    // usage maximum (101)
    HID_UsageMaxS(255),                         // usage maximum (255)
    HID_Input(HID_Data|HID_Array|HID_Absolute), // input (data, array, absolute)

    HID_UsagePage(HID_USAGE_PAGE_CONSUMER),     // usage page (consumer)
    HID_Usage(HID_USAGE_PAGE_UNDEFINED),        // usage (consumer control)
    HID_LogicalMin(0x80),                       // logical minimum (-128)
    HID_LogicalMax(0x7F),                       // logical maximum (+127)
    HID_ReportCount(64),                        // report count (64)
    HID_ReportSize(8),                          // report size (8)
    HID_Feature(HID_Data|HID_Variable|HID_Absolute),// feature (data, variable, absolute)

    HID_EndCollection,                          // end collection

};
const uint16_t HID_ReportDescSize = sizeof(HID_ReportDescriptor);


/*****************************************************************************
* Description   : HID_ReportDescriptor2[]
*****************************************************************************/
const uint8_t HID_ReportDescriptor2[] =
{
    //bit keyboard
    HID_UsagePage(HID_USAGE_PAGE_GENERIC),      // usage page (generic desktop)
    HID_Usage(HID_USAGE_GENERIC_KEYBOARD),      // usage (keyboard)
    HID_Collection(HID_Application),            // collection (application)
    HID_ReportID(1),                            // report ID    (1)
    HID_UsagePage(HID_USAGE_PAGE_KEYBOARD),     // usage page (keyboard)
    HID_UsageMin(4),                            // usage minimum (224)
    HID_UsageMax(0x70),                         // usage maximum (231)
    HID_LogicalMin(0),                          // logical minimum (0)
    HID_LogicalMax(1),                          // logical maximum (1)
    HID_ReportSize(1),                          // report size (1)
    HID_ReportCount(0x78),                      // report count ()
    HID_Input(HID_Data|HID_Variable|HID_Absolute),  // input (data, variable, absolute)
    HID_EndCollection,                          // end collection

//  // system control
//  HID_UsagePage(HID_USAGE_PAGE_GENERIC),      // usage page (generic desktop)
//  HID_Usage(HID_USAGE_GENERIC_SYSTEM_CTL),    // usage (System Control)
//  HID_Collection(HID_Application),            // collection (application)
//  HID_ReportID(2),                            // report ID    (2)
//  HID_UsagePage(HID_USAGE_PAGE_GENERIC),      // usage page (generic desktop)
//  HID_UsageMin(HID_USAGE_GENERIC_SYSCTL_POWER),   // usage minimum (System Power Down)
//  HID_UsageMax(HID_USAGE_GENERIC_SYSCTL_WAKE),    // usage maximum (System Wake Up)
//  HID_LogicalMin(0),                          // logical minimum (0)
//  HID_LogicalMax(1),                          // logical maximum (1)
//  HID_ReportCount(3),                         // report count (3)
//  HID_ReportSize(1),                          // report size (1)
//  HID_Input(HID_Data|HID_Variable|HID_Absolute),  // input (data, variable, absolute)
//  HID_ReportCount(1),                         // report count (1)
//  HID_ReportSize(5),                          // report size (5)
//  HID_Input(HID_Constant),                    // input (constant)
//  HID_EndCollection,                          // end collection

    // system control
//    0X05,   0X01,                               // Usage Page
//    0X09,   0X0C,                               // Usage
//    0XA1,   0X01,                               // Collection
//    0X85,   0X02,                               // Report ID
//    0X19,   0XC6,                               // Usage Minimum
//    0X29,   0XC8,                               // Usage Maximum
//    0X15,   0X00,                               // Logical Minimum
//    0X25,   0X01,                               // Logical Maximum
//    0X75,   0X01,                               // Report Size
//    0X95,   0X01,                               // Report Count
//    0X81,   0X02,                               // Input
//    0X75,   0X01,                               // Report Size
//    0X95,   0X07,                               // Report Count
//    0X81,   0X03,                               // Input
//    0XC0,                                       // End Collection


    //system control
    0x05, 0x01,
    0x09, 0x80,
    0xA1, 0x01,
    0x85, 0x02,  //report ID (2)
    0x05, 0x01,
    0x19, 0x81,
    0x29, 0x83,
    0x15, 0x00,
    0x25, 0x01,
    0x95, 0x03, //count 3
    0x75, 0x01,
    0x81, 0x02,
    0x95, 0x01,
    0x75, 0x05,
    0x81, 0x01,
    0xc0,



    //consumer
    HID_UsagePage(HID_USAGE_PAGE_CONSUMER),     // usage page (consumer)
    HID_Usage(HID_USAGE_CONSUMER_CONTROL),      // usage (consumer control)
    HID_Collection(HID_Application),            // collection (application)
    HID_ReportID(3),                            // report ID    (3)
    HID_LogicalMin(0),                          // logical minimum (0)
    HID_LogicalMaxS(0x1fff),                    // logical maximum (1)
    HID_UsageMin(0),                            // usage minimum
    HID_UsageMaxS(0x1fff),                      // usage maximum
    HID_ReportSize(16),                         // report size (16)
    HID_ReportCount(1),                         // report count (1)
    HID_Input(0),
    HID_EndCollection,                          // end collection

    //vendor
    HID_UsagePageVendor(0x1c),                  //usage vendor
    HID_Usage(0x92),
    HID_Collection(1),
    HID_ReportID(4),                            // report ID    (4)
    HID_UsageMin(0),
    HID_UsageMaxS(0x00FF),
    HID_LogicalMin(0),
    HID_LogicalMaxS(0x00FF),
    HID_ReportSize(8),
    HID_ReportCount(0x3F),
    HID_Output(0),
    HID_UsageMin(0),
    HID_UsageMaxS(0x00FF),
    HID_Input(0),
    HID_EndCollection,




//    //mouse
//    0x05, 0x01,
//    0x09, 0x02,
//    0xal, 0x01,
//    HID_ReportID(5),
//    0x09, 0x01,
//    0xa1, 0x00,
//    0x05, 0x09,
//    0x19, 0x01,
//    0x29, 0x05,
//    0x15, 0x00,
//    0x25, 0x01,
//    0x95, 0x05,
//    0x75, 0x01,
//    0x81, 0x02,
//    0x95, 0x01,
//    0x75, 0x03,
//    0x81, 0x01,
//    0x05, 0x01,
//    0x09, 0x30,
//    0x09, 0x31,
//    0x16, 0x00, 0x80,
//    0x26, 0xff, 0x7f,
//    0x75, 0x10,
//    0x95, 0x02,
//    0x81, 0x06,
//    0x09, 0x38,
//    0x15, 0x81,
//    0x25, 0x7f,
//    0x75, 0x08,
//    0x95, 0x01,
//    0x81, 0x06,
//    0x05, 0x0c,
//    0x0a, 0x38, 0x02,
//    0x15, 0x81,
//    0x25, 0x7f,
//    0x75, 0x08,
//    0x95, 0X01,
//    0x81, 0x06,
//    0xc0,
//    0xc0,

    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x06,                    //   REPORT_ID (6)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x08,                    //     REPORT_COUNT (8)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //     END_COLLECTION
    0xc0,                           // END_COLLECTION

};
const uint16_t HID_ReportDescSize2 = sizeof(HID_ReportDescriptor2);

/*****************************************************************************
* Description   : HID_ReportDescriptor3[]
*****************************************************************************/
const uint8_t HID_ReportDescriptor3[] =
{
    0x06, 0x1C, 0xFF,
    0x09, 0x92,
    0xA1, 0x01,
    0x19, 0x00,
    0x2A, 0xFF, 0x00,
    0x15, 0x00,
    0x26, 0xFF, 00,
    0x75, 0x08,
    0x95, 0x3F,
    0x91, 0x00,
    0x19, 0x00,
    0x29, 0xFF,
    0x81, 0x00,
    0xC0,
};
const uint16_t HID_ReportDescSize3 = sizeof(HID_ReportDescriptor3);



#define nUsb_NumInterfaces 2
#define nUsb_TotalLength (USB_CONFIGUARTION_DESC_SIZE+USB_INTERFACE_DESC_SIZE*2+USB_HID_DESC_SIZE*2+USB_ENDPOINT_DESC_SIZE*2)

const uint8_t USB_ConfigDescriptor[] =
{
    /* Configuration 1 */
    USB_CONFIGUARTION_DESC_SIZE,            /* bLength */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType */
    USB_WBVAL(nUsb_TotalLength),            /* wTotalLength */
    nUsb_NumInterfaces,                     /* bNumInterfaces */
    USB_CONFIG_VALUE,                       /* bConfigurationValue */
    0x00,                                   /* iConfiguration */
    0xA0,
    0xC8,                                   /* bMaxPower */

/*****************************************************************************
* Description   : Interface 0[]
*****************************************************************************/
    /* Interface 0, Alternate Setting 0, HID Class */
    USB_INTERFACE_DESC_SIZE,                /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,          /* bDescriptorType */
    USB_INTERFACE_0,                        /* bInterfaceNumber */
    0x00,                                   /* bAlternateSetting */
    0x01,                                   /* bNumEndpoints */
    0x03,                                   /* bInterfaceClass */
    HID_SUBCLASS_BOOT,                      /* bInterfaceSubClass */
    HID_PROTOCOL_KEYBOARD,
    0x00,                                   /* iInterface */

    /* HID Class Descriptor */
    USB_HID_DESC_SIZE,                      /* bLength */
    HID_HID_DESCRIPTOR_TYPE,                /* bDescriptorType */
    USB_WBVAL(0x0111), /* 1.11 */           /* bcdHID */
    0x00,                                   /* bCountryCode */
    0x01,                                   /* bNumDescriptors */
    HID_REPORT_DESCRIPTOR_TYPE,             /* bDescriptorType */
    USB_WBVAL(HID_ReportDescSize),          /* wDescriptorLength */

    /* Endpoint1, HID Interrupt In */
    USB_ENDPOINT_DESC_SIZE,                 /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* bDescriptorType */
    USB_ENDPOINT_IN(USB_EP1),               /* bEndpointAddress */
    USB_ENDPOINT_TYPE_INTERRUPT,            /* bmAttributes */
    USB_WBVAL(USB_EP1_PACKET_SIZE),         /* wMaxPacketSize */
    0x01,          /* 1ms */                /* bInterval */

/*****************************************************************************
* Description   : Interface 1[]
*****************************************************************************/
    /* Interface 1, Alternate Setting 0, HID Class */
    USB_INTERFACE_DESC_SIZE,                /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,          /* bDescriptorType */
    USB_INTERFACE_1,                        /* bInterfaceNumber */
    0x00,                                   /* bAlternateSetting */
    0x01,                                   /* bNumEndpoints */
    0x03,                                   /* bInterfaceClass */
    HID_SUBCLASS_BOOT,                      /* bInterfaceSubClass */
    HID_PROTOCOL_MOUSE,                     /* bInterfaceProtocol */
    0x00,                                   /* iInterface */

    /* HID Class Descriptor */
    USB_HID_DESC_SIZE,                      /* bLength */
    HID_HID_DESCRIPTOR_TYPE,                /* bDescriptorType */
    USB_WBVAL(0x0111), /* 1.11 */           /* bcdHID */
    0x00,                                   /* bCountryCode */
    0x01,                                   /* bNumDescriptors */
    HID_REPORT_DESCRIPTOR_TYPE,             /* bDescriptorType */
    USB_WBVAL(HID_ReportDescSize2),         /* wDescriptorLength */

    /* Endpoint2, HID Interrupt In */
    USB_ENDPOINT_DESC_SIZE,                 /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* bDescriptorType */
    USB_ENDPOINT_IN(USB_EP2),               /* bEndpointAddress */
    USB_ENDPOINT_TYPE_INTERRUPT,            /* bmAttributes */
    USB_WBVAL(USB_EP2_PACKET_SIZE),         /* wMaxPacketSize */
    0x01,          /* 1ms */                /* bInterval */

    /* Endpoint3, HID Interrupt In */
//    USB_ENDPOINT_DESC_SIZE,                 /* bLength */
//    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* bDescriptorType */
//    USB_ENDPOINT_IN(USB_EP3),               /* bEndpointAddress */
//    USB_ENDPOINT_TYPE_INTERRUPT,            /* bmAttributes */
//    USB_WBVAL(USB_EP3_PACKET_SIZE),         /* wMaxPacketSize */
//    0x01,          /* 1ms */                /* bInterval */

/*****************************************************************************
* Description   : Interface 2[]
*****************************************************************************/
#if (0)
    /* Interface 2, Alternate Setting 0, HID Class */
    USB_INTERFACE_DESC_SIZE,                /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,          /* bDescriptorType */
    USB_INTERFACE_2,                        /* bInterfaceNumber */
    0x00,                                   /* bAlternateSetting */
    0x01,                                   /* bNumEndpoints */
    0x03,                                   /* bInterfaceClass */
    HID_SUBCLASS_NONE,                      /* bInterfaceSubClass */
    HID_PROTOCOL_NONE,                      /* bInterfaceProtocol */
    0x00,                                   /* iInterface */
    /* HID Class Descriptor */
    USB_HID_DESC_SIZE,                      /* bLength */
    HID_HID_DESCRIPTOR_TYPE,                /* bDescriptorType */
    USB_WBVAL(0x0111), /* 1.11 */           /* bcdHID */
    0x00,                                   /* bCountryCode */
    0x01,                                   /* bNumDescriptors */
    HID_REPORT_DESCRIPTOR_TYPE,             /* bDescriptorType */
    USB_WBVAL(HID_ReportDescSize3),         /* wDescriptorLength */
    /* Endpoint3, HID Interrupt In */
    USB_ENDPOINT_DESC_SIZE,                 /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* bDescriptorType */
    #if (USB_EP3_DIRECTION == USB_DIRECTION_IN)
        USB_ENDPOINT_IN(USB_EP3),           /* bEndpointAddress */
    #else
        USB_ENDPOINT_OUT(USB_EP3),          /* bEndpointAddress */
    #endif
    USB_ENDPOINT_TYPE_INTERRUPT,            /* bmAttributes */
    USB_WBVAL(USB_EP3_PACKET_SIZE),         /* wMaxPacketSize */
    0x08,          /* 1ms */                /* bInterval */
#endif
};


/* USB String Descriptor (optional) */
/*****************************************************************************
* Description   : USB_LanguageStringDescriptor[]
*****************************************************************************/
const uint8_t USB_LanguageStringDescriptor[] =
{
/* Index 0x00: LANGID Codes */
    0x04,                               /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,         /* bDescriptorType */
    USB_WBVAL(0x0409),                  /* US English *//* wLANGID */
};

/*****************************************************************************
* Description   : USB_ManufacturerStringDescriptor[]
*****************************************************************************/
const uint8_t USB_ManufacturerStringDescriptor[] =
{
/* Index 0x01: Manufacturer */
    (5*2 + 2),                          /* bLength (5 Char + Type + lenght) */
    USB_STRING_DESCRIPTOR_TYPE,         /* bDescriptorType */
    'W',0,
    'C',0,
    'H',0,
    'C',0,
    'N',0,
};

/*****************************************************************************
* Description   : USB_ProductStringDescriptor[]
*****************************************************************************/
const uint8_t USB_ProductStringDescriptor[] =
{
/* Index 0x02: Product */
    (10*2 + 2),                         /* bLength ( 10 Char + Type + lenght) */
    USB_STRING_DESCRIPTOR_TYPE,         /* bDescriptorType */
    'U',0,
    'S',0,
    'B',0,
    ' ',0,
    'D',0,
    'E',0,
    'V',0,
    'I',0,
    'C',0,
    'E',0,
};

/*****************************************************************************
* Description   : USB_SerialNumberStringDescriptor[]
*****************************************************************************/
const uint8_t USB_SerialNumberStringDescriptor[] =
{
/* Index 0x03: Serial Number */
    (4*2 + 2),                          /* bLength (4 Char + Type + lenght) */
    USB_STRING_DESCRIPTOR_TYPE,         /* bDescriptorType */
    'D',0,
    'E',0,
    'M',0,
    'O',0,
};




