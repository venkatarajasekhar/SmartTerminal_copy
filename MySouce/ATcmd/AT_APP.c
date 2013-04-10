#include "AT_APP.h"

#define AT_CMD_MAX_LEN	100

struct ReceiveATcmdBufTypdef
{
	rt_uint8_t Str[AT_CMD_MAX_LEN];
	rt_uint8_t Ready;
	struct ReceiveATcmdBufTypdef *next;
};

struct ReceiveATcmdBufTypdef *pWrite;
struct ReceiveATcmdBufTypdef *pRead;

	//为了优化速度，使用内部RAM接收再转移到外部SRAM环形链表中

//在这里列出AT指令,在指令必须加入回车符
//因为从GPRS发来的数据最后又\r和\n；而是用scan（）函数在等到的数据中还保留着\r；所以
//在命令中必须加入\r，否则比较会出错
//当只要比较前面的字符则不用加\r，这样的话只要收到的数据前面与列表一样都认为对的
#define Command_list_num 24  //命令列表的个数
const rt_uint8_t *AT_Command_list[]=
{
 "OK\r",		  		//0 正确
 "ERROR\r",		  	//1 错误
 "\r",			 	//2 只收到换行
 "^RSSILVL:0\r",  	//3 接收信号强度  无信号
 "^RSSILVL:20\r", 	//4 接收信号强度  1格
 "^RSSILVL:40\r", 	//5 接收信号强度  2格
 "^RSSILVL:60\r", 	//6 接收信号强度  3格
 "^RSSILVL:80\r", 	//7 接收信号强度  4格
 "^RSSILVL:99\r", 	//8 接收信号强度  5格
 "^MODE:0\r",	  		//9 系统模式变化指示  无服务 
 "^MODE:2\r",	  		//10 系统模式变化指示  CDMA 模式 
 "AT^IPINIT\r",	  	//11 TCP/UDP连接初始化
 "AT^IPINIT?\r",  	//12 TCP/UDP连接初始化? 将返回连接是否成功和IP等
 "^IPINIT:0",	  		//13 
 "^IPINIT:1",	  		//14
 "AT^IPOPEN=",	  	//15
 "+CME ERROR:",   	//16  返回错误代码
 "AT^IPSEND=3,",		//17
 "AT^IPENTRANS=1\r",	//18
 "^SYSINFO:",			//19
 "^DSDORMANT: 1\r",		//20 睡眠
 "^IPSTATE: 1,0,remote close\r", //21 网络断开
 ">\r",				//22 设置 Phone Number OK
 "^HCMGSS:"			//23 已发送短信条数
};
/****************************************************************************
// File Name: 
// Description:字符串比较函数 ，是根据 \0来判断是否字符串结束的
// Author:lindabell@欧海
// Date:
****************************************************************************/

rt_uint8_t StrCompare(const rt_uint8_t *str1,const rt_uint8_t *str2)
{
	while(1)
	{
		if((*str1=='\0')|(*str2=='\0')) break; //对比完成
		if(*str1!=*str2) return 0;  //字符不相等
		str1++;
		str2++;
	}
	return 1;
}

/****************************************************************************
// File Name: 
// Description:对收到的字符串进行译码
               译码匹配，返回匹配值；否则返回0xFF
// Author:lindabell@欧海
// Date:
****************************************************************************/

static rt_uint8_t DecodeCommand(rt_uint8_t *Str)
{
	rt_uint8_t i;
	for(i=0;i<Command_list_num;i++)
	{
		if(StrCompare(AT_Command_list[i],Str)) //字符串匹配，退出
		{
			return i;
		}
	}
	return 0xff;
}


void ATcmdBuf_init(rt_uint8_t nodeNum)
{
	rt_uint8_t i;
	
	for (i=0;i<nodeNum;i++)
	{
		if(i==0)
		{
			pWrite=rt_malloc(sizeof(struct ReceiveATcmdBufTypdef));	
			pWrite->Ready=0;
			pRead=pWrite;
		}
		else
		{
			pWrite->next=rt_malloc(sizeof(struct ReceiveATcmdBufTypdef));
			pWrite=pWrite->next;
			pWrite->Ready=0;
		}		
		
	}
	pWrite->next=pRead;
	pWrite=pRead;
}

struct rt_semaphore ATsem;

