#include "Keyboard.h"

/*
	这个文件API接口
     这里通过调用于语音相关的函数，构建成钩子函数，提供给RTT使用
*/
#include <rtthread.h>



struct rt_device KeyboardDve;

static rt_err_t  rt_hw_Keyboard_init	(rt_device_t dev)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RT_ASSERT(dev!= RT_NULL);//假时产生断言
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure); //初始化PPT键
	
	return RT_EOK;
}

static rt_err_t  rt_hw_Keyboard_open	(rt_device_t dev,rt_uint16_t oflag)
{
	RT_ASSERT(dev!= RT_NULL);//假时产生断言
	RT_ASSERT(oflag==0);		//目前不支持参数
	
	dev->open_flag|=(RT_DEVICE_OFLAG_OPEN|oflag);
	
	return RT_EOK;
}

static rt_err_t  rt_hw_Keyboard_close	(rt_device_t dev)
{
	RT_ASSERT(dev!= RT_NULL);//假时产生断言
	
	dev->open_flag=RT_DEVICE_OFLAG_CLOSE;
	
	return RT_EOK;
}

static rt_size_t rt_hw_Keyboard_read	(rt_device_t dev,rt_off_t pos, void* buffer,rt_size_t size)
{
	u8 *pState;
	RT_ASSERT(dev!= RT_NULL);//假时产生断言
	
	pState=(u8 *)buffer;
	*pState=GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2);
	
	return 1;//返回 size
}

static rt_size_t rt_hw_Keyboard_write	(rt_device_t dev,rt_off_t pos, const void* buffer,rt_size_t size)
{
	RT_ASSERT(dev!= RT_NULL);//假时产生断言
	RT_ASSERT(0);
	return 0;//返回 size
}

static rt_err_t  rt_hw_Keyboard_control	(rt_device_t dev,rt_uint8_t cmd, void *args)
{
	RT_ASSERT(dev!= RT_NULL);//假时产生断言
	
	return RT_EOK;
}

rt_err_t rt_hw_Keyboard_register(rt_device_t dev, const char* name, rt_uint32_t flag)
{
	RT_ASSERT(dev!= RT_NULL);	//假时产生断言
	
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

   return rt_device_register(dev, name, RT_DEVICE_FLAG_RDONLY | flag);	  //注册到RTT，并返回
   
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
	u8 PPTstate=0;     		//PPT当前状态
	u8 LastPPTstate=0;  	//PPT上次状态
	rt_size_t size=0;
	dev=rt_device_find("Keyboard");		 	//获取keyboard设备
	RT_ASSERT(dev != RT_NULL);
	dev->init(dev);				  			//初始化keyboard
	dev->open(dev,0);			  			//打开keyboard
	record=rt_device_find("record");	//获取encoder设备
	RT_ASSERT(record!= RT_NULL);//假时产生断言

	while(1)
	{
		
				
		dev->read(dev,0,&PPTstate,size);			//读取PPT的状态
		if(PPTstate !=0)	 //平时状态
		{
			if(LastPPTstate == 0)
			{
				//rt_kprintf("PPT 松开\r\n");
				rt_hw_led_off(1);
				LastPPTstate=1; //记录当前PPT状态
				record->close(record);		//关闭Encoder设备
			}
		}
		else		   	//PPT按下状态
		{
			if(LastPPTstate != 0)
			{
				//rt_kprintf("PPT 按下\r\n");
				rt_hw_led_on(1);
				rt_sem_release(BeepSem);
				LastPPTstate=0; //记录当前PPT状态
				record->open(record,RT_DEVICE_OFLAG_RDONLY);		  //打开Encoder设备，开始AD采样和语言压缩并发送到串口1
			}
		}
		
		rt_thread_delay( RT_TICK_PER_SECOND/2);  //延时0.5s，对PPT键采样
		
	}

}

