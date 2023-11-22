/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-23     Rick       the first version
 */
#include "rtthread.h"
#include "rtdevice.h"
#include "board.h"
#include "dfs.h"

#define DBG_TAG "record"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_err_t csv_write(char *buf,uint32_t size)
{
    time_t t;
    struct tm *tm_info;

    int file_exist = 0;
    char file_name[12];
    char write_ptr[64];
    char write_time[9];

    get_timestamp(&t);
    tm_info = localtime(&t);

    rt_sprintf(file_name, "%04d%02d%02d.csv",tm_info->tm_year + 1900,tm_info->tm_mon + 1,tm_info->tm_mday);

    file_exist = access(file_name, 0);

    int fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND);
    if(fd < 0)
    {
        return RT_ERROR;
    }

    if(file_exist != 0)
    {
        rt_sprintf(write_ptr,"DATE TIME RF VALVE PSI HANDSHAKE RETRY RSSI SNR BUTTON\n");
        write(fd, write_ptr, strlen(write_ptr));
        LOG_D("file create successfully\n");
    }

    rt_sprintf(write_time, "%04d/%02d/%02d %02d:%02d:%02d ",tm_info->tm_year + 1900,tm_info->tm_mon + 1,tm_info->tm_mday,tm_info->tm_hour,tm_info->tm_min,tm_info->tm_sec);
    write(fd, write_time, 20);

    write(fd, buf, size);
    close(fd);

    return RT_EOK;
}

rt_err_t heart_record_write(uint8_t select,uint8_t shake,uint8_t retry_count,int rssi,int snr,uint8_t button)
{
    if(get_sd_valid() == 0)
    {
        return RT_ERROR;
    }

    char write_ptr[64];
    rt_memset(write_ptr,0,sizeof(write_ptr));

    rt_sprintf(write_ptr, "%s %d %d %d %d %d %d %d\n",(select > 0)?"LORA":"PIPE",valve_status_get(),psi_status_get(),shake,retry_count,(shake > 0)?rssi:0,(shake > 0)?snr:0,button);

    return csv_write(write_ptr,strlen(write_ptr));
}
