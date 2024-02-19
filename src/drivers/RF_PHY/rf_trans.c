/**
 * @brief rf transport api. 
 * 
 * @copyright Copyright (c) 2022 zerosensei
 * 
 */
#include "rf_trans.h"
#include "device_config.h"
#include "sys/util.h"

#define RECVREG                       (*((PUINT32V)(0x4000C100+0x38)))
#define RECVCMPSTA                    (RECVREG&BV(1))
#define RECVCMPSTACLR                 (RECVREG &= ~(BV(1)))

rfConfig_t rf_config = {
    .accessAddress = DEFAULT_MAC,
    .CRCInit = 0x555555,
    .Channel = 0,
    .LLEMode = LLE_MODE_PHY_2M | LLE_MODE_BASIC,
};

uint8_t rf_trans_taskid = 0;
uint8_t register_id = 0;

enum rf_trans_status {
    RF_TRANS_STATUS_SENDING,
    RF_TRANS_STATUS_RCVING,

    RF_TRANS_NUM_STATUS,
};

ATOMIC_DEFINE(rf_trans_status, RF_TRANS_NUM_STATUS);

static int rx_timeout = 0;

struct rf_trans_cb *cbs;

static uint8_t *rcv_buf;
static size_t rcv_len = 0;

void RF_2G4StatusCallBack( uint8 sta, uint8 crc, uint8 *rxBuf )
{
    switch( sta ) {
    case TX_MODE_TX_FINISH:
    {
#ifdef CONFIG_RF_DEBUG_GPIO
        GPIOA_InverseBits(DEV_SEND_END_PIN);
#endif
        atomic_clear_bit(rf_trans_status, RF_TRANS_STATUS_SENDING);
        if(cbs->rf_trans_tx_finish_cb)
            cbs->rf_trans_tx_finish_cb(1);
#ifdef CONFIG_RF_DONGLE
        rf_start_rcv();
#endif
        break;
    }

    case TX_MODE_TX_FAIL:
    {
#ifdef CONFIG_RF_DEBUG_GPIO
        GPIOA_InverseBits(DEV_SEND_END_PIN);
#endif
        atomic_clear_bit(rf_trans_status, RF_TRANS_STATUS_SENDING);
        if(cbs->rf_trans_tx_finish_cb)
            cbs->rf_trans_tx_finish_cb(0);
        //TODO: tx failed then resend.
        break;
    }		

    case RX_MODE_RX_DATA: 
    {
        atomic_clear_bit(rf_trans_status, RF_TRANS_STATUS_RCVING);
        tmos_stop_task(rf_trans_taskid, RF_TRANS_RCV_TO_EVT);

        if(crc)
            break;
#ifdef CONFIG_RF_DEBUG_GPIO
        GPIOA_InverseBits(DEV_RCV_END_PIN);
#endif

        /**
         * write data to ringbuffer, 
         * send msg to notify the registered task
         */
        if(!rcv_buf)
            break;

        rcv_len = rxBuf[1];

        tmos_memcpy(rcv_buf, &rxBuf[2], rcv_len);

        if(cbs->rf_trans_rssi_cb)
            cbs->rf_trans_rssi_cb((char)rxBuf[0]);

        if(cbs->rf_trans_rcv_cb)
            cbs->rf_trans_rcv_cb(rcv_buf, rcv_len);

        break;
    }

    default:
        break;
    }
}

int rf_send(uint8_t *data, size_t len)
{
    if(atomic_test_bit(rf_trans_status, RF_TRANS_STATUS_SENDING))
        return -RF_EACCES;

    atomic_set_bit(rf_trans_status, RF_TRANS_STATUS_SENDING);

 
    return RF_Tx(data, len, 0xff, 0xff);
}

int rf_set_rcv(uint8_t *buf, int to_us)
{
    if(atomic_test_bit(rf_trans_status, RF_TRANS_STATUS_RCVING))
        return -RF_EACCES;

    if(!buf)
        return -RF_EINVAL;

    rcv_buf = (uint8_t *)buf;
    rcv_len = 0;
    rx_timeout = to_us;

    return 0;
}

__attribute__((section(".highcode")))
void rf_start_rcv(void)
{
    if(rx_timeout == 0)
        return;

    if(rx_timeout > 0)    
        tmos_start_task(rf_trans_taskid, RF_TRANS_RCV_TO_EVT,
                MS1_TO_SYSTEM_TIME((rx_timeout + 624)/625));

    atomic_set_bit(rf_trans_status, RF_TRANS_STATUS_RCVING);
    RF_Rx(NULL, 0, 0xff, 0xff);

    /* rx window */
    PFIC_SetPendingIRQ(SWI_IRQn);
}

/**
 * @brief read rf data in rxbuf
 * 
 * @return -1: receving, should not read.
 */
ssize_t rf_read(void)
{
    if(atomic_test_bit(rf_trans_status, RF_TRANS_STATUS_RCVING))
        return -1;

    return rcv_len; 
}

__HIGH_CODE
int rf_set_addr(uint32_t addr)
{
    if(atomic_test_bit(rf_trans_status, RF_TRANS_STATUS_RCVING))
        return -RF_EACCES;

    rf_config.accessAddress = addr;

    return RF_Config(&rf_config);
}

__attribute__((section(".highcode")))
int rf_set_channel(uint32_t channel)
{
    if(atomic_test_bit(rf_trans_status, RF_TRANS_STATUS_RCVING))
        return -RF_EACCES;

    rf_config.Channel = channel;

    return RF_Config(&rf_config);
}

void rf_trans_register_cbs(struct rf_trans_cb *cb)
{
    cbs = cb;
}

uint16_t rf_trans_event(uint8 task_id, uint16 events)
{
    if (events & SYS_EVENT_MSG) {
        uint8_t *pMsg;

        if ((pMsg = tmos_msg_receive(task_id)) != NULL) {
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & RF_TRANS_RCV_TO_EVT) {
        if(!atomic_test_and_clear_bit(rf_trans_status, RF_TRANS_STATUS_RCVING))
            return (events ^ RF_TRANS_RCV_TO_EVT);
#ifdef CONFIG_RF_DEBUG_GPIO
        GPIOA_InverseBits(DEV_RCV_END_PIN);
#endif
        RF_Shut();
        if(cbs->rf_trans_rcv_cb)
            cbs->rf_trans_rcv_cb(NULL, 0);
        return (events ^ RF_TRANS_RCV_TO_EVT);
    }

    return 0;
}


uint8_t rf_trans_init(void)
{
    rf_trans_taskid = TMOS_ProcessEventRegister(rf_trans_event);
    register_id = 0;
    tmos_memset(rf_trans_status, 0, sizeof(rf_trans_status));
    rf_config.rfStatusCB = RF_2G4StatusCallBack;
    cbs = NULL;
    PFIC_SetPriority(SWI_IRQn, 0xf0);
    PFIC_EnableIRQ(SWI_IRQn);

    return RF_Config(&rf_config);
}

__HIGH_CODE
void SW_Handler(void)
{
#ifdef CONFIG_RF_DEBUG_GPIO
    GPIOA_InverseBits(DEV_RCV_START_PIN);
#endif
    uint32_t temp = 0;
    while(1) {
        temp++;
        if(RECVCMPSTA) {
            RECVCMPSTACLR;
            break;
        }
        DelayUs(1);

        if(rx_timeout >= 0 && (temp > (rx_timeout + 8 * BLE_BUFF_MAX_LEN))) {
            break;
        }
    }
    rx_timeout = 0;
}
