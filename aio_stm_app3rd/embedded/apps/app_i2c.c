/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_i2c.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/10
  Last Modified :
  Description   : i2c app
  Function List :
  History       :
  1.Date        : 2015/4/10
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include "includes.h"
#include "driver_i2c.h"
#include "i2c_it.h"

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
extern QueueHandle_t xI2CTxedChars;
extern QueueHandle_t xI2CRxedChars;
extern SemaphoreHandle_t xSemaphoreI2CStop;
extern InternalADCResult *pgInternalADCResult;
/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

#define AIO_STM32_READ_MASK     (0x01)
#define AIO_STM32_WRITE_MASK    (0x00)

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/

//range at 0x00 to 0x7F

const u8 CID_POWER_STATUS           = 0x00;
const u8 CID_SELF_CHECK             = 0x01;
const u8 CID_ECG_PROBE_RESULT       = 0x02;
const u8 CID_PROTECT_PRESSURE       = 0x03;
const u8 CID_CURRENT_PRESSURE       = 0x04;
const u8 CID_SOFTWARE_UPDATE        = 0x05;
const u8 CID_NIBP_VERIFY            = 0x06;
const u8 CID_NIBP_PROTECT_STATE     = 0x07;
const u8 CID_RESP_OHM               = 0x08;
const u8 CID_ECG_PROBE_MODE         = 0x09;

typedef struct
{
    u8 u8PowerStatus;
    u8 u8SelfCheck;
    
    /*
     * detect reslut for probe fall off and overload
     * |-bit7-|-bit6-|-bit5-|-bit4-|-bit3-|-bit2-|-bit1-|-bit0-|
     * |ECGV  |ECG2  |ECG1  |V1_OFF|RL_OFF|LL_OFF|RA_OFF|LA_OFF|
     */
    u8 u8ecgReslut;
} I2CAutoUpdateVal_TypeDef;

static I2CAutoUpdateVal_TypeDef gI2CAutoUpdateVal;
static I2CAutoUpdateVal_TypeDef *pgI2CAutoUpdateVal = &gI2CAutoUpdateVal;

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/
static void app_i2c_txrx_task(void *pvParameters);
static void app_i2c_result_task(void *pvParameters);

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int app_i2c_init(void)
{
    return i2c_dev_init(AioI2C);
}


