/*
 * keyscan_ultrathin.c
 *
 *  Created on: Jun 26, 2025
 *      Author: chen_xin_ming
 */

#ifdef KEYBOARD_TYPE_PPK

#include "keyscan_ppk.h"
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
// Special note: on PPK, '1''s raw scan_code is 0, but we want to use 0 to represent no key in the array,
// so we artificially remap '1' scancode to 27. You can see the code in keyscan.c, void ScanKeyPPK() function.


/********************************   PPK (Palm Portable Keyboard) keycodes   ********************************/

#ifdef KEYBOARD_LAYOUT_ISO
const uint8_t key8_table[] = //German keycodes
{
HID_KEY_NONE,        HID_KEY_2,            HID_KEY_3,           HID_KEY_Z,          HID_KEY_4,   HID_KEY_5,    HID_KEY_6,     HID_KEY_7,
HID_KEY_GUI_LEFT,    HID_KEY_Q,            HID_KEY_W,           HID_KEY_E,          HID_KEY_R,   HID_KEY_T,    HID_KEY_Y,     HID_KEY_GRAVE,
HID_KEY_X,           HID_KEY_A,            HID_KEY_S,           HID_KEY_D,          HID_KEY_F,   HID_KEY_G,    HID_KEY_H,     HID_KEY_SPACE,
HID_KEY_CAPS_LOCK,   HID_KEY_TAB,          HID_KEY_CONTROL_LEFT,HID_KEY_1,          HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_FN,          HID_KEY_ALT_LEFT,   HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_C,   HID_KEY_V,    HID_KEY_B,     HID_KEY_N,
HID_KEY_MINUS,       HID_KEY_EQUAL,        HID_KEY_BACKSPACE,   HID_KEY_HOME,       HID_KEY_8,   HID_KEY_9,    HID_KEY_0,     HID_KEY_EUROPE_2,
HID_KEY_BRACKET_LEFT,HID_KEY_BRACKET_RIGHT,HID_KEY_EUROPE_1,    HID_KEY_END,        HID_KEY_U,   HID_KEY_I,    HID_KEY_O,     HID_KEY_P,
HID_KEY_APOSTROPHE,  HID_KEY_RETURN,       HID_KEY_PAGE_UP,     HID_KEY_NONE,       HID_KEY_J,   HID_KEY_K,    HID_KEY_L,     HID_KEY_SEMICOLON,
HID_KEY_SLASH,       HID_KEY_ARROW_UP,     HID_KEY_PAGE_DOWN,   HID_KEY_NONE,       HID_KEY_M,   HID_KEY_COMMA,HID_KEY_PERIOD,HID_KEY_ALT_RIGHT,
HID_KEY_DELETE,      HID_KEY_ARROW_LEFT,   HID_KEY_ARROW_DOWN,  HID_KEY_ARROW_RIGHT,HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_SHIFT_LEFT,  HID_KEY_SHIFT_RIGHT
};
#elif PPK_TYPE_JORNADA
const uint8_t key8_table[] = //Jornada US keycodes
{
HID_KEY_NONE,        HID_KEY_2,            HID_KEY_3,             HID_KEY_Z,          HID_KEY_4,   HID_KEY_5,    HID_KEY_6,     HID_KEY_7,
HID_KEY_FN,          HID_KEY_Q,            HID_KEY_W,             HID_KEY_E,          HID_KEY_R,   HID_KEY_T,    HID_KEY_Y,     HID_KEY_GRAVE,
HID_KEY_X,           HID_KEY_A,            HID_KEY_S,             HID_KEY_D,          HID_KEY_F,   HID_KEY_G,    HID_KEY_H,     HID_KEY_SPACE,
HID_KEY_CAPS_LOCK,   HID_KEY_TAB,          HID_KEY_CONTROL_LEFT,  HID_KEY_1,          HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_GUI_LEFT,      HID_KEY_ALT_LEFT,   HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_NONE,          HID_KEY_NONE,       HID_KEY_C,   HID_KEY_V,    HID_KEY_B,     HID_KEY_N,
HID_KEY_MINUS,       HID_KEY_EQUAL,        HID_KEY_BACKSPACE,     HID_KEY_VOL_INC,    HID_KEY_8,   HID_KEY_9,    HID_KEY_0,     HID_KEY_SPACE,
HID_KEY_BRACKET_LEFT,HID_KEY_BRACKET_RIGHT,HID_KEY_BACKSLASH,     HID_KEY_VOL_DEC,    HID_KEY_U,   HID_KEY_I,    HID_KEY_O,     HID_KEY_P,
HID_KEY_APOSTROPHE,  HID_KEY_RETURN,       HID_KEY_BRIGHTNESS_DEC,HID_KEY_NONE,       HID_KEY_J,   HID_KEY_K,    HID_KEY_L,     HID_KEY_SEMICOLON,
HID_KEY_SLASH,       HID_KEY_ARROW_UP,     HID_KEY_BRIGHTNESS_INC,HID_KEY_NONE,       HID_KEY_M,   HID_KEY_COMMA,HID_KEY_PERIOD,HID_KEY_INSERT,
HID_KEY_DELETE,      HID_KEY_ARROW_LEFT,   HID_KEY_ARROW_DOWN,    HID_KEY_ARROW_RIGHT,HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_SHIFT_LEFT,  HID_KEY_SHIFT_RIGHT
};
#else
const uint8_t key8_table[] = //Other PPK US keycodes
{
HID_KEY_NONE,        HID_KEY_2,            HID_KEY_3,           HID_KEY_Z,          HID_KEY_4,   HID_KEY_5,    HID_KEY_6,     HID_KEY_7,
HID_KEY_GUI_LEFT,    HID_KEY_Q,            HID_KEY_W,           HID_KEY_E,          HID_KEY_R,   HID_KEY_T,    HID_KEY_Y,     HID_KEY_GRAVE,
HID_KEY_X,           HID_KEY_A,            HID_KEY_S,           HID_KEY_D,          HID_KEY_F,   HID_KEY_G,    HID_KEY_H,     HID_KEY_SPACE,
HID_KEY_CAPS_LOCK,   HID_KEY_TAB,          HID_KEY_CONTROL_LEFT,HID_KEY_1,          HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_FN,          HID_KEY_ALT_LEFT,   HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_NONE,        HID_KEY_NONE,         HID_KEY_NONE,        HID_KEY_NONE,       HID_KEY_C,   HID_KEY_V,    HID_KEY_B,     HID_KEY_N,
HID_KEY_MINUS,       HID_KEY_EQUAL,        HID_KEY_BACKSPACE,   HID_KEY_HOME,       HID_KEY_8,   HID_KEY_9,    HID_KEY_0,     HID_KEY_SPACE,
HID_KEY_BRACKET_LEFT,HID_KEY_BRACKET_RIGHT,HID_KEY_BACKSLASH,   HID_KEY_END,        HID_KEY_U,   HID_KEY_I,    HID_KEY_O,     HID_KEY_P,
HID_KEY_APOSTROPHE,  HID_KEY_RETURN,       HID_KEY_PAGE_UP,     HID_KEY_NONE,       HID_KEY_J,   HID_KEY_K,    HID_KEY_L,     HID_KEY_SEMICOLON,
HID_KEY_SLASH,       HID_KEY_ARROW_UP,     HID_KEY_PAGE_DOWN,   HID_KEY_NONE,       HID_KEY_M,   HID_KEY_COMMA,HID_KEY_PERIOD,HID_KEY_INSERT,
HID_KEY_DELETE,      HID_KEY_ARROW_LEFT,   HID_KEY_ARROW_DOWN,  HID_KEY_ARROW_RIGHT,HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,  HID_KEY_NONE,
HID_KEY_SHIFT_LEFT,  HID_KEY_SHIFT_RIGHT
};
#endif

