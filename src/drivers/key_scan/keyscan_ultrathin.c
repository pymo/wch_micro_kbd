/*
 * keyscan_ultrathin.c
 *
 *  Created on: Apr 26, 2025
 *      Author: chen_xin_ming
 */
#ifdef KEYBOARD_TYPE_ULTRATHIN

#include "keyscan_ultrathin.h"

#include "soc.h"

#define KEY_MATRIX_ROWS 11
#define KEY_MATRIX_ROW_LOG_2 4
#define KEY_MATRIX_COLUMNS 5


uint32_t high_voltage_threshold;

// GPIOB
static const uint32_t scan_matrix_row[] = {
        GPIO_Pin_4,
        GPIO_Pin_7,
        GPIO_Pin_14,
        GPIO_Pin_15,
};
// GPIOA
static const uint32_t scan_matrix_col[] = {
        GPIO_Pin_10,
        GPIO_Pin_4,
        GPIO_Pin_13,
        GPIO_Pin_14,
        GPIO_Pin_15,
};
static const uint8_t adc_channel[] = {
        255, // Col 0 does not use analog input.
        0,
        3,
        4,
        5
};

// How does the key8_table variable work:
// Whenever the MCU receives a scan code from the keyboard, it looks up key8_table[scan_code] to get the HID code.
// e.g. if you want the scancode 28 to map to `A`, the 29th element (because it starts with 0) in this array should be HID_KEY_A.

/*  3      4   9    10     14
1   Up     M   U    Down    K
2   Left   N  R Fn  R Space J
5   Return BS  P      /     =
6   .      '   O   Del      -
7   ,      ;   I   Right    L
8                  RShift
11  V      B   F     C   L Alt
12  Caps L Ctrl  D   X      T
13  H      G   L Fn L Space Y
15  Q      W   A   L Shift  E
16  Cmd   Tab  S     Z      R
*/
const uint8_t key8_table[] = //P10900U keycodes
{
HID_KEY_NONE,
HID_KEY_ARROW_UP,  HID_KEY_M,           HID_KEY_U,   HID_KEY_ARROW_DOWN,    HID_KEY_K,
HID_KEY_ARROW_LEFT,HID_KEY_N,           HID_KEY_FN2, HID_KEY_SPACE/*right*/,HID_KEY_J,
HID_KEY_RETURN,    HID_KEY_BACKSPACE,   HID_KEY_P,   HID_KEY_SLASH,         HID_KEY_EQUAL,
HID_KEY_PERIOD,    HID_KEY_APOSTROPHE,  HID_KEY_O,   HID_KEY_DELETE,        HID_KEY_MINUS,
HID_KEY_COMMA,     HID_KEY_SEMICOLON,   HID_KEY_I,   HID_KEY_ARROW_RIGHT,   HID_KEY_L,
HID_KEY_NONE,      HID_KEY_NONE,        HID_KEY_NONE,HID_KEY_SHIFT_RIGHT,   HID_KEY_NONE,
HID_KEY_V,         HID_KEY_B,           HID_KEY_F,   HID_KEY_C,             HID_KEY_ALT_LEFT,
HID_KEY_CAPS_LOCK, HID_KEY_CONTROL_LEFT,HID_KEY_D,   HID_KEY_X,             HID_KEY_T,
HID_KEY_H,         HID_KEY_G,           HID_KEY_FN,  HID_KEY_SPACE/*left*/, HID_KEY_Y,
HID_KEY_Q,         HID_KEY_W,           HID_KEY_A,   HID_KEY_SHIFT_LEFT,    HID_KEY_E,
HID_KEY_GUI_LEFT,  HID_KEY_TAB,         HID_KEY_S,   HID_KEY_Z,             HID_KEY_R
};

uint8_t fn_key_table[256] = {HID_KEY_NONE};

