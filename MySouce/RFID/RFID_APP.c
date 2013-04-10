#include "RFID_APP.h"
#include "RFID_drive.h"
#include "beep.h"
#include "FM1702_IO.h"

#include "LCDAPP.h"

rt_uint8_t  PassWord[6]={0xff,0xff,0xff,0xff,0xff,0xff};
rt_uint8_t block;
rt_uint8_t DataBuf[16];
void RFIDAPP_thread_entry(void* parameter)
{
	rt_device_t RFID;
	rt_err_t status;
	rt_uint16_t i;
	rt_uint8_t size;
	rt_uint8_t SnrBuf[4];
	
	RFID=rt_device_find("RFID");
	if(!RFID) 
	{
		rt_kprintf("find RFID fail\n");
		return ;
	}
	else rt_kprintf("find RFID OK\n");
	
	status=RFID->init(RFID);
	status=RFID->open(RFID,RT_DEVICE_OFLAG_RDWR);
	if(status!=RT_EOK)
	{
		rt_kprintf("open RFID fail\n");
		return ;
	}
	else
	{
		FM1702_beepON
		rt_thread_delay(RT_TICK_PER_SECOND/2);
		FM1702_beepOFF
		
	}
	status=RFID->control(RFID,RFID_ConfigISOType,"A");

	while(1)
	{
		//find card
		//rt_kprintf("find card\n");
		status=RFID->control(RFID,RFID_FindCard,0);
		if(status!=RT_EOK) 
		{
			//rt_kprintf("find RFID card\n");
			rt_thread_delay(RT_TICK_PER_SECOND/5);
			continue;
		}
		//防冲撞
		rt_kprintf("Anticoll\n");
		status=RFID->control(RFID,RFID_Anticoll,SnrBuf);
		if(status!=RT_EOK) continue;
		 
		rt_kprintf("Serial Number:");
		for(i=0;i<4;i++) rt_kprintf("%02X ",SnrBuf[i]);
		rt_kprintf("\n");
		
		//Select card
		rt_kprintf("SelCard\n");
		status=RFID->control(RFID,RFID_SelCard,0);
		if(status!=RT_EOK) continue;
		//load password into FIFO
		rt_kprintf("load password\n");
		status=RFID->control(RFID,RFID_LoadPasswordInFiFO,PassWord);
		if(status!=RT_EOK) continue;
		//Authenticate
		rt_kprintf("Authenticate\n");
		block=1;
		status=RFID->control(RFID,RFID_Authenticate_A,&block);
		if(status!=RT_EOK) continue;
		//read / Write

		size=RFID->read(RFID,block,DataBuf,0);
		if(size!=0)
		{
			status=RT_EOK;
			rt_kprintf("read1:");
			for(i=0;i<size;i++) rt_kprintf("%X ",DataBuf[i]);
			rt_kprintf("\n");
		}
		else continue;		
		
		for (i=0;i<16;i++)DataBuf[i]+=1;	//改变数据

		size=RFID->write(RFID,block,DataBuf,16);
		if(size!=0)
		{
			rt_kprintf("writed:");
			for(i=0;i<size;i++) rt_kprintf("%X ",DataBuf[i]);
			rt_kprintf("\n");
		}
		else continue;	
		
		size=RFID->read(RFID,block,DataBuf,0);
		if(size!=0)
		{
			status=RT_EOK;
			rt_kprintf("read2:");
			for(i=0;i<size;i++) rt_kprintf("%X ",DataBuf[i]);
			rt_kprintf("\n");
		}
		else continue;	
		
		//比较序列号
		if(SnrBuf[0]==0xBC)
		if(SnrBuf[1]==0x03)
		if(SnrBuf[2]==0xDD)
		if(SnrBuf[3]==0x0F)
		{			
			if(LCD_Event!=0)rt_event_send(LCD_Event,1<<2);
			rt_kprintf("RFID send LCD_Event 1<<2\n");
		}
		
		if(SnrBuf[0]==0xDE)
		if(SnrBuf[1]==0xFF)
		if(SnrBuf[2]==0x4E)
		if(SnrBuf[3]==0xE2)
		{
			if(LCD_Event!=0)rt_event_send(LCD_Event,1<<3);
			rt_kprintf("RFID send LCD_Event 1<<3\n");
		}
		
		FM1702_beepON
		rt_thread_delay(RT_TICK_PER_SECOND/2);
		FM1702_beepOFF
		//wait card leave
		rt_kprintf("wait card leave\n");
		while(status==RT_EOK)
		{
			status=RFID->control(RFID,RFID_FindCard,0);
			status=RFID->control(RFID,RFID_FindCard,0);
			rt_thread_delay(RT_TICK_PER_SECOND);
			
		}
		rt_kprintf("card leave\n");
		
	}
}
