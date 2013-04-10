#ifndef _DecoderAPI_H_BAB
#define _DecoderAPI_H_BAB

#include"VocoderCommon.h"
#include <rtthread.h>

struct ReceiveEncodeDataTypedef
{
	u16 Ready;
	s8 Data[20];
	struct ReceiveEncodeDataTypedef *next;
	
};//Ҫ���������

//extern struct ReceiveEncodeDataTypedef ReceiveEncodeData[100];
//extern struct ReceiveEncodeDataTypedef *pReadEncodeData;
//extern struct ReceiveEncodeDataTypedef *pWriteEncodeData;


//extern __IO int16_t *outBuffer;	 //�������0ָ��
//extern __IO int16_t OUT_Buffer[2][FRAME_SIZE];		 //�������
//extern int enh;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */	 //���ò���
//extern void *dec_state;/* Holds the states of the encoder & the decoder */	 //��ָ��

//extern struct rt_device Decoder;

//extern rt_mailbox_t DecoderMB;	//����

void Decoder_Register(void);
rt_err_t CopyToDecoderBuf(u8 *pData);
void Decoder_thread_entery(void* parameter);


#define Decoder_THREAD_STACK_SIZE 2048
extern struct rt_thread Decoder_thread;
extern rt_uint8_t Decoder_stack[Decoder_THREAD_STACK_SIZE];


#endif
