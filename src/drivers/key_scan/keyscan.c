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
#include "BLE/hidkbd.h"

uint32_t last_pressed = 0;
KEYBOARD_BOOT_STATE k_state = K_STATE_0_START;
bool is_first_boot = true;

__HIGH_CODE
void RstAllPins(void) {
    GPIOB_ResetBits(LED_YELLOW|LED_GREEN|LED_BLUE);      // Turn off LED
}

__HIGH_CODE
void SetAllPins(void) {/*
 //PWR_PeriphWakeUpCfg( DISABLE, RB_SLP_GPIO_WAKE, 0 );
 GPIOA_SetBits(bTXD1);
 GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
 UART1_DefInit();
 UART1_BaudRateCfg(921600);*/

}

bool ContainsKeyboardId(uint8_t* keyboard_id_buff, uint8_t keyboard_id_buff_len){
    for (uint8_t i = 0; i< keyboard_id_buff_len; ++i){
        if (keyboard_id_buff_len >= 2){
            if(keyboard_id_buff[keyboard_id_buff_len - 2] == KEYBOARD_ID0
                    && keyboard_id_buff[keyboard_id_buff_len - 1] == KEYBOARD_ID1) {
                return true;
            }
        }
    }
    return false;
}

int key_deal(void) {
    // Reboot if no recent keypress, otherwise keyboard falls asleep
    uint32 epoc_ms = TMOS_GetSystemClock() * SYSTEM_TIME_MICROSEN  / 1000;
    if (epoc_ms < last_pressed) {
        // Timer has been reinitialized since the last press event.
        // This could happen after BLE channel switch, simply reset the last_pressed value.
        PRINT("TMOS SystemClock has been reset, update last_pressed.\n");
        last_pressed = epoc_ms;
    } else if (epoc_ms - last_pressed > KEEPALIVE_TIMEOUT_MS) {
        PRINT("rebooting keyboard for timeout\n");
        last_pressed = epoc_ms;
        // Briefly gets TMOS out of sleep mode, otherwise Serial port does not work properly during the keyboard init.
        pm_start_working(PM_BRIEF_KEEPALIVE_TIMEOUT, PM_IDLE_TIMEOUT);
        k_state = K_STATE_0_START;
    }

    int ret = readKeyVal();
    if (ret > 0) {

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
    } else if (ret < 0) {
        //PRINT("read key error\n");
        return -1;
    }
    return 1;
}

