#include "UART1_new.h"
#include "stm32f10x.h"
#include "rtthread.h"

static struct rt_device Uart1_Dev;
static rt_int8_t openCnt=0;	//设备打开次数

/***************************************************************
*** Description:串口管教配置 
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/

static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* Configure USART1 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    
  /* Configure USART1 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}

/***************************************************************
*** Description: 串口中断、DMA中断配置
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/

static void NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTz Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USARTz Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/***************************************************************
*** Description: 串口DMA配置
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/
static u8 buff='0';  //用于防止第一次配置DMA时出错警告
static void DMA_Configuration(void)
{
  DMA_InitTypeDef DMA_InitStructure;
 
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* DMA1_Channel4 (triggered by USART1 Tx event) Config */
  DMA_DeInit(DMA1_Channel4);
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) &USART1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&buff;/////////////////
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize =sizeof(buff);////////////////////////////////
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  
}

static void UART1_MDA_Disable(void)
{
	DMA_Cmd(DMA1_Channel4, DISABLE);		//使能 DMA
}

/***************************************************************
*** Description:串口DMA发送  外部接口函数 
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/

static void USART1_DMA_Wirte(const u8 *pBuff,const int Len)
{
   
   	DMA_Cmd(DMA1_Channel4, DISABLE);		//使能 DMA
	DMA1_Channel4->CNDTR =Len;				//数据个数
	DMA1_Channel4->CMAR =(u32)pBuff;		//数据 开始地址
	
	DMA_Cmd(DMA1_Channel4, ENABLE);			//使能 DMA
}



/****************************************************************************************/
/**
构建一个数据链，分成两个链表；分别是空链表和满链表
每次使用时从空链表中取得一个节点，把数据装满了；再把这个节点挂到满链表中
这样DMA就可以连续从满链表中发送数据了
*/
#define DATA_NODE_BUFF_MAX_LEN 	1024
#define NODE_NUM	5

/**
数据节点
*/
struct Data_Node
{
	u8 buff[DATA_NODE_BUFF_MAX_LEN];
	u16 len;
	struct Data_Node *Next;
};

/**
数据链节点
*/
struct Data_Node_list
{	
	struct Data_Node *Node;
	
	struct Data_Node_list *Pre;	
	struct Data_Node_list *Next;
};

#define USING_MALLC_AS_BUFF		1	//使用动态内存作为缓冲区

struct Data_Node_list empty_list_Head;	///<空链表的头
struct Data_Node_list empty_list_Tail;	///<空链表的尾

struct Data_Node_list full_list_Head;	///<满链表的头
struct Data_Node_list full_list_Tail;	///<满链表的尾

struct Data_Node_list *pCurrentMDASendNode=0;	//当前DMA正在使用的节点

#if USING_MALLC_AS_BUFF==0
struct Data_Node node[NODE_NUM];			//定义NODE_NUM个缓冲节点
#endif

struct Data_Node_list Node_list[NODE_NUM];	//需要NODE_NUM个链表节点来存放数据节点

/**
将装有数据的链节点插入到满链表中
*/
__INLINE void Insert_Node_Full_list(struct Data_Node_list *pNode_list)
{
	full_list_Tail.Pre->Next=pNode_list;
	pNode_list->Pre=full_list_Tail.Pre;
	pNode_list->Next=&full_list_Tail;
	full_list_Tail.Pre=pNode_list;
}

/**
将使用完的链节点回收到空链表中
*/
__INLINE void Insert_Node_Empty_list(struct Data_Node_list *pNode_list)
{
	empty_list_Tail.Pre->Next=pNode_list;
	pNode_list->Pre=empty_list_Tail.Pre;
	pNode_list->Next=&empty_list_Tail;
	empty_list_Tail.Pre=pNode_list;
}

