/**
 * @file rf_sync.h
 * 
 * @copyright Copyright (c) 2022 zerosensei
 * 
 */
#ifndef __RF_SYNC_H
#define __RF_SYNC_H

#include "rf_port.h"
#include "rf_hop.h"

#ifndef CONFIG_RF_SYNC_UNIT_MS
#define CONFIG_RF_SYNC_UNIT_MS          1
#endif

#ifndef CONFIG_RF_HB_INTERVAL_MS
#define CONFIG_RF_HB_INTERVAL_MS          100
#endif

#if (CONFIG_RF_HB_INTERVAL_MS < CONFIG_RF_SYNC_UNIT_MS)
#warning "CONFIG_RF_HB_INTERVAL_MS should not be smaller than CONFIG_RF_SYNC_UNIT_MS !"
#endif

#define RF_PORT_HEARTBEAT_SET_EVT       (1<<0)
extern uint8_t rf_sync_task_id;
static inline uint32_t rf_sync_get_current_stamp(void)
{
    return RTC_GetCycle32k();
}

void rf_sync_start(size_t len);
void rf_sync_end(void);
bool rf_is_sync(void);
void rf_sync(void);
void rf_sync_init(void);
void rf_set_sync_interval(uint8_t intv);
void rf_sync_ealy(void);
void rf_sync_ealy1(void);

#endif /* __RF_SYNC_H */
