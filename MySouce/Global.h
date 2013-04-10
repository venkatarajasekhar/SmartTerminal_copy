#ifndef _Global_H_BAB
#define _Global_H_BAB

#include "rtthread.h"

/**
GlobalEvent ֻ��һ���̷߳��ͺ����㣬�����߳�ֻ�ܽ��յ��������㣻��Ϊ֪ͨʹ��
*/
#define NET_CONNET_EVENT			(1<<0)		//�����Ѿ������¼�
#define AUTHENTCATED_EVENT			(1<<1)		//�Ѿ���Ȩ�ɹ��¼�
extern struct rt_event GlobalEvent;  //����״̬�¼�

/**
	GNSSЭ���¼�
*/
#define GNSS_RegisterACKEvent	(1<<0)
extern struct rt_event GNSSEvent;		

void Global_init(void);

#endif
