/*************************************************************
*文件名称：queue.c
*创建日期：2013年06月30日
*完成日期：2013年06月30日
*修改日期：
*作    者：sjm
**************************************************************/
#include "stm32f4xx.h"
#include "queue.h"
//#include "sysinit.h"

/*************************************************************
*函数名称：CreateQueue
*除数功能：创建一个数据队列
*输入参数：buf,length
*输出参数：无
*作    者：sjm
**************************************************************/
void CreateQueue(struct QueuePara *Queue,unsigned short length)
{
	Queue->Length = length;		//队列最大数据数
	Queue->Index = 0;				//队列起始头指针
	Queue->Cnt = 0;				//队列当前数据个数
}

/*************************************************************
*函数名称：GetDataFromQueue
*除数功能：从队列中获得一个数据
*输入参数：buf
*输出参数：数据
*作    者：sjm
**************************************************************/
unsigned char GetDataFromQueue(struct QueuePara *Queue,unsigned char *buf)
{
	unsigned char i;
//	__set_PRIMASK(1);
	i = buf[Queue->Index];			//取得数据
	if(Queue->Cnt > 0)
	  Queue->Cnt--;					//数据个数减1
	Queue->Index++;					//队列头加1
	if(Queue->Index >= Queue->Length)
	{
		Queue->Index = 0;				//调整队列头指针
	}
//	__set_PRIMASK(0);
	return(i);					//返回取得的数据
}

/*************************************************************
*函数名称：InsertDataToQueue1
*除数功能：向队列插入一个数据
*输入参数：buf，dat
*输出参数：操作状态
*作    者：sjm
**************************************************************/
unsigned char InsertDataToQueue1(struct QueuePara *Queue,unsigned char *buf,unsigned char dat)
{
	unsigned short i;
	
	if(Queue->Cnt >= Queue->Length)		//队列满
	{
		return QUEUE_FULL;			//返回队列满错误码
	}
	else
	{	
//		__set_PRIMASK(1);
		if((Queue->Index + Queue->Cnt) < Queue->Length)	//查找队尾指针
		{
			i = Queue->Index + Queue->Cnt;
		}
		else
		{
			i = Queue->Index + Queue->Cnt - Queue->Length;
		}
		buf[i] = dat;						//插入数据到队尾
		Queue->Cnt++;							//数据个数加1
//		__set_PRIMASK(0);
		return QUEUE_OK;					//返回操作成功标志
	}
} 

/*************************************************************
*函数名称：InsertDataToQueue
*除数功能：向队列插入一个数据
*输入参数：buf，dat
*输出参数：操作状态
*作    者：sjm
**************************************************************/
unsigned char InsertDataToQueue(struct QueuePara *Queue,unsigned char *buf,unsigned char dat)
{
	unsigned short i;
	
	#ifdef DebugUART1
	if(Queue==QueueTXChannel[0])
		InsertDataToQueue1(QueueTXChannel[1],BufferTXChannel[1],dat);
	#endif
	
	#ifdef DebugUART5
	if(Queue==QueueTXChannel[4])
		InsertDataToQueue1(QueueTXChannel[1],BufferTXChannel[1],dat);
	#endif
	
	if(Queue->Cnt >= Queue->Length)		//队列满
	{
		return QUEUE_FULL;			//返回队列满错误码
	}
	else
	{	
//		__set_PRIMASK(1);
		if((Queue->Index + Queue->Cnt) < Queue->Length)	//查找队尾指针
		{
			i = Queue->Index + Queue->Cnt;
		}
		else
		{
			i = Queue->Index + Queue->Cnt - Queue->Length;
		}
		buf[i] = dat;						//插入数据到队尾
		Queue->Cnt++;							//数据个数加1
//		__set_PRIMASK(0);
		return QUEUE_OK;					//返回操作成功标志
	}
} 

/*************************************************************
*函数名称：GetQueueDataNum
*除数功能：获得队列中元素个数
*输入参数：buf
*输出参数：元素个数
*作    者：sjm
**************************************************************/
unsigned short GetQueueDataNum(struct QueuePara *Queue)
{
	return(Queue->Cnt);			//返回队列中数据个数
}
/*************************************************************
*函数名称：ClearQueue
*除数功能：清空队列
*输入参数：buf
*输出参数：无
*作    者：sjm
**************************************************************/
void ClearQueue(struct QueuePara *Queue)
{
	Queue->Index = 0;				//队列头指针复位
	Queue->Cnt = 0;				//队列数据个数清零
}

