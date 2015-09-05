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
} GP_FIFO_StructDef;

typedef struct
{
    int *write_ptr;      // дָ��
    int *read_ptr;       // ��ָ��
    int *buffer;         // ָ��FIFO��Ԫ�ص�ַ
    u32 fifo_size;      // FIFO��С(ints)
    char fifo_empty;    // FIFO�ձ�־
    char fifo_full;     // FIFO����־
} INT_FIFO_StructDef;

typedef struct
{
    char *write_ptr;      // дָ��
    char *read_ptr;       // ��ָ��
    char *buffer;         // ָ��FIFO��Ԫ�ص�ַ
    u32 fifo_size;      // FIFO��С(bytes)
    char fifo_empty;    // FIFO�ձ�־
    char fifo_full;     // FIFO����־
} CHAR_FIFO_StructDef;

//genery purpuse fifo
FIFO_EXT void gp_fifo_init( GP_FIFO_StructDef *fifo_ptr,
                            void *buffer_ptr,
                            u32 data_size,
                            u32 fifo_size );
FIFO_EXT int gp_fifo_push(GP_FIFO_StructDef *fifo_ptr, void *buffer_ptr);
FIFO_EXT int gp_fifo_pop(GP_FIFO_StructDef* fifo_ptr, void* buffer_ptr);
FIFO_EXT u32 gp_fifo_count(GP_FIFO_StructDef *fifo_ptr);
FIFO_EXT void gp_fifo_reset(GP_FIFO_StructDef *fifo_ptr);


FIFO_EXT void int_fifo_init( INT_FIFO_StructDef *fifo_ptr, int *buffer_ptr, u32 fifo_size );
FIFO_EXT int int_fifo_push(INT_FIFO_StructDef *fifo_ptr, int *buffer_ptr);
FIFO_EXT int int_fifo_pop(INT_FIFO_StructDef* fifo_ptr, int* buffer_ptr);
FIFO_EXT u32 int_fifo_count(INT_FIFO_StructDef *fifo_ptr);
FIFO_EXT void int_fifo_reset(INT_FIFO_StructDef *fifo_ptr);


FIFO_EXT void char_fifo_init( CHAR_FIFO_StructDef *fifo_ptr, char *buffer_ptr, u32 fifo_size );
FIFO_EXT int char_fifo_push(CHAR_FIFO_StructDef *fifo_ptr, char *buffer_ptr);
FIFO_EXT int char_fifo_pop(CHAR_FIFO_StructDef* fifo_ptr, char* buffer_ptr);
FIFO_EXT u32 char_fifo_count(CHAR_FIFO_StructDef *fifo_ptr);
FIFO_EXT void char_fifo_reset(CHAR_FIFO_StructDef *fifo_ptr);

#ifdef __cplusplus
}
#endif

#endif /* FIFO_H_ */

