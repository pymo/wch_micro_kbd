/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "CH58x_common.h"
#include "key_special.h"
#include "config.h"
#include "device_config.h"
#include "RF_task/rf_dev.h"
#include "HAL/HAL.h"
#include "backlight/backlight.h"
#include "USB/usbuser.h"
#include "RingBuffer/lwrb.h"
#include "HAL_FLASH/include/easyflash.h"
#include "BLE/hidkbd.h"
#include "mode/mode.h"
#include "key_parse.h"
#include "sys/util.h"

enum special_key_flag {
    SPECIAL_KEY_MODE_RF24,
    SPECIAL_KEY_MODE_BLE,
    SPECIAL_KEY_MODE_USB,
    SPECIAL_KEY_MODE_TEST,
    SPECIAL_KEY_RF_PAIR,
    SPECIAL_KEY_VOL_MUTE,
    SPECIAL_KEY_VOL_DEC,
    SPECIAL_KEY_VOL_INC,
    SPECIAL_KEY_LED_ENH,
    SPECIAL_KEY_LED_WEAK,
    SPECIAL_KEY_LED_SWIT,
    SPECIAL_KEY_BLE_CHAN_1,
    SPECIAL_KEY_BLE_CHAN_2,
    SPECIAL_KEY_BLE_CHAN_3,
    SPECIAL_KEY_BLE_CHAN_4,

    SPECIAL_KEY_NUM,
};

struct speical_data {
    bool key_state;     // 1: press ; 0: release
    bool long_key_flag;
};

struct special_handler {
	uint8_t flag;
	void (*handler)(struct speical_data *data);
};

#define SPECIAL_HANDLER(_flag, _handler) \
{ \
	.flag = _flag, \
	.handler = _handler, \
}


uint8_t key_spe_taskid = 0;

static const uint8_t special_key_map[SPECIAL_KEY_NUM] = {
//  Scroll-Lock, Print-screen, Pause, 0, 5
    0x47, 0x46, 0x48, 0x27, 0x22, 
//  Mute, Decre, Incre
    0x3a, 0x3b, 0x3c,
//  F6,   F7,   F8 
    0x3f, 0x40, 0x41,
//  1,    2,    3,    4
    0x1e, 0x1f, 0x20, 0x21
};




static void mode_rf24(struct speical_data *data)
{
    if (data->key_state) {
        return ;
    }

    if (device_mode == MODE_RF24)
    {
        return ;
    }

    device_mode = MODE_RF24;
    SaveDeviceInfo("device_mode");

    mode_select(device_mode);
}

static void mode_ble(struct speical_data *data)
{
    if (data->key_state) {
        return ;
    }

    if (device_mode == MODE_BLE)
    {
        return ;
    }

    device_mode = MODE_BLE;
    SaveDeviceInfo("device_mode");

    mode_select(device_mode);
}

static void mode_usb(struct speical_data *data)
{
    if (device_mode == MODE_USB)
    {
        return ;
    }

    device_mode = MODE_USB;
    SaveDeviceInfo("device_mode");

    mode_select(device_mode);
}

static void mode_test(struct speical_data *data)
{
    ARG_UNUSED(data);
}

static void rf_pair(struct speical_data *data)
{
    if (data->key_state) {
        return ;
    }

    if (device_mode == MODE_RF24) {
        PRINT("Pair function enable.\n");
        OnBoard_SendMsg(rf_dev_taskid, RF_PAIR_MESSAGE, 1, NULL);    
    }
}

static void vol_mute(struct speical_data *data)
{
    uint8_t buf[2] = {0};
    uint8_t report_id = CONSUME_ID;

    if (data->long_key_flag) {
        return ;
    }

    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->key_state) {
        buf[0] = 0xe2;
    }

    if(lwrb_get_free(&KEY_buff) >= (2 + 1)) {
        lwrb_write(&KEY_buff, &report_id, 1);
        lwrb_write(&KEY_buff, buf, 2);
    }
}

static void vol_dec(struct speical_data *data)
{
    uint8_t buf[2] = {0};
    uint8_t report_id = CONSUME_ID;

    if (data->long_key_flag) {
        return ;
    }

    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->key_state) {
        buf[0] = 0xea;
    }

    if(lwrb_get_free(&KEY_buff) >= (2 + 1)) {
        lwrb_write(&KEY_buff, &report_id, 1);
        lwrb_write(&KEY_buff, buf, 2);
    }
}

static void vol_inc(struct speical_data *data)
{
    uint8_t buf[2] = {0};
    uint8_t report_id = CONSUME_ID;

    if (data->long_key_flag) {
        return ;
    }

    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->key_state) {
        buf[0] = 0xe9;
    }

    if(lwrb_get_free(&KEY_buff) >= (2 + 1)) {
        lwrb_write(&KEY_buff, &report_id, 1);
        lwrb_write(&KEY_buff, buf, 2);
    }
}

