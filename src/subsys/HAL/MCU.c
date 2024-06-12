/********************************** (C) COPYRIGHT *******************************
 * File Name          : MCU.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2019/11/05
 * Description        : 硬件任务处理函数及BLE和硬件初始化
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include <USB_task/USB_kbd.h>
#include "CH58x_common.h"
#include "HAL.h"
#include "RF_task/rf_dev.h"
#include "device_config.h"
#include "BLE/hidkbd.h"
#include "PM/pm_task.h"
#include "led_task/led_task.h"
#include "key_task/key_task.h"

tmosTaskID halTaskID;
uint8_t no_key_count = 0;
uint8_t battery_percentage = 100;

// The ADC of ESP32-C3 is not linear to the voltage... so we don't bother
// converting the raw value to volts. We just measure the raw value across the
// entire run time, then we can calculate the raw reading at 0%, 5%, ..., 100%
// battery life. It is a rough estimation anyway.
//
// If you are using a different ESP32 board, you may need to change this table.
// This lookup table must have 21 entries. Each entry must be different,
// otherwise we will have divide-by-zero error below.

const uint32_t bat_percent_lookup[] = {
        2000, 2340, 2390, 2420, 2450, 2475, 2490,
        2505, 2520, 2530, 2550, 2570, 2600, 2645,
        2675, 2705, 2740, 2770, 2805, 2845, 2905};
#define BAT_SAMPLE_WINDOW_SIZE 10
uint32_t bat_latest_samples[BAT_SAMPLE_WINDOW_SIZE] = {0};
uint8_t latest_sample_index = BAT_SAMPLE_WINDOW_SIZE;

uint8_t AdcToBatteryPercentage(uint32_t adc_value){
/*    // Put the latest adc_value into the bat_latest_samples
    if (latest_sample_index == BAT_SAMPLE_WINDOW_SIZE) {
        for (uint8_t i = 0; i < BAT_SAMPLE_WINDOW_SIZE; ++i) {
            bat_latest_samples[i] = adc_value;
        }
        latest_sample_index = 1;
    } else {
        bat_latest_samples[latest_sample_index] = adc_value;
        latest_sample_index += 1;
        if(latest_sample_index >= BAT_SAMPLE_WINDOW_SIZE) latest_sample_index=0;
    }
    // Calculates the average value
    uint32_t adcsum = 0;
    for(uint8_t i = 0; i < BAT_SAMPLE_WINDOW_SIZE; ++i )
    {
      adcsum +=bat_latest_samples[i];
    }
    uint32_t adcavg = adcsum/BAT_SAMPLE_WINDOW_SIZE;*/
    uint32_t adcavg = adc_value;
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
            percent = 5 * (n - 1) +
                          5 * (adcavg - bat_percent_lookup[n - 1]) /
                              (bat_percent_lookup[n] - bat_percent_lookup[n - 1]);
          break;
        }
      }
    }
    if (n > 20) {
        percent = 100;
    }
    return percent;
}

uint8_t GetBatteryPercentage(){
    return battery_percentage;
}

/*******************************************************************************
 * @fn          Lib_Calibration_LSI
 *
 * @brief       内部32k校准
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void Lib_Calibration_LSI( void )
{
    uint32_t irq_status;
    SYS_DisableAllIrq( &irq_status );
    Calibration_LSI( Level_64 );
    SYS_RecoverIrq( irq_status );
}

#if(defined(BLE_SNV)) && (BLE_SNV == TRUE)
/*******************************************************************************
 * @fn      Lib_Read_Flash
 *
 * @brief   Callback function used for BLE lib.
 *
 * @param   addr - Read start address
 * @param   num - Number of units to read (unit: 4 bytes)
 * @param   pBuf - Buffer to store read data
 *
 * @return  None.
 */
uint32_t Lib_Read_Flash(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
    EEPROM_READ(addr, pBuf, num * 4);
    return 0;
}

