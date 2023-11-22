/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-20     Rick       the first version
 */
#ifndef FILESYSTEM_LITTLE_H_
#define FILESYSTEM_LITTLE_H_
#include "stdint.h"

#define SDCARD_IN         (1 << 0)
#define SDCARD_OUT        (1 << 1)

int flash_Init(void);

#endif /* FILESYSTEM_LITTLE_H_ */