void key_loop() {
    static uint32_t ms_last_state_change = 0;
    static uint8_t keyboard_id_buff[20];
    static uint8_t keyboard_id_buff_len = 0;
    uint32_t ms_current = TMOS_GetSystemClock() * SYSTEM_TIME_MICROSEN  / 1000;
    if (ms_current < ms_last_state_change) {
        PRINT(
                "ms_current decreased, set the ms_last_state_change to be the same.\n");
        ms_last_state_change = ms_current;
    }
    bool keyboard_initialized = k_state >= K_STATE_6_ID_RECEIVED;
    // Sets the LED state
    set_keyboard_init(!keyboard_initialized);

    if (!keyboard_initialized
            && ms_current > ms_last_state_change + INIT_FAILURE_TIMEOUT_MS) {
        PRINT("Failed to boot keyboard, retry...\n");
        k_state = K_STATE_0_START;
        keyboard_id_buff_len = 0;
    }
    switch (k_state) {
    case K_STATE_0_START:
        PRINT("beginning keyboard boot sequence...\n");
        ms_last_state_change = ms_current;
        /* 配置串口0：先配置IO口模式，再配置串口 */
        GPIOB_SetBits(GPIO_Pin_7);
        GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);      // RXD-配置上拉输入
        GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA); // TXD-配置推挽输出，TXD not used in this application.
        UART0_DefInit();
        UART0_BaudRateCfg(9600);
        GPIOA_ModeCfg(RTS_PIN | DCD_PIN, GPIO_ModeIN_Floating);
        GPIOB_SetBits(VCC_PIN);
        GPIOB_ModeCfg(VCC_PIN, GPIO_ModeOut_PP_5mA); // VCC_CTL配置推挽输出，注意先让IO口输出高电平
        k_state = K_STATE_1_OFF;
        break;
    case K_STATE_1_OFF:
        if (ms_current > ms_last_state_change + (is_first_boot)?KEYBOARD_REBOOT_DELAY_MS_LONG:KEYBOARD_REBOOT_DELAY_MS_SHORT) {
            ms_last_state_change = ms_current;
            GPIOB_ResetBits(VCC_PIN);
#ifdef PPK_TYPE_HANDSPRING
            // TODO(): This should be K_STATE_5_RTS_HIGH, but the hardware circuit has some issues when directly connecting RXD0, should change back to K_STATE_5_RTS_HIGH once we have 0ohm disconnect in the circuit.
            k_state = K_STATE_6_ID_RECEIVED;
            PRINT("Keyboard init complete.\n");
#else
            k_state = K_STATE_2_ON;
#endif
        }
        break;
    case K_STATE_2_ON:
        if ((ms_current > ms_last_state_change + 10)
                && GPIOA_ReadPortPin(DCD_PIN)) {
            PRINT("DCD_PIN response done.\n");
            ms_last_state_change = ms_current;
            if (GPIOA_ReadPortPin(RTS_PIN)) { // RTS high, needs to lower it first
                GPIOA_ResetBits(RTS_PIN);
                GPIOA_ModeCfg(RTS_PIN, GPIO_ModeOut_PP_5mA);
                k_state = K_STATE_3_DCD_RESPONDED;
            } else {
                GPIOA_ModeCfg(RTS_PIN, GPIO_ModeOut_PP_5mA);
                k_state = K_STATE_4_RTS_LOW;
            }
        }
        break;
    case K_STATE_3_DCD_RESPONDED:
        if (ms_current > ms_last_state_change + 200) {
            ms_last_state_change = ms_current;
            k_state = K_STATE_4_RTS_LOW;
        }
        break;
    case K_STATE_4_RTS_LOW:
        if (ms_current > ms_last_state_change + 10) {
            GPIOA_SetBits(RTS_PIN);
            PRINT("waiting for keyboard serial ID... \n");
            ms_last_state_change = ms_current;
            k_state = K_STATE_5_RTS_HIGH;
        }
        break;

    case K_STATE_5_RTS_HIGH:
        if (keyboard_id_buff_len >= 10) {
            PRINT(
                    "Serial buffer exhausted before keyboard serial ID received, reuse the buffer.\n");
            keyboard_id_buff_len = 0;
        }
        uint8_t len = UART0_RecvString(keyboard_id_buff + keyboard_id_buff_len);
        for (int i = 0; i < len; i++) {
            PRINT("byte from keyboard: %#x \n",
                    keyboard_id_buff[keyboard_id_buff_len + i]);
        }

        keyboard_id_buff_len += len;
            // Checks keyboard ID
            if (ContainsKeyboardId(keyboard_id_buff, keyboard_id_buff_len)){
                is_first_boot = false;
                last_pressed = ms_current;
                ms_last_state_change = ms_current;
                k_state = K_STATE_6_ID_RECEIVED;
                keyboard_id_buff_len = 0;
            }
        break;
    case K_STATE_6_ID_RECEIVED:
        ms_last_state_change = ms_current;
        key_deal();
        break;
    }

}

void keyPress(uint8_t *pbuf, uint8_t *key_num, uint8_t raw_keycode) {
    for (uint8_t i = 0; i < *key_num; ++i) {
        if (raw_keycode == pbuf[i])
            return;
    }
    pbuf[*key_num] = raw_keycode;
    (*key_num)++;
    last_pressed = TMOS_GetSystemClock() * SYSTEM_TIME_MICROSEN  / 1000;
}

void keyRelease(uint8_t *pbuf, uint8_t *key_num, uint8_t raw_keycode) {
    for (uint8_t i = 0; i < *key_num; ++i) {
        if (raw_keycode == pbuf[i]) {
            pbuf[i] = 0;
            // Remove the item at i and move the following items (if there are any) forward.
            *key_num = CompactIntegers(pbuf, *key_num);
            return;
        }
    }
}

