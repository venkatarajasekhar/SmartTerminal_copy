#include "GNSS_APP.h"
/**
这个文件存放一些GNSS应用相关的
*/


/**
注册、鉴权线程
通过读取文件获得相关信息，通过判断是否已经注册了（没有则注册），否则鉴权
鉴权成功后发布鉴权成功事件以通知通知其他线程
*/
#include "Global.h"
#include "GNSS_Terminal.h"
#include "GBT2260.h"
#include "JTT415_2006.h"
#include "dfs_posix.h"

static u8 gResult;	//鉴权结果

static void Authenticate_callback(u16 AnswerSerialNumber,u16 AnswerID,u8 Result)
{
	GNSS_printf("%s\n",__FUNCTION__);
	if(AnswerID!=eTerminalAuthenticate) rt_kprintf("error Authenticate_callback AnswerID\n");
	gResult=Result;
	rt_event_send(&GNSSEvent,GNSS_RegisterACKEvent);

}
static void Unregister_callback(u16 AnswerSerialNumber,u16 AnswerID,u8 Result)
{
	GNSS_printf("%s\n",__FUNCTION__);
	if(AnswerID!=eTerminalUnregister) rt_kprintf("error Unregister_callback AnswerID\n");
	gResult=Result;
	rt_event_send(&GNSSEvent,GNSS_RegisterACKEvent);
}
/**
终端注册
终端注销
终端鉴权
*/
#define Register		1
#define	Canceled		2
#define	authenticate	3
void Register_thread_entry(void* parameter)
{
	u32 param;
	u8 *p;
	u8 i;
	rt_int32_t fd;
	rt_uint32_t Value;
	TerminalRegisterStructTypeDef *pTerminalRegisterStruct;

	param=(rt_uint32_t)parameter;
	pTerminalRegisterStruct=rt_malloc(sizeof(TerminalRegisterStructTypeDef));
	RT_ASSERT(pTerminalRegisterStruct!=0);

	//接收网络就绪事件
	rt_event_recv(&GlobalEvent,NET_CONNET_EVENT,RT_EVENT_FLAG_OR,RT_WAITING_FOREVER,&Value);

	//增加测控制，不鉴权了。直接发布鉴权成功事件
	rt_event_send(&GlobalEvent,AUTHENTCATED_EVENT);
	return ;
	
		switch (param)
		{
		case Register://注册命令
			//读取配置信息
			fd=open(GNSS_PATH(register.dat),O_RDONLY,0);
			if (fd<0)
			{
				rt_kprintf("open %s fail\n",GNSS_PATH(register.dat));
			}
			Value=read(fd,pTerminalRegisterStruct,sizeof(TerminalRegisterStructTypeDef));
			//rt_kprintf("read %d bytes\n",Value);
			close(fd);
			
			//判断是否有鉴权码，没有则注册否则就鉴权
			if (pTerminalRegisterStruct->AuthenticationCodeStr[0]=='\0')
			{

				GNSS_T_ask_Register(pTerminalRegisterStruct);

				rt_event_recv(&GNSSEvent,GNSS_RegisterACKEvent,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&Value);
				if (Value&GNSS_RegisterACKEvent)
				{
					//验证是否收到鉴权码
					fd=open(GNSS_PATH(register.dat),O_RDONLY,0);
					if (fd<0)
					{
						rt_kprintf("open %s fail\n",GNSS_PATH(register.dat));
						RT_ASSERT(0);
					}
					Value=read(fd,pTerminalRegisterStruct,sizeof(TerminalRegisterStructTypeDef));
					//	rt_kprintf("read %d bytes\n",Value);
					close(fd);
					if (pTerminalRegisterStruct->AuthenticationCodeStr[0]=='\0')
					{
						rt_kprintf("no Authentication Code\n");
						RT_ASSERT(0);
					}

				}

				//注册成功
			}
			//注册成功或已经注册过了，就自动转到鉴权 （这里没有break）
		case authenticate://终端鉴权
			//计算鉴权码个数，但不超过10个（包括\0）
			p=pTerminalRegisterStruct->AuthenticationCodeStr;
			for (i=0;i<10;i++)
			{
				if (*p=='\0') break;
				p++;
			}
			if (i>=10)
			{
				rt_kprintf("error;string >=10\n");
				RT_ASSERT(0);
			}

			GNSS_T_Ask_Authenticate(pTerminalRegisterStruct->AuthenticationCodeStr,i+1,Authenticate_callback);

			//等待通用应答
			rt_event_recv(&GNSSEvent,GNSS_RegisterACKEvent,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&Value);
			if (Value&GNSS_RegisterACKEvent)
			{
				switch (gResult)
				{
				case 0://成功/确认
					//鉴权已经成功，发布鉴权成功事件
					rt_event_send(&GlobalEvent,AUTHENTCATED_EVENT);
					rt_kprintf("Result=Success\n");
					break;
				case 1://失败
					rt_kprintf("Result=fail\n");
					break;
				case 2://消息有误
					rt_kprintf("Result=error\n");
					break;
				case 3://不支持
					rt_kprintf("Result=not support\n");
					break;
				default :
					rt_kprintf("error:not support\n");
					RT_ASSERT(0)
				}
			}
			break;
		case Canceled://终端注销
			GNSS_T_Ask_Unregister(Unregister_callback);
			
			//等待通用应答
			rt_event_recv(&GNSSEvent,GNSS_RegisterACKEvent,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&Value);
			if (Value&GNSS_RegisterACKEvent)
			{
				switch (gResult)
				{
				case 0://成功/确认
					//鉴权已经成功，发布鉴权成功事件
					rt_event_send(&GlobalEvent,AUTHENTCATED_EVENT);
					rt_kprintf("Result=Success\n");
					break;
				case 1://失败
					rt_kprintf("Result=fail\n");
					break;
				case 2://消息有误
					rt_kprintf("Result=error\n");
					break;
				case 3://不支持
					rt_kprintf("Result=not support\n");
					break;
				default :
					rt_kprintf("error:not support\n");
					RT_ASSERT(0)
				}
			}

			break;

		default:
			RT_ASSERT(0);
		}	

rt_free(pTerminalRegisterStruct);
}

