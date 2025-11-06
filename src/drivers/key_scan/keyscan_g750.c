/*
 * keyscan_ultrathin.c
 *
 *  Created on: Jun 26, 2025
 *      Author: chen_xin_ming
 */

#ifdef KEYBOARD_TYPE_G750

#include "keyscan_g750.h"
#include "soc.h"

#if PCB_REV >= 3
#define TurnOffVcc(pin) GPIOA_ResetBits(pin)
#define TurnOnVcc(pin)  GPIOA_SetBits(pin)
#else
#define TurnOffVcc(pin) GPIOA_SetBits(pin)
#define TurnOnVcc(pin)  GPIOA_ResetBits(pin)
#endif

uint8_t fn_key_table[256] = {HID_KEY_NONE};

// How does the key8_table variable work:
// Whenever the MCU receives a scan code from the keyboard, it looks up key8_table[scan_code] to get the HID code.
// e.g. if you want the scancode 28 to map to `A`, the 29th element (because it starts with 0) in this array should be HID_KEY_A.

/********************************   G750 keycodes   ********************************/

#ifdef KEYBOARD_LAYOUT_ISO
//G750 UK/ES scan codes
const uint8_t key8_table[] =
{
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_FN,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_GUI_LEFT,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_TAB,       HID_KEY_GRAVE,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_SPACE,      HID_KEY_SHIFT_LEFT,HID_KEY_NONE,         HID_KEY_CONTROL_LEFT,HID_KEY_Q,         HID_KEY_1,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_Z,         HID_KEY_S,            HID_KEY_A,           HID_KEY_W,         HID_KEY_2,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_C,          HID_KEY_X,         HID_KEY_D,            HID_KEY_E,           HID_KEY_4,         HID_KEY_3,         HID_KEY_NONE,
HID_KEY_ARROW_UP,  HID_KEY_NONE,       HID_KEY_V,         HID_KEY_F,            HID_KEY_T,           HID_KEY_R,         HID_KEY_5,         HID_KEY_ARROW_RIGHT,
HID_KEY_NONE,      HID_KEY_N,          HID_KEY_B,         HID_KEY_H,            HID_KEY_G,           HID_KEY_Y,         HID_KEY_6,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_M,         HID_KEY_J,            HID_KEY_U,           HID_KEY_7,         HID_KEY_8,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_COMMA,      HID_KEY_K,         HID_KEY_I,            HID_KEY_O,           HID_KEY_0,         HID_KEY_9,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_PERIOD,     HID_KEY_SLASH,     HID_KEY_L,            HID_KEY_SEMICOLON,   HID_KEY_P,         HID_KEY_MINUS,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_EUROPE_1,  HID_KEY_NONE,         HID_KEY_BRACKET_LEFT,HID_KEY_EQUAL,     HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_CAPS_LOCK, HID_KEY_SHIFT_RIGHT,HID_KEY_RETURN,    HID_KEY_BRACKET_RIGHT,HID_KEY_SPACE,       HID_KEY_BACKSLASH, HID_KEY_ARROW_LEFT,HID_KEY_NONE,
HID_KEY_ARROW_DOWN,HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_DELETE,    HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_ALT_RIGHT, HID_KEY_BACKSPACE,  HID_KEY_NONE,      HID_KEY_CONTROL_RIGHT,HID_KEY_EUROPE_2
};
#elif KEYBOARD_LAYOUT_G740
//G740 Palm variant scan codes
const uint8_t key8_table[] =
{
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_FN,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_GUI_LEFT,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_TAB,       HID_KEY_GRAVE,     HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_SPACE,      HID_KEY_SHIFT_LEFT,HID_KEY_NONE,         HID_KEY_CONTROL_LEFT,HID_KEY_Q,         HID_KEY_1,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_Z,         HID_KEY_S,            HID_KEY_A,           HID_KEY_W,         HID_KEY_2,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_C,          HID_KEY_X,         HID_KEY_D,            HID_KEY_E,           HID_KEY_4,         HID_KEY_3,         HID_KEY_NONE,
HID_KEY_SHIFT_RIGHT, HID_KEY_NONE,       HID_KEY_V,         HID_KEY_F,            HID_KEY_T,           HID_KEY_R,         HID_KEY_5,         HID_KEY_ARROW_DOWN,
HID_KEY_NONE,        HID_KEY_N,          HID_KEY_B,         HID_KEY_H,            HID_KEY_G,           HID_KEY_Y,         HID_KEY_6,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_M,         HID_KEY_J,            HID_KEY_U,           HID_KEY_7,         HID_KEY_8,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_COMMA,      HID_KEY_K,         HID_KEY_I,            HID_KEY_O,           HID_KEY_0,         HID_KEY_9,         HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_PERIOD,     HID_KEY_SLASH,     HID_KEY_L,            HID_KEY_SEMICOLON,   HID_KEY_P,         HID_KEY_MINUS,     HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_APOSTROPHE,HID_KEY_NONE,         HID_KEY_BRACKET_LEFT,HID_KEY_EQUAL,     HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_CAPS_LOCK,   HID_KEY_ARROW_UP,   HID_KEY_RETURN,    HID_KEY_BRACKET_RIGHT,HID_KEY_SPACE,       HID_KEY_BACKSLASH, HID_KEY_ARROW_LEFT,HID_KEY_NONE,
HID_KEY_ARROW_RIGHT, HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_DELETE,    HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_PRINT_SCREEN,HID_KEY_BACKSPACE,  HID_KEY_NONE,      HID_KEY_APPLICATION,  HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_ALT_RIGHT
};
#else
// (Most common) G750, G7L0 and G7L1 US scan codes (assume alt and Today keycaps are swapped)
const uint8_t key8_table[] =
{
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_CONTROL_LEFT,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_GUI_LEFT,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_TAB,       HID_KEY_GRAVE,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_SPACE,      HID_KEY_SHIFT_LEFT,HID_KEY_NONE,         HID_KEY_FN,HID_KEY_Q,         HID_KEY_1,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_Z,         HID_KEY_S,            HID_KEY_A,           HID_KEY_W,         HID_KEY_2,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_C,          HID_KEY_X,         HID_KEY_D,            HID_KEY_E,           HID_KEY_4,         HID_KEY_3,         HID_KEY_NONE,
HID_KEY_ARROW_UP,  HID_KEY_NONE,       HID_KEY_V,         HID_KEY_F,            HID_KEY_T,           HID_KEY_R,         HID_KEY_5,         HID_KEY_ARROW_RIGHT,
HID_KEY_NONE,      HID_KEY_N,          HID_KEY_B,         HID_KEY_H,            HID_KEY_G,           HID_KEY_Y,         HID_KEY_6,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_M,         HID_KEY_J,            HID_KEY_U,           HID_KEY_7,         HID_KEY_8,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_COMMA,      HID_KEY_K,         HID_KEY_I,            HID_KEY_O,           HID_KEY_0,         HID_KEY_9,         HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_PERIOD,     HID_KEY_SLASH,     HID_KEY_L,            HID_KEY_SEMICOLON,   HID_KEY_P,         HID_KEY_MINUS,     HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_APOSTROPHE,HID_KEY_NONE,         HID_KEY_BRACKET_LEFT,HID_KEY_EQUAL,     HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_CAPS_LOCK, HID_KEY_SHIFT_RIGHT,HID_KEY_RETURN,    HID_KEY_BRACKET_RIGHT,HID_KEY_SPACE,       HID_KEY_BACKSLASH, HID_KEY_ARROW_LEFT,HID_KEY_NONE,
HID_KEY_ARROW_DOWN,HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_DELETE,    HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_ALT_RIGHT, HID_KEY_BACKSPACE,  HID_KEY_NONE,      HID_KEY_CONTROL_RIGHT,HID_KEY_NONE,        HID_KEY_NONE,      HID_KEY_NONE,
HID_KEY_NONE,      HID_KEY_NONE,       HID_KEY_NONE,      HID_KEY_SPACE
};
#endif

