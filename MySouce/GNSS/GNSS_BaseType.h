/**
  基本类型定义
*/
#ifndef _GNSS_BaseType_H_BAB
#define _GNSS_BaseType_H_BAB

#include "rtthread.h"

/**
  GNSS协议类型，可以是服务器或终端，也可以是两者
*/
#define GNSS_TYPE_Terminal	1	//作为服务器
#define GNSS_TYPE_Server	0	//作为终端


#define GNSS_SUPPORT_OS			1 //是否支持OS
#define GNSS_SUPPORT_ASSERT		1 //是否支持OS
#define GNSS_SUPPORT_PRINTF		1 //是否支持printf()函数

typedef unsigned char	u8;
typedef	signed	 char	s8;
typedef unsigned short	u16;
typedef signed   short	s16;
typedef	unsigned int	u32;
typedef signed   int	s32;


//接口实现
#if GNSS_SUPPORT_OS
#define GNSS_malloc(size)  	rt_malloc(size)	//动态内存分配
#define GNSS_free(point)	rt_free(point)	//动态内存分配
#endif

//时间
#include <time.h>
//typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */
#define GNSS_time(t)			time(t) 
#define GNSS_mktime(tm)			mktime(tm)

//打印接口
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

//文件接口
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