void init_fn_key_table(){
    fn_key_table[HID_KEY_TAB] = HID_KEY_ESCAPE;
    fn_key_table[HID_KEY_ARROW_UP] = HID_KEY_VOL_INC;
    fn_key_table[HID_KEY_ARROW_DOWN] = HID_KEY_VOL_DEC;
    fn_key_table[HID_KEY_ARROW_LEFT] = HID_KEY_BRIGHTNESS_DEC;
    fn_key_table[HID_KEY_ARROW_RIGHT] = HID_KEY_BRIGHTNESS_INC;
    fn_key_table[HID_KEY_DELETE] = HID_KEY_INSERT;
    fn_key_table[HID_KEY_Q] = HID_KEY_1;
    fn_key_table[HID_KEY_W] = HID_KEY_2;
    fn_key_table[HID_KEY_E] = HID_KEY_3;
    fn_key_table[HID_KEY_R] = HID_KEY_4;
    fn_key_table[HID_KEY_T] = HID_KEY_5;
    fn_key_table[HID_KEY_Y] = HID_KEY_6;
    fn_key_table[HID_KEY_U] = HID_KEY_7;
    fn_key_table[HID_KEY_I] = HID_KEY_8;
    fn_key_table[HID_KEY_O] = HID_KEY_9;
    fn_key_table[HID_KEY_P] = HID_KEY_0;
    fn_key_table[HID_KEY_SLASH] = HID_KEY_BACKSLASH;
    fn_key_table[HID_KEY_MINUS] = HID_KEY_BRACKET_LEFT;
    fn_key_table[HID_KEY_EQUAL] = HID_KEY_BRACKET_RIGHT;
    fn_key_table[HID_KEY_APOSTROPHE] = HID_KEY_GRAVE;
#ifdef ENABLE_NUMLOCK
    fn_key_table[HID_KEY_CAPS_LOCK] = HID_KEY_NUM_LOCK;
#endif
    fn_key_table[HID_KEY_J] = HID_KEY_WWW_HOME;
    fn_key_table[HID_KEY_K] = HID_KEY_APPLICATION;
    fn_key_table[HID_KEY_L] = HID_KEY_PRINT_SCREEN;
    fn_key_table[HID_KEY_COMMA] = HID_KEY_PREVIOUS;
    fn_key_table[HID_KEY_PERIOD] = HID_KEY_NEXT;
    fn_key_table[HID_KEY_SPACE] = HID_KEY_PLAY;
    fn_key_table[HID_KEY_B] = HID_KEY_MODE_USB;
    fn_key_table[HID_KEY_A] = HID_KEY_MODE_BLE_CHAN_1;
    fn_key_table[HID_KEY_S] = HID_KEY_MODE_BLE_CHAN_2;
    fn_key_table[HID_KEY_D] = HID_KEY_MODE_BLE_CHAN_3;
    fn_key_table[HID_KEY_F] = HID_KEY_MODE_BLE_CHAN_4;
}

uint8_t key8_table_size = sizeof(key8_table);

__HIGH_CODE
void ResetAllPinsStatus(){
    for (uint8_t i = 0; i<KEY_MATRIX_ROW_LOG_2;i++){
        GPIOB_SetBits(scan_matrix_row[i]);
    }
}

__HIGH_CODE
uint32_t ReadColVoltage(uint8_t col){
    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);
    uint32_t adcsum = 0;
    GPIOA_ModeCfg(scan_matrix_col[col], GPIO_ModeIN_Floating);
    signed short RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中
    ADC_ChannelCfg(adc_channel[col]);
    for (uint8_t i = 0; i < 20; i++) {
        adcsum += ADC_ExcutSingleConver() + RoughCalib_Value;      // 连续采样20次
    }
    GPIOA_ModeCfg(scan_matrix_col[col], GPIO_ModeIN_PU);
    uint32_t adcavg = adcsum / 20;
    uint32_t voltage_mv = adcavg * 1000 * 2 * 1.05 / 2048;
    return voltage_mv;
}

// Inject a Shift key, if there is already a Shift key, then don't inject.
void AppendListWithShift(struct KeyWithLayer* key_list, uint8_t* key_num){
    uint8_t shift_left = FindKeycode(key_list, *key_num, HID_KEY_SHIFT_LEFT);
    if(shift_left!=KEY_NOT_FOUND) return;
    uint8_t shift_right = FindKeycode(key_list, *key_num, HID_KEY_SHIFT_RIGHT);
    if(shift_right!=KEY_NOT_FOUND) return;
    key_list[*key_num].key=HID_KEY_SHIFT_LEFT;
    key_list[*key_num].layer=KEY_LAYER_FN2;
    key_list[*key_num].sent = KEY_SENT_NOTYET;
    ++(*key_num);
}

