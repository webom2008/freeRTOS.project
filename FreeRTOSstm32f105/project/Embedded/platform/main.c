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
extern char APP_VERSION[];

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

#define _MAIN_INFO_
#ifdef _MAIN_INFO_
#define MAIN_INFO(fmt, arg...) udprintf("\r\n[Main] "fmt, ##arg)
#else
#define MAIN_INFO(fmt, arg...) do{}while(0)
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


static int system_init(void)
{
    version_init();
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
    Uart1Init();
    return 0;
}

static int platform_init(void)
{
    int res = 0;
    
    res |= app_led_init();
    res |= app_serial_init();
    res |= app_cli_init();
    return res;
}

static int startAllApps(void)
{
    app_led_start();
    app_serial_start();
    app_cli_start();
    return 0;
}

int main(void)
{
    int res = 0;
    
    res |= system_init();
    res |= platform_init();
    
    MAIN_INFO("VERSION:%s",APP_VERSION);
    if (res)
    {
        MAIN_INFO("System Initialize failed!");
        while(1);
    }
    else
    {
        startAllApps();
        vTaskStartScheduler();
    }
}

