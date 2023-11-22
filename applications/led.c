/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-23     Tobby       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "Pin_Config.h"
#include "signal_led.h"
#include "led.h"

#define DBG_TAG "LED"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static uint8_t beep_mode;

rt_thread_t led_thread = RT_NULL;

static led_t *LED1_G = RT_NULL;
static led_t *LED1_R = RT_NULL;
static led_t *LED2_G = RT_NULL;
static led_t *LED2_R = RT_NULL;
static led_t *LED3_G = RT_NULL;
static led_t *LED3_R = RT_NULL;
static led_t *LED4_G = RT_NULL;
static led_t *LED4_R = RT_NULL;
static led_t *LED5_G = RT_NULL;
static led_t *LED5_R = RT_NULL;
static led_t *LED6_G = RT_NULL;
static led_t *LED6_R = RT_NULL;
static led_t *LED7_G = RT_NULL;
static led_t *LED7_R = RT_NULL;
static led_t *LED8_G = RT_NULL;
static led_t *LED8_R = RT_NULL;
static led_t *LED8_B = RT_NULL;
static led_t *BEEP = RT_NULL;
static led_t *WARN_BEEP = RT_NULL;

//定义内存操作函数接口
led_mem_opreation_t led_mem_opreation;

/*************LED1***************/
static void LED1_G_ON(void *param)
{
    ws2812b_green(2, 1);
}

static void LED1_G_OFF(void *param)
{
    ws2812b_green(2, 0);
}

static void LED1_R_ON(void *param)
{
    ws2812b_red(2, 1);
}

static void LED1_R_OFF(void *param)
{
    ws2812b_red(2, 0);
}

/*************LED2***************/
static void LED2_G_ON(void *param)
{
    ws2812b_green(1, 1);
}

static void LED2_G_OFF(void *param)
{
    ws2812b_green(1, 0);
}

static void LED2_R_ON(void *param)
{
    ws2812b_red(1, 1);
}

static void LED2_R_OFF(void *param)
{
    ws2812b_red(1, 0);
}

/*************LED3***************/
static void LED3_G_ON(void *param)
{
    ws2812b_green(0, 1);
}
static void LED3_G_OFF(void *param)
{
    ws2812b_green(0, 0);
}
static void LED3_R_ON(void *param)
{
    ws2812b_red(0, 1);
}
static void LED3_R_OFF(void *param)
{
    ws2812b_red(0, 0);
}

/*************LED4***************/
static void LED4_G_ON(void *param)
{
    ws2812b_green(3, 1);
}
static void LED4_G_OFF(void *param)
{
    ws2812b_green(3, 0);
}
static void LED4_R_ON(void *param)
{
    ws2812b_red(3, 1);
}
static void LED4_R_OFF(void *param)
{
    ws2812b_red(3, 0);
}

/*************LED5***************/
static void LED5_G_ON(void *param)
{
    ws2812b_green(4, 1);
}

static void LED5_G_OFF(void *param)
{
    ws2812b_green(4, 0);
}

static void LED5_R_ON(void *param)
{
    ws2812b_red(4, 1);
}

static void LED5_R_OFF(void *param)
{
    ws2812b_red(4, 0);
}

/*************LED6***************/
static void LED6_G_ON(void *param)
{
    ws2812b_green(5, 1);
}
static void LED6_G_OFF(void *param)
{
    ws2812b_green(5, 0);
}
static void LED6_R_ON(void *param)
{
    ws2812b_red(5, 1);
}
static void LED6_R_OFF(void *param)
{
    ws2812b_red(5, 0);
}

/*************LED7***************/
static void LED7_G_ON(void *param)
{
    ws2812b_green(6, 1);
}

static void LED7_G_OFF(void *param)
{
    ws2812b_green(6, 0);
}

static void LED7_R_ON(void *param)
{
    ws2812b_red(6, 1);
}

static void LED7_R_OFF(void *param)
{
    ws2812b_red(6, 0);
}

/*************LED8***************/
static void LED8_G_ON(void *param)
{
    ws2812b_green(7, 1);
}

static void LED8_G_OFF(void *param)
{
    ws2812b_green(7, 0);
}

static void LED8_R_ON(void *param)
{
    ws2812b_red(7, 1);
}

static void LED8_R_OFF(void *param)
{
    ws2812b_red(7, 0);
}

static void LED8_B_ON(void *param)
{
    ws2812b_blue(7, 1);
}

static void LED8_B_OFF(void *param)
{
    ws2812b_blue(7, 0);
}

static void BEEP_ON(void *param)
{
    rt_pin_mode(BUZZER, PIN_MODE_OUTPUT);
    rt_pin_write(BUZZER, PIN_HIGH);
}

