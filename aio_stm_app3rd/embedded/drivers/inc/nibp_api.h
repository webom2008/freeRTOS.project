/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : nibp_api.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/6
  Last Modified :
  Description   : nibp_api.c header file
  Function List :
  History       :
  1.Date        : 2015/3/6
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __NIBP_API_H__
#define __NIBP_API_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "stm32f10x.h"

void    nibp_api_init(void);
u8      nibp_api_getPressReleState(void);
void    nibp_api_setPressRele(u8 state);
void    nibp_api_checkPressRele(void);
u8      nibp_api_isVerifying(void);
int     nibp_api_setSafePress(u8 *pBuf, u8 nLen);
int     nibp_api_getSafePress(u8 *pBuf, u8 nLen);
int     nibp_api_setVerify(u8 state);
void    nibp_api_printf(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NIBP_API_H__ */
