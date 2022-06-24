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

struct rt_event rf_event;
rt_thread_t warn_t = RT_NULL;

extern rf_info info_433;
extern rf_info info_4068;

uint8_t warning_status,warning_status_past;

void warn_callback(void *parameter)
{
    uint32_t e;
    while(1)
    {
        if (rt_event_recv(&rf_event, (RF433_Offline | RF4068_Offline | RF433_Online | RF4068_Online),RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0, &e) == RT_EOK)
        {
            if(e & RF433_Offline)
            {
                led_rf433_start(1);
                if(info_4068.alive)
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
            else if(e & RF4068_Offline)
            {
                led_rf4068_start(1);
                if(info_433.alive)
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
            else if(e & RF433_Online)
            {
                psi_led_resume();
                if(info_4068.alive)
                {
                    beep_calc(info_4068.rssi_level,info_433.rssi_level);
                }
                else if(info_4068.alive==0 && info_4068.startup==1)
                {
                    beep_start(2);
                }
            }
            else if(e & RF4068_Online)
            {
                psi_led_resume();
                if(info_433.alive)
                {
                    beep_calc(info_4068.rssi_level,info_433.rssi_level);
                }
                else if(info_433.alive==0 && info_433.startup==1)
                {
                    beep_start(2);
                }
            }
        }
        rt_thread_mdelay(100);
    }
}
void warn_init(void)
{
    rt_event_init(&rf_event, "rf_event", RT_IPC_FLAG_PRIO);
    warn_t = rt_thread_create("warn", warn_callback, RT_NULL, 1024, 12, 10);
    if(warn_t != RT_NULL)
    {
        rt_thread_startup(warn_t);
    }
}
