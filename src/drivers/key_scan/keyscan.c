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

__HIGH_CODE
void RstAllPins(void)
{/*
    for (uint8_t i = 0; i < 17; i++)
    {
        i == 10 ? GPIOA_ResetBits(IOmap[i]) : GPIOB_ResetBits(IOmap[i]);
    }*/
}

__HIGH_CODE
void SetAllPins(void)
{/*
    for (uint8_t i = 0; i < 17; i++)
    {
        i == 10 ? GPIOA_SetBits(IOmap[i]) : GPIOB_SetBits(IOmap[i]);
    }*/
}

void keyInit(void)
{
    /* 配置串口0：先配置IO口模式，再配置串口 */
    GPIOB_SetBits(GPIO_Pin_7);
    GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);      // RXD-配置上拉输入
    GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA); // TXD-配置推挽输出，注意先让IO口输出高电平
    UART0_DefInit();
    UART0_BaudRateCfg(9600);

    GPIOA_ModeCfg(RTS_PIN|DCD_PIN, GPIO_ModeIN_Floating);
    GPIOB_SetBits(VCC_PIN);
    GPIOB_ModeCfg(VCC_PIN, GPIO_ModeOut_PP_5mA); // VCC_CTL配置推挽输出，注意先让IO口输出高电平
    DelayMs(1500);
    GPIOB_ResetBits(VCC_PIN);
    PRINT("beginning keyboard boot sequence...\n");
    while (!GPIOA_ReadPortPin(DCD_PIN)) {
      DelayMs(1);
    };
    PRINT("DCD_PIN response done.\n");
    if (!GPIOA_ReadPortPin(RTS_PIN)) {
        PRINT("RTS_PIN low.\n");
        DelayMs(10);
        GPIOA_SetBits(RTS_PIN);
        GPIOA_ModeCfg(RTS_PIN, GPIO_ModeOut_PP_5mA);
    } else {
        PRINT("RTS_PIN high.\n");
        GPIOA_ResetBits(RTS_PIN);
        GPIOA_ModeCfg(RTS_PIN, GPIO_ModeOut_PP_5mA);
      DelayMs(800);
      GPIOA_SetBits(RTS_PIN);
    }
    PRINT("waiting for keyboard serial ID... \n");
}

void keyPress(uint8_t *pbuf, uint8_t *key_num, uint8_t raw_keycode){
    for (uint8_t i = 0; i < *key_num; ++i) {
        if(raw_keycode==pbuf[i]) return;
    }
    pbuf[*key_num]=raw_keycode;
    (*key_num)++;
}

void keyRelease(uint8_t *pbuf, uint8_t *key_num, uint8_t raw_keycode){
    for (uint8_t i = 0; i < *key_num; ++i) {
        if(raw_keycode==pbuf[i]){
            pbuf[i]=0;
            // Remove the item at i and move the following items (if there are any) forward.
            *key_num = CompactIntegers(pbuf, *key_num);
            return;
        }
    }
}

// Reset the key buffers
void keyReset(uint8_t *pbuf, uint8_t *key_num){
    for (uint8_t i = 0; i < *key_num; ++i) {
            pbuf[i]=0;
    }
    *key_num = 0;
}

void keyScan(uint8_t *pbuf, uint8_t *key_num)
{
    static uint8_t keyboard_rx_buff[100];
    static uint8_t keyboard_rx_buff_len=0;
    static uint8_t last_byte=0;
    // Append the newly read data to the end of the previous data (if there is any left).
    uint8_t len = UART0_RecvString(keyboard_rx_buff+keyboard_rx_buff_len);
    keyboard_rx_buff_len += len;
    uint8_t i = 0;

    for(i = 0;i < keyboard_rx_buff_len;i++)
    {
        PRINT("byte from keyboard: %#x \n", keyboard_rx_buff[i]);
    }

#ifdef KEYBOARD_TYPE_G750
    for(i = 0;i+1 < keyboard_rx_buff_len;i++)
    {
        uint8_t raw_keycode = keyboard_rx_buff[i]&0b01111111;
        if (raw_keycode^keyboard_rx_buff[i+1]==0b11111111){
            bool key_up = keyboard_rx_buff[i]&0b10000000;
            if (key_up) keyRelease(pbuf, key_num, raw_keycode);
            else keyPress(pbuf, key_num, raw_keycode);
            i++;
        }
    }
    // The incoming keycode is always 2 bytes, so if this is the last byte, we have to wait until the next UART read to get the second byte (or something is wrong)
    if (i+1==keyboard_rx_buff_len){
        keyboard_rx_buff[0]=keyboard_rx_buff[i];
        keyboard_rx_buff_len = 1;
    }else{
        keyboard_rx_buff_len = 0;
    }
#endif

#ifdef KEYBOARD_TYPE_PPK
    for(i = 0;i < keyboard_rx_buff_len;i++)
    {
        uint8_t raw_keycode = keyboard_rx_buff[i]&0b01111111;
        if (raw_keycode == 0) raw_keycode = 27;
        bool key_up = keyboard_rx_buff[i]&0b10000000;
        if(last_byte==keyboard_rx_buff[i]){
            // keyboard duplicates the final key-up byte
            keyReset(pbuf, key_num);
        } else {
            if (key_up){
                keyRelease(pbuf, key_num, raw_keycode);
            } else {
                keyPress(pbuf, key_num, raw_keycode);
            }
            last_byte =  keyboard_rx_buff[i];
        }
    }
    keyboard_rx_buff_len = 0;
#endif
}

int readKeyVal(void)
{
    static uint8_t current_key_map[120] = {0};
    static uint8_t last_key_map[120] = {0};
    static uint8_t key_num = 0;
    uint8_t curruent_key_8[8] = {0};
    uint8_t curruent_key_16[16] = {0};
    static uint8_t last_key_8[8] = {0};
    static uint8_t last_key_16[16] = {0};

    keyScan(current_key_map, &key_num);
    // TODO: what does return -1 mean?
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
    handle_first_layer_special_key(curruent_key_8, is_long_key_trigged);

    if (is_long_key_trigged) {
        return 0;
    }

     PRINT("key8=[");
     for(int i = 0; i < 8; i++) {
         if(i) PRINT(" ");
         PRINT("%#x", curruent_key_8[i]);
     }
     PRINT("]\n");

     PRINT("key16=[");
     for(int i = 0; i < 16; i++) {
         if(i) PRINT(" ");
         PRINT("%#x", curruent_key_16[i]);
     }
     PRINT("]\n\n");

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
