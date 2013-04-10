#ifndef _AdscreenAPP_H_BAB
#define _AdscreenAPP_H_BAB

#include "rtthread.h"

void ADSApp_thread_entry(void* parameter);

void AD_Get_text(rt_uint8_t *pStr,rt_uint16_t len);

#endif
