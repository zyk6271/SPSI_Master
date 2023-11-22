/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-20     Rick       the first version
 */
#include <rtdevice.h>
#include <spi_flash.h>
#include <drv_spi.h>
#include <string.h>
#include <stdlib.h>
#include "dfs_fs.h"
#include "storage.h"
#include "agile_button.h"

#define DBG_TAG "filesystem"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_event_t sd_event_t;
rt_thread_t sd_watch_t = RT_NULL;;

static uint8_t sd_valid = 0;
static agile_btn_t *SD_Sense_Button = RT_NULL;

uint8_t get_sd_valid(void)
{
    return sd_valid;
}

void sd_mount(void)
{
    uint8_t retry_count = 3;
    while(retry_count -- > 0)
    {
        if(rt_spi_tf_init() == RT_EOK)
        {
            break;
        }
        else
        {
            LOG_E("rt_spi_tf_init fail\r\n");
        }
    }
    if (dfs_mount("sd0", "/", "elm", 0, 0) == RT_EOK)
    {
        sd_valid = 1;
        LOG_I("dfs_mount success\r\n");
    }
    else
    {
        sd_valid = 0;
        LOG_E("dfs_mount fail\r\n");
    }
}

void sd_unmount(void)
{
    if(dfs_unmount("/") == RT_EOK)
    {
        sd_valid = 0;
        LOG_I("dfs_unmount success\r\n");
    }
    else
    {
        LOG_E("dfs_unmount fail\r\n");
    }
}

void sd_watch_callback(void *parameter)
{
    rt_uint32_t e;
    while(1)
    {
        rt_event_recv(sd_event_t, (SDCARD_IN | SDCARD_OUT),RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &e);
        switch(e)
        {
        case SDCARD_IN:
            sd_mount();
            break;
        case SDCARD_OUT:
            sd_unmount();
            break;
        default:
            break;
        }
    }
}

uint64_t sd_total_size_get(void)
{
    uint8_t result;
    long long cap;
    struct statfs buffer;

    result = dfs_statfs("/", &buffer);
    if (result != 0)
    {
        rt_kprintf("dfs_statfs failed.\n");
        return -1;
    }

    cap = ((long long)buffer.f_bsize) * ((long long)buffer.f_blocks) / 1024LL;
    return cap;
}

uint64_t sd_free_size_get(void)
{
    uint8_t result;
    long long cap;
    struct statfs buffer;

    result = dfs_statfs("/", &buffer);
    if (result != 0)
    {
        rt_kprintf("dfs_statfs failed.\n");
        return -1;
    }
    cap = ((long long)buffer.f_bsize) * ((long long)buffer.f_bfree) / 1024LL;
    return cap;
}

static void sd_in_event_cb(agile_btn_t *btn)
{
    LOG_I("sd_in_event_cb\r\n");
    rt_event_send(sd_event_t,SDCARD_IN);
}

static void sd_out_event_cb(agile_btn_t *btn)
{
    LOG_I("sd_out_event_cb\r\n");
    rt_event_send(sd_event_t,SDCARD_OUT);
}

int storage_Init(void)
{
    SD_Sense_Button = agile_btn_create(45, 0, PIN_MODE_INPUT);
    agile_btn_set_event_cb(SD_Sense_Button, BTN_PRESS_DOWN_EVENT, sd_in_event_cb);
    agile_btn_set_event_cb(SD_Sense_Button, BTN_PRESS_UP_EVENT, sd_out_event_cb);
    agile_btn_start(SD_Sense_Button);

    rt_hw_spi_device_attach("spi1", "sd_spi", GPIOA, GPIO_PIN_15);
    msd_init("sd0", "sd_spi");

    sd_event_t = rt_event_create("sd_event", RT_IPC_FLAG_PRIO);
    sd_watch_t = rt_thread_create("sd_watch",sd_watch_callback,RT_NULL,1024,25,10);
    rt_thread_startup(sd_watch_t);

    return RT_EOK;
}
