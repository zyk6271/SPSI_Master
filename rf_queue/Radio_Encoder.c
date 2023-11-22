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

#define DBG_TAG "RF_EN"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static struct rt_completion rf_lora_txdone_sem;
static struct rt_completion rf_pipe_txdone_sem;

rt_thread_t rf_encode_lora_t = RT_NULL;
rt_thread_t rf_encode_pipe_t = RT_NULL;

const char device_id[4] = {0x31,0x32,0x33,0x34};
const char target_id[4] = {0x34,0x33,0x32,0x31};

static rt_mq_t rf_mq_lora;
static rt_mq_t rf_mq_pipe;

const char* rf_device_id_get(void)
{
    return device_id;
}

const char* rf_target_id_get(void)
{
    return target_id;
}

void rf_lora_enqueue(uint8_t control,uint8_t value)
{
    Radio_Frame_Format send_buf = {0};
    rt_memset(&send_buf,0,sizeof(Radio_Frame_Format));

    rt_memcpy(send_buf.device_id,device_id,4);
    rt_memcpy(send_buf.target_id,target_id,4);
    send_buf.control = control;
    send_buf.value = value;

    rt_mq_send(rf_mq_lora, &send_buf, sizeof(Radio_Frame_Format));
}
void rf_lora_urgent_enqueue(uint8_t control,uint8_t value)
{
    Radio_Frame_Format send_buf = {0};
    rt_memset(&send_buf,0,sizeof(Radio_Frame_Format));

    rt_memcpy(send_buf.device_id,device_id,4);
    rt_memcpy(send_buf.target_id,target_id,4);
    send_buf.control = control;
    send_buf.value = value;

    rt_mq_urgent(rf_mq_lora, &send_buf, sizeof(Radio_Frame_Format));
}

void rf_lora_txdone_callback(void)
{
    rt_completion_done(&rf_lora_txdone_sem);
}

void rf_pipe_enqueue(uint8_t control,uint8_t value)
{
    Radio_Frame_Format send_buf = {0};
    rt_memset(&send_buf,0,sizeof(Radio_Frame_Format));

    rt_memcpy(send_buf.device_id,device_id,4);
    rt_memcpy(send_buf.target_id,target_id,4);
    send_buf.control = control;
    send_buf.value = value;

    rt_mq_send(rf_mq_pipe, &send_buf, sizeof(Radio_Frame_Format));
}

void rf_pipe_urgent_enqueue(uint8_t control,uint8_t value)
{
    Radio_Frame_Format send_buf = {0};
    rt_memset(&send_buf,0,sizeof(Radio_Frame_Format));

    rt_memcpy(send_buf.device_id,device_id,4);
    rt_memcpy(send_buf.target_id,target_id,4);
    send_buf.control = control;
    send_buf.value = value;

    rt_mq_urgent(rf_mq_pipe, &send_buf, sizeof(Radio_Frame_Format));
}

void rf_pipe_txdone_callback(void)
{
    rt_completion_done(&rf_pipe_txdone_sem);
}

void rf_encode_lora_entry(void *paramaeter)
{
    Radio_Frame_Format send_buf;
    while (1)
    {
        rt_memset(&send_buf,0,sizeof(Radio_Frame_Format));
        rt_mq_recv(rf_mq_lora,&send_buf, sizeof(Radio_Frame_Format), RT_WAITING_FOREVER);
        rf_lora_send(&send_buf,sizeof(Radio_Frame_Format));
        rt_thread_mdelay(600);
    }
}
void rf_encode_pipe_entry(void *paramaeter)
{
    Radio_Frame_Format send_buf;
    while (1)
    {
        rt_memset(&send_buf,0,sizeof(Radio_Frame_Format));
        rt_mq_recv(rf_mq_pipe,&send_buf, sizeof(Radio_Frame_Format), RT_WAITING_FOREVER);
        rf_pipe_send(&send_buf,sizeof(Radio_Frame_Format));
        rt_thread_mdelay(600);
    }
}
void rf_enqueue_init(void)
{
    rt_completion_init(&rf_lora_txdone_sem);
    rt_completion_init(&rf_pipe_txdone_sem);

    rf_mq_lora = rt_mq_create("rf_mq_lora", sizeof(Radio_Frame_Format), 5, RT_IPC_FLAG_PRIO);
    rf_mq_pipe = rt_mq_create("rf_mq_pipe", sizeof(Radio_Frame_Format), 5, RT_IPC_FLAG_PRIO);

    rf_encode_lora_t = rt_thread_create("rf_encode_lora", rf_encode_lora_entry, RT_NULL, 1024, 10, 10);
    rt_thread_startup(rf_encode_lora_t);
    rf_encode_pipe_t = rt_thread_create("rf_encode_pipe", rf_encode_pipe_entry, RT_NULL, 1024, 10, 10);
    rt_thread_startup(rf_encode_pipe_t);
}
