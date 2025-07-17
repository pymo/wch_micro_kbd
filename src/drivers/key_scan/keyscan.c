#include "keyscan.h"
#include "key_parse.h"
#include "soc.h"
#include "RingBuffer/lwrb.h"
#include "config.h"
#include "device_config.h"
#include "led_task/led_task.h"
#include "USB_task/USB_kbd.h"
#include "RF_task/rf_dev.h"
#include "PM/pm_task.h"
#include "HAL/HAL.h"
#include "USB/usbuser.h"
#include "key_special.h"
#include "key_util.h"
#include "BLE/hidkbd.h"

#include "keyscan_ultrathin.h"
#include "keyscan_ppk.h"
#include "keyscan_g750.h"

KeyboardBootState k_state = K_STATE_0_START;
static uint8_t current_key_map[MAX_KEY_NUM] = { 0 };

__HIGH_CODE
void RstAllPins(void) {
    GPIOB_ResetBits(LED_RED|LED_GREEN|LED_BLUE);      // Turn off LED
}

__HIGH_CODE
void SetAllPins(void) {
}

void ReportKeys(void) {
        switch (device_mode) {
        case MODE_BLE:
            pm_start_working(PM_WORKING_TIMEOUT, PM_IDLE_TIMEOUT);
            OnBoard_SendMsg(hidEmuTaskId, RF_MS_STATE_CHANGE, PM_STATE_ACTIVE,
            NULL);
            OnBoard_SendMsg(hidEmuTaskId, KEY_MESSAGE, 1, NULL);
            break;
        case MODE_RF24:
            pm_start_working(PM_WORKING_TIMEOUT, PM_IDLE_TIMEOUT);
            OnBoard_SendMsg(rf_dev_taskid, KEY_MESSAGE, 1, NULL);
            break;

        case MODE_USB:
            /* USB don't sleep */
            pm_start_working(PM_TIMEOUT_FOREVER, PM_TIMEOUT_FOREVER);
            OnBoard_SendMsg(USBTaskID, KEY_MESSAGE, 1, NULL);
            break;

        default:
            pm_start_working(PM_TIMEOUT_FOREVER, PM_TIMEOUT_FOREVER);
            break;
        }
}

int ScanKeyAndGenerateReport(uint8_t *current_key_map, uint8_t key_num) {
    static uint8_t last_key_map[MAX_KEY_NUM] = { 0 };
    uint8_t curruent_key_8[8] = { 0 };
    uint8_t curruent_key_16[16] = { 0 };
    static uint8_t last_key_8[8] = { 0 };
    static uint8_t last_key_16[16] = { 0 };

    // TODO: what does return -1 mean?
    if (key_num == 0xff) {
        return -1;
    }
    //Checks if it is long press key
    bool is_key;
    static bool is_long_key_trigged = false;

    is_key = (key_num!=0);

    static uint32_t last_time = 0;
    uint32_t current_time = get_current_time();

    if (tmos_memcmp(current_key_map, last_key_map,
            MAX_KEY_NUM) == true) {
        if (!is_key || (current_time - last_time <= LONGKEY_TIME)
                || is_long_key_trigged) {
            return 0;
        }
        is_long_key_trigged = true;

        PRINT("long key triggered.\n");
    } else {
        is_long_key_trigged = false;
    }
    // Back up the current_key_map into last_key_map
    last_time = current_time;
    tmos_memcpy(last_key_map, current_key_map, MAX_KEY_NUM);
    // Parse raw keycodes into HID keycodes, including special keys
    int ret = key_parse(current_key_map, key_num, curruent_key_8,
            curruent_key_16);

    if (ret < 0)
        return -1;

    if (!special_key_handler(curruent_key_8, is_long_key_trigged)) {
        PRINT("special_key_handler returns false\n");
        ReportKeys();
        return true;
    }

    if (is_long_key_trigged) {
        return 0;
    }
#ifdef DEBUG
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
#endif
    if (tmos_memcmp(curruent_key_8, last_key_8, 8) != true) {
        if (lwrb_get_free(&KEY_buff) < 8 + 1)
            return -1;

        uint8_t report_id = KEYNORMAL_ID;

        lwrb_write(&KEY_buff, &report_id, 1);
        lwrb_write(&KEY_buff, curruent_key_8, 8);
    }

    if (tmos_memcmp(curruent_key_16, last_key_16, 16) != true) {
        if (lwrb_get_free(&KEY_buff) < 16 + 1)
            return -1;

        uint8_t report_id = KEYBIT_ID;

        lwrb_write(&KEY_buff, &report_id, 1);
        lwrb_write(&KEY_buff, curruent_key_16, 16);
    }

    memcpy(last_key_8, curruent_key_8, 8);
    memcpy(last_key_16, curruent_key_16, 16);
    ReportKeys();
    return true;
}

