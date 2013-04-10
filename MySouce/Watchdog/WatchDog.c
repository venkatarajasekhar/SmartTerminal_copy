#include "WatchDog.h"
#include "stm32f10x.h"

__IO uint32_t LsiFreq = 40000;

#define OverTime 2000//ms

void WatchDogInit(void)
{
	#ifndef ADDWatchDog
		#error "ADDWatchDog undefine,ADDWatchDog must be define and 1 or 0 ,orther was forbid"
	#else
		#if ADDWatchDog
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
		IWDG_SetPrescaler(IWDG_Prescaler_32); //0.8ms 
		IWDG_SetReload(OverTime/0.8);
		IWDG_ReloadCounter();
		IWDG_Enable();
		#endif
	#endif
}

void  FeedDog(void)
{
	#ifndef ADDWatchDog
		#error "ADDWatchDog undefine,ADDWatchDog must be define and 1 or 0 ,orther was forbid"
	#else
		#if ADDWatchDog
		IWDG_ReloadCounter();
		#endif
	#endif	
}
