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

void keyscan_task_init(void);

extern uint8_t key_task_id;
extern uint32_t gpio_b_interrupt_count;
extern uint32_t last_gpio_b_interrupt_count;

#endif /* SUBSYS_KEY_TASK_KEY_TASK_H */
