/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-12     Rick       the first version
 */
#ifndef APPLICATIONS_SEG_H_
#define APPLICATIONS_SEG_H_

#include "stdint.h"

void rf4068_rssi_count_increase(void);
void rf4068_rssi_count_resume(void);
void rf4068_alive_count_increase(void);
void rf4068_alive_count_resume(void);
void rf433_rssi_count_increase(void);
void rf433_rssi_count_resume(void);
void rf433_alive_count_increase(void);
void rf433_alive_count_resume(void);
void alive_count_increase(uint8_t select);
void seg_init(void);

#endif /* APPLICATIONS_SEG_H_ */
