/*
 * Copyright (c) 2024 Xinming Chen
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUBSYS_BUTTON_TASK_BUTTON_TASK_H
#define SUBSYS_BUTTON_TASK_BUTTON_TASK_H

#include "stdbool.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* EVENTS */
#define BUTTON_UPDATE_EVENT          (1<<0)

/* BUTTON color */
#define BUTTON_PAIR GPIO_Pin_22
#define BUTTON_READ_INTERVAL_MS 150
#define BUTTON_LONG_PRESS_MS 3000

void button_task_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SUBSYS_BUTTON_TASK_BUTTON_TASK_H */
