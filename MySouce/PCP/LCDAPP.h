#ifndef _LCDAPP_H_BAB
#define _LCDAPP_H_BAB

#include "rtthread.h"

extern rt_event_t 	LCD_Event;

void LCDApp_thread_entry(void* parameter);

void LCD_Get_text(rt_uint8_t type,rt_uint8_t *pStr,rt_uint16_t len);

#endif
