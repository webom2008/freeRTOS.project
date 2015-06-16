/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_led.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : driver for led
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "stm32f10x.h"
#include "driver_led.h"
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define LED1_PIN                         GPIO_Pin_3
#define LED1_GPIO_PORT                   GPIOB
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOB

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK};

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

void led_dev_init(Led_TypeDef Led)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* Enable the GPIO_LED Clock */
    RCC_APB2PeriphClockCmd(GPIO_CLK[Led], ENABLE);

    // �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    
     // �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);   
     

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);

    led_dev_off(Led);
}

void led_dev_on(Led_TypeDef Led)
{
    GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
}

void led_dev_off(Led_TypeDef Led)
{
    GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
}

void led_dev_toggle(Led_TypeDef Led)
{
    GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}

