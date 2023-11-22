/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-06     Rick       the first version
 */
#include "rtthread.h"
#include "heart.h"
#include "psi.h"
#include "led.h"
#include "warn.h"

#define DBG_TAG "WARN"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern rf_status rf_pipe_status;
extern rf_status rf_lora_status;

void rf_offline(rf_status *handle)
{
    if(handle->rf_id == 0)
    {
        led_rf_pipe_signal_start(1);
    }
    else
    {
        led_rf_lora_signal_start(1);
    }

    if(rf_pipe_status.alive == 1 || rf_lora_status.alive == 1)
    {
        psi_led_resume();
        beep_start(2);
    }
    else
    {
        psi_led_lost();
        beep_start(3);
    }
}
void rf_online(rf_status *handle)
{
    psi_led_resume();
    if(rf_pipe_status.alive == 1 && rf_lora_status.alive == 1)
    {
        beep_calc(rf_pipe_status.rssi_level,rf_lora_status.rssi_level);
    }
    else if((rf_pipe_status.alive == 0 && rf_pipe_status.startup == 1) || (rf_lora_status.alive == 0 && rf_lora_status.startup == 1))
    {
        beep_start(2);
    }
}
