/********************************** (C) COPYRIGHT *******************************
* File Name          : CH57x_SYS.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/15
* Description 
*******************************************************************************/

#include "soc.h"

typedef enum
{
  CLK_FLASH_4 = 0X01,
  CLK_FLASH_5 = 0X05,
  CLK_FLASH_6 = 0X02,
  CLK_FLASH_7 = 0X06,
  CLK_FLASH_8 = 0X03,
  CLK_FLASH_9 = 0X07,

  AHB_READY_SHORT = 0X00,
  AHB_READY_NORMAL = 0X40,
  AHB_READY_LONG = 0X80,
  AHB_READY_LONGER = 0XC0,

  AHB_SAMPLE_NORMAL = 0X00,
  AHB_SAMPLE_DELAY = 0X10,
  AHB_SAMPLE_BEFORE = 0X20,

  AHB_SCSWIDTH_3 = 0X00,
  AHB_SCSWIDTH_2 = 0X08,

}FLASH_CLKTypeDef;

extern uint32_t _highcode_lma;
extern uint32_t _highcode_vma_start;
extern uint32_t _highcode_vma_end;

extern uint32_t _data_lma;
extern uint32_t _data_vma;
extern uint32_t _edata;

extern uint32_t _sbss;
extern uint32_t _ebss;


__attribute__((section(".highcode_copy")))
static void __attribute__((noinline)) copy_section(uint32_t * p_load, uint32_t * p_vma, uint32_t * p_vma_end)
{
    while(p_vma <= p_vma_end)
    {
        *p_vma = *p_load;
        ++p_load;
        ++p_vma;
    }
}

__attribute__((section(".highcode_copy")))
static void __attribute__((noinline)) zero_section(uint32_t * start, uint32_t * end)
{
    uint32_t * p_zero = start;

    while(p_zero <= end)
    {
        *p_zero = 0;
        ++p_zero;
    }
}

__attribute__((section(".highcode_copy")))
void mySystemInit(void) {
  uint32_t i;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  SAFEOPERATE;
  R8_PLL_CONFIG &= ~(1<<5);   //
  R8_SAFE_ACCESS_SIG = 0;
  if ( !( R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON ) ){
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        SAFEOPERATE;
        R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON;    // PLL power on
        for(i=0;i<2000;i++){  __nop();__nop();  }
    }
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R16_CLK_SYS_CFG = ( 1 << 6 ) | ( CLK_SOURCE_PLL_60MHz & 0x1f );
    __nop();__nop();__nop();__nop();
    R8_SAFE_ACCESS_SIG = 0;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    SAFEOPERATE;
    R8_FLASH_CFG = CLK_FLASH_6;
    R8_SAFE_ACCESS_SIG = 0;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;__nop();__nop();
    R8_PLL_CONFIG |= 1<<7;
    R8_SAFE_ACCESS_SIG = 0;

    copy_section(&_highcode_lma, &_highcode_vma_start, &_highcode_vma_end);
    //PRINT("&_highcode_vma_start:%x\n",&_highcode_vma_start);
    //PRINT("&_highcode_vma_end:%x\n",&_highcode_vma_end);
    copy_section(&_data_lma, &_data_vma, &_edata);
    zero_section(&_sbss, &_ebss);
}


void SystemInit(void){
    mySystemInit();
}

static char *cause_str(uint32_t cause)
{
	switch (cause) {
	case 0:
		return "Instruction address misaligned";
	case 1:
		return "Instruction Access fault";
	case 2:
		return "Illegal instruction";
	case 3:
		return "Breakpoint";
	case 4:
		return "Load address misaligned";
	case 5:
		return "Load access fault";
	case 6:
		return "Store/AMO address misaligned";
	case 7:
		return "Store/AMO access fault";
	case 8:
		return "Environment call from U-mode";
	case 9:
		return "Environment call from S-mode";
	case 11:
		return "Environment call from M-mode";
	case 12:
		return "Instruction page fault";
	case 13:
		return "Load page fault";
	case 15:
		return "Store/AMO page fault";
	default:
		return "unknown";
	}
}

__HIGH_CODE
__INTERRUPT
void HardFault_Handler(void)
{

  FLASH_ROM_SW_RESET();
  sys_safe_access_enable();
  R16_INT32K_TUNE = 0xFFFF;
  sys_safe_access_enable();
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  sys_safe_access_disable();

  while(1);
}
