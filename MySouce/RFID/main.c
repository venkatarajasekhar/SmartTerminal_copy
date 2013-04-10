#include "FM1702_IO.h"
#include "slrc1702.h"
#include "iso14443a.h"



static u8 delay_cnt;

static u8  CardType;
static u8  SnrBuf[4];
static u8  CardSize;
static u8  UncodedBuf[6];
static u8  CodedBuf[12];

static u8  auth_mode;
static u8  block;
static u8 DataBuf[16];

int ttmain(void)
{
	s8 status;
	u8 i; 
	
	UncodedBuf[0]=0xFF;
	UncodedBuf[1]=0xFF;
	UncodedBuf[2]=0xFF;
	UncodedBuf[3]=0xFF;
	UncodedBuf[4]=0xFF;
	UncodedBuf[5]=0xFF;

	auth_mode=0x60;	//A password
	block=0;

	InitiHardware();
	FM1702_debug_sendByte(0xAA);
	status=Rc1702Ready();					//init FM1702 chip and processor interface
	if(status==MI_OK)
	{
		FM1702_beepON;               //蜂鸣器响一声，表示fm1702复位成功
		 FM1702_DelayMs(200);
		FM1702_DelayMs(200);
		FM1702_beepOFF;
	}
	
	i=FM1702_Read(RegFIFOLength);	
	
	PcdConfigISOType('A');


	
	while (1)
	{

		//find card
		status=PcdRequest(0x52,&CardType);
		if (status==MI_OK)
		{
			if ((status==MI_OK)&&(CardType==0x04)) status=MI_OK;	//check the Card Type is S50 or not
			else status=MI_CHK_FAILED;
		}
		else continue ;


		FM1702_debug_sendByte(0xA1);
		//防冲撞
		if (status==MI_OK)
		{
			status=PcdAnticoll(SnrBuf);
		}
		else continue ;

		FM1702_debug_sendByte(0xA2);
		//Select card
		if (status==MI_OK)
		{
			status=PcdSelect(SnrBuf,&CardSize);
		}
		else continue ;

		FM1702_debug_sendByte(0xA3);
		//load password into FIFO
		if (status==MI_OK)
		{
			ChangeCodeKey(UncodedBuf,CodedBuf);
			status=PcdAuthKey(CodedBuf);
		}
		else continue ;

		FM1702_debug_sendByte(0xA4);
		//Authenticate
		if (status==MI_OK)
		{
			status=PcdAuthState(auth_mode,block,SnrBuf);
		}
		else continue ;

		FM1702_debug_sendByte(0xA5);
		//read / Write
		if (status==MI_OK)
		{
			status=PcdRead(block,DataBuf);

			if (status==MI_OK)
			{
				for (i=0;i<16;i++)FM1702_debug_sendByte(DataBuf[i]);
			}

		}
		else continue ; 

		FM1702_beepON;
		FM1702_DelayMs(1000);
		FM1702_beepOFF;
		//wait card leave
		while (status==MI_OK)
		{
			status=PcdRequest(0x52,&CardType);
			status=PcdRequest(0x52,&CardType);
			//delay
			FM1702_DelayMs(100);
		}

	}
}
