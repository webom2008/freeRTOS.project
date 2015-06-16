/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_uart.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/9
  Last Modified :
  Description   : uart driver
  Function List :
  History       :
  1.Date        : 2015/4/9
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#include "defines.h"
#include "queue.h"
#include "list.h"

#include "stm32f10x.h"
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
#define serINVALID_QUEUE		    ( ( QueueHandle_t ) 0 )
#define serNO_BLOCK			        ( ( TickType_t ) 0 )

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
void vUARTInterruptHandler( void );




//==============================================================================
#define BUF_OK          0
#define BUF_FULL        1
#define BUF_EMPTY       2
#define BUF_RX_NUM(x,y)    (x)>=(y)?(x-y):(BUFFER_RX_SIZE-(y-x))
#define BUFFER_RX_SIZE     128
#define BUFFER_TX_SIZE     512
typedef unsigned char   BUF_TYPE;

static BUF_TYPE uart_rx_buf[BUFFER_RX_SIZE];
static BUF_TYPE uart_tx_buf[BUFFER_TX_SIZE];

typedef struct _CIRCULARBUFFER
{
    BUF_TYPE *buf;
    int len;
    int head;
    int tail;

}CIRCULARBUFFER;

static CIRCULARBUFFER gCirCularRxBuffer;
static CIRCULARBUFFER *gpCirCularRxBuffer = &gCirCularRxBuffer;
static CIRCULARBUFFER gCirCularTxBuffer;
static CIRCULARBUFFER *gpCirCularTxBuffer = &gCirCularTxBuffer;

static int BufferInit(CIRCULARBUFFER *pDev,BUF_TYPE *buf)
{
    pDev->buf = buf;
    pDev->head = 0;
    pDev->tail = 0;
    pDev->len = 0;
    
    return 0;
}

static int isBufferFull(CIRCULARBUFFER *pDev, int buf_size)
{
    if((pDev->head+1)%buf_size == pDev->tail)
    {
        return BUF_FULL;
    }
    
    return BUF_OK;
}

static int isBufferEmpty(CIRCULARBUFFER *pDev)
{
    if(pDev->head != pDev->tail)
    {
        return BUF_OK;
    }
    return BUF_EMPTY;
}

static int BufferReInit(void)
{
    BufferInit(gpCirCularRxBuffer,uart_rx_buf);
    BufferInit(gpCirCularTxBuffer,uart_tx_buf);
    return 0;
}

static int BufferRxEnqueue(BUF_TYPE data)
{
    if(BUF_FULL == isBufferFull(gpCirCularRxBuffer, BUFFER_RX_SIZE))
    {
        return BUF_FULL;
    }
    
    gpCirCularRxBuffer->buf[gpCirCularRxBuffer->head] = data;
    gpCirCularRxBuffer->head = (gpCirCularRxBuffer->head+1)%BUFFER_RX_SIZE;

    return BUF_OK;
}

static int BufferRxDequeue(BUF_TYPE *pBuf)
{    
    if(BUF_EMPTY == isBufferEmpty(gpCirCularRxBuffer))
    {
        return BUF_EMPTY;
    }

    *pBuf = gpCirCularRxBuffer->buf[gpCirCularRxBuffer->tail];
    gpCirCularRxBuffer->tail = (gpCirCularRxBuffer->tail+1) % BUFFER_RX_SIZE;
        
    return BUF_OK;
}

static int BufferTxEnqueue(BUF_TYPE data)
{
    if(BUF_FULL == isBufferFull(gpCirCularTxBuffer, BUFFER_TX_SIZE))
    {
        return BUF_FULL;
    }
    
    gpCirCularTxBuffer->buf[gpCirCularTxBuffer->head] = data;
    gpCirCularTxBuffer->head = (gpCirCularTxBuffer->head+1)%BUFFER_TX_SIZE;

    return BUF_OK;
}

static int BufferTxDequeue(BUF_TYPE *pBuf)
{
    if(BUF_EMPTY == isBufferEmpty(gpCirCularTxBuffer))
    {
        return BUF_EMPTY;
    }

    *pBuf = gpCirCularTxBuffer->buf[gpCirCularTxBuffer->tail];
    gpCirCularTxBuffer->tail = (gpCirCularTxBuffer->tail+1) % BUFFER_TX_SIZE;
    return BUF_OK;
}
//==============================================================================

/******************************************************************************/
/*  UART API Start                                                         */
/******************************************************************************/

int uart_dev_getRxLen(void)
{    
    int len = 0;
    len = BUF_RX_NUM(gpCirCularRxBuffer->head,gpCirCularRxBuffer->tail);
    return len;
}

