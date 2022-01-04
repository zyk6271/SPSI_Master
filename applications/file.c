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
#include "stdint.h"
#include "file.h"
#include "flashwork.h"
#include "finsh.h"

#define DBG_TAG "file"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint32_t Global_Nums=0;
void ID_Init(void)
{
    Global_Nums = Flash_Get_IDNums();
    if(Flash_Get_Boot()==0)
    {
        Flash_Boot_Change(1);
        delete_file_1();
    }
    LOG_I("Init ID is %ld\r\n",Global_Nums);
}
uint32_t ID_Get(void)
{
    return Global_Nums;
}
void ID_Increase(void)
{
    Global_Nums ++;
    Flash_IDNums_Change(Global_Nums);
}
uint8_t select_file(uint32_t num)
{
    if((num%400000)==0)
    {
        delete_file_1();
    }
    else if((num%400000)==200000)
    {
        delete_file_2();
    }
    if((num%400000)<200000)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
void File_Output(uint8_t freq,uint8_t valve,uint8_t psi,uint8_t shake,uint8_t send_num,int rssi,uint8_t first,uint8_t button)
{
    ID_Increase();//序列增加
    char *buf = rt_malloc(64);
    sprintf(buf,"%d %d %d %d %d %d %d %d %d\n",Global_Nums,(freq>0)?433:4068,valve,psi,shake,send_num,rssi,first,button);
    if(select_file(Global_Nums)==0)
    {
        write_file_1(buf,strlen(buf));
    }
    else
    {
        write_file_2(buf,strlen(buf));
    }
    LOG_D("%s\r\n",buf);
    rt_free(buf);
}