/**
指定包上传
*/
/*
void MultimediaUpload_thread_entry(void* parameter)
{
	MultimediaTypedef Multimedia;
	u8 * pData;
	int fd;
	int ret;
	struct stat Buf;
	rt_uint32_t Value;

	pData=(u8 *)GNSS_malloc(512);
	Multimedia.MultimediaID=0x12345678;
	Multimedia.type=0;
	Multimedia.format=0;
	Multimedia.event=3;
	Multimedia.channelID=8;
	Multimedia.TotalPackets=0x87654321;
	Multimedia.PacketID=0;

	fd=GNSS_fopen("/Picture/girl.jpg",O_RDONLY,0);
	if(fd<0)
	{
		GNSS_printf("open %s fail\n","/Picture/girl.jpg");
	}
	//计算总包数
	GNSS_fstat(fd,&Buf);
	GNSS_printf("size=%d\n",Buf.st_size);
	Multimedia.TotalPackets=Buf.st_size/512;
	if(Buf.st_size%512!=0)
	{
		Multimedia.TotalPackets+=1;
	}
//	GNSS_flseek(fd,(2-1)*512,SEEK_SET);
	while (1)
	{
		ret=GNSS_fread(fd,pData,512);
		if(ret==0) 
		{
			GNSS_printf("EOF\n");
			break;
		}
		Multimedia.PacketID++;
		Multimedia.pData=pData;
		Multimedia.len=ret;
		
		//接收网络就绪事件
		rt_event_recv(&GlobalEvent,AUTHENTCATED_EVENT,RT_EVENT_FLAG_OR,0,&Value);
		if(Value&AUTHENTCATED_EVENT)
		{
			GNSS_T_Ask_MultimediaUpload(&Multimedia);
		}
		
		rt_thread_delay(RT_TICK_PER_SECOND/10);
	}
	
	GNSS_free(pData);
}
*/
void MultimediaUpload_thread_entry(void* parameter)
{
	MultimediaTypedef Multimedia;
	u8 * pData;
	int fd;
	int ret;
	struct stat Buf;
	u32 Value;
	u16 i;
	u8 Result;
	MessageStructTypeDef *pMessageStruct;
//	MultimediaUploadCMDTypedef MultimediaUploadCMD;
	u8 *PathName;
	u32 MultimediaID;
	pMessageStruct=(MessageStructTypeDef *)parameter;
	PathName=(u8 *)GNSS_malloc(100);
	switch(pMessageStruct->MessageHeadUnion.MessageHead.MessageID)
	{
	case eStorageUploadMultimediaDataCMD://多媒体上传命令
		Result=GNSS_T_Get_MultimediaUploadCMD(&pMessageStruct->MessageHeadUnion.MessageHead,pMessageStruct->Data,&MultimediaID); 
		if(Result==dSuccess)
		{
			pData=(u8 *)GNSS_malloc(512);
			if(!pData)
			{
				GNSS_printf("mallco fail\n");
				break;
			}
			Multimedia.MultimediaID=MultimediaID;
			Multimedia.type=0;
			Multimedia.format=0;
			Multimedia.event=0;
			Multimedia.channelID=0;
			Multimedia.TotalPackets=0x87654321;
			Multimedia.PacketID=0;
			
			GNSS_sprintf(PathName,"/Picture/%08X.jpg",MultimediaID);
			fd=GNSS_fopen(PathName,O_RDONLY,0);
			if(fd<0)
			{
				GNSS_printf("open %s fail\n",PathName);
				GNSS_free(pData);
				break;
			}
			//计算总包数
			GNSS_fstat(fd,&Buf);
			GNSS_printf("size=%d\n",Buf.st_size);
			Multimedia.TotalPackets=Buf.st_size/512;
			if(Buf.st_size%512!=0)
			{
				Multimedia.TotalPackets+=1;
			}
			//	GNSS_flseek(fd,(2-1)*512,SEEK_SET);
			while (1)
			{
				ret=GNSS_fread(fd,pData,512);
				if(ret==0) 
				{
					GNSS_printf("EOF\n");
					break;
				}
				Multimedia.PacketID++;
				Multimedia.pData=pData;
				Multimedia.len=ret;
				
			//接收网络就绪事件
			rt_event_recv(&GlobalEvent,AUTHENTCATED_EVENT,RT_EVENT_FLAG_OR,RT_WAITING_FOREVER,&Value);

			GNSS_T_Ask_MultimediaUpload(&Multimedia);
				
			rt_thread_delay(RT_TICK_PER_SECOND/10);
			}

			GNSS_free(pData);
		}
		break;
	case eUploadMultimediaDataACK:
		break;
	}

	GNSS_free(parameter);
	GNSS_free(PathName);

}

