#ifndef _Camera_drive_H_BAB
#define _Camera_drive_H_BAB

#include "rtthread.h" 

#define dImagePacketSizeH 0x04
#define dImagePacketSizeL 0x00

#define dTakePhoto160128CMD 		1
#define dTakePhoto320240CMD 		2
#define dTakePhoto640480CMD 		3
#define dReadNextPhotoDataCMD 		4
#define dReadCurrentPhotoDataCMD 	5

#define dOK 			0
#define dErrcheckSum 	1
#define dPartOfdata  	2
#define dEndChars		3
#define dNoneDatapart	4

#define dCameraBufMaxLen 	2048
#define cameraBufSize 		2048

typedef struct
{
	rt_uint8_t buf[cameraBufSize+11]; 
	rt_uint16_t size;
}CameraDataTypedef;


extern struct rt_device CameraDevice;

void Camera_Register(void);




#endif
