#include "SoundAPP.h"


int enh=1;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */	 //���ò���
void *dec_state;/* Holds the states of the encoder & the decoder */	 //��ָ��
static SpeexBits DecoderBits;

//rt_mailbox_t DecoderMB;	//����
struct rt_semaphore DecoderSem;

struct ReceiveEncodeDataTypedef *pReadEncodeData;
struct ReceiveEncodeDataTypedef *pWriteEncodeData;
static struct rt_mutex  EDBMutex;
static rt_mutex_t EncodeDataBuffermutex=&EDBMutex;//���Ż���������



struct ReceiveEncodeDataTypedef
{
	u16 Ready;
	s8 Data[20];
	struct ReceiveEncodeDataTypedef *next;
	
};//Ҫ���������

static ReceiveBufInit(u16 num)
{
	u16 i=0;
	RT_ASSERT(num<65535);
	
//	EncodeDataBuffermutex=rt_mutex_create("EncodeDataBuffermutex",RT_IPC_FLAG_FIFO);//����������������
rt_mutex_init(EncodeDataBuffermutex,"EDBMutex",RT_IPC_FLAG_FIFO);
	
	pReadEncodeData=(struct ReceiveEncodeDataTypedef *)rt_malloc(sizeof(struct ReceiveEncodeDataTypedef)); //�����ڴ�
	pReadEncodeData->Ready=0;

	pWriteEncodeData=pReadEncodeData;  //��¼����ͷ
	
	for(i=0;i<num-1;i++)
	{
		pReadEncodeData->next=(struct ReceiveEncodeDataTypedef *)rt_malloc(sizeof(struct ReceiveEncodeDataTypedef));//�����ڴ�
		pReadEncodeData=pReadEncodeData->next;
		pReadEncodeData->Ready=0;
	}
	pReadEncodeData->next=pWriteEncodeData;

   pReadEncodeData=pWriteEncodeData;  //��ָ�뿪ʼָ��ͬһ���ڵ�
	
}


/***************************************************************
*** Description:����������copy��decoder�Ļ������� 
*** Author:  lindabell & ŷ��
*** Date:
***************************************************************/
//���⣺ÿ�յ�һ��������copy�����Ż���������ֻ�е������ʱ����decoder�вŻᲥ��
//�ڵ����������ʱ�����Է���ֵ���д������Բ��������û�����������Ч����һ����
rt_err_t CopyToDecoderBuf(u8 *pData)
{
	static u8 i;
	static rt_err_t err=RT_EOK;
//	RT_ASSERT(pWriteEncodeData->Ready == 0);//decoderû�л�������
	err=rt_mutex_take(EncodeDataBuffermutex,RT_WAITING_FOREVER);
	RT_ASSERT(err == RT_EOK);
	if(pWriteEncodeData->Ready != 0 ) 	  //������hard falut ָ������ �ڻ����ָ���ȡReady��ֵ
	{	
		
		rt_kprintf("������û�пյ�\r\n");
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
*** Description: ������������ں���
*** Author:  lindabell & ŷ��
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
	
	ReceiveBufInit(1000);//��ʼ�����սṹ��
	
	speex_bits_init(&DecoderBits);
  dec_state = speex_decoder_init(&speex_nb_mode);				   //�����ʼ��
  if(dec_state!=0)
  {
   speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);			   //��������
  }
	
	
	
	device=rt_device_find("sound");
	RT_ASSERT(device!= RT_NULL);//��ʱ��������
//	DecoderMB=rt_mb_create("DecoderMB",5,RT_IPC_FLAG_FIFO);	//����
	
	rt_sem_init(&DecoderSem,"DecoderSem",0,RT_IPC_FLAG_FIFO);
	
	HalfLeftEvent=rt_event_create("HalfLeftEvent",RT_IPC_FLAG_FIFO);
    
	sound=(struct sound_device *)device;
	device=&sound->parent;
	
	device->tx_complete=complete;
	device->control(device,1,HalfLeftEvent);
	device->init(device);	 //��ʼ��Ӳ��
	device->open(device,0);	 //���豸	
	 	
	while(1)
	{

		
	//	rt_mb_recv(DecoderMB,&value,RT_WAITING_FOREVER);   //�ȴ���������
		rt_sem_take(&DecoderSem,RT_WAITING_FOREVER);

		err=rt_mutex_take(EncodeDataBuffermutex,RT_WAITING_FOREVER);
		RT_ASSERT(err == RT_EOK);
		
		if(pReadEncodeData->Ready ==0) 		//������û�о�����δ��ѹ����������
		{
			
			err=rt_mutex_release(EncodeDataBuffermutex);
			RT_ASSERT(err == RT_EOK);
			continue;
		}
		//���绺����û�о����õ����ݣ�ֱ������

			
		//rt_kprintf("decoder\n");
		/* Copy the encoded data into the bit-stream struct */
		speex_bits_read_from(&DecoderBits,pReadEncodeData->Data, ENCODED_FRAME_SIZE);	 //�ӱ��뻺������������
		/* Decode the data */
		speex_decode_int(dec_state, &DecoderBits, (spx_int16_t*)tempBuffer);	//����
		len=device->write(device,0,tempBuffer,FRAME_SIZE*2);
		
		
		if(len<(FRAME_SIZE*2))
		{
			device->open(device,0);	 //���豸
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
