/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidkbd.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/10
 * Description        : 蓝牙键盘应用程序，初始化广播连接参数，然后广播，直至连接主机后，定时上传键值
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "soc.h"
#include "devinfoservice.h"
#include "battservice.h"
#include "hidkbdservice.h"
#include "hiddev.h"
#include "hidkbd.h"
#include "HAL.h"
#include "RingBuffer/lwrb.h"
#include "USB/usbuser.h"
#include "PM/pm_task.h"
#include "device_config.h"
#include "led_task/led_task.h"
/*********************************************************************
 * MACROS
 */
// HID keyboard input report length
#define HID_KEYBOARD_IN_RPT_LEN     8

// HID LED output report length
#define HID_LED_OUT_RPT_LEN         1

/*********************************************************************
 * CONSTANTS
 */
// Param update delay
#define START_PARAM_UPDATE_EVT_DELAY          6400  // 6400/625=10.24s

// Param update delay
#define START_PHY_UPDATE_DELAY                1600

// HID idle timeout in msec; set to zero to disable timeout
#define DEFAULT_HID_IDLE_TIMEOUT              60000

// Minimum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     9

// Maximum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     12

// Low energy connection interval (units of 1.25ms)
#define LE_DESIRED_MIN_CONN_INTERVAL            40

// Low energy connection interval (units of 1.25ms)
#define LE_DESIRED_MAX_CONN_INTERVAL            80

// Slave latency to use if parameter update request
#define DEFAULT_DESIRED_SLAVE_LATENCY         3

// Supervision timeout value (units of 10ms)
#define DEFAULT_DESIRED_CONN_TIMEOUT          300

// Default passcode
#define DEFAULT_PASSCODE                      0

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_WAIT_FOR_REQ

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE                     FALSE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                  TRUE

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL           10

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Task ID
uint8 hidEmuTaskId = INVALID_TASK_ID;

size_t hidEmu_Mtu = 23;

uint8_t devAddr[6];
uint8_t devAddrType;

static bool terminate_flag = false;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8_t advertData[] = {
    0x02,
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
};

static uint8_t scanRspData[] = {
    0x02,// length of this data
    GAP_ADTYPE_POWER_LEVEL,
    0,       // 0dBm

    0x03,
    GAP_ADTYPE_APPEARANCE,
    LO_UINT16(GAP_APPEARE_HID_KEYBOARD),
    HI_UINT16(GAP_APPEARE_HID_KEYBOARD),

    0x03,
    GAP_ADTYPE_16BIT_COMPLETE,
    LO_UINT16(HID_SERV_UUID),
    HI_UINT16(HID_SERV_UUID),

    0x0F,
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'P', 'D', 'A', ' ' , 'K', 'e', 'y', 'b', 'o', 'a', 'r' , 'd', ' ', '1'
};

// Device name attribute value
static  uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "PDA Keyboard 1";

// HID Dev configuration
static hidDevCfg_t hidEmuCfg = {
DEFAULT_HID_IDLE_TIMEOUT,   // Idle timeout
        HID_FEATURE_FLAGS           // HID feature flags
        };

uint16 hidEmuConnHandle = GAP_CONNHANDLE_INIT;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void hidEmu_ProcessTMOSMsg(tmos_event_hdr_t *pMsg);
static uint8 hidEmuRcvReport(uint8 len, uint8 *pData);
static uint8 hidEmuRptCB(uint8 id, uint8 type, uint16 uuid, uint8 oper,
        uint16 *pLen, uint8 *pData);
static void hidEmuEvtCB(uint8 evt);
static void hidEmuStateCB(gapRole_States_t newState, gapRoleEvent_t * pEvent);

/*********************************************************************
 * PROFILE CALLBACKS
 */

static hidDevCB_t hidEmuHidCBs = { hidEmuRptCB, hidEmuEvtCB,
NULL, hidEmuStateCB };

/*********************************************************************
 * PUBLIC FUNCTIONS
 */


