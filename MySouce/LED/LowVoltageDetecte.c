#include "LowVoltageDetecte.h"

void LVD_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable the BUTTON Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure Button pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

u8 LVD_Pin(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);
}
