/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_interAdc.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/10
  Last Modified :
  Description   : interran app
  Function List :
  History       :
  1.Date        : 2015/4/10
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
#include "driver_interAdc.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern __IO uint16_t RegularConvData_Tab[];

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/
extern u32 IsOnTime(const u32 u32Target);

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
static InternalADCResult gInternalADCResult;
InternalADCResult *pgInternalADCResult = &gInternalADCResult;

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int app_interAdc_init(void)
{
    InAdc_dev_init();
    InAdc_dev_StarConvert();
    return 0;
}

static void InAdc_api_refresh(void)
{
    uint32_t *pBuf = (uint32_t *)pgInternalADCResult;
    uint8_t index;
    
    for(index = 0; index < ADC_TOTAL_CH_USED; index++)
    {
        pBuf[index] = ((RegularConvData_Tab[index]* 3300) / 0xFFF); //mv
    }

    //
}

static void InAdc_api_printf(void)
{
    uint32_t *pBuf = (uint32_t *)pgInternalADCResult;
    uint8_t index;
#if 0
    printf("\r\n[ADC]:");
    for(index = 0; index < ADC_TOTAL_CH_USED; index++)
    {
        printf("[%d]=%dmV,", index, pBuf[index]);
//        printf("[%d]=%d,", index, RegularConvData_Tab[index]);
    }
#else
    printf("\r\n[ADC]:");
    vTaskDelay((TickType_t)10);
    for(index = 0; index < ADC_TOTAL_CH_USED; index++)
    {
        printf("[%d]=%dmV,", index, pBuf[index]);
//        printf("[%d]=%d,", index, RegularConvData_Tab[index]);
        vTaskDelay((TickType_t)10);
    }
#endif
}

void app_interAdc_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    for (;;)
    {
        InAdc_dev_complete();
        InAdc_api_refresh();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_interAdcPrint_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    for (;;)
    {
        InAdc_api_printf();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

int app_interAdc_start(void)
{
    xTaskCreate(app_interAdc_task,\
                "interAdc Task",\
                1000,\
                NULL,\
                ADC_TASK_PRIORITY,\
                NULL);
#ifdef __ADC_DEBUG_PRINTF__
    xTaskCreate(app_interAdcPrint_task,\
                "app_interAdcPrint_task",\
                20,\
                NULL,\
                PRINTF_DEBUG_TASK_PRIORITY,\
                NULL);
#endif
    return 0;
}