static rt_err_t MC323A_RX(rt_device_t dev,rt_size_t size)
{
	static rt_uint8_t Buf[AT_CMD_MAX_LEN];
	static rt_uint8_t index=0;
	static rt_uint8_t i;
	
	
	if(size==0x7E)
	{
		index=0;
		return RT_EOK;
	}
	
	Buf[index]=size;
	if(Buf[index]=='\n')
	{
		if(Buf[index-1]=='\r')
		{
			for(i=0;i<=index;i++)pWrite->Str[i]=Buf[i];
			pWrite->Str[i]='\0';
			pWrite->Ready=1;
			pWrite=pWrite->next;
			rt_sem_release(&ATsem);
			index=0;
			return RT_EOK;
		}
	}
	index++;
	if(index>=AT_CMD_MAX_LEN) index=0;
	
	return RT_EOK;
}
#include "dfs_posix.h"
/**
read ip Address and port from SD card file 
the frommat is:"192.168.255.255:65535"   note:include \0
*/
static rt_int8_t readIPAddrFromfile(const char *file,  char * const pIpAddrBuf,char *const pIpPort)
{
	struct stat fState;
	int i;
	int fd;

	char *pBuf;
	pBuf=rt_malloc(30);
	RT_ASSERT(pBuf>0)
	fd=stat(file,&fState);	//read the file state
	RT_ASSERT(fd>=0)
	rt_kprintf("ip.dat size:%d\n",fState.st_size);
	RT_ASSERT(fState.st_size<=22) //192.168.255.255:65535 包好/0也最多是22bytes
	fd=open(file,O_RDONLY,0);
	RT_ASSERT(fd>=0)
	read(fd,pBuf,fState.st_size);	//read IP Address string
	close(fd);	
	
	for(i=0;i<fState.st_size;i++) rt_kprintf("%02X ",pBuf[i]);
	
	for(i=0;i<fState.st_size;i++) 
	{
		if(pBuf[i]==':') break;
	}
	RT_ASSERT(i<fState.st_size)
	rt_memcpy(pIpAddrBuf,pBuf,i);
	pIpAddrBuf[i]='\0';

	rt_memcpy(pIpPort,pBuf+i+1,fState.st_size-(i+1));
	
	rt_free(pBuf);
	
	rt_kprintf("IP:%s\n",pIpAddrBuf);
	rt_kprintf("Port:%s\n",pIpPort);
	
	return 0;
}

#include "MC323A.h"

// "AT^IPOPEN=1,\"TCP\",\"113.121.46.53\",6002,8002\r\n"

// #define SERVER_IP 		113.121.41.141
// #define SERVER_PORT		6000
// #define LOCAL_PORT		4000

// #define CMD1(x,y,z)		"AT^IPOPEN=1,\"TCP\",\""###x"\","###y","###z"\r\n"
// #define CMD2(x,y,z)	CMD1(x,y,z)
// #define AT_OPEN_CMD	CMD2(SERVER_IP,SERVER_PORT,LOCAL_PORT)

const rt_uint8_t SMSEnd[]={0x00,0x1A};

#include "Global.h"
#include "UnicodeToGB2312.h"

