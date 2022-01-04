/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-22     Rick       the first version
 */
#ifndef RADIO_RADIO_ENCODER_H_
#define RADIO_RADIO_ENCODER_H_

#include "stdint.h"

void RadioQueueInit(void);
void rf_433_Enqueue(uint32_t target_id,uint8_t control,uint8_t value);
void rf_4068_Enqueue(uint32_t target_id,uint8_t control,uint8_t value);

#endif /* RADIO_RADIO_ENCODER_H_ */
