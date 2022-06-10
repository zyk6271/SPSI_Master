/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-23     Rick       the first version
 */
#ifndef APPLICATIONS_FILE_H_
#define APPLICATIONS_FILE_H_

#include "stdint.h"

void ID_Init(void);
void File_Output(uint8_t select,uint8_t valve,uint8_t psi,uint8_t shake,uint8_t send_num,int rssi,uint8_t first,uint8_t button);

#endif /* APPLICATIONS_FILE_H_ */