/*******************************************************************************
 * @fn      Lib_Write_Flash
 *
 * @brief   Callback function used for BLE lib.
 *
 * @param   addr - Write start address
 * @param   num - Number of units to write (unit: 4 bytes)
 * @param   pBuf - Buffer with data to be written
 *
 * @return  None.
 */
uint32_t Lib_Write_Flash(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
    EEPROM_ERASE(addr, num * 4);
    EEPROM_WRITE(addr, pBuf, num * 4);
    return 0;
}
#endif


void test( uint8_t code, uint32_t status ){
//    LOG_DEBUG("code= %#x, status=%#x\n", code, status);
}

/*******************************************************************************
 * @fn          CH57X_BLEInit
 *
 * @brief       BLE 库初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void CH57X_BLEInit( void )
{
  uint8 i;
  bleConfig_t cfg;
  if ( tmos_memcmp( VER_LIB, VER_FILE, strlen( VER_FILE )  ) == FALSE )
  {
    PRINT( "head file error...\n" );
    while( 1 )
      ;
  }

  SysTick_Config( SysTick_LOAD_RELOAD_Msk );
  PFIC_DisableIRQ( SysTick_IRQn );

  tmos_memset( &cfg, 0, sizeof(bleConfig_t) );
  cfg.MEMAddr = ( u32 ) MEM_BUF;
  cfg.MEMLen = ( u32 ) BLE_MEMHEAP_SIZE;
  cfg.BufMaxLen = ( u32 ) BLE_BUFF_MAX_LEN;
  cfg.BufNumber = ( u32 ) BLE_BUFF_NUM;
  cfg.TxNumEvent = ( u32 ) BLE_TX_NUM_EVENT;
  cfg.TxPower = ( u32 ) BLE_TX_POWER;
#if (defined (BLE_SNV)) && (BLE_SNV == TRUE)
  if(device_bond.ID_Num == 0) {
      cfg.SNVAddr = ( u32 ) BLE_SNV_ADDR0;
  } else if(device_bond.ID_Num == 1){
      cfg.SNVAddr = ( u32 ) BLE_SNV_ADDR1;
  } else if(device_bond.ID_Num == 2){
      cfg.SNVAddr = ( u32 ) BLE_SNV_ADDR2;
  } else if(device_bond.ID_Num == 3){
      cfg.SNVAddr = ( u32 ) BLE_SNV_ADDR3;
  } else {
      //should not be here!
  }

  cfg.readFlashCB = Lib_Read_Flash;
  cfg.writeFlashCB = Lib_Write_Flash;
  cfg.staCB = test;
#endif
#if( CLK_OSC32K )	
  cfg.SelRTCClock = ( u32 ) CLK_OSC32K;
#endif
  cfg.ConnectNumber = ( PERIPHERAL_MAX_CONNECTION & 3 ) | ( CENTRAL_MAX_CONNECTION << 2 );
  cfg.srandCB = SYS_GetSysTickCnt;
#if (defined TEM_SAMPLE)  && (TEM_SAMPLE == TRUE)
  cfg.tsCB = HAL_GetInterTempValue;    // 根据温度变化校准RF和内部RC( 大于7摄氏度 )
#if( CLK_OSC32K )
  cfg.rcCB = Lib_Calibration_LSI;    // 内部32K时钟校准
#endif
#endif
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
  cfg.sleepCB = CH58X_LowPower;    // 启用睡眠
#endif
#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
  /* four mac */
  if(device_bond.ID_Num == 0) {
      for ( i = 0; i < 6; i++ )
        cfg.MacAddr[i] = MacAddr[5 - i];
  } else if(device_bond.ID_Num == 1){
      MacAddr[5] += 1;  //change your mac here.
      for ( i = 0; i < 6; i++ )
        cfg.MacAddr[i] = MacAddr[5 - i];
  } else if(device_bond.ID_Num == 2){
      MacAddr[5] += 2; //change your mac here.
      for ( i = 0; i < 6; i++ )
        cfg.MacAddr[i] = MacAddr[5 - i];
  } else if(device_bond.ID_Num == 3){
      MacAddr[5] += 3; //change your mac here.
      for ( i = 0; i < 6; i++ )
        cfg.MacAddr[i] = MacAddr[5 - i];
  } else {
      //should not be here!
  }