void PrintStringToHost(uint8_t *current_key_map, uint8_t *key_num, uint8_t *str, int len){
    for (int i=0;i<len;i++){
        uint8_t hid_code = AsciiToHidCode(str[i]);
        uint8_t raw_code = 0;
        for (int j=0;j<key8_table_size;j++){
            if(key8_table[j]==hid_code){
                raw_code = j;
                break;
            }
        }
        keyPress(current_key_map, key_num, raw_code);
        ScanKeyAndGenerateReport(current_key_map, *key_num);
        keyRelease(current_key_map, key_num, raw_code);
        ScanKeyAndGenerateReport(current_key_map, *key_num);
    }
}

void key_loop() {
    static uint32_t ms_last_state_change = 0;
    uint32_t ms_current = TMOS_GetSystemClock() * SYSTEM_TIME_MICROSEN  / 1000;
    if (ms_current < ms_last_state_change) {
        PRINT("ms_current decreased, set the ms_last_state_change to be the same.\n");
        ms_last_state_change = ms_current;
    }
    bool keyboard_initialized = k_state >= K_STATE_6_ID_RECEIVED;
    // Sets the LED state
    set_keyboard_init(!keyboard_initialized);

    if (!keyboard_initialized
            && ms_current > ms_last_state_change + INIT_FAILURE_TIMEOUT_MS) {
        PRINT("Failed to boot keyboard, retry...\n");
        k_state = K_STATE_0_START;
    }
    if(!keyboard_initialized) {
        KeyboardBootState state_before = k_state;
        InitKeyboard(&k_state, ms_current, ms_last_state_change);
        // If the state changes, remember the timestamp of it.
        if (k_state!=state_before){
            ms_last_state_change = ms_current;
        }
    } else {
        ms_last_state_change = ms_current;
        // Reboot if no recent keypress, otherwise keyboard falls asleep
        uint32 epoc_ms = TMOS_GetSystemClock() * SYSTEM_TIME_MICROSEN  / 1000;
        if (epoc_ms < last_pressed) {
            // Timer has been reinitialized since the last press event.
            // This could happen after BLE channel switch, simply reset the last_pressed value.
            PRINT("TMOS SystemClock has been reset, update last_pressed.\n");
            last_pressed = epoc_ms;
        } else {
#ifndef KEYBOARD_TYPE_ULTRATHIN
            if (epoc_ms - last_pressed > KEEPALIVE_TIMEOUT_MS) {
                PRINT("rebooting keyboard for timeout\n");
                last_pressed = epoc_ms;
                // Briefly gets TMOS out of sleep mode, otherwise Serial port does not work properly during the keyboard init.
                pm_start_working(PM_BRIEF_KEEPALIVE_TIMEOUT, PM_IDLE_TIMEOUT);
                k_state = K_STATE_0_START;
            }
#endif
        }
        uint8_t key_num = ScanRawKeycodes(current_key_map);
        // We still need to scan the keys even if no new keys are pressed, to handle the long press key.
        if (key_num!=KEYSCAN_NOT_READY){
          int ret = ScanKeyAndGenerateReport(current_key_map, key_num);
        }
    }
}
