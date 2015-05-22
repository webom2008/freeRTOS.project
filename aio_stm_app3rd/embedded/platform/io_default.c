/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : io_default.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/13
  Last Modified :
  Description   : default io handle
  Function List :
  History       :
  1.Date        : 2015/3/13
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "stm32f10x.h"
#include "io_default.h"

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
#if 0
#define PIN_UNUSED_OTYPE        GPIO_Mode_Out_OD
#else
#define PIN_UNUSED_OTYPE        GPIO_Mode_Out_PP
#endif
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

static void io_portA_unused_cfg(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode    = PIN_UNUSED_OTYPE;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin     =   GPIO_Pin_8 \
                                        | GPIO_Pin_11 \
                                        | GPIO_Pin_12 \
                                        | GPIO_Pin_13 \
                                        | GPIO_Pin_14 \
                                        | GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, (uint16_t)GPIO_InitStructure.GPIO_Pin);
}

static void io_portB_unused_cfg(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode    = PIN_UNUSED_OTYPE;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
    
    GPIO_InitStructure.GPIO_Pin     =   GPIO_Pin_2 \
                                        | GPIO_Pin_3 \
                                        | GPIO_Pin_4 \
                                        | GPIO_Pin_8 \
                                        | GPIO_Pin_9 \
                                        | GPIO_Pin_10 \
                                        | GPIO_Pin_11 \
                                        | GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, (uint16_t)GPIO_InitStructure.GPIO_Pin);
}

static void io_portC_unused_cfg(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    /* PC13, PC14 and PC15 are supplied through the power switch. 
     * Since the switch only sinks a limited amount of current (3 mA), 
     * the use of GPIOs PC13 to PC15 in output mode is limited: 
     * the speed should not exceed 2 MHz with a maximum load of 30 pF 
     * and these IOs must not be used as a current source 
     * (e.g. to drive an LED).
     */
    GPIO_InitStructure.GPIO_Mode    = PIN_UNUSED_OTYPE;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin     =   GPIO_Pin_13 \
                                        | GPIO_Pin_14 \
                                        | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, (uint16_t)GPIO_InitStructure.GPIO_Pin);
}

static void io_portD_unused_cfg(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    //Remap PD0 and PD1, default is OSC_IN OSC_OUT
    GPIO_PinRemapConfig(GPIO_Remap_PD01 , ENABLE);
    
    GPIO_InitStructure.GPIO_Mode    = PIN_UNUSED_OTYPE;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz; //@note:only 50MHz at output mode
    GPIO_InitStructure.GPIO_Pin     =   GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, (uint16_t)GPIO_InitStructure.GPIO_Pin);
}

//PB5,TIM3-CH2
static void io_test_pwm(void)
{
    uint16_t TimerPeriod = 0;
    uint16_t Channel2Pulse = 0;
    
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

//    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3 , ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 1 kHz
    TimerPeriod = (SystemCoreClock / 1000 ) - 1;
    /* Compute CCR1 value to generate a duty cycle at 50% for channel 1 and 1N */
    Channel2Pulse = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* Channel 2Configuration in PWM mode */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
//    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
//    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

    TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    TIM_Cmd(TIM3, ENABLE);

    TIM_CtrlPWMOutputs(TIM3, ENABLE);
}

void io_default_init(void)
{
    io_portA_unused_cfg();
    io_portB_unused_cfg();
    io_portC_unused_cfg();
    io_portD_unused_cfg();

//    io_test_pwm();
}

