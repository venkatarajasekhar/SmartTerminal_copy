#include "LCDAPP.h"
#include "LCD_drive.h"



rt_mailbox_t LCD_MB;
rt_event_t 	LCD_Event;
/**
	���ڽ��մ�Һ�������ص����ݰ�
@param InData series chars input
@return the len of receive ,0 :is not finish receive
*/
/* static rt_uint16_t LCDReceiveByte(rt_uint8_t InData)
{
	static rt_uint16_t couter=0;
	rt_uint16_t temp;

	LCDscreenRbuf[couter]=InData;
	
	if((couter==0)&(InData!=0x7e)) return 0;
	else if((couter!=0)&(InData==0x7e))
	{
		if(couter==1) 	return 0;	//��˵��������0x7e��һ������
		temp=couter+1;
		couter=0;
		return temp;	//�����յ����ַ�����

	}
	 couter++;
	 return 0;
} */

/**
�ǳ����÷���size��������յ������ݲ�����size
*/
static rt_err_t LCD_input(rt_device_t dev,rt_size_t size)
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
		if(couter==1) 	return RT_EOK;	//��˵��������0x7e��һ������
		temp=couter+1;
		couter=0;
		
		//rt_mb_send(LCD_MB,temp);
		rt_event_send(LCD_Event,1);
		
		return temp;	//�����յ����ַ�����

	}
	 couter++;
	 
	 return RT_EOK;
}


#include "PCP.h"
#include "GNSS.h"

const char HZ[]="ASDFGHJKL";

rt_uint8_t *pText=0;
rt_uint16_t TextLen=0;
rt_uint8_t type;

#define Driver1 "��ӭ���ǳ�˾�������ı���ǣ�1002"
#define Driver2 "��ӭŷ��˾�������ı���ǣ�1008"

void LCDApp_thread_entry(void* parameter)
{
	rt_device_t LCD;
	PCPsrtuctTypedef *pPCPsrtuct;
	rt_uint16_t i;
	rt_uint32_t Value;
	rt_uint8_t *pCh;
	
	pPCPsrtuct=(PCPsrtuctTypedef *)rt_malloc(sizeof(PCPsrtuctTypedef));

 //   LCD_MB=rt_mb_create("LCD_MB",1,RT_IPC_FLAG_FIFO);
	LCD_Event=rt_event_create("LCD_Event",RT_IPC_FLAG_FIFO);
	RT_ASSERT(LCD_Event!=0)
	LCD=rt_device_find("LCD");
	if (!LCD)
	{
		rt_kprintf("can not find LCD device file:%s\tline:%s\n",__FILE__,__LINE__);
		return ;
	}
	rt_device_set_rx_indicate(LCD,LCD_input);
	LCD->init(LCD);
	LCD->open(LCD,RT_DEVICE_OFLAG_RDWR);

	pPCPsrtuct->CMD=0x07;
	pPCPsrtuct->Company=0x1234;
	pPCPsrtuct->Type=0x56;
	pPCPsrtuct->Version=0x78;
	
	pCh=pPCPsrtuct->userData;
	/* for (i=0;i<10;i++)	
	{
		pPCPsrtuct->userData[i]=i+'0';// 1~10
	} */
	
	for (i=0;i<10;i++)	
	{
		*pCh++=i+'0';// 1~10
	} 
	
	*pCh++='A';
	*pCh++='b';
	*pCh++='C';
	*pCh++='d';
	
	rt_memcpy(pCh,"³����ѧ",sizeof("³����ѧ")-1);
	
	
	pPCPsrtuct->size=10+4+sizeof("³����ѧ")-1;
	
	LCD->control(LCD,1,pPCPsrtuct);
	

	while (1)
	{
				
		rt_event_recv(LCD_Event,(1<<0)|(1<<1)|(1<<2)|(1<<3),RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&Value);
		
		if(Value&(1<<0))	//get PCP ACK
		{
			rt_kprintf("get LCD Message\n");
		}
		else if(Value&(1<<1))
		{
			//���ָ���Ƿ���ȷ
			if(!pText) continue;
			
			rt_memcpy(pPCPsrtuct->userData,pText,TextLen);
			pPCPsrtuct->size=TextLen;
			LCD->control(LCD,dLCD_SEND,pPCPsrtuct);
			
			//�ͷ���Դ
			rt_free(pText);
			TextLen=0;
		}
		else if(Value&(1<<2)) // only for test RFID
		{
			rt_memcpy(pPCPsrtuct->userData,Driver1,sizeof(Driver1)-1);
			pPCPsrtuct->size=sizeof(Driver1)-1;
			LCD->control(LCD,dLCD_SEND,pPCPsrtuct);
			rt_kprintf("get RFID 1<<2\n");
		}
		else if(Value&(1<<3)) // only for test nRFID
		{
			rt_memcpy(pPCPsrtuct->userData,Driver2,sizeof(Driver2)-1);
			pPCPsrtuct->size=sizeof(Driver2)-1;
			LCD->control(LCD,dLCD_SEND,pPCPsrtuct);
			rt_kprintf("get RFID 1<<3\n");
		}
		
/*		rt_mutex_take(PcpLockMutex,RT_WAITING_FOREVER);		
		Vaule=PcpUnPackage(pPCPsrtuct,LCD->user_data);
		rt_mutex_release(PcpLockMutex);

		if (Vaule==RT_EOK)
		{
			LCD->control(LCD,dLCD_SEND,pPCPsrtuct);
		} 
*/				
	
	}
	
	rt_free(pPCPsrtuct);
	rt_event_delete(LCD_Event);
	LCD->close(LCD);
}

void LCD_Get_text(rt_uint8_t type,rt_uint8_t *pStr,rt_uint16_t len)
{
	RT_ASSERT(len<dPCPuserDataSize)
	pText=rt_malloc(len);
	RT_ASSERT(pText!=0)
	rt_memcpy(pText,pStr,len);
	TextLen=len;
	rt_event_send(LCD_Event,1<<1);
}


