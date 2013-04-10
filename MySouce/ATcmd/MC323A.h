#ifndef _MC323A_H_BAB
#define _MC323A_H_BAB

#include "stm32f10x.h"
#include "rtthread.h"

#define MC323A_NET_ENABLE	0x01
#define MC323A_NET_DISABLE	0x00

#define MC323A_CMD_NET_STATE 0x01

extern u8 MC323A_state;
extern struct rt_device MC323A_Dev;

void MC323A_register(void);

s8 MC323A_GetDataStream(u8 Data);

#endif
