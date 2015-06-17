/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_uart.c
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/9
  Last Modified :
  Description   : DMA1_Channel4 for UART1_Tx
                  DMA1_Channel5 for UART1_Rx
  Function List :
  History       :
  1.Date        : 2015/4/9
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
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define UART1_BAUD_RATE         230400
#define UART1_QUEUE_LEN         1024

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
static xSemaphoreHandle xSerialTxHandleLock = NULL;
static xSemaphoreHandle xSerialRxHandleLock = NULL;

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


//================== CIRCULARBUFFER defines Start ==============================
#define BUF_OK          0
#define BUF_FULL        1
#define BUF_EMPTY       2
#define BUF_RX_NUM(x,y)    (x)>=(y)?(x-y):(UART1_QUEUE_LEN-(y-x))

typedef unsigned char   BUF_TYPE;

static BUF_TYPE uart_rx_buf[UART1_QUEUE_LEN];
static BUF_TYPE uart_tx_buf[UART1_QUEUE_LEN];

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
    if(BUF_FULL == isBufferFull(gpCirCularRxBuffer, UART1_QUEUE_LEN))
    {
        return BUF_FULL;
    }
    
    gpCirCularRxBuffer->buf[gpCirCularRxBuffer->head] = data;
    gpCirCularRxBuffer->head = (gpCirCularRxBuffer->head+1)%UART1_QUEUE_LEN;
    return BUF_OK;
}

static int BufferRxDequeue(BUF_TYPE *pBuf)
{    
    if(BUF_EMPTY == isBufferEmpty(gpCirCularRxBuffer))
    {
        return BUF_EMPTY;
    }

    *pBuf = gpCirCularRxBuffer->buf[gpCirCularRxBuffer->tail];
    gpCirCularRxBuffer->tail = (gpCirCularRxBuffer->tail+1) % UART1_QUEUE_LEN;
        
    return BUF_OK;
}

static int BufferTxEnqueue(BUF_TYPE data)
{
    if(BUF_FULL == isBufferFull(gpCirCularTxBuffer, UART1_QUEUE_LEN))
    {
        return BUF_FULL;
    }
    
    gpCirCularTxBuffer->buf[gpCirCularTxBuffer->head] = data;
    gpCirCularTxBuffer->head = (gpCirCularTxBuffer->head+1)%UART1_QUEUE_LEN;

    return BUF_OK;
}

static int BufferTxDequeue(BUF_TYPE *pBuf)
{
    if(BUF_EMPTY == isBufferEmpty(gpCirCularTxBuffer))
    {
        return BUF_EMPTY;
    }

    *pBuf = gpCirCularTxBuffer->buf[gpCirCularTxBuffer->tail];
    gpCirCularTxBuffer->tail = (gpCirCularTxBuffer->tail+1) % UART1_QUEUE_LEN;
    return BUF_OK;
}
//================== CIRCULARBUFFER defines End ================================

//================== CIRCULARBUFFER API defines Start ==========================

static int xSerialGetRxLen(void)
{    
    int len = 0;
    len = BUF_RX_NUM(gpCirCularRxBuffer->head,gpCirCularRxBuffer->tail);
    return len;
}

int xSerialGetChar(u8* pValue)
{
    xSemaphoreTake( xSerialRxHandleLock, portMAX_DELAY );
    if( BUF_OK == BufferRxDequeue(pValue) )
    {
        xSemaphoreGive( xSerialRxHandleLock );
        return 1;
    }
    xSemaphoreGive( xSerialRxHandleLock );
    return 0;
}

int xSerialGetBuffer(u8 *pBuf, const int nByteLen)
{
    int len = 0;
    u8 *pData = pBuf;

    xSemaphoreTake( xSerialRxHandleLock, portMAX_DELAY );
    for (len = 0; len < nByteLen; len++)
    {
        if(BUF_EMPTY == BufferRxDequeue(pData++))
        {
            break;
        }
    }
    xSemaphoreGive( xSerialRxHandleLock );
    return len;
}

int xSerialPutChar(u8 cOutChar)
{
    int xReturn;
    
    xSemaphoreTake( xSerialTxHandleLock, portMAX_DELAY );
    
	if(BUF_OK == BufferTxEnqueue(cOutChar))
	{
		xReturn = 1;
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
	}
	else
	{
		xReturn = 0;
	}
    
    xSemaphoreGive( xSerialTxHandleLock );
    
	return xReturn;
}

int xSerialPutBuffer(const u8 *pBuf, const int nByteLen)
{
    int xReturn, i, nLen = 0;
    u8 data;
    const u8 *pData = pBuf;
    
    xSemaphoreTake( xSerialTxHandleLock, portMAX_DELAY );

    for (i = 0; i < nByteLen; i++)
    {
        if(BUF_FULL == BufferTxEnqueue(pData[i]))
        {
            // BUFFER FULL
            break;
        }
    }
    xReturn = i;
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    
    xSemaphoreGive( xSerialTxHandleLock );
	return xReturn;
}
//================== CIRCULARBUFFER API defines End ============================

static void Uart1InitGPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

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
}

static void Uart1InitConfig(unsigned long ulWantedBaud)
{
    USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = ulWantedBaud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init( USART1, &USART_InitStructure );
    
    USART_Cmd(USART1, ENABLE);
}

static void Uart1InitNVICConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
  
    // Enable the USART1 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPriority10Uart1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}


/*****************************************************************************
 Prototype    : Uart1Init
 Description  : init for uart
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
void Uart1Init(void)
{
    xSerialTxHandleLock = xSemaphoreCreateMutex();
    xSerialRxHandleLock = xSemaphoreCreateMutex();
	do{} while ((NULL == xSerialTxHandleLock)||(NULL == xSerialRxHandleLock));

	BufferReInit();    
    Uart1InitGPIO();
	Uart1InitConfig(UART1_BAUD_RATE);
    Uart1InitNVICConfig();
}

void USART1_IRQHandler(void)
{
    int ret = 0;
    u8 temp = 0;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        temp = USART_ReceiveData(USART1);
        BufferRxEnqueue(temp);
        if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
        {
            USART_ClearFlag(USART1, USART_FLAG_ORE);
            USART_ReceiveData(USART1);
        }
    }
  
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        ret = BufferTxDequeue(&temp);
        if(ret == BUF_OK)
        {
            USART_SendData(USART1, temp);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}

#if 1 // user define printf
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
static char strbuf[80];

int udprintf(const char* fmt, ...)
{
    va_list argptr;
    int nLen = 0;
    
    memset(strbuf, 0, sizeof(strbuf));
    
    va_start(argptr, fmt);
    vsnprintf(strbuf, sizeof(strbuf), fmt, argptr);
    va_end(argptr);
    
    nLen = strlen(strbuf);
    xSerialPutBuffer(strbuf, nLen);
    return nLen;
}
#else
int udprintf(const char* fmt, ...)
{
    return 0;
}
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
    xSerialPutChar(ch);
#endif
    return ch;
}
#endif

