/**
 * @brief 
 * 
 * @copyright Copyright (c) 2022 zerosensei
 * 
 */
#ifndef __RF_TRANS_H
#define __RF_TRANS_H

#include "soc.h"
#include "config.h"
#include "sys/atomic.h"
#include "rf_config.h"

#define RF_RCV_EVENT        (0x01)

#define RF_TRANS_RCV_TO_EVT             (1<<0)

#ifdef CONFIG_RF_DEBUG_GPIO
#define DEV_HOP_0_PIN       GPIO_Pin_0
#define DEV_RCV_START_PIN   GPIO_Pin_5
#define DEV_RCV_END_PIN     GPIO_Pin_4
#define DEV_SEND_END_PIN    GPIO_Pin_6
#endif


enum rf_error_code{
    RF_ENO,
    RF_EACCES,
    RF_EINVAL,
    RF_ENOBUFS,
    RF_EMSGSIZE,
    RF_EAGAIN,

    RF_NUM_ERROR,
};

enum rf_data_type {
    RF_TYPE_REQ_PAIR,
    RF_TYPE_REQ_KEY,
    RF_TYPE_CMD_PAIR,
    RF_TYPE_CMD_KEY,
    RF_TYPE_CMD_HB,

    RF_NUM_TYPE,
};



#define DEV_KBD_FLAG    (1<<0)
#define DEV_MOSUE_FLAG  (1<<1)

struct __attribute__((__packed__)) rf_trans_buf{
    uint8_t idx;
    uint8_t dev_flag;
    uint8_t type;

    uint8_t user_len;
    uint8_t user_data[200];
};

struct rf_trans_cb{
    void (* rf_trans_rcv_cb)(uint8_t *data, size_t len);
    void (* rf_trans_tx_finish_cb)(bool is_success);
    void (* rf_trans_rssi_cb)(char rssi);
};

int rf_send(uint8_t *data, size_t len);
int rf_set_rcv(uint8_t *buf, int to_us);
ssize_t rf_read(void);
int rf_set_addr(uint32_t addr);
int rf_set_channel(uint32_t channel);
uint8_t rf_trans_init(void);
void rf_trans_register_cbs(struct rf_trans_cb *cb);
void rf_start_rcv(void);

#endif /* __RF_TRANS_H */
