#ifndef _SPI_H_BAB
#define _SPI_H_BAB

#include "FM1702_IO.h"
#include "STC12C5A60S2.h"

void SPI_init(void);
u8 SPI_WriteByte(u8 dat);

#endif
