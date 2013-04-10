#ifndef _PCP_H_BAB
#define _PCP_H_BAB

#include "rtthread.h"
#include "stm32f10x.h"


#define dType_infoTerminal					0x01	//行业信息终端机
#define dType_SchedulingScreen				0x02	//调度显示屏
#define dType_CarNavigation					0x03	//车载导航显示屏
#define dType_OilDetectInstrument			0x04	//油量检测仪
#define dType_AccelerateDetectInstrument	0x05	//加速度检测仪
#define dType_BurglarAlarm					0x06	//防盗报警器
#define dType_InterfaceExtension			0x07	//接口扩展器
#define dType_LoadDetectInstrument			0x08	//载重检测器
#define dType_PassengerDetectInstrument		0x09	//客流检测器
#define dType_GeneralSensor					0x0a	//通用传感器

#define dCmd_PowerOn		0x01	//上电指示/应答
#define dCmd_LinkInquire	0x02	//链路探询/应答
#define dCmd_PowerControl	0x03	//外设电源控制
#define dCmd_Version		0x04	//查询外设版本号 




#define dPCPuserDataSize 	0xff
#define dPCP_MAX_LEN 		(dPCPuserDataSize+8)

typedef struct  
{
	u8	Checksum;	//校验和
	u16	Version;	//版本
	u16	Company;	//厂商
	u8	Type;		//外设类型
	u8	CMD;		//命令
	u8 userData[dPCPuserDataSize];//用户数据
	u8 size;
}PCPsrtuctTypedef;

#define dSendBufSize	(sizeof(PCPsrtuctTypedef)+2+10) 

extern rt_mutex_t PcpLockMutex;

rt_err_t PcpInit(void);
u16 PcpPackage(u8 *pOutBuf,const PCPsrtuctTypedef *pPCPsrtuct);
u8 PcpUnPackage(PCPsrtuctTypedef *pPCPsrtuct,u8 *pInBuf);

#endif

