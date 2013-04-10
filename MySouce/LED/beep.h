#ifndef _BEPP_H_BAB
#define _BEPP_H_BAB

#include "stm32f10x.h"
#include "rtthread.h"
void beep_init(void);
void beep_ON(void);
void beep_OFF(void);

extern rt_sem_t  BeepSem;

void beep_On_time(rt_int16_t n);

void beep_thread_entery(void* parameter);

#endif