void init_fn_key_table(){
    fn_key_table[HID_KEY_TAB] = HID_KEY_ESCAPE;
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_PAGE_UP;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_PAGE_DOWN;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_HOME;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_END;
    fn_key_table[HID_KEY_DELETE] = HID_KEY_INSERT;
    fn_key_table[HID_KEY_1] = HID_KEY_F1;
    fn_key_table[HID_KEY_2] = HID_KEY_F2;
    fn_key_table[HID_KEY_3] = HID_KEY_F3;
    fn_key_table[HID_KEY_4] = HID_KEY_F4;
    fn_key_table[HID_KEY_5] = HID_KEY_F5;
    fn_key_table[HID_KEY_6] = HID_KEY_F6;
    fn_key_table[HID_KEY_7] = HID_KEY_F7;
    fn_key_table[HID_KEY_8] = HID_KEY_F8;
    fn_key_table[HID_KEY_9] = HID_KEY_F9;
    fn_key_table[HID_KEY_0] = HID_KEY_F10;
    fn_key_table[HID_KEY_MINUS] = HID_KEY_F11;
    fn_key_table[HID_KEY_EQUAL] = HID_KEY_F12;
#ifdef ENABLE_NUMLOCK
    fn_key_table[HID_KEY_CAPS_LOCK] = HID_KEY_NUM_LOCK;
#endif
    fn_key_table[HID_KEY_BACKSPACE] = HID_KEY_POWER;
    fn_key_table[HID_KEY_Q] = HID_KEY_EMAIL;
    fn_key_table[HID_KEY_R] = HID_KEY_CALCULATOR;
    fn_key_table[HID_KEY_P] = HID_KEY_PRINT_SCREEN;
    fn_key_table[HID_KEY_BRACKET_LEFT] = HID_KEY_BRIGHTNESS_DEC;
    fn_key_table[HID_KEY_BRACKET_RIGHT] = HID_KEY_BRIGHTNESS_INC;
    fn_key_table[HID_KEY_COMMA] = HID_KEY_VOL_DEC;
    fn_key_table[HID_KEY_PERIOD] = HID_KEY_VOL_INC;
    fn_key_table[HID_KEY_GUI_LEFT] = HID_KEY_WWW_HOME;
    fn_key_table[HID_KEY_SPACE] = HID_KEY_PLAY;
    fn_key_table[HID_KEY_U] = HID_KEY_MODE_USB;
    fn_key_table[HID_KEY_A] = HID_KEY_MODE_BLE_CHAN_1;
    fn_key_table[HID_KEY_S] = HID_KEY_MODE_BLE_CHAN_2;
    fn_key_table[HID_KEY_D] = HID_KEY_MODE_BLE_CHAN_3;
    fn_key_table[HID_KEY_F] = HID_KEY_MODE_BLE_CHAN_4;
}

