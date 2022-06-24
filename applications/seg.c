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
#include "heart.h"

#define DBG_TAG "seg"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_thread_t seg_t = RT_NULL;

uint8_t rf4068_rssi_count=0;
uint8_t rf4068_alive_conut=0;
uint8_t rf433_rssi_count=0;
uint8_t rf433_alive_conut=0;
uint8_t seg1_level=0;
uint8_t seg2_level=0;

void rf4068_rssi_count_increase(void)
{
    if(rf4068_rssi_count<5)
    {
        rf4068_rssi_count++;
    }
}
void rf4068_rssi_count_resume(void)
{
    rf4068_rssi_count = 0;
}
void rf4068_alive_count_increase(void)
{
    if(rf4068_alive_conut<2)
    {
        rf4068_alive_conut++;
    }
    rf4068_rssi_count = 0;
}
void rf4068_alive_count_resume(void)
{
    rf4068_alive_conut = 0;
}
void rf433_rssi_count_increase(void)
{
    if(rf433_rssi_count<5)
    {
        rf433_rssi_count++;
    }
}
void rf433_rssi_count_resume(void)
{
    rf433_rssi_count = 0;
}
void rf433_alive_count_increase(void)
{
    if(rf433_alive_conut<2)
    {
        rf433_alive_conut++;
    }
    rf433_rssi_count = 0;
}
void rf433_alive_count_resume(void)
{
    rf433_alive_conut = 0;
}
void alive_count_increase(uint8_t select)
{
    if(select)
    {
        rf433_alive_count_increase();
    }
    else
    {
        rf4068_alive_count_increase();
    }
}
void seg1_control(uint8_t level)
{
    if(seg1_level!=level)
    {
        seg1_level = level;
        rt_pin_write(OUT1,level);
        LOG_I("seg 1 is write to %d\r\n",level);
    }
}
void seg2_control(uint8_t level)
{
    if(seg2_level!=level)
    {
        seg2_level = level;
        rt_pin_write(OUT2,level);
        LOG_I("seg 2 is write to %d\r\n",level);
    }
}
void seg_callback(void *parameter)
{
    rt_pin_mode(OUT1,PIN_MODE_OUTPUT);
    rt_pin_mode(OUT2,PIN_MODE_OUTPUT);
    rt_pin_write(OUT1,0);
    rt_pin_write(OUT2,0);
    while(1)
    {
        if(rf4068_rssi_count>4 || rf433_rssi_count>4)
        {
            seg1_control(1);
        }
        else
        {
            seg1_control(0);
        }
        if(rf4068_alive_conut>1 || rf433_alive_conut>1)
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
void seg_init(void)
{
    seg_t = rt_thread_create("seg", seg_callback, RT_NULL, 1024, 10, 10);
    if(seg_t!=RT_NULL)
    {
        rt_thread_startup(seg_t);
    }
}
