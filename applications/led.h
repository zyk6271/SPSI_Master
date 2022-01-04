/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-23     Rick       the first version
 */
#ifndef APPLICATIONS_LED_H_
#define APPLICATIONS_LED_H_

#include "stdint.h"

void led_rf433_start(uint8_t mode);
void led_rf433_stop(void);
void led_rf4068_start(uint8_t mode);
void led_rf4068_stop(void);
void beep_start(uint8_t select);
void beep_stop(void);
void beep_calc(uint8_t level_4068,uint8_t level_433);

#endif /* APPLICATIONS_LED_H_ */
