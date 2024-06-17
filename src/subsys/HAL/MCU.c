/********************************** (C) COPYRIGHT *******************************
 * File Name          : MCU.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2019/11/05
 * Description        : Ӳ������������BLE��Ӳ����ʼ��
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include <USB_task/USB_kbd.h>
#include "CH58x_common.h"
#include "HAL.h"
#include "ADC.h"
#include "RF_task/rf_dev.h"
#include "device_config.h"
#include "BLE/hidkbd.h"
#include "PM/pm_task.h"
#include "led_task/led_task.h"
#include "key_task/key_task.h"
#include "button_task/button_task.h"

tmosTaskID halTaskID;

/*******************************************************************************
 * @fn          Lib_Calibration_LSI
 *
 * @brief       �ڲ�32kУ׼
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
 * @brief       BLE ���ʼ��
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
  cfg.tsCB = HAL_GetInterTempValue;    // �����¶ȱ仯У׼RF���ڲ�RC( ����7���϶� )
#if( CLK_OSC32K )
  cfg.rcCB = Lib_Calibration_LSI;    // �ڲ�32Kʱ��У׼
#endif
#endif
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
  cfg.sleepCB = CH58X_LowPower;    // ����˯��
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
 * @brief       Ӳ����������
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
  {    // ����HAL����Ϣ������tmos_msg_receive��ȡ��Ϣ��������ɺ�ɾ����Ϣ��
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
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)	// У׼���񣬵���У׼��ʱС��10ms
    BLE_RegInit();    // У׼RF
#if( CLK_OSC32K )	
    Lib_Calibration_LSI();    // У׼�ڲ�RC
#endif
    tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );
    return events ^ HAL_REG_INIT_EVENT;
#endif
  }

  if(events & HAL_ADC_EVENT) {
//#define MIN_BATTERY_VOL         2.5
//#define ADC_THRESHOLD (MIN_BATTERY_VOL*2048/1.05)  //����2.5V˯��

//    if(adcavg < (uint32_t)ADC_THRESHOLD) {
//        PRINT("battery low...\n");
//#if( DEBUG == Debug_UART1 )  // ʹ���������������ӡ��Ϣ��Ҫ�޸����д���
//      while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 )
//      __nop();
//#endif
//    }
    ADCBatterySample();
    tmos_start_task( halTaskID, HAL_ADC_EVENT, MS1_TO_SYSTEM_TIME(ADC_INTERVAL_MS));
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
 * @brief       Ӳ����ʼ��
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
  button_task_init();
  keyscan_task_init();
#endif

#if ( defined BLE_CALIBRATION_ENABLE ) && ( BLE_CALIBRATION_ENABLE == TRUE )
  tmos_start_task( halTaskID, HAL_REG_INIT_EVENT, MS1_TO_SYSTEM_TIME( BLE_CALIBRATION_PERIOD ) );    // ���У׼���񣬵���У׼��ʱС��10ms
#endif

  // tmos_start_task( halTaskID, HAL_TEST_EVENT, MS1_TO_SYSTEM_TIME(6000) );    // ���һ����������
}

/*******************************************************************************
 * @fn          HAL_GetInterTempValue
 *
 * @brief       ���ʹ����ADC�жϲ��������ڴ˺�������ʱ�����ж�.
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
