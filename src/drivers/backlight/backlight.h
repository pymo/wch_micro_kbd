#ifndef __BACKLIGHT__H
#define __BACKLIGHT__H

#include "stdbool.h"
#include "stdint.h"

#define BK_LINE0    1<<0
#define BK_LINE1    1<<1
#define BK_LINE2    1<<2
#define BK_LINE3    1<<3
#define BK_LINE4    1<<4
#define BK_LINE5    1<<5

#define LR0     GPIO_Pin_6
#define LR1     GPIO_Pin_7
#define LR2     GPIO_Pin_0
#define LR3     GPIO_Pin_1
#define LR4     GPIO_Pin_2
#define LR5     GPIO_Pin_3

#define LED_SCR     GPIO_Pin_12
#define LED_NUM     GPIO_Pin_13
#define LED_CAP     GPIO_Pin_12





#define BK_LINEALL  0x3f
#define BK_STEP     8

typedef enum{
    level_0,
    level_1=8,
    level_2=16,
    level_3=24,
    level_4=32,
    level_5=40,
    level_6=48,
    level_7=56,
    level_8=64
}bk_levels_t;

void set_bk(uint8_t line, bk_levels_t level);
bool enhance_bk(uint8_t line);
bool weaken_bk(uint8_t line);
void disbale_bk(uint8_t line);
void set_led_num(bool s);
void set_led_cap(bool s);
void set_led_scr(bool s);
void bkinit(void);

#endif /* __BACKLIGHT__H */
