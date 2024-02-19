/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_KEY_SCAN_KEY_TABLE_H
#define DRIVERS_KEY_SCAN_KEY_TABLE_H

#include <stdint.h>

const uint8_t key8_table[] = //默认的键码表
{
    //        R0   R1   R2   R3   R4   R5
    0x00, 0x29, 0x35, 0x2b, 0x39, 0x02, 0x01, // c0                 //esc           ~`              tab         caps-lock  shift-L      ctr-l
    0x00, 0x3a, 0x1e, 0x14, 0x04, 0x1d, 0x08, // c1                 //F1            1!              Q           A          Z            win-l
    0x00, 0x3b, 0x1f, 0x1a, 0x16, 0x1b, 0x04, // c2                 //f2            2@              W           S          X            alt-l
    0x00, 0x3c, 0x20, 0x08, 0x07, 0x06, 0x2c, // c3                 //F3            3#              E           D          C            Space
    0x00, 0x3d, 0x21, 0x15, 0x09, 0x19, 0x40, // c4                 //F4            4$              R           F          V            alt-r
    0x00, 0x3e, 0x22, 0x17, 0x0a, 0x05, 0xfe, // c5                 //F5            5%              T           G          B            Fn

    0x00, 0x3f, 0x23, 0x1c, 0x0b, 0x11, 0x10, // c6                 //F6            6^              Y           H          N
    0x00, 0x40, 0x24, 0x18, 0x0d, 0x10, 0x80, // c7                 //F7            7&              U           J          M            win-r
    0x00, 0x41, 0x25, 0x0c, 0x0e, 0x36, 0x10, // c8                 //F8            8*              I           K          <,           ctr-r
    0x00, 0x42, 0x26, 0x12, 0x0f, 0x37, 0x00, // c9                 //F9            9               O           L          >.
    0x00, 0x43, 0x27, 0x13, 0x33, 0x38, 0x00, // c10                //F10           0               P           ;:         /?
    0x00, 0x44, 0x2d, 0x2f, 0x34, 0x20, 0x00, // c11                //F11           -_              [{          '"         shift-r
    0x00, 0x45, 0x2e, 0x30, 0x31, 0x00, 0x00, // c12                //F12           =+              ]}          \|

    0x00, 0x00, 0x2a, 0x00, 0x28, 0x00, 0x00, // c13                //0x00          Backspace                   Enter-R
    0x00, 0x46, 0x49, 0x4c, 0x00, 0x00, 0x50, // c14                //Print-screen  Insert          Delete      0x00,       0x00,       左
    0x00, 0x47, 0x4a, 0x4d, 0x00, 0x52, 0x51, // c15                //Scroll-Lock   Home            End         0x00,       上                           下
    0x00, 0x48, 0x4b, 0x4e, 0x00, 0x00, 0x4f, // c16                //Pause         Page-Up         Page-Down   0x00        0x00        右
    0x00, 0x00, 0x53, 0x5f, 0x5c, 0x59, 0x00, // c17                //Backlight     Num-lock        7HOME       4(小键盘)   1End       0x00
    0x00, 0x00, 0x54, 0x60, 0x5d, 0x5a, 0x62, // c18                //Locking       /               8(小键盘)   5(小键盘)   2(小键盘)   0Ins
    0x00, 0x00, 0x55, 0x61, 0x5e, 0x5b, 0x63, // c19                //0x00          *               9Pgup       6(小键盘)   3PgDn       =del
    0x00, 0x00, 0x56, 0x57, 0x00, 0x00, 0x58  // c20                 //0x00          -               +           0x00        0x00        Enter-R2
};

#endif /* DRIVERS_KEY_SCAN_KEY_TABLE_H */


