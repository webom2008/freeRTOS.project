/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_ecg.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/11
  Last Modified :
  Description   : driver_ecg.c header file
  Function List :
  History       :
  1.Date        : 2015/4/11
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DRIVER_ECG_H__
#define __DRIVER_ECG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "stm32f10x.h"

typedef enum
{
    LA_OFF = 0,
    RA_OFF,
    LL_OFF,
    RL_OFF,
    V1_OFF
} ECG_SelectTypeDef;

void ecg_dev_gpio_init(void);
u8 ecg_dev_setChannel(ECG_SelectTypeDef channel);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_ECG_H__ */