void AT_thread_entry(void* parameter)
{
	rt_uint32_t Value;
	static rt_uint8_t state=0;
	rt_device_t MC323A;
	rt_uint8_t IpSel=0;

	ATcmdBuf_init(5);
	rt_sem_init(&ATsem,"ATsem",0,RT_IPC_FLAG_FIFO);

	
	MC323A=rt_device_find("MC323A");
	if(!MC323A)
	{
		rt_kprintf("find MC323A fail\n");
		RT_ASSERT(0);
	}
	MC323A->rx_indicate=MC323A_RX;

	MC323A->init(MC323A);

	
	MC323A->open(MC323A,RT_DEVICE_OFLAG_RDWR);
		
//不联网时直接转到成功去执行
#ifdef BABdebug
goto debug_in;
#endif 
	while (1)
	{
		rt_sem_take(&ATsem,RT_WAITING_FOREVER);
		if(pRead->Ready==0) continue;

			
		Value=DecodeCommand(pRead->Str);
		//rt_kprintf("Value=%d\n",Value);
		pRead->Ready=0;
		pRead=pRead->next;
	
		switch(Value)
		{
			case 0://"OK\r",		  		//0 正确
					if(state==1)
					{
						MC323A->write(MC323A,0,"AT^IPINIT?\r\n",sizeof("AT^IPINIT?\r\n")-1);
						state=2;
					}
					else if(state==2)
					{
						//设置短信参数
						MC323A->write(MC323A,0,"AT^HSMSSS=0,0,6,0\r\n",sizeof("AT^HSMSSS=0,0,6,0\r\n")-1);
						state=3;
					}
					//add SMS common
					else if(state==3)
					{
						//set phone number
						rt_uint16_t *pUnicode;
						rt_uint8_t *pSendBuf;
						pUnicode=rt_malloc(50);
						pSendBuf=rt_malloc(50);
						MC323A->write(MC323A,0,"AT^HCMGS=\"15668061345\"\r\n",sizeof("AT^HCMGS=\"15668061345\"\r\n")-1);
						rt_thread_delay(RT_TICK_PER_SECOND/10);
					
						GB2312ToUnicode_Str(pUnicode,"智能终端启动");
						NetworkByteOrder(pSendBuf,pUnicode,sizeof("智能终端启动")-1);
						MC323A->write(MC323A,0,pSendBuf,sizeof("智能终端启动")-1);
						MC323A->write(MC323A,0,SMSEnd,sizeof(SMSEnd));
					
						state=4;
						rt_free(pUnicode);
						rt_free(pSendBuf);
					}
					else if(state==4)//wait SMS sended success
					{
						state==5;
					}
			else if(state==5)
					{
						//建立TCP连接
						char *pBuf;
						char *pIpAddrStr;
						char *pIpPort;
		TCP_Dial:		pBuf=rt_malloc(100);
						pIpAddrStr=rt_malloc(20);
						pIpPort=rt_malloc(8);
						if(IpSel==0)readIPAddrFromfile("/configure/ip1.dat",pIpAddrStr,pIpPort);	//读取IP地址
						else 		readIPAddrFromfile("/configure/ip2.dat",pIpAddrStr,pIpPort);	//读取IP地址
						rt_sprintf(pBuf,"AT^IPOPEN=1,\"TCP\",\"%s\",%s,8002\r\n",pIpAddrStr,pIpPort);//格式化成AT命令
						MC323A->write(MC323A,0,pBuf,rt_strlen(pBuf));
						rt_free(pBuf);
						rt_free(pIpAddrStr);
						rt_free(pIpPort);
						//MC323A->write(MC323A,0,AT_OPEN_CMD,sizeof(AT_OPEN_CMD)-1);
						state=6;
					}
					else if(state==6)
					{
						//数据到达主动上报
						MC323A->write(MC323A,0,"AT^IPDATMODE=1\r\n",sizeof("AT^IPDATMODE=1\r\n")-1);
						state=7;
					}
					else if(state==7)
					{
						//透明传输
						MC323A->write(MC323A,0,"AT^IPENTRANS=1\r\n",sizeof("AT^IPENTRANS=1\r\n")-1);
						state=8;
					}
					else if(state==8)
					{
						//
						//MC323A->write(MC323A,0,"AT^IPSEND=1,\"AA\"\r\n",sizeof("AT^IPSEND=1,\"AA\"\r\n")-1);
						//设置网络状态为enable
debug_in:				Value=MC323A_NET_ENABLE;
						MC323A->control(MC323A,MC323A_CMD_NET_STATE,&Value);
						//发送通知网络就绪事件
						rt_event_send(&GlobalEvent,NET_CONNET_EVENT);//这里应该通知其他线程，网络已经就绪了，可以发送数据到网络了
						rt_kprintf("Net conneted\n");
						state=9;
					}
				break;
			case 10://^MODE:2
					 //注册模块到网络
					MC323A->write(MC323A,0,"AT^IPINIT=,\"card\",\"card\"\r\n",sizeof("AT^IPINIT=,\"card\",\"card\"\r\n")-1);
					state=1;
				break;
			case 1://"ERROR\r",		  	//1 错误
					//注册模块到网络
					if(state==1)	
					{
						MC323A->write(MC323A,0,"AT^IPINIT=,\"card\",\"card\"\r\n",sizeof("AT^IPINIT=,\"card\",\"card\"\r\n")-1);
						
					}
					else if(state==6)
					{
						if(state==6)IpSel=1;	//由于使用ip1无法连接服务器，使用ip2进行拨号
						//重新复位CDMA，使其进入拨号序列
						MC323A->write(MC323A,0,"AT^RESET\r\n",sizeof("AT^RESET\r\n")-1);
						state=0;
					}
					
				break;
			case 21://"^IPSTATE: 1,0,remote close\r"	 //21 网络断开
					//取消网络就绪状态
					rt_event_recv(&GlobalEvent,NET_CONNET_EVENT,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&Value);//这里应该通知网络断开了
					//设置网络状态为disable
					Value=MC323A_NET_DISABLE;
					MC323A->control(MC323A,MC323A_CMD_NET_STATE,&Value);
					//重新复位CDMA，使其进入拨号序列
					MC323A->write(MC323A,0,"AT^RESET\r\n",sizeof("AT^RESET\r\n")-1);
					state=0;
					
				break;
			case 23://"^HCMGSS:"
				rt_thread_delay(RT_TICK_PER_SECOND/10);
				goto TCP_Dial;
				break;
		}
		
	}
	
	MC323A->close(MC323A);
}
