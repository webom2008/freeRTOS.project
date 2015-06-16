/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_uart.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/9
  Last Modified :
  Description   : driver_uart.c header file
  Function List :
  History       :
  1.Date        : 2015/4/9
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DRIVER_UART_H__
#define __DRIVER_UART_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "portmacro.h"

#define UART_NO_BLOCK	    ( ( TickType_t ) 0 )
#define UART_BLOCK_TIME	    ( ( TickType_t ) 0xffff )

void Uart1Init(void);
signed portBASE_TYPE xSerialGetChar(u8 *pcRxedChar, TickType_t xBlockTime );
signed portBASE_TYPE xSerialPutChar(u8 cOutChar);
signed portBASE_TYPE xSerialPutBuffer(const u8 *pBuf, const int nByteLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_UART_H__ */