KeyLayerType GetKeyLayer(uint8_t *raw_keycodes, uint8_t raw_keycode_len){
    bool has_fn = false;
    bool has_fn2 = false;
    bool has_shift = false;
    for (uint8_t idx = 0; idx < raw_keycode_len; ++idx)
    {
        if (raw_keycodes[idx]>key8_table_size){
            PRINT("raw keycode %#x out of range!\n", raw_keycodes[idx]);
            continue;
        }
        uint8_t hid_keycode = key8_table[raw_keycodes[idx]];
        if (hid_keycode == HID_KEY_FN){
            has_fn = true;
        } else if (hid_keycode == HID_KEY_FN2){
            has_fn2 = true;
        } else if (hid_keycode == HID_KEY_SHIFT_LEFT || hid_keycode == HID_KEY_SHIFT_RIGHT){
            has_shift = true;
        }
    }
    if (has_fn && has_fn2) return KEY_LAYER_FN3;
    else if (has_fn2) return KEY_LAYER_FN2;
    else if (has_fn) return KEY_LAYER_FN;
    else if (has_shift) return KEY_LAYER_SHIFT;
    return KEY_LAYER_ORIGINAL;
}

void PreprocessKeyList(struct KeyWithLayer* key_list, uint8_t* key_num){
    bool needs_inject_shift = false;
    bool needs_remove_shift = false;
    for (int i = 0; i < *key_num; i++){
        uint8_t key;
        key = key_list[i].key;
        KeyLayerType key_layer;
        key_layer = key_list[i].layer;
        if((key == HID_KEY_Q && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_W && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_E && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_R && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_T && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_Y && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_U && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_I && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_O && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_P && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_SLASH && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_MINUS && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_EQUAL && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_APOSTROPHE && key_layer == KEY_LAYER_FN2) ||
           (key == HID_KEY_ARROW_UP && key_layer == KEY_LAYER_SHIFT)){
            needs_inject_shift = true;
        } else if(key == HID_KEY_SLASH && key_layer == KEY_LAYER_SHIFT){
            needs_remove_shift = true;
        }
    }
    if (needs_inject_shift)
        AppendListWithShift(key_list, key_num);
    if (needs_remove_shift){
        uint8_t shift_left = FindKeycode(key_list, *key_num, HID_KEY_SHIFT_LEFT);
        if(shift_left!=KEY_NOT_FOUND) key_list[shift_left].key=0;
        uint8_t shift_right = FindKeycode(key_list, *key_num, HID_KEY_SHIFT_RIGHT);
        if(shift_right!=KEY_NOT_FOUND) key_list[shift_right].key=0;
    }
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
        case KEY_LAYER_FN2:
            switch (key) {
                case HID_KEY_ARROW_UP:
                    key = HID_KEY_PAGE_UP;
                    break;
                case HID_KEY_ARROW_DOWN:
                    key = HID_KEY_PAGE_DOWN;
                    break;
                case HID_KEY_ARROW_LEFT:
                    key = HID_KEY_HOME;
                    break;
                case HID_KEY_ARROW_RIGHT:
                    key = HID_KEY_END;
                    break;
                case HID_KEY_Q:
                case HID_KEY_W:
                case HID_KEY_E:
                case HID_KEY_R:
                case HID_KEY_T:
                case HID_KEY_Y:
                case HID_KEY_U:
                case HID_KEY_I:
                case HID_KEY_O:
                case HID_KEY_P:
                case HID_KEY_SLASH:
                case HID_KEY_MINUS:
                case HID_KEY_EQUAL:
                case HID_KEY_APOSTROPHE:
                    key = fn_key_table[key];
                    break;
                default:
                    break;
            }
            break;

        case KEY_LAYER_FN3:
            switch (key) {
                case HID_KEY_Q:
                    key = HID_KEY_F1;
                    break;
                case HID_KEY_W:
                    key = HID_KEY_F2;
                    break;
                case HID_KEY_E:
                    key = HID_KEY_F3;
                    break;
                case HID_KEY_R:
                    key = HID_KEY_F4;
                    break;
                case HID_KEY_T:
                    key = HID_KEY_F5;
                    break;
                case HID_KEY_Y:
                    key = HID_KEY_F6;
                    break;
                case HID_KEY_U:
                    key = HID_KEY_F7;
                    break;
                case HID_KEY_I:
                    key = HID_KEY_F8;
                    break;
                case HID_KEY_O:
                    key = HID_KEY_F9;
                    break;
                case HID_KEY_P:
                    key = HID_KEY_F10;
                    break;
                case HID_KEY_SLASH:
                    key = HID_KEY_F11;
                    break;
                case HID_KEY_MINUS:
                    key = HID_KEY_F12;
                    break;
                default:
                    break;
            }
            break;
        case KEY_LAYER_SHIFT:
            if (key==HID_KEY_ARROW_UP) key = HID_KEY_SLASH;
            break;
        default:
            PRINT("Not implemented key layer: %u \n", key_list[key_index].layer);
            break;
    }
    PRINT("ConvertLayeredKey key=%u layer=%u sent=%u -> HID keycode %u\n", key_list[key_index].key, key_list[key_index].layer, key_list[key_index].sent, key);
    return key;
}
void InitKeyboard(KeyboardBootState* k_state, uint32_t ms_current, uint32_t ms_last_state_change)
{
    switch (*k_state) {
        case K_STATE_0_START:
            init_fn_key_table();
            for (uint8_t i = 0; i<KEY_MATRIX_COLUMNS;i++){
                GPIOA_ModeCfg(scan_matrix_col[i], GPIO_ModeIN_PU);
            }
            for (uint8_t i = 0; i<KEY_MATRIX_ROW_LOG_2;i++){
                GPIOB_ModeCfg(scan_matrix_row[i], GPIO_ModeOut_PP_5mA);
            }
            GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_Floating);

            // On col 2,3,4 we are using analog voltage instead of digital I/O to read multiple keypress.
            // So we need to find the lower bound of the high voltage, and use its 92% value as the threshold.
            // Set the rows of the matrix to be all high to avoid pulling down any column.
            ResetAllPinsStatus();
            uint32_t min_high_voltage = 100000;
            uint32_t max_high_voltage = 0;
            for (uint8_t i = 1; i<KEY_MATRIX_COLUMNS;i++){
                uint32_t voltage = ReadColVoltage(i);
                if( voltage < min_high_voltage) min_high_voltage = voltage;
                if( voltage > max_high_voltage) max_high_voltage = voltage;
            }
            uint32_t deviation = max_high_voltage-min_high_voltage;
            high_voltage_threshold = min_high_voltage-(50>deviation?50:deviation);
            PRINT("min_high_voltage: %d, high_voltage_threshold %d\n", min_high_voltage, high_voltage_threshold);
            *k_state = K_STATE_6_ID_RECEIVED;
            break;
        default:
            // unexpected state, restore to start state
            *k_state = K_STATE_0_START;
            break;
        }
}

