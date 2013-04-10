#ifndef _LED_my_h_
#define _LED_my_h_
//ÎÄ¼þ°üº¬
#include "stm32f10x.h"
#include <rtthread.h>

//led define
#define led1_rcc                    RCC_APB2Periph_GPIOB
#define led1_gpio                   GPIOB
#define led1_pin                    (GPIO_Pin_2)

#define led2_rcc                    RCC_APB2Periph_GPIOF
#define led2_gpio                   GPIOF
#define led2_pin                    (GPIO_Pin_11)

#define led3_rcc                    RCC_APB2Periph_GPIOF
#define led3_gpio                   GPIOF
#define led3_pin                    (GPIO_Pin_6)

#define led4_rcc                    RCC_APB2Periph_GPIOF
#define led4_gpio                   GPIOF
#define led4_pin                    (GPIO_Pin_7)


void rt_hw_led_on(rt_uint32_t n);
void rt_hw_led_off(rt_uint32_t n);

void led_thread_entery(void* parameter);



#endif
