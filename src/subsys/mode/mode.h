/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUBSYS_MODE_MODE_H
#define SUBSYS_MODE_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

/* EVENTS */
#define MODE_CTRL_SWITCH_EVENT          (1<<0)

void mode_ctrl_init(void);
void mode_select(uint8_t mod);

#ifdef __cplusplus
}
#endif

#endif /* SUBSYS_MODE_MODE_H */