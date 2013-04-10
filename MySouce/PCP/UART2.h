#ifndef _UART2_H_BAB
#define _UART2_H_BAB

#include "rtthread.h"
#include "stm32f10x.h"

void UART2_Register(void);
void Uart2App_thread_entry(void* parameter);
#endif
