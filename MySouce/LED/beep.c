#include "beep.h"

static void beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

static void beep_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

static void beep_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

//-----------------------------------------------------------------------------//

rt_sem_t  BeepSem;

/**
beep œÏ∂‡…Ÿ¥Œ
*/
void beep_On_time(rt_int16_t n)
{
	rt_int16_t i;
	RT_ASSERT(n<20)
	for(i=0;i<n;i++)rt_sem_release(BeepSem);
}

/**
The beep wait The sem
*/
void beep_thread_entery(void* parameter)
{
	beep_init();
	BeepSem=rt_sem_create("beep",1,RT_IPC_FLAG_FIFO);
	if(BeepSem==0)
	{
		rt_kprintf("init beep fail\n");
		return;
	}
	while(1)  
	{	
		rt_sem_take(BeepSem,RT_WAITING_FOREVER);

		beep_ON();
		rt_thread_delay(RT_TICK_PER_SECOND/10);
		beep_OFF();
		rt_thread_delay(RT_TICK_PER_SECOND/10);
	}
}
