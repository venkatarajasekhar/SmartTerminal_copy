#ifndef _ManualAlarm_H_BAB
#define _ManualAlarm_H_BAB

#include "stm32f10x.h"

void ManualAlarm_init(void);
u8 ManualAlarm_ReadPin(void);
u8 ManualAlarm_IsInterrupt(void);

void EXTI4_IRQHandler(void);

#endif