#else
  {
    uint8_t ID_Num = device_bond.ID_Num;

    /* Note: Should not read "device_bond" here */
    tmos_memcpy(cfg.MacAddr, device_bond.ID[ID_Num].local_addr, 6);

  }

  for ( i = 0; i < 6; i++ ) {
    LOG_INFO("cfg.MacAddr[%d]=%#x", i,cfg.MacAddr[i]);
  }
#endif
  if ( !cfg.MEMAddr || cfg.MEMLen < 4 * 1024 )
    while( 1 )
      ;
  i = BLE_LibInit( &cfg );
  if ( i )
  {
    PRINT( "LIB init error code: %x ...\n", i );
    while( 1 )
      ;
  }
}

static void HAL_ProcessTMOSMsg( tmos_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {


    default:
        break;
  }
}

/*******************************************************************************
 * @fn          HAL_ProcessEvent
 *
 * @brief       硬件层事务处理
 *
 * input parameters
 *
 * @param       task_id.
 * @param       events.
 *
 * output parameters
 *
 * @param       events.
 *
 * @return      None.
 */
tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  uint8 * msgPtr;
  if ( events & SYS_EVENT_MSG )
  {    // 处理HAL层消息，调用tmos_msg_receive读取消息，处理完成后删除消息。
    msgPtr = tmos_msg_receive( task_id );
    if ( msgPtr )
    {
      HAL_ProcessTMOSMsg( (tmos_event_hdr_t *)msgPtr );
      /* De-allocate */
      tmos_msg_deallocate( msgPtr );
    }
    return events ^ SYS_EVENT_MSG;
  }

  if ( events & HAL_REG_INIT_EVENT )
  {
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)	// 校准任务，单次校准耗时小于10ms
    BLE_RegInit();    // 校准RF
#if( CLK_OSC32K )	
    Lib_Calibration_LSI();    // 校准内部RC
#endif
    tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );
    return events ^ HAL_REG_INIT_EVENT;
#endif
  }

  if(events & HAL_ADC_EVENT) {
    static bool is_adc_init = FALSE;
    if(!is_adc_init) {
      ADC_ExtSingleChSampInit( SampleFreq_3_2, ADC_PGA_0 );
      ADC_ChannelCfg( 5 );
      is_adc_init = TRUE;
    }

    uint32_t adcsum = 0;
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_Floating);
    signed short RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中
    for(uint8_t i = 0; i < 20; i++ )
    {
      adcsum += ADC_ExcutSingleConver() + RoughCalib_Value;      // 连续采样20次
    }
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_PU);
    uint32_t adcavg = adcsum/20;
    float voltage =  ((float)adcavg)/2048.0*1.05;
    PRINT("adc avg: %d, offset: %d voltage: %f\n", adcavg, RoughCalib_Value, voltage);
    battery_percentage = AdcToBatteryPercentage(adcavg);

//#define MIN_BATTERY_VOL         2.5
//#define ADC_THRESHOLD (MIN_BATTERY_VOL*2048/1.05)  //低于2.5V睡眠

