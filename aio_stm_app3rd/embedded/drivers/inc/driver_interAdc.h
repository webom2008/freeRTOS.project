/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_interAdc.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/10
  Last Modified :
  Description   : driver_interAdc.c header file
  Function List :
  History       :
  1.Date        : 2015/4/10
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __DRIVER_INTERADC_H__
#define __DRIVER_INTERADC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define ADC_TOTAL_CH_USED          7

extern void InAdc_dev_complete(void);
extern void InAdc_dev_init(void);
extern void InAdc_dev_StarConvert(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_INTERADC_H__ */
