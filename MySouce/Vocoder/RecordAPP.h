#ifndef _RecordAPP_H_BAB
#define _RecordAPP_H_BAB

#include "rtthread.h"

#define Record_THREAD_STACK_SIZE 2048

extern struct rt_thread record_thread;					 //Encoder�߳�ָ��
extern rt_uint8_t record_stack[Record_THREAD_STACK_SIZE]; //Encoder��stack


void Record_thread_entery(void* parameter);

#endif
