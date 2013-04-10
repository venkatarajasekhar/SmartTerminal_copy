/**
  �������Ͷ���
*/
#ifndef _GNSS_BaseType_H_BAB
#define _GNSS_BaseType_H_BAB

#include "rtthread.h"

/**
  GNSSЭ�����ͣ������Ƿ��������նˣ�Ҳ����������
*/
#define GNSS_TYPE_Terminal	1	//��Ϊ������
#define GNSS_TYPE_Server	0	//��Ϊ�ն�


#define GNSS_SUPPORT_OS			1 //�Ƿ�֧��OS
#define GNSS_SUPPORT_ASSERT		1 //�Ƿ�֧��OS
#define GNSS_SUPPORT_PRINTF		1 //�Ƿ�֧��printf()����

typedef unsigned char	u8;
typedef	signed	 char	s8;
typedef unsigned short	u16;
typedef signed   short	s16;
typedef	unsigned int	u32;
typedef signed   int	s32;


//�ӿ�ʵ��
#if GNSS_SUPPORT_OS
#define GNSS_malloc(size)  	rt_malloc(size)	//��̬�ڴ����
#define GNSS_free(point)	rt_free(point)	//��̬�ڴ����
#endif

//ʱ��
#include <time.h>
//typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */
#define GNSS_time(t)			time(t) 
#define GNSS_mktime(tm)			mktime(tm)

//��ӡ�ӿ�
#if GNSS_SUPPORT_PRINTF
#include <stdio.h>
#define GNSS_printf(format,...)	rt_kprintf(format,##__VA_ARGS__)
#else
#define GNSS_printf(format,...) 
#endif

#define GNSS_sprintf(buf,format, ...)	rt_sprintf(buf,format,##__VA_ARGS__)
#define GNSS_sscanf(str,format,...)		sscanf(str,format,##__VA_ARGS__)
#define GNSS_memcpy(dst,src,count)		memcpy(dst,src,count)

#if GNSS_SUPPORT_ASSERT
#if !GNSS_SUPPORT_PRINTF
#error GNSS_ASSERT() was base on GNSS_printf(),so GNSS_SUPPORT_PRINTF must be set enable
#endif
#define GNSS_ASSERT(EX)   if (!(EX)) {GNSS_printf("(%s) assert failed at %s:%d \n", \
#EX, __FUNCTION__, __LINE__); while (1);}
#else
#define GNSS_ASSERT(EX) {}
#endif

//�ļ��ӿ�
#include "dfs_posix.h"

#define GNSS_DIR_PATH  "/configure/"	//GNSS deault dir
#define GNSS_Conet(x,y) x#y
#define GNSS_PATH(name) GNSS_Conet(GNSS_DIR_PATH,name)   

#define GNSS_fopen(Filename,OpenFlag,mode)	open(Filename,OpenFlag,mode)
#define GNSS_fread(fd,InBuf,len)			read(fd,InBuf,len)
#define GNSS_fwrite(fd,OutBuf,len)			write(fd,OutBuf,len)
#define GNSS_fclose(fd)						close(fd)
#define GNSS_flseek(fd,Offset,Origin)		lseek(fd,Offset,Origin)
#define GNSS_fstat(fd,stat)					fstat(fd,stat)	
#define GNSS_fopendir(PathName)				opendir(PathName)
#define GNSS_freaddir(d)					readdir(d)
#define GNSS_fclosedir(dir)					closedir(dir)

//GNSS error code
#define GNSS_EOK                          (0)               /**< There is no error       */
#define GNSS_ERROR                        (-1)               /**< A generic error happens */
#define GNSS_ETIMEOUT                     (-2)               /**< Timed out               */
#define GNSS_EFULL                        (-3)               /**< The resource is full    */
#define GNSS_EEMPTY                       (-4)               /**< The resource is empty   */
#define GNSS_ENOMEM                       (-5)               /**< No memory               */
#define GNSS_ENOSYS                       (-6)               /**< No system               */
#define GNSS_EBUSY                        (-7)               /**< Busy                    */
#define GNSS_EIO						  (-8) 				/**< IO error 				 */









#endif
