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
#include <stdlib.h>
#include "little.h"
#include "file.h"
#include "heart.h"
#include "Radio_Encoder.h"
#include "led.h"
#include "psi.h"
#include "seg.h"
#include "warn.h"

#define DBG_TAG "heart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define send 0
#define check 1
#define randtime_7 2
#define randtime_8 3

rt_thread_t heart_t = RT_NULL;
rt_thread_t buttontest_t = RT_NULL;

rt_timer_t connect_timer = RT_NULL;
rt_timer_t heart_timer = RT_NULL;

rt_sem_t connect_sem = RT_NULL;
rt_sem_t heart_sem = RT_NULL;
rt_sem_t button_sem = RT_NULL;

rf_info info_433;
rf_info info_4068;

uint8_t rf_now ;

extern uint32_t Target_ID ;
extern uint8_t PSI_Status,Valve_Status;
extern struct rt_event rf_event;

uint8_t heart_mode;
uint32_t rand_time;
uint32_t remain_time;

void heart_request(uint8_t rf_select)
{
    rf_now = rf_select;
    switch(rf_select)
    {
    case 0:
        info_4068.received = 0;
        rf_4068_Urgent_Enqueue(Target_ID,0,PSI_Status);
        break;
    case 1:
        info_433.received = 0;
        rf_433_Urgent_Enqueue(Target_ID,0,PSI_Status);
        break;
    }
}
void button_request(uint8_t rf_select)
{
    rf_now = rf_select;
    switch(rf_select)
    {
    case 0:
        info_4068.testreceived = 0;
        rf_4068_Enqueue(Target_ID,1,PSI_Status);
        break;
    case 1:
        info_433.testreceived = 0;
        rf_433_Enqueue(Target_ID,1,PSI_Status);
        break;
    }
}
void heart_single(void)
{
    rt_sem_release(button_sem);
}
MSH_CMD_EXPORT(heart_single,heart_single);
void psi_upload(uint8_t value)
{
    if(info_4068.alive==1 || info_433.alive==1 )
    {
        rf_433_Enqueue(Target_ID,2,value);
        rf_4068_Enqueue(Target_ID,2,value);
    }
}
void connect_timer_callback(void *parameter)
{
    rt_sem_release(connect_sem);
}
void heart_time_start(uint32_t time)
{
    rt_timer_control(heart_timer, RT_TIMER_CTRL_SET_TIME, &time);
    rt_timer_start(heart_timer);
}
void heart_time_increase(uint32_t time)
{
    uint32_t temp;
    rt_timer_control(heart_timer, RT_TIMER_CTRL_GET_TIME, &temp);
    temp +=time;
    rt_timer_control(heart_timer, RT_TIMER_CTRL_SET_TIME, &temp);
    rt_timer_start(heart_timer);
}
void heart_timer_callback(void *parameter)
{
    rt_sem_release(heart_sem);
}
void radio_refresh(rf_info *temp)
{
    temp->retry = 0;
    temp->alive = 1;
    temp->startup = 1;
    rf_now = !rf_now;
}
uint32_t get_srand_time(uint32_t min,uint32_t max)
{
    uint32_t value;
    srand(rt_tick_get());
    value = rand() % (max*10) + min*10;
    if(value==0)value = 1;
    LOG_D("Rand Time is %d\r\n",value*100);
    return value*100;
}
void rf_offline(rf_info *temp)
{
    if(temp->alive)
    {
        temp->alive=0;
        if(temp->freq == 433)
        {
            rt_event_send(&rf_event, RF433_Offline);
        }
        else
        {
            rt_event_send(&rf_event, RF4068_Offline);
        }
    }
    else if(temp->startup==0)
    {
        temp->startup = 1;
        if(temp->freq == 433)
        {
            rt_event_send(&rf_event, RF433_Offline);
        }
        else
        {
            rt_event_send(&rf_event, RF4068_Offline);
        }
    }
    LOG_I("rf %d is offline\r\n",temp->freq);
}
void rf_write(uint8_t rf_select,int rssi)
{
    if(rf_select){
        File_Output(1,Valve_Status,PSI_Status,1,info_433.retry,rssi,1,0);
    }
    else {
        File_Output(0,Valve_Status,PSI_Status,1,info_4068.retry,rssi,1,0);
    }
}
rt_mutex_t rf_lock = RT_NULL;
void buttontest_callback(void *parameter)
{
    rf_info *info_temp = rt_malloc(sizeof(rf_info));
    while(1)
    {
        if(rf_now)
        {
            info_temp = &info_433;
        }
        else
        {
            info_temp = &info_4068;
        }
        if(rt_sem_take(button_sem,100) == RT_EOK)
        {
            button_request(rf_now);
            receiver_close();
            switch(info_temp->retry)
            {
            case 0:
                rt_thread_mdelay(1000);
                if(info_temp->testreceived)
                {
                    File_Output(rf_now,Valve_Status,PSI_Status,1,0,info_temp->rssi,1,1);
                    LOG_I("radio_%d first heart success\r\n",info_temp->freq);
                    info_temp->retry = 0;
                    info_temp->alive = 1;
                    info_temp->startup = 1;
                    if(rt_mutex_trytake(rf_lock)==RT_EOK)
                    {
                        rf_now = !rf_now;
                        rt_mutex_release(rf_lock);
                    }
                }
                else
                {
                    LOG_W("radio_%d first heart fail\r\n",info_temp->freq);
                    info_temp->retry++;
                }
                break;
            case 1:
                rt_thread_mdelay(1000);
                if(info_temp->testreceived)
                {
                    File_Output(rf_now,Valve_Status,PSI_Status,1,1,info_temp->rssi,1,1);
                    LOG_I("radio_%d second heart success\r\n",info_temp->freq);
                    info_temp->retry = 0;
                    info_temp->alive = 1;
                    info_temp->startup = 1;
                    if(rt_mutex_trytake(rf_lock)==RT_EOK)
                    {
                        rf_now = !rf_now;
                        rt_mutex_release(rf_lock);
                    }
                }
                else
                {
                    LOG_W("radio_%d second heart fail\r\n",info_temp->freq);
                    info_temp->retry++;
                }
                break;
            case 2:
                rt_thread_mdelay(1000);
                if(info_temp->testreceived)
                {
                    File_Output(rf_now,Valve_Status,PSI_Status,1,2,info_temp->rssi,1,1);
                    LOG_I("radio_%d third heart success\r\n",info_temp->freq);
                    info_temp->retry = 0;
                    info_temp->alive = 1;
                    info_temp->startup = 1;
                    if(rt_mutex_trytake(rf_lock)==RT_EOK)
                    {
                        rf_now = !rf_now;
                        rt_mutex_release(rf_lock);
                    }
                }
                else
                {
                    LOG_W("radio_%d third heart fail\r\n",info_temp->freq);
                    info_temp->retry++;
                }
                break;
            case 3:
                rt_thread_mdelay(1000);
                File_Output(rf_now,Valve_Status,PSI_Status,info_temp->testreceived,3,(info_temp->testreceived>0)?info_temp->rssi:0,1,1);
                if(info_temp->testreceived)
                {
                    LOG_I("radio_%d final heart success\r\n",info_temp->freq);
                    info_temp->retry = 0;
                    info_temp->alive = 1;
                    info_temp->startup = 1;
                    if(rt_mutex_trytake(rf_lock)==RT_EOK)
                    {
                        rf_now = !rf_now;
                        rt_mutex_release(rf_lock);
                    }
                }
                else
                {
                    LOG_W("radio_%d final heart fail\r\n",info_temp->freq);
                    alive_count_increase(rf_now);
                    if(rt_mutex_trytake(rf_lock)==RT_EOK)
                    {
                        rf_now = !rf_now;
                        rt_mutex_release(rf_lock);
                    }
                    rf_offline(info_temp);
                    info_temp->retry = 0;
                }
                break;
            }
        }
    }
}
void heart_callback(void *parameter)
{
    rf_info *info_temp = rt_malloc(sizeof(rf_info));
    while(1)
    {
        if(rt_sem_take(connect_sem,100)==RT_EOK)
        {
            heart_request(1);
            rt_thread_mdelay(1000);
            if(info_433.received)
            {
                LOG_I("rf_connected success\r\n");
                File_Output(1,Valve_Status,PSI_Status,info_433.received,info_433.retry,info_433.rssi,0,0);
                rt_timer_stop(connect_timer);
                radio_refresh(&info_433);
                heart_time_start(10000);
                heart_mode = send;
            }
            else
            {
                LOG_W("rf_connected fail\r\n");
            }
        }
        if(rt_sem_take(heart_sem,100) == RT_EOK)
        {
            switch(heart_mode)
            {
            case send://发送
                if(rf_now)
                {
                    info_temp = &info_433;
                }
                else
                {
                    info_temp = &info_4068;
                }
                heart_request(rf_now);
                heart_time_start(1000);
                heart_mode = check;
                rt_mutex_trytake(rf_lock);
                break;
            case 1://应答检查
                switch(info_temp->retry)
                {
                case 0:
                    if(info_temp->received)
                    {
                        heart_mode = send;
                        File_Output(rf_now,Valve_Status,PSI_Status,1,0,info_temp->rssi,1,0);
                        LOG_I("radio_%d first heart success\r\n",info_temp->freq);
                        radio_refresh(info_temp);
                        heart_time_start(28700);
                    }
                    else
                    {
                        heart_mode = randtime_7;
                        LOG_W("radio_%d first heart fail\r\n",info_temp->freq);
                        info_temp->retry++;
                        rt_sem_release(heart_sem);
                    }
                    break;
                case 1:
                    if(info_temp->received)
                    {
                        heart_mode = send;
                        File_Output(rf_now,Valve_Status,PSI_Status,1,1,info_temp->rssi,1,0);
                        LOG_I("radio_%d second heart success\r\n",info_temp->freq);
                        radio_refresh(info_temp);
                        heart_time_start(28700);
                    }
                    else
                    {
                        heart_mode = randtime_7;
                        LOG_W("radio_%d second heart fail\r\n",info_temp->freq);
                        info_temp->retry++;
                        rt_sem_release(heart_sem);
                    }
                    break;
                case 2:
                    if(info_temp->received)
                    {
                        heart_mode = send;
                        File_Output(rf_now,Valve_Status,PSI_Status,1,2,info_temp->rssi,1,0);
                        LOG_I("radio_%d third heart success\r\n",info_temp->freq);
                        radio_refresh(info_temp);
                        heart_time_start(28700);
                    }
                    else
                    {
                        heart_mode = randtime_8;
                        LOG_W("radio_%d third heart fail\r\n",info_temp->freq);
                        info_temp->retry++;
                        heart_time_start(1000);
                    }
                    break;
                case 3:
                    File_Output(rf_now,Valve_Status,PSI_Status,info_temp->received,3,(info_temp->received>0)?info_temp->rssi:0,1,0);
                    if(info_temp->received)
                    {
                        LOG_I("radio_%d final heart success\r\n",info_temp->freq);
                        radio_refresh(info_temp);
                        heart_time_start(28700);
                    }
                    else
                    {
                        LOG_W("radio_%d final heart fail\r\n",info_temp->freq);
                        alive_count_increase(rf_now);
                        rf_now = !rf_now;
                        rf_offline(info_temp);
                        info_temp->retry = 0;
                        rt_sem_release(heart_sem);
                    }
                    heart_mode = send;
                    break;
                }
                rt_mutex_release(rf_lock);
                break;
            case randtime_7://第1，2次随机发送等待
                heart_mode = send;
                rand_time = get_srand_time(0,7);
                heart_time_start(rand_time);
                break;
            case randtime_8://第3次随机发送等待
                heart_mode = send;
                rand_time = get_srand_time(0,8);
                heart_time_start(rand_time);
                break;
            }
        }
    }
}
void heart_init(void)
{
    info_433.freq = 433;
    info_4068.freq = 4068;
    rf_lock = rt_mutex_create("rf_Lock", RT_IPC_FLAG_PRIO);
    heart_t = rt_thread_create("heart", heart_callback, RT_NULL, 2048, 7, 10);
    buttontest_t = rt_thread_create("buttontest", buttontest_callback, RT_NULL, 2048, 7, 10);
    connect_sem = rt_sem_create("connect_sem", 0, RT_IPC_FLAG_PRIO);
    connect_timer = rt_timer_create("connect", connect_timer_callback, RT_NULL, 5000, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_PERIODIC);
    heart_sem = rt_sem_create("heart_sem", 0, RT_IPC_FLAG_PRIO);
    heart_timer = rt_timer_create("heart", heart_timer_callback, RT_NULL, 1000, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_ONE_SHOT);
    button_sem = rt_sem_create("button_sem", 0, RT_IPC_FLAG_PRIO);
    if(heart_t!=RT_NULL)
    {
        rt_thread_startup(heart_t);
    }
    if(buttontest_t!=RT_NULL)
    {
        rt_thread_startup(buttontest_t);
    }
    rt_timer_start(connect_timer);
}
MSH_CMD_EXPORT(heart_init,heart_init);
