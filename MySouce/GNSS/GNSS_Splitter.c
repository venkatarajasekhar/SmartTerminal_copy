
#include "GNSS_Splitter.h"
#include "GNSS.h"
#include "GNSS_Terminal.h"
#include "GNSS_Platform.h"

#include "DecoderAPI.h"
#include "Global.h"
/**
  GNSS 数据分配器
*/
void GNSS_Splitter(void)
{
	static MessageHeadTypeDef MessageHead;
	static u8 bodyBuf[GNSS_FrameBytes_MAX];
	s16 ret;

	ret=GNSS_TryGetMessage(&MessageHead,bodyBuf);
	if(ret!=0) return;

//	GNSS_printf("get a fream\n");
	switch(MessageHead.MessageID)
	{
#if GNSS_TYPE_Terminal
	//终端响应list ...
	case eServerCommonACK:
		GNSS_printf("get server common ACK\n");
		GNSS_SearchAndExecute((bodyBuf[0]<<8)|bodyBuf[1],(bodyBuf[2]<<8)|bodyBuf[3],bodyBuf[4]);		
		break;
	case eTerminalRegisterACK:
		ret=GNSS_T_GetACK_Register(bodyBuf);
		if(ret==GNSS_EOK) rt_event_send(&GNSSEvent,GNSS_RegisterACKEvent);
		break;
	case eSetTerminalParameters:
		GNSS_T_Get_SetParam(&MessageHead,bodyBuf);
		break;
	case eGetTerminalParameters:
		GNSS_T_Get_QueryParam(MessageHead.MessageSerialNumber);
		break;
	case eTerminalControl:
		GNSS_T_Get_Control(&MessageHead,bodyBuf);
		break;
	case eGetLocationInformation:
		//add to do ,eg; a mailbox to tell the event
		{
			LocateBasicInfoTypeDef Basic;
			GNSS_T_Get_QueryLocateInfo(&Basic,0,0);
		}
		break;
	case eTemporaryPositionTrackingControl:
		GNNS_T_Get_TempLocateTrack(&MessageHead,bodyBuf);
		break;
	case eTextMessage:
		GNSS_T_Get_TextInfo(&MessageHead,bodyBuf);
		break;
	case eQuestionDownlink:
		GNSS_T_Get_Question(&MessageHead,bodyBuf);
		break;
	case eSetInformationMenu:
		GNSS_T_Get_SetMenu(&MessageHead,bodyBuf);
		break;
	case eInformationServices:
		GNSS_T_Get_InfoService(&MessageHead,bodyBuf);
		break;
	case eCallBack:
		GNSS_T_Get_PhoneCallBack(&MessageHead,bodyBuf);
		break;
	case eUploadMultimediaDataACK:
		GNSS_T_GetAck_MultimediaUpload(&MessageHead,bodyBuf);
		break;
	case eStorageUploadMultimediaDataCMD:
		{
			extern void MultimediaUpload_thread_entry(void* parameter);
			rt_thread_t Thread;
			
			MessageStructTypeDef *pMessageStruct;
			pMessageStruct=(MessageStructTypeDef *)GNSS_malloc(sizeof(MessageStructTypeDef));
			if(!pMessageStruct)
			{
				GNSS_ASSERT(pMessageStruct!=0)
			}
			
			pMessageStruct->MessageHeadUnion.MessageHead=MessageHead;
			GNSS_memcpy(pMessageStruct->Data,bodyBuf,pMessageStruct->MessageHeadUnion.MessageHead.MessageProperty.Bit.MessageLen);
			
			
			Thread=rt_thread_create("mediaUpload",		MultimediaUpload_thread_entry,	pMessageStruct,0x2048,28,  5);
			if(Thread!=RT_NULL) rt_thread_startup(Thread);
		}
		
		break;
	case eSendVoiceData://接收到语音数据		
		CopyToDecoderBuf(bodyBuf);
		break;
#endif
#if GNSS_TYPE_Server
	//服务器响应list ...
	case eTerminalCommonACK:
		GNSS_printf("get Terminal common ACK\n");
		GNSS_SearchAndExecute((bodyBuf[0]<<8)|bodyBuf[1],(bodyBuf[2]<<8)|bodyBuf[3],bodyBuf[4]);		
		break;
	case eTerminalHeartbeat:
		GNSS_S_ACK_Heartbeat(MessageHead.MessageSerialNumber,MessageHead.MessageID);
		break;
	case eTerminalRegister:
		GNSS_S_ACK_Register(&MessageHead,bodyBuf);
		break;
#endif
	}
}

#ifdef RT_USING_FINSH
#include <finsh.h>

/**
  应答callback funtion
*/
void GNSS_T_Heartbeat_callback(u16 AnswerSerialNumber,u16 AnswerID,u8 Result)
{
	GNSS_printf("In callback funtion\n");
	GNSS_printf("AnswerSerialNumber=%04X\n",AnswerSerialNumber);
	GNSS_printf("AnswerID=%04X\n",AnswerID);
	GNSS_printf("Result=%02X\n",Result);
}

void GNSS_test(void)
{
	GNSS_Init();
	GNSS_T_Ask_Heartbeat(GNSS_T_Heartbeat_callback);
	rt_thread_delay(RT_TICK_PER_SECOND);
	GNSS_Close();
}

FINSH_FUNCTION_EXPORT(GNSS_test, GNSS_test);

#endif


struct rt_semaphore GNSS_R_sem;

void GNSS_Splitter_thread_entry(void* parameter)
{
	GNSS_Init();
	rt_sem_init(&GNSS_R_sem,"GNSS_R_sem",0,RT_IPC_FLAG_FIFO);
	//GNSS_T_Heartbeat(0);
	while (1)
	{
		rt_sem_take(&GNSS_R_sem,RT_WAITING_FOREVER);
//		rt_kprintf("get frame\n");
		GNSS_Splitter();
	}
	GNSS_Close();
	rt_sem_detach(&GNSS_R_sem);
}


















