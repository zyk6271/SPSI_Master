/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-23     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "Pin_Config.h"
#include "led.h"
#include "psi.h"

#define DBG_TAG "PSI"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_timer_t psi_timer = RT_NULL;
rt_thread_t psi_thread = RT_NULL;

static uint8_t psi_status = 0;
static uint8_t valve_status = 2;
static uint8_t psi_lost = 0;

uint8_t psi_status_get(void)
{
    return psi_status;
}

void psi_led_init(void)
{
    if(psi_status)
    {
        led_transmitter_on();
    }
    else
    {
        led_transmitter_off();
    }
}

void valve_led_init(void)
{
    if(valve_status)
    {
        led_receiver_on();
    }
    else
    {
        led_receiver_off();
    }
}

void psi_led_lost(void)
{
    if(psi_lost == 0)
    {
        psi_lost = 1;
        led_transmitter_lost();
        led_receiver_lost();
    }
}

void psi_led_resume(void)
{
    if(psi_lost)
    {
        psi_lost = 0;
        psi_led_init();
        valve_led_init();
        if(psi_status)
        {
            psi_open();
        }
        else
        {
            psi_close();
        }
    }
}

void psi_open(void)
{
    if(psi_status == 0 && psi_lost == 0)
    {
        psi_status = 1;
        led_transmitter_on();
        psi_upload(1);
    }
}
void psi_close(void)
{
    if(psi_status == 1 && psi_lost == 0)
    {
        psi_status = 0;
        led_transmitter_off();
        psi_upload(0);
    }
}

void psi_timer_callback(void *parameter)
{
    psi_close();
}

void valve_control(uint8_t state)
{
    if(state)
    {
        valve_open();
    }
    else
    {
        valve_close();
    }
}

uint8_t valve_status_get(void)
{
    return valve_status;
}

void valve_open(void)
{
    if(valve_status != 1 && psi_lost == 0)
    {
        led_receiver_on();
    }
    valve_status = 1;
}

void valve_close(void)
{
    if(valve_status != 0 && psi_lost == 0)
    {
        led_receiver_off();
        led_transmitter_warning();
    }
    valve_status = 0;
}

void psi_thread_callback(void *parameter)
{
    while(1)
    {
        if(rt_pin_read(AC) == 0)
        {
            psi_open();
            rt_timer_start(psi_timer);
        }
        rt_thread_mdelay(200);
    }
}
void psi_init(void)
{
    rt_pin_mode(AC, PIN_MODE_INPUT);
    psi_timer = rt_timer_create("psi_timer", psi_timer_callback, RT_NULL, 5000, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_ONE_SHOT);
    psi_thread = rt_thread_create("psi_thread", psi_thread_callback, RT_NULL, 1024, 10, 10);
    rt_thread_startup(psi_thread);
}
