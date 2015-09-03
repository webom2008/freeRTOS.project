/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : uart_driver_test.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/9/3
  Last Modified :
  Description   : uart driver test app
  Function List :
  History       :
  1.Date        : 2015/9/3
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
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define TEST_PERIOD_MS      1000
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/



static void uart_driver_task(void *pvParameters)
{
    char rBuf[64];
    int rLen = 0;
    unsigned int test_count = 0;
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart_driver_task running...");
    for (;;)
    {
        udprintf("\r\n>>uart_driver_task :%d",test_count++);
        Uart1Write("\r\n>>Uart1Write Testing...",
            strlen("\r\n>>Uart1Write Testing..."));
        
        udprintf("\r\n>>Uart1Read :");
        memset(rBuf, 0x00, sizeof(rBuf));
        rLen = Uart1Read(rBuf, sizeof(rBuf));
        if (rLen > 0)
        {
            udprintf("rLen=%d :",rLen);
            if (rLen >= sizeof(rBuf)) rLen = sizeof(rBuf) -1;
            rBuf[rLen] = '\0';
            udprintf("%s",rBuf);
        }
        else
        {
            udprintf("Empty");
        }
        
        vTaskDelay(TEST_PERIOD_MS / portTICK_PERIOD_MS); //delay 1s
    }
}

int uart_driver_test(void)
{
    xTaskCreate(uart_driver_task,
                "Uart Driver Task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
    return 0;
}

