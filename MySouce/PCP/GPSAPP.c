#include "GPSAPP.h"

#include "GNSS.h"

#include <stdio.h>

rt_mailbox_t GPS_MB;



s8 Unpack_GPRMC(u8 *pGPRMCstr,LocateBasicInfoTypeDef *pOutLocateBasicInfo)
{
	u8 *tempBuf;
	u8 tempStr[3];
	u8 index=0;
	u8 i,j;
	u8 CommaNum=0;
	float fValue;
	u32 Value;
	
	tempBuf=(u8 *)rt_malloc(100);
	if(!tempBuf) return -1;
	
	while(*pGPRMCstr)
	{	
		if(*pGPRMCstr==',')
		{
			CommaNum++;
			tempBuf[index++]='\0';
			//rt_kprintf("%s\n",tempBuf);
			switch(CommaNum)
			{
			case 1:
				break;
			case 2:
				rt_memcpy(tempStr,&tempBuf[0],2);
				tempStr[2]='\0';
				sscanf(tempStr,"%x",&Value);
				pOutLocateBasicInfo->TimeBCD[3]=Value;

				rt_memcpy(tempStr,&tempBuf[2],2);
				tempStr[2]='\0';
				sscanf(tempStr,"%x",&Value);
				pOutLocateBasicInfo->TimeBCD[4]=Value;

				rt_memcpy(tempStr,&tempBuf[4],2);
				tempStr[2]='\0';
				sscanf(tempStr,"%x",&Value);
				pOutLocateBasicInfo->TimeBCD[5]=Value;
				break;
			case 3:
				if(tempBuf[0]!='A') return -1;
				break;
			case 4:
				for(i=0,j=0;i<index;i++)
				{
					if(tempBuf[i]=='.')
					{
						continue;
					}
					else
					{
						tempBuf[j]=tempBuf[i];
						j++;
					}
				}
				sscanf(tempBuf,"%d",&pOutLocateBasicInfo->Latitude);
				break;
			case 5:
				break;
			case 6:
				for(i=0,j=0;i<index;i++)
				{
					if(tempBuf[i]=='.')
					{
						continue;
					}
					else
					{
						tempBuf[j]=tempBuf[i];
						j++;
					}
				}
				sscanf(tempBuf,"%d",&pOutLocateBasicInfo->Longitude);
				break;
			case 7:
				break;
			case 8:
				sscanf(tempBuf,"%f",&fValue);
				pOutLocateBasicInfo->Speed=fValue*1.825/10;
				break;
			case 9:
				for(i=0;i<index;i++)
				{
					if(tempBuf[i]=='.')
					{
						tempBuf[i]='\0';
						break;
					}
				}
				sscanf(tempBuf,"%d",&Value);
				pOutLocateBasicInfo->Direction=Value;
				break;
			case 10:
				rt_memcpy(tempStr,&tempBuf[0],2);
				tempStr[2]='\0';
				sscanf(tempStr,"%x",&Value);
				pOutLocateBasicInfo->TimeBCD[2]=Value;

				rt_memcpy(tempStr,&tempBuf[2],2);
				tempStr[2]='\0';
				sscanf(tempStr,"%x",&Value);
				pOutLocateBasicInfo->TimeBCD[1]=Value;

				rt_memcpy(tempStr,&tempBuf[4],2);
				tempStr[2]='\0';
				sscanf(tempStr,"%x",&Value);
				pOutLocateBasicInfo->TimeBCD[0]=Value;
				break;
			case 11:
				break;
			case 12:
				break;
			}

			index=0;
		}
		else
		{
			tempBuf[index++]=*pGPRMCstr;
		}
		pGPRMCstr++;
	}

	pOutLocateBasicInfo->Altitude=0;
	rt_free(tempBuf);
	return 0;
}

void Ask_LocateInfo_callback(u16 AnswerSerialNumber,u16 AnswerID,u8 Result)
{
	GNSS_printf("%s\n",__FUNCTION__);
}
void Heartbeat_callback(u16 AnswerSerialNumber,u16 AnswerID,u8 Result)
{
	GNSS_printf("%s\n",__FUNCTION__);
}
//数据到达回调函数
//非常规用法，使用size传入的是收到的字符，而不是数量
static rt_err_t GPS_input(rt_device_t dev,rt_size_t size)
{
	static rt_uint8_t index=0;
	rt_uint8_t InData;
	rt_uint8_t *UserData;
	
	UserData=(rt_uint8_t *)dev->user_data;
	InData=(rt_uint8_t)size;
	
	if (InData=='$')
	{
		index=0;
	}
		
	UserData[index++]=InData;
	
	if(InData=='\n')
	{
		UserData[index++]='\0';
		rt_mb_send(GPS_MB,index);
		
		index=0;
	}
	
	return RT_EOK;
	
}

#include "ATcommand.h"
#include "Global.h"
#include "GNSS_Terminal.h"

void GPSApp_thread_entry(void* parameter)
{
	rt_uint32_t i;
	rt_uint32_t GpsLen;
	rt_device_t GPS;
	rt_uint8_t *Pchar;
	LocateBasicInfoTypeDef *pLocateBasicInfo;
	rt_uint32_t Value;
	s8 ret;
	rt_uint8_t couter=0;
	
	pLocateBasicInfo=rt_malloc(sizeof(LocateBasicInfoTypeDef));
	
	GPS=rt_device_find("GPS");
	if(!GPS)
	{
		rt_kprintf("find GPS Device fail\n");
		return ;
	}
	GPS_MB=rt_mb_create("GPS_MB",1,RT_IPC_FLAG_FIFO);
	rt_device_set_rx_indicate(GPS,GPS_input);
	GPS->init(GPS);
	GPS->open(GPS,0);
	
	while(1)
	{
			
		rt_mb_recv(GPS_MB,&GpsLen,RT_WAITING_FOREVER);
		
/*		//发送心跳
		//接收网络就绪事件
		rt_event_recv(&GlobalEvent,AUTHENTCATED_EVENT,RT_EVENT_FLAG_OR,0,&Value);
		if(Value&AUTHENTCATED_EVENT)
		{
			GNSS_T_Ask_Heartbeat(Heartbeat_callback);
		}
*/		
		
		couter++;
		if(couter<10) continue;
		couter=0;
		
		//rt_kprintf("%s",GPS->user_data);

		ret=Unpack_GPRMC(GPS->user_data,pLocateBasicInfo);
		if(ret<0) continue;
		
		//接收网络就绪事件
		rt_event_recv(&GlobalEvent,AUTHENTCATED_EVENT,RT_EVENT_FLAG_OR,0,&Value);
		if(Value&AUTHENTCATED_EVENT)
		{
			pLocateBasicInfo->AlarmFlage.All=0;
			pLocateBasicInfo->CarState.All=0;
			GNSS_T_Ask_LocateInfo(pLocateBasicInfo,0,0,Ask_LocateInfo_callback);
		}
		
		
	}
	
	GPS->close(GPS);
	rt_free(pLocateBasicInfo);
}


