#include "RecordAPP.h"


#include "Record_drive.h"
#include <speex/speex.h>

#include "GNSS_Terminal.h"

/*
	这个文件API接口
     这里通过调用于语音相关的函数，构建成钩子函数，提供给RTT使用
*/

/*60seconds contain 3000 frames of 20ms, and every 20ms will be encoded into 20bytes so 1min=60000bytes */
#define ENCODED_FRAME_SIZE      20
#define FRAME_SIZE              160


char out_bytes[ENCODED_FRAME_SIZE];		   	//输出字节  即：压缩后输出的数据
void *enc_state;

//参数
int quality = 4; 
int complexity=1; 
int vbr=0; 

static SpeexBits EncoderBits;



rt_mailbox_t recMB;


struct rt_thread record_thread;					 	//record线程指针
rt_uint8_t record_stack[Record_THREAD_STACK_SIZE]; 	//record的stack


rt_device_t record;

volatile rt_int16_t Buffer[2][FRAME_SIZE];

rt_uint8_t currentIndex=0;


static rt_err_t record_input2(rt_device_t dev,rt_size_t size)
{
	static rt_uint16_t i=0;
	
	Buffer[currentIndex][i]=(size)^0x8000;
	
	i++;
	
	if(i==FRAME_SIZE)
	{
		i=0;
		if(currentIndex)
		{
			rt_mb_send(recMB,1<<2);
			i=0;
			currentIndex=0;
		}
		else
		{
			rt_mb_send(recMB,1<<1);
			i=0;
			currentIndex=1;
		}
	}
	
	return RT_EOK;
}

#include "Global.h"

void Record_thread_entery(void* parameter)
{	 
	rt_uint32_t Value;
	rt_uint32_t Result=0;
	
	
	/* Speex encoding initializations */ 
	speex_bits_init(&EncoderBits);
	enc_state = speex_encoder_init(&speex_nb_mode);			       		//narrowband
	speex_encoder_ctl(enc_state, SPEEX_SET_VBR, &vbr);			   		//variable bit-rate(VBR) disable
	speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY,&quality);		   	//0~10 
	speex_encoder_ctl(enc_state, SPEEX_SET_COMPLEXITY, &complexity); 	//best 
	
	recMB=rt_mb_create("recMB",5,RT_IPC_FLAG_FIFO);
	
	record=rt_device_find("record");
 	if(!record)
	{
		rt_kprintf("find record fail\n");
		return ;
	}

	record->rx_indicate=record_input2;
	record->init(record);
	//record->open(record,RT_DEVICE_OFLAG_RDONLY); //it was opened at keyboard thread
	
	while(1)
	{

		rt_mb_recv(recMB,&Result,RT_WAITING_FOREVER);
		
		
		if(Result==(1<<1))
		{
			/* Flush all the bits in the struct so we can encode a new frame */
			speex_bits_reset(&EncoderBits);
			/* Encode the frame */
			speex_encode_int(enc_state, (spx_int16_t*)Buffer[0], &EncoderBits);	//从IN_Buffer[0]中读出AD采样的数据进行压缩
			/* Copy the bits to an array of char that can be decoded */
			speex_bits_write(&EncoderBits, (char *)out_bytes, ENCODED_FRAME_SIZE);	 //写到编码输出缓冲区
			
		}
		else if(Result==(1<<2)) 
		{
			/* Flush all the bits in the struct so we can encode a new frame */
			speex_bits_reset(&EncoderBits);
			/* Encode the frame */
			speex_encode_int(enc_state, (spx_int16_t*)Buffer[1], &EncoderBits);	//从IN_Buffer[0]中读出AD采样的数据进行压缩
			/* Copy the bits to an array of char that can be decoded */
			speex_bits_write(&EncoderBits, (char *)out_bytes, ENCODED_FRAME_SIZE);	 //写到编码输出缓冲区
		}
		
		//获取鉴权成功事件
		rt_event_recv(&GlobalEvent,AUTHENTCATED_EVENT,RT_EVENT_FLAG_OR,RT_WAITING_FOREVER,&Value);
		if(Value&AUTHENTCATED_EVENT)
		{

			GNSS_T_Voice(out_bytes,ENCODED_FRAME_SIZE);
		}
		 
	}
	record->close(record);
	rt_kprintf("record close\n");
}
