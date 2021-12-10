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

#define DBG_TAG "RF_DE"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

uint8_t Learn_Flag=1;
uint32_t Main_ID = 0;
extern uint32_t Self_Id;
extern int ubRssi;

void NormalSolve(uint8_t *rx_buffer,uint8_t rx_len)
{
    Message Rx_message;
    if(rx_buffer[rx_len-1]==0x0A&&rx_buffer[rx_len-2]==0x0D)
     {
         sscanf((const char *)&rx_buffer[1],"{%ld,%ld,%d,%d,%d}",&Rx_message.Target_ID,&Rx_message.From_ID,&Rx_message.Counter,&Rx_message.Command,&Rx_message.Data);
         if(Rx_message.Target_ID==Self_Id)
         {
             LOG_D("NormalSolve verify ok\r\n");
             switch(Rx_message.Command)
             {
             case 1://学习
                 break;
             }
         }
     }
}
void rf433_rx_callback(uint8_t *rx_buffer,uint8_t rx_len)
{
    LOG_I("RX 433 is %s\r\n",rx_buffer);
    switch(rx_buffer[1])
    {
    case 'A':
        NormalSolve(rx_buffer,rx_len);
        break;
    }
}
void rf4068_rx_callback(uint8_t *rx_buffer,uint8_t rx_len)
{
    LOG_I("RX 4068 is %s\r\n",rx_buffer);
    switch(rx_buffer[1])
    {
    case 'A':
        NormalSolve(rx_buffer,rx_len);
        break;
    }
}
