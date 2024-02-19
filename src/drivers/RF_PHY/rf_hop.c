/**
 * @brief rf frequency hopping, hopping when receive data successfully.
 * 
 * @copyright Copyright (c) 2022 zerosensei
 * 
 */
#include "rf_hop.h"
#include "rf_sync.h"
#include "sys/util.h"

const uint32_t rf_channel_map[CONFIG_RF_CHANNEL_NUM] = CONFIG_RF_CHANNEL_MAP;
static uint32_t current_channel_idx = 0;
bool is_hopping = false;
static uint32_t current_stamp = 0;
static uint32_t last_stamp = 0;

void rf_hop_start(void)
{
    is_hopping = true;
    last_stamp = rf_sync_get_current_stamp();  //是否需要加时间
}

void rf_hop_end(void)
{
    rf_set_channel(rf_channel_map[0]);
    current_channel_idx = 0;
    is_hopping = false;
}


 __attribute__((section(".highcode")))
void rf_hop(void)
{ 
    if(!is_hopping){
#ifdef CONFIG_RF_DEBUG_GPIO
        GPIOA_InverseBits(DEV_HOP_0_PIN);
#endif
        return;
    }

    current_stamp = rf_sync_get_current_stamp();
    uint32_t stamp_delta = current_stamp - last_stamp;

    stamp_delta += MS_TO_RTC(CONFIG_RF_SYNC_UNIT_MS/2.0);
    current_channel_idx += RTC_TO_MS(stamp_delta) / CONFIG_RF_SYNC_UNIT_MS;
    current_channel_idx = current_channel_idx % ARRAY_SIZE(rf_channel_map);

    // PRINT("%d\n", rf_channel_map[current_channel_idx]);

#ifdef CONFIG_RF_DEBUG_GPIO
    if(current_channel_idx == 0)
        GPIOA_InverseBits(DEV_HOP_0_PIN);
#endif

    rf_set_channel(rf_channel_map[current_channel_idx]);
    last_stamp = current_stamp;
}   