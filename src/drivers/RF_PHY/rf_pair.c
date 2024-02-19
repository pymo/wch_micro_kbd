/**
 * @brief rf pair deal, start when triggered pair event.
 *      If get pair request, add the remote address
 *      and the local address to get the access address. 
 * 
 * @copyright Copyright (c) 2022 zerosensei
 * 
 */
#include "rf_pair.h"
#include "device_config.h"
#include "sys/byteorder.h"
#include "rf_trans.h"
#ifdef CONFIG_RF_PAIR

#ifdef CONFIG_RF_PAIR_DEBUG
#define RF_PAIR_DBG  PRINT
#else
#define RF_PAIR_DBG
#endif

uint8_t rf_pair_task = 0;

struct rf_pair pair_buf;

uint32_t rf_local_addr;
uint32_t rf_remote_addr;
uint32_t rf_access_addr;

void rf_pair_end(void);


__attribute__((section(".highcode")))
void rf_pair_deal(uint8_t *data, uint8_t len)
{
    if(!len || !data)
        return;

    struct rf_pair *pair_rcv = (struct rf_pair *)data;

    rf_access_addr = pair_rcv->addr + rf_local_addr;

    pair_buf.addr = rf_access_addr;
#ifdef CONFIG_RF_DEV_KBD
    pair_buf.dev = RF_DEV_KBD;
#endif

#ifdef CONFIG_RF_DEV_MOUSE
    pair_buf.dev = RF_DEV_MOUSE;
#endif

    rf_port_load_data(RF_TYPE_CMD_PAIR, (uint8_t *)&pair_buf, sizeof(pair_buf));
    if(tmos_get_task_timer(rf_pair_task, RF_PAIR_END_EVT) == 0)
    {
        tmos_start_task(rf_pair_task, RF_PAIR_END_EVT,0);
    }

}

uint16_t RF_Pair_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if (events & SYS_EVENT_MSG) {
        uint8 *pMsg;

        if ((pMsg = tmos_msg_receive(task_id)) != NULL) {

            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & RF_PAIR_TIMEOUT_EVT){
        if(device_pair.Pair_flag)
            rf_set_addr(device_pair.Mac_addr);

        return (events ^ RF_PAIR_TIMEOUT_EVT);
    }

    if(events & RF_PAIR_END_EVT){
        rf_pair_end();
        tmos_stop_task(rf_pair_task, RF_PAIR_EVT);
        return (events ^ RF_PAIR_END_EVT);
    }

    if(events & RF_PAIR_EVT){
        pair_buf.addr = rf_local_addr;
#ifdef CONFIG_RF_DEV_KBD
        pair_buf.dev = RF_DEV_KBD;
#endif

#ifdef CONFIG_RF_DEV_MOUSE
        pair_buf.dev = RF_DEV_MOUSE;
#endif
        rf_port_load_data(RF_TYPE_CMD_PAIR, (uint8_t *)&pair_buf, sizeof(pair_buf));
        tmos_start_task(rf_pair_task, RF_PAIR_EVT, MS1_TO_SYSTEM_TIME(10));
        return (events ^ RF_PAIR_EVT);
    }

    return 0;
}



int rf_pair_start(void)
{
    RF_PAIR_DBG("start pair\n");
    tmos_memset(&pair_buf, 0, sizeof(pair_buf));

    pair_buf.addr = rf_local_addr;
#ifdef CONFIG_RF_DEV_KBD
    pair_buf.dev = RF_DEV_KBD;
#endif

#ifdef CONFIG_RF_DEV_MOUSE
    pair_buf.dev = RF_DEV_MOUSE;
#endif
    rf_port_load_data(RF_TYPE_CMD_PAIR, (uint8_t *)&pair_buf, sizeof(pair_buf));



    extern rfConfig_t rf_config;
    rf_config.accessAddress = DEFAULT_MAC;

    RF_Config(&rf_config);

    tmos_start_task(rf_pair_task, RF_PAIR_TIMEOUT_EVT, MS1_TO_SYSTEM_TIME(CONFIG_RF_DEV_PAIR_TO));
    tmos_start_task(rf_pair_task, RF_PAIR_EVT, MS1_TO_SYSTEM_TIME(10));
    return 0;
}

void rf_pair_end(void)
{
    RF_PAIR_DBG("pair end\n");
    rf_set_addr(rf_access_addr);
    device_pair.Pair_flag = 1;
    device_pair.Mac_addr = rf_access_addr;
    RF_PAIR_DBG("rf access addr: %#lx\n", rf_access_addr);
    tmos_stop_task(rf_pair_task, RF_PAIR_TIMEOUT_EVT);
    SaveDeviceInfo("device_pair");
}

void rf_pair_task_init(void)
{
    uint8_t mac_temp[6];
    GetMACAddress( mac_temp );
    rf_local_addr = sys_get_le32(mac_temp);
    PRINT("rf local addr: %#lx\n", rf_local_addr);
    if(device_pair.Pair_flag){
        int ret;
        if((ret = rf_set_addr(device_pair.Mac_addr)) < 0){
            PRINT("set addr error %d\n", ret);
        }
    }
           
    rf_pair_task = TMOS_ProcessEventRegister(RF_Pair_ProcessEvent);
}

#endif /* CONFGI_RF_PAIR */