//#ifdef RT_USING_FINSH
//#include <finsh.h>
//FINSH_FUNCTION_EXPORT(Multi, Multi);
//#endif

#ifdef RT_USING_FINSH
#include <finsh.h>
void test_register(void)
{
	rt_uint32_t Value;
	rt_int32_t fd;
	TerminalRegisterStructTypeDef *pTerminalRegisterStruct;
	
	pTerminalRegisterStruct=rt_malloc(sizeof(TerminalRegisterStructTypeDef));
	
		pTerminalRegisterStruct->Province=YanTaiShi/10000;
		pTerminalRegisterStruct->City=YanTaiShi%10000;
		pTerminalRegisterStruct->ManufacturerID[0]=0x30;
		pTerminalRegisterStruct->ManufacturerID[1]=0x31;
		pTerminalRegisterStruct->ManufacturerID[2]=0x32;
		pTerminalRegisterStruct->ManufacturerID[3]=0x33;
		pTerminalRegisterStruct->ManufacturerID[4]=0x34;
		pTerminalRegisterStruct->TerminalModel[0]=0x1;
		pTerminalRegisterStruct->TerminalModel[1]=0x2;
		pTerminalRegisterStruct->TerminalModel[2]=0x3;
		pTerminalRegisterStruct->TerminalModel[3]=0x4;
		pTerminalRegisterStruct->TerminalModel[4]=0x5;
		pTerminalRegisterStruct->TerminalModel[5]=0x6;
		pTerminalRegisterStruct->TerminalModel[6]=0x7;
		pTerminalRegisterStruct->TerminalModel[7]=0x8;
		pTerminalRegisterStruct->TerminalID[0]=0x20;
		pTerminalRegisterStruct->TerminalID[1]=0x21;
		pTerminalRegisterStruct->TerminalID[2]=0x22;
		pTerminalRegisterStruct->TerminalID[3]=0x23;
		pTerminalRegisterStruct->TerminalID[4]=0x24;
		pTerminalRegisterStruct->TerminalID[5]=0x25;
		pTerminalRegisterStruct->TerminalID[6]=0x26;
		pTerminalRegisterStruct->CarBodyColor=dCarLicensePlateColor_Black;
		pTerminalRegisterStruct->CarLicenseStr[0]=0x00;
		pTerminalRegisterStruct->CarLicenseStr[1]=0x01;
		pTerminalRegisterStruct->CarLicenseStr[2]=0x02;
		pTerminalRegisterStruct->CarLicenseStr[3]=0x03;
		pTerminalRegisterStruct->CarLicenseStr[4]=0x04;
		pTerminalRegisterStruct->CarLicenseStr[5]=0x05;
		pTerminalRegisterStruct->CarLicenseStr[6]=0x06;
		pTerminalRegisterStruct->CarLicenseStr[7]=0x07;
		pTerminalRegisterStruct->CarLicenseStr[8]=0x08;
		pTerminalRegisterStruct->AuthenticationCodeStr[0]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[1]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[2]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[3]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[4]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[5]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[6]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[7]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[8]='\0';
		pTerminalRegisterStruct->AuthenticationCodeStr[9]='\0';
		
	fd=open(GNSS_PATH(register.dat),O_WRONLY|O_CREAT,0);
	if(fd<0)
	{
		rt_kprintf("open %s fail\n",GNSS_PATH(register.dat));
	}
	Value=write(fd,pTerminalRegisterStruct,sizeof(TerminalRegisterStructTypeDef));
	rt_kprintf("write %d bytes\n",Value);
	close(fd);
	rt_free(pTerminalRegisterStruct);
	
}
FINSH_FUNCTION_EXPORT(test_register, test_register);
#endif


