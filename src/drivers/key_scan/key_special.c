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
#include "led_task/led_task.h"
#include "USB/usbuser.h"
#include "RingBuffer/lwrb.h"
#include "HAL_FLASH/include/easyflash.h"
#include "BLE/hidkbd.h"
#include "mode/mode.h"
#include "key_parse.h"
#include "sys/util.h"
#include "key_codes.h"
#include "key_util.h"

struct speical_data {
    bool key_state;     // 1: press ; 0: release
    bool long_key_flag;
};

/*
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

*/
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

void send_consumer_key(struct speical_data *data, uint16_t usage_id)
{
    uint8_t buf[2] = {0};
    uint8_t report_id = CONSUME_ID;

    if (data->long_key_flag) {
        return ;
    }

    PRINT("Consumer %u: %s\n", usage_id, data->key_state ? "pressed" : "released");

    if (data->key_state) {
        buf[0] = LO_UINT16(usage_id);
        buf[1] = HI_UINT16(usage_id);
    }

    if(lwrb_get_free(&KEY_buff) >= (2 + 1)) {
        lwrb_write(&KEY_buff, &report_id, 1);
        lwrb_write(&KEY_buff, buf, 2);
    }
}

void switch_ble_channel(struct speical_data *data, uint8_t channel)
{
    bool needs_mode_change = false;
    if (channel > 4) {
        return ;
    }
    if (data->key_state) {
        return ;
    }

    if (device_mode != MODE_BLE)
    {
        device_mode = MODE_BLE;
        SaveDeviceInfo("device_mode");
        needs_mode_change = true;
    }

    PRINT("%s: %s\n", __FUNCTION__, data->long_key_flag ? "long" : "short");
    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->long_key_flag) {
        ef_get_env_blob("device_bond", &device_bond, sizeof(device_bond), NULL);

        PRINT("Channel %d current MAC: ( ", channel);

        for (int i = 0; i < 6; i++) {
            PRINT("%#x ", device_bond.ID[channel-1].local_addr[i]);
        }
        PRINT(")\n");

        device_bond.ID[channel-1].isbond = false;
        device_bond.ID_Num = channel-1;
        device_bond.ID[channel-1].local_addr[2]++;
        SaveDeviceInfo("device_bond");
        needs_mode_change = true;

    } else {
        /* clear MAC++ if long key pressed */
        ReadDeviceInfo("device_bond");

        if (device_bond.ID_Num != channel-1) {
            device_bond.ID_Num = channel-1;
            SaveDeviceInfo("device_bond");
            needs_mode_change = true;
        }
    }
    if(needs_mode_change) mode_select(MODE_BLE);
}

bool special_key_handler(uint8_t *keys_8, bool is_long)
{
    bool have_specia_key = false;
    uint32_t spk_flag_curr= 0;
    static uint32_t spk_flag_before = 0;
    struct speical_data data = {0};
    static uint8_t saved_flag = 0xff;

    for (int i = 1; i < 8; i++) {
        bool is_speical_key = keys_8[i] > HID_KEY_SPECIAL_START_INDEX;
        if (!is_speical_key) continue;
        have_specia_key = true;
        spk_flag_curr |=  BIT(keys_8[i]-HID_KEY_SPECIAL_START_INDEX-1);
        keys_8[i] = 0;
    }
    if (spk_flag_curr) {
        for (int i = 0 ; i < HID_KEY_MODE_USB-HID_KEY_SPECIAL_START_INDEX-1; i++) {
            uint8_t hid_key = i+HID_KEY_SPECIAL_START_INDEX+1;
            // 0 -> 1
            if (spk_flag_curr & BIT(i)) {
                if (!(spk_flag_before & BIT(i))) {
                    // 避免长按导致键值再次触发
                    if (!is_long) {
                        saved_flag = i;
                        data.key_state = true;
                        send_consumer_key(&data, consumer_key_table[i]);
                    }
                }
            } else {
                // 1 -> 0
                if (spk_flag_before & BIT(i)) {
                    saved_flag = i;
                    data.key_state = false;
                    send_consumer_key(&data, consumer_key_table[i]);
                }
            }
        }
        spk_flag_before = spk_flag_curr;
        for (int i = HID_KEY_MODE_USB-HID_KEY_SPECIAL_START_INDEX-1 ; i < HID_KEY_FN-HID_KEY_SPECIAL_START_INDEX-1; i++) {
            uint8_t hid_key = i+HID_KEY_SPECIAL_START_INDEX+1;
            if (spk_flag_curr & BIT(i)) {
                data.long_key_flag = is_long;
                switch (hid_key){
                case HID_KEY_MODE_USB:
                    mode_usb(&data);
                    break;
                case HID_KEY_MODE_BLE_CHAN_1:
                    switch_ble_channel(&data, 1);
                    break;
                case HID_KEY_MODE_BLE_CHAN_2:
                    switch_ble_channel(&data, 2);
                    break;
                case HID_KEY_MODE_BLE_CHAN_3:
                    switch_ble_channel(&data, 3);
                    break;
                case HID_KEY_MODE_BLE_CHAN_4:
                    switch_ble_channel(&data, 4);
                    break;
                default:
                    break;
                }
                // 清除标志防止切换后再次触发
                saved_flag = 0xff;

                // 模式切换后，键值无需上传
                return false;
            }
        }
    } else {
        spk_flag_before = 0;

        // Release the special keys if none is pressed
        if (saved_flag < HID_KEY_MODE_USB-HID_KEY_SPECIAL_START_INDEX-1) {
            data.key_state = false;
            data.long_key_flag = is_long;
            send_consumer_key(&data, consumer_key_table[saved_flag]);
            saved_flag = 0xff;
        }

    }

    // Get rid of 0s in the middle
    CompactIntegers(keys_8+2,6);

    return true;
}


