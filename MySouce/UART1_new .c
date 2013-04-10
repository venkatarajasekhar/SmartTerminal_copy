#include "UART1_new.h"
#include "stm32f10x.h"
#include "rtthread.h"

static struct rt_device Uart1_Dev;
static rt_int8_t openCnt=0;	//�豸�򿪴���

/***************************************************************
*** Description:���ڹܽ����� 
*** Author:  lindabell & ŷ��
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
*** Description: �����жϡ�DMA�ж�����
*** Author:  lindabell & ŷ��
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
*** Description: ����DMA����
*** Author:  lindabell & ŷ��
*** Date:
***************************************************************/
static u8 buff='0';  //���ڷ�ֹ��һ������DMAʱ������
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
	DMA_Cmd(DMA1_Channel4, DISABLE);		//ʹ�� DMA
}

/***************************************************************
*** Description:����DMA����  �ⲿ�ӿں��� 
*** Author:  lindabell & ŷ��
*** Date:
***************************************************************/

static void USART1_DMA_Wirte(const u8 *pBuff,const int Len)
{
   
   	DMA_Cmd(DMA1_Channel4, DISABLE);		//ʹ�� DMA
	DMA1_Channel4->CNDTR =Len;				//���ݸ���
	DMA1_Channel4->CMAR =(u32)pBuff;		//���� ��ʼ��ַ
	
	DMA_Cmd(DMA1_Channel4, ENABLE);			//ʹ�� DMA
}



/****************************************************************************************/
/**
����һ�����������ֳ����������ֱ��ǿ������������
ÿ��ʹ��ʱ�ӿ�������ȡ��һ���ڵ㣬������װ���ˣ��ٰ�����ڵ�ҵ���������
����DMA�Ϳ����������������з���������
*/
#define DATA_NODE_BUFF_MAX_LEN 	1024
#define NODE_NUM	5

/**
���ݽڵ�
*/
struct Data_Node
{
	u8 buff[DATA_NODE_BUFF_MAX_LEN];
	u16 len;
	struct Data_Node *Next;
};

/**
�������ڵ�
*/
struct Data_Node_list
{	
	struct Data_Node *Node;
	
	struct Data_Node_list *Pre;	
	struct Data_Node_list *Next;
};

#define USING_MALLC_AS_BUFF		1	//ʹ�ö�̬�ڴ���Ϊ������

struct Data_Node_list empty_list_Head;	///<�������ͷ
struct Data_Node_list empty_list_Tail;	///<�������β

struct Data_Node_list full_list_Head;	///<�������ͷ
struct Data_Node_list full_list_Tail;	///<�������β

struct Data_Node_list *pCurrentMDASendNode=0;	//��ǰDMA����ʹ�õĽڵ�

#if USING_MALLC_AS_BUFF==0
struct Data_Node node[NODE_NUM];			//����NODE_NUM������ڵ�
#endif

struct Data_Node_list Node_list[NODE_NUM];	//��ҪNODE_NUM������ڵ���������ݽڵ�

/**
��װ�����ݵ����ڵ���뵽��������
*/
__INLINE void Insert_Node_Full_list(struct Data_Node_list *pNode_list)
{
	full_list_Tail.Pre->Next=pNode_list;
	pNode_list->Pre=full_list_Tail.Pre;
	pNode_list->Next=&full_list_Tail;
	full_list_Tail.Pre=pNode_list;
}

/**
��ʹ��������ڵ���յ���������
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
	
	//��ʼ���ա�������ͷβ
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
	//�����ݽڵ�ҽӵ���������

#if USING_MALLC_AS_BUFF==0	
	for (i=0;i<NODE_NUM;i++)
	{		
		Node_list[i].Node=&node[i];
			
		Insert_Node_Empty_list(&Node_list[i]);
		
	}
#else	
	//ʹ�ö�̬�ڴ�
	
	for (i=0;i<NODE_NUM;i++)
	{		
		Node_list[i].Node=rt_malloc(sizeof(struct Data_Node));
			
		Insert_Node_Empty_list(&Node_list[i]);
		
	}
#endif	

//rt_kprintf("device open end\n");
}


/**
��������ȡ��һ�����ڵ㣬û�нڵ�ʱ����0
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
�ӿ�����ȡ��һ�����ڵ㣬û�пսڵ�ʱ����0
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
		return RT_EOK; //�ж��Ƿ��Ѿ����ˣ�����ֱ�ӷ���
	}
	openCnt++;
	Data_Node_list_init();
	
	dev->open_flag|=(RT_DEVICE_OFLAG_OPEN|oflag);
	
	return RT_EOK;
}

static rt_err_t  USART1_new_hw_close(rt_device_t dev)
{
	//�����ǰ�������ݷ���ʱ������æ״̬

	if(dev->open_flag==RT_DEVICE_OFLAG_CLOSE)	return RT_EOK; //�ж��Ƿ��Ѿ��ر��ˣ�����ֱ�ӷ���
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
	return 0;//���� size
}
/**
1.�ȴӻ���������ڵ�
2.������copy��������
3.�ж�DMA�Ƿ�����������ѽڵ�嵽full�����У�����ֱ��DMA����
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
	

	return size;//���� size
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

   return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR | flag);	  //ע�ᵽRTT��������

}

/***************************************************************
*** Description: ���ڳ�ʼ��  �ⲿ�ӿں���
*** Author:  lindabell & ŷ��
*** Date:
***************************************************************/


void USART1_new_register(void)
{
	rt_hw_USART1_new_register(&Uart1_Dev,"uart1",RT_DEVICE_FLAG_RDWR);
}

/***************************************************************
*** Description:����DMA�жϷ���  �ⲿ�ӿں��� 
*** Author:  lindabell & ŷ��
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
// File Name:  ����1�жϺ���  1)����BUG��Ϊ�ֲ���ͷ7E����β7E����һ������ȱ��һ��7E����Ӱ��
							  2)��Э�����\r\nʱҲ���������
// Description:
// Author:lindabell@ŷ��
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

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		
		USART_ClearFlag(USART1,USART_FLAG_RXNE);		  //��������жϱ�־
		temp=USART_ReceiveData(USART1);

		//�ж������Ƿ������
		if(MC323A_state==MC323A_NET_ENABLE)
		{ 
			if(GNSS_GetDataStream(temp)>0)	//�յ�һ֡����ʱ�����ź���
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