// Reset the key buffers
void keyReset(uint8_t *pbuf, uint8_t *key_num) {
    for (uint8_t i = 0; i < *key_num; ++i) {
        pbuf[i] = 0;
    }
    *key_num = 0;
}

void keyScan(uint8_t *pbuf, uint8_t *key_num) {
    static uint8_t keyboard_rx_buff[100];
    static uint8_t keyboard_rx_buff_len = 0;
    static uint8_t last_byte = 0;
    // Append the newly read data to the end of the previous data (if there is any left).
    uint8_t len = UART0_RecvString(keyboard_rx_buff + keyboard_rx_buff_len);
    keyboard_rx_buff_len += len;
    uint8_t i = 0;

    for (i = 0; i < keyboard_rx_buff_len; i++) {
        PRINT("byte from keyboard: %#x \n", keyboard_rx_buff[i]);
    }

#ifdef KEYBOARD_TYPE_G750
    for(i = 0;i+1 < keyboard_rx_buff_len;i++)
    {
        uint8_t raw_keycode = keyboard_rx_buff[i]&0b01111111;
        if (raw_keycode^keyboard_rx_buff[i+1]==0b11111111) {
            bool key_up = keyboard_rx_buff[i]&0b10000000;
            if (key_up) keyRelease(pbuf, key_num, raw_keycode);
            else keyPress(pbuf, key_num, raw_keycode);
            i++;
        }
    }
    // The incoming keycode is always 2 bytes, so if this is the last byte, we have to wait until the next UART read to get the second byte (or something is wrong)
    if (i+1==keyboard_rx_buff_len) {
        keyboard_rx_buff[0]=keyboard_rx_buff[i];
        keyboard_rx_buff_len = 1;
    } else {
        keyboard_rx_buff_len = 0;
    }
#endif

#ifdef KEYBOARD_TYPE_PPK
    for (i = 0; i < keyboard_rx_buff_len; i++) {
        uint8_t raw_keycode = keyboard_rx_buff[i] & 0b01111111;
        if (raw_keycode == 0)
            raw_keycode = 27;
        bool key_up = keyboard_rx_buff[i] & 0b10000000;
        if (last_byte == keyboard_rx_buff[i]) {
            // keyboard duplicates the final key-up byte
            keyReset(pbuf, key_num);
        } else {
            if (key_up) {
                keyRelease(pbuf, key_num, raw_keycode);
            } else {
                keyPress(pbuf, key_num, raw_keycode);
            }
            last_byte = keyboard_rx_buff[i];
        }
    }
    keyboard_rx_buff_len = 0;
#endif
}

int readKeyVal(void) {
    static uint8_t current_key_map[120] = { 0 };
    static uint8_t last_key_map[120] = { 0 };
    static uint8_t key_num = 0;
    uint8_t curruent_key_8[8] = { 0 };
    uint8_t curruent_key_16[16] = { 0 };
    static uint8_t last_key_8[8] = { 0 };
    static uint8_t last_key_16[16] = { 0 };

    keyScan(current_key_map, &key_num);
    // TODO: what does return -1 mean?
    if (key_num == 0xff) {
        return -1;
    }

    bool is_key;
    static bool is_long_key_trigged = false;

    is_key = !tmos_isbufset(current_key_map, 0, sizeof(current_key_map));

    static uint32_t last_time = 0;
    uint32_t current_time = get_current_time();

    if (tmos_memcmp(current_key_map, last_key_map,
            sizeof(current_key_map)) == true) {
        if (!(is_key && (current_time - last_time > LONGKEY_TIME)
                && !is_long_key_trigged)) {
            return 0;
        }
        is_long_key_trigged = true;

        PRINT("long key triggered\n");
    } else {
        is_long_key_trigged = false;
    }

    last_time = current_time;
    tmos_memcpy(last_key_map, current_key_map, sizeof(current_key_map));

    int ret = key_parse(current_key_map, key_num, curruent_key_8,
            curruent_key_16);

    if (ret < 0)
        return -1;

    if (!special_key_handler(curruent_key_8, is_long_key_trigged)) {

        return true;
    }
    handle_first_layer_special_key(curruent_key_8, is_long_key_trigged);

    if (is_long_key_trigged) {
        return 0;
    }
    /*
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
     */
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

    return true;
}
