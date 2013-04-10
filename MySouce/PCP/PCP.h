#ifndef _PCP_H_BAB
#define _PCP_H_BAB

#include "rtthread.h"
#include "stm32f10x.h"


#define dType_infoTerminal					0x01	//��ҵ��Ϣ�ն˻�
#define dType_SchedulingScreen				0x02	//������ʾ��
#define dType_CarNavigation					0x03	//���ص�����ʾ��
#define dType_OilDetectInstrument			0x04	//���������
#define dType_AccelerateDetectInstrument	0x05	//���ٶȼ����
#define dType_BurglarAlarm					0x06	//����������
#define dType_InterfaceExtension			0x07	//�ӿ���չ��
#define dType_LoadDetectInstrument			0x08	//���ؼ����
#define dType_PassengerDetectInstrument		0x09	//���������
#define dType_GeneralSensor					0x0a	//ͨ�ô�����

#define dCmd_PowerOn		0x01	//�ϵ�ָʾ/Ӧ��
#define dCmd_LinkInquire	0x02	//��·̽ѯ/Ӧ��
#define dCmd_PowerControl	0x03	//�����Դ����
#define dCmd_Version		0x04	//��ѯ����汾�� 




#define dPCPuserDataSize 	0xff
#define dPCP_MAX_LEN 		(dPCPuserDataSize+8)

typedef struct  
{
	u8	Checksum;	//У���
	u16	Version;	//�汾
	u16	Company;	//����
	u8	Type;		//��������
	u8	CMD;		//����
	u8 userData[dPCPuserDataSize];//�û�����
	u8 size;
}PCPsrtuctTypedef;

#define dSendBufSize	(sizeof(PCPsrtuctTypedef)+2+10) 

extern rt_mutex_t PcpLockMutex;

rt_err_t PcpInit(void);
u16 PcpPackage(u8 *pOutBuf,const PCPsrtuctTypedef *pPCPsrtuct);
u8 PcpUnPackage(PCPsrtuctTypedef *pPCPsrtuct,u8 *pInBuf);

#endif

