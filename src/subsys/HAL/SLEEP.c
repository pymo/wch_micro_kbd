/********************************** (C) COPYRIGHT *******************************
 * File Name          : SLEEP.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2019/11/05
 * Description        : ˯�����ü����ʼ��
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CH58x_common.h"
#include "HAL.h"
#include "device_config.h"
#include "USB_task/USB_kbd.h"
#include "RF_task/rf_dev.h"
#include "PM/pm_task.h"
#include "key_task/key_task.h"

extern uint32_t last_gpio_b_interrupt_count;
extern uint32_t gpio_b_interrupt_count;

#if 1
__HIGH_CODE
void suspend_to_ram(uint32_t ticks)
{
    RstAllPins();

    uint8_t x32Kpw, x32Mpw;
    __attribute__((aligned(4))) uint8_t MacAddr[6] = {0};

    GetMACAddress(MacAddr);
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%�����
    if(R16_RTC_CNT_32K>0x3fff){     // ����500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE�����������͵������
    }

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R8_BAT_DET_CTRL = 0;                // �رյ�ѹ���
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R8_SAFE_ACCESS_SIG = 0;

    PFIC -> SCTLR |= (1<<2);                //deep sleep

    uint16_t pp_flags = RB_PWR_PLAN_EN
                        | RB_PWR_MUST_0010
                        | RB_PWR_CORE
                        | RB_PWR_RAM2K
                        | RB_PWR_RAM30K
                        | RB_PWR_EXTEND
#if DCDC_ENABLE
                        | RB_PWR_DCDC_PRE
                        | RB_PWR_DCDC_EN
#endif
                        ;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
    R8_PLL_CONFIG |= (1<<5);

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R8_SLP_POWER_CTRL &= ~RB_WAKE_DLY_MOD;
    R8_SLP_POWER_CTRL |= 0x02;
    R8_SAFE_ACCESS_SIG = 0;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R16_POWER_PLAN = pp_flags;

    PFIC_EnableIRQ(GPIO_B_IRQn);
    do{
            R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
            R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
            SAFEOPERATE;
            R16_POWER_PLAN = pp_flags;

            // Sleep starts, updates last_gpio_b_interrupt_count to the last known gpio_b_interrupt_count
            // If gpio_b_interrupt_count's value changed during the sleep, it means the GPIOB is triggered.
            last_gpio_b_interrupt_count=gpio_b_interrupt_count;

            __WFI();
            __nop();
            __nop();
            DelayUs(70);
            {
                __attribute__((aligned(4))) uint8_t mac[6] = {0};

                GetMACAddress(mac);

                if(*((uint32_t *)mac) == *((uint32_t *)MacAddr))
                    break;
            }
            //UART1_SendByte(0x31);

            do
            {
                ticks = R32_RTC_CNT_32K;
            } while(ticks != R32_RTC_CNT_32K);

            {
                R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
                R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
                SAFEOPERATE;
                R8_SLP_WAKE_CTRL &=  ~(RB_SLP_GPIO_WAKE);    // RTC����
                //UART1_SendByte(0x13);
                ticks += 100;                       //3.2Ms�����»����豸
                if( ticks > 0xA8C00000 )   ticks -= 0xA8C00000;
                RTC_SetTignTime( ticks );
            }
    }while(1);

    //PFIC_DisableIRQ(GPIO_A_IRQn);
    PFIC_DisableIRQ(GPIO_B_IRQn);
    R8_SLP_WAKE_CTRL |=  (RB_SLP_GPIO_WAKE);    // RTC����
    SetAllPins();
    if( RTCTigFlag )  // ע�����ʹ����RTC����Ļ��ѷ�ʽ����Ҫע���ʱ32M����δ�ȶ�
    {
/*        tmosTimer key_time = tmos_get_task_timer(key_task_id, KEY_SCAN_EVENT);

        if(key_time && key_time <= 2)
        {

          R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
          R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
          SAFEOPERATE;
          R8_PLL_CONFIG &= ~(1<<5);
          R8_SAFE_ACCESS_SIG = 0;

          //key_deal();

          return ;
        }
*/
        ticks += WAKE_UP_RTC_MAX_TIME;
        if( ticks > 0xA8C00000 )   ticks -= 0xA8C00000;
        RTC_SetTignTime( ticks );

        FLASH_ROM_SW_RESET();
        R8_FLASH_CTRL = 0x04;   //flash�ر�
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        __nop();__nop();
        R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_KEEP;
        R8_SAFE_ACCESS_SIG =0;
        PFIC -> SCTLR |= (1<<2);                //deep sleep


        sys_safe_access_enable();
        R8_PLL_CONFIG &= ~(1<<5);
        sys_safe_access_disable();

        __WFI();
        __nop();__nop();

    }
    /*extern uint8_t no_key_count;
    if(no_key_count < 50){
        OnBoard_SendMsg(key_task_id, KEY_PRESSED, 1, NULL);
    }*/

    sys_safe_access_enable();
    R8_PLL_CONFIG &= ~(1<<5);

    HSECFG_Current( HSE_RCur_100 );     // ��Ϊ�����(�͹��ĺ�����������HSEƫ�õ���)
}

