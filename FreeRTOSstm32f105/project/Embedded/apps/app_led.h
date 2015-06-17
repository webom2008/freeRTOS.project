/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : app_led.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : app_led.c header file
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __APP_LED_H__
#define __APP_LED_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum
{
    LED_CTRL_ON,
    LED_CTRL_OFF,
    LED_CTRL_PERIOD,
} LED_CTRL_TypeDef;

extern int app_led_init(void);
extern int app_led_start(void);
extern void app_led_ctl(LED_CTRL_TypeDef ctl, int period);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __APP_LED_H__ */