uint8_t key8_table_size = sizeof(key8_table);

KeyLayerType GetKeyLayer(uint8_t *raw_keycodes, uint8_t raw_keycode_len){
    for (uint8_t idx = 0; idx < raw_keycode_len; ++idx)
    {
        if (raw_keycodes[idx]>key8_table_size){
            PRINT("raw keycode %#x out of range!\n", raw_keycodes[idx]);
            continue;
        }
        uint8_t hid_keycode = key8_table[raw_keycodes[idx]];
        if (hid_keycode == HID_KEY_FN){
            return KEY_LAYER_FN;
        }
    }
    return KEY_LAYER_ORIGINAL;
}

void PreprocessKeyList(struct KeyWithLayer* key_list, uint8_t* key_num){
}

// If Fn+key is mapped to another key, replace it here.
// The result key may not be an simple HID keycode, it's OK, we will replace it in key_special.c
uint8_t ConvertLayeredKey(struct KeyWithLayer* key_list, uint8_t key_num, uint8_t key_index){
    uint8_t key;
    key = key_list[key_index].key;
    switch (key_list[key_index].layer) {
        case KEY_LAYER_ORIGINAL:
            break;
        case KEY_LAYER_FN:
            if (fn_key_table[key]!=0) key = fn_key_table[key];
            break;
        default:
            PRINT("Not implemented key layer: %u \n", key_list[key_index].layer);
            break;
    }
    //PRINT("ConvertLayeredKey key=%u layer=%u sent=%u -> HID keycode %u\n", key_list[key_index].key, key_list[key_index].layer, key_list[key_index].sent, key);
    return key;
}
void InitKeyboard(KeyboardBootState* k_state, uint32_t ms_current, uint32_t ms_last_state_change)
{
    switch (*k_state) {
        case K_STATE_0_START:
            PRINT("beginning keyboard boot sequence...\n");

            init_fn_key_table();
            /* 配置串口0：先配置IO口模式，再配置串口 */
            GPIOB_SetBits(GPIO_Pin_7);
            GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);      // RXD-配置上拉输入
            GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA); // TXD-配置推挽输出，TXD not used in this application.
            UART0_DefInit();
            UART0_BaudRateCfg(4800);
            UART0_ByteTrigCfg(UART_1BYTE_TRIG);
            TurnOffVcc(VCC_PIN);
            GPIOA_ModeCfg(VCC_PIN, GPIO_ModeOut_PP_20mA); // VCC_CTL配置推挽输出，注意先让IO口输出高电平
            *k_state = K_STATE_1_OFF;
            break;
        case K_STATE_1_OFF:
            if (ms_current > ms_last_state_change + KEYBOARD_REBOOT_DELAY_MS_SHORT) {
                ms_last_state_change = ms_current;
                TurnOnVcc(VCC_PIN);
                // TODO(): This should be K_STATE_5_RTS_HIGH, but the hardware circuit has some issues when directly connecting RXD0, should change back to K_STATE_5_RTS_HIGH once we have 0ohm disconnect in the circuit.
                *k_state = K_STATE_6_ID_RECEIVED;
                PRINT("Keyboard init complete.\n");
            }
            break;
        default:
            // unexpected state, restore to start state
            *k_state = K_STATE_0_START;
            break;
        }
}

