/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_ecg.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/11
  Last Modified :
  Description   : ecg app
  Function List :
  History       :
  1.Date        : 2015/4/11
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

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
#define ECG_DETECT_TIME         100//ms
#define ECG_PROBE_PERIOD        500

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
int app_ecg_init(void)
{
    ecg_api_init();
    return 0;
}

static void ecg_sample_task(void *pvParameters)
{
    u8 u8CntForUpdate = 0;
    /* Just to stop compiler warnings. */
	( void ) pvParameters;

    for(;;)
    {
        ecg_api_saveDetectData();
        ecg_api_setNextChannel();
        if (u8CntForUpdate++ >= (ECG_PROBE_PERIOD/ECG_DETECT_TIME))
        {
            u8CntForUpdate = 0;
            ecg_api_updateProbeStatus();
        }
        vTaskDelay(ECG_DETECT_TIME / portTICK_PERIOD_MS);
    }
}

static void ecg_printf_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    for (;;)
    {
        ecg_api_printf();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

int app_ecg_start(void)
{
    xTaskCreate(ecg_sample_task,\
                "ecg_sample_task",\
                20,\
                NULL,\
                ECG_TASK_PRIORITY,\
                NULL);
#ifdef __ECG_DEBUG_PRINTF__
    xTaskCreate(ecg_printf_task,\
                "ecg_printf_task",\
                20,\
                NULL,\
                PRINTF_DEBUG_TASK_PRIORITY,\
                NULL);
#endif
    return 0;
}
