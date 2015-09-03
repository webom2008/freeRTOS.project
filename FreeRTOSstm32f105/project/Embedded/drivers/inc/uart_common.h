/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : uart_common.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/8/26
  Last Modified :
  Description   : uart_common.c header file
  Function List :
  History       :
  1.Date        : 2015/8/26
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __UART_COMMON_H__
#define __UART_COMMON_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdbool.h>
#include "stm32f10x.h"

typedef enum
{
    UART_INTERRUPT_MODE = 0,
    UART_DMA_MODE
} UART_WORK_MODE_DEF;

typedef enum
{
    UART_NUM01 = 0,
    UART_NUM02,
    UART_NUM03,
    UART_NUM04,
    UART_NUM05,

    UART_NUM_MAX
} UART_NUM_DEF;


typedef enum
{
    B9600   = 9600u,
    B19200  = 19200u,
    B115200 = 115200u,
    B230400 = 230400u
} UART_BAUDRATE_DEF;


typedef struct _UART_DMA_BUFFER_DEF_
{
    uint8_t     *pBuffer;
    uint32_t    bytes;
} UART_DMA_BUFFER_TypeDef;

typedef struct _UART_DEVICE_DEF_
{
    UART_NUM_DEF num;
    UART_WORK_MODE_DEF mode;
    UART_BAUDRATE_DEF baudrate;
    UART_DMA_BUFFER_TypeDef *pTxDMABuffer;
    UART_DMA_BUFFER_TypeDef *pRxDMABuffer;
    bool IsDeviceOpen;
    uint8_t IRQPriority;
} UART_DEVICE_TypeDef;



int UartDeviceDefaultInit(UART_DEVICE_TypeDef *pUartDevice);

int UartCommonInit(const UART_DEVICE_TypeDef *pUartDevice);

int UartCommonTerminate(const UART_DEVICE_TypeDef *pUartDevice);
int udprintf(const char* fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __UART_COMMON_H__ */

