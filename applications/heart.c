#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "heart.h"

#define DBG_TAG "heart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static uint8_t rf_now = 0;
static uint8_t test_btn_heart_start = 0;

rf_status rf_pipe_status = {0};
rf_status rf_lora_status = {1};

rt_thread_t heart_t = RT_NULL;
rt_timer_t test_btn_heart_timer = RT_NULL;

void psi_upload(uint8_t value)
{
    if (rf_pipe_status.alive == 1 || rf_lora_status.alive == 1)
    {
        rf_lora_enqueue(2, value);
        rf_pipe_enqueue(2, value);
    }
}

uint8_t get_srand_time(void)
{
    uint32_t value;
    srand(rt_tick_get());
    value = rand() % 10 + 20;
    return value;
}

uint8_t rf_heart_check(rf_status *handle,uint8_t *receive_flag)
{
    if(*receive_flag == 0)
    {
        LOG_D("RF %s heart failed count %d\n",(handle->rf_id > 0)?"LoRa":"PIPE",handle->retry);
        if(handle->retry < 3)
        {
            handle->retry++;
        }
        else
        {
            handle->alive = 0;
            rf_offline(handle);
            alive_count_increase(handle->rf_id);
        }
        return RT_ERROR;
    }
    else
    {
        handle->retry = 0;
        LOG_D("RF %s heart successfully\n",(handle->rf_id > 0)?"LoRa":"PIPE",handle->retry);
        return RT_EOK;
    }
}

void test_btn_heart_callback(void *parameter)
{
    test_btn_heart_start = 0;

    heart_record_write(0,rf_pipe_status.test_received,rf_pipe_status.retry,rf_pipe_status.rssi,rf_pipe_status.snr,1);
    heart_record_write(1,rf_lora_status.test_received,rf_lora_status.retry,rf_lora_status.rssi,rf_lora_status.snr,1);
    rf_heart_check(&rf_pipe_status,&rf_pipe_status.test_received);
    rf_heart_check(&rf_lora_status,&rf_lora_status.test_received);
}

void test_btn_heart(void)
{
    if(test_btn_heart_start)
    {
        return;
    }
    else
    {
        test_btn_heart_start = 1;
    }

    rf_pipe_status.test_received = 0;
    rf_lora_status.test_received = 0;
    rf_pipe_urgent_enqueue(1, psi_status_get());
    rf_lora_urgent_enqueue(1, psi_status_get());

    rt_timer_start(test_btn_heart_timer);
}

void heart_callback(void *parameter)
{
    uint8_t connect_success = 0;
    while(connect_success == 0)
    {
        rt_thread_mdelay(5000);
        if(rf_lora_status.received == 0)
        {
            rf_lora_urgent_enqueue(0, psi_status_get());
            LOG_D("RF try to connect now\n");
        }
        else
        {
            connect_success = 1;
            LOG_D("RF connect successfully\n");
        }
    }
    while(connect_success == 1)
    {
        if(rf_now)
        {
            rf_now = 0;
            rf_lora_status.received = 0;
            rf_lora_urgent_enqueue(0, psi_status_get());
            rt_thread_mdelay(3000);
            heart_record_write(1,rf_lora_status.received,rf_lora_status.retry,rf_lora_status.rssi,rf_lora_status.snr,0);
            if(rf_heart_check(&rf_lora_status,&rf_lora_status.received) == RT_EOK)
            {
                rt_thread_mdelay(30 * 1000);
            }
            else
            {
                rt_thread_mdelay(get_srand_time() * 1000);
            }
        }
        else
        {
            rf_now = 1;
            rf_pipe_status.received = 0;
            rf_pipe_urgent_enqueue(0, psi_status_get());
            rt_thread_mdelay(3000);
            heart_record_write(0,rf_pipe_status.received,rf_pipe_status.retry,rf_pipe_status.rssi,rf_pipe_status.snr,0);
            if(rf_heart_check(&rf_pipe_status,&rf_pipe_status.received) == RT_EOK)
            {
                rt_thread_mdelay(30 * 1000);
            }
            else
            {
                rt_thread_mdelay(get_srand_time() * 1000);
            }
        }
    }
}

void heart_init(void)
{
    test_btn_heart_timer = rt_timer_create("test_btn_heart", test_btn_heart_callback, RT_NULL, 3000, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
    heart_t = rt_thread_create("heart_t", heart_callback, RT_NULL, 2048, 10, 10);
    rt_thread_startup(heart_t);
}