static inline void bt_adv_direct(uint8_t addr_type, uint8_t *addr)
{
    uint8_t adv_event_type;
    adv_event_type = GAP_ADTYPE_ADV_HDC_DIRECT_IND;


    uint8_t Adv_Direct_Addr[B_ADDR_LEN];
    uint8_t Adv_Direct_Type = (addr_type&0x03);

    tmos_memcpy(Adv_Direct_Addr, addr,
            sizeof(Adv_Direct_Addr));

    PRINT("Adv Direct type %#x (", Adv_Direct_Type);
    for (int i = 0; i < 6; i++) {
        if(i) PRINT(" ");
        PRINT("%#x", Adv_Direct_Addr[i]);
    }
    PRINT(")\n");



    GAPRole_SetParameter( GAPROLE_ADV_DIRECT_ADDR, sizeof(Adv_Direct_Addr),
            Adv_Direct_Addr);
    GAPRole_SetParameter( GAPROLE_ADV_DIRECT_TYPE, sizeof(Adv_Direct_Type),
            &Adv_Direct_Type);
//    GAPRole_SetParameter( GAPROLE_ADV_EVENT_TYPE, sizeof(adv_event_type),
//            &adv_event_type);
}


uint8_t direct_count = 0;
bool bt_adv_data_init(void) {
    // Setup the GAP Peripheral Role Profile
    uint8 initial_advertising_enable = TRUE;



    uint16 advInt =32; //0.625us * 80 = 50ms

    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);

    uint8_t ID_Num = device_bond.ID_Num;


    if (device_bond.ID[ID_Num].isbond) {
        set_bluetooth_indicator(BLULETOOTH_RECONNECTING);

        GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
        GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);

        PRINT("remote_addr_type = %d\n",device_bond.ID[ID_Num].remote_addr_type);

//         if(device_bond.ID[ID_Num].remote_addr_type == ADDRTYPE_PUBLIC) {

        uint8_t enable = ENABLE;


        if(device_bond.ID[ID_Num].remote_addr_type&0x30){
            GAPBondMgr_SetParameter( GAPBOND_AUTO_SYNC_RL, sizeof(uint8), &enable);
        }
        else{
            enable = DISABLE;
            GAPBondMgr_SetParameter( GAPBOND_AUTO_SYNC_RL, sizeof(uint8), &enable);
        }

        if(direct_count < 2)
        {
         bt_adv_direct(device_bond.ID[ID_Num].remote_addr_type,
                 device_bond.ID[ID_Num].remote_addr);
        }else{
        uint8_t adv_event_type;
        adv_event_type = GAP_ADTYPE_ADV_IND;
        GAPRole_SetParameter( GAPROLE_ADV_EVENT_TYPE, sizeof(adv_event_type),
                &adv_event_type);

        uint8_t policy = GAP_FILTER_POLICY_WHITE;
        uint8_t ret = GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, sizeof(policy),
            &policy);
        }
    }
    else{
        set_bluetooth_indicator(BLULETOOTH_PAIRING);
        scanRspData[26] = '1' + ID_Num;
        attDeviceName[13] = '1' + ID_Num;

        GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
        GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                scanRspData);
    }
    direct_count++;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
            &initial_advertising_enable);
    return true;
}


//关闭广播
bool bt_adv_data_stop(void) {

  uint8 initial_advertising_enable =FALSE;
  GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
          &initial_advertising_enable);

  return true;
}


