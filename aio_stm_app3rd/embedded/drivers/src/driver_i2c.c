/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : i2c_device.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/2/16
  Last Modified :
  Description   : driver for i2c
  Function List :
  History       :
  1.Date        : 2015/2/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "driver_i2c.h"
#include "i2c_it.h"

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
#define AioI2C_ADDR                 0x60

#define ClockSpeed              100000

#define AioI2C_SCL_CLK          RCC_APB2Periph_GPIOB
#define AioI2C_SCL_PORT         GPIOB
#define AioI2C_SCL_PIN          GPIO_Pin_6
#define AioI2C_SCL_SOURCE       GPIO_PinSource6

#define AioI2C_SDA_CLK          RCC_APB2Periph_GPIOB
#define AioI2C_SDA_PORT         GPIOB
#define AioI2C_SDA_PIN          GPIO_Pin_7
#define AioI2C_SDA_SOURCE       GPIO_PinSource7

#define AioI2C_CLK              RCC_APB1Periph_I2C1
#define AioI2C_ISR_TXEM         ((uint32_t)0x00000001)

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

void i2c_dev_gpio_init(I2C_TypeDef *pDev)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (I2C1 == pDev)
    {
        /* Set GPIO frequency to 50MHz */
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        /* Select Alternate function mode */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        /* Initialize I2Cx SCL Pin */
        GPIO_InitStructure.GPIO_Pin = AioI2C_SCL_PIN;
        GPIO_Init(AioI2C_SCL_PORT, &GPIO_InitStructure);

        /* Initialize I2Cx SDA Pin */
        GPIO_InitStructure.GPIO_Pin = AioI2C_SDA_PIN;
        GPIO_Init(AioI2C_SDA_PORT, &GPIO_InitStructure);
    }
    else //TODO
    {

    }
}

static void i2c_dev_rcc_init(I2C_TypeDef *pDev)
{
    I2C_DeInit(pDev);
    
    if (I2C1 == pDev)
    {
        /* Enable I2Cx device clock */
        RCC_APB1PeriphClockCmd(AioI2C_CLK, ENABLE);
        /* Enable I2Cx SCL and SDA Pin Clock */
        RCC_APB2PeriphClockCmd((AioI2C_SCL_CLK|AioI2C_SDA_CLK|RCC_APB2Periph_AFIO), ENABLE);
    }
    else //TODO
    {

    }
}

void i2c_dev_cfg(I2C_TypeDef *pDev)
{
    I2C_InitTypeDef InitTypeDef;
    
    I2C_DeInit(pDev);
    
    InitTypeDef.I2C_Mode = I2C_Mode_I2C;
    InitTypeDef.I2C_DutyCycle = I2C_DutyCycle_16_9;//I2C_DutyCycle_16_9 or I2C_DutyCycle_2
    InitTypeDef.I2C_OwnAddress1 = AioI2C_ADDR;
    InitTypeDef.I2C_Ack = I2C_Ack_Enable;
    InitTypeDef.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    InitTypeDef.I2C_ClockSpeed = ClockSpeed;
    I2C_Init(pDev, &InitTypeDef);
    I2C_Cmd(pDev, ENABLE);
}

int i2c_dev_init(I2C_TypeDef *pDev)
{
    int res = -1;
    
    i2c_dev_rcc_init(pDev);
    i2c_dev_gpio_init(pDev);
    i2c_dev_cfg(pDev);
    res = i2c_it_cfg();

    return res;
}

