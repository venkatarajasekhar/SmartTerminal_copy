#ifndef _GNSS_IO_H_BAB
#define _GNSS_IO_H_BAB

#include "GNSS_BaseType.h"

void GNSS_IO_open(void);
void GNSS_IO_close(void);

u16 GNSS_Send(void *buff,u16 size);


#endif

