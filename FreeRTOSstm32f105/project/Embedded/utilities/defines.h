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


#include "FreeRTOS.h"
#include "task.h"


//PRIORITY DEFINES
#define TEST_DRIVERS_TASK_PRIORITY  ( tskIDLE_PRIORITY + 1 )

/* QWB:20150411
 * 1.�߼����ȼ�����configMAX_SYSCALL_INTERRUPT_PRIORITY����ʹ��OS_API����,������ʹ��
 * 2.�߼����ȼ�configMAX_SYSCALL_INTERRUPT_PRIORITY��Ӳ�����ȼ��Ĺ�ϵ
 * a).FreeRTOS����Ӳ���ж�ֻ��4bitsǶ��ģʽ,û�����ж�
 * b).configMAX_SYSCALL_INTERRUPT_PRIORITY = 191 = 0xB0 ��4bits=0x0B=11
 * c).��Ҫ�õ�OS_API���жϺ���ֻ��ʹ�� 0~11��Щ�ж����ȼ��ҿ�Ƕ��,������ʹ��12~15�����ȼ�
 * d).ʹ��Ӳ���жϺ�Ϊ 12~15 ��,���ᱻ�ں��ӳ�,���ҿ�Ƕ��
 * e).��ʹ��OS_API��Ӳ���ж�,��������,ȫ���жϿ���ʹ�� 0 ~ 15
 */
#define KERNEL_INTERRUPT_PRIORITY_BASE              (u8)(configMAX_SYSCALL_INTERRUPT_PRIORITY>>4)

//lower number, higher priority
//1.The follow Priority can't use OS_API 
#define IRQPriority12DMA1ch4         (KERNEL_INTERRUPT_PRIORITY_BASE + 1) //12
#define IRQPriority13DMA1ch5         (KERNEL_INTERRUPT_PRIORITY_BASE + 2) //13
#define IRQPriority14unDefine        (KERNEL_INTERRUPT_PRIORITY_BASE + 3) //14
#define IRQPriority15unDefine        (KERNEL_INTERRUPT_PRIORITY_BASE + 4) //15

//2.The follow Priority can use OS_API
#define IRQPriority11unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE)     //11
#define IRQPriority10Uart1          (KERNEL_INTERRUPT_PRIORITY_BASE - 1) //10
#define IRQPriority09unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 2) //9
#define IRQPriority08unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 3) //8
#define IRQPriority07unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 4) //7
#define IRQPriority06unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 5) //6
#define IRQPriority05unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 6) //5
#define IRQPriority04unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 7) //4
#define IRQPriority03unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 8) //3
#define IRQPriority02unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 9) //2
#define IRQPriority01unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 10) //1
#define IRQPriority00unDefine       (KERNEL_INTERRUPT_PRIORITY_BASE - 11) //0




//ASCLL
//0x0A - LF (NL line feed, new line)
//0x0D - CR (carriage return)
#define CLI_CMD_END_TAG             (u8)(0x0D) //



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DEFINES_H__ */
