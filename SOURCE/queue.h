/*************************************************************
*文件名称：queue.h
*创建日期：2013年06月30日
*完成日期：
*作    者：sjm
**************************************************************/
#include "global.h"

#ifndef _QUEUE_H_
#define _QUEUE_H_


#define QUEUE_FULL 	0x01
#define QUEUE_OK	0x02

struct QueuePara
{
	unsigned short Length;
	unsigned short Index;
	unsigned short Cnt;
};


extern void CreateQueue(struct QueuePara *Queue,unsigned short length);
extern unsigned char GetDataFromQueue(struct QueuePara *Queue,unsigned char *buf);
extern unsigned char InsertDataToQueue(struct QueuePara *Queue,unsigned char *buf,unsigned char dat);
extern unsigned short GetQueueDataNum(struct QueuePara *Queue);
extern void ClearQueue(struct QueuePara *Queue);



#endif
