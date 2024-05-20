#include "backlight.h"
#include "CH58x_common.h"
#include "device_config.h"

static int led_level;

#if I2C_LED
#include "I2C/myi2c.h"
void set_led_num(bool s) {
    myi2c_beginTransmission(/*device address*/);     // Initialize the Tx buffer
    myi2c_write(/*data*/);// Put slave register address in Tx buffer

    myi2c_endTransmission(true);
}

void set_led_cap(bool s) {
    myi2c_beginTransmission(/*device address*/);     // Initialize the Tx buffer
    myi2c_write(/*data*/);// Put slave register address in Tx buffer

    myi2c_endTransmission(true);
}

void set_led_scr(bool s) {
    myi2c_beginTransmission(/*device address*/);     // Initialize the Tx buffer
    myi2c_write(/*data*/);// Put slave register address in Tx buffer

    myi2c_endTransmission(true);
}

void set_bk(uint8_t line, bk_levels_t level) {
    myi2c_beginTransmission(/*device address*/);     // Initialize the Tx buffer
    myi2c_write(/*data*/);// Put slave register address in Tx buffer

    myi2c_endTransmission(true);
}

void disbale_bk(uint8_t line) {
    myi2c_beginTransmission(/*device address*/);     // Initialize the Tx buffer
    myi2c_write(/*data*/);// Put slave register address in Tx buffer

    myi2c_endTransmission(true);
}

void bkinit(void) {
    myi2c_Init();

    myi2c_beginTransmission(/*device address*/);     // Initialize the Tx buffer
    myi2c_write(/*data*/);// Put slave register address in Tx buffer

    myi2c_endTransmission(true);
}

#else

void set_led_num(bool s) {
    (s) ? GPIOB_ResetBits(LED_NUM) : GPIOB_SetBits(LED_NUM);
}

void set_led_cap(bool s) {
    (s) ? GPIOB_ResetBits(LED_CAP) : GPIOB_SetBits(LED_CAP);
}

void set_led_scr(bool s) {
//    (s) ? GPIOB_ResetBits(LED_SCR) : GPIOB_SetBits(LED_SCR);
}

/*
void set_bk(uint8_t line, bk_levels_t level) {
    PWMX_ACTOUT(line & BK_LINEALL, level, Low_Level, ENABLE);
}

void disbale_bk(uint8_t line) {
    PWMX_ACTOUT(line & BK_LINEALL, 0, Low_Level, DISABLE);
}
*/
void bkinit(void) {
    //GPIOPinRemap(ENABLE, RB_PIN_PWMX);

    //¼üÅÌ×´Ì¬µÆÅäÖÃ
    GPIOB_SetBits(LED_CAP);
    GPIOB_SetBits(LED_NUM);
    GPIOB_ModeCfg(LED_CAP, GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg(LED_NUM, GPIO_ModeOut_PP_5mA);


    // user config
    // if(device_mode != MODE_USB)
    //     disbale_bk(BK_LINEALL);
}
#endif
/*
bool enhance_bk(uint8_t line) {
    if(device_mode != MODE_USB)
        return false;

    led_level += BK_STEP;
    if (led_level > level_8) {
        led_level = level_8;
        return false;
    }
    set_bk(line, led_level);
    device_led.led_level = led_level;
    return true;
}

bool weaken_bk(uint8_t line) {
    if(device_mode != MODE_USB)
        return false;
    led_level -= BK_STEP;
    if (led_level < level_0) {
        led_level = level_0;
        return false;
    }
    set_bk(line, led_level);
    device_led.led_level = led_level;
    return true;
}
*/
