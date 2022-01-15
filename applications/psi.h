/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-06     Rick       the first version
 */
#ifndef APPLICATIONS_PSI_H_
#define APPLICATIONS_PSI_H_

#include "stdint.h"

void psi_led_init(void);
void psi_led_resume(void);
void psi_led_lost(void);
void valve_open(void);
void valve_close(void);
void valve_control(uint8_t mode);
void psi_upload(uint8_t value);

#endif /* APPLICATIONS_PSI_H_ */