void bt_bond_init(void) {
    // Setup the GAP Bond Manager
    uint32 passkey = DEFAULT_PASSCODE;
    uint8 pairMode = GAPBOND_PAIRING_MODE_INITIATE;
    uint8 mitm = DEFAULT_MITM_MODE;
    uint8 ioCap = DEFAULT_IO_CAPABILITIES;
    uint8 bonding = DEFAULT_BONDING_MODE;
    uint8 erase = ENABLE;


    GAPBondMgr_SetParameter( GAPBOND_PERI_DEFAULT_PASSCODE, sizeof(uint32),
            &passkey);
    GAPBondMgr_SetParameter( GAPBOND_PERI_PAIRING_MODE, sizeof(uint8),
            &pairMode);
    GAPBondMgr_SetParameter( GAPBOND_PERI_MITM_PROTECTION, sizeof(uint8),
            &mitm);
    GAPBondMgr_SetParameter( GAPBOND_PERI_IO_CAPABILITIES, sizeof(uint8),
            &ioCap);
    GAPBondMgr_SetParameter( GAPBOND_PERI_BONDING_ENABLED, sizeof(uint8),
            &bonding);
    GAPBondMgr_SetParameter( GAPBOND_ERASE_AUTO, sizeof(uint8), &erase);
}


