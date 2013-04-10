#ifndef _FM1702_IO_H_BAB
#define _FM1702_IO_H_BAB

#include "stm32f10x.h"

void FM1702_mian(void);

// #include "STC12C5A60S2.h"
// #include "BaseType.h"
// #include "UART.h"
// #include "SPI.h"
// #include "delay.h"

	//interfact

// sbit _RST  =   P1^3;         	// FM1702¸´Î»¹Ü½Å¶¨Òå ///RC500ALE
// sbit _Beep        =   P2^0;			//·äÃùÆ÷

// sbit _NSS  =   P1^4;
// sbit _SCK  =   P1^7;
// sbit _MOSI =   P1^5;
// sbit _MISO =   P1^6; 

#define FM1702_beepON	GPIO_ResetBits(GPIOA,GPIO_Pin_8);		//_Beep=0;
#define FM1702_beepOFF	GPIO_SetBits(GPIOA,GPIO_Pin_8);		//_Beep=1;

#define FM1702_RST_L 	GPIO_ResetBits(GPIOG,GPIO_Pin_15);		//_RST=0;
#define FM1702_RST_H 	GPIO_SetBits(GPIOG,GPIO_Pin_15);		//_RST=1;

#define FM1702_NSS_L	GPIO_ResetBits(GPIOA,GPIO_Pin_15);		//_NSS=0;
#define FM1702_NSS_H	GPIO_SetBits(GPIOA,GPIO_Pin_15);		//_NSS=1;

#define FM1702_SCK_L	GPIO_ResetBits(GPIOB,GPIO_Pin_3);		//_SCK=0;
#define FM1702_SCK_H	GPIO_SetBits(GPIOB,GPIO_Pin_3);			//_SCK=1;

#define FM1702_MOSI_L	GPIO_ResetBits(GPIOB,GPIO_Pin_5);		//_MOSI=0;
#define FM1702_MOSI_H	GPIO_SetBits(GPIOB,GPIO_Pin_5);			//_MOSI=1;


#define FM1702_MISO		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//_MISO

#define FM1702_debug_sendByte(DATA)  //Uart_sendByte(DATA)
 

void FM1702_DelayMs(u16 _MS);
void FM1702_Delay50us(u8 _50us);

void InitiHardware(void);

//Ð´1702¼Ä´æÆ÷
void FM1702_Write(u8 Address,u8 value);

//¶Á1702¼Ä´æÆ÷
u8 FM1702_Read(u8 Address);

#endif
