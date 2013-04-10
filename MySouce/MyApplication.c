#include "MyApplication.h"
/*
这个是用来初始化我的工程的
只要与RTT不相关，使得更改容易
*/


#include"UART1_new.h"
#include "GNSS_Splitter.h"


#include "UART2.h"
#include "GPS_drive.h"
#include "GPSAPP.h"
#include "Camera_drive.h" 
#include "CameraAPP.h"
#include "LCD_drive.h"
#include "LCDAPP.h"
#include "Adscreen_drive.h"
#include "AdscreenAPP.h"

#include "LED_my.h"
#include "beep.h"
#include "cpuusage.h"
#include "Keyboard.h"
#include "Record_drive.h"
#include "RecordAPP.h"
#include "Sound_drive.h"
#include "SoundAPP.h"
#include "MC323A.h"
#include "AT_APP.h"
#include "Global.h"
   
#include "DecoderAPI.h"


#include "GNSS_APP.h"
#include "RFID_drive.h"
#include "RFID_APP.h"

/**
读配置信息线程

用于完成启动的相关配置的初始化'
*/

static void register_device(void)
{
	
	USART1_new_register();
	//外设设备
	UART2_Register();
	GPS_Register();
	Camera_Register();
	LCD_Register();
	ADS_Register();
	
	Keyboard_Register();
	Record_Register();
//	sound_Register();
	Decoder_Register();
	MC323A_register();
	
	RFID_Register();
}

void MyApplicationInit(void)
{
	rt_err_t result=0;
	rt_thread_t Thread;
	
	//初始化全局变量等
	Global_init();
	//注册devices
	register_device();
	
	//创建线程
	
	Thread = rt_thread_create("Splitter",	GNSS_Splitter_thread_entry, RT_NULL,2048, 10, 20);
	if (Thread != RT_NULL)	rt_thread_startup(Thread);
	
	result=rt_thread_init(&record_thread, "Record",Record_thread_entery,RT_NULL,&record_stack[0], sizeof(record_stack),23, 20);
	if (result == RT_EOK) 	rt_thread_startup(&record_thread);
	
	 result=rt_thread_init(&Decoder_thread, "Decoder", Decoder_thread_entery,RT_NULL,&Decoder_stack[0], sizeof(Decoder_stack),22, 20);
	if (result == RT_EOK)rt_thread_startup(&Decoder_thread);
	
	Thread=rt_thread_create("Keyboard",		Keyboard_thread_entery,		RT_NULL,2048, 27, 10);
	if(Thread!=RT_NULL) rt_thread_startup(Thread);
	 
	Thread=rt_thread_create("AT",			AT_thread_entry,			RT_NULL,2048, 26, 10);
	if(Thread!=RT_NULL) rt_thread_startup(Thread);

	//外部设备
 	 Thread = rt_thread_create("GPSApp",	GPSApp_thread_entry, 		RT_NULL,2048, 29, 10);
	if (Thread != RT_NULL)	rt_thread_startup(Thread); 
	
	Thread = rt_thread_create("CameraApp",	CameraApp_thread_entry, 	RT_NULL,2048, 29, 10);
	if (Thread != RT_NULL)	rt_thread_startup(Thread); 
	
	Thread = rt_thread_create("LCDApp",		LCDApp_thread_entry, 		RT_NULL,2048, 29, 10);
	if (Thread != RT_NULL)	rt_thread_startup(Thread); 
	
	Thread = rt_thread_create("ADSApp",		ADSApp_thread_entry, 		RT_NULL,2048, 29, 10);
	if (Thread != RT_NULL)	rt_thread_startup(Thread); 

//	Thread=rt_thread_create("CpuUsage",		cpu_usage_thread_entery,	RT_NULL,2048, 30, 10);
//	if(Thread!=RT_NULL) rt_thread_startup(Thread);
	
	Thread=rt_thread_create("led",			led_thread_entery,			RT_NULL,0x100,28,  5);
	if(Thread!=RT_NULL) rt_thread_startup(Thread);
	
	Thread=rt_thread_create("beep",			beep_thread_entery,			RT_NULL,0x100,28,  5);
	if(Thread!=RT_NULL) rt_thread_startup(Thread);
	
	Thread=rt_thread_create("rfid",			RFIDAPP_thread_entry,		RT_NULL,0x200,28,  5);
	if(Thread!=RT_NULL) rt_thread_startup(Thread);
///////////////////////////////////////////////////////////////////////////////////////////////
	Thread=rt_thread_create("Register",			Register_thread_entry,	(void *)1,0x2048,28,  5);
	if(Thread!=RT_NULL) rt_thread_startup(Thread);
	
//	Thread=rt_thread_create("mediaUpload",		MultimediaUpload_thread_entry,	0,0x2048,28,  5);
//	if(Thread!=RT_NULL) rt_thread_startup(Thread);


}


	

