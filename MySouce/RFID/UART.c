#include "UART.h"

void UartInit(void)		//9600bps@22.1184MHz
{
	PCON &= 0x7f;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	TMOD &= 0x0f;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ 
	TL1 = 0xFA;		//�趨��ʱ��ֵ
	TH1 = 0xFA;		//�趨��ʱ����װֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	
	ES=1;
	TR1 = 1;		//������ʱ��1
}

void Uart_sendByte(u8 Data)
{
	SBUF=Data;
	while(TI==0);
	TI=0;
}

/////////////////////////////////////////////////////////////////////
//�����жϣ�������λ������
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

