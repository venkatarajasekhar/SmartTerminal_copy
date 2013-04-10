#ifndef _ATcommand_H_BAB
#define _ATcommand_H_BAB

#include <rtthread.h>  
#include "stm32f10x.h"

extern rt_mailbox_t ATMailbox;	//ÓÊÏä
extern rt_event_t CDMAReadyEvent;		//CDMA¾ÍÐ÷ÊÂ¼þ

struct ReceiveATcmdBufTypdef
{
	u8 Str[100];
	u8 Ready;
	struct ReceiveATcmdBufTypdef *next;
};


extern struct ReceiveATcmdBufTypdef *pWriteReceiveATcmdBuf;

struct ReceiveATcmdBufTypdef * copyString(u8 Len,u8 *pStr);


void ATcommand_thread_entery(void* parameter);

void initATcmdBase(void);
#endif
