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

enum special_key_flag {
    SPECIAL_KEY_VOL_DEC,
    SPECIAL_KEY_VOL_INC,
    SPECIAL_KEY_BRIGHTNESS_DEC,
    SPECIAL_KEY_BRIGHTNESS_INC,
    SPECIAL_KEY_HOME,
    SPECIAL_KEY_BACK,
    SPECIAL_KEY_MODE_USB,
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

#ifdef KEYBOARD_TYPE_G750
static const uint8_t special_key_map[SPECIAL_KEY_NUM] = {
        HID_KEY_P,  // SPECIAL_KEY_VOL_DEC,
        HID_KEY_SEMICOLON,  // SPECIAL_KEY_VOL_INC,
        HID_KEY_COMMA,  // SPECIAL_KEY_BRIGHTNESS_DEC,
        HID_KEY_PERIOD,  // SPECIAL_KEY_BRIGHTNESS_INC,
        HID_KEY_GUI_RIGHT,  // SPECIAL_KEY_HOME, not present in keyboard, just a placeholder
        HID_KEY_WWW_HOME,  // SPECIAL_KEY_BACK,
        HID_KEY_M,  // SPECIAL_KEY_MODE_USB
        HID_KEY_J,  // SPECIAL_KEY_BLE_CHAN_1,
        HID_KEY_K,  // SPECIAL_KEY_BLE_CHAN_2,
        HID_KEY_L,  // SPECIAL_KEY_BLE_CHAN_3,
        HID_KEY_U,  // SPECIAL_KEY_BLE_CHAN_4,
};
#endif

#ifdef KEYBOARD_TYPE_PPK
static const uint8_t special_key_map[SPECIAL_KEY_NUM] = {
        HID_KEY_ARROW_DOWN,  // SPECIAL_KEY_VOL_DEC,
        HID_KEY_ARROW_UP,  // SPECIAL_KEY_VOL_INC,
        HID_KEY_ARROW_LEFT,  // SPECIAL_KEY_BRIGHTNESS_DEC,
        HID_KEY_ARROW_RIGHT,  // SPECIAL_KEY_BRIGHTNESS_INC,
        HID_KEY_GUI_LEFT,  // SPECIAL_KEY_HOME,
        HID_KEY_GUI_RIGHT,  // SPECIAL_KEY_BACK, not present in keyboard, just a placeholder
        HID_KEY_U,  // SPECIAL_KEY_MODE_USB
        HID_KEY_A,  // SPECIAL_KEY_BLE_CHAN_1,
        HID_KEY_S,  // SPECIAL_KEY_BLE_CHAN_2,
        HID_KEY_D,  // SPECIAL_KEY_BLE_CHAN_3,
        HID_KEY_F,  // SPECIAL_KEY_BLE_CHAN_4,
};
#endif

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

    PRINT("%s: %s\n", __FUNCTION__, data->key_state ? "pressed" : "released");

    if (data->key_state) {
        buf[0] = LO_UINT16(usage_id);
        buf[1] = HI_UINT16(usage_id);
    }

    if(lwrb_get_free(&KEY_buff) >= (2 + 1)) {
        lwrb_write(&KEY_buff, &report_id, 1);
        lwrb_write(&KEY_buff, buf, 2);
    }
}

static void vol_dec(struct speical_data *data){
    send_consumer_key(data, HID_USAGE_CONSUMER_VOLUME_DEC);
}

static void vol_inc(struct speical_data *data){
    send_consumer_key(data, HID_USAGE_CONSUMER_VOLUME_INC);
}

static void brightness_dec(struct speical_data *data){
    send_consumer_key(data, HID_USAGE_CONSUMER_BRIGHTNESS_DEC);
}

static void brightness_inc(struct speical_data *data){
    send_consumer_key(data, HID_USAGE_CONSUMER_BRIGHTNESS_INC);
}

static void www_back(struct speical_data *data){
    send_consumer_key(data, HID_USAGE_CONSUMER_WWW_BACK);
}

static void www_home(struct speical_data *data){
    send_consumer_key(data, HID_USAGE_CONSUMER_WWW_HOME);
}

/*
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
*/
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

static void ble_chan_1(struct speical_data *data){
    switch_ble_channel(data, 1);
}
static void ble_chan_2(struct speical_data *data){
    switch_ble_channel(data, 2);
}

static void ble_chan_3(struct speical_data *data){
    switch_ble_channel(data, 3);
}

static void ble_chan_4(struct speical_data *data){
    switch_ble_channel(data, 4);
}

static const struct special_handler handler[] = {
    SPECIAL_HANDLER(SPECIAL_KEY_VOL_DEC, vol_dec),
    SPECIAL_HANDLER(SPECIAL_KEY_VOL_INC, vol_inc),
    SPECIAL_HANDLER(SPECIAL_KEY_BRIGHTNESS_DEC, brightness_dec),
    SPECIAL_HANDLER(SPECIAL_KEY_BRIGHTNESS_INC, brightness_inc),
    SPECIAL_HANDLER(SPECIAL_KEY_HOME, www_home),
    SPECIAL_HANDLER(SPECIAL_KEY_BACK, www_back),
    SPECIAL_HANDLER(SPECIAL_KEY_MODE_USB, mode_usb),
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
        //key_fn_flag = 0;

#ifdef KEYBOARD_TYPE_PPK
        // For PPK, Fn+LEFTGUI=WWW_HOME, so we need to extract the LEFTGUI status from the first byte.
        if (keys_8[0] & KEYBOARD_MODIFIER_LEFTGUI) {
            key_list_special[num++] = SPECIAL_KEY_HOME;
            keys_8[0] &= (~KEYBOARD_MODIFIER_LEFTGUI);
        }
#endif
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

        // Release the special keys if none is pressed
        if (saved_flag < SPECIAL_KEY_NUM) {
            data.key_state = false;
            data.long_key_flag = is_long;

            handler[saved_flag].handler(&data);
            saved_flag = 0xff;
        }

    }

    // Get rid of 0s in the middle
    CompactIntegers(keys_8+2,6);

    return true;
}

void handle_first_layer_special_key(uint8_t *keys_8, bool is_long){
    bool special_key_pressed = false;
    static bool special_key_previously_pressed = false;
    for(int i = 2; i < 8; i++) {
        if (keys_8[i]==HID_KEY_WWW_HOME) {
            struct speical_data data = {0};
            data.key_state = true;
            data.long_key_flag = is_long;
            send_consumer_key(&data, HID_USAGE_CONSUMER_WWW_HOME);
            keys_8[i]=0;
            special_key_pressed = true;
            special_key_previously_pressed = true;
        }
    }
    // Release the special keys if none is pressed
    if(!special_key_pressed && special_key_previously_pressed){
        struct speical_data data = {0};
        data.key_state = false;
        data.long_key_flag = is_long;
        send_consumer_key(&data, HID_USAGE_CONSUMER_WWW_HOME);
        special_key_previously_pressed = false;
    }
    // Get rid of 0s in the middle
    CompactIntegers(keys_8+2,6);
}


