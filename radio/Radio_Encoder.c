/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-22     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdio.h>
#include "AX5043.h"
#include "Radio_Encoder.h"
#include "Radio_Drv.h"
#include "Radio_Common.h"

#define DBG_TAG "RF_EN"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_thread_t Radio_Queue433 = RT_NULL;
rt_thread_t Radio_Queue4068 = RT_NULL;

extern struct ax5043 rf_433;
extern struct ax5043 rf_4068;

uint32_t Self_ID = 0;
uint32_t Target_ID = 0;
uint32_t Default_Self_ID = 50000000;
uint32_t Default_Target_ID = 60000000;

typedef struct
{
    uint8_t NowNum;
    uint8_t TargetNum;
    uint32_t Taget_Id[100];
    uint8_t Counter[100];
    uint8_t Command[100];
    uint8_t Data[100];
}Radio_Queue;

Radio_Queue Queue_433={0};
Radio_Queue Queue_4068={0};

void rf_433_Enqueue(uint32_t target_id,uint8_t control,uint8_t value)
{
    uint8_t NumTemp = Queue_433.TargetNum;
    if(NumTemp<100)
    {
        NumTemp ++;
        LOG_D("Queue Num Increase,Value is %d\r\n",NumTemp);
    }
    else
    {
        LOG_I("Queue is Full,Value is %d\r\n",NumTemp);
        return;
    }
    Queue_433.Taget_Id[NumTemp] = target_id;
    Queue_433.Command[NumTemp] = control;
    Queue_433.Data[NumTemp] = value;
    Queue_433.TargetNum++;
    LOG_D("GatewayDataEnqueue Success\r\n");
}
void rf_433_send(uint32_t target_id,uint8_t control,uint8_t value)
{
    uint8_t *buf = rt_malloc(50);
    sprintf((char *)buf,"M{%08ld,%08ld,%02d,%02d}M",target_id,Self_ID,control,value);
    Normal_send(&rf_433,buf,28);
    rt_free(buf);
}
void rf_433_Dequeue(void *paramaeter)
{
    LOG_I("rf_433_Dequeue Init Success\r\n");
    while(1)
    {
        if(Queue_433.NowNum == Queue_433.TargetNum)
        {
            Queue_433.NowNum = 0;
            Queue_433.TargetNum = 0;
        }
        else if(Queue_433.TargetNum>0 && Queue_433.TargetNum>Queue_433.NowNum)
        {
            Queue_433.NowNum++;
            rt_thread_mdelay(50);
            rf_433_send(Queue_433.Taget_Id[Queue_433.NowNum],Queue_433.Command[Queue_433.NowNum],Queue_433.Data[Queue_433.NowNum]);
            LOG_D("rf_433 Send With Now Num %d,Target Num is %d,Target_Id %ld,counter %d,command %d,data %d\r\n",Queue_433.NowNum,Queue_433.TargetNum,Queue_433.Taget_Id[Queue_433.NowNum],Queue_433.Counter[Queue_433.NowNum],Queue_433.Command[Queue_433.NowNum],Queue_433.Data[Queue_433.NowNum]);
            rt_thread_mdelay(100);
        }
        rt_thread_mdelay(50);
    }
}
void rf_4068_Enqueue(uint32_t target_id,uint8_t control,uint8_t value)
{
    uint8_t NumTemp = Queue_4068.TargetNum;
    if(NumTemp<100)
    {
        NumTemp ++;
        LOG_D("Queue Num Increase,Value is %d\r\n",NumTemp);
    }
    else
    {
        LOG_I("Queue is Full,Value is %d\r\n",NumTemp);
        return;
    }
    Queue_4068.Taget_Id[NumTemp] = target_id;
    Queue_4068.Command[NumTemp] = control;
    Queue_4068.Data[NumTemp] = value;
    Queue_4068.TargetNum++;
    LOG_D("GatewayDataEnqueue Success\r\n");
}
void rf_4068_send(uint32_t target_id,uint8_t control,uint8_t value)
{
    uint8_t *buf = rt_malloc(50);
    sprintf((char *)buf,"M{%08ld,%08ld,%02d,%02d}M",target_id,Self_ID,control,value);
    Normal_send(&rf_4068,buf,28);
    rt_free(buf);
}
void rf_4068_Dequeue(void *paramaeter)
{
    LOG_I("rf_4068_Dequeue Init Success\r\n");
    while(1)
    {
        if(Queue_4068.NowNum == Queue_4068.TargetNum)
        {
            Queue_4068.NowNum = 0;
            Queue_4068.TargetNum = 0;
        }
        else if(Queue_4068.TargetNum>0 && Queue_4068.TargetNum>Queue_4068.NowNum)
        {
            Queue_4068.NowNum++;
            rt_thread_mdelay(50);
            rf_4068_send(Queue_4068.Taget_Id[Queue_4068.NowNum],Queue_4068.Command[Queue_4068.NowNum],Queue_4068.Data[Queue_4068.NowNum]);
            LOG_D("rf_4068 Send With Now Num %d,Target Num is %d,Target_Id %ld,counter %d,command %d,data %d\r\n",Queue_4068.NowNum,Queue_4068.TargetNum,Queue_4068.Taget_Id[Queue_4068.NowNum],Queue_4068.Counter[Queue_4068.NowNum],Queue_4068.Command[Queue_4068.NowNum],Queue_4068.Data[Queue_4068.NowNum]);
            rt_thread_mdelay(100);
        }
        rt_thread_mdelay(50);
    }
}
void RadioQueueInit(void)
{
//    int *p;
//    p=(int *)(0x0801FFFC);//这就是已知的地址，要强制类型转换
//    Self_Id = *p;//从Flash加载ID
//    if(Self_Id==0xFFFFFFFF || Self_Id==0)
//    {
//        Self_Id = Self_Default_Id;
//    }
    Self_ID = Default_Self_ID;
    Target_ID = Default_Target_ID;
    Radio_Queue433 = rt_thread_create("Radio_Queue433", rf_433_Dequeue, RT_NULL, 1024, 10, 10);
    if(Radio_Queue433)rt_thread_startup(Radio_Queue433);
    Radio_Queue4068 = rt_thread_create("Radio_Queue4068", rf_4068_Dequeue, RT_NULL, 1024, 10, 10);
    if(Radio_Queue4068)rt_thread_startup(Radio_Queue4068);
}
MSH_CMD_EXPORT(RadioQueueInit,RadioQueueInit);
