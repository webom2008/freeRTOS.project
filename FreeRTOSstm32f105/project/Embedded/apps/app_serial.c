/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_serial.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/6/16
  Last Modified :
  Description   : app serial 
  Function List :
  History       :
  1.Date        : 2015/6/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include <string.h>

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

#define _SERIAL_INFO_
#ifdef _SERIAL_INFO_
#define SERIAL_INFO(fmt, arg...) printf("\r\n[Serial] "fmt, ##arg)
#else
#define SERIAL_INFO(fmt, arg...) do{}while(0)
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

static void app_serial_task(void *pvParameters)
{
    char pBuf[33];
    int nLen = 0;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    for (;;)
    {
        memset(pBuf, 0, sizeof(pBuf));
        nLen = xSerialGetBuffer((u8 *)pBuf, 32, UART_NO_BLOCK);
        pBuf[32] = '\0'; //string end flag
        if (nLen)
        {
            SERIAL_INFO("Get String = %s, nLen=%d", pBuf, nLen);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS); //delay 1s
    }
}

int app_serial_init(void)
{
    return 0;
}

int app_serial_start(void)
{
    xTaskCreate(app_serial_task,
                "Serial Task",
                128,
                NULL,
                SERIAL_TASK_PRIORITY,
                NULL);
    return 0;
}