void init_fn_key_table(){
    fn_key_table[HID_KEY_TAB] = HID_KEY_ESCAPE;
#ifdef PPK_TYPE_JORNADA
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_PAGE_UP;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_PAGE_DOWN;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_HOME;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_END;
#else
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_VOL_INC;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_VOL_DEC;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_BRIGHTNESS_DEC;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_BRIGHTNESS_INC;
#endif
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
    fn_key_table[HID_KEY_GUI_LEFT] = HID_KEY_WWW_HOME;
    fn_key_table[HID_KEY_SPACE] = HID_KEY_PLAY;
    fn_key_table[HID_KEY_COMMA] = HID_KEY_PREVIOUS;
    fn_key_table[HID_KEY_PERIOD] = HID_KEY_NEXT;
    fn_key_table[HID_KEY_U] = HID_KEY_MODE_USB;
    fn_key_table[HID_KEY_A] = HID_KEY_MODE_BLE_CHAN_1;
    fn_key_table[HID_KEY_S] = HID_KEY_MODE_BLE_CHAN_2;
    fn_key_table[HID_KEY_D] = HID_KEY_MODE_BLE_CHAN_3;
    fn_key_table[HID_KEY_F] = HID_KEY_MODE_BLE_CHAN_4;
#ifdef KEYBOARD_LAYOUT_ISO
    fn_key_table[HID_KEY_ALT_RIGHT] = HID_KEY_PRINT_SCREEN;  // For German layout
    fn_key_table[HID_KEY_DELETE] = HID_KEY_INSERT;  // For German layout
#else
    fn_key_table[HID_KEY_INSERT] = HID_KEY_PRINT_SCREEN;
#endif
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

void InitKeyboard(KeyboardBootState* k_state, uint32_t ms_current, uint32_t ms_last_state_change)
{
    static uint8_t keyboard_id_buff[20];
    static uint8_t keyboard_id_buff_len = 0;
    static bool is_first_boot = true;
    switch (*k_state) {
    case K_STATE_0_START:
        PRINT("beginning keyboard boot sequence...\n");
        init_fn_key_table();
        /* 配置串口0：先配置IO口模式，再配置串口 */
        GPIOB_SetBits(GPIO_Pin_7);
        GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);      // RXD-配置上拉输入
        GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_5mA); // TXD-配置推挽输出，TXD not used in this application.
        UART0_DefInit();
        UART0_BaudRateCfg(9600);
        GPIOB_ModeCfg(RTS_PIN | DCD_PIN, GPIO_ModeIN_Floating);
        TurnOffVcc(VCC_PIN);
        GPIOA_ModeCfg(VCC_PIN, GPIO_ModeOut_PP_20mA); // VCC_CTL配置推挽输出，注意先让IO口输出高电平
        *k_state = K_STATE_1_OFF;
        keyboard_id_buff_len = 0;
        break;
    case K_STATE_1_OFF:
        if (ms_current > ms_last_state_change + (is_first_boot)?KEYBOARD_REBOOT_DELAY_MS_LONG:KEYBOARD_REBOOT_DELAY_MS_SHORT) {
            TurnOnVcc(VCC_PIN);
#if defined(PPK_TYPE_HANDSPRING)
            // TODO(): This should be K_STATE_5_RTS_HIGH, but the hardware circuit has some issues when directly connecting RXD0, should change back to K_STATE_5_RTS_HIGH once we have 0ohm disconnect in the circuit.
            *k_state = K_STATE_6_ID_RECEIVED;
            PRINT("Keyboard init complete.\n");
#else
            *k_state = K_STATE_2_ON;
#endif
        }
        break;
    case K_STATE_2_ON:
#ifdef PPK_TYPE_SONY
        // I don't know why but Sony Clie's DCD response is very hard to capture,
        // bypassing the DCD check until I get a oscilloscope to check...
        if(ms_current > ms_last_state_change + 700)
#else
        // Wait for 10ms, see https://github.com/pymo/ppk_bluetooth/issues/4
        if((ms_current > ms_last_state_change + 10) && GPIOB_ReadPortPin(DCD_PIN))
#endif
        {
            PRINT("DCD_PIN response done.\n");
            if (GPIOB_ReadPortPin(RTS_PIN)) { // RTS high, needs to lower it first
                GPIOB_ResetBits(RTS_PIN);
                GPIOB_ModeCfg(RTS_PIN, GPIO_ModeOut_PP_5mA);
                *k_state = K_STATE_3_DCD_RESPONDED;
            } else {
                GPIOB_ModeCfg(RTS_PIN, GPIO_ModeOut_PP_5mA);
                *k_state = K_STATE_4_RTS_LOW;
            }
        }
        break;
    case K_STATE_3_DCD_RESPONDED:
        if (ms_current > ms_last_state_change + 200) {
            *k_state = K_STATE_4_RTS_LOW;
        }
        break;
    case K_STATE_4_RTS_LOW:
        if (ms_current > ms_last_state_change + 10) {
            GPIOB_SetBits(RTS_PIN);
            PRINT("waiting for keyboard serial ID... \n");
            *k_state = K_STATE_5_RTS_HIGH;
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
            PRINT("byte from keyboard: %#x\n",
                    keyboard_id_buff[keyboard_id_buff_len + i]);
        }

        keyboard_id_buff_len += len;
            // Checks keyboard ID
            if (ContainsKeyboardId(keyboard_id_buff, keyboard_id_buff_len)){
                PRINT("Keyboard init complete.\n");
                is_first_boot = false;
                last_pressed = ms_current;
                *k_state = K_STATE_6_ID_RECEIVED;
                keyboard_id_buff_len = 0;
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

        keyboard_rx_buff_len += len;

    for (i = 0; i < keyboard_rx_buff_len; i++) {
        PRINT("byte from keyboard: %#x \n", keyboard_rx_buff[i]);
        uint8_t raw_keycode = keyboard_rx_buff[i] & 0b01111111;
        // '1''s raw keycode is 0, but we want to use 0 to represent no key in the array,
        // so we artificially remap '1' to 27.
        // This is also why in key_table.h, index 27 is set to HID_KEY_1.
        if (raw_keycode == 0) raw_keycode = 27;
        bool key_up = keyboard_rx_buff[i] & 0b10000000;
        if (last_byte == keyboard_rx_buff[i]) {
            // keyboard duplicates the final key-up byte
            keyReset(raw_keys, &key_num);
            continue;
        } else {
            last_byte = keyboard_rx_buff[i];
#if PRINT_KEYCODE_MODE
            if (!key_up) {
                char str_buf[10];
                sprintf(str_buf, "%#x \0", raw_keycode);
                PrintStringToHost(current_key_map, &key_num, str_buf, strlen(str_buf));
            }
            continue;
#endif
            if (key_up) {
                keyRelease(raw_keys, &key_num, raw_keycode);
            } else {
                keyPress(raw_keys, &key_num, raw_keycode);
            }
        }
    }
    keyboard_rx_buff_len = 0;
    return key_num;
}

#endif
