/**
 * @file rf_pair.h
 * 
 * @copyright Copyright (c) 2022 zerosensei
 * 
 */
#ifndef __RF_PAIR_H
#define __RF_PAIR_H

#include "rf_port.h"

#define RF_PAIR_TIMEOUT_EVT                 (1<<0)
#define RF_PAIR_END_EVT                     (1<<1)
#define RF_PAIR_EVT                         (1<<3)



#define PAIR_DEV_KBD            (1<<0)
#define PAIR_DEV_MOUSE          (1<<1)

#ifndef CONFIG_RF_DEV_PAIR_TO     
#define CONFIG_RF_DEV_PAIR_TO   (10 * 1000)
#endif

struct __attribute__((__packed__)) rf_pair{
    uint8_t dev : 2;

    uint32_t addr;
};


void rf_pair_deal(uint8_t *data, uint8_t len);
void rf_pair_task_init(void);
int rf_pair_start(void);

#endif /* __RF_PAIR_H */
