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

uint8_t Psi_status,Valve_Status;
uint8_t Psi_Lost;
void psi_led_init(void)
{
    if(Psi_status)
    {
        transmitter_on();
    }
    else
    {
        transmitter_off();
    }
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
    }
}
void psi_led_lost(void)
{
    if(Psi_Lost==0)
    {
        Psi_Lost = 1;
        transmitter_lost();
        receiver_lost();
    }
}
rt_timer_t psi_timer = RT_NULL;
void psi_timer_callback(void *parameter)
{
    //close valve

}
void psi_pin_callback(void *parameter)
{
    //open valve
    rt_timer_start(psi_timer);
}
void psi_init(void)
{
    rt_pin_mode(AC, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(AC, PIN_IRQ_MODE_FALLING, psi_pin_callback, RT_NULL);
    rt_pin_irq_enable(AC, PIN_IRQ_ENABLE);
    rt_timer_create("psi_timer", psi_timer_callback, RT_NULL, 5000, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_ONE_SHOT);
}