static void BEEP_OFF(void *param)
{
    rt_pin_mode(BUZZER, PIN_MODE_OUTPUT);
    rt_pin_write(BUZZER, PIN_LOW);
}

static void led_run(void *parameter)
{
    led_transmitter_off();
    ws2812b_init();
    while (1)
    {
        rt_thread_mdelay(LED_TICK_TIME);
        led_ticks();
        RGB_SendArray();
    }
}

void led_Init(void)
{
    led_mem_opreation.malloc_fn = (void* (*)(size_t)) rt_malloc;
    led_mem_opreation.free_fn = rt_free;
    led_set_mem_operation(&led_mem_opreation);

    LED1_G = led_create(LED1_G_ON, LED1_G_OFF, NULL);
    LED1_R = led_create(LED1_R_ON, LED1_R_OFF, NULL);

    LED2_G = led_create(LED2_G_ON, LED2_G_OFF, NULL);
    LED2_R = led_create(LED2_R_ON, LED2_R_OFF, NULL);

    LED3_G = led_create(LED3_G_ON, LED3_G_OFF, NULL);
    LED3_R = led_create(LED3_R_ON, LED3_R_OFF, NULL);

    LED4_G = led_create(LED4_G_ON, LED4_G_OFF, NULL);
    LED4_R = led_create(LED4_R_ON, LED4_R_OFF, NULL);

    LED5_G = led_create(LED5_G_ON, LED5_G_OFF, NULL);
    LED5_R = led_create(LED5_R_ON, LED5_R_OFF, NULL);

    LED6_G = led_create(LED6_G_ON, LED6_G_OFF, NULL);
    LED6_R = led_create(LED6_R_ON, LED6_R_OFF, NULL);

    LED7_G = led_create(LED7_G_ON, LED7_G_OFF, NULL);
    LED7_R = led_create(LED7_R_ON, LED7_R_OFF, NULL);

    LED8_G = led_create(LED8_G_ON, LED8_G_OFF, NULL);
    LED8_R = led_create(LED8_R_ON, LED8_R_OFF, NULL);
    LED8_B = led_create(LED8_B_ON, LED8_B_OFF, NULL);

    BEEP = led_create(BEEP_ON, BEEP_OFF, NULL);
    WARN_BEEP = led_create(BEEP_ON, BEEP_OFF, NULL);

    led_thread = rt_thread_create("led_thread", led_run, RT_NULL, 512, 15, 10);
    rt_thread_startup(led_thread);
}

void led_rf_lora_signal_lost_callback(void)
{
    led_set_mode(LED1_R, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED2_R, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED3_R, LOOP_PERMANENT, "1,0,");
    led_start(LED1_R);
    led_start(LED2_R);
    led_start(LED3_R);
}

void led_rf_lora_signal_lost(void)
{
    led_set_mode(LED1_R, 1, "150,150,");
    led_set_mode(LED2_R, 1, "150,150,");
    led_set_mode(LED3_R, 1, "150,150,");
    led_stop(LED1_G);
    led_stop(LED2_G);
    led_stop(LED3_G);
    led_start(LED1_R);
    led_start(LED2_R);
    led_start(LED3_R);
    led_set_blink_over_callback(LED3_R, led_rf_lora_signal_lost_callback);
}

void led_rf_lora_signal_ultralow_callback(void)
{
    led_set_mode(LED1_R, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED1_G, LOOP_PERMANENT, "1,0,");
    led_start(LED1_G);
    led_start(LED1_R);
}

void led_rf_lora_signal_ultralow(void)
{
    led_set_mode(LED1_R, 1, "150,150,");
    led_set_mode(LED1_G, 1, "150,150,");
    led_stop(LED2_R);
    led_stop(LED2_G);
    led_stop(LED3_R);
    led_stop(LED3_G);
    led_start(LED1_G);
    led_start(LED1_R);
    led_set_blink_over_callback(LED1_R, led_rf_lora_signal_ultralow_callback);
}

void led_rf_lora_signal_low_callback(void)
{
    led_set_mode(LED1_G, LOOP_PERMANENT, "1,0,");
    led_start(LED1_G);
}

void led_rf_lora_signal_low(void)
{
    led_set_mode(LED1_G, 1, "150,150,");
    led_stop(LED1_R);
    led_stop(LED2_R);
    led_stop(LED2_G);
    led_stop(LED3_R);
    led_stop(LED3_G);
    led_start(LED1_G);
    led_set_blink_over_callback(LED1_G, led_rf_lora_signal_low_callback);
}

void led_rf_lora_signal_mid_callback(void)
{
    led_set_mode(LED1_G, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED2_G, LOOP_PERMANENT, "1,0,");
    led_start(LED1_G);
    led_start(LED2_G);
}

