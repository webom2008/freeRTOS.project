/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : i2c_it.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/3/2
  Last Modified :
  Description   : i2c_it.c header file
  Function List :
  History       :
  1.Date        : 2015/3/2
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __I2C_IT_H__
#define __I2C_IT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "stm32f10x.h"

extern int i2c_it_cfg(void);
extern void i2c_it_enable(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __I2C_IT_H__ */