static void Data_Node_list_init(void)
{
	u8 i;
//	struct Data_Node_list *pNode;
	
	//初始化空、满链表头尾
	empty_list_Head.Pre=0;
	empty_list_Head.Node=0;
	empty_list_Head.Next=&empty_list_Tail;
	empty_list_Tail.Node=0;
	empty_list_Tail.Next=0;
	empty_list_Tail.Pre=&empty_list_Head;
	
	full_list_Head.Pre=0;
	full_list_Head.Node=0;
	full_list_Head.Next=&full_list_Tail;
	full_list_Tail.Node=0;
	full_list_Tail.Next=0;
	full_list_Tail.Pre=&full_list_Head;
	
//	rt_kprintf("first init\n");
//	pNode=&empty_list_Head;
//	rt_kprintf("pNode=%X\tPre=%X\tNode=%X\tNext=%X\t\n",pNode,pNode->Pre,pNode->Node,pNode->Next);
//	pNode=&empty_list_Tail;
//	rt_kprintf("pNode=%X\tPre=%X\tNode=%X\tNext=%X\t\n",pNode,pNode->Pre,pNode->Node,pNode->Next);
//	pNode=&full_list_Head;
//	rt_kprintf("pNode=%X\tPre=%X\tNode=%X\tNext=%X\t\n",pNode,pNode->Pre,pNode->Node,pNode->Next);
//	pNode=&full_list_Tail;
//	rt_kprintf("pNode=%X\tPre=%X\tNode=%X\tNext=%X\t\n",pNode,pNode->Pre,pNode->Node,pNode->Next);
	
//	rt_kprintf("first init end\n");
	//把数据节点挂接到空链表上

#if USING_MALLC_AS_BUFF==0	
	for (i=0;i<NODE_NUM;i++)
	{		
		Node_list[i].Node=&node[i];
			
		Insert_Node_Empty_list(&Node_list[i]);
		
	}
#else	
	//使用动态内存
	
	for (i=0;i<NODE_NUM;i++)
	{		
		Node_list[i].Node=rt_malloc(sizeof(struct Data_Node));
			
		Insert_Node_Empty_list(&Node_list[i]);
		
	}
#endif	

//rt_kprintf("device open end\n");
}


/**
从满链中取出一个链节点，没有节点时返回0
*/
static struct Data_Node_list * Get_Full_Node(void)
{
	struct Data_Node_list *pTemp;
		
	if (full_list_Head.Next->Node!=0)
	{
		pTemp=full_list_Head.Next;
		full_list_Head.Next=pTemp->Next;
		pTemp->Next->Pre=&full_list_Head;
		
		return pTemp;
	}
	
	return 0;
		
}
 
/**
从空链中取出一个链节点，没有空节点时返回0
*/
static struct Data_Node_list * Get_Empty_Node(void)
{
	struct Data_Node_list *pTemp;
		
	if (empty_list_Head.Next->Node!=0)
	{
		pTemp=empty_list_Head.Next;
		empty_list_Head.Next=pTemp->Next;
		pTemp->Next->Pre=&empty_list_Head;
		
		return pTemp;
	}
	
	return 0;
}

#if USING_MALLC_AS_BUFF

static void free_node(void)
{
	struct Data_Node_list *pTemp;
	while(1)
	{
		pTemp=Get_Full_Node();
		if(!pTemp) break;
		
		rt_free(pTemp->Node);
	}
	
	while(1)
	{
		pTemp=Get_Empty_Node();
		if(!pTemp) break;
		
		rt_free(pTemp->Node);
	}
	
}

#endif
/////////////////////////////////////////////////////////////////////////////////////////////////

static rt_err_t  USART1_new_hw_init	(rt_device_t dev)
{
  USART_InitTypeDef USART_InitStructure;
         
 if(dev->flag&RT_DEVICE_FLAG_ACTIVATED) return RT_EOK;
 
  /* NVIC configuration */
  NVIC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();

  /* Configure the DMA */
  DMA_Configuration();

/* USART1 and USARTz configuration -------------------------------------------*/
  /* USART1 and USARTz configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);
 
  /* Enable USART1 DMA TX request */
  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
  DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);

  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);

  /* Enable USART1 DMA TX Channel */
 // DMA_Cmd(DMA1_Channel4, ENABLE);

  dev->flag|=RT_DEVICE_FLAG_ACTIVATED;
  
	return RT_EOK;
}

