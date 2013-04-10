#include "Keyboard.h"

/*
	����ļ�API�ӿ�
     ����ͨ��������������صĺ����������ɹ��Ӻ������ṩ��RTTʹ��
*/
#include <rtthread.h>



struct rt_device KeyboardDve;

static rt_err_t  rt_hw_Keyboard_init	(rt_device_t dev)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RT_ASSERT(dev!= RT_NULL);//��ʱ��������
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure); //��ʼ��PPT��
	
	return RT_EOK;
}

static rt_err_t  rt_hw_Keyboard_open	(rt_device_t dev,rt_uint16_t oflag)
{
	RT_ASSERT(dev!= RT_NULL);//��ʱ��������
	RT_ASSERT(oflag==0);		//Ŀǰ��֧�ֲ���
	
	dev->open_flag|=(RT_DEVICE_OFLAG_OPEN|oflag);
	
	return RT_EOK;
}

static rt_err_t  rt_hw_Keyboard_close	(rt_device_t dev)
{
	RT_ASSERT(dev!= RT_NULL);//��ʱ��������
	
	dev->open_flag=RT_DEVICE_OFLAG_CLOSE;
	
	return RT_EOK;
}

static rt_size_t rt_hw_Keyboard_read	(rt_device_t dev,rt_off_t pos, void* buffer,rt_size_t size)
{
	u8 *pState;
	RT_ASSERT(dev!= RT_NULL);//��ʱ��������
	
	pState=(u8 *)buffer;
	*pState=GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2);
	
	return 1;//���� size
}

static rt_size_t rt_hw_Keyboard_write	(rt_device_t dev,rt_off_t pos, const void* buffer,rt_size_t size)
{
	RT_ASSERT(dev!= RT_NULL);//��ʱ��������
	RT_ASSERT(0);
	return 0;//���� size
}

static rt_err_t  rt_hw_Keyboard_control	(rt_device_t dev,rt_uint8_t cmd, void *args)
{
	RT_ASSERT(dev!= RT_NULL);//��ʱ��������
	
	return RT_EOK;
}

rt_err_t rt_hw_Keyboard_register(rt_device_t dev, const char* name, rt_uint32_t flag)
{
	RT_ASSERT(dev!= RT_NULL);	//��ʱ��������
	
	dev->type 		= RT_Device_Class_Unknown;
	dev->rx_indicate 	= RT_NULL;
	dev->tx_complete	= RT_NULL;
	dev->init 		= rt_hw_Keyboard_init;
	dev->open		= rt_hw_Keyboard_open;
	dev->close		= rt_hw_Keyboard_close;
	dev->read 		= rt_hw_Keyboard_read;
	dev->write 		= rt_hw_Keyboard_write;
	dev->control 		= rt_hw_Keyboard_control;
	dev->user_data	= RT_NULL;

   return rt_device_register(dev, name, RT_DEVICE_FLAG_RDONLY | flag);	  //ע�ᵽRTT��������
   
}

struct rt_device KeyboardDevice;

void Keyboard_Register(void)
{
	rt_hw_Keyboard_register(&KeyboardDevice,"Keyboard",RT_DEVICE_FLAG_DEACTIVATE);
}


#include "LED_my.h"
#include "beep.h"

void Keyboard_thread_entery(void* parameter)
{
rt_device_t dev,record;
	u8 PPTstate=0;     		//PPT��ǰ״̬
	u8 LastPPTstate=0;  	//PPT�ϴ�״̬
	rt_size_t size=0;
	dev=rt_device_find("Keyboard");		 	//��ȡkeyboard�豸
	RT_ASSERT(dev != RT_NULL);
	dev->init(dev);				  			//��ʼ��keyboard
	dev->open(dev,0);			  			//��keyboard
	record=rt_device_find("record");	//��ȡencoder�豸
	RT_ASSERT(record!= RT_NULL);//��ʱ��������

	while(1)
	{
		
				
		dev->read(dev,0,&PPTstate,size);			//��ȡPPT��״̬
		if(PPTstate !=0)	 //ƽʱ״̬
		{
			if(LastPPTstate == 0)
			{
				//rt_kprintf("PPT �ɿ�\r\n");
				rt_hw_led_off(1);
				LastPPTstate=1; //��¼��ǰPPT״̬
				record->close(record);		//�ر�Encoder�豸
			}
		}
		else		   	//PPT����״̬
		{
			if(LastPPTstate != 0)
			{
				//rt_kprintf("PPT ����\r\n");
				rt_hw_led_on(1);
				rt_sem_release(BeepSem);
				LastPPTstate=0; //��¼��ǰPPT״̬
				record->open(record,RT_DEVICE_OFLAG_RDONLY);		  //��Encoder�豸����ʼAD����������ѹ�������͵�����1
			}
		}
		
		rt_thread_delay( RT_TICK_PER_SECOND/2);  //��ʱ0.5s����PPT������
		
	}

}