#else
__HIGH_CODE
void suspend_to_ram(uint32_t ticks)
{
    RstAllPins();

    uint8_t x32Kpw, x32Mpw;
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%�����
    if(R16_RTC_CNT_32K>0x3fff){     // ����500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE�����������͵������
    }

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R8_BAT_DET_CTRL = 0;                // �رյ�ѹ���
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R8_SAFE_ACCESS_SIG = 0;

    PFIC -> SCTLR |= (1<<2);                //deep sleep

    uint16_t pp_flags = RB_PWR_PLAN_EN
                        | RB_PWR_MUST_0010
                        | RB_PWR_CORE
                        | RB_PWR_RAM2K
                        | RB_PWR_RAM30K
                        | RB_PWR_EXTEND
#if DCDC_ENABLE
                        | RB_PWR_DCDC_PRE
                        | RB_PWR_DCDC_EN
#endif
                        ;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
    R8_PLL_CONFIG |= (1<<5);

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R8_SLP_POWER_CTRL &= ~RB_WAKE_DLY_MOD;
    R8_SLP_POWER_CTRL |= 0x02;
    R8_SAFE_ACCESS_SIG = 0;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R16_POWER_PLAN = pp_flags;

    PFIC_EnableIRQ(GPIO_A_IRQn);
    __nop();
    __nop();

    __WFI();

    /* Wait for flash to stabilize */
    DelayUs(150);
    PFIC_DisableIRQ(GPIO_A_IRQn);
    SetAllPins();
    if( RTCTigFlag )   // ע�����ʹ����RTC����Ļ��ѷ�ʽ����Ҫע���ʱ32M����δ�ȶ�
    {

        tmosTimer key_time = 10;//tmos_get_task_timer(key_task_id, KEY_SCAN_EVENT);

        if(key_time && key_time <= 2) {

          R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
          R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
          SAFEOPERATE;
          R8_PLL_CONFIG &= ~(1<<5);
          R8_SAFE_ACCESS_SIG = 0;

          key_deal();
          
          return ;
        }

        ticks += WAKE_UP_RTC_MAX_TIME;
        if( ticks > 0xA8C00000 )   ticks -= 0xA8C00000;
        RTC_SetTignTime( ticks );

        FLASH_ROM_SW_RESET();
        R8_FLASH_CTRL = 0x04;   //flash�ر�
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        __nop();__nop();
        R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_KEEP;
        R8_SAFE_ACCESS_SIG =0;
        PFIC -> SCTLR |= (1<<2);                //deep sleep

        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        SAFEOPERATE;
        R8_PLL_CONFIG &= ~(1<<5);
        R8_SAFE_ACCESS_SIG = 0;

        __WFI();
        __nop();__nop();
        /* Wait for flash to stabilize */
        DelayUs(150);
    }

    HSECFG_Current( HSE_RCur_100 );     // ��Ϊ�����(�͹��ĺ�����������HSEƫ�õ���)
}
#endif
/*******************************************************************************
 * @fn          CH58X_LowPower
 *
 * @brief       ����˯��
 *
 * input parameters
 *
 * @param       time-���ѵ�ʱ��㣨RTC����ֵ��
 *
 * output parameters
 *
 * @param       
 *
 * @return      None.
 */

