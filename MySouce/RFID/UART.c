#include "UART.h"

void UartInit(void)		//9600bps@22.1184MHz
{
	PCON &= 0x7f;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	TMOD &= 0x0f;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式 
	TL1 = 0xFA;		//设定定时初值
	TH1 = 0xFA;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	
	ES=1;
	TR1 = 1;		//启动定时器1
}

void Uart_sendByte(u8 Data)
{
	SBUF=Data;
	while(TI==0);
	TI=0;
}

/////////////////////////////////////////////////////////////////////
//串行中断，接收上位机数据
/////////////////////////////////////////////////////////////////////
static u8 len=0;
static u8 cnt=0;
void seri_int (void) interrupt 4 
{
	if(RI==1)
	{
		RI=0;		
	}
	else
	{
		TI=0;
	}
} 

