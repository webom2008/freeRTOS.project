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

void Uart1Init(void);
int xSerialGetChar(u8 *pcRxedChar);
int xSerialGetBuffer(u8 *pBuf, const int nByteLen);
int xSerialPutChar(u8 cOutChar);
int xSerialPutBuffer(const u8 *pBuf, const int nByteLen);
int udprintf(const char* fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_UART_H__ */
