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

#define DBG_TAG "file"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint32_t Global_Nums=0;
void ID_Init(void)
{
    Global_Nums = Flash_Get_IDNums();
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
void File_Output(void)
{
    ID_Increase();//序列增加
    char *buf = rt_malloc(64);
    sprintf(buf,"%d 0 %d 0 0 0 0 0 0\n",Global_Nums,Global_Nums%2);
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
MSH_CMD_EXPORT(File_Output,File_Output);
void ftest(void)
{
    for(uint32_t i=1;i<400000;i++)
    {
        File_Output();
    }
}
MSH_CMD_EXPORT(ftest,ftest);
