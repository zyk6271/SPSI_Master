/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-22     Rick       the first version
 */
#include <agile_button.h>
#include <stdlib.h>
#include <Pin_Config.h>

#define DBG_TAG "BUTTON"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static agile_btn_t *test_btn = RT_NULL;

void test_single_callback(agile_btn_t *btn)
{
    test_btn_heart();
    LOG_I("heart single\r\n");
}

void test_hold_callback(agile_btn_t *btn)
{
    beep_start(4);
    rt_thread_mdelay(3000);
    rt_hw_cpu_reset();
    LOG_I("heart hold\r\n");
}

void button_init(void)
{
    test_btn = agile_btn_create(SW1, PIN_LOW, PIN_MODE_INPUT_PULLUP);
    agile_btn_set_elimination_time(test_btn,50);
    agile_btn_set_hold_cycle_time(test_btn,3000);
    agile_btn_set_event_cb(test_btn, BTN_CLICK_EVENT, test_single_callback);
    agile_btn_set_event_cb(test_btn, BTN_HOLD_EVENT, test_hold_callback);
    agile_btn_start(test_btn);
}
