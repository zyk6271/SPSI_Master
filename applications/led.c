/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-22     Rick       the first version
 */
#include <rtthread.h>
#include <agile_led.h>
#include <stdlib.h>
#include "Pin_Config.h"
#include "led.h"

#define DBG_TAG "LED"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static agile_led_t *LED1_G = RT_NULL;
static agile_led_t *LED1_R = RT_NULL;
static agile_led_t *LED2_G = RT_NULL;
static agile_led_t *LED2_R = RT_NULL;
static agile_led_t *LED3_G = RT_NULL;
static agile_led_t *LED3_R = RT_NULL;
static agile_led_t *LED4_G = RT_NULL;
static agile_led_t *LED4_R = RT_NULL;
static agile_led_t *LED5_G = RT_NULL;
static agile_led_t *LED5_R = RT_NULL;
static agile_led_t *LED6_G = RT_NULL;
static agile_led_t *LED6_R = RT_NULL;
static agile_led_t *LED7_G = RT_NULL;
static agile_led_t *LED7_R = RT_NULL;
static agile_led_t *LED8_G = RT_NULL;
static agile_led_t *LED8_R = RT_NULL;
static agile_led_t *BEEP = RT_NULL;
/*
*   该LED组件经修改,模式字符串的首位为关灯延迟时长
*/
void led_Init(void)
{
    LED1_G = agile_led_create(LED1_G_Pin, PIN_LOW, "3000,300,500", -1);
    LED1_R = agile_led_create(LED1_R_Pin, PIN_LOW, "0,300,500", -1);
    LED2_G = agile_led_create(LED2_G_Pin, PIN_LOW, "0,100,100", -1);
    LED2_R = agile_led_create(LED2_R_Pin, PIN_LOW, "0,100,100", -1);
    LED3_G = agile_led_create(LED3_G_Pin, PIN_LOW, "0,100,100", -1);
    LED3_R = agile_led_create(LED3_R_Pin, PIN_LOW, "0,100,100", -1);
    LED4_G = agile_led_create(LED4_G_Pin, PIN_LOW, "0,100,100", -1);
    LED4_R = agile_led_create(LED4_R_Pin, PIN_LOW, "0,100,100", -1);
    LED5_G = agile_led_create(LED5_G_Pin, PIN_LOW, "0,100,100", -1);
    LED5_R = agile_led_create(LED5_R_Pin, PIN_LOW, "0,100,100", -1);
    LED6_G = agile_led_create(LED6_G_Pin, PIN_LOW, "0,100,100", -1);
    LED6_R = agile_led_create(LED6_R_Pin, PIN_LOW, "0,100,100", -1);
    LED7_G = agile_led_create(LED7_G_Pin, PIN_LOW, "3000,300,500", -1);
    LED7_R = agile_led_create(LED7_R_Pin, PIN_LOW, "3000,300,500", -1);
    LED8_G = agile_led_create(LED8_G_Pin, PIN_LOW, "0,100,100", -1);
    LED8_R = agile_led_create(LED8_R_Pin, PIN_LOW, "0,300,500", -1);
    BEEP = agile_led_create(BUZZER, PIN_HIGH, "0,300,500", -1);
    psi_led_init();
}
void rf_433_lost(void)
{
    agile_led_set_light_mode(LED1_R,"300,100",1);
    agile_led_set_light_mode(LED2_R,"300,100",1);
    agile_led_set_light_mode(LED3_R,"300,100",1);
    agile_led_off(LED1_G);
    agile_led_off(LED2_G);
    agile_led_off(LED3_G);
    agile_led_start(LED1_R);
    agile_led_start(LED2_R);
    agile_led_start(LED3_R);
}
void rf_433_ultralow(void)
{
    agile_led_set_light_mode(LED1_R,"300,100",1);
    agile_led_set_light_mode(LED1_G,"300,100",1);
    agile_led_off(LED2_R);
    agile_led_off(LED2_G);
    agile_led_off(LED3_R);
    agile_led_off(LED3_G);
    agile_led_start(LED1_G);
    agile_led_start(LED1_R);
}
void rf_433_low(void)
{
    agile_led_set_light_mode(LED1_G,"300,100",1);
    agile_led_off(LED1_R);
    agile_led_off(LED2_R);
    agile_led_off(LED2_G);
    agile_led_off(LED3_R);
    agile_led_off(LED3_G);
    agile_led_start(LED1_G);
}
void rf_433_mid(void)
{
    agile_led_set_light_mode(LED1_G,"300,100",1);
    agile_led_set_light_mode(LED2_G,"300,100",1);
    agile_led_off(LED1_R);
    agile_led_off(LED2_R);
    agile_led_off(LED3_R);
    agile_led_off(LED3_G);
    agile_led_start(LED1_G);
    agile_led_start(LED2_G);
}
void rf_433_high(void)
{
    agile_led_set_light_mode(LED1_G,"300,100",1);
    agile_led_set_light_mode(LED2_G,"300,100",1);
    agile_led_set_light_mode(LED3_G,"300,100",1);
    agile_led_off(LED1_R);
    agile_led_off(LED2_R);
    agile_led_off(LED3_R);
    agile_led_start(LED1_G);
    agile_led_start(LED2_G);
    agile_led_start(LED3_G);
}
void led_rf433_start(uint8_t mode)
{
    switch(mode)
    {
    case 1:
        rf_433_lost();
        break;
    case 2:
        rf_433_ultralow();
        break;
    case 3:
        rf_433_low();
        break;
    case 4:
        rf_433_mid();
        break;
    case 5:
        rf_433_high();
        break;
    }
}
void led_rf433_stop(void)
{
    agile_led_off(LED1_R);
    agile_led_off(LED1_G);
    agile_led_off(LED2_R);
    agile_led_off(LED2_G);
    agile_led_off(LED3_R);
    agile_led_off(LED3_G);
}
void rf_4068_lost(void)
{
    agile_led_set_light_mode(LED4_R,"300,100",1);
    agile_led_set_light_mode(LED5_R,"300,100",1);
    agile_led_set_light_mode(LED6_R,"300,100",1);
    agile_led_off(LED4_G);
    agile_led_off(LED5_G);
    agile_led_off(LED6_G);
    agile_led_start(LED4_R);
    agile_led_start(LED5_R);
    agile_led_start(LED6_R);
}
void rf_4068_ultralow(void)
{
    agile_led_set_light_mode(LED4_R,"300,100",1);
    agile_led_set_light_mode(LED4_G,"300,100",1);
    agile_led_off(LED5_R);
    agile_led_off(LED5_G);
    agile_led_off(LED6_R);
    agile_led_off(LED6_G);
    agile_led_start(LED4_G);
    agile_led_start(LED4_R);
}
void rf_4068_low(void)
{
    agile_led_set_light_mode(LED4_G,"300,100",1);
    agile_led_off(LED4_R);
    agile_led_off(LED5_R);
    agile_led_off(LED5_G);
    agile_led_off(LED6_R);
    agile_led_off(LED6_G);
    agile_led_start(LED4_G);
}
void rf_4068_mid(void)
{
    agile_led_set_light_mode(LED4_G,"300,100",1);
    agile_led_set_light_mode(LED5_G,"300,100",1);
    agile_led_off(LED4_R);
    agile_led_off(LED5_R);
    agile_led_off(LED6_R);
    agile_led_off(LED6_G);
    agile_led_start(LED4_G);
    agile_led_start(LED5_G);
}
void rf_4068_high(void)
{
    agile_led_set_light_mode(LED4_G,"300,100",1);
    agile_led_set_light_mode(LED5_G,"300,100",1);
    agile_led_set_light_mode(LED6_G,"300,100",1);
    agile_led_off(LED4_R);
    agile_led_off(LED5_R);
    agile_led_off(LED6_R);
    agile_led_start(LED4_G);
    agile_led_start(LED5_G);
    agile_led_start(LED6_G);
}
void led_rf4068_start(uint8_t mode)
{
    switch(mode)
    {
    case 1:
        rf_4068_lost();
        break;
    case 2:
        rf_4068_ultralow();
        break;
    case 3:
        rf_4068_low();
        break;
    case 4:
        rf_4068_mid();
        break;
    case 5:
        rf_4068_high();
        break;
    }
}
void led_rf4068_stop(void)
{
    agile_led_off(LED4_R);
    agile_led_off(LED4_G);
    agile_led_off(LED5_R);
    agile_led_off(LED5_G);
    agile_led_off(LED6_R);
    agile_led_off(LED6_G);
}
void beep_calc(uint8_t level_4068,uint8_t level_433)
{
    if(level_4068 == 1 || level_433 == 1)
    {
        beep_start(0);
    }
}
void transmitter_lost(void)
{
    agile_led_set_light_mode(LED7_R,"300,100",1);
    agile_led_off(LED7_G);
    agile_led_start(LED7_R);
}
void transmitter_off(void)
{
    agile_led_set_light_mode(LED7_R,"300,100",1);
    agile_led_set_light_mode(LED7_G,"300,100",1);
    agile_led_start(LED7_G);
    agile_led_start(LED7_R);
}
void transmitter_on(void)
{
    agile_led_set_light_mode(LED7_G,"300,100",1);
    agile_led_off(LED7_R);
    agile_led_start(LED7_G);
}
void receiver_lost(void)
{
    agile_led_set_light_mode(LED8_R,"300,100",1);
    agile_led_off(LED8_G);
    agile_led_start(LED8_R);
}
void receiver_off(void)
{
    agile_led_set_light_mode(LED8_R,"300,100",1);
    agile_led_set_light_mode(LED8_G,"300,100",1);
    agile_led_start(LED8_G);
    agile_led_start(LED8_R);
}
void receiver_on(void)
{
    agile_led_set_light_mode(LED8_G,"300,100",1);
    agile_led_off(LED8_R);
    agile_led_start(LED8_G);
}
void beep_start(uint8_t select)
{
    agile_led_stop(BEEP);
    agile_led_off(BEEP);
    switch(select)
    {
    case 0://10s/1声
        agile_led_set_light_mode(BEEP,"300,200,10000",-1);
        break;
    case 1://5s/2声
        agile_led_set_light_mode(BEEP,"300,200,200,200,5000",-1);
        break;
    case 2://5s/3声
        agile_led_set_light_mode(BEEP,"300,200,200,200,200,200,5000",-1);
        break;
    }
    agile_led_start(BEEP);
}
void beep_stop(void)
{
    agile_led_stop(BEEP);
    agile_led_off(BEEP);
}
