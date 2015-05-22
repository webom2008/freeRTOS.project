/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_ecg.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/11
  Last Modified :
  Description   : ecg driver
  Function List :
  History       :
  1.Date        : 2015/4/11
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "driver_ecg.h"

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
#define LOFF_SEL_A_PIN      GPIO_Pin_15
#define LOFF_SEL_A_PORT     GPIOB
#define LOFF_SEL_A_CLK      RCC_APB2Periph_GPIOB

#define LOFF_SEL_B_PIN      GPIO_Pin_14
#define LOFF_SEL_B_PORT     GPIOB
#define LOFF_SEL_B_CLK      RCC_APB2Periph_GPIOB

#define LOFF_SEL_C_PIN      GPIO_Pin_13
#define LOFF_SEL_C_PORT     GPIOB
#define LOFF_SEL_C_CLK      RCC_APB2Periph_GPIOB

#define SEL_A_HIGH()        GPIO_SetBits(LOFF_SEL_A_PORT, LOFF_SEL_A_PIN)
#define SEL_A_LOW()         GPIO_ResetBits(LOFF_SEL_A_PORT, LOFF_SEL_A_PIN)

#define SEL_B_HIGH()        GPIO_SetBits(LOFF_SEL_B_PORT, LOFF_SEL_B_PIN)
#define SEL_B_LOW()         GPIO_ResetBits(LOFF_SEL_B_PORT, LOFF_SEL_B_PIN)

#define SEL_C_HIGH()        GPIO_SetBits(LOFF_SEL_C_PORT, LOFF_SEL_C_PIN)
#define SEL_C_LOW()         GPIO_ResetBits(LOFF_SEL_C_PORT, LOFF_SEL_C_PIN)

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

void ecg_dev_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LOFF_SEL_A_CLK | LOFF_SEL_B_CLK | LOFF_SEL_C_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LOFF_SEL_A_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LOFF_SEL_A_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = LOFF_SEL_B_PIN;
    GPIO_Init(LOFF_SEL_B_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = LOFF_SEL_C_PIN;
    GPIO_Init(LOFF_SEL_C_PORT, &GPIO_InitStructure);
}

u8 ecg_dev_setChannel(ECG_SelectTypeDef channel)
{
    switch(channel)
    {
        case RA_OFF://000
        {
            SEL_C_LOW();
            SEL_B_LOW();
            SEL_A_LOW();
            break;
        }
        case RL_OFF://001
        {
            SEL_C_LOW();
            SEL_B_LOW();
            SEL_A_HIGH();
            break;
        }
        case LA_OFF://010
        {
            SEL_C_LOW();
            SEL_B_HIGH();
            SEL_A_LOW();
            break;
        }
        case LL_OFF://011
        {
            SEL_C_LOW();
            SEL_B_HIGH();
            SEL_A_HIGH();
            break;
        }
        case V1_OFF://100
        {
            SEL_C_HIGH();
            SEL_B_LOW();
            SEL_A_LOW();
            break;
        }
//        case ECG1_OVERLOAD://101
//        {
//            SEL_C_HIGH();
//            SEL_B_LOW();
//            SEL_A_HIGH();
//            break;
//        }
//        case ECG2_OVERLOAD://110
//        {
//            SEL_C_HIGH();
//            SEL_B_HIGH();
//            SEL_A_LOW();
//            break;
//        }
//        case ECGV_OVERLOAD://111
//        {
//            SEL_C_HIGH();
//            SEL_B_HIGH();
//            SEL_A_HIGH();
//            break;
//        }
        default:
//            printf("\r\nECG_SelectChannel error = %d",channel);
            return 0;
    }
    return 1;
}

