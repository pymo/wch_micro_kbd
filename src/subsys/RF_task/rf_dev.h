
#ifndef __RF_DEV_H
#define __RF_DEV_H

#include "RF_PHY/rf_port.h"
#include "RF_PHY/rf_sync.h"

#define PAIR_EVENT                  0x0001
#define RF_SEND_EVENT               0x0002
#define PM_FLAG_EVT                 0x0004

typedef struct __attribute__((__packed__))
{
    uint8_t report_id;
    uint8_t data[16];
}rf_data_t;

void rf_dev_init(void);
void rf_dev_uinit(void);

extern uint8_t rf_dev_taskid;


#endif /* __RF_DEV_H */
