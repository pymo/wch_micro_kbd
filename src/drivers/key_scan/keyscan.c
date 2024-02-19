#include "keyscan.h"
#include "key_parse.h"
#include "soc.h"
#include "RingBuffer/lwrb.h"
#include "config.h"
#include "device_config.h"
#include "RF_task/rf_dev.h"
#include "HAL/HAL.h"
#include "USB/usbuser.h"
#include "key_special.h"

#include "backlight/backlight.h"

static const uint32_t IOmap[] = {
    1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 14, 1 << 15, 1 << 16, 
    1 << 17, 1 << 8, 1 << 9, 1 << 8, 1 << 18, 1 << 19, 1 << 20,
    1 << 21, 1 << 22, 1 << 23
};

__HIGH_CODE
void RstAllPins(void)
{
    for (uint8_t i = 0; i < 17; i++)
    {
        i == 10 ? GPIOA_ResetBits(IOmap[i]) : GPIOB_ResetBits(IOmap[i]);
    }
}

__HIGH_CODE
void SetAllPins(void)
{
    for (uint8_t i = 0; i < 17; i++)
    {
        i == 10 ? GPIOA_SetBits(IOmap[i]) : GPIOB_SetBits(IOmap[i]);
    }
}

void keyInit(void)
{
    GPIOA_ModeCfg(
        GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5,
        GPIO_ModeIN_PU);
    SetAllPins();
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg(
        GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
            GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_16 | GPIO_Pin_17 | GPIO_Pin_18 |
            GPIO_Pin_19 | GPIO_Pin_20 | GPIO_Pin_21 | GPIO_Pin_22 | GPIO_Pin_23,
        GPIO_ModeOut_PP_5mA);
}

void keyScan(uint8_t *pbuf, uint8_t *key_num)
{
    uint8_t KeyNum;
    static uint8_t secbuf[120];

    uint8_t firstbuf[120] = { 0 };  //每一次扫描 firstbuf复位为0

    KeyNum = 0;

    for (uint8_t i = 0; i < 17; i++)
    {
        i == 10 ? GPIOA_ResetBits(IOmap[i]) : GPIOB_ResetBits(IOmap[i]);
        __nop();
        __nop(); //由于上拉输入拉低需要一定的时间，所以必须延时一段时间再读IO
        {
            if (Key_S0 == 0)
            {
                firstbuf[KeyNum++] = i * 7 + 1;
            }
            if (Key_S1 == 0)
            {
                firstbuf[KeyNum++] = i * 7 + 2;
            }
            if (Key_S2 == 0)
            {
                firstbuf[KeyNum++] = i * 7 + 3;
            }
            if (Key_S3 == 0)
            {
                firstbuf[KeyNum++] = i * 7 + 4;
            }
            if (Key_S4 == 0)
            {
                firstbuf[KeyNum++] = i * 7 + 5;
            }
            if (Key_S5 == 0)
            {
                firstbuf[KeyNum++] = i * 7 + 6;
            }
        }
        i == 10 ? GPIOA_SetBits(IOmap[i]) : GPIOB_SetBits(IOmap[i]);

        while (!(Key_S0 && Key_S1 && Key_S2 && Key_S3 && Key_S4 && Key_S5))
        {
            continue;
        }
    }

    //这一次与上一次键值相等 去抖动作用
    if (tmos_memcmp(firstbuf, secbuf, sizeof(firstbuf)) == true)
    {
        tmos_memcpy(pbuf, secbuf, sizeof(firstbuf));
        *key_num = KeyNum;
    }

    tmos_memcpy(secbuf, firstbuf, sizeof(firstbuf));
}


int readKeyVal(void)
{
    static uint8_t current_key_map[120] = {0};
    static uint8_t last_key_map[120] = {0};
    uint8_t key_num = 0xff;
    uint8_t curruent_key_8[8] = {0};
    uint8_t curruent_key_16[16] = {0};
    static uint8_t last_key_8[8] = {0};
    static uint8_t last_key_16[16] = {0};

    keyScan(current_key_map, &key_num);
    if(key_num == 0xff)
    {
        return -1;
    }

    bool is_key;
    static bool is_long_key_trigged = false;

    is_key = !tmos_isbufset(current_key_map, 0, sizeof(current_key_map));

    static uint32_t last_time = 0;
    uint32_t current_time = get_current_time();

    if (tmos_memcmp(current_key_map, last_key_map,
                    sizeof(current_key_map)) == true) { 
        if (!(is_key && (current_time - last_time > LONGKEY_TIME) &&
                    !is_long_key_trigged)) {
            return 0;
        }
        is_long_key_trigged = true;

        PRINT("long key trigged\n");
    } else {
        is_long_key_trigged = false;
    }

    last_time = current_time;
    tmos_memcpy(last_key_map, current_key_map, sizeof(current_key_map));

    int ret = key_parse(current_key_map, key_num, 
            curruent_key_8, curruent_key_16);

    if(ret < 0)
        return -1;


    if (!special_key_handler(curruent_key_8, is_long_key_trigged)) {

        return true;
    }

    if (is_long_key_trigged) {
        return 0;
    }

//     PRINT("key8=[");
//     for(int i = 0; i < 8; i++) {
//         if(i) PRINT(" ");
//         PRINT("%#x", curruent_key_8[i]);
//     }
//     PRINT("]\n");
//
//     PRINT("key16=[");
//     for(int i = 0; i < 16; i++) {
//         if(i) PRINT(" ");
//         PRINT("%#x", curruent_key_16[i]);
//     }
//     PRINT("]\n\n");

    if(tmos_memcmp(curruent_key_8, last_key_8, 8) != true) {
        if(lwrb_get_free(&KEY_buff) < 8 + 1)
            return -1;

        uint8_t report_id = KEYNORMAL_ID;

        lwrb_write(&KEY_buff, &report_id, 1);
        lwrb_write(&KEY_buff, curruent_key_8, 8);
    }

    if(tmos_memcmp(curruent_key_16, last_key_16, 16) != true) {
        if(lwrb_get_free(&KEY_buff) < 16 + 1)
            return -1;

        uint8_t report_id = KEYBIT_ID;

        lwrb_write(&KEY_buff, &report_id, 1);    
        lwrb_write(&KEY_buff, curruent_key_16, 16);  
    }

    memcpy(last_key_8, curruent_key_8, 8);
    memcpy(last_key_16, curruent_key_16, 16);

    return true;
}
