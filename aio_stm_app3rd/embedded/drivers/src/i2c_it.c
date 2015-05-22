/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : i2c_it.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/2
  Last Modified :
  Description   : i2c interrupt handle
  Function List :
  History       :
  1.Date        : 2015/3/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#include <stdio.h>

#include "defines.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

#include "driver_i2c.h"
#include "i2c_it.h"

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
#define AioI2C_EV_IRQn          I2C1_EV_IRQn
#define AioI2C_ER_IRQn          I2C1_ER_IRQn
#define AIOI2C_BUFFER_LEN       (32)
#define serINVALID_QUEUE        ( ( QueueHandle_t ) 0 )

//#define I2C_IT_INFO
#ifdef I2C_IT_INFO
#define I2C_IT_DBG(fmt, arg...) printf("\r\n[I2C_IT] "fmt, ##arg)
#else
#define I2C_IT_DBG(fmt, arg...)
#endif



/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/


/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
QueueHandle_t xI2CTxedChars = NULL;
QueueHandle_t xI2CRxedChars = NULL;
SemaphoreHandle_t xSemaphoreI2CStop = NULL;

static const u8 u8I2CBufferLenMax = 20;
static u8 u8I2CRxBuffer[u8I2CBufferLenMax];
static u8 u8I2CRxBufferLen = 0;

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
int i2c_it_cfg(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
	xI2CRxedChars = xQueueCreate( AIOI2C_BUFFER_LEN, ( unsigned portBASE_TYPE ) sizeof(unsigned char));
	xI2CTxedChars = xQueueCreate( AIOI2C_BUFFER_LEN, ( unsigned portBASE_TYPE ) sizeof(unsigned char));
    vSemaphoreCreateBinary(xSemaphoreI2CStop);
    
    if ((serINVALID_QUEUE == xI2CRxedChars) \
        ||(serINVALID_QUEUE == xI2CTxedChars) \
        || (NULL == xSemaphoreI2CStop))
    {
        //no malloc space
        return -1;
    }
    
    /* Configure and enable I2Cx event interrupt -------------------------------*/
    NVIC_InitStructure.NVIC_IRQChannel = AioI2C_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = AioI2C_EV_IRQPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    /* Configure and enable I2Cx error interrupt -------------------------------*/
    NVIC_InitStructure.NVIC_IRQChannel = AioI2C_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = AioI2C_ER_IRQPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    return 0;
}

void i2c_it_enable(void)
{
    I2C_ITConfig(AioI2C, I2C_IT_BUF|I2C_IT_EVT, ENABLE);
    I2C_ITConfig(AioI2C, I2C_IT_ERR, ENABLE);
}

void vI2C1_EV_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    volatile uint32_t event;
    uint32_t tmp_reg;
    unsigned char cChar;
    u8 i;

//    printf("\r\nI2C_EV");
    event = I2C_GetLastEvent(AioI2C);
    switch (event)
    {
/* Slave Transmitter ---------------------------------------------------*/
    case I2C_EVENT_SLAVE_BYTE_TRANSMITTED:
    case I2C_EVENT_SLAVE_BYTE_TRANSMITTING:             /* EV3 */
        {
    		xQueueReceiveFromISR( xI2CTxedChars, &cChar, &xHigherPriorityTaskWoken );
            I2C_SendData(AioI2C,cChar);
            break;
        }

/* Slave Receiver ------------------------------------------------------*/
    case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:     /* EV1 */
    case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:
        {
            /*
            *   - ADDR (Address sent) is cleared by software sequence: a read operation to 
            *     I2C_SR1 register (I2C_GetITStatus()) followed by a read operation to 
            *     I2C_SR2 register ((void)(I2Cx->SR2)).
            */
            tmp_reg = AioI2C->SR2;
            tmp_reg = tmp_reg; //not used,because of warning.
            I2C_IT_DBG("ADDR->");
            break;
        }

    case I2C_EVENT_SLAVE_BYTE_RECEIVED:                /* EV2 */
        {
            cChar = I2C_ReceiveData(AioI2C);
            if(u8I2CRxBufferLen < u8I2CBufferLenMax) u8I2CRxBuffer[u8I2CRxBufferLen++] = cChar;
            break;
        }

    case I2C_EVENT_SLAVE_STOP_DETECTED:                /* EV4 */
        {
            /*
            *   - STOPF (STOP detection) is cleared by software sequence: a read operation 
            *     to I2C_SR1 register (I2C_GetITStatus()) followed by a write operation to 
            *     I2C_CR1 register (I2C_Cmd() to re-enable the I2C peripheral).
            */
            I2C_Cmd(AioI2C, ENABLE);
            for (i = 0; i < u8I2CRxBufferLen; i++)
            {
                xQueueSendFromISR( xI2CRxedChars, &u8I2CRxBuffer[i], &xHigherPriorityTaskWoken );
            }
            xSemaphoreGiveFromISR(xSemaphoreI2CStop,&xHigherPriorityTaskWoken);
            break;
        }

    default:
        {
            I2C_IT_DBG("0x%08X!",event);
            break;
        }
    }
    
    if (SET == I2C_GetITStatus(AioI2C,I2C_IT_BTF))
    {
/*
*   - BTF (Byte Transfer Finished) is cleared by software sequence: a read 
*     operation to I2C_SR1 register (I2C_GetITStatus()) followed by a 
*     read/write to I2C_DR register (I2C_SendData()).
*/
        I2C_IT_DBG("BTF->");
        //I2C_ReceiveData(AioI2C);
    }
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void vI2C1_ER_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    
//    printf("\r\nI2C_ER");
    if (SET == I2C_GetITStatus(AioI2C,I2C_IT_AF))
    {
        I2C_IT_DBG("I2C_IT_AF");
        I2C_ClearITPendingBit(AioI2C,I2C_IT_AF);
        u8I2CRxBufferLen = 0;
    }
    if (SET == I2C_GetITStatus(AioI2C,I2C_IT_BERR))
    {
        I2C_IT_DBG("I2C_IT_BERR");
        I2C_ClearITPendingBit(AioI2C,I2C_IT_BERR);
    }
    if (SET == I2C_GetITStatus(AioI2C,I2C_IT_ARLO))
    {
        I2C_IT_DBG("I2C_IT_ARLO");
        I2C_ClearITPendingBit(AioI2C,I2C_IT_ARLO);
    }
    if (SET == I2C_GetITStatus(AioI2C,I2C_IT_OVR))
    {
        I2C_IT_DBG("I2C_IT_OVR");
        I2C_ClearITPendingBit(AioI2C,I2C_IT_OVR);
    }
    if (SET == I2C_GetITStatus(AioI2C,I2C_IT_PECERR))
    {
        I2C_IT_DBG("I2C_IT_PECERR");
        I2C_ClearITPendingBit(AioI2C,I2C_IT_PECERR);
    }
    if (SET == I2C_GetITStatus(AioI2C,I2C_IT_TIMEOUT))
    {
        I2C_IT_DBG("I2C_IT_TIMEOUT");
        I2C_ClearITPendingBit(AioI2C,I2C_IT_TIMEOUT);
    }
    if (SET == I2C_GetITStatus(AioI2C,I2C_IT_SMBALERT))
    {
        I2C_IT_DBG("I2C_IT_ALERT");
        I2C_ClearITPendingBit(AioI2C,I2C_IT_SMBALERT);
    }
    
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

