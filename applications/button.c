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

void test_callback(agile_btn_t *btn)
{
    heart_single();
    LOG_I("heart single\r\n");
}
void button_init(void)
{
    test_btn = agile_btn_create(SW1, PIN_LOW, PIN_MODE_INPUT_PULLUP);
    agile_btn_set_elimination_time(test_btn,100);
    agile_btn_set_event_cb(test_btn, BTN_PRESS_UP_EVENT, test_callback);
    agile_btn_start(test_btn);
}
