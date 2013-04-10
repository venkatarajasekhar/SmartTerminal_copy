#ifndef _CDMAHardware_H_BAB
#define _CDMAHardware_H_BAB

#include <rtthread.h>  
#include "stm32f10x.h"

#define GPRS_POWERON_PIN(x) GPIO_WriteBit(GPIOD,GPIO_Pin_7,x)	 //GPRS 上电引脚
#define GPRS_RESET_PIN(x)	GPIO_WriteBit(GPIOD,GPIO_Pin_6,x)	 //GPRS 复位引脚

extern struct rt_device CDMAdevice;
rt_err_t rt_hw_CDMA_register(rt_device_t dev, const char* name, rt_uint32_t flag);
#endif
