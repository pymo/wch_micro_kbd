/*
 * key_table.c
 *
 *  Created on: Aug 9, 2024
 *      Author: chen_
 */
#include "key_util.h"
#include "USB/hid.h"
#include "soc.h"

uint32_t last_pressed = 0;
// Maps the specially-defined codes to the standard consumer HID keycode.
// The amount and order must be exactly the same as the special codes in key_codes.h
uint16_t consumer_key_table[] = {
        HID_USAGE_CONSUMER_EMAIL, // HID_KEY_EMAIL
        HID_USAGE_CONSUMER_CALCULATOR, // HID_KEY_CALCULATOR
        HID_USAGE_CONSUMER_VOLUME_DEC, // HID_KEY_VOL_DEC
        HID_USAGE_CONSUMER_VOLUME_INC, // HID_KEY_VOL_INC
        HID_USAGE_CONSUMER_BRIGHTNESS_DEC, // HID_KEY_BRIGHTNESS_DEC
        HID_USAGE_CONSUMER_BRIGHTNESS_INC, // HID_KEY_BRIGHTNESS_INC
        HID_USAGE_CONSUMER_PLAYPAUSE, // HID_KEY_PLAY
        HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK, // HID_KEY_PREVIOUS
        HID_USAGE_CONSUMER_SCAN_NEXT_TRACK, // HID_KEY_NEXT
        HID_USAGE_CONSUMER_WWW_HOME, // HID_KEY_WWW_HOME
        HID_USAGE_CONSUMER_WWW_BACK // HID_KEY_WWW_BACK
};

uint8_t CompactIntegers(uint8_t *buf, uint8_t len)
{
    uint8_t new_len = 0;

    for (int i = 0; i < len; i++) {
        if (buf[i] != 0) {
            buf[new_len] = buf[i];
            new_len++;
        }
    }
    for (int i = new_len; i < len; i++) {
        buf[i] = 0;
    }

    return new_len;
}

uint8_t CompactKeyLayer(struct KeyWithLayer *buf, uint8_t len)
{
    uint8_t new_len = 0;

    for (int i = 0; i < len; i++) {
        if (buf[i].key != 0) {
            buf[new_len] = buf[i];
            new_len++;
        }
    }
    for (int i = new_len; i < len; i++) {
        buf[i].key = 0;
    }

    return new_len;
}

uint8_t FindKeycode(struct KeyWithLayer* key_list, uint8_t key_num, uint8_t key_code){
    // If not found, return KEY_NOT_FOUND
    for(int key_index = 0; key_index < key_num; ++key_index) {
        if (key_list[key_index].key == key_code) {
            return key_index;
        }
    }
    return KEY_NOT_FOUND;
}

void keyPress(uint8_t *pbuf, uint8_t *key_num, uint8_t raw_keycode) {
    for (uint8_t i = 0; i < *key_num; ++i) {
        if (raw_keycode == pbuf[i])
            return;
    }
    pbuf[*key_num] = raw_keycode;
    (*key_num)++;
    last_pressed = TMOS_GetSystemClock() * SYSTEM_TIME_MICROSEN  / 1000;
}

void keyRelease(uint8_t *pbuf, uint8_t *key_num, uint8_t raw_keycode) {
    for (uint8_t i = 0; i < *key_num; ++i) {
        if (raw_keycode == pbuf[i]) {
            pbuf[i] = 0;
            // Remove the item at i and move the following items (if there are any) forward.
            *key_num = CompactIntegers(pbuf, *key_num);
            return;
        }
    }
}

// Reset the key buffers
void keyReset(uint8_t *pbuf, uint8_t *key_num) {
    for (uint8_t i = 0; i < *key_num; ++i) {
        pbuf[i] = 0;
    }
    *key_num = 0;
}



uint8_t AsciiToHidCode(uint8_t ascii){
    if (ascii>='A' && ascii<='Z'){
        return ascii-'A'+HID_KEY_A;
    }
    if (ascii>='a' && ascii<='z'){
        return ascii-'a'+HID_KEY_A;
    }
    if (ascii>='1' && ascii<='9'){
        return ascii-'1'+HID_KEY_1;
    }
    if (ascii=='0'){
        return HID_KEY_0;
    }
    if (ascii=='\n'){
        return HID_KEY_RETURN;
    }
    return HID_KEY_SPACE;
}
