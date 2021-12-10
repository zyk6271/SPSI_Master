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
#include "easyflash.h"

#define DBG_TAG "flashwork"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

char read_value_temp[64]={0};
uint32_t Flash_Get_IDNums(void)
{
    uint8_t read_len = 0;
    uint32_t read_value = 0;
    char *keybuf="id";
    memset(read_value_temp,0,64);
    read_len = ef_get_env_blob(keybuf, read_value_temp, 64, NULL);
    if(read_len>0)
    {
        read_value = atol(read_value_temp);
    }
    else
    {
        read_value = 0;
    }
    LOG_D("Reading Key %s value %ld \r\n", keybuf, read_value);//输出
    return read_value;
}
void Flash_IDNums_Change(uint32_t value)
{
    const char *keybuf="id";
    char *Temp_ValueBuf = rt_malloc(64);
    sprintf(Temp_ValueBuf, "%ld", value);
    ef_set_env(keybuf, Temp_ValueBuf);
    rt_free(Temp_ValueBuf);
    LOG_D("Writing %ld to key %s\r\n", value,keybuf);
}