void led_rf_lora_signal_mid(void)
{
    led_set_mode(LED1_G, 1, "150,150,");
    led_set_mode(LED2_G, 1, "150,150,");
    led_stop(LED1_R);
    led_stop(LED2_R);
    led_stop(LED3_R);
    led_stop(LED3_G);
    led_start(LED1_G);
    led_start(LED2_G);

    led_set_blink_over_callback(LED2_G, led_rf_lora_signal_mid_callback);
}

void led_rf_lora_signal_high_callback(void)
{
    led_set_mode(LED1_G, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED2_G, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED3_G, LOOP_PERMANENT, "1,0,");
    led_start(LED1_G);
    led_start(LED2_G);
    led_start(LED3_G);
}

void led_rf_lora_signal_high(void)
{
    led_set_mode(LED1_G, 1, "150,150,");
    led_set_mode(LED2_G, 1, "150,150,");
    led_set_mode(LED3_G, 1, "150,150,");
    led_stop(LED1_R);
    led_stop(LED2_R);
    led_stop(LED3_R);
    led_start(LED1_G);
    led_start(LED2_G);
    led_start(LED3_G);
    led_set_blink_over_callback(LED3_G, led_rf_lora_signal_high_callback);

}

void led_rf_lora_signal_start(uint8_t mode)
{
    switch (mode)
    {
    case 1:
        led_rf_lora_signal_lost();
        break;
    case 2:
        led_rf_lora_signal_ultralow();
        break;
    case 3:
        led_rf_lora_signal_low();
        break;
    case 4:
        led_rf_lora_signal_mid();
        break;
    case 5:
        led_rf_lora_signal_high();
        break;
    }
}

void led_rf_lora_signal_stop(void)
{
    led_stop(LED1_R);
    led_stop(LED1_G);
    led_stop(LED2_R);
    led_stop(LED2_G);
    led_stop(LED3_R);
    led_stop(LED3_G);
}

void led_rf_pipe_signal_lost_callback(void)
{
    led_set_mode(LED4_R, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED5_R, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED6_R, LOOP_PERMANENT, "1,0,");
    led_start(LED4_R);
    led_start(LED5_R);
    led_start(LED6_R);
}

void led_rf_pipe_signal_lost(void)
{
    led_set_mode(LED4_R, 1, "150,150,");
    led_set_mode(LED5_R, 1, "150,150,");
    led_set_mode(LED6_R, 1, "150,150,");
    led_stop(LED4_G);
    led_stop(LED5_G);
    led_stop(LED6_G);
    led_start(LED4_R);
    led_start(LED5_R);
    led_start(LED6_R);
    led_set_blink_over_callback(LED6_R, led_rf_pipe_signal_lost_callback);
}

void led_rf_pipe_signal_ultralow_callback(void)
{
    led_set_mode(LED4_R, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED4_G, LOOP_PERMANENT, "1,0,");
    led_start(LED4_R);
    led_start(LED4_G);
}

void led_rf_pipe_signal_ultralow(void)
{
    led_set_mode(LED4_R, 1, "150,150,");
    led_set_mode(LED4_G, 1, "150,150,");
    led_stop(LED5_R);
    led_stop(LED5_G);
    led_stop(LED6_R);
    led_stop(LED6_G);
    led_start(LED4_R);
    led_start(LED4_G);
    led_set_blink_over_callback(LED4_G, led_rf_pipe_signal_ultralow_callback);
}

void led_rf_pipe_signal_low_callback(void)
{
    led_set_mode(LED4_G, LOOP_PERMANENT, "1,0,");
    led_start(LED4_G);
}

void led_rf_pipe_signal_low(void)
{
    led_set_mode(LED4_G, 1, "150,150,");
    led_stop(LED4_R);
    led_stop(LED5_R);
    led_stop(LED5_G);
    led_stop(LED6_R);
    led_stop(LED6_G);
    led_start(LED4_G);
    led_set_blink_over_callback(LED4_G, led_rf_pipe_signal_low_callback);
}

void led_rf_pipe_signal_mid_callback(void)
{
    led_set_mode(LED4_G, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED5_G, LOOP_PERMANENT, "1,0,");
    led_start(LED4_G);
    led_start(LED5_G);
}

void led_rf_pipe_signal_mid(void)
{
    led_set_mode(LED4_G, 1, "150,150,");
    led_set_mode(LED5_G, 1, "150,150,");
    led_stop(LED4_R);
    led_stop(LED5_R);
    led_stop(LED6_R);
    led_stop(LED6_G);
    led_start(LED4_G);
    led_start(LED5_G);
    led_set_blink_over_callback(LED5_G, led_rf_pipe_signal_mid_callback);
}

