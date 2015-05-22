/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_serial.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : serial app
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
#include "driver_uart.h"

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
#define SERIAL_BOAUDRATE            ( (unsigned long) 230400 )
#define comSTACK_SIZE				configMINIMAL_STACK_SIZE

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
static xComPortHandle xPort = NULL;/* Handle to the com port used by both tasks. */


/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
#ifdef __COM_TEST_PRINTF__
/*****************************************************************************
 Prototype    : vComTxTask
 Description  : ComTxTask
 Input        : void *pvParameters  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/4/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static void vComTxTask( void *pvParameters )
{
    const char pBuf[30] = "\r\n>>vComTxTask running";
    unsigned char i, nLen;
    
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    nLen = strlen(pBuf);
    
	for( ;; )
	{
        for (i = 0; i < nLen; i++)
        {
            xSerialPutChar( xPort, pBuf[i], comNO_BLOCK );
        }
        vTaskDelay(500 / portTICK_PERIOD_MS); //delay 1s
	}
}

/*****************************************************************************
 Prototype    : vComRxTask
 Description  : ComRxTask
 Input        : void *pvParameters  
 Output       : None
 Return Value : static
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/4/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
static void vComRxTask(void *pvParameters )
{
    signed char cByteRxed;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	for( ;; )
	{
		/* Block on the queue that contains received bytes until a byte is
		available. */
		if( xSerialGetChar( xPort, &cByteRxed, comRX_BLOCK_TIME ) )
		{
            printf("\r\nvComRxTask input:%c",cByteRxed);
		}
        else // timeout
        {
            printf("\r\nvComRxTask timeout");
        }
	}
}
#endif

/*****************************************************************************
 Prototype    : app_serial_init
 Description  : init for App
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/4/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int app_serial_init(void)
{
	xSerialPortInitMinimal( SERIAL_BOAUDRATE);
    return 0;
}

/*****************************************************************************
 Prototype    : app_serial_start
 Description  : start task
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/4/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
int app_serial_start(void)
{
#ifdef __COM_TEST_PRINTF__
	xTaskCreate(    vComTxTask,
                    "COMTx",
                    comSTACK_SIZE,
                    NULL,
                    SERIAL_TEST_PRIORITY,
                    ( TaskHandle_t * ) NULL );
    
	xTaskCreate(    vComRxTask, "COMRx",
                    comSTACK_SIZE,
                    NULL,
                    SERIAL_TEST_PRIORITY,
                    ( TaskHandle_t * ) NULL );
#endif
    return 0;
}