__attribute__((section(".highcode")))
u32 CH58X_LowPower( u32 time )
{
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  uint32_t time_sleep, time_curr;
  u32 irq_status;
  if(device_mode == MODE_RF24)
  {
      if(pm_is_in_idle() == 1)
      {
          if(rf_port_is_data_loaded())
          {
              return 2;
          }
      }
  }
  else if(device_mode == MODE_USB)
  {
    if(is_host_sleep())
    {
      /**
       * ����shutdown˯�ߣ����Ѻ�оƬ��λ��
       * ���ܲ�����λ�źţ����ܻ���USB HOST
       */
    }
    else
    {
       return 2;
    }
   }
#if !HAL_LE_WORKING
  if(!pm_is_in_idle()){
      return 2;
  }
#endif
  SYS_DisableAllIrq( &irq_status );
  time_curr = RTC_GetCycle32k();
  // ���˯��ʱ��
  if (time < time_curr) {
      time_sleep = time + (RTC_TIMER_MAX_VALUE - time_curr);
  } else {
      time_sleep = time - time_curr;
  }
  //PRINT("CH58X_LowPower(%d)\n",time_sleep);

  // ��˯��ʱ��С����С˯��ʱ���������˯��ʱ�䣬��˯��
  if ((time_sleep < SLEEP_RTC_MIN_TIME) || 
      (time_sleep > SLEEP_RTC_MAX_TIME)) {
      SYS_RecoverIrq(irq_status);
      return 2;
  }
  RTC_SetTignTime( time );
  SYS_RecoverIrq( irq_status );
#if( DEBUG == Debug_UART1 )  // ʹ���������������ӡ��Ϣ��Ҫ�޸����д���
  while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 )
  {
    __nop();
  }
#endif
// LOW POWER-sleepģʽ
  if ( !RTCTigFlag )
  {
    // LowPower_Sleep( RB_PWR_RAM2K | RB_PWR_RAM30K | RB_PWR_EXTEND );
    // if( RTCTigFlag )   // ע�����ʹ����RTC����Ļ��ѷ�ʽ����Ҫע���ʱ32M����δ�ȶ�
    // {
    //   time += WAKE_UP_RTC_MAX_TIME;
    //   if( time > 0xA8C00000 )   time -= 0xA8C00000;
    //   RTC_SetTignTime( time );
    //   LowPower_Idle();
    // }
    // HSECFG_Current( HSE_RCur_100 );     // ��Ϊ�����(�͹��ĺ�����������HSEƫ�õ���)

    PWR_PeriphWakeUpCfg( ENABLE, RB_SLP_GPIO_WAKE, Short_Delay );
    suspend_to_ram(time);
    PWR_PeriphWakeUpCfg( DISABLE, RB_SLP_GPIO_WAKE, Short_Delay );
    if(last_gpio_b_interrupt_count!=gpio_b_interrupt_count){
        PRINT("gpio_b_interrupt_count=%d\n",gpio_b_interrupt_count);
        OnBoard_SendMsg(key_task_id, KEY_PRESSED, 1, NULL);
        pm_start_working(PM_WORKING_TIMEOUT, PM_IDLE_TIMEOUT);
        last_gpio_b_interrupt_count=gpio_b_interrupt_count;
    }
  }
  else
  {
    return 3;
  }
#endif
  return 0;
}

/*******************************************************************************
 * @fn          HAL_SleepInit
 *
 * @brief       ����˯�߻��ѵķ�ʽ-RTC���ѣ�����ģʽ
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       
 *
 * @return      None.
 */
void HAL_SleepInit( void )
{
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
/*  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  SAFEOPERATE;
  R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE | RB_SLP_GPIO_WAKE;    // RTC����
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  SAFEOPERATE;
  R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;    // ����ģʽ
  R8_SAFE_ACCESS_SIG = 0;    */

    PWR_PeriphWakeUpCfg( ENABLE, RB_SLP_RTC_WAKE, Short_Delay );

    sys_safe_access_enable();
    R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;
    sys_safe_access_disable();
  GPIOB_ITModeCfg(GPIO_Pin_4, GPIO_ITMode_FallEdge );        // �½��ػ���
  PFIC_EnableIRQ( RTC_IRQn );

  if(device_mode == MODE_USB){
      PWR_PeriphWakeUpCfg( ENABLE, RB_SLP_USB_WAKE, 0 );
  }
#endif
}


