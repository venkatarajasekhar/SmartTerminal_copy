#ifndef _LCD_drive_BAB
#define _LCD_drive_BAB

#include "rtthread.h"

#define dLCD_SEND  	1
#define dCMD_READ_PCP	2

extern struct rt_device LCDDevice;

void LCD_Register(void);


#endif
