/********************************** (C) COPYRIGHT *******************************
 * File Name          : CONFIG.h
 * Author             : WCH
 * Version            : V1.10
 * Date               : 2019/11/05
 * Description        : 配置说明及默认值，建议在工程配置里的预处理中修改当前值
 *******************************************************************************/

/******************************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

#define	ID_CH573							0x73

#define CHIP_ID								ID_CH573

#ifdef CH57xBLE_ROM
#include "CH58xBLE_ROM.H"
#else
#include "CH58xBLE_LIB.H"
#endif
/*********************************************************************
 【MAC】
 BLE_MAC											- 是否自定义蓝牙Mac地址 ( 默认:FALSE - 使用芯片Mac地址 )，需要在main.c修改Mac地址定义

 【DCDC】
 DCDC_ENABLE                  - 是否使能DCDC ( 默认:FALSE )

 【SLEEP】
 HAL_SLEEP   									- 是否开启睡眠功能 ( 默认:FALSE )
 WAKE_UP_RTC_MAX_TIME					- 等待32M晶振稳定时间，根据不同睡眠类型取值可分为： 睡眠模式/下电模式         -	45(默认)
                                                                                                                                                                                        暂停模式					-	45
                                                                                                                                                                                        空闲模式					-	5
 
 【TEMPERATION】
 TEM_SAMPLE										- 是否打开根据温度变化校准的功能，单次校准耗时小于10ms( 默认:TRUE )
 
 【CALIBRATION】
 BLE_CALIBRATION_ENABLE				- 是否打开定时校准的功能，单次校准耗时小于10ms( 默认:TRUE )
 BLE_CALIBRATION_PERIOD				- 定时校准的周期，单位ms( 默认:120000 ) 
 
 【SNV】
 BLE_SNV    				          - 是否开启SNV功能，用于储存绑定信息( 默认:TRUE )
 BLE_SNV_ADDR    					    - SNV信息保存地址，使用data flash最后( 默认:0x77E00 )
                              - 如果配置了SNVNum参数，则需要对应修改Lib_Write_Flash函数内擦除的flash大小，大小为SNVBlock*SNVNum

 【RTC】
 CLK_OSC32K										- RTC时钟选择，如包含主机角色必须使用外部32K( 默认:0 外部(32768Hz)，1：内部(32000Hz)，2：内部(32768Hz) )

 【MEMORY】
 BLE_MEMHEAP_SIZE  						- 蓝牙协议栈使用的RAM大小，不小于6K ( 默认:(1024*6) )

 【DATA】
 BLE_BUFF_MAX_LEN							- 单个连接最大包长度( 默认:27 (ATT_MTU=23)，取值范围[27~251] )
 BLE_BUFF_NUM									- 控制器缓存的包数量( 默认:5 )
 BLE_TX_NUM_EVENT							- 单个连接事件最多可以发多少个数据包( 默认:1 )
 BLE_TX_POWER									- 发射功率( 默认:LL_TX_POWEER_0_DBM (0dBm) )
 
 【MULTICONN】
 PERIPHERAL_MAX_CONNECTION	  - 最多可同时做多少从机角色( 默认:1 )
 CENTRAL_MAX_CONNECTION				- 最多可同时做多少主机角色( 默认:3 )
 **********************************************************************/

/*********************************************************************
 * 默认配置值
 */
#ifndef BLE_MAC
#define BLE_MAC											FALSE
#endif
#ifndef DCDC_ENABLE
#define DCDC_ENABLE                                     TRUE
#endif
#ifndef HAL_SLEEP
#define HAL_SLEEP										TRUE
#endif
#if HAL_SLEEP==TRUE
#define HAL_LE_WORKING                                  FALSE            //工作模式是否睡眠
#define HAL_LE_IDLE                                     HAL_SLEEP        //空闲模式是否睡眠
#define HAL_LE_STANDBY                                  TRUE            //待机模式是否睡眠
#endif
#ifndef SLEEP_RTC_MIN_TIME                   
#define SLEEP_RTC_MIN_TIME                              US_TO_RTC(1000)
#endif
#ifndef SLEEP_RTC_MAX_TIME                   
#define SLEEP_RTC_MAX_TIME                              MS_TO_RTC(RTC_TO_MS(RTC_TIMER_MAX_VALUE) - 1000 * 60 * 60)  // 23 hours
#endif
#ifndef WAKE_UP_RTC_MAX_TIME
#define WAKE_UP_RTC_MAX_TIME                            US_TO_RTC(1400)
#endif
#ifndef HAL_ADC
#define HAL_ADC                                         TRUE
#endif
#ifndef HAL_KEY
#define HAL_KEY											TRUE
#endif
#ifndef HAL_LED
#define HAL_LED											TRUE
#endif
#ifndef TEM_SAMPLE
#define TEM_SAMPLE									    TRUE
#endif
#ifndef BLE_CALIBRATION_ENABLE
#define BLE_CALIBRATION_ENABLE			                TRUE
#endif
#ifndef BLE_CALIBRATION_PERIOD
#define BLE_CALIBRATION_PERIOD			                120000
#endif
#ifndef BLE_SNV
#define BLE_SNV											TRUE
#endif
#ifndef BLE_SNV_ADDR0
#define BLE_SNV_ADDR0								    0x77E00-FLASH_ROM_MAX_SIZE-512*0
#endif

#ifndef BLE_SNV_ADDR1
#define BLE_SNV_ADDR1                                   0x77E00-FLASH_ROM_MAX_SIZE-512*1
#endif

#ifndef BLE_SNV_ADDR2
#define BLE_SNV_ADDR2                                   0x77E00-FLASH_ROM_MAX_SIZE-512*2
#endif

#ifndef BLE_SNV_ADDR3
#define BLE_SNV_ADDR3                                   0x77E00-FLASH_ROM_MAX_SIZE-512*3
#endif

#ifndef CLK_OSC32K
#define CLK_OSC32K									    0							// 该项请勿在此修改，必须在工程配置里的预处理中修改，如包含主机角色必须使用外部32K
#endif
#ifndef BLE_MEMHEAP_SIZE
#define BLE_MEMHEAP_SIZE						        (1024*6)
#endif
#ifndef BLE_BUFF_MAX_LEN
#define BLE_BUFF_MAX_LEN						        31
#endif
#ifndef BLE_BUFF_NUM
#define BLE_BUFF_NUM								    5
#endif
#ifndef BLE_TX_NUM_EVENT
#define BLE_TX_NUM_EVENT						        3
#endif
#ifndef BLE_TX_POWER
#define BLE_TX_POWER								    LL_TX_POWEER_0_DBM
#endif
#ifndef PERIPHERAL_MAX_CONNECTION
#define PERIPHERAL_MAX_CONNECTION		                1
#endif
#ifndef CENTRAL_MAX_CONNECTION
#define CENTRAL_MAX_CONNECTION			                3
#endif

//#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE) && (defined DEBUG)
//#error "When the sleep function is turned on, the print function needs to be turned off!"
//#endif

extern u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];
extern uint8_t MacAddr[6];

#endif

/******************************* endfile @ config ******************************/
