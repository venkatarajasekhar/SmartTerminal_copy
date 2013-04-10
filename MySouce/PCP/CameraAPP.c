#include "CameraAPP.h"
#include "Camera_drive.h"



/*************************************************
***摄像头数据接收函数
***return 0:
***return 1:check sum err
***return 2:get a part of data 
***return 3:get end chars and a part of data 
***return 4:there is no used data 
note;there is not to check The checkSum so there was a BUG here
**************************************************/

static rt_uint8_t CameraData(rt_uint8_t Data)
{
	static rt_device_t CameraDev;
	static CameraDataTypedef *pCameraDataStruct;
	static rt_uint8_t *pData=RT_NULL;
	static rt_uint16_t index=0;
	static rt_uint8_t checkSum8=0;
	static rt_uint8_t endFlage=0;
	
	if(pData==RT_NULL)	//第一次进入时，获取一次camer数据结构地址
	{
		CameraDev=rt_device_find("Camera");
		RT_ASSERT(CameraDev != RT_NULL);
		pCameraDataStruct=(CameraDataTypedef *)CameraDev->user_data;
		
		pCameraDataStruct->size=0;
		pData=pCameraDataStruct->buf;
//		rt_kprintf("第一次进入\r\n");
	}
	
	index++;
	//rt_kprintf("index=%d",index);
	if(index>cameraBufSize+11)
	{
		RT_ASSERT(index<cameraBufSize+11);
	}
	else if( Data=='\r')
	{
		*pData++=Data;
		endFlage=1;
	}
	else if(Data=='\n')	
	{	
		*pData++=Data;
		if(endFlage==1)
		{				
			if(index<=9)
			{
				endFlage=0;
				pCameraDataStruct->size=index;
				pData=pCameraDataStruct->buf;
				index=0;
				checkSum8=0;
//				rt_kprintf("空数据\r\n");
				return dNoneDatapart;
			}
			else if(index==((dImagePacketSizeH<<8)|dImagePacketSizeL)+11)
			{
				endFlage=0;
				pCameraDataStruct->size=index;
				pData=pCameraDataStruct->buf;
//				rt_kprintf("接收一桢 size=%d\r\n",index);
				index=0;
				return dPartOfdata;
			} 
			else if(*(pData-4)==0xD9 )
			{
				endFlage=0;
				pCameraDataStruct->size=index;
				pData=pCameraDataStruct->buf;
//				rt_kprintf("接收完毕 size=%d\r\n",index);
				index=0;
				return dEndChars;
			}
			
		}
		endFlage=0;
		
	}
	else
	{
		*pData++=Data;
		checkSum8+=Data;
	}

		
	return RT_EOK;
}


/*-----------------------------------------------------------------------------------------------------------------------*/
#include "dfs_posix.h"
#include "time.h"

rt_event_t CameraEV;

/**
非常规用法，size传入的是收到的数据并不是size
*/
static rt_err_t Camera_input(rt_device_t dev,rt_size_t size)
{
	rt_err_t ret;
	
	ret=CameraData(size);
    if (ret)
	{
		rt_event_send(CameraEV,(1<<ret));
	}
	
	return RT_EOK;
	
}


void CameraApp_thread_entry(void* parameter)
{
	int fd;
	rt_uint32_t Vaule;
	rt_device_t Camera;
	CameraDataTypedef *pCameraDataStruct;
	rt_uint32_t length;
	
	CameraEV=rt_event_create("CamerEV",RT_IPC_FLAG_FIFO);
	Camera=rt_device_find("Camera");
	if (!Camera)
	{
		rt_kprintf("can not find Camera device file:%s\tline:%s\n",__FILE__,__LINE__);
		return ;
	}
	rt_device_set_rx_indicate(Camera,Camera_input);
	Camera->init(Camera);
	Camera->open(Camera,RT_DEVICE_OFLAG_RDWR);
	pCameraDataStruct=(CameraDataTypedef *)Camera->user_data;
	Camera->control(Camera,dTakePhoto640480CMD,0);
	while (1)
	{
			
		rt_event_recv(CameraEV,(1<<1)|(1<<2)|(1<<3)|(1<<4), RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&Vaule);
		//rt_kprintf("CameraEV:0x%X\n",Vaule);
		
		if (Vaule&(1<<1))		//check sum err
		{
			
		}
		if ((Vaule&(1<<2))|(Vaule&(1<<3)))		//get a part of data 
		{
			fd=open("/Picture/Camera1.jpg",O_CREAT|O_APPEND|O_RDWR,0);
            if(fd<0)
            {
                rt_kprintf("open %s failed\n","/Camera1.jpg");
                break;
            }
            
            length=write(fd,pCameraDataStruct->buf+8,pCameraDataStruct->size-11);
            if(length!=(pCameraDataStruct->size-11))
            {
                rt_kprintf("write data failed\r\n");
            }
   
			close(fd);
            pCameraDataStruct->size=0;
            Camera->control(Camera,dReadNextPhotoDataCMD,RT_NULL);
		}

		if (Vaule&(1<<4))		//there is no used data 
		{
			{
				
				rt_device_t device;
				time_t *pNow=rt_malloc(sizeof(time_t));
				rt_uint8_t *pStr=(rt_uint8_t *)rt_malloc(100);
				struct tm *p;
				int state;
				fd=open("/Picture/Camera1.jpg",O_CREAT|O_APPEND|O_RDWR,0);
				device = rt_device_find("rtc");
				if (device != RT_NULL)
				{ 
					rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, pNow);
					//rt_kprintf("%X\n",*pNow);
					//p=localtime(pNow);
					rt_sprintf(pStr,"/Picture/%04X.jpg",*pNow);
					//rt_kprintf("%d-%d-%d\t%d:%d:%d\n",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
					//rt_sprintf(pStr,"/%d%d.%s",p->tm_hour, p->tm_min,"jpg");
					state=rename("/Picture/Camera1.jpg",pStr);
					rt_kprintf("%s\n",pStr);
					//rt_kprintf("%d\n",state);
				}
				rt_free(pStr);
				rt_free(pNow);
				close(fd);
			}
            pCameraDataStruct->size=0;
			rt_thread_delay(RT_TICK_PER_SECOND*60);//delay 60s and take photo again
			Camera->control(Camera,dTakePhoto640480CMD,0);//take photo again
		}
	}
	
	Camera->close(Camera);
	rt_event_delete(CameraEV);
}


