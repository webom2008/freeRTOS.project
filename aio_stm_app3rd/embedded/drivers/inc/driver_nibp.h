/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : driver_nibp.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/15
  Last Modified :
  Description   : driver_nibp.c header file
  Function List :
  History       :
  1.Date        : 2015/4/15
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DRIVER_NIBP_H__
#define __DRIVER_NIBP_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

void nibp_dev_gpio_init(void);
void nibp_dev_setPressRelePin(char action);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRIVER_NIBP_H__ */
