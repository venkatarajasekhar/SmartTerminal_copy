#ifndef _MyApplication_H_BAB
#define _MyApplication_H_BAB

#include "rtthread.h"
#define MyApplication //��������������������MyApplicationInit�ĵ���

#define dRegisterFilePath		"/configure/register.dat"     	//ע����Ϣ�ļ�

extern rt_mailbox_t registerMB;

void MyApplicationInit(void);

#endif

