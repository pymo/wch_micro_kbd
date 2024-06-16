/*
 * ADC.c
 *
 *  Created on: Jun 15, 2024
 *      Author: chen_xin_ming
 */

#include <stdbool.h>
#include <stdint.h>

#include "soc.h"
#include "ADC.h"

uint8_t battery_percentage = 100;

uint8_t GetBatteryPercentage() {
    return battery_percentage;
}

// We don't bother converting the raw value to volts. We just measure the raw value across the
// entire run time, then we can calculate the raw reading at 0%, 5%, ..., 100%
// battery life. It is a rough estimation anyway.
//
// If you are using a different battery, you may need to change this table.
// This lookup table must have 21 entries. Each entry must be different,
// otherwise we will have divide-by-zero error below.

const uint32_t bat_percent_lookup[] = {
        2375, 3164, 3304, 3353, 3376, 3407,
        3427, 3450, 3473, 3497, 3524, 3551,
        3584, 3617, 3652, 3693, 3729, 3767,
        3817, 3861, 3947 };
#define BAT_SAMPLE_WINDOW_SIZE 5

uint8_t AdcToBatteryPercentage(uint32_t adc_value) {
    static uint32_t bat_latest_samples[BAT_SAMPLE_WINDOW_SIZE] = { 0 };
    static uint8_t latest_sample_index = BAT_SAMPLE_WINDOW_SIZE;
    // Put the latest adc_value into the bat_latest_samples
    if (latest_sample_index == BAT_SAMPLE_WINDOW_SIZE) {
        for (uint8_t i = 0; i < BAT_SAMPLE_WINDOW_SIZE; ++i) {
            bat_latest_samples[i] = adc_value;
        }
        latest_sample_index = 1;
    } else {
        bat_latest_samples[latest_sample_index] = adc_value;
        latest_sample_index += 1;
        if (latest_sample_index >= BAT_SAMPLE_WINDOW_SIZE)
            latest_sample_index = 0;
    }
    // Calculates the average value
    uint32_t adcsum = 0;
    for (uint8_t i = 0; i < BAT_SAMPLE_WINDOW_SIZE; ++i) {
        adcsum += bat_latest_samples[i];
    }
    uint32_t adcavg = adcsum / BAT_SAMPLE_WINDOW_SIZE;
    PRINT("window average adc: %d\n", adcavg);
    // Looks up the table to determine the percentage
    uint8_t percent = 100;
    int n;
    for (n = 0; n <= 20; n++) {
        if (adcavg < bat_percent_lookup[n]) {
            if (n == 0) {
                percent = 0;
                break;
            } else {
                // adc_value falls between bat_percent_lookup[n-1] and
                // bat_percent_lookup[n]. Do interpolation.
                percent = 5 * (n - 1)
                        + 5 * (adcavg - bat_percent_lookup[n - 1])
                                / (bat_percent_lookup[n]
                                        - bat_percent_lookup[n - 1]);
                break;
            }
        }
    }
    if (n > 20) {
        percent = 100;
    }
    return percent;
}

void ADCBatterySample() {
    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);
    ADC_ChannelCfg(5);
    uint32_t adcsum = 0;
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_Floating);
    signed short RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中
    for (uint8_t i = 0; i < 20; i++) {
        adcsum += ADC_ExcutSingleConver() + RoughCalib_Value;      // 连续采样20次
    }
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_PU);
    uint32_t adcavg = adcsum / 20;
    uint32_t voltage_mv = adcavg * 1000 * 2 * 1.05 / 2048;
    PRINT("adc sample: %d, offset: %d voltage: %d\n", adcavg, RoughCalib_Value,
            voltage_mv);
    battery_percentage = AdcToBatteryPercentage(adcavg);
}
