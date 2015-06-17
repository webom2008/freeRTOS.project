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
typedef enum
{
    LED_MODE_SINGLE,
    LED_CTRL_CYCLE,
} LED_MODE_TypeDef;

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
//#define _LED_INFO_
#ifdef _LED_INFO_
#define LED_INFO(fmt, arg...) udprintf("\r\n[LED] "fmt, ##arg)
#else
#define LED_INFO(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
static int led_period = 1000;
static LED_MODE_TypeDef led_work_mode = LED_CTRL_CYCLE;

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


void app_led_ctl(LED_CTRL_TypeDef ctl, int period)
{
    if (LED_CTRL_ON == ctl)
    {
        led_work_mode = LED_MODE_SINGLE;
        led_dev_on(LED1);
    }
    else if (LED_CTRL_OFF == ctl)
    {
        led_work_mode = LED_MODE_SINGLE;
        led_dev_off(LED1);
    }
    else if (LED_CTRL_PERIOD == ctl)
    {
        led_period = period;
        led_work_mode = LED_CTRL_CYCLE;
        led_dev_toggle(LED1);
    }
}


static void app_led_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[LED] app_led_task running...");
    for (;;)
    {
        LED_INFO("running...");
        if (LED_CTRL_CYCLE == led_work_mode)
        {
            led_dev_toggle(LED1);
        }
        vTaskDelay(led_period / portTICK_PERIOD_MS); //delay 1s
    }
}

int app_led_init(void)
{
    led_dev_init(LED1);
    return 0;
}

int app_led_start(void)
{
    xTaskCreate(app_led_task,
                "LED Task",
                configMINIMAL_STACK_SIZE,
                NULL,
                LED_TASK_PRIORITY,
                NULL);
    return 0;
}

