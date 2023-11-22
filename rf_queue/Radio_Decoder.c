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
#include "pin_config.h"
#include "heart.h"
#include "led.h"
#include "seg.h"
#include "warn.h"

#define DBG_TAG "RF_DE"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern rf_status rf_pipe_status;
extern rf_status rf_lora_status;

uint8_t rf_pipe_rssi_level_select(int rssi)
{
    uint8_t value;
    if(rssi > -68)
    {
        value = 5;
        rf_pipe_rssi_count_resume();
        rf_pipe_alive_count_resume();
    }
    else if(rssi <= -68 && rssi > -81)
    {
        value = 4;
        rf_pipe_rssi_count_resume();
        rf_pipe_alive_count_resume();
    }
    else if(rssi <= -81 && rssi > -90)
    {
        value = 3;
        rf_pipe_rssi_count_resume();
        rf_pipe_alive_count_resume();
    }
    else if(rssi <= -90)
    {
        value = 2;
        rf_pipe_rssi_count_increase();
        rf_pipe_alive_count_resume();
    }
    return value;
}
uint8_t rf_lora_rssi_level_select(int rssi)
{
    uint8_t value;
    if(rssi > -68)
    {
        value = 5;
        rf_lora_rssi_count_resume();
        rf_lora_alive_count_resume();
    }
    else if(rssi <= -68 && rssi > -81)
    {
        value = 4;
        rf_lora_rssi_count_resume();
        rf_lora_alive_count_resume();
    }
    else if(rssi <= -81 && rssi > -90)
    {
        value = 3;
        rf_lora_rssi_count_resume();
        rf_lora_alive_count_resume();
    }
    else if(rssi <= -90)
    {
        value = 2;
        rf_lora_rssi_count_increase();
        rf_lora_alive_count_resume();
    }
    return value;
}

void rf_lora_decoder(int rssi,int snr,uint8_t *rx_buffer,uint8_t rx_len)
{
    Radio_Frame_Format *recv_buffer = rx_buffer;
    uint8_t target_id_check = rt_memcmp(recv_buffer->target_id, rf_device_id_get(), 4);
    uint8_t device_id_check = rt_memcmp(recv_buffer->device_id, rf_target_id_get(), 4);
    if(target_id_check == 0 && device_id_check == 0)
    {
        rf_lora_status.alive = 1;
        rf_lora_status.startup = 1;
        rf_lora_status.received = 1;
        rf_lora_status.rssi = rssi;
        rf_lora_status.snr = snr;
        rf_lora_status.rssi_level = rf_lora_rssi_level_select(rssi);
        rf_online(rf_lora_status);
        led_rf_lora_signal_start(rf_lora_status.rssi_level);
        led_receiver_blink();
        switch(recv_buffer->control)
        {
            case 0://心跳
                LOG_I("RF LORA heart is received\r\n");
                valve_control(recv_buffer->value);
                break;
            case 1://按钮
                rf_lora_status.test_received = 1;
                LOG_I("RF LORA test is received\r\n");
                valve_control(recv_buffer->value);
                break;
            case 2://remote
                LOG_I("RF LORA remote control is received\r\n");
                valve_control(recv_buffer->value);
                heart_record_write(1,rf_lora_status.received,rf_lora_status.retry,rf_lora_status.rssi,rf_lora_status.snr,0);
                break;
        }
    }
}

void rf_pipe_decoder(int rssi,int snr,uint8_t *rx_buffer,uint8_t rx_len)
{
    Radio_Frame_Format *recv_buffer = &rx_buffer[1];
    uint8_t target_id_check = rt_memcmp(recv_buffer->target_id, rf_device_id_get(), 4);
    uint8_t device_id_check = rt_memcmp(recv_buffer->device_id, rf_target_id_get(), 4);
    if(target_id_check == 0 && device_id_check == 0)
    {
        rf_pipe_status.alive = 1;
        rf_pipe_status.startup = 1;
        rf_pipe_status.received = 1;
        if(rssi > -26)
        {
            rf_pipe_status.rssi = -26;
        }
        else
        {
            rf_pipe_status.rssi = rssi;
        }
        rf_pipe_status.snr = snr;
        rf_pipe_status.rssi_level = rf_pipe_rssi_level_select(rssi);
        rf_online(rf_pipe_status);
        led_rf_pipe_signal_start(rf_pipe_status.rssi_level);
        led_receiver_blink();
        switch(recv_buffer->control)
        {
            case 0://心跳
                LOG_I("RF PIPE heart is received\r\n");
                valve_control(recv_buffer->value);
                break;
            case 1://按钮
                rf_pipe_status.test_received = 1;
                LOG_I("RF PIPE test is received\r\n");
                valve_control(recv_buffer->value);
                break;
            case 2://remote
                LOG_I("RF PIPE remote control is received\r\n");
                valve_control(recv_buffer->value);
                heart_record_write(0,rf_pipe_status.received,rf_pipe_status.retry,rf_pipe_status.rssi,rf_pipe_status.snr,0);
                break;
        }
    }
}

void rf_pipe_rx_callback(int rssi,int snr,uint8_t *rx_buffer,uint8_t rx_len)
{
    LOG_D("RF PIPE is %s,RSSI is %d\r\n",rx_buffer,rssi);
    rf_pipe_decoder(rssi,snr,rx_buffer,rx_len);
}

void rf_lora_rx_callback(int rssi,int snr,uint8_t *rx_buffer,uint8_t rx_len)
{
    LOG_D("RF LORA is %s,RSSI is %d\r\n",rx_buffer,rssi);
    rf_lora_decoder(rssi,snr,rx_buffer,rx_len);
}
