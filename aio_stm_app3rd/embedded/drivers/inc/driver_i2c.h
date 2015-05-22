/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_i2c.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/2/16
  Last Modified :
  Description   : i2c_device.c header file
  Function List :
  History       :
  1.Date        : 2015/2/16
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DRIVER_I2C_H__
#define __DRIVER_I2C_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "stm32f10x.h"

#define AioI2C                      I2C1

int i2c_dev_init(I2C_TypeDef *pDev);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_I2C_H__ */
