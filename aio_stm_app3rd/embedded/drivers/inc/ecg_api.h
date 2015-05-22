/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : ecg_api.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/6
  Last Modified :
  Description   : ecg_api.c header file
  Function List :
  History       :
  1.Date        : 2015/3/6
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __ECG_API_H__
#define __ECG_API_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include "stm32f10x.h"

void ecg_api_init(void);
u8 ecg_api_setNextChannel(void);
u8 ecg_api_saveDetectData(void);
void ecg_api_updateProbeStatus(void);
void ecg_api_printf(void);
int ecg_api_getProbe(u8 *pVal);
u8 ecg_api_getProbeMode(void);
void ecg_api_setProbeMode(u8 mode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ECG_API_H__ */
