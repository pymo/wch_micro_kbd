
#include "soc.h"
#include "rf_dev.h"
#include "HAL.h"
#include "USB/usbuser.h"
#include "RingBuffer/lwrb.h"
#include "RF_PHY/rf_pair.h"
#include "backlight/backlight.h"
#include "device_config.h"
extern u8 pm_is_idle;
uint8_t rf_dev_taskid = 0;
static rf_data_t send_buf = {
        .report_id = 0xff,
        .data = {0}
};


void rf_key_deal(uint8_t *data, uint8_t len);
void state_changed(uint8_t state);

struct rf_port_cb rf_deal= {
    .req_key_cb = rf_key_deal,
    .req_pair_cb = rf_pair_deal,
    .connect_state_changed = state_changed,
};

/** We loaded the keys before opening receive window, 
 * so wo do not need to load the keys to the send buffer.
 */
void rf_key_deal(uint8_t *data, uint8_t len)
{
    rf_data_t *tempbuf = (rf_data_t *)data;
    if(tempbuf->report_id == 0 && 
        len == sizeof(rf_data_t) - sizeof(tempbuf->data) + 1) {
        (tempbuf->data[0] & 0x01 )?set_led_num(true):set_led_num(false);
        (tempbuf->data[0] & 0x02 )?set_led_cap(true):set_led_cap(false);
        (tempbuf->data[0] & 0x04 )?set_led_scr(true):set_led_scr(false);
//        (tempbuf->data[0] & 0x08 )?(pm_is_idle = 2):0;
         LOG_INFO("get led: %#x", tempbuf->data[0]);
    }
}

void state_changed(uint8_t state)
{
    if (state) {
        PRINT("rf connected\n");

    } else {
        PRINT("rf disconnected\n");
        pm_is_idle = 2;
    }
}

__HIGH_CODE
int rf_tx_deal(void)
{

    if(rf_port_is_data_loaded())
        return -RF_EACCES;

    if (lwrb_get_full(&KEY_buff) == 0)
        return -RF_EAGAIN;


    uint8_t report_id, key_len = 0;

    lwrb_read(&KEY_buff, &report_id, 1);

    if(report_id == KEYNORMAL_ID) {
        key_len= 8;
    } else if( report_id == KEYBIT_ID ){
        key_len = 16;
    } else if (report_id == SYS_ID) {
        key_len = 2;
    } else if(report_id == CONSUME_ID){
        key_len = 2;
    } else if(report_id == VENDOR_ID){
//        key_len = 2;
    } else if(report_id == MOUSE_ID){
        key_len = 4;
    } else {
        /* should not be here */
        LOG_DEBUG("report id error!");
        lwrb_reset(&KEY_buff);
    }

    lwrb_read(&KEY_buff, send_buf.data, key_len);

    send_buf.report_id = report_id;

    int ret = rf_port_load_data(RF_TYPE_CMD_KEY, (uint8_t *)&send_buf,
        sizeof(rf_data_t) - sizeof(send_buf.data) + key_len);

    return (ret < 0 ? ret : lwrb_get_full(&KEY_buff)); 
}

static void RF_device_TMOSMsg(tmos_event_hdr_t *pMsg)
{
    switch (pMsg->event) {
    case KEY_MESSAGE:{
        SendMSG_t *msg = (SendMSG_t *)pMsg;
        msg->hdr.status?tmos_set_event(rf_dev_taskid, RF_SEND_EVENT):0;
    }  break;

    case RF_PAIR_MESSAGE:{
        SendMSG_t *msg = (SendMSG_t *)pMsg;
        msg->hdr.status?tmos_set_event(rf_dev_taskid, PAIR_EVENT):0;
    }break;

    default:
        break;
    }
}

uint16_t rf_device_event(uint8_t task_id, uint16_t events)
{
    if (events & SYS_EVENT_MSG) {
        tmos_event_hdr_t *pMsg;

        if ((pMsg = (tmos_event_hdr_t *)tmos_msg_receive(rf_dev_taskid)) != NULL) {
            RF_device_TMOSMsg((tmos_event_hdr_t *) pMsg);
            // Release the TMOS message
            tmos_msg_deallocate((uint8_t *)pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & PAIR_EVENT) {
#ifdef CONFIG_RF_PAIR
        rf_pair_start();
#endif
        return events ^ PAIR_EVENT;
    }

    if (events & RF_SEND_EVENT) {
        int ret = rf_tx_deal();
        if(ret < 0){
            if(ret == -RF_EACCES){
                tmos_start_task(rf_dev_taskid, RF_SEND_EVENT, MS1_TO_SYSTEM_TIME(2));
            }
        } else if(ret > 0){
            tmos_start_task(rf_dev_taskid, RF_SEND_EVENT, MS1_TO_SYSTEM_TIME(1));
        }
        return events ^ RF_SEND_EVENT;
    }

    if(events & PM_FLAG_EVT){
        pm_is_idle = 2;
        return (events ^ RF_PAIR_END_EVT);
    }


    return 0;
}

void rf_dev_init(void)
{
    SetSysClock(CLK_SOURCE_PLL_80MHz);
    UART1_BaudRateCfg(921600);
    rf_dev_taskid = TMOS_ProcessEventRegister(rf_device_event);

    if(rf_trans_init()){
        PRINT("rf_trans_init error\n");
    }

    rf_port_init(&rf_deal);
    rf_sync_init();
#ifdef CONFIG_RF_PAIR
    rf_pair_task_init();
#endif

    //JUST FOR TEST!
    //  tmos_set_event(rf_dev_taskid, PAIR_EVENT);

    if (device_pair.Pair_flag) {
        tmos_start_task(rf_sync_task_id, RF_PORT_HEARTBEAT_SET_EVT,MS1_TO_SYSTEM_TIME(CONFIG_RF_HB_INTERVAL_MS));
    }
}

void rf_dev_uinit(void)
{
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    UART1_BaudRateCfg(921600);
    
    rf_dev_taskid = 0;
}
