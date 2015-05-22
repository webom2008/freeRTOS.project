/******************************************************************************

  Copyright (C), 2005-2014, CVTE.

 ******************************************************************************
  File Name     : defines.h
  Version       : Initial Draft
  Author        : qiuweibo
  Created       : 2015/4/8
  Last Modified :
  Description   : defines.h header file
  Function List :
  History       :
  1.Date        : 2015/4/8
    Author      : qiuweibo
    Modification: Created file

******************************************************************************/
#ifndef __DEFINES_H__
#define __DEFINES_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

//#define __ECG_DEBUG_PRINTF__
//#define __ADC_DEBUG_PRINTF__
//#define __NIBP_DEBUG_PRINTF__


#include "FreeRTOS.h"
#include "task.h"


//PRIORITY DEFINES
#define PRINTF_DEBUG_TASK_PRIORITY  ( tskIDLE_PRIORITY + 1 )
#define LED_TASK_PRIORITY	        ( tskIDLE_PRIORITY + 2 )
#define I2C_TASK_PRIORITY	        ( tskIDLE_PRIORITY + 2 )
#define POWER_TASK_PRIORITY	        ( tskIDLE_PRIORITY + 3 )
#define ADC_TASK_PRIORITY	        ( tskIDLE_PRIORITY + 3 )
#define ECG_TASK_PRIORITY	        ( tskIDLE_PRIORITY + 3 )
#define NIBP_TASK_PRIORITY	        ( tskIDLE_PRIORITY + 4 )

/* QWB:20150411
 * 1.逻辑优先级低于configMAX_SYSCALL_INTERRUPT_PRIORITY可以使用OS_API函数,否则不能使用
 * 2.逻辑优先级configMAX_SYSCALL_INTERRUPT_PRIORITY与硬件优先级的关系
 * a).FreeRTOS配置硬件中断只用4bits嵌套模式,没有子中断
 * b).configMAX_SYSCALL_INTERRUPT_PRIORITY = 191 = 0xB0 高4bits=0x0B=11
 * c).需要用的OS_API的中断函数只能使用 0~11这些中断优先级且可嵌套,不可以使用12~15的优先级
 * d).使用硬件中断号为 12~15 的,不会被内核延迟,并且可嵌套
 * e).不使用OS_API的硬件中断,不受限制,全部中断可以使用 0 ~ 15
 */
#define KERNEL_INTERRUPT_PRIORITY_BASE              11 //base on configMAX_SYSCALL_INTERRUPT_PRIORITY

//lower number, higher priority
//1.The follow Priority can't use OS_API 
#define unDefineIRQPriority1        (KERNEL_INTERRUPT_PRIORITY_BASE + 1) //12
#define unDefineIRQPriority2        (KERNEL_INTERRUPT_PRIORITY_BASE + 2) //13
#define unDefineIRQPriority3        (KERNEL_INTERRUPT_PRIORITY_BASE + 3) //14
#define comInterruptPriority        (KERNEL_INTERRUPT_PRIORITY_BASE + 4) //15

//2.The follow Priority can use OS_API
#define unDefineIRQPriority4        (KERNEL_INTERRUPT_PRIORITY_BASE - 1) //10
#define AioI2C_EV_IRQPriority       (KERNEL_INTERRUPT_PRIORITY_BASE - 2) //9
#define AioI2C_ER_IRQPriority       (KERNEL_INTERRUPT_PRIORITY_BASE - 3) //8
#define unDefineIRQPriority5        (KERNEL_INTERRUPT_PRIORITY_BASE - 4) //7
#define unDefineIRQPriority6        (KERNEL_INTERRUPT_PRIORITY_BASE - 5) //6
#define unDefineIRQPriority7        (KERNEL_INTERRUPT_PRIORITY_BASE - 6) //5
#define unDefineIRQPriority8        (KERNEL_INTERRUPT_PRIORITY_BASE - 7) //4
#define unDefineIRQPriority9        (KERNEL_INTERRUPT_PRIORITY_BASE - 8) //3
#define unDefineIRQPriority10       (KERNEL_INTERRUPT_PRIORITY_BASE - 9) //2
#define unDefineIRQPriority11       (KERNEL_INTERRUPT_PRIORITY_BASE - 10) //1
#define unDefineIRQPriority12       (KERNEL_INTERRUPT_PRIORITY_BASE - 11) //0

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DEFINES_H__ */
