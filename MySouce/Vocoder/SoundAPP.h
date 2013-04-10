#ifndef _SoundAPP_H_BAB
#define _SoundAPP_H_BAB

#include"VocoderCommon.h"
#include <rtthread.h>





rt_err_t CopyToDecoderBuf(u8 *pData);
void Decoder_thread_entery(void* parameter);


#define Decoder_THREAD_STACK_SIZE 2048
extern struct rt_thread Decoder_thread;
extern rt_uint8_t Decoder_stack[Decoder_THREAD_STACK_SIZE];

#endif