static rt_err_t  USART1_new_hw_open	(rt_device_t dev,rt_uint16_t oflag)
{
	
	RT_ASSERT(dev);
	
	if(dev->open_flag&RT_DEVICE_OFLAG_OPEN) 
	{
		openCnt++;
		return RT_EOK; //判断是否已经打开了，是则直接返回
	}
	openCnt++;
	Data_Node_list_init();
	
	dev->open_flag|=(RT_DEVICE_OFLAG_OPEN|oflag);
	
	return RT_EOK;
}

static rt_err_t  USART1_new_hw_close(rt_device_t dev)
{
	//如果当前还有数据发送时，返回忙状态

	if(dev->open_flag==RT_DEVICE_OFLAG_CLOSE)	return RT_EOK; //判断是否已经关闭了，是则直接返回
	if(openCnt>1)
	{
		openCnt--;
		return RT_EOK;
	}
	
	openCnt=0;
	
	if(pCurrentMDASendNode)
	{
		return RT_EBUSY;
	}
	
	USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);

	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	UART1_MDA_Disable();
	USART_Cmd(USART1, DISABLE);
	
	#if USING_MALLC_AS_BUFF
	free_node();
	#endif
	
	dev->open_flag=RT_DEVICE_OFLAG_CLOSE;

	return RT_EOK;
}

static rt_size_t USART1_new_hw_read	(rt_device_t dev,rt_off_t pos, void* buffer,rt_size_t size)
{
	return 0;//返回 size
}
/**
1.先从缓冲区申请节点
2.把数据copy到缓冲区
3.判断DMA是否工作，工作则把节点插到full链表中，否则直接DMA发送
*/
static rt_size_t USART1_new_hw_write(rt_device_t dev,rt_off_t pos, const void* buffer,rt_size_t size)
{
	struct Data_Node_list *pNodeData;
		
	RT_ASSERT(dev != RT_NULL);
	RT_ASSERT(buffer != RT_NULL);
	RT_ASSERT(size <= DATA_NODE_BUFF_MAX_LEN);
	
	 pNodeData=Get_Empty_Node();
	 if(!pNodeData)
	 {
		 return 0;
	 }	
	 	 
	rt_memcpy(pNodeData->Node->buff,buffer,size);
	pNodeData->Node->len=size; 
	
	if(!pCurrentMDASendNode)
	{
		pCurrentMDASendNode=pNodeData;
		USART1_DMA_Wirte(pCurrentMDASendNode->Node->buff,pCurrentMDASendNode->Node->len);
	}
	else
	{
		Insert_Node_Full_list(pNodeData);
	}
	

	return size;//返回 size
}



static rt_err_t  USART1_new_hw_control	(rt_device_t dev,rt_uint8_t cmd, void *args)
{
		
	return RT_EOK;
}
static rt_err_t rt_hw_USART1_new_register(rt_device_t device, const char* name, rt_uint32_t flag)
{
   RT_ASSERT(device != RT_NULL);
//
//	if ((flag & RT_DEVICE_FLAG_DMA_RX) ||
//		(flag & RT_DEVICE_FLAG_INT_TX))
//	{
//		RT_ASSERT(0);
//	}

	device->type 		= RT_Device_Class_Char;
	device->rx_indicate = RT_NULL;
	device->tx_complete = RT_NULL;
	device->init 		= USART1_new_hw_init;
	device->open		= USART1_new_hw_open;
	device->close		= USART1_new_hw_close;
	device->read 		= USART1_new_hw_read;
	device->write 	    = USART1_new_hw_write;
	device->control 	= USART1_new_hw_control;
	device->user_data	= RT_NULL;

   return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR | flag);	  //注册到RTT，并返回

}

/***************************************************************
*** Description: 串口初始化  外部接口函数
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/


void USART1_new_register(void)
{
	rt_hw_USART1_new_register(&Uart1_Dev,"uart1",RT_DEVICE_FLAG_RDWR);
}

/***************************************************************
*** Description:串口DMA中断服务  外部接口函数 
*** Author:  lindabell & 欧海
*** Date:
***************************************************************/

