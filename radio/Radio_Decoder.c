/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-22     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <stdio.h>
#include "drv_spi.h"
#include <string.h>
#include "AX5043.h"
#include "Radio_Decoder.h"
#include "Radio_Encoder.h"
#include "Flashwork.h"
#include "pin_config.h"
#include "Radio_Drv.h"
#include "heart.h"
#include "led.h"

#define DBG_TAG "RF_DE"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern uint32_t Self_ID;
extern uint32_t Target_ID;

extern rf_info info_433;
extern rf_info info_4068;

uint8_t rssi_level_select(int rssi)
{
    uint8_t value;
    if(rssi > -68)
    {
        value = 5;
    }
    else if(rssi <= -68 && rssi > -81)
    {
        value = 4;
    }
    else if(rssi <= -81 && rssi > -90)
    {
        value = 3;
    }
    else if(rssi <= -90 && rssi > -95)
    {
        value = 2;
    }
    else if(rssi <= -95)
    {
        value = 1;
    }
    return value;
}
void Solve_433(int rssi,uint8_t *rx_buffer,uint8_t rx_len)
{
    Message Rx_message;
    if(rx_buffer[rx_len-1]=='S')
     {
         sscanf((const char *)&rx_buffer[1],"S{%ld,%ld,%d,%d}S",&Rx_message.Target_ID,&Rx_message.From_ID,&Rx_message.Command,&Rx_message.Data);
         if(Rx_message.Target_ID==Self_ID && Rx_message.From_ID==Target_ID)
         {
             switch(Rx_message.Command)
             {
             case 0://心跳
                 info_433.received = 1;
                 info_433.rssi = rssi;
                 info_433.rssi_level = rssi_level_select(rssi);
                 LOG_I("RF 433 heart is received\r\n");
                 led_rf433_start(info_433.rssi_level);
                 break;
             case 1://手动测试

                 //打开关闭PSI
                 break;
             }
         }
     }
}
void Solve_4068(int rssi,uint8_t *rx_buffer,uint8_t rx_len)
{
    Message Rx_message;
    if(rx_buffer[rx_len-1]=='S')
     {
         sscanf((const char *)&rx_buffer[1],"S{%ld,%ld,%d,%d}S",&Rx_message.Target_ID,&Rx_message.From_ID,&Rx_message.Command,&Rx_message.Data);
         if(Rx_message.Target_ID==Self_ID && Rx_message.From_ID==Target_ID)
         {
             switch(Rx_message.Command)
             {
             case 0://心跳
                 info_4068.received = 1;
                 info_4068.rssi = rssi;
                 info_4068.rssi_level = rssi_level_select(rssi);
                 LOG_I("RF 4068 heart is received\r\n");
                 led_rf4068_start(info_4068.rssi_level);
                 break;
             case 1://手动测试

                 //打开关闭PSI
                 break;
             }
         }
     }
}
void rf433_rx_callback(int rssi,uint8_t *rx_buffer,uint8_t rx_len)
{
    LOG_D("RX 433 is %s,RSSI is %d\r\n",rx_buffer,rssi);
    switch(rx_buffer[1])
    {
    case 'S':
        Solve_433(rssi,rx_buffer,rx_len-1);
        break;
    }
}
void rf4068_rx_callback(int rssi,uint8_t *rx_buffer,uint8_t rx_len)
{
    LOG_D("RX 4068 is %s,RSSI is %d\r\n",rx_buffer,rssi);
    switch(rx_buffer[1])
    {
    case 'S':
        Solve_4068(rssi,rx_buffer,rx_len-1);
        break;
    }
}