/*********************************************************************
 * @fn      HidEmu_Init
 *
 * @brief   Initialization function for the HidEmuKbd App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void HidEmu_Init() {
    hidEmuTaskId = TMOS_ProcessEventRegister(HidEmu_ProcessEvent);

    direct_count = 0;
    bt_adv_data_init();

    // Set the GAP Characteristics
    GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,
            (void *) attDeviceName);

    bt_bond_init();

    // Setup Battery Characteristic Values
    {
        uint8 critical = DEFAULT_BATT_CRITICAL_LEVEL;
        Batt_SetParameter( BATT_PARAM_CRITICAL_LEVEL, sizeof(uint8), &critical);
    }

    conn_params.interval_current = 0;
    conn_params.interval_max = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    conn_params.interval_min = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    conn_params.latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    conn_params.timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set up HID keyboard service
    Hid_AddService();

    // Register for HID Dev callback
    HidDev_Register(&hidEmuCfg, &hidEmuHidCBs);

    // Setup a delayed profile startup
    tmos_set_event(hidEmuTaskId, START_DEVICE_EVT);
}

void HidEmu_Unit(void)
{
    uint8 initial_advertising = FALSE;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
            &initial_advertising);


    hidEmuTaskId = 0;
}

bool isNonRetryableResult(uint8_t ret){
    return ret == 0 || ret == bleNotReady;
}
/*********************************************************************
 * @fn      HidEmu_ProcessEvent
 *
 * @brief   HidEmuKbd Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 HidEmu_ProcessEvent(uint8 task_id, uint16 events)
{
    if (events & SYS_EVENT_MSG) {
        uint8 *pMsg;

        if ((pMsg = tmos_msg_receive(hidEmuTaskId)) != NULL) {
            hidEmu_ProcessTMOSMsg((tmos_event_hdr_t *) pMsg);

            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & START_DEVICE_EVT) {
        return (events ^ START_DEVICE_EVT);
    }

    if(events & STOP_ADV_EVT)
    {
        bt_adv_data_stop();
        pm_goto_standby();
        return (events ^ STOP_ADV_EVT);
    }

    if (events & START_PARAM_UPDATE_EVT) {
        // Send connect param update request

        static uint8_t retry = 0;

        if (hidEmuConnHandle == GAP_CONNHANDLE_INIT) {
            return (events ^ START_PARAM_UPDATE_EVT);
        }


        // If the negotiated interval and timeout does not meet the desired value, re-negotiate up to 5 times.
        if(conn_params.interval_current >= conn_params.interval_min &&
                conn_params.interval_current <= conn_params.interval_max &&
                conn_params.timeout <= DEFAULT_DESIRED_CONN_TIMEOUT){
            retry = 0;

            return (events ^ START_PARAM_UPDATE_EVT);
        }

        retry++;
        if(retry < 5){
            tmos_start_task(hidEmuTaskId, START_PARAM_UPDATE_EVT, MS1_TO_SYSTEM_TIME(2000));
        } else {
            retry = 0;
        }

        PRINT("conn param update: %#x, %#x, %#x, %#x\n",conn_params.interval_min,
                    conn_params.interval_max,
                    conn_params.latency,
                    DEFAULT_DESIRED_CONN_TIMEOUT);
        GAPRole_PeripheralConnParamUpdateReq(hidEmuConnHandle,
            conn_params.interval_min,
            conn_params.interval_max,
            conn_params.latency,
            DEFAULT_DESIRED_CONN_TIMEOUT, hidEmuTaskId);

        return (events ^ START_PARAM_UPDATE_EVT);
    }

    if (events & START_PHY_UPDATE_EVT) {
        // start phy update
        PRINT("Send Phy Update %x...\n",
                GAPRole_UpdatePHY( hidEmuConnHandle, 0, GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M, 0 ));

        return (events ^ START_PHY_UPDATE_EVT);
    }

    if(events & OPEN_NOTE_EVT){
        extern void OpenHotiChannel();
        OpenHotiChannel();
        return (events ^ OPEN_NOTE_EVT);
    }

    if (events & BLE_SEND_DATA_EVT) {

        uint8_t report_id;
        uint8_t keyVal[16] = { 0 };
        uint8_t ret;
        if (lwrb_get_full(&KEY_buff)) {
            lwrb_peek(&KEY_buff, 0, &report_id, 1);

            if (report_id == KEYNORMAL_ID) {
                lwrb_peek(&KEY_buff, 1, keyVal, 8);
                PRINT("HID_RPT_ID_KEY_IN:[");
                for (int i = 0; i < 8; i++) {
                    if(i) PRINT(" ");
                    PRINT("%#x", keyVal[i]);
                }
                PRINT("]\n");
                ret = HidDev_Report( HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT,
                        8, keyVal);
                PRINT("Status:%x\n\r",ret);
                if (isNonRetryableResult(ret)) {
                    lwrb_skip(&KEY_buff, 8 + 1);
                }

            } else if (report_id == KEYBIT_ID) {
                lwrb_peek(&KEY_buff, 1, keyVal, 16);
                PRINT("HID_R_KEYBIT_IN:[");
                for (int i = 0; i < 15; i++) {
                    if(i) PRINT(" ");
                    PRINT("%#x", keyVal[i]);
                }
                PRINT("]\n");
                ret = HidDev_Report( HID_RPT_ID_KEYBIT_IN, HID_REPORT_TYPE_INPUT,
                        15, &keyVal[1]);
                PRINT("Status:%x\n\r",ret);
                if (isNonRetryableResult(ret)) {
                    lwrb_skip(&KEY_buff, 16 + 1);
                }

            } else if(report_id == SYS_ID) {
                lwrb_peek(&KEY_buff, 1, keyVal, 2);
                PRINT("HID_RPT_ID_SYS_IN:[");
                for (int i = 0; i < 2; i++) {
                    if(i) PRINT(" ");
                    PRINT("%#x", keyVal[i]);
                }
                PRINT("]\n");

                ret = HidDev_Report( HID_RPT_ID_SYS_IN, HID_REPORT_TYPE_INPUT,
                        2, keyVal);
                PRINT("Status:%x\n\r",ret);
                if (isNonRetryableResult(ret)) {
                    lwrb_skip(&KEY_buff, 2 + 1);
                }

            } else if (report_id == CONSUME_ID) {
                lwrb_peek(&KEY_buff, 1, keyVal, 2);
                PRINT("HID_RPT_ID_KEYCONSUME_IN:[");
                for (int i = 0; i < 2; i++) {
                    if(i) PRINT(" ");
                    PRINT("%#x", keyVal[i]);
                }
                PRINT("]\n");
                ret = HidDev_Report( HID_RPT_ID_KEYCONSUME_IN, HID_REPORT_TYPE_INPUT,
                        2, keyVal);
                PRINT("Status:%x\n\r",ret);
                if (isNonRetryableResult(ret)) {
                    lwrb_skip(&KEY_buff, 2 + 1);
                }

            } else if (report_id == VENDOR_ID) {

            } else if (report_id == MOUSE_ID) {
                lwrb_peek(&KEY_buff, 1, keyVal, 4);
                PRINT("HID_RPT_ID_MOUSE_IN:[");
                for (int i = 0; i < 2; i++) {
                    if(i) PRINT(" ");
                    PRINT("%#x", keyVal[i]);
                }
                PRINT("]\n");

                ret = HidDev_Report( HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT,
                        4, keyVal);PRINT("Status:%x\n\r",ret);
                if (isNonRetryableResult(ret)) {
                    lwrb_skip(&KEY_buff, 2 + 1);
                }

            } else {
                LOG_DEBUG("report id error:%d", report_id);
            }

            if (lwrb_get_full(&KEY_buff)) {
                tmos_start_task(hidEmuTaskId, BLE_SEND_DATA_EVT,
                        MS1_TO_SYSTEM_TIME(10));  //FIXME：根据返回值判断是否再次发送
            }
        }
        return (events ^ BLE_SEND_DATA_EVT);
    }
    return 0;
}

/*********************************************************************
 * @fn      hidEmu_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void hidEmu_ProcessTMOSMsg(tmos_event_hdr_t *pMsg) {
    switch (pMsg->event) {
    case GATT_MSG_EVENT: {
        gattMsgEvent_t *msg = (gattMsgEvent_t *) pMsg;
        if (msg->method == ATT_MTU_UPDATED_EVENT) {
            hidEmu_Mtu = msg->msg.mtuEvt.MTU;
            LOG_INFO("Mtu:%d\n", msg->msg.mtuEvt.MTU);
        }
        break;
    }
    case KEY_MESSAGE: {
        if (hidEmuConnHandle == GAP_CONNHANDLE_INIT) {
            lwrb_reset(&KEY_buff);
            break;
        }
        SendMSG_t *msg = (SendMSG_t *) pMsg;

        if(msg->hdr.status){
            tmos_set_event(hidEmuTaskId, BLE_SEND_DATA_EVT);
            tmos_set_event(hidEmuTaskId, START_PARAM_UPDATE_EVT);
        }
        break;
    }


    case RF_MS_STATE_CHANGE: {

        switch (pMsg->status)
        {
        case PM_STATE_SLEEP:
            conn_params.interval_max = LE_DESIRED_MAX_CONN_INTERVAL;
            conn_params.interval_min = LE_DESIRED_MIN_CONN_INTERVAL;
            break;

        case PM_STATE_ACTIVE:
            conn_params.interval_max = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
            conn_params.interval_min = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
            break;

        default:
            break;
        }

        if(hidEmuConnHandle != GAP_CONNHANDLE_INIT) {
            tmos_start_task(hidEmuTaskId, START_PARAM_UPDATE_EVT, MS1_TO_SYSTEM_TIME(100));
        }

        break;
    }

    default:
        break;
    }
}

/*********************************************************************
 * @fn      hidEmuStateCB
 *
 * @brief   GAP state change callback.
 *
 * @param   newState - new state
 *
 * @return  none
 */

