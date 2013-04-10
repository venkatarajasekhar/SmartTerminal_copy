#ifndef _CAN_H_BAB
#define _CAN_H_BAB

#include "stm32f10x.h"
#include <rtthread.h>

extern struct rt_device CANdevice;

rt_err_t CAN_init(rt_device_t dev);
void USB_LP_CAN1_RX0_IRQHandler(void);
#endif

