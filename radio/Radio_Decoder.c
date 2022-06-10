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
#include "seg.h"
#include "warn.h"

#define DBG_TAG "RF_DE"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern uint32_t Self_ID;
extern uint32_t Target_ID;

extern rf_info info_433;
extern rf_info info_4068;

extern uint8_t PSI_Status;

extern struct rt_event rf_event;

uint8_t rf4068_rssi_level_select(int rssi)
{
    uint8_t value;
    if(rssi > -68)
    {
        value = 5;
        rf4068_rssi_count_resume();
        rf4068_alive_count_resume();
    }
    else if(rssi <= -68 && rssi > -81)
    {
        value = 4;
        rf4068_rssi_count_resume();
        rf4068_alive_count_resume();
    }
    else if(rssi <= -81 && rssi > -90)
    {
        value = 3;
        rf4068_rssi_count_resume();
        rf4068_alive_count_resume();
    }
    else if(rssi <= -90)
    {
        value = 2;
        rf4068_rssi_count_increase();
        rf4068_alive_count_resume();
    }
    return value;
}
uint8_t rf433_rssi_level_select(int rssi)
{
    uint8_t value;
    if(rssi > -68)
    {
        value = 5;
        rf433_rssi_count_resume();
        rf433_alive_count_resume();
    }
    else if(rssi <= -68 && rssi > -81)
    {
        value = 4;
        rf433_rssi_count_resume();
        rf433_alive_count_resume();
    }
    else if(rssi <= -81 && rssi > -90)
    {
        value = 3;
        rf433_rssi_count_resume();
        rf433_alive_count_resume();
    }
    else if(rssi <= -90)
    {
        value = 2;
        rf433_rssi_count_increase();
        rf433_alive_count_resume();
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
             rt_event_send(&rf_event, RF433_Online);
             info_433.received = 1;
             info_433.rssi = rssi;
             info_433.rssi_level = rf433_rssi_level_select(rssi);
             switch(Rx_message.Command)
             {
             case 0://心跳
                 LOG_I("RF 433 heart is received\r\n");
                 valve_control(Rx_message.Data);
                 break;
             case 1://按钮
                 info_433.testreceived = 1;
                 LOG_I("RF 433 heart is received\r\n");
                 receiver_blink(Rx_message.Data);
                 valve_control(Rx_message.Data);
                 break;
             case 2://手动测试
                 LOG_I("RF 433 PSI Control is received\r\n");
                 valve_control(Rx_message.Data);
                 rf_write(1,rssi);
                 break;
             }
             led_rf433_start(info_433.rssi_level);
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
             rt_event_send(&rf_event, RF4068_Online);
             info_4068.received = 1;
             info_4068.rssi = rssi;
             info_4068.rssi_level = rf4068_rssi_level_select(rssi);
             switch(Rx_message.Command)
             {
             case 0://心跳
                 LOG_I("RF 4068 heart is received\r\n");
                 valve_control(Rx_message.Data);
                 break;
             case 1://按钮
                 info_4068.testreceived = 1;
                 LOG_I("RF 4068 heart is received\r\n");
                 receiver_blink(Rx_message.Data);
                 valve_control(Rx_message.Data);
                 break;
             case 2://手动测试
                 LOG_I("RF 4068 PSI Control is received\r\n");
                 valve_control(Rx_message.Data);
                 rf_write(0,rssi);
                 break;
             }
             led_rf4068_start(info_4068.rssi_level);
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