static void hidEmuStateCB(gapRole_States_t newState, gapRoleEvent_t * pEvent) {
    switch (newState & GAPROLE_STATE_ADV_MASK) {
    case GAPROLE_STARTED: {
//        uint8 ownAddr[6];
//        GAPRole_GetParameter( GAPROLE_BD_ADDR, ownAddr);
//        GAP_ConfigDeviceAddr( ADDRTYPE_STATIC, ownAddr);
        hidEmuConnHandle = GAP_CONNHANDLE_INIT;
        PRINT("Initialized..\n");
    }
        break;

    case GAPROLE_ADVERTISING:
        if (pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT) {
            PRINT("Advertising..\n");
        }
        break;

    case GAPROLE_CONNECTED:
        if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT) {
            gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *) pEvent;

            lwrb_reset(&KEY_buff);
            // get connection handle
            hidEmuConnHandle = event->connectionHandle;
            tmos_start_task(hidEmuTaskId, START_PARAM_UPDATE_EVT,START_PARAM_UPDATE_EVT_DELAY);
            tmos_start_task(hidEmuTaskId,OPEN_NOTE_EVT,1600);
            PRINT("Connected..\n");

            tmos_memcpy(devAddr, event->devAddr, 6);

            devAddrType = event->devAddrType;

            PRINT("conn addr type: %d (", devAddrType);
            for(int i = 0 ; i < 6; i++) {
                PRINT(" %#x", devAddr[i]);
            }
            PRINT(" )\n");
            terminate_flag = false;
        }
        break;

    case GAPROLE_WAITING:
        set_bluetooth_indicator(BLULETOOTH_OFF);
        if (pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT) {
            PRINT("Waiting for advertising..\n");
        } else if (pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT) {
            hidEmuConnHandle = GAP_CONNHANDLE_INIT;
            PRINT("Disconnected.. Reason:0x%x\n", pEvent->linkTerminate.reason);
            /* Prevent reconnection */
            if (terminate_flag) {
                terminate_flag = false;
                HidEmu_Init();
                break;
            }
        } else if (pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT) {
            PRINT("Advertising timeout..\n");
        }
        // Enable advertising
        {
            bt_adv_data_init();
            PRINT("advertising.\n");
        }
        break;

    case GAPROLE_ERROR:
        PRINT("Error %x ..\n", pEvent->gap.opcode);
        break;

    default:
        break;
    }
}
/*********************************************************************
 * @fn      hidEmuRcvReport
 *
 * @brief   Process an incoming HID keyboard report.
 *
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  status
 */
