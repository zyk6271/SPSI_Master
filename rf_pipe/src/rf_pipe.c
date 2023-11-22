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
#include "pin_config.h"
#include "AX5043.h"
#include "rf_pipe_config.h"
#include "rf_pipe_drv.h"
#include "rf_pipe_port.h"

#define DBG_TAG "RF_PIPE"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static struct ax5043 rf_pipe;

rt_sem_t ax5043_irq_sem = RT_NULL;
rt_thread_t rf_pipe_task = RT_NULL;
rt_timer_t rf_pipe_send_timer = RT_NULL;

void AX5043_IRQ_Callback(void *parameter)
{
    rt_sem_release(ax5043_irq_sem);
}

struct ax5043_config *rf_pipe_config_init(void)
{
    static struct ax5043_config *config;
    config = rt_malloc(sizeof(struct ax5043_config));
    config->axradio_phy_vcocalib = 0;
    config->axradio_phy_preamble_len = 32;
    config->axradio_phy_preamble_byte = 0x55;
    config->axradio_phy_preamble_flags = 0x38;
    config->axradio_phy_preamble_appendbits = 0;
    config->axradio_phy_preamble_appendpattern = 0x00;
    config->axradio_phy_chanfreq[0] = 0x01900c9b;
    config->axradio_phy_chanvcoiinit[0] = 0x00;
    config->axradio_phy_chanpllrnginit[0] = 0xFF;
    config->axradio_phy_maxfreqoffset = 157;
    config->axradio_phy_rssireference = 0x3A;
    config->axradio_phy_rssioffset = 70;
    config->axradio_framing_synclen = 32;
    config->axradio_framing_syncflags = 0x38;
    config->axradio_framing_syncword[0] = 0xcc;
    config->axradio_framing_syncword[1] = 0xaa;
    config->axradio_framing_syncword[2] = 0xcc;
    config->axradio_framing_syncword[3] = 0xaa;
    return config;
}
void rf_pipe_driver_init(void)
{
    rf_pipe.config = rf_pipe_config_init();
    rf_pipe.socket = rf_pipe_radio_spi_init();
    strcpy(rf_pipe.name,"rf_pipe");
    memcpy(rf_pipe.RegValue,ax5043_registers,sizeof(ax5043_registers));

    rt_pin_mode(AX5043_IRQ, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(AX5043_IRQ, PIN_IRQ_MODE_RISING, AX5043_IRQ_Callback, RT_NULL);
    rt_pin_irq_enable(AX5043_IRQ, PIN_IRQ_ENABLE);

    rf_startup(&rf_pipe);

    vcoi_rng_get(&rf_pipe);
    AX5043ReceiverON(&rf_pipe);
}

void rf_pipe_send_timer_callback(void *parameter)
{
    if(rf_pipe.ubRFState != trxstate_rx)
    {
        LOG_E("rf_pipe Send timeout\r\n");
        SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_RADIOEVENTMASK0, 0x00);
        rf_restart(&rf_pipe);
        AX5043Receiver_Continuous(&rf_pipe);
    }
}

void rf_irq_clean(void)
{
    rt_sem_control(ax5043_irq_sem,RT_IPC_CMD_RESET,0);
}

void rf_pipe_send(uint8_t *buf,uint32_t size)
{
    rt_timer_start(rf_pipe_send_timer);
    RF_Send(&rf_pipe,buf,size);
    LOG_D("rf_pipe_send %s\r\n",buf);
}

