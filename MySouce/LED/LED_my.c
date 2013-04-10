
#include"LED_my.h"
/***************************************************************
*** Description: 初始化led的IO口
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/

static void rt_hw_led_init(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(led1_rcc|led2_rcc|led3_rcc|led4_rcc,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = led1_pin;
    GPIO_Init(led1_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = led2_pin;
    GPIO_Init(led2_gpio, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = led3_pin;
    GPIO_Init(led3_gpio, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = led4_pin;
    GPIO_Init(led4_gpio, &GPIO_InitStructure);
	
	rt_hw_led_off(1);
	rt_hw_led_off(2);
	rt_hw_led_off(3);
	rt_hw_led_off(4);
}

/***************************************************************
*** Description: turn on led 
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/

void rt_hw_led_on(rt_uint32_t n)
{
    switch (n)
    {
    case 1:
        GPIO_ResetBits(led1_gpio, led1_pin);
        break;
    case 2:
        GPIO_ResetBits(led2_gpio, led2_pin);
        break;
	case 3:
        GPIO_ResetBits(led3_gpio, led3_pin);
        break;
	case 4:
        GPIO_ResetBits(led4_gpio, led4_pin);
        break;
    default:
        break;
    }
}

/***************************************************************
*** Description: turn off led 
*** Author:  lindabell & 欧海
*** Date:
@n：1~4 那个led turn off
***************************************************************/

void rt_hw_led_off(rt_uint32_t n)
{
    switch (n)
    {
     case 1:
        GPIO_SetBits(led1_gpio, led1_pin);
        break;
    case 2:
        GPIO_SetBits(led2_gpio, led2_pin);
        break;
	case 3:
        GPIO_SetBits(led3_gpio, led3_pin);
        break;
	case 4:
        GPIO_SetBits(led4_gpio, led4_pin);
        break;
    default:
        break;
    }
}

/***************************************************************
*** Description: led线程的入口函数
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/
#include "beep.h"
#include "ManualAlarm.h"
#include "LowVoltageDetecte.h"
void led_thread_entery(void* parameter)
{

	rt_hw_led_init();
	ManualAlarm_init();
	LVD_init();
	while(1)  
	{
		if(ManualAlarm_IsInterrupt())
		{
			rt_hw_led_on(2);
			beep_On_time(2);
		}
		else rt_hw_led_off(2);
		
		if(LVD_Pin()==0)
		{
			beep_On_time(3);
		}
		
		rt_hw_led_on(3);
		rt_thread_delay( RT_TICK_PER_SECOND);
		
		rt_hw_led_off(3);
		rt_thread_delay( RT_TICK_PER_SECOND );	

	}
}


#ifdef RT_USING_FINSH
#include <finsh.h>
static rt_uint8_t led_inited = 0;
void led(rt_uint32_t led, rt_uint32_t value)
{
    /* init led configuration if it's not inited. */
    if (!led_inited)
    {
        rt_hw_led_init();
        led_inited = 1;
    }

    if ( led == 1 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(1);
            break;
        case 1:
            rt_hw_led_on(1);
            break;
        default:
            break;
        }
    }

    if ( led == 2 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(2);
            break;
        case 1:
            rt_hw_led_on(2);
            break;
        default:
            break;
        }
    }
	
	if ( led == 3 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(3);
            break;
        case 1:
            rt_hw_led_on(3);
            break;
        default:
            break;
        }
    }
	
	if ( led == 4 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(4);
            break;
        case 1:
            rt_hw_led_on(4);
            break;
        default:
            break;
        }
    }
}
FINSH_FUNCTION_EXPORT(led, set led[1 - 4] on[1] or off[0].)
#endif