static uint8 hidEmuRcvReport(uint8 len, uint8 *pData) {
    // verify data length
    if (len == HID_LED_OUT_RPT_LEN) {
        // set LEDs
        (pData[0] & 0x01) ? set_led_num(1) : set_led_num(0);
        (pData[0] & 0x02) ? set_led_cap(1) : set_led_cap(0);
        (pData[0] & 0x04) ? set_led_scr(1) : set_led_scr(0);

        return SUCCESS;
    } else {
        return ATT_ERR_INVALID_VALUE_SIZE;
    }
}

/*********************************************************************
 * @fn      hidEmuRptCB
 *
 * @brief   HID Dev report callback.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   oper - operation:  read, write, etc.
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  GATT status code.
 */
static uint8 hidEmuRptCB(uint8 id, uint8 type, uint16 uuid, uint8 oper,
        uint16 *pLen, uint8 *pData) {
    uint8 status = SUCCESS;

    // write
    if (oper == HID_DEV_OPER_WRITE) {
        if (uuid == REPORT_UUID) {
            // process write to LED output report; ignore others
            if (type == HID_REPORT_TYPE_OUTPUT) {
                status = hidEmuRcvReport(*pLen, pData);
            }
        }

        if (status == SUCCESS) {
            status = Hid_SetParameter(id, type, uuid, *pLen, pData);
        }
    }
    // read
    else if (oper == HID_DEV_OPER_READ) {
        status = Hid_GetParameter(id, type, uuid, pLen, pData);
    }
    // notifications enabled
    else if (oper == HID_DEV_OPER_ENABLE) {
//          tmos_start_task( hidEmuTaskId, START_REPORT_EVT, 500 );
    }
    return status;
}

/*********************************************************************
 * @fn      hidEmuEvtCB
 *
 * @brief   HID Dev event callback.
 *
 * @param   evt - event ID.
 *
 * @return  HID response code.
 */
static void hidEmuEvtCB(uint8 evt) {
    // process enter/exit suspend or enter/exit boot mode
    return;
}

void Disconnet_conn(void)
{
    if (GAPRole_TerminateLink(hidEmuConnHandle) == SUCCESS) {
        PRINT("disconnet: %d success\n", hidEmuConnHandle);
        terminate_flag = true;
    }
}
/*********************************************************************
 *********************************************************************/
