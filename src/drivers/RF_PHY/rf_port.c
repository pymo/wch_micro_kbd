/**
 * @file rf_port.c
 * @brief rf device port
 *      When generating data needs to be sent, open the receive window and
 *      wait for the request. 
 *      If get the request, should send the date immediately.
 *      
 * @copyright Copyright (c) 2022 zerosensei
 * 
 */
#include "rf_port.h"
#include "RingBuffer/lwrb.h"

struct rf_trans_buf rf_buf_send;
struct rf_trans_buf rf_buf_resend;
struct rf_trans_buf rf_buf_rcv;
uint8_t *rf_rcv_buf;

lwrb_t rf_ringbuf_data;
uint8_t *rf_ringbuffer;
bool is_data_loaded = false;

static uint8_t rf_state_saved = RF_DISCONNECTED;

struct rf_port_cb *rf_port;

void rf_port_rcv_cb(uint8_t *data, size_t len);


struct rf_trans_cb rf_port_cbs = {
    .rf_trans_rcv_cb = rf_port_rcv_cb,
};

static int rcv_timeout_us = 0;
static int extra_timeout_us = 0;
static uint32_t lost_count = 0;

bool rf_port_is_data_loaded(void)
{
    return is_data_loaded;
}

/**
 * Should implement after checking rf_port_is_data_loaded() 
 * if you don't know whether the loaded data has been sent.
 */
__attribute__((section(".highcode")))
int rf_port_load_data(uint8_t type, uint8_t *data, size_t len)
{
    if(len > sizeof(rf_buf_send.user_data))
        return -RF_EMSGSIZE;

    rf_buf_send.type = type;
    rf_buf_send.user_len = len;
    tmos_memcpy(rf_buf_send.user_data, data, len);

    is_data_loaded = true;

    return 0;
}

__attribute__((section(".highcode")))
void rf_port_rcv_open(int timeout_us)
{
    int ret;
    tmos_memset(&rf_buf_rcv, 0, sizeof(struct rf_trans_buf));

    rcv_timeout_us = timeout_us;
    if((ret = rf_set_rcv((uint8_t *)&rf_buf_rcv, rcv_timeout_us + extra_timeout_us)) != SUCCESS){
        // PRINT("set rcv error: %d\n", ret);
        return;
    }
    rf_start_rcv();
}

uint8_t local_idx = 0xff;
/**
 * Should not delay too much time before processing rf_send,
 * otherwise otherwise the dongle receiving end will time out.
 * 
 * If the device receive timeout, @param len should be 0.
 */
__attribute__((section(".highcode")))
void rf_port_rcv_cb(uint8_t *data, size_t len)
{
//    static uint8_t local_idx = 0xff;
    
    if(!len){
        if(!rf_is_sync()) {
            /* sync failed, expand the reception window */
           extra_timeout_us = extra_timeout_us ? (extra_timeout_us * 2) : 100; 
           if(extra_timeout_us >= ((CONFIG_RF_CHANNEL_NUM+2) * CONFIG_RF_SYNC_UNIT_MS * 1000)){

                extra_timeout_us = 0;
                is_data_loaded = false;
                if (rf_port->connect_state_changed &&
                        (rf_state_saved != RF_DISCONNECTED)) {
                    rf_port->connect_state_changed(RF_DISCONNECTED);
                }
                rf_state_saved = RF_DISCONNECTED;
           }
            return ;  
        }

        lost_count++;

        if(lost_count == 1)
        {
            rf_sync_ealy();
        }

        if(lost_count > 35){
            rf_sync_ealy1();
        }

        if(lost_count > 50){  
            /* retry */
            extra_timeout_us = 0;
            lost_count = 0;
            rf_sync_end();
        }
        if(lost_count > 500)
        {
            is_data_loaded = false;
        }
        return;
    }

    rf_sync_start(len);
    extra_timeout_us = 0;
    lost_count = 0;

    struct rf_trans_buf *rcv_data = (struct rf_trans_buf *)data;

    /* loss the cmd, resend*/
    if(local_idx == rcv_data->idx){
        rf_send((uint8_t *)&rf_buf_resend, sizeof(rf_buf_resend) -
            sizeof(rf_buf_resend.user_data) + rf_buf_resend.user_len);

        return;
    }

    local_idx = rcv_data->idx;
    //PRINT("%d\n",rcv_data->type);
    switch (rcv_data->type) {
    case RF_TYPE_REQ_PAIR:
#ifdef CONFIG_RF_PAIR
        if(rf_port->req_pair_cb)
            rf_port->req_pair_cb(rcv_data->user_data, rcv_data->user_len);
#endif
        break;

    case RF_TYPE_REQ_KEY:
        if(rf_port->req_key_cb)
            rf_port->req_key_cb(rcv_data->user_data, rcv_data->user_len);
        break;

    default:
        break;
    }

    rf_send((uint8_t *)&rf_buf_send, sizeof(struct rf_trans_buf) -
        sizeof(rf_buf_send.user_data) + rf_buf_send.user_len);



    tmos_memcpy(&rf_buf_resend, &rf_buf_send, sizeof(struct rf_trans_buf));

    is_data_loaded = false;

    if (rf_port->connect_state_changed &&
            (rf_state_saved != RF_CONNECTED)) {
        rf_port->connect_state_changed(RF_CONNECTED);
    }
    rf_state_saved = RF_CONNECTED;

    //PRINT("dev send %d :[", sizeof(struct rf_trans_buf) - sizeof(rf_buf_send.user_data) + rf_buf_send.user_len);

//    uint8_t *p = (uint8_t *)&rf_buf_send;
//    for(int i = 0; i < sizeof(struct rf_trans_buf) - sizeof(rf_buf_send.user_data) + rf_buf_send.user_len; i++){
//        if(i) PRINT(" ");
//        PRINT("%#x", p[i]);
//    }PRINT("]\n");
}

void rf_port_init(struct rf_port_cb *cb)
{   
    if(cb)
        rf_port = cb;
    rf_trans_register_cbs(&rf_port_cbs);
}

