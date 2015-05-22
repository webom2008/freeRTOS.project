/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_led.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : driver_led.c header file
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DRIVER_LED_H__
#define __DRIVER_LED_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum 
{
  LED1 = 0,
  LEDn
} Led_TypeDef;

void led_dev_init(Led_TypeDef Led);
void led_dev_on(Led_TypeDef Led);
void led_dev_off(Led_TypeDef Led);
void led_dev_toggle(Led_TypeDef Led);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_LED_H__ */
