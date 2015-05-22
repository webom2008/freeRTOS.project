/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : includes.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : includes.h header file
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

#include "stm32f10x.h"
#include "../apps/app_led.h"
#include "../apps/app_serial.h"
#include "../apps/app_interAdc.h"
#include "../apps/app_i2c.h"
#include "../apps/app_ecg.h"
#include "../apps/app_nibp.h"
#include "../apps/app_power.h"


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __INCLUDES_H__ */