void led_rf_pipe_signal_high_callback(void)
{
    led_set_mode(LED4_G, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED5_G, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED6_G, LOOP_PERMANENT, "1,0,");
    led_start(LED4_G);
    led_start(LED5_G);
    led_start(LED6_G);
}

void led_rf_pipe_signal_high(void)
{
    led_set_mode(LED4_G, 1, "150,150,");
    led_set_mode(LED5_G, 1, "150,150,");
    led_set_mode(LED6_G, 1, "150,150,");
    led_stop(LED4_R);
    led_stop(LED5_R);
    led_stop(LED6_R);
    led_start(LED4_G);
    led_start(LED5_G);
    led_start(LED6_G);
    led_set_blink_over_callback(LED6_G, led_rf_pipe_signal_high_callback);
}

void led_rf_pipe_signal_start(uint8_t mode)
{
    switch (mode)
    {
    case 1:
        led_rf_pipe_signal_lost();
        break;
    case 2:
        led_rf_pipe_signal_ultralow();
        break;
    case 3:
        led_rf_pipe_signal_low();
        break;
    case 4:
        led_rf_pipe_signal_mid();
        break;
    case 5:
        led_rf_pipe_signal_high();
        break;
    }
}

void led_rf_pipe_signal_stop(void)
{
    led_stop(LED4_R);
    led_stop(LED4_G);
    led_stop(LED5_R);
    led_stop(LED5_G);
    led_stop(LED6_R);
    led_stop(LED6_G);
}

void led_transmitter_lost(void)
{
    led_stop(LED7_G);
    led_set_mode(LED7_R, LOOP_PERMANENT, "1,0,");
    led_start(LED7_R);
    led_stop(WARN_BEEP);
}

void led_transmitter_off(void)
{
    led_set_mode(LED7_R, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED7_G, LOOP_PERMANENT, "1,0,");
    led_start(LED7_G);
    led_start(LED7_R);
}

void led_transmitter_warning(void)
{
    led_stop(LED7_G);
    led_set_mode(LED7_R, LOOP_PERMANENT,
            "50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50\
                                    ,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,2000");
    led_set_mode(WARN_BEEP, LOOP_PERMANENT,
            "50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50\
                                    ,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,2000");
    led_start(LED7_R);
    led_start(WARN_BEEP);
}

void led_transmitter_on(void)
{
    led_stop(LED7_R);
    led_set_mode(LED7_G, LOOP_PERMANENT, "1,0,");
    led_start(LED7_G);
    led_stop(WARN_BEEP);
}

void led_receiver_lost(void)
{
    led_stop(LED8_G);
    led_set_mode(LED8_R, LOOP_PERMANENT, "1,0,");
    led_start(LED8_R);
}

void led_receiver_close(void)
{
    led_stop(LED8_G);
    led_stop(LED8_R);
}

void led_receiver_blink(void)
{
    led_stop(LED8_B);
    led_set_mode(LED8_B, 1, "100,100,");
    led_start(LED8_B);
}

void led_receiver_off(void)
{
    led_set_mode(LED8_R, LOOP_PERMANENT, "1,0,");
    led_set_mode(LED8_G, LOOP_PERMANENT, "1,0,");
    led_start(LED8_G);
    led_start(LED8_R);
}

void led_receiver_on(void)
{
    led_stop(LED8_R);
    led_set_mode(LED8_G, LOOP_PERMANENT, "1,0,");
    led_start(LED8_G);
}

void beep_calc(uint8_t pipe_signal_level, uint8_t lora_signal_level)
{
    if (pipe_signal_level == 2 || pipe_signal_level == 2)
    {
        beep_start(1);
        return;
    }
    else if (pipe_signal_level >= 2 && pipe_signal_level >= 2)
    {
        beep_stop();
    }
}

void beep_start(uint8_t select)
{
    if (select != beep_mode)
    {
        beep_mode = select;
        led_stop(BEEP);
        switch (select)
        {
        case 1: //10s/1声
            led_set_mode(BEEP, LOOP_PERMANENT, "200,10000,");
            break;
        case 2: //5s/2声
            led_set_mode(BEEP, LOOP_PERMANENT, "200,200,200,5000,");
            break;
        case 3: //5s/3声
            led_set_mode(BEEP, LOOP_PERMANENT, "200,200,200,200,200,5000,");
            break;
        case 4: //5s/5声
            led_set_mode(BEEP, 1, "200,200,200,200,200,200,200,200,200,200,");
            break;
        }
        led_start(BEEP);
    }
}

void beep_stop(void)
{
    beep_mode = 0;
    led_stop(BEEP);
}

