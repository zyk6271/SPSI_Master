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
#include <agile_led.h>
#include <stdlib.h>
#include "Pin_Config.h"
#include "led.h"
#include "psi.h"

#define DBG_TAG "PSI"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_sem_t psi_sem = RT_NULL;
rt_timer_t psi_timer = RT_NULL;
rt_thread_t psi_thread = RT_NULL;

uint8_t PSI_Status=0;
uint8_t Valve_Status=2;
uint8_t Psi_Counter,Psi_Lost;

void psi_open(void)
{
    if(PSI_Status==0 && Psi_Lost==0)
    {
        PSI_Status = 1;
        transmitter_on();
        psi_upload(1);
        LOG_I("Detect psi is open\r\n");
    }
}
void psi_close(void)
{
    if(PSI_Status==1 && Psi_Lost==0)
    {
        PSI_Status = 0;
        transmitter_off();
        psi_upload(0);
        LOG_I("Detect psi is close\r\n");
    }
}
void psi_led_init(void)
{
    if(PSI_Status)
    {
        transmitter_on();
    }
    else
    {
        transmitter_off();
    }
}
void valve_led_init(void)
{
    if(Valve_Status)
    {
        receiver_on();
    }
    else
    {
        receiver_off();
    }
}
void psi_led_resume(void)
{
    if(Psi_Lost)
    {
        Psi_Lost = 0;
        psi_led_init();
        valve_led_init();
        if(PSI_Status)
        {
            psi_open();
        }
        else
        {
            psi_close();
        }
    }
}
void psi_led_lost(void)
{
    if(!Psi_Lost)
    {
        Psi_Lost = 1;
        transmitter_lost();
        receiver_lost();
    }
}
void psi_timer_callback(void *parameter)
{
    psi_close();
}
void psi_pin_callback(void *parameter)
{
   rt_sem_release(psi_sem);
   rt_pin_irq_enable(AC, PIN_IRQ_DISABLE);
}
void valve_control(uint8_t mode)
{
    if(mode)
    {
        valve_open();
    }
    else
    {
        valve_close();
    }
}
void valve_open(void)
{
    if(Valve_Status!=1 && Psi_Lost==0)
    {
        receiver_on();
    }
    Valve_Status = 1;
}
void valve_close(void)
{
    if(Valve_Status!=0 && Psi_Lost==0)
    {
        receiver_off();
    }
    Valve_Status = 0;
}
void psi_thread_callback(void *parameter)
{
    while(1)
    {
        if(rt_sem_take(psi_sem,100) == RT_EOK)
        {
            Psi_Counter++;
            if(Psi_Counter>4)
            {
                Psi_Counter = 0;
                psi_open();
                rt_timer_start(psi_timer);
            }
        }
        else
        {
            Psi_Counter = 0;
        }
        rt_pin_irq_enable(AC, PIN_IRQ_ENABLE);
        rt_thread_mdelay(200);
    }
}
void psi_init(void)
{
    rt_pin_mode(AC, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(AC, PIN_IRQ_MODE_FALLING, psi_pin_callback, RT_NULL);
    psi_sem = rt_sem_create("psi_sem", 0, RT_IPC_FLAG_PRIO);
    psi_timer = rt_timer_create("psi_timer", psi_timer_callback, RT_NULL, 5000, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_ONE_SHOT);
    psi_thread = rt_thread_create("psi_thread", psi_thread_callback, RT_NULL, 1024, 11, 10);
    if(psi_thread!=RT_NULL)
    {
        rt_thread_startup(psi_thread);
    }
    rt_pin_irq_enable(AC, PIN_IRQ_ENABLE);
}
