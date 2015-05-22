/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_nibp.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/15
  Last Modified :
  Description   : nibp app
  Function List :
  History       :
  1.Date        : 2015/4/15
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
#define NIBP_HANDLE_TIME_MS         10
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
 
int app_nibp_init(void)
{
    nibp_api_init();
    return 0;
}

static void nibp_sample_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    for (;;)
    {
        if (nibp_api_isVerifying()) return;
        else nibp_api_checkPressRele();
        vTaskDelay(NIBP_HANDLE_TIME_MS / portTICK_PERIOD_MS);
    }
}

static void nibp_printf_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    for (;;)
    {
        nibp_api_printf();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

int app_nibp_start(void)
{
    xTaskCreate(nibp_sample_task,\
                "nibp_sample_task",\
                20,\
                NULL,\
                NIBP_TASK_PRIORITY,\
                NULL);
    
#ifdef __NIBP_DEBUG_PRINTF__
    xTaskCreate(nibp_printf_task,\
                "nibp_printf_task",\
                20,\
                NULL,\
                PRINTF_DEBUG_TASK_PRIORITY,\
                NULL);
#endif
}