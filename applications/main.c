/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-16     RT-Thread    first version
 */

#include <rtthread.h>
#include "little.h"
#include "file.h"
#include "Radio_Encoder.h"
#include "seg.h"
#include "psi.h"
#include "led.h"
#include "flashwork.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    flash_Init();
    ID_Init();
    rf_433_start();
    rf_4068_start();
    led_Init();
    warn_thread_init();
    heart_init();
    button_init();
    psi_init();
    RadioQueueInit();
    seg_init();
    while (1)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
