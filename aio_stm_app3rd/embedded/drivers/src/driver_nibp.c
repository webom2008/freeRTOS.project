/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_nibp.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/15
  Last Modified :
  Description   : nibp driver
  Function List :
  History       :
  1.Date        : 2015/4/15
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "stm32f10x.h"

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
#define NIBP_PRESS_RELE_PIN             GPIO_Pin_5
#define NIBP_PRESS_RELE_PORT            GPIOA
#define NIBP_PRESS_RELE_GPIO_CLK        RCC_APB2Periph_GPIOA

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
void nibp_dev_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(NIBP_PRESS_RELE_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = NIBP_PRESS_RELE_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(NIBP_PRESS_RELE_PORT, &GPIO_InitStructure);
}

void nibp_dev_setPressRelePin(char action)
{
    if (!action) // 0
    {
        GPIO_ResetBits(NIBP_PRESS_RELE_PORT, NIBP_PRESS_RELE_PIN);
    }
    else
    {
        GPIO_SetBits(NIBP_PRESS_RELE_PORT, NIBP_PRESS_RELE_PIN);
    }
}

