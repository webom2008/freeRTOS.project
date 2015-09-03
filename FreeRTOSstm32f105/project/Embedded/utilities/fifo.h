/*
 * fifo.h
 *
 *  Created on: 2014-09-25
 *      Author: QiuWeibo
 */

#ifndef FIFO_H_
#define FIFO_H_

#ifdef __cplusplus
 extern "C" {
#endif 


#ifdef FIFO_GLOBALS
#define FIFO_EXT
#else
#define FIFO_EXT    extern
#endif

#include "stm32f10x.h"

typedef struct
{
    u8 *write_ptr;      // дָ��
    u8 *read_ptr;       // ��ָ��
    u8 *buffer;         // ָ��FIFO��Ԫ�ص�ַ
    u32 data_size;      // FIFOԪ�صĴ�С
    u32 fifo_size;      // FIFO��С
    char fifo_empty;    // FIFO�ձ�־
    char fifo_full;     // FIFO����־
} FIFO_StructDef;

FIFO_EXT void gp_fifo_init( FIFO_StructDef *fifo_ptr,
                            void *buffer_ptr,
                            u32 data_size,
                            u32 fifo_size );
FIFO_EXT int gp_fifo_push(FIFO_StructDef *fifo_ptr, void *buffer_ptr);
FIFO_EXT int gp_fifo_pop(FIFO_StructDef* fifo_ptr, void* buffer_ptr);
FIFO_EXT u32 gp_fifo_count(FIFO_StructDef *fifo_ptr);
FIFO_EXT void gp_fifo_reset(FIFO_StructDef *fifo_ptr);

#ifdef __cplusplus
}
#endif

#endif /* FIFO_H_ */

