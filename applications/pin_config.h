/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-06     Rick       the first version
 */

#ifndef APPLICATIONS_PIN_CONFIG_H_
#define APPLICATIONS_PIN_CONFIG_H_
/*
 * RF
 */
#define RF_SW1_PIN                      6
#define RF_SW2_PIN                      7
#define TCXO_PWR_PIN                    16
//SW
#define SW1                    2 //PA2
//BUZZER
#define LED_DATA              0  //PA0
//BUZZER
#define BUZZER                24 //PB8
//SIGNAL
#define AC                    4  //PA4
#define OUT1                  18 //PB2
#define OUT2                  28 //PB12
#define RELAY                 1  //PA1

//AX5043
#define AX5043_IRQ            11 //PA11

#endif /* APPLICATIONS_PIN_CONFIG_H_ */