__HIGH_CODE
uint8_t ScanRawKeycodes(uint8_t *raw_keys){
    static uint8_t keyboard_rx_buff[MAX_KEY_NUM];
    static uint8_t keyboard_rx_buff_len = 0;
    static uint8_t last_byte = 0;
    static uint8_t key_num = 0;
    // Append the newly read data to the end of the previous data (if there is any left).
    uint8_t len = UART0_RecvString(keyboard_rx_buff + keyboard_rx_buff_len);
    uint8_t i = 0;

    for (i = keyboard_rx_buff_len; i < keyboard_rx_buff_len+len; i++) {
        PRINT("byte from keyboard: %#x \n", keyboard_rx_buff[i]);
    }
    keyboard_rx_buff_len += len;

    for(i = 0;i+1 < keyboard_rx_buff_len;i++)
    {
        uint8_t raw_keycode = keyboard_rx_buff[i]&0b01111111;
        if (raw_keycode^keyboard_rx_buff[i+1]==0b11111111) {
            bool key_up = keyboard_rx_buff[i]&0b10000000;
#if PRINT_KEYCODE_MODE
            if (!key_up) {
                char str_buf[10];
                sprintf(str_buf, "%#x \0", raw_keycode);
                PrintStringToHost(current_key_map, &key_num, str_buf, strlen(str_buf));
            }
#else
            if (key_up) keyRelease(raw_keys, &key_num, raw_keycode);
            else keyPress(raw_keys, &key_num, raw_keycode);
#endif
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
    return key_num;
}

#endif
