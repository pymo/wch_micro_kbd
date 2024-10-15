/*
 * ADC.h
 *
 *  Created on: Jun 15, 2024
 *      Author: chen_xin_ming
 */

#ifndef SUBSYS_HAL_ADC_H_
#define SUBSYS_HAL_ADC_H_

#include <stdbool.h>

#define ADC_INTERVAL_MS     60000 //12000
#define BATTERY_PIN         GPIO_Pin_5  // GPIOA
#define BATTERY_PIN_CHANNEL 1  // PA5=AIN1
#define USB_PIN             GPIO_Pin_12  // GPIOA

void ADCBatterySample();
uint8_t GetBatteryPercentage();
bool GetChargingState();

#endif /* SUBSYS_HAL_ADC_H_ */
