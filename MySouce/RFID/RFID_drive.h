#ifndef _RFID_drive_H_BAB
#define _RFID_drive_H_BAB

#include "rtthread.h"

#define RFID_ConfigISOType			1		//≈‰÷√ø®¿‡–Õ
#define RFID_FindCard				2		//—∞ø®
#define RFID_Anticoll				3		//∑¿≥Â◊≤
#define RFID_SelCard				4		//Select card—°ø®
#define RFID_LoadPasswordInFiFO		5		//load password into FIFO
#define RFID_Authenticate_A			6		//Authenticate A √‹¬Î
#define RFID_Authenticate_B			7		////Authenticate B √‹¬Î

void RFID_Register(void);

#endif
