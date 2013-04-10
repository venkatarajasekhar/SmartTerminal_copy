#include "AdscreenAPP.h"

rt_mailbox_t ADS_MB;


/**
非常规用法，size传入的是收到的数据并不是size
*/
static rt_err_t ADS_input(rt_device_t dev,rt_size_t size)
{	
	static rt_uint16_t couter=0;
	rt_uint16_t temp;
	rt_uint8_t *SaveBuf;
	rt_uint8_t InData;	
	
	InData=size;
	SaveBuf=(rt_uint8_t *)dev->user_data;

	SaveBuf[couter]=InData;
	
	if((couter==0)&(InData!=0x7e)) return 0;
	else if((couter!=0)&(InData==0x7e))
	{
		if(couter==1) 	return RT_EOK;	//这说明有两个0x7e在一起来的
		temp=couter+1;
		couter=0;
		
		rt_mb_send(ADS_MB,temp);
		
		return temp;	//返回收到的字符个数

	}
	 couter++;
	 
	 return RT_EOK;
}


#include "PCP.h"
#define dCMD_MESSAGE 0x40

const char testStr[]="1234567890";
rt_uint8_t *pADText=0;
rt_uint16_t ADTextLen=0;
void ADSApp_thread_entry(void* parameter)
{
	rt_err_t retVal;
	rt_uint32_t Vaule;
	rt_device_t ADS;
	PCPsrtuctTypedef *pPCPsrtuct;
	rt_uint8_t len;
	static rt_uint8_t state=1; 
	pPCPsrtuct=(PCPsrtuctTypedef *)rt_malloc(sizeof(PCPsrtuctTypedef));

    ADS_MB=rt_mb_create("ADS_MB",1,RT_IPC_FLAG_FIFO);
	ADS=rt_device_find("ADS");
	if (!ADS)
	{
		rt_kprintf("can not find ADS device file:%s\tline:%s\n",__FILE__,__LINE__);
		return ;
	}
	rt_device_set_rx_indicate(ADS,ADS_input);
	ADS->init(ADS);
	ADS->open(ADS,RT_DEVICE_OFLAG_RDWR);
    
	while (1)
	{
	
		//get data from sever
		//convert data
		//add to struct of PCPsrtuct
		//send to Peripheral
		//wait ack of Peripheral
		
		switch (state)
		{
		case 1://get  version
			pPCPsrtuct->Version=0;
			pPCPsrtuct->Company=0;
			pPCPsrtuct->Type=dType_infoTerminal;
			pPCPsrtuct->CMD=dCmd_Version;
			pPCPsrtuct->size=0;
			
			ADS->control(ADS,1,pPCPsrtuct);
			
			retVal=rt_mb_recv(ADS_MB,&Vaule,RT_TICK_PER_SECOND*5); //wait for 5 seconds
			if (retVal==-RT_ETIMEOUT)
			{
				rt_kprintf("recv time out\n");
				break;
			}
			retVal=ADS->control(ADS,2,pPCPsrtuct);
			if (retVal!=RT_EOK)
			{
				rt_kprintf("get a packet od data,but is err\n");
			}
			else
			{
				rt_kprintf("get version success\n");
				state=2;
			}
									
			break;
		case 2://power on
			pPCPsrtuct->CMD=dCmd_PowerOn;
			pPCPsrtuct->size=0;
			ADS->control(ADS,1,pPCPsrtuct);
			
			retVal=rt_mb_recv(ADS_MB,&Vaule,RT_TICK_PER_SECOND*5); //wait for 5 seconds
			if (retVal==-RT_ETIMEOUT)
			{
				rt_kprintf("recv time out\n");
				break;
			}
			retVal=ADS->control(ADS,2,pPCPsrtuct);
			if (retVal!=RT_EOK)
			{
				rt_kprintf("get a packet od data,but is err\n");
			}
			else if (pPCPsrtuct->CMD==dCmd_PowerOn)
			{
				rt_kprintf("Power on success\n");
				state=3;
			}	
			break;
		case 3:
			state=4;
			break;
		case 4://send data
			//to do :get message for server
			
	//		for (len=0;len<sizeof(testStr)-1;len++)
	//		{
	//			pPCPsrtuct->userData[len]=testStr[len];
	//		}
	//		pPCPsrtuct->size=len;
			retVal=rt_mb_recv(ADS_MB,&Vaule,RT_WAITING_FOREVER); 
			pPCPsrtuct->CMD=dCMD_MESSAGE;
			if(!pADText) break;
			rt_memcpy(pPCPsrtuct->userData,pADText,ADTextLen);
			pPCPsrtuct->size=ADTextLen;
			
			rt_kprintf("textLen:%d\n",ADTextLen);
			rt_kprintf("%s\n",pADText);
			//释放资源
			rt_free(pADText);
			ADTextLen=0;
			
			ADS->control(ADS,1,pPCPsrtuct);
			
			retVal=rt_mb_recv(ADS_MB,&Vaule,RT_TICK_PER_SECOND*5); //wait for 5 seconds
			if (retVal==-RT_ETIMEOUT)
			{
				rt_kprintf("recv time out\n");
				break;
			}
			retVal=ADS->control(ADS,2,pPCPsrtuct);
			if (retVal!=RT_EOK)
			{
				rt_kprintf("get a packet data,but is err\n");
			}
			else
			{
				rt_kprintf("Get AD ACK\n");
			}
			//rt_thread_delay(RT_TICK_PER_SECOND*5);//5s delay
			break;
		}
	
	}
	
	rt_free(pPCPsrtuct);
	rt_mb_delete(ADS_MB);
	ADS->close(ADS);
}

void AD_Get_text(rt_uint8_t *pStr,rt_uint16_t len)
{
	RT_ASSERT(len<dPCPuserDataSize)
	pADText=rt_malloc(len);
	RT_ASSERT(pADText!=0)
	rt_memcpy(pADText,pStr,len);
	ADTextLen=len;
	rt_mb_send(ADS_MB,0);
}
