#include "ManualAlarm.h"

static u8 ManualAlarmStatus=0;	//

void ManualAlarm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the BUTTON Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE| RCC_APB2Periph_AFIO, ENABLE);

	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOE, &GPIO_InitStructure);



	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);

	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;


	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  

	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure); 
}
/**
read the pin status
*/
u8 ManualAlarm_ReadPin(void)
{
	return GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);
}

/**
check the interrput hapen or not
*/
u8 ManualAlarm_IsInterrupt(void)
{
	if(ManualAlarmStatus)
	{
		ManualAlarmStatus=0;
		return 1;
	}
	else return 0;
}

/**
the Manual Alarm pin interrupt ISR
*/
void EXTI4_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
	ManualAlarmStatus=1;
    /* Clear the Key Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line4);
  }
}


