/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_interAdc.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/10
  Last Modified :
  Description   : app_interAdc.c header file
  Function List :
  History       :
  1.Date        : 2015/4/10
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __APP_INTERADC_H__
#define __APP_INTERADC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef struct
{
    uint32_t BBP;
    uint32_t D3V3;
    uint32_t VDDINT;
    uint32_t PROBE;
    uint32_t BASER;
    uint32_t D5V_N;
    uint32_t D5V_P;
} InternalADCResult;

extern int app_interAdc_init(void);
extern int app_interAdc_start(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __APP_INTERADC_H__ */
