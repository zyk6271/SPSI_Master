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

rt_thread_t warn_t = RT_NULL;

extern rf_info info_433;
extern rf_info info_4068;

uint8_t warning_status,warning_status_past=3;

void warn_callback(void *parameter)
{
    while(1)
    {
        if(info_433.alive == 1 && info_4068.alive == 1)
        {
            warning_status = 0;
        }
        else if(info_433.alive == 1 && info_4068.alive == 0)
        {
            warning_status = 1;
        }
        else if(info_433.alive == 0 && info_4068.alive == 1)
        {
            warning_status = 2;
        }
        if(info_433.alive == 0 && info_4068.alive == 0)
        {
            warning_status = 3;
        }
        if(warning_status_past != warning_status)
        {
            switch(warning_status)
            {
            case 0:
                beep_stop();
                psi_led_resume();
                break;
            case 1:
                if(warning_status_past!=3)
                {
                    led_rf4068_start(1);
                }
                beep_calc(info_4068.rssi_level,info_433.rssi_level,info_4068.alive,info_433.alive);
                psi_led_resume();
                break;
            case 2:
                if(warning_status_past!=3)
                {
                    led_rf433_start(1);
                }
                beep_calc(info_4068.rssi_level,info_433.rssi_level,info_4068.alive,info_433.alive);
                psi_led_resume();
                break;
            case 3:
                if(warning_status_past==0)
                {
                    led_rf433_start(1);
                    led_rf4068_start(1);
                }
                else if(warning_status_past==1)
                {
                    led_rf433_start(1);
                }
                else if(warning_status_past==2)
                {
                    led_rf4068_start(1);
                }
                beep_calc(info_4068.rssi_level,info_433.rssi_level,info_4068.alive,info_433.alive);
                psi_led_lost();
                break;
            }
            warning_status_past = warning_status;
        }
        rt_thread_mdelay(100);
    }
}
void warn_thread_init(void)
{
    warn_t = rt_thread_create("warn", warn_callback, RT_NULL, 1024, 12, 10);
    if(warn_t != RT_NULL)
    {
        rt_thread_startup(warn_t);
    }
}
