#ifndef _Global_H_BAB
#define _Global_H_BAB

#include "rtthread.h"

/**
GlobalEvent 只能一个线程发送和清零，其余线程只能接收但不能清零；作为通知使用
*/
#define NET_CONNET_EVENT			(1<<0)		//网络已经连接事件
#define AUTHENTCATED_EVENT			(1<<1)		//已经鉴权成功事件
extern struct rt_event GlobalEvent;  //网络状态事件

/**
	GNSS协议事件
*/
#define GNSS_RegisterACKEvent	(1<<0)
extern struct rt_event GNSSEvent;		

void Global_init(void);

#endif
