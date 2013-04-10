#include "SoundAPP.h"


int enh=1;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */	 //设置参数
void *dec_state;/* Holds the states of the encoder & the decoder */	 //空指针
static SpeexBits DecoderBits;

//rt_mailbox_t DecoderMB;	//邮箱
struct rt_semaphore DecoderSem;

struct ReceiveEncodeDataTypedef *pReadEncodeData;
struct ReceiveEncodeDataTypedef *pWriteEncodeData;
static struct rt_mutex  EDBMutex;
static rt_mutex_t EncodeDataBuffermutex=&EDBMutex;//播放缓冲区互斥



struct ReceiveEncodeDataTypedef
{
	u16 Ready;
	s8 Data[20];
	struct ReceiveEncodeDataTypedef *next;
	
};//要译码的数据

static ReceiveBufInit(u16 num)
{
	u16 i=0;
	RT_ASSERT(num<65535);
	
//	EncodeDataBuffermutex=rt_mutex_create("EncodeDataBuffermutex",RT_IPC_FLAG_FIFO);//创建缓冲区互斥量
rt_mutex_init(EncodeDataBuffermutex,"EDBMutex",RT_IPC_FLAG_FIFO);
	
	pReadEncodeData=(struct ReceiveEncodeDataTypedef *)rt_malloc(sizeof(struct ReceiveEncodeDataTypedef)); //申请内存
	pReadEncodeData->Ready=0;

	pWriteEncodeData=pReadEncodeData;  //记录链表头
	
	for(i=0;i<num-1;i++)
	{
		pReadEncodeData->next=(struct ReceiveEncodeDataTypedef *)rt_malloc(sizeof(struct ReceiveEncodeDataTypedef));//申请内存
		pReadEncodeData=pReadEncodeData->next;
		pReadEncodeData->Ready=0;
	}
	pReadEncodeData->next=pWriteEncodeData;

   pReadEncodeData=pWriteEncodeData;  //两指针开始指向同一个节点
	
}


/***************************************************************
*** Description:把语音数据copy到decoder的缓冲区中 
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/
//问题：每收到一个语音包copy到播放缓冲来，而只有当溢出的时候在decoder中才会播放
//在调用这个函数时并不对返回值进行处理；所以不管溢出和没有溢出产生的效果是一样的
rt_err_t CopyToDecoderBuf(u8 *pData)
{
	static u8 i;
	static rt_err_t err=RT_EOK;
//	RT_ASSERT(pWriteEncodeData->Ready == 0);//decoder没有缓冲区了
	err=rt_mutex_take(EncodeDataBuffermutex,RT_WAITING_FOREVER);
	RT_ASSERT(err == RT_EOK);
	if(pWriteEncodeData->Ready != 0 ) 	  //老是有hard falut 指向这里 在汇编中指向读取Ready的值
	{	
		
		rt_kprintf("缓冲区没有空的\r\n");
		rt_mutex_release(EncodeDataBuffermutex);
		return RT_ERROR;
	}
	for(i=0;i<20;i++)pWriteEncodeData->Data[i]=*pData++;
	pWriteEncodeData->Ready=1;
	pWriteEncodeData=pWriteEncodeData->next;
	
	err=rt_mutex_release(EncodeDataBuffermutex);
	RT_ASSERT(err == RT_EOK);
//	rt_kprintf("Got a new voice data\r\n");
	
	rt_sem_release(&DecoderSem);
	return RT_EOK;
}


/***************************************************************
*** Description: 语音解码器入口函数
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/
struct rt_thread Decoder_thread;
rt_uint8_t Decoder_stack[Decoder_THREAD_STACK_SIZE];





#include "Sound_drive.h"
int16_t tempBuffer[FRAME_SIZE];
static rt_uint32_t cnt=0;

static rt_err_t complete(rt_device_t dev, void* buffer)
{
	dev->close(dev);
	cnt=0;
	rt_kprintf("*");
	return RT_EOK;
}
void Decoder_thread_entery(void* parameter)
{
	static rt_err_t err=RT_EOK;
	rt_uint32_t cout=0; 
	rt_uint32_t value; 
	rt_uint32_t len;
	static u8 state=1;
	struct sound_device *sound;
	rt_device_t device;
	rt_event_t HalfLeftEvent;
	
	ReceiveBufInit(1000);//初始化接收结构体
	
	speex_bits_init(&DecoderBits);
  dec_state = speex_decoder_init(&speex_nb_mode);				   //解码初始化
  if(dec_state!=0)
  {
   speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);			   //回音消除
  }
	
	
	
	device=rt_device_find("sound");
	RT_ASSERT(device!= RT_NULL);//假时产生断言
//	DecoderMB=rt_mb_create("DecoderMB",5,RT_IPC_FLAG_FIFO);	//邮箱
	
	rt_sem_init(&DecoderSem,"DecoderSem",0,RT_IPC_FLAG_FIFO);
	
	HalfLeftEvent=rt_event_create("HalfLeftEvent",RT_IPC_FLAG_FIFO);
    
	sound=(struct sound_device *)device;
	device=&sound->parent;
	
	device->tx_complete=complete;
	device->control(device,1,HalfLeftEvent);
	device->init(device);	 //初始化硬件
	device->open(device,0);	 //打开设备	
	 	
	while(1)
	{

		
	//	rt_mb_recv(DecoderMB,&value,RT_WAITING_FOREVER);   //等待邮箱数据
		rt_sem_take(&DecoderSem,RT_WAITING_FOREVER);

		err=rt_mutex_take(EncodeDataBuffermutex,RT_WAITING_FOREVER);
		RT_ASSERT(err == RT_EOK);
		
		if(pReadEncodeData->Ready ==0) 		//缓冲区没有就绪的未解压的语音数据
		{
			
			err=rt_mutex_release(EncodeDataBuffermutex);
			RT_ASSERT(err == RT_EOK);
			continue;
		}
		//假如缓冲区没有就绪好的数据，直接跳过

			
		//rt_kprintf("decoder\n");
		/* Copy the encoded data into the bit-stream struct */
		speex_bits_read_from(&DecoderBits,pReadEncodeData->Data, ENCODED_FRAME_SIZE);	 //从编码缓冲区读出数据
		/* Decode the data */
		speex_decode_int(dec_state, &DecoderBits, (spx_int16_t*)tempBuffer);	//解码
		len=device->write(device,0,tempBuffer,FRAME_SIZE*2);
		
		
		if(len<(FRAME_SIZE*2))
		{
			device->open(device,0);	 //打开设备
			device->control(device,4,0);
			rt_kprintf("%");
			rt_event_recv(HalfLeftEvent,1,RT_EVENT_FLAG_CLEAR|RT_EVENT_FLAG_OR,RT_WAITING_FOREVER,&value);
			rt_kprintf("$");
		}
		
		
			pReadEncodeData->Ready=0;
			pReadEncodeData=pReadEncodeData->next;
			
			err=rt_mutex_release(EncodeDataBuffermutex);
			RT_ASSERT(err == RT_EOK);
	}
  	
}
