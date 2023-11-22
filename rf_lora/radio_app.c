/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-13     Rick       the first version
 */
#include "rtthread.h"
#include "radio.h"
#include "radio_app.h"

#define DBG_TAG "RADIO_APP"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static RadioEvents_t RadioEvents;

static void OnTxDone(void)
{
    LOG_D("OnTxDone\r\n");
    rf_lora_txdone_callback();

    Radio.SetMaxPayloadLength(MODEM_LORA, 255);
    Radio.Rx(0);
}

static void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    LOG_D("rssi %d,cfo %d,recv Size %d,recv payload is %s\r\n",rssi,snr,size,payload);
    rf_lora_rx_callback(rssi,snr,payload,size);
}

static void OnTxTimeout(void)
{
    LOG_W("OnTxTimeout\r\n");
}

static void OnRxTimeout(void)
{
    LOG_W("OnRxTimeout\r\n");
}

static void OnRxError(void)
{
    LOG_W("OnRxError\r\n");
}

void rf_lora_init(void)
{
    rt_memset(&RadioEvents,0,sizeof(RadioEvents_t));

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

    Radio.Init(&RadioEvents);

    Radio.SetChannel(RF_FREQUENCY);

    Radio.SetTxConfig( MODEM_LORA, LORA_TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON_DISABLE,
                                   true, 0, 0, LORA_IQ_INVERSION_ON_DISABLE, 5000 );

    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON_DISABLE,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON_DISABLE, true );

    Radio.SetMaxPayloadLength(MODEM_LORA, 255);
    Radio.Rx(0);
}

void rf_lora_send(uint8_t *buf,uint32_t size)
{
    Radio.Send(buf,size);
    LOG_D("rf_lora_send %s\r\n",buf);
}
