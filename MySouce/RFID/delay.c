#include "delay.h"

/////////////////////////////////////////////////////////////////////
//用T2做延时子程序
/////////////////////////////////////////////////////////////////////

u8 reloadH;
u8 reloadL;

u16 cnt;

void InitTimer0(void)//@22.1184Mhz   1ms
{
    TMOD |= 0x01;
    TH0 = reloadH;
    TL0 = reloadL; 
    EA = 1; 
    ET0 = 1;
    TR0 = 1;
}

void Timer0Interrupt(void) interrupt 1
{
    TH0 = reloadH;
    TL0 = reloadL;
    //add your code here!
	if(cnt>0) cnt--;
	else TR0=0;	//暂定定时
}

void STC_DelayMs(u16 _MS)
{ 
	cnt=_MS;
	reloadH=0xf8;
    reloadL=0xcd;
	InitTimer0();
	while(cnt>0); 
}

void STC_Delay_50us(u8 _50us)
{ 
	cnt=_50us;
	reloadH=0xff;
    reloadL=0xa4;
	InitTimer0();
	while(cnt>0);
}

