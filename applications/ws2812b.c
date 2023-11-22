/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-24     Tobby       the first version
 */
#include "ws2812b.h"
#include "rtthread.h"
#include "rtdevice.h"
#include "board.h"

#define DBG_TAG "ws2818b"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

const RGB_Color_TypeDef RED      = {255,0,0};
const RGB_Color_TypeDef GREEN    = {0,255,0};
const RGB_Color_TypeDef BLUE     = {0,0,255};
const RGB_Color_TypeDef BLACK    = {0,0,0};
const RGB_Color_TypeDef ORANGE   = {255,165,0};

static uint32_t Pixel_Buf[Pixel_NUM + 1][24] = {0};

static TIM_HandleTypeDef pwm_tim_handle;
static DMA_HandleTypeDef pwm_ch1_handle;

void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&pwm_ch1_handle);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

void ws2812b_hw_init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* DMA controller clock enable */
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);

    pwm_tim_handle.Instance = TIM2;
    pwm_tim_handle.Init.Prescaler = 0;
    pwm_tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwm_tim_handle.Init.Period = 59;
    pwm_tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    pwm_tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&pwm_tim_handle) != HAL_OK)
    {
      Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&pwm_tim_handle, &sMasterConfig) != HAL_OK)
    {
      Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&pwm_tim_handle, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
      Error_Handler();
    }

    HAL_TIM_MspPostInit(&pwm_tim_handle);
}

void ws2812b_red(uint8_t id,uint8_t value)
{
    if(value)
    {
        for(uint8_t i=0;i<8;i++) Pixel_Buf[id][i]   = ( (30 & (1 << (7 -i)))? (CODE_1):CODE_0 );//数组某一行0~7转化存放R
    }
    else
    {
        for(uint8_t i=0;i<8;i++) Pixel_Buf[id][i]   = ( (0 & (1 << (7 -i)))? (CODE_1):CODE_0 );//数组某一行0~7转化存放R
    }
}

void ws2812b_green(uint8_t id,uint8_t value)
{

    if(value)
    {
        for(uint8_t i=8;i<16;i++) Pixel_Buf[id][i]   = ( (30 & (1 << (15 -i)))? (CODE_1):CODE_0 );//数组某一行8-15转化存放G
    }
    else
    {
        for(uint8_t i=8;i<16;i++) Pixel_Buf[id][i]   = ( (0 & (1 << (15 -i)))? (CODE_1):CODE_0 );//数组某一行8-15转化存放G
    }
}

void ws2812b_blue(uint8_t id,uint8_t value)
{
    if(value)
    {
        for(uint8_t i=16;i<24;i++) Pixel_Buf[id][i]   = ( (255 & (1 << (23 -i)))? (CODE_1):CODE_0 );//数组某一行8-15转化存放R
    }
    else
    {
        for(uint8_t i=16;i<24;i++) Pixel_Buf[id][i]   = ( (0 & (1 << (23 -i)))? (CODE_1):CODE_0 );//数组某一行8-15转化存放R
    }
}

void ws2812b_orange(uint8_t id,uint8_t value)
{
    if(value)
    {
        for(uint8_t i=0;i<8;i++) Pixel_Buf[id][i]   = ( (30 & (1 << (7 -i)))? (CODE_1):CODE_0 );//数组某一行0~7转化存放G
        for(uint8_t i=8;i<16;i++) Pixel_Buf[id][i]   = ( (30 & (1 << (15 -i)))? (CODE_1):CODE_0 );//数组某一行8-15转化存放R

    }
    else
    {
        for(uint8_t i=0;i<8;i++) Pixel_Buf[id][i]   = ( (0 & (1 << (7 -i)))? (CODE_1):CODE_0 );//数组某一行0~7转化存放G
        for(uint8_t i=8;i<16;i++) Pixel_Buf[id][i]   = ( (0 & (1 << (15 -i)))? (CODE_1):CODE_0 );//数组某一行8-15转化存放R
    }
}

void ws2812b_init(void)
{
    ws2812b_hw_init();
    RGB_SetColor(0,BLACK);
    RGB_SetColor(1,BLACK);
    RGB_SetColor(2,BLACK);
    RGB_SetColor(3,BLACK);
    RGB_SetColor(4,BLACK);
    RGB_SetColor(5,BLACK);
    RGB_SetColor(6,BLACK);
    RGB_SetColor(7,BLACK);
    //HAL_TIM_PWM_Start_DMA(&pwm_tim_handle, TIM_CHANNEL_1, (uint32_t *)Pixel_Buf,(Pixel_NUM + 1)*24);
}

void RGB_SetColor(uint8_t id,RGB_Color_TypeDef Color)
{
    uint8_t i;

    for(i=0;i<8;i++) Pixel_Buf[id][i]   = ( (Color.R & (1 << (7 -i)))? (CODE_1):CODE_0 );//数组某一行0~7转化存放G
    for(i=8;i<16;i++) Pixel_Buf[id][i]  = ( (Color.G & (1 << (15-i)))? (CODE_1):CODE_0 );//数组某一行8~15转化存放R
    for(i=16;i<24;i++) Pixel_Buf[id][i] = ( (Color.B & (1 << (23-i)))? (CODE_1):CODE_0 );//数组某一行16~23转化存放B
}

void RGB_SendArray(void)
{
    HAL_TIM_PWM_Stop_DMA(&pwm_tim_handle, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_DMA(&pwm_tim_handle, TIM_CHANNEL_1, (uint32_t *)Pixel_Buf,(Pixel_NUM + 1)*24);
}
