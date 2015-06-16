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
#include <string.h>

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
#define UART1_BAUD_RATE         230400
#define UART1_DATA_REG          (uint32_t)0x40013804
#define UART1_DMA_TX_BUF_LEN    256
#define UART1_DMA_RX_BUF_LEN    256
#define UART1_QUEUE_LEN         1024

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal variables                           *
 *----------------------------------------------*/
static u8 USART1_SEND_DATA[UART1_DMA_TX_BUF_LEN];
static u8 USART1_RECEIVE_DATA[UART1_DMA_RX_BUF_LEN];
static volatile u8 USART1_TX_Finish = 1;
static QueueHandle_t xRxedChars = NULL;
static QueueHandle_t xCharsForTx = NULL;


/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

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
    
    //enable idle interrupt
    USART_ITConfig(USART1, USART_IT_IDLE , ENABLE);
    //
    USART_Cmd(USART1, ENABLE);

    //fixed CPU bugs: if send data after uart config, the first char don't send.
    USART_ClearFlag(USART1, USART_FLAG_TC);
}

static void Uart1InitDMAConfig(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    /* DMA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//DMA1
    
    /* DMA1 Channel4 (triggered by USART1 Tx event) Config */
    DMA_DeInit(DMA1_Channel4);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = UART1_DATA_REG;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART1_SEND_DATA;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = UART1_DMA_TX_BUF_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TE, ENABLE);
    /* Enable USART1 DMA TX request */
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel4, DISABLE);
    
    /* DMA1 Channel5 (triggered by USART1 Rx event) Config */
    DMA_DeInit(DMA1_Channel5);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = UART1_DATA_REG;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART1_RECEIVE_DATA;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = UART1_DMA_RX_BUF_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TE, ENABLE);

    /* Enable USART1 DMA RX request */
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
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

    //Enable DMA Channel4 Interrupt 
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPriority12DMA1ch4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable DMA Channel5 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPriority13DMA1ch5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
	/* Create the queues used to hold Rx/Tx characters. */
	xRxedChars  = xQueueCreate( UART1_QUEUE_LEN, ( unsigned portBASE_TYPE ) sizeof( u8 ) );
	xCharsForTx = xQueueCreate( UART1_QUEUE_LEN, ( unsigned portBASE_TYPE ) sizeof( u8 ) );

    // Queue was not created and must not be used.
	do{}while((NULL == xRxedChars) || (NULL == xCharsForTx));
    
    Uart1InitGPIO();
    Uart1InitNVICConfig();
    Uart1InitDMAConfig();
	Uart1InitConfig(UART1_BAUD_RATE);
}

static signed portBASE_TYPE xSerialRxHandleFromISR(u8 *pBuf, const int nByteLen)
{
    int i = 0;

    for (i = 0; i < nByteLen; i++)
    {
    	if( xQueueSendFromISR( xRxedChars, pBuf, UART_NO_BLOCK ) != pdPASS )
    	{
	        return pdFAIL;
    	}
        else
        {
            pBuf++;
        }
    }
	return pdPASS;
}

signed portBASE_TYPE xSerialGetChar(u8 *pcRxedChar, TickType_t xBlockTime )
{
	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}

signed portBASE_TYPE xSerialGetBuffer(u8 *pBuf, const int nByteLen, TickType_t xBlockTime)
{
    signed portBASE_TYPE i = 0;
    
    for (i = 0; i < nByteLen; i++)
    {
        if (pdTRUE != xSerialGetChar(pBuf, xBlockTime))
        {
            return i;
        }
        pBuf++;
    }
    return i;
}

signed portBASE_TYPE xSerialPutChar(u8 cOutChar)
{
    signed portBASE_TYPE xReturn;
    BaseType_t res;
    int nLen = 0;
    u8 data;

	if( pdPASS ==  xQueueSend( xCharsForTx, &cOutChar, UART_NO_BLOCK ))
	{
		xReturn = pdPASS;
        if (USART1_TX_Finish) //DMA had finish transfer
        {
            do
            {
                res = xQueueReceive( xCharsForTx, &data, UART_NO_BLOCK );
                if( pdPASS == res)
                {
                    USART1_SEND_DATA[nLen] = data;
                    nLen++;

                }
            } while( pdPASS == res);
            
            DMA_Cmd(DMA1_Channel4, DISABLE); //stop DMA before changing data count
            DMA1_Channel4->CNDTR = nLen; //update len of bytes
            USART1_TX_Finish = 0;//start DMA transfer set status flag
            DMA_Cmd(DMA1_Channel4, ENABLE);
        }
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}

signed portBASE_TYPE xSerialPutBuffer(const u8 *pBuf, const int nByteLen)
{
    signed portBASE_TYPE i = 0;
    
    for (i = 0; i < nByteLen; i++)
    {
        if (pdPASS != xSerialPutChar(pBuf[i]))
        {
            return i;
        }
    }
    return i; 
}

void USART1_IRQHandler(void)
{
    u16 DATA_LEN;
    u32 tem_reg;
    
    // uart idle interrupt
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        DMA_Cmd(DMA1_Channel5, DISABLE);                //close DMA incase receive data while handling
        DATA_LEN = UART1_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Channel5); 
        
        if(DATA_LEN > 0)
        {   
            // display return func
            //xSerialPutBuffer(USART1_RECEIVE_DATA, DATA_LEN);

            //push data in quene
            xSerialRxHandleFromISR(USART1_RECEIVE_DATA, DATA_LEN);
        }
        
        DMA_ClearFlag(DMA1_FLAG_GL5 | DMA1_FLAG_TC5 | DMA1_FLAG_TE5 | DMA1_FLAG_HT5);//clear status flag
        DMA1_Channel5->CNDTR = UART1_DMA_RX_BUF_LEN;    //restore count
        DMA_Cmd(DMA1_Channel5, ENABLE);                 //open DMA after handled
        
        //clear Idle flag by read SR and DR
        tem_reg = USART1->SR;
        tem_reg = USART1->DR;
        tem_reg = tem_reg; // slove warning 
    }

    // error happen
    if(USART_GetITStatus(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE);
    }
    
    USART_ClearITPendingBit(USART1, USART_IT_IDLE);
}

/*****************************************************************************
 Prototype    : DMA1_Channel5_IRQHandler
 Description  : DMA1_Channel5 for UART1_Rx
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/6/16
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
    DMA_ClearITPendingBit(DMA1_IT_TC5);
    DMA_ClearITPendingBit(DMA1_IT_TE5);
    DMA_Cmd(DMA1_Channel5, DISABLE);            //close DMA incase receive data while handling
    DMA1_Channel5->CNDTR = UART1_DMA_RX_BUF_LEN;//restore count
    DMA_Cmd(DMA1_Channel5, ENABLE);             //open DMA after handled
}

/*****************************************************************************
 Prototype    : DMA1_Channel4_IRQHandler
 Description  : DMA1_Channel4 for UART1_Tx
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2015/6/16
    Author       : qiuweibo
    Modification : Created function

*****************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
    DMA_ClearITPendingBit(DMA1_IT_TC4);
    DMA_ClearITPendingBit(DMA1_IT_TE4);
    DMA_Cmd(DMA1_Channel4, DISABLE);    // close DMA
    USART1_TX_Finish=1;                 // set finish flag
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
    xSerialPutChar(ch);
#endif
    return ch;
}
#endif
