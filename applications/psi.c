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
