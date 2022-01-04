/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-10     Rick       the first version
 */
#include <rtthread.h>
#include <stdio.h>
#include "little.h"
#include "file.h"
#include "heart.h"
#include "Radio_Encoder.h"
#include "led.h"

#define DBG_TAG "heart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t heart_t = RT_NULL;
rt_timer_t test_timer = RT_NULL;

rf_info info_433={433,0,1,0,0,0,0};
rf_info info_4068={4068,0,1,0,0,0,0};

uint8_t PSI_Status ;
uint8_t Valve_Status ;

uint8_t rf_now ;
uint8_t rf_connected;

extern uint32_t Target_ID ;

void heart_request(uint8_t rf_select)
{
    rf_now = rf_select;
    switch(rf_select)
    {
    case 0:
        info_4068.received = 0;
        rf_4068_Enqueue(Target_ID,0,PSI_Status);
        break;
    case 1:
        info_433.received = 0;
        rf_433_Enqueue(Target_ID,0,PSI_Status);
        break;
    }
}
void heart_single(void)
{
    switch(rf_now)
    {
    case 0:
        info_4068.received = 0;
        rf_4068_Enqueue(Target_ID,0,PSI_Status);
        break;
    case 1:
        info_433.received = 0;
        rf_433_Enqueue(Target_ID,0,PSI_Status);
        break;
    }
}
uint8_t warning_status,warning_status_past;
void heart_warning(void)
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
            beep_calc(info_4068.rssi_level,info_433.rssi_level);
            psi_led_resume();
            break;
        case 1:
            if(warning_status_past!=3)
            {
                led_rf4068_start(1);
            }
            beep_start(1);
            psi_led_resume();
            break;
        case 2:
            if(warning_status_past!=3)
            {
                led_rf433_start(1);
            }
            beep_start(1);
            psi_led_resume();
            break;
        case 3:
            if(!warning_status_past)
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
            beep_start(2);
            psi_led_lost();
            break;
        }
        warning_status_past = warning_status;
    }
}

void heart_callback(void *parameter)
{
    rf_info *info_temp = &info_433;
    while(1)
    {
        if(rf_connected==0)
        {
            heart_request(1);
            rt_thread_mdelay(1000);
            if(info_temp->received)
            {
                LOG_I("rf_connected success\r\n");
                File_Output(1,Valve_Status,PSI_Status,info_temp->received,info_temp->retry,info_temp->rssi,0,0);
                rf_connected = 1;
                psi_led_init();
                rt_thread_mdelay(5000);
            }
            else
            {
                LOG_W("rf_connected fail\r\n");
                rt_thread_mdelay(10000);
            }
        }
        else
        {
            if(rf_now)info_temp = &info_433;else info_temp = &info_4068;
            switch(info_temp->retry)
            {
            case 0:
                heart_request(rf_now);
                rt_thread_mdelay(1000);
                if(info_temp->received)
                {
                    File_Output(rf_now,Valve_Status,PSI_Status,info_temp->received,info_temp->retry,info_temp->rssi,1,0);
                    LOG_I("radio_%d first heart success\r\n",info_temp->freq);
                    info_temp->retry = 0;
                    info_temp->alive = 1;
                    rf_now = !rf_now;
                    heart_warning();
                    rt_thread_mdelay(3000);
                }
                else
                {
                    LOG_W("radio_%d first heart fail\r\n",info_temp->freq);
                    info_temp->retry++;
                    heart_warning();
                    rt_thread_mdelay(5000);
                }
                break;
            case 1:
                heart_request(rf_now);
                rt_thread_mdelay(1000);
                if(info_temp->received)
                {
                    File_Output(rf_now,Valve_Status,PSI_Status,info_temp->received,info_temp->retry,info_temp->rssi,1,0);
                    LOG_I("radio_%d second heart success\r\n",info_temp->freq);
                    info_temp->retry = 0;
                    info_temp->alive = 1;
                    rf_now = !rf_now;
                    heart_warning();
                    rt_thread_mdelay(3000);
                }
                else
                {
                    LOG_W("radio_%d second heart fail\r\n",info_temp->freq);
                    info_temp->retry++;
                    heart_warning();
                    rt_thread_mdelay(5000);
                }
                break;
            case 2:
                heart_request(rf_now);
                rt_thread_mdelay(1000);
                if(info_temp->received)
                {
                    File_Output(rf_now,Valve_Status,PSI_Status,info_temp->received,info_temp->retry,info_temp->rssi,1,0);
                    LOG_I("radio_%d third heart success\r\n",info_temp->freq);
                    info_temp->retry = 0;
                    info_temp->alive = 1;
                    rf_now = !rf_now;
                    heart_warning();
                    rt_thread_mdelay(3000);
                }
                else
                {
                    LOG_W("radio_%d third heart fail\r\n",info_temp->freq);
                    info_temp->retry++;
                    heart_warning();
                    rt_thread_mdelay(5000);
                }
                break;
            case 3:
                heart_request(rf_now);
                rt_thread_mdelay(1000);
                File_Output(rf_now,Valve_Status,PSI_Status,info_temp->received,info_temp->retry,(info_temp->received>0)?info_temp->rssi:0,1,0);
                rf_now = !rf_now;
                if(info_temp->received)
                {
                    LOG_I("radio_%d final heart success\r\n",info_temp->freq);
                    info_temp->retry = 0;
                    info_temp->alive = 1;
                    heart_warning();
                    rt_thread_mdelay(3000);
                }
                else
                {
                    LOG_W("radio_%d final heart fail\r\n",info_temp->freq);
                    info_temp->alive = 0;
                    info_temp->retry = 0;
                    heart_warning();
                    rt_thread_mdelay(5000);
                }
                break;
            }
        }
    }
}
void heart_init(void)
{
    heart_t = rt_thread_create("heart", heart_callback, RT_NULL, 2048, 10, 10);
    if(heart_t!=RT_NULL)
    {
        rt_thread_startup(heart_t);
    }
}
MSH_CMD_EXPORT(heart_init,heart_init);
