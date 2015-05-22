/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : main.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : main
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern char APP_VERSION[40];

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/
extern void version_init(void);

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

static int system_init(void)
{
    version_init();
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
    return 0;
}

static int platform_init(void)
{
    int res = -1;
    
    res = app_led_init();
    if (res < 0) return res;
    
    res = app_serial_init();
    if (res < 0) return res;
    
    res = app_interAdc_init();
    if (res < 0) return res;
    
    res = app_i2c_init();
    if (res < 0) return res;

    res = app_ecg_init();
    if (res < 0) return res;
    
    res = app_nibp_init();
    if (res < 0) return res;
    
    res = app_power_init();
    if (res < 0) return res;
    return 0;
}

static int startAllApps(void)
{
    app_led_start();
    //app_serial_start();
    app_interAdc_start();
    app_i2c_start();
    app_ecg_start();
    app_nibp_start();
    app_power_start();
    return 0;
}

int main(void)
{
    int res = 0;
    
    res |= system_init();
    res |= platform_init();
    io_default_init();
    printf("\r\nFreeRTOS_AioStmApp3rd:%s",APP_VERSION);
    if (res < 0)
    {
        for(;;);
    }
    else
    {
        startAllApps();
        vTaskStartScheduler();
    }
}

