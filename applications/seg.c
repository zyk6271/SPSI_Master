/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-12     Rick       the first version
 */
#include "rtthread.h"
#include "rtdevice.h"
#include "pin_config.h"
#include "seg.h"
#include "stdint.h"

#define DBG_TAG "seg"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t seg_t = RT_NULL;

uint8_t rf_pipe_rssi_count=0;
uint8_t rf_pipe_alive_conut=0;

uint8_t rf_lora_rssi_count=0;
uint8_t rf_lora_alive_conut=0;

uint8_t seg1_level=0;
uint8_t seg2_level=0;

void rf_pipe_rssi_count_increase(void)
{
    if(rf_pipe_rssi_count<5)
    {
        rf_pipe_rssi_count++;
    }
}

void rf_pipe_rssi_count_resume(void)
{
    rf_pipe_rssi_count = 0;
}

void rf_pipe_alive_count_increase(void)
{
    if(rf_pipe_alive_conut<2)
    {
        rf_pipe_alive_conut++;
    }
    rf_pipe_rssi_count = 0;
}

void rf_pipe_alive_count_resume(void)
{
    rf_pipe_alive_conut = 0;
}

void rf_lora_rssi_count_increase(void)
{
    if(rf_lora_rssi_count<5)
    {
        rf_lora_rssi_count++;
    }
}

void rf_lora_rssi_count_resume(void)
{
    rf_lora_rssi_count = 0;
}

void rf_lora_alive_count_increase(void)
{
    if(rf_lora_alive_conut<2)
    {
        rf_lora_alive_conut++;
    }
    rf_lora_rssi_count = 0;
}

void rf_lora_alive_count_resume(void)
{
    rf_lora_alive_conut = 0;
}

void alive_count_increase(uint8_t select)
{
    if(select)
    {
        rf_lora_alive_count_increase();
    }
    else
    {
        rf_pipe_alive_count_increase();
    }
}

void seg1_control(uint8_t level)
{
    if(seg1_level != level)
    {
        seg1_level = level;
        rt_pin_write(OUT1,level);
    }
}

void seg2_control(uint8_t level)
{
    if(seg2_level != level)
    {
        seg2_level = level;
        rt_pin_write(OUT2,level);
    }
}

void seg_callback(void *parameter)
{
    while(1)
    {
        if(rf_pipe_rssi_count > 4 || rf_lora_rssi_count > 4)
        {
            seg1_control(1);
        }
        else
        {
            seg1_control(0);
        }
        if(rf_pipe_alive_conut > 1 || rf_lora_alive_conut > 1)
        {
            seg2_control(1);
        }
        else
        {
            seg2_control(0);
        }
        rt_thread_mdelay(500);
    }
}
void seg_hw_init(void)
{
    rt_pin_mode(OUT1,PIN_MODE_OUTPUT);
    rt_pin_mode(OUT2,PIN_MODE_OUTPUT);
    rt_pin_write(OUT1,0);
    rt_pin_write(OUT2,0);
}
void seg_init(void)
{
    seg_hw_init();
    seg_t = rt_thread_create("seg_t", seg_callback, RT_NULL, 512, 10, 10);
    if(seg_t!=RT_NULL)
    {
        rt_thread_startup(seg_t);
    }
}
