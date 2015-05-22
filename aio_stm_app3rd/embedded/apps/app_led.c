/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_led.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : app for led
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
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
static void app_led_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    for (;;)
    {
        led_dev_toggle(LED1);
//        printf("\r\n>>app_led_task running");
        vTaskDelay(500 / portTICK_PERIOD_MS); //delay 1s
    }
}

int app_led_init(void)
{
    led_dev_init(LED1);
    return 0;
}

int app_led_start(void)
{
    xTaskCreate(app_led_task,"LED Task",1000,NULL,LED_TASK_PRIORITY,NULL);
    return 0;
}

