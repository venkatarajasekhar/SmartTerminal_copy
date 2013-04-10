#ifndef UART_EXPAND_H
#define UART_EXPAND_H
#include "MyType.h"
#define GM8125_MS(x)      GPIO_WriteBit(GPIOA,GPIO_Pin_4,x)
#define GM8125_STADD0(x)  GPIO_WriteBit(GPIOA,GPIO_Pin_5,x)
#define GM8125_STADD1(x)  GPIO_WriteBit(GPIOA,GPIO_Pin_6,x)
#define GM8125_STADD2(x)  GPIO_WriteBit(GPIOA,GPIO_Pin_7,x)
#define GM8125_RESET(x)   GPIO_WriteBit(GPIOB,GPIO_Pin_1,x)
#define GM8125_SRADD0     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)
#define GM8125_SRADD1     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)
#define GM8125_SRADD2     GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define ParentBaudRate 115200      //母口波特率
#define Databit_10bOR11b 0         //0:10 bit 1:11 bit
#define ChildBaudRate  0X03       //0~4   子口波特率
#define SingleChannel  1          //1 0R 0  单/多串口选择
#define GM8125_RegisterBuf ((Databit_10bOR11b<<3)|ChildBaudRate|0XF0)   //GM8125配置字
///////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    UARTExnon=0,
    UARTEx1=1,
    UARTEx2=2,
    UARTEx3=3,
    UARTEx4=4,
    UARTEx5=5
} UARTxTypeDef;
////////////////////////////////////////////////////////////////////////////////////
void USART2_Configuration_Default(void);
void ConfigureGM8125ChildBaudRate(void);
void init_GM8125(void);
void UART_EX_SendDATA(UARTxTypeDef UARTExx,unsigned char DATA);
void UART_EX_SendMultipleDATAs(UARTxTypeDef UARTExx,unsigned char *DataP,unsigned int num);
U8 UART_EX_ReceiveDATA(void);
#endif
