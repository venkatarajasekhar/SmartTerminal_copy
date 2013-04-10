#ifndef _RFID_drive_H_BAB
#define _RFID_drive_H_BAB

#include "rtthread.h"

#define RFID_ConfigISOType			1		//���ÿ�����
#define RFID_FindCard				2		//Ѱ��
#define RFID_Anticoll				3		//����ײ
#define RFID_SelCard				4		//Select cardѡ��
#define RFID_LoadPasswordInFiFO		5		//load password into FIFO
#define RFID_Authenticate_A			6		//Authenticate A ����
#define RFID_Authenticate_B			7		////Authenticate B ����

void RFID_Register(void);

#endif