__HIGH_CODE
uint8_t ScanRawKeycodes(uint8_t *raw_keys){
    uint8_t KeyNum;
    static uint8_t secbuf[MAX_KEY_NUM];

    uint8_t firstbuf[MAX_KEY_NUM] = { 0 };  //每一次扫描 firstbuf复位为0

    KeyNum = 0;

    for (uint8_t i = 0; i < KEY_MATRIX_ROWS; i++)
    {
        // Converts i into bits on the ROWs, the PCB uses a 4-16 decoder and the order is reversed.
        uint8_t decoder_input = KEY_MATRIX_ROWS - i - 1; // Decoder input: 10, 9, 8, ... 0
        for (uint8_t j = 0; j < KEY_MATRIX_ROW_LOG_2; j++){
            // If the j'th bit of decoder_input is high, set the corresponding pin to high, otherwise low.
            if (decoder_input & (1<<j)){
                GPIOB_SetBits(scan_matrix_row[j]);
            } else {
                GPIOB_ResetBits(scan_matrix_row[j]);
            }
        }
        __nop();
        __nop(); //由于上拉输入拉低需要一定的时间，所以必须延时一段时间再读IO
        for (uint8_t j = 0; j<KEY_MATRIX_COLUMNS;j++){
            bool key_pressed = false;
          if (j>=1){
              uint32_t voltage_mv = ReadColVoltage(j);
              key_pressed = (voltage_mv<high_voltage_threshold);
              //if (key_pressed) PRINT("Col %d voltage: %d\n", j, voltage_mv);
          } else {
              key_pressed = !GPIOA_ReadPortPin(scan_matrix_col[j]);
          }
          if(key_pressed){
              firstbuf[KeyNum++] = (i*KEY_MATRIX_COLUMNS + j + 1);
              //PRINT("Pressed key row %d col %d, keycode=%d\n", i, j , (i*KEY_MATRIX_COLUMNS + j + 1));
          }
        }
    }
    ResetAllPinsStatus();
    //这一次与上一次键值相等 去抖动作用
    if (tmos_memcmp(firstbuf, secbuf, sizeof(firstbuf)) == true)
    {
        tmos_memcpy(raw_keys, secbuf, sizeof(firstbuf));
        return KeyNum;
    }

    tmos_memcpy(secbuf, firstbuf, sizeof(firstbuf));
    return KEYSCAN_NOT_READY;
}

#endif