void rf_pipe_task_callback(void *parameter)
{
    while(1)
    {
        static rt_err_t result;
        result = rt_sem_take(ax5043_irq_sem, RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            switch (rf_pipe.ubRFState)
            {
            case trxstate_rx: //0x01
                ReceiveData(&rf_pipe);
                if (rf_pipe.RxLen != 0)
                {
                    LOG_D("ReceiveData %s\r\n",buffer);
                }
                rf_pipe_rx_callback(rf_pipe.ubRssi,0,rf_pipe.RXBuff,rf_pipe.RxLen);
                AX5043Receiver_Continuous(&rf_pipe);
                break;
            case trxstate_wait_xtal:     //3
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_IRQMASK1, 0x00);//AX5043_IRQMASK1 = 0x00 otherwise crystal ready will fire all over again
                rf_pipe.ubRFState = trxstate_xtal_ready;
                break;
            case trxstate_pll_ranging:     //5
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_IRQMASK1, 0x00);//AX5043_IRQMASK1 = 0x00 otherwise autoranging done will fire all over again
                rf_pipe.ubRFState = trxstate_pll_ranging_done;
                break;
            case trxstate_pll_settling:     //7
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_RADIOEVENTMASK0, 0x00);// AX5043_RADIOEVENTMASK0 = 0x00;
                rf_pipe.ubRFState = trxstate_pll_settled;
                break;
            case trxstate_tx_xtalwait:    //9
                SpiReadSingleAddressRegister(&rf_pipe,REG_AX5043_RADIOEVENTREQ0); //make sure REVRDONE bit is cleared, so it is a reliable indicator that the packet is out
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFOSTAT, 0x03);
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_IRQMASK1, 0x00);
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_IRQMASK0, 0x08);// enable fifo free threshold
                rf_pipe.ubRFState = trxstate_tx_longpreamble;
                if ((SpiReadSingleAddressRegister(&rf_pipe,REG_AX5043_MODULATION) & 0x0F) == 9) { // 4-FSK
                    SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFODATA, (AX5043_FIFOCMD_DATA | (7 << 5)));
                    SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFODATA, 2);  // length (including flags)
                    SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFODATA, 0x01);  // flag PKTSTART -> dibit sync
                    SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFODATA, 0x11); // dummy byte for forcing dibit sync
                }
                TransmitData(&rf_pipe);
                break;
            case trxstate_tx_packet:
                if (rf_pipe.TxLen < 11)
                {
                    SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFOSTAT, 4); // commit
                }
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFODATA, AX5043_FIFOCMD_DATA | (7 << 5));//AX5043_FIFODATA = AX5043_FIFOCMD_DATA | (7 << 5);
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFODATA,rf_pipe.TxLen + 1);//write FIFO chunk length byte (data length+MAC_len + Flag byte=64+3+1)
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFODATA,3);//AX5043_FIFODATA = flags=0x03;
                SpiWriteData(&rf_pipe,rf_pipe.TXBuff,rf_pipe.TxLen);
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_FIFOSTAT, 4); // commit

                rf_pipe.ubRFState = trxstate_tx_waitdone;
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_RADIOEVENTMASK0, 0x01); // enable REVRDONE event
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_IRQMASK0, 0x40); // enable radio controller irq
                break;
            case trxstate_tx_waitdone:                 //D
                rt_timer_stop(rf_pipe_send_timer);
                rf_pipe_txdone_callback();
                SpiReadSingleAddressRegister(&rf_pipe,REG_AX5043_RADIOEVENTREQ0);        //clear Interrupt flag
                if (SpiReadSingleAddressRegister(&rf_pipe,REG_AX5043_RADIOSTATE) != 0)
                {
                    break;
                }
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_RADIOEVENTMASK0, 0x00);
                AX5043Receiver_Continuous(&rf_pipe);
                break;
            default:
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_IRQMASK1, 0x00);
                SpiWriteSingleAddressRegister(&rf_pipe,REG_AX5043_IRQMASK0, 0x00);
                break;
            }
        }
    }
}

void rf_pipe_init(void)
{
    ax5043_irq_sem = rt_sem_create("ax5043_irq_sem", 0, RT_IPC_FLAG_FIFO);
    rf_pipe_send_timer = rt_timer_create("rf_pipe_send timeout", rf_pipe_send_timer_callback, RT_NULL, 1000, RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    rf_pipe_task = rt_thread_create("rf_pipe_task", rf_pipe_task_callback, RT_NULL, 2048, 8, 10);
    rt_thread_startup(rf_pipe_task);
    rf_pipe_driver_init();
}
