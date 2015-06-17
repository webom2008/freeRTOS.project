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

#define SERIAL_NO_BLOCK	            ( ( TickType_t ) 0 )
#define SERIAL_RX_QUEUE_LEN         ( ( UBaseType_t ) 64 )


//#define _SERIAL_INFO_
#ifdef _SERIAL_INFO_
#define SERIAL_INFO(fmt, arg...) udprintf("\r\n[Serial] "fmt, ##arg)
#else
#define SERIAL_INFO(fmt, arg...) do{}while(0)
#endif

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
static SemaphoreHandle_t xSemphrCLICommandEndFlag = NULL;
static QueueHandle_t xSerialRxQueue = NULL;

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


long serial_recvCLICommand(signed char *pBuf, signed char nMaxLen)
{
    long nRecvLen = 0;
    signed char cByteRxed;

    memset(pBuf, (u8)0x00, nMaxLen);
    if( pdTRUE == xSemaphoreTake(xSemphrCLICommandEndFlag, portMAX_DELAY))
    {
        do
        {
            if( pdPASS == xQueueReceive( xSerialRxQueue, &cByteRxed, SERIAL_NO_BLOCK ))
        	{
                pBuf[nRecvLen++] = cByteRxed;
        	}
            else
            {
                break;
            }
        } while (( CLI_CMD_END_TAG != cByteRxed ) &&(nRecvLen < nMaxLen));

        if (nRecvLen < nMaxLen)
        {
            pBuf[nRecvLen] = '\0';
        }
        else
        {
            pBuf[nMaxLen-1] = '\0';
        }
        
        SERIAL_INFO("xSemaphoreTake");

//        SERIAL_INFO("serial_recvCLICommand=%s",pBuf);
        
        // @notes: We must not to Release the semaphore here.
        //xSemaphoreGive( xSemphrCLICommandEndFlag );
    }
    return nRecvLen;
}


long serial_sendCLIResult(signed char *pBuf, signed char nMaxLen)
{
    return xSerialPutBuffer((u8 *)pBuf, nMaxLen);
}

static void app_serial_task(void *pvParameters)
{
    u8 pBuf[32];
    int getLen, i;
    BaseType_t res;
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    
    udprintf("\r\n[Serial] app_serial_task running...");
    xSemaphoreTake(xSemphrCLICommandEndFlag, ( TickType_t )0);
	for( ;; )
	{
		getLen = xSerialGetBuffer(pBuf, sizeof(pBuf));
        
        if (getLen > 0)
        {
            xSerialPutBuffer(pBuf, getLen);
            SERIAL_INFO("xSerialGetBuffer Len=%d First=%d",getLen, pBuf[0]);
        }

        for (i = 0; i < getLen; i++)
        {
            res = xQueueSend( xSerialRxQueue, &pBuf[i], SERIAL_NO_BLOCK );
            if( (pdPASS != res) || ( CLI_CMD_END_TAG == pBuf[i] ))
        	{
                xSemaphoreGive(xSemphrCLICommandEndFlag);
                SERIAL_INFO("xSemaphoreGive");
        	}
        }
        
        vTaskDelay(10);
	}
}

int app_serial_init(void)
{
    vSemaphoreCreateBinary( xSemphrCLICommandEndFlag );
    
    xSerialRxQueue = xQueueCreate( SERIAL_RX_QUEUE_LEN, 
        ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    
    while ((NULL  == xSemphrCLICommandEndFlag) \
            || (NULL == xSerialRxQueue))
    {
        // The resource created failed.
    }
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

