/*
 * Copyright (c) 2022 zerosensei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUBSYS_KEY_TASK_KEY_TASK_H
#define SUBSYS_KEY_TASK_KEY_TASK_H

#include "soc.h"
#include "key_scan/keyscan.h"

/*task event*/
#define KEY_SCAN_EVENT              (1<<0)
#define KEY_SCAN_SHORT_TO_EVENT     (1<<1)
#define KEY_SCAN_LONG_TO_EVENT      (1<<2)

int key_deal(void);
void keyscan_task_init(void);

extern uint8_t key_task_id;

#endif /* SUBSYS_KEY_TASK_KEY_TASK_H */