//    if(adcavg < (uint32_t)ADC_THRESHOLD) {
//        PRINT("battery low...\n");
//#if( DEBUG == Debug_UART1 )  // 使用其他串口输出打印信息需要修改这行代码
//      while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 )
//      __nop();
//#endif
//    }
    tmos_start_task( halTaskID, HAL_ADC_EVENT, MS1_TO_SYSTEM_TIME(1000));   //60s 检测一次
    return events ^ HAL_ADC_EVENT;
  }


  if ( events & HAL_TEST_EVENT )
  {
#include "RingBuffer/lwrb.h"


    ARG_UNUSED(RF_OTAbuff_data);
    ARG_UNUSED(RF_OTAbuff);
    ARG_UNUSED(KEY_buff_data);

    uint8_t buf[8] = {0};
    static uint8_t code = 0;
    uint8_t report_id = 0;
    static bool flag = false;

    if (lwrb_get_free(&KEY_buff) >= 9) {
      if (code < 0x04) {
        code = 0x04;
      }
      if (code > 0x26) {
        code = 0x04;
      }

      if (flag) {
        if (code == 0x26) {
          buf[3] = 0x28;
        }

        buf[2] = code;

        code++;
        flag = false;
      } else {
        flag = true;
      
      }

      lwrb_write(&KEY_buff, &report_id, 1);
      lwrb_write(&KEY_buff, buf, 8);
      OnBoard_SendMsg(rf_dev_taskid, KEY_MESSAGE, 1, NULL);
    } else {
      PRINT("over flow\n");
    }



    // OnBoard_SendMsg(hidEmuTaskId, KEY_MESSAGE, 1, NULL);
    // OnBoard_SendMsg(USBTaskID, KEY_MESSAGE, 1, NULL);

    tmos_start_task( halTaskID, HAL_TEST_EVENT, MS1_TO_SYSTEM_TIME(10));
    return events ^ HAL_TEST_EVENT;
  }
  return 0;
}

/*******************************************************************************
 * @fn          HAL_Init
 *
 * @brief       硬件初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void HAL_Init()
{
  halTaskID = TMOS_ProcessEventRegister( HAL_ProcessEvent );
  HAL_TimeInit();

#if (defined HAL_ADC) && (HAL_ADC == TRUE)
  tmos_start_task(halTaskID, HAL_ADC_EVENT, MS1_TO_SYSTEM_TIME(10));
#endif

#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE)
  HAL_SleepInit();
  pm_task_init();
#endif

#if (defined HAL_LED) && (HAL_LED == TRUE)
  led_task_init();
#endif

#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  keyscan_task_init();
#endif

#if ( defined BLE_CALIBRATION_ENABLE ) && ( BLE_CALIBRATION_ENABLE == TRUE )
  tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );    // 添加校准任务，单次校准耗时小于10ms
#endif

  // tmos_start_task( halTaskID, HAL_TEST_EVENT, MS1_TO_SYSTEM_TIME(6000) );    // 添加一个测试任务
}

/*******************************************************************************
 * @fn          HAL_GetInterTempValue
 *
 * @brief       如果使用了ADC中断采样，需在此函数中暂时屏蔽中断.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
uint16 HAL_GetInterTempValue( void )
{
  uint8 sensor, channel, config, tkey_cfg;
  uint16 adc_data;
  
  tkey_cfg = R8_TKEY_CFG;
  sensor = R8_TEM_SENSOR;
  channel = R8_ADC_CHANNEL;
  config = R8_ADC_CFG;
  ADC_InterTSSampInit();
  R8_ADC_CONVERT |= RB_ADC_START;
  while( R8_ADC_CONVERT & RB_ADC_START );
  adc_data = R16_ADC_DATA;
  R8_TEM_SENSOR = sensor;
  R8_ADC_CHANNEL = channel;
  R8_ADC_CFG = config;
  R8_TKEY_CFG = tkey_cfg;
  return ( adc_data );
}


uint8 OnBoard_SendMsg(uint8_t registeredTaskID, uint8 event, uint8 state, void *data)
{
    SendMSG_t *msgPtr;

  if ( registeredTaskID != TASK_NO_TASK )
  {
    // Send the address to the task
    msgPtr = ( SendMSG_t * ) tmos_msg_allocate( sizeof(SendMSG_t));
    if ( msgPtr )
    {
      msgPtr->hdr.event = event;
      msgPtr->hdr.status = state;
      msgPtr->pData = data;
      tmos_msg_send( registeredTaskID, ( uint8 * ) msgPtr );

    }
    return ( SUCCESS );
  }
  else
  {
    return ( FAILURE );
  }
}


/******************************** endfile @ mcu ******************************/
