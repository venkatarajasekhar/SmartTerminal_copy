#ifndef _DecoderAPI_H_BAB
#define _DecoderAPI_H_BAB

#include"VocoderCommon.h"
#include <rtthread.h>

struct ReceiveEncodeDataTypedef
{
	u16 Ready;
	s8 Data[20];
	struct ReceiveEncodeDataTypedef *next;
	
};//要译码的数据

//extern struct ReceiveEncodeDataTypedef ReceiveEncodeData[100];
//extern struct ReceiveEncodeDataTypedef *pReadEncodeData;
//extern struct ReceiveEncodeDataTypedef *pWriteEncodeData;


//extern __IO int16_t *outBuffer;	 //输出缓冲0指针
//extern __IO int16_t OUT_Buffer[2][FRAME_SIZE];		 //输出缓冲
//extern int enh;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */	 //设置参数
//extern void *dec_state;/* Holds the states of the encoder & the decoder */	 //空指针

//extern struct rt_device Decoder;

//extern rt_mailbox_t DecoderMB;	//邮箱

void Decoder_Register(void);
rt_err_t CopyToDecoderBuf(u8 *pData);
void Decoder_thread_entery(void* parameter);


#define Decoder_THREAD_STACK_SIZE 2048
extern struct rt_thread Decoder_thread;
extern rt_uint8_t Decoder_stack[Decoder_THREAD_STACK_SIZE];


#endif
