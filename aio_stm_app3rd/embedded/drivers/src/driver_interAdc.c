/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_interAdc.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/10
  Last Modified :
  Description   : internal adc driver
  Function List :
  History       :
  1.Date        : 2015/4/10
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "stm32f10x.h"
#include "driver_interAdc.h"

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
#define ADC1_DR_Address             ((uint32_t)0x4001244C)

#define ADC_CH_BBP                  ADC_Channel_1
#define ADC_CH_BBP_CLK              RCC_APB2Periph_GPIOA
#define ADC_CH_BBP_PORT             GPIOA
#define ADC_CH_BBP_PIN              GPIO_Pin_1
#define ADC_CH_BBP_SOURCE           GPIO_PinSource1

#define ADC_CH_D3V3                 ADC_Channel_2
#define ADC_CH_D3V3_CLK             RCC_APB2Periph_GPIOA
#define ADC_CH_D3V3_PORT            GPIOA
#define ADC_CH_D3V3_PIN             GPIO_Pin_2
#define ADC_CH_D3V3_SOURCE          GPIO_PinSource2

#define ADC_CH_VDDINT               ADC_Channel_3
#define ADC_CH_VDDINT_CLK           RCC_APB2Periph_GPIOA
#define ADC_CH_VDDINT_PORT          GPIOA
#define ADC_CH_VDDINT_PIN           GPIO_Pin_3
#define ADC_CH_VDDINT_SOURCE        GPIO_PinSource3

#define ADC_CH_PROBE                ADC_Channel_6
#define ADC_CH_PROBE_CLK            RCC_APB2Periph_GPIOA
#define ADC_CH_PROBE_PORT           GPIOA
#define ADC_CH_PROBE_PIN            GPIO_Pin_6
#define ADC_CH_PROBE_SOURCE         GPIO_PinSource6

#define ADC_CH_BASER                ADC_Channel_7
#define ADC_CH_BASER_CLK            RCC_APB2Periph_GPIOA
#define ADC_CH_BASER_PORT           GPIOA
#define ADC_CH_BASER_PIN            GPIO_Pin_7
#define ADC_CH_BASER_SOURCE         GPIO_PinSource7

#define ADC_CH_5V_N                 ADC_Channel_8
#define ADC_CH_5V_N_CLK             RCC_APB2Periph_GPIOB
#define ADC_CH_5V_N_PORT            GPIOB
#define ADC_CH_5V_N_PIN             GPIO_Pin_0
#define ADC_CH_5V_N_SOURCE          GPIO_PinSource0

#define ADC_CH_5V_P                 ADC_Channel_9
#define ADC_CH_5V_P_CLK             RCC_APB2Periph_GPIOB
#define ADC_CH_5V_P_PORT            GPIOB
#define ADC_CH_5V_P_PIN             GPIO_Pin_1
#define ADC_CH_5V_P_SOURCE          GPIO_PinSource1

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
__IO uint16_t RegularConvData_Tab[ADC_TOTAL_CH_USED];

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
static void InAdc_dev_rcc_init(void)
{
    /* ADCCLK 12MHz Max */
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);//24MHz/4=6MHz
	
    RCC_APB2PeriphClockCmd(ADC_CH_BBP_CLK \
                          | ADC_CH_D3V3_CLK\
                          | ADC_CH_VDDINT_CLK\
                          | ADC_CH_PROBE_CLK\
                          | ADC_CH_BASER_CLK\
                          | ADC_CH_5V_N_CLK\
                          | ADC_CH_5V_P_CLK\
                          | RCC_APB2Periph_AFIO, ENABLE);

    /* Enable ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    /* Enable DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
}

static void InAdc_dev_gpio_init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    

    GPIO_InitStructure.GPIO_Pin     = ADC_CH_BBP_PIN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	
    GPIO_Init(ADC_CH_BBP_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = ADC_CH_D3V3_PIN ;
    GPIO_Init(ADC_CH_D3V3_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = ADC_CH_VDDINT_PIN ;
    GPIO_Init(ADC_CH_VDDINT_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = ADC_CH_PROBE_PIN ;
    GPIO_Init(ADC_CH_PROBE_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = ADC_CH_BASER_PIN ;
    GPIO_Init(ADC_CH_BASER_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = ADC_CH_5V_N_PIN ;
    GPIO_Init(ADC_CH_5V_N_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = ADC_CH_5V_P_PIN ;
    GPIO_Init(ADC_CH_5V_P_PORT, &GPIO_InitStructure);
}

static void InAdc_dev_cfg(void)
{
    ADC_InitTypeDef     ADC_InitStructure;

    /* ADC1 DeInit */  
    ADC_DeInit(ADC1);

    //ADC1 configuration
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_TOTAL_CH_USED;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Convert the ADC1 Channel11 and channel10 with 55.5 Cycles as sampling time */ 
    ADC_RegularChannelConfig(ADC1, ADC_CH_BBP,      1,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_CH_D3V3 ,    2,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_CH_VDDINT ,  3,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_CH_PROBE ,   4,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_CH_BASER ,   5,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_CH_5V_N ,    6,  ADC_SampleTime_55Cycles5);  
    ADC_RegularChannelConfig(ADC1, ADC_CH_5V_P ,    7,  ADC_SampleTime_55Cycles5);

    //Enable ADC1 DMA
    ADC_DMACmd(ADC1, ENABLE);

    //Enable ADC1
    ADC_Cmd(ADC1, ENABLE);

    //Enable ADC1 reset calibration register  
    ADC_ResetCalibration(ADC1);
    //Check the end of ADC1 reset calibration register
    while(ADC_GetResetCalibrationStatus(ADC1));

    //Start ADC1 calibration
    ADC_StartCalibration(ADC1);
    //Check the end of ADC1 calibration
    while(ADC_GetCalibrationStatus(ADC1));

    /* ADC1 regular Software Start Conv */ 
//    ADC_StartOfConversion(ADC1);
}

static void InAdc_dev_cfgDMA(void)
{
    DMA_InitTypeDef   DMA_InitStructure;

    /* DMA1 Channel1 Config */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_Tab;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ADC_TOTAL_CH_USED;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
}

void InAdc_dev_StarConvert(void)
{
    DMA_Cmd(DMA1_Channel1,ENABLE);//Enable DMA1 channel1
    ADC_Cmd(ADC1,ENABLE);
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);//Start ADC1 Software Conversion
}

void InAdc_dev_init(void)
{
    InAdc_dev_rcc_init();
    InAdc_dev_gpio_init();
    InAdc_dev_cfgDMA();
    InAdc_dev_cfg();
}

void InAdc_dev_complete(void)
{
    /* Test DMA1 TC flag */
    while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET );
    
    /* Clear DMA TC flag */
    DMA_ClearFlag(DMA1_FLAG_TC1);
}

