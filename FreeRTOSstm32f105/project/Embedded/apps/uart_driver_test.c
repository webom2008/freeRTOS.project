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
#ifdef CONFIG_UART1_DMA_MODE
extern EventGroupHandle_t xUart1RxEventGroup;
#endif

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

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/



static void uart_driver_task(void *pvParameters)
{
#ifdef CONFIG_UART1_INT_MODE
    char rBuf[64];
    int rLen = 0;
#endif
    unsigned int test_count = 0;
    const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS; //delay 1s
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart_driver_task running...");
    for (;;)
    {
        udprintf("\r\n>>uart_driver_task :%d",test_count++);
        Uart1Write("\r\n>>Uart1Write Testing...",
            strlen("\r\n>>Uart1Write Testing..."));
#ifdef CONFIG_UART1_INT_MODE
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
#endif
        vTaskDelay(xTicksToWait);
    }
}

#ifdef CONFIG_UART1_DMA_MODE
static void uart1_unpack_task(void *pvParameters)
{
    int rLen = 0;
    char rBuf[64];
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[TEST] uart1_unpack_task running...");
    for (;;)
    {
//        udprintf("\r\n>>uart1_unpack_task Testing...");
		uxBits = xEventGroupWaitBits(
					xUart1RxEventGroup,	// The event group being tested.
					UART1_DMA_RX_COMPLETE_EVENT_BIT \
					| UART1_DMA_RX_INCOMPLETE_EVENT_BIT,	// The bits within the event group to wait for.
					pdTRUE,			// BIT_COMPLETE and BIT_TIMEOUT should be cleared before returning.
					pdFALSE,		// Don't wait for both bits, either bit will do.
					xTicksToWait );	// Wait a maximum of 100ms for either bit to be set.

		if( ( uxBits & UART1_DMA_RX_COMPLETE_EVENT_BIT ) != 0 )
		{
            rLen = Uart1Read(rBuf, sizeof(rBuf));
            udprintf("\r\n[TEST] Uart1Read COMPLETE rLen=%d",rLen);
		}
		else if( ( uxBits & UART1_DMA_RX_INCOMPLETE_EVENT_BIT ) != 0 )
		{
            rLen = Uart1Read(rBuf, sizeof(rBuf));
            udprintf("\r\n[TEST] Uart1Read INCOMPLETE rLen=%d",rLen);
		}
		else
		{
		}
    }
}
#endif

int uart_driver_test(void)
{
    xTaskCreate(uart_driver_task,
                "uart_driver_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
#ifdef CONFIG_UART1_DMA_MODE
    xTaskCreate(uart1_unpack_task,
                "uart1_unpack_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_DRIVERS_TASK_PRIORITY,
                NULL);
#endif
    return 0;
}