static void led_enh(struct speical_data *data)
{
    if (!data->key_state) {
        return ;        
    }

    PRINT("%s\n", __FUNCTION__);

    if (enhance_bk(BK_LINEALL)) {
        SaveDeviceInfo("device_led");
    }   
}

static void led_weak(struct speical_data *data)
{
    if (!data->key_state) {
        return ;        
    }
    PRINT("%s\n", __FUNCTION__);

    if (weaken_bk(BK_LINEALL)) {
            SaveDeviceInfo("device_led");
    }
}

static void led_swit(struct speical_data *data)
{
    if (!data->key_state) {
        return ;        
    }
    PRINT("%s\n", __FUNCTION__);

    if (device_led.led_en) {
        device_led.led_en = false;
        disbale_bk(BK_LINEALL);
        SaveDeviceInfo("device_led");
    } else {
        device_led.led_en = true;
        set_bk(BK_LINEALL, device_led.led_level);
        SaveDeviceInfo("device_led");
    }  
}

static void ble_chan_1(struct speical_data *data)
{
    if (device_mode != MODE_BLE) {
        return ;
    }
    PRINT("%s: %s\n", __FUNCTION__, data->long_key_flag ? "long" : "short");
    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->long_key_flag) {
        ef_get_env_blob("device_bond", &device_bond, sizeof(device_bond), NULL);

        PRINT("Channel 1 current MAC: ( ");

        for (int i = 0; i < 6; i++) {
            PRINT("%#x ", device_bond.ID[0].local_addr[i]);
        }
        PRINT(")\n");

        device_bond.ID[0].isbond = false;
        device_bond.ID_Num = 0;
        device_bond.ID[0].local_addr[2]++;

    } else {
        /* clear MAC++ if long key pressed */
        ReadDeviceInfo("device_bond");

        if (device_bond.ID_Num == 0) {
            return ;
        }

        device_bond.ID_Num = 0;
        SaveDeviceInfo("device_bond");
    }
    mode_select(MODE_BLE);
}

static void ble_chan_2(struct speical_data *data)
{
    if (device_mode != MODE_BLE) {
        return ;
    }

    PRINT("%s: %s\n", __FUNCTION__, data->long_key_flag ? "long" : "short");
    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->long_key_flag) {
        ef_get_env_blob("device_bond", &device_bond, sizeof(device_bond), NULL);

        PRINT("Channel 2 current MAC: ( ");

        for (int i = 0; i < 6; i++) {
            PRINT("%#x ", device_bond.ID[1].local_addr[i]);
        }
        PRINT(")\n");

        device_bond.ID[1].isbond = false;
        device_bond.ID_Num = 1;
        device_bond.ID[1].local_addr[2]++;
    } else {
        /* clear MAC++ if long key pressed */
        ReadDeviceInfo("device_bond");

        if (device_bond.ID_Num == 1) {
            return ;
        }

        device_bond.ID_Num = 1;
        SaveDeviceInfo("device_bond");
    }

    mode_select(MODE_BLE);  
}

static void ble_chan_3(struct speical_data *data)
{
    if (device_mode != MODE_BLE) {
        return ;
    }

    PRINT("%s: %s\n", __FUNCTION__, data->long_key_flag ? "long" : "short");
    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->long_key_flag) {
        ef_get_env_blob("device_bond", &device_bond, sizeof(device_bond), NULL);

        PRINT("Channel 3 current MAC: ( ");

        for (int i = 0; i < 6; i++) {
            PRINT("%#x ", device_bond.ID[2].local_addr[i]);
        }
        PRINT(")\n");

        device_bond.ID[2].isbond = false;
        device_bond.ID_Num = 2;
        device_bond.ID[2].local_addr[2]++;
    } else {
        /* clear MAC++ if long key pressed */
        ReadDeviceInfo("device_bond");

        if (device_bond.ID_Num == 2) {
            return ;
        }

        device_bond.ID_Num = 2;
        SaveDeviceInfo("device_bond");
    }

    mode_select(MODE_BLE);  
}

static void ble_chan_4(struct speical_data *data)
{
    if (device_mode != MODE_BLE) {
        return ;
    }

    PRINT("%s: %s\n", __FUNCTION__, data->long_key_flag ? "long" : "short");
    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->long_key_flag) {
        ef_get_env_blob("device_bond", &device_bond, sizeof(device_bond), NULL);

        PRINT("Channel 4 current MAC: ( ");

        for (int i = 0; i < 6; i++) {
            PRINT("%#x ", device_bond.ID[3].local_addr[i]);
        }
        PRINT(")\n");

        device_bond.ID[3].isbond = false;
        device_bond.ID_Num = 3;
        device_bond.ID[3].local_addr[2]++;
    } else {
        /* clear MAC++ if long key pressed */
        ReadDeviceInfo("device_bond");

        if (device_bond.ID_Num == 3) {
            return ;
        }

        device_bond.ID_Num = 3;
        SaveDeviceInfo("device_bond");
    }
    mode_select(MODE_BLE);    
}