void DMA1_Channel4_IRQHandler(void)
{
   /* enter interrupt */
    rt_interrupt_enter();
	DMA_ClearFlag(DMA1_FLAG_TC4);
   
	if (pCurrentMDASendNode)
	{
		Insert_Node_Empty_list(pCurrentMDASendNode);
	}
	
	pCurrentMDASendNode=Get_Full_Node();
	if(pCurrentMDASendNode)
	{
		USART1_DMA_Wirte(pCurrentMDASendNode->Node->buff,pCurrentMDASendNode->Node->len);
	}
	/* leave interrupt */
    rt_interrupt_leave();

}
//////////////////////////////////////////////////////////////////////

/****************************************************************************
// File Name:  串口1中断函数  1)存在BUG因为分不开头7E还是尾7E，当一包数据缺少一个7E将会影响
							  2)当协议存在\r\n时也会出现问题
// Description:
// Author:lindabell@欧海
// Date:
****************************************************************************/

#include "GNSS.h"
#include "GNSS_Splitter.h"
#include "MC323A.h"

void USART1_IRQHandler(void)
{
	u8 temp;
	 /* enter interrupt */
    rt_interrupt_enter();

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		
		USART_ClearFlag(USART1,USART_FLAG_RXNE);		  //清除接收中断标志
		temp=USART_ReceiveData(USART1);

		//判断网络是否就绪了
		if(MC323A_state==MC323A_NET_ENABLE)
		{ 
			if(GNSS_GetDataStream(temp)>0)	//收到一帧数据时发送信号量
			{
				//rt_kprintf("get a fream\n");
				rt_sem_release(&GNSS_R_sem);
			}
		}
		
	
		if(MC323A_Dev.open_flag&RT_DEVICE_OFLAG_OPEN)
		{
			MC323A_Dev.rx_indicate(&MC323A_Dev,temp);
		}

		
	}
	/* leave interrupt */
    rt_interrupt_leave();
}



#ifdef RT_USING_FINSH
#include <finsh.h>

void uart1_test(void)
{
	
	rt_device_t uart1;
	rt_err_t err;
	u32 couter=0;
	
	uart1=rt_device_find("uart1");
	if (!uart1)
	{
		rt_kprintf("can not find uart1 device\n");
	}
	uart1->init(uart1);
	uart1->open(uart1,RT_DEVICE_OFLAG_RDWR);
	
	while (1)
	{
		couter++;
		if(couter==1001)break;
		if(0==uart1->write(uart1,0,"HelloWorld",sizeof("HelloWorld")-1))
		{
			rt_thread_delay(RT_TICK_PER_SECOND/10);
			uart1->write(uart1,0,"HelloWorld",sizeof("HelloWorld")-1);
		}
	}
	
	
	err=uart1->close(uart1);
	if(err!=RT_EOK)
	{
		rt_kprintf("close fail\n");
		rt_thread_delay(RT_TICK_PER_SECOND/10);
		err=uart1->close(uart1);
		if(err!=RT_EOK) rt_kprintf("close fail again\n");
	}

	rt_kprintf("end\n");
}


FINSH_FUNCTION_EXPORT(uart1_test, Uart1 driver test);
#endif

#ifdef RT_USING_FINSH
#include <finsh.h>

void node_test(void)
{
	struct Data_Node_list *pNode;
	u8 couter=0;
	
	Data_Node_list_init();
	
	rt_kprintf("get empty list\n");
	
	while (1)
	{
		pNode=Get_Empty_Node();
		
		if (!pNode) break;
		couter++;
		rt_kprintf("pNode=%X\tPre=%X\tNode=%X\tNext=%X\t\n",pNode,pNode->Pre,pNode->Node,pNode->Next);
		Insert_Node_Full_list(pNode);
	}
	
	rt_kprintf("get full list\n");
	
	while (1)
	{
		pNode=Get_Full_Node();
		
		if (!pNode) break;
		couter++;
		rt_kprintf("pNode=%X\tPre=%X\tNode=%X\tNext=%X\t\n",pNode,pNode->Pre,pNode->Node,pNode->Next);
		Insert_Node_Empty_list(pNode);
	}
	
	#if USING_MALLC_AS_BUFF

	free_node();

	#endif
}


FINSH_FUNCTION_EXPORT(node_test, Uart1 driver test);
#endif


