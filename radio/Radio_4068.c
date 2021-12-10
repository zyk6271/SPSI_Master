/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-01     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_spi.h"
#include <string.h>
#include "AX5043.h"
#include "Radio_Common.h"
#include "pin_config.h"
#include "Radio_Decoder.h"
#include "Radio_Encoder.h"
#include "Radio_Drv.h"
#include "Config_4068.h"

#define DBG_TAG "radio_4068"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

rt_sem_t IRQ2_Sem=RT_NULL;
rt_thread_t rf_4068_task=RT_NULL;
rt_timer_t rf_4068_send_timer = RT_NULL;

void IRQ2_ISR(void *parameter)
{
    rt_sem_release(IRQ2_Sem);
}
void IRQ2_Bounding(void)
{
    rt_pin_mode(IRQ2, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(IRQ2, PIN_IRQ_MODE_RISING, IRQ2_ISR, RT_NULL);
    rt_pin_irq_enable(IRQ2, PIN_IRQ_ENABLE);
}
struct ax5043_config *rf_4068_config_init(void)
{
    static struct ax5043_config *config;
    config = rt_malloc(sizeof(struct ax5043_config));
    config->axradio_phy_vcocalib = 0;
    config->axradio_framing_synclen = 32;
    config->axradio_phy_preamble_byte = 0x55;
    config->axradio_phy_preamble_flags = 0x38;
    config->axradio_phy_preamble_len = 32;
    config->axradio_phy_chanfreq[0] = 0x0190716d;
    config->axradio_phy_chanvcoiinit[0] = 0x00;
    config->axradio_phy_chanpllrnginit[0] = 0xFF;
    config->axradio_phy_maxfreqoffset = 157;
    return config;
}
void rf_4068_sem_init(void)
{
    IRQ2_Sem = rt_sem_create("IRQ2_Sem", 0, RT_IPC_FLAG_FIFO);
}
void rf_4068_Init(void)
{
    rf_4068.config = rf_4068_config_init();
    rf_4068.socket = rf_4068_radio_spi_init();
    strcpy(rf_4068.name,"rf_4068");
    memcpy(rf_4068.RegValue,set_registers_4068,sizeof(set_registers_4068));
    memcpy(rf_4068.TXRegValue,set_registers_tx_4068,sizeof(set_registers_tx_4068));
    memcpy(rf_4068.RXRegValue,set_registers_rx_4068,sizeof(set_registers_rx_4068));
    IRQ2_Bounding();
    rf_startup(&rf_4068);
    vcoi_rng_get(&rf_4068);
    Ax5043SetRegisters_RX(&rf_4068);
    AX5043ReceiverON(&rf_4068);
}
void rf_4068_send_timer_callback(void *parameter)
{
    if(rf_4068.ubRFState == trxstate_tx_waitdone)
    {
        LOG_W("rf_4068 Send timeout\r\n");
        SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_PWRMODE, AX5043_PWRSTATE_XTAL_ON);    //AX5043_PWRMODE = AX5043_PWRSTATE_XTAL_ON=0x00;
        SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_RADIOEVENTMASK0, 0x00);
        Ax5043SetRegisters_RX(&rf_4068);
        AX5043Receiver_Continuous(&rf_4068);
    }
}
void rf_4068_task_callback(void *parameter)
{
    while(1)
    {
        static rt_err_t result;
        result = rt_sem_take(IRQ2_Sem, RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            switch (rf_4068.ubRFState)
            {
            case trxstate_rx: //0x01
                ReceiveData(&rf_4068);
                AX5043Receiver_Continuous(&rf_4068);
                if (rf_4068.RxLen != 0)
                {
                    rf4068_rx_callback(rf_4068.RXBuff,rf_4068.RxLen);
                }
                break;
            case trxstate_wait_xtal:     //3
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_IRQMASK1, 0x00);//AX5043_IRQMASK1 = 0x00 otherwise crystal ready will fire all over again
                rf_4068.ubRFState = trxstate_xtal_ready;
                break;
            case trxstate_pll_ranging:     //5
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_IRQMASK1, 0x00);//AX5043_IRQMASK1 = 0x00 otherwise autoranging done will fire all over again
                rf_4068.ubRFState = trxstate_pll_ranging_done;
                break;
            case trxstate_pll_settling:     //7
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_RADIOEVENTMASK0, 0x00);// AX5043_RADIOEVENTMASK0 = 0x00;
                rf_4068.ubRFState = trxstate_pll_settled;
                break;
            case trxstate_tx_xtalwait:    //9
                SpiReadSingleAddressRegister(&rf_4068,REG_AX5043_RADIOEVENTREQ0); //make sure REVRDONE bit is cleared, so it is a reliable indicator that the packet is out
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_FIFOSTAT, 0x03);
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_IRQMASK1, 0x00);
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_IRQMASK0, 0x08);// enable fifo free threshold
                rf_4068.ubRFState = trxstate_tx_longpreamble;
                if ((SpiReadSingleAddressRegister(&rf_4068,REG_AX5043_MODULATION) & 0x0F) == 9) { // 4-FSK
                    SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_FIFODATA, (AX5043_FIFOCMD_DATA | (7 << 5)));
                    SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_FIFODATA, 2);  // length (including flags)
                    SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_FIFODATA, 0x01);  // flag PKTSTART -> dibit sync
                    SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_FIFODATA, 0x11); // dummy byte for forcing dibit sync
                }
                TransmitData(&rf_4068);
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_PWRMODE, AX5043_PWRSTATE_FULL_TX); //AX5043_PWRMODE = AX5043_PWRSTATE_FULL_TX;
                rt_timer_start(rf_4068_send_timer);
                break;
            case trxstate_tx_longpreamble:
            case trxstate_tx_shortpreamble:
            case trxstate_tx_packet:
                TransmitData(&rf_4068);
                break;
            case trxstate_tx_waitdone:                 //D
                SpiReadSingleAddressRegister(&rf_4068,REG_AX5043_RADIOEVENTREQ0);        //clear Interrupt flag
                if (SpiReadSingleAddressRegister(&rf_4068,REG_AX5043_RADIOSTATE) != 0)
                {
                    break;
                }
                rt_timer_stop(rf_4068_send_timer);
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_RADIOEVENTMASK0, 0x00);
                Ax5043SetRegisters_RX(&rf_4068);
                AX5043Receiver_Continuous(&rf_4068);
                break;
            default:
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_IRQMASK1, 0x00);
                SpiWriteSingleAddressRegister(&rf_4068,REG_AX5043_IRQMASK0, 0x00);
                break;
            }
        }
    }
}
void rf_4068_start(void)
{
    rf_4068_sem_init();
    rf_4068_task = rt_thread_create("rf_4068_task", rf_4068_task_callback, RT_NULL, 2048, 10, 10);
    rt_thread_startup(rf_4068_task);
    rf_4068_send_timer = rt_timer_create("rf_4068_send timeout", rf_4068_send_timer_callback, RT_NULL, 120, RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    rf_4068_Init();
    LOG_I("Radio 4068 Init success\r\n");
}
MSH_CMD_EXPORT(rf_4068_start,rf_4068_start);
uint8_t buf1[]={0x31,0x31,0x32,0x32,0x33,0x33,0x34,0x34,0x35,0x35,0x36,0x36,0x37,0x37};
void sendtest1(void)
{
    Normal_send(&rf_4068,buf1,14);
}
MSH_CMD_EXPORT(sendtest1,sendtest1);
