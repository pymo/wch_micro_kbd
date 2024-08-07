/*
 * ADC.h
 *
 *  Created on: Jun 15, 2024
 *      Author: chen_xin_ming
 */

#ifndef SUBSYS_HAL_ADC_H_
#define SUBSYS_HAL_ADC_H_

#include <stdbool.h>

#define ADC_INTERVAL_MS 60000 //12000

void ADCBatterySample();
uint8_t GetBatteryPercentage();
bool GetChargingState();

#endif /* SUBSYS_HAL_ADC_H_ */