int uart_dev_getByte(u8* pValue)
{
    if( BUF_OK == BufferRxDequeue(pValue) )
    {
        return 1;
    }
    return 0;
}

int uart_dev_getBuf(u8 *buf)
{
    int len = 0;
    
    while(len < BUFFER_RX_SIZE)
    {
        if(BufferRxDequeue(&buf[len]) == BUF_OK)
        {
            len++;
        }
        else
        {
            break;
        }
    }
    return len;
}

void uart_dev_sendByte(const u8 u8Byte)
{
    BufferTxEnqueue(u8Byte);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}

void uart_dev_sendBuf(u8* u8Buf, const u16 u16Length)
{
    int i = 0;
    for(i=0; i<u16Length; i++)
    {
        BufferTxEnqueue(u8Buf[i]);
    }
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}
/******************************************************************************/
/*  UART API End                                                         */
/******************************************************************************/


/*****************************************************************************
 Prototype    : xSerialPortInitMinimal
 Description  : init for uart
 Input        : unsigned long ulWantedBaud
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/4/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
xComPortHandle xSerialPortInitMinimal(  unsigned long ulWantedBaud)
{
    xComPortHandle xReturn;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    BufferReInit();
    
	/* Enable USART1 clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1\
	                        | RCC_APB2Periph_GPIOA\
	                        | RCC_APB2Periph_AFIO, ENABLE );	

	/* Configure USART1 Rx (PA10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
	/* Configure USART1 Tx (PA9) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	USART_InitStructure.USART_BaudRate = ulWantedBaud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init( USART1, &USART_InitStructure );
	
	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPriority10Uart1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	
	USART_Cmd( USART1, ENABLE );
	xReturn = ( xComPortHandle ) 1;

	/* This demo file only supports a single port but we have to return
	something to comply with the standard demo header file. */
	return xReturn;
}

/*****************************************************************************
 Prototype    : xSerialGetChar
 Description  : get char from uart
 Input        : xComPortHandle pxPort    
                signed char *pcRxedChar  
                TickType_t xBlockTime    
 Output       : None
 Return Value : signed
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/4/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{
	/* The port handle is not required as this driver only supports one port. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
//	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
//	{
//		return pdTRUE;
//	}
//	else
	{
		return pdFALSE;
	}
}

/*****************************************************************************
 Prototype    : xSerialPutChar
 Description  : put char to uart
 Input        : xComPortHandle pxPort  
                signed char cOutChar   
                TickType_t xBlockTime  
 Output       : None
 Return Value : signed
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/4/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
    signed portBASE_TYPE xReturn;

//	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) == pdPASS )
//	{
//		xReturn = pdPASS;
//		USART_ITConfig( USART1, USART_IT_TXE, ENABLE );
//	}
//	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}

/*****************************************************************************
 Prototype    : vSerialClose
 Description  : close uart
 Input        : xComPortHandle xPort  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/4/9
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
}

/*****************************************************************************
 Prototype    : vUARTInterruptHandler
 Description  : uart1 interrupt handler
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
void vUARTInterruptHandler( void )
{
    int ret = 0;
    u8 temp = 0;
    
    if(RESET != USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        temp = USART_ReceiveData(USART1);
        BufferRxEnqueue(temp);
        if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(USART1, USART_FLAG_ORE);
            USART_ReceiveData(USART1);
        }
    }
  
    if(RESET != USART_GetITStatus(USART1, USART_IT_TXE))
    {
        ret = BufferTxDequeue(&temp);
        if(BUF_OK == ret)
        {
            USART_SendData(USART1, temp);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}

#if 0
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
static char strbuf[80];
int udprintf(const char* fmt, ...)
{
    va_list argptr;
    int nLen = 0;
    short res;
    
    memset(strbuf, 0, sizeof(strbuf));
    
    va_start(argptr, fmt);
//    vsnprintf(strbuf, sizeof(strbuf), fmt, argptr);
    va_end(argptr);
    
    nLen = strlen(strbuf);
    vSerialPutString(NULL, strbuf, nLen);
    return nLen;
}
#else

#include <stdio.h>

#pragma import(__use_no_semihosting) 

_sys_exit(int x) 
{ 
  x = x; 
}
 
struct __FILE 
{ 
  int handle; 
}; 

FILE __stdout;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{

#if 0 //wait mode
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART1, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
#else //interrupt mode
    uart_dev_sendByte(ch);
#endif
    return ch;
}
#endif