#ifdef RT_USING_FINSH
#include <finsh.h>

void LocateInfo_callback(u16 AnswerSerialNumber,u16 AnswerID,u8 Result)
{
	GNSS_printf("%s\n",__FUNCTION__);
}
void test_LocateInfo(void)
{
	u8 i;
		LocateBasicInfoTypeDef Basic;
		ExtraLocateInfoTypeDef ExtraLocateInfo[2];
		//init basic
		Basic.AlarmFlage.All=0x12345678;
		Basic.CarState.All=0x12345678;
		Basic.Latitude=0xa1234567;
		Basic.Longitude=0xb1234567;
		Basic.Altitude=0xc123;
		Basic.Speed=0xd123;
		Basic.Direction=0xe123;
		for (i=0;i<6;i++) 
		{
			Basic.TimeBCD[i]=i+0x30;
		}

		ExtraLocateInfo[0].ExtraInfoID=0x01;
		ExtraLocateInfo[0].ExtraInfoLen=4;
		ExtraLocateInfo[0].ExtraInfo[0]=0;
		ExtraLocateInfo[0].ExtraInfo[1]=1;
		ExtraLocateInfo[0].ExtraInfo[2]=2;
		ExtraLocateInfo[0].ExtraInfo[3]=3;

		ExtraLocateInfo[1].ExtraInfoID=0x02;
		ExtraLocateInfo[1].ExtraInfoLen=4;
		ExtraLocateInfo[1].ExtraInfo[0]=0;
		ExtraLocateInfo[1].ExtraInfo[1]=1;
		ExtraLocateInfo[1].ExtraInfo[2]=2;
		ExtraLocateInfo[1].ExtraInfo[3]=3;

		GNSS_T_Ask_LocateInfo(&Basic,ExtraLocateInfo,2,LocateInfo_callback);
}

FINSH_FUNCTION_EXPORT(test_LocateInfo, test_LocateInfo);
#endif