static const struct special_handler handler[] = {
    SPECIAL_HANDLER(SPECIAL_KEY_MODE_RF24, mode_rf24),
    SPECIAL_HANDLER(SPECIAL_KEY_MODE_BLE, mode_ble),
    SPECIAL_HANDLER(SPECIAL_KEY_MODE_USB, mode_usb),
    SPECIAL_HANDLER(SPECIAL_KEY_MODE_TEST, mode_test),
    SPECIAL_HANDLER(SPECIAL_KEY_RF_PAIR, rf_pair),
    SPECIAL_HANDLER(SPECIAL_KEY_VOL_MUTE, vol_mute),
    SPECIAL_HANDLER(SPECIAL_KEY_VOL_DEC, vol_dec),
    SPECIAL_HANDLER(SPECIAL_KEY_VOL_INC, vol_inc),
    SPECIAL_HANDLER(SPECIAL_KEY_LED_ENH, led_enh),
    SPECIAL_HANDLER(SPECIAL_KEY_LED_WEAK, led_weak),
    SPECIAL_HANDLER(SPECIAL_KEY_LED_SWIT, led_swit),
    SPECIAL_HANDLER(SPECIAL_KEY_BLE_CHAN_1, ble_chan_1),
    SPECIAL_HANDLER(SPECIAL_KEY_BLE_CHAN_2, ble_chan_2),
    SPECIAL_HANDLER(SPECIAL_KEY_BLE_CHAN_3, ble_chan_3),
    SPECIAL_HANDLER(SPECIAL_KEY_BLE_CHAN_4, ble_chan_4),
};


static inline uint8_t is_speical_key(uint8_t *keys_8, uint8_t *key_list_special)
{
    uint8_t idx = 0;
    uint8_t key_idx = 0;

    uint8_t num = 0;
    /* is fn pressed */
    if (key_fn_flag) {
        key_fn_flag = 0;

        for (key_idx = 2; key_idx < 8; key_idx++) {
            // 键值为0 则后面无更多键值
            if (!keys_8[key_idx]) {
                break;
            }

            for (idx = 0; idx < SPECIAL_KEY_NUM; idx++) {
                if (keys_8[key_idx] == special_key_map[idx]) {
                    key_list_special[num++] = idx;
                    keys_8[key_idx] = 0;
                }
            }
        }

        return num;

    }

    return 0;
}

bool special_key_handler(uint8_t *keys_8, bool is_long)
{
    uint8_t spk_num = 0;
    uint8_t spk_buffer[6] = {0};

    static uint8_t saved_flag = 0xff;
    struct speical_data data = {0};

    uint32_t spk_flag_curr= 0;
    static uint32_t spk_flag_before = 0;

    spk_num = is_speical_key(keys_8, spk_buffer);

    if (spk_num) {

        for (int i = 0; i < spk_num; i++) {
            spk_flag_curr |=  BIT(spk_buffer[i]);
        }

        for (int i = 0 ; i < SPECIAL_KEY_BLE_CHAN_1; i++) {
            // 0 -> 1
            if (spk_flag_curr & BIT(i)) {
                if (!(spk_flag_before & BIT(i))) {
                    // 避免长按导致键值再次触发
                    if (!is_long) {
                        data.key_state = true;
                        saved_flag = i;
                        handler[saved_flag].handler(&data);
                    }
                } 
            } else {
                // 1 -> 0
                if (spk_flag_before & BIT(i)) {
                    data.key_state = false;
                    saved_flag = i;
                    handler[saved_flag].handler(&data);
                } 
            }
        }

        spk_flag_before = spk_flag_curr;

        for (int i = SPECIAL_KEY_BLE_CHAN_1; i < SPECIAL_KEY_NUM; i++) {
            if (spk_flag_curr & BIT(i)) {
                saved_flag = i;
                data.long_key_flag = is_long;
                handler[saved_flag].handler(&data);
                // 清除标志防止切换后再次触发
                saved_flag = 0xff;

                // 模式切换后，键值无需上传
                return false;
            }
        }

    } else {
        spk_flag_before = 0;
        
        // 无特殊键， 若特殊键未上传释放键则上传
        if (saved_flag < SPECIAL_KEY_NUM) {
            data.key_state = false;
            data.long_key_flag = is_long;

            handler[saved_flag].handler(&data);
            saved_flag = 0xff;
        }

    }

    // 去除中间0
    for(int i = 2; i < 8; i++) {
        if (!keys_8[i]) {
            for (int j = i + 1; j < 8; j++) {
                if (keys_8[j]) {
                    keys_8[i] = keys_8[j];
                    keys_8[j] = 0;
                    break;
                }
            }
        }
    }

    return true;
}