void i2c_api_readPktHandle(unsigned char id)
{
    u8 CID = id >> 1;
    u8 i, u8SendData[8] = {0};
    
//    printf("\r\nReadID=%02X",CID);
    switch(CID)
    {
    case CID_POWER_STATUS:
        u8SendData[0] = pgI2CAutoUpdateVal->u8PowerStatus;
		if( xQueueSend( xI2CTxedChars, ( void * ) u8SendData, ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
        break;
    case CID_SELF_CHECK:
        u8SendData[0] = pgI2CAutoUpdateVal->u8SelfCheck;
		if( xQueueSend( xI2CTxedChars, ( void * ) u8SendData, ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
        break;
    case CID_ECG_PROBE_RESULT:
        u8SendData[0] = pgI2CAutoUpdateVal->u8ecgReslut;
        u8SendData[1] = ecg_api_getProbeMode();
		if( xQueueSend( xI2CTxedChars, ( void * ) &u8SendData[0], ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
		if( xQueueSend( xI2CTxedChars, ( void * ) &u8SendData[1], ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
        break;
    case CID_CURRENT_PRESSURE:
        u8SendData[0] = (pgInternalADCResult->BBP >> 8) & 0xFF;
        u8SendData[1] = pgInternalADCResult->BBP & 0xFF;
		if( xQueueSend( xI2CTxedChars, ( void * ) &u8SendData[0], ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
		if( xQueueSend( xI2CTxedChars, ( void * ) &u8SendData[1], ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
        break;
    case CID_PROTECT_PRESSURE:
        nibp_api_getSafePress(u8SendData, 8);
        for (i=0;i<8;i++)
        {
            if(pdFALSE == xQueueSend( xI2CRxedChars, &u8SendData[i], ( TickType_t ) 10 ) )
            {
                return;
            }
        }
        break;
    case CID_NIBP_PROTECT_STATE:
        u8SendData[0] = nibp_api_getPressReleState();
		if( xQueueSend( xI2CTxedChars, ( void * ) &u8SendData[0], ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
        break;
    case CID_RESP_OHM:
        u8SendData[0] = (pgInternalADCResult->BASER >> 8) & 0xFF;
        u8SendData[1] = pgInternalADCResult->BASER & 0xFF;
		if( xQueueSend( xI2CTxedChars, ( void * ) &u8SendData[0], ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
		if( xQueueSend( xI2CTxedChars, ( void * ) &u8SendData[1], ( TickType_t ) 10 ) != pdPASS )
		{
			// Failed to post the message, even after 10 ticks.
		}
        break;
    case CID_ECG_PROBE_MODE:
        break;
    default:
        break;
    }
}

void i2c_api_writePktHandle(unsigned char id)
{
    u8 CID = id >> 1;
    u8 len = 0,i;
    u8 pBuf[8] = {0,};
    
    if(pdFALSE == xQueueReceive( xI2CRxedChars, &len, ( TickType_t ) 10 ) )
    {
        return;
    }
    
    if (0 == len) return;  //no data lenght
    
//    printf("\r\nWriteID=%02X",CID);
    switch(CID)
    {
    case CID_PROTECT_PRESSURE:
        for (i=0;i<8;i++)
        {
            if(pdFALSE == xQueueReceive( xI2CRxedChars, &pBuf[i], ( TickType_t ) 10 ) )
            {
                return;
            }
        }
        nibp_api_setSafePress(pBuf,8);
        break;
    case CID_POWER_STATUS:
        break;
    case CID_SELF_CHECK:
        break;
    case CID_ECG_PROBE_RESULT:
        break;
    case CID_SOFTWARE_UPDATE:
        break;
    case CID_NIBP_VERIFY:
        if(pdFALSE == xQueueReceive( xI2CRxedChars, pBuf, ( TickType_t ) 10 ) )
        {
            return;
        }
        nibp_api_setVerify(pBuf[0]);
        break;
    case CID_NIBP_PROTECT_STATE:
        if(pdFALSE == xQueueReceive( xI2CRxedChars, pBuf, ( TickType_t ) 10 ) )
        {
            return;
        }
        nibp_api_setPressRele(pBuf[0]);
        break;
    case CID_ECG_PROBE_MODE:
        if(pdFALSE == xQueueReceive( xI2CRxedChars, pBuf, ( TickType_t ) 10 ) )
        {
            return;
        }
        ecg_api_setProbeMode(pBuf[0]);
        break;
    default:
        break;
    }
}

int app_i2c_start(void)
{
    xTaskCreate(app_i2c_txrx_task,\
                "app_i2c_txrx_task",\
                100,\
                NULL,\
                I2C_TASK_PRIORITY,\
                NULL);
    xTaskCreate(app_i2c_result_task,\
                "app_i2c_result_task",\
                20,\
                NULL,\
                I2C_TASK_PRIORITY,\
                NULL);
    return 0;
}

static void app_i2c_txrx_task(void *pvParameters)
{
    unsigned char id = 0;
    unsigned portBASE_TYPE len;
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    //first take because of vSemaphoreCreateBinary() will give at create success
    //keep semaphore empty
    xSemaphoreTake(xSemaphoreI2CStop, ( TickType_t )0); 
    
    for (;;)
    {
        if ( pdTRUE == xSemaphoreTake(xSemaphoreI2CStop, portMAX_DELAY))
        {
            len = uxQueueMessagesWaiting(xI2CRxedChars);
//            printf("\r\nI2C:Rx=%d",len);
            
            if( xQueueReceive( xI2CRxedChars, &id, ( TickType_t ) 10 ) )
            {
                if (id & AIO_STM32_READ_MASK) //read operation
                {
                    i2c_api_readPktHandle(id);
                }
                else //write operation
                {
                    i2c_api_writePktHandle(id);
                }
            }
        }
    }
}

static void app_i2c_result_task(void *pvParameters)
{
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
    i2c_it_enable();
    for (;;)
    {
        pgI2CAutoUpdateVal->u8PowerStatus = power_getStatus();
        pgI2CAutoUpdateVal->u8SelfCheck = 0;//Sys_Self_Check_Status
        ecg_api_getProbe(&pgI2CAutoUpdateVal->u8ecgReslut);
        vTaskDelay(100 / portTICK_PERIOD_MS); //delay 1s
//        printf("\r\n>>i2c_i2c_result");
    }
}
