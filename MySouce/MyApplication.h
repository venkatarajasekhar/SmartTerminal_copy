#ifndef _MyApplication_H_BAB
#define _MyApplication_H_BAB

#include "rtthread.h"
#define MyApplication //不定义这个，将不会包含MyApplicationInit的调用

#define dRegisterFilePath		"/configure/register.dat"     	//注册信息文件

extern rt_mailbox_t registerMB;

void MyApplicationInit(void);

#endif

