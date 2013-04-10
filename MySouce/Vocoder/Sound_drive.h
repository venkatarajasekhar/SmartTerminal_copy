#ifndef _Sound_drive_H_BAB
#define _Sound_drive_H_BAB

#include "rtthread.h"

//#define DATA_BUF_MAX	2048
#define DATA_BUF_NUM	20

struct node
{
	rt_int16_t Buf[160];
	rt_int16_t len;
};
struct sound_device
{
    /* inherit from rt_device */
    struct rt_device parent;

//    rt_int16_t Buf[DATA_BUF_MAX];
    rt_uint16_t read_index, put_index;

	rt_int16_t Buffer[DATA_BUF_NUM][160];
};

void sound_Register(void);

#endif
