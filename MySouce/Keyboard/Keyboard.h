#ifndef _Keyboard_H_BAB
#define _Keyboard_H_BAB

#include "stm32f10x.h"
#include <rtthread.h> 

void Keyboard_Register(void);

void Keyboard_thread_entery(void* parameter);

#endif
