/**
 * @brief ��dongle��ͬ��
 *          ͬ�����ƣ�
 *          1. ���յ�request�󣬿���ͬ��������request��������dongle�ķ���ʱ�䣬 �����ݴ�ʱ�������´ο������մ��ڵ�ʱ��ڵ㡣
 *          2. ����������Ҫ������dongle���򲻻�����ͬ����������򿪽��մ��ڣ�����ͬ�������������ݷ�����������ͬ��ʱ�䡣
 *              ��Ҫע����ǣ��������ⲿ�����ѡ����Ҫ������������ʱ�䡣
 *          3. ������CMD�¼�������������Ҫ����dongle���������մ������ɴκ�dongleδȡ���ݣ���رս��մ��ڣ�ֱ���´�CMD�¼�������
 * 
 * @copyright Copyright (c) 2022 zerosensei
 * 
 */
#include "rf_sync.h"
#include "RTC.h"

static uint32_t rf_sync_start_stamp;
uint8_t rf_sync_task_id = 0;
bool is_sync = false;
static uint8_t sync_interval;


 __attribute__((section(".highcode")))
void rf_sync_start(size_t len)
{
    rf_sync_start_stamp = rf_sync_get_current_stamp();

//    rf_sync_start_stamp -= US_TO_RTC(len * 8 + 200 + 200); //���ݳ���+����ִ��ʱ��+ԣ��
    rf_sync_start_stamp = (rf_sync_start_stamp < US_TO_RTC(len * 8 + 200 + 200)) ?
                    rf_sync_start_stamp + RTC_TIMER_MAX_VALUE - US_TO_RTC(len * 8 + 200 + 200) :
                    rf_sync_start_stamp - US_TO_RTC(len * 8 + 200 + 200);

    is_sync = true;
#ifdef CONFIG_RF_HOP
    rf_hop_start();
#endif
}

 __attribute__((section(".highcode")))
 void rf_sync_ealy(void)
 {
     rf_sync_start_stamp-=1;
 }

 __attribute__((section(".highcode")))
 void rf_sync_ealy1(void)
 {
     rf_sync_start_stamp-=16;
 }

void rf_sync_end(void)
{
    is_sync = false;
#ifdef CONFIG_RF_HOP
    rf_hop_end();
#endif
}

bool rf_is_sync(void)
{
    return is_sync;
}

 __attribute__((section(".highcode")))
void rf_set_sync_interval(uint8_t intv)
{
    sync_interval = intv;
}

 __attribute__((section(".highcode")))
static inline uint32_t rf_sync_get_delta_stamp(void)
{
    uint32_t curret_stamp = rf_sync_get_current_stamp();

    uint32_t clk_delta;
    if(curret_stamp - rf_sync_start_stamp < 0) {
        clk_delta = rf_sync_start_stamp - curret_stamp + RTC_TIMER_MAX_VALUE;
    } else {
        clk_delta = curret_stamp - rf_sync_start_stamp;
    }

    return clk_delta;
}

 __attribute__((section(".highcode")))
static inline uint32_t rf_sync_get_upper_deviation(void)
{
    uint32_t ud;

    //TODO: Configurable time.
    ud = (MS_TO_RTC(CONFIG_RF_SYNC_UNIT_MS) -
            (rf_sync_get_delta_stamp() % MS_TO_RTC(CONFIG_RF_SYNC_UNIT_MS))) %
                    MS_TO_RTC(CONFIG_RF_SYNC_UNIT_MS);

    return ud;
}

 __attribute__((section(".highcode")))
void rf_sync(void)
{
    if(rf_sync_get_upper_deviation() != 0)
        return;

    //open reveive window  
    if(rf_port_is_data_loaded())
    {
#ifdef CONFIG_RF_HOP
        rf_hop();
#endif
        rf_port_rcv_open(CONFIG_RF_DEV_RCV_WINDOW_US);  
        tmos_start_task(rf_sync_task_id, RF_PORT_HEARTBEAT_SET_EVT, MS1_TO_SYSTEM_TIME(CONFIG_RF_HB_INTERVAL_MS));
    }
}

uint16_t rf_sync_event(uint8 task_id, uint16 events)
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

    if (events & RF_PORT_HEARTBEAT_SET_EVT) {
        //if(is_sync)
        if(!rf_port_is_data_loaded()){
            rf_port_load_data(RF_TYPE_CMD_HB, NULL, 0);
        }
            
        return (events ^ RF_PORT_HEARTBEAT_SET_EVT);
    }

    return 0;
}

void rf_sync_init(void)
{
    sync_interval = CONFIG_RF_SYNC_UNIT_MS;
    rf_sync_task_id = TMOS_ProcessEventRegister(rf_sync_event);
}
