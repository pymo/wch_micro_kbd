/*
 * keyscan_ultrathin.c
 *
 *  Created on: Apr 26, 2025
 *      Author: chen_xin_ming
 */

#include "keyscan_ultrathin.h"
#include "key_table.h"

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

void InitScanPins(void)
{
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
}

__HIGH_CODE
uint8_t ScanKeyUltraThin(uint8_t *current_key_map){
    uint8_t KeyNum;
    static uint8_t secbuf[KEY_MAP_SIZE];

    uint8_t firstbuf[KEY_MAP_SIZE] = { 0 };  //每一次扫描 firstbuf复位为0

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
              if (key_pressed) PRINT("Col %d voltage: %d\n", j, voltage_mv);
          } else {
              key_pressed = !GPIOA_ReadPortPin(scan_matrix_col[j]);
          }
          if(key_pressed){
              firstbuf[KeyNum++] = (i*KEY_MATRIX_COLUMNS + j + 1);
              PRINT("Pressed key row %d col %d, keycode=%d\n", i, j , (i*KEY_MATRIX_COLUMNS + j + 1));
          }
        }
    }
    ResetAllPinsStatus();
    //这一次与上一次键值相等 去抖动作用
    if (tmos_memcmp(firstbuf, secbuf, sizeof(firstbuf)) == true)
    {
        tmos_memcpy(current_key_map, secbuf, sizeof(firstbuf));
        return KeyNum;
    }

    tmos_memcpy(secbuf, firstbuf, sizeof(firstbuf));
    return KEYSCAN_NOT_READY;
}
