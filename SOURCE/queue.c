/*************************************************************
*�ļ����ƣ�queue.c
*�������ڣ�2013��06��30��
*������ڣ�2013��06��30��
*�޸����ڣ�
*��    �ߣ�sjm
**************************************************************/
#include "stm32f4xx.h"
#include "queue.h"
//#include "sysinit.h"

/*************************************************************
*�������ƣ�CreateQueue
*�������ܣ�����һ�����ݶ���
*���������buf,length
*�����������
*��    �ߣ�sjm
**************************************************************/
void CreateQueue(struct QueuePara *Queue,unsigned short length)
{
	Queue->Length = length;		//�������������
	Queue->Index = 0;				//������ʼͷָ��
	Queue->Cnt = 0;				//���е�ǰ���ݸ���
}

/*************************************************************
*�������ƣ�GetDataFromQueue
*�������ܣ��Ӷ����л��һ������
*���������buf
*�������������
*��    �ߣ�sjm
**************************************************************/
unsigned char GetDataFromQueue(struct QueuePara *Queue,unsigned char *buf)
{
	unsigned char i;
//	__set_PRIMASK(1);
	i = buf[Queue->Index];			//ȡ������
	if(Queue->Cnt > 0)
	  Queue->Cnt--;					//���ݸ�����1
	Queue->Index++;					//����ͷ��1
	if(Queue->Index >= Queue->Length)
	{
		Queue->Index = 0;				//��������ͷָ��
	}
//	__set_PRIMASK(0);
	return(i);					//����ȡ�õ�����
}

/*************************************************************
*�������ƣ�InsertDataToQueue1
*�������ܣ�����в���һ������
*���������buf��dat
*�������������״̬
*��    �ߣ�sjm
**************************************************************/
unsigned char InsertDataToQueue1(struct QueuePara *Queue,unsigned char *buf,unsigned char dat)
{
	unsigned short i;
	
	if(Queue->Cnt >= Queue->Length)		//������
	{
		return QUEUE_FULL;			//���ض�����������
	}
	else
	{	
//		__set_PRIMASK(1);
		if((Queue->Index + Queue->Cnt) < Queue->Length)	//���Ҷ�βָ��
		{
			i = Queue->Index + Queue->Cnt;
		}
		else
		{
			i = Queue->Index + Queue->Cnt - Queue->Length;
		}
		buf[i] = dat;						//�������ݵ���β
		Queue->Cnt++;							//���ݸ�����1
//		__set_PRIMASK(0);
		return QUEUE_OK;					//���ز����ɹ���־
	}
} 

/*************************************************************
*�������ƣ�InsertDataToQueue
*�������ܣ�����в���һ������
*���������buf��dat
*�������������״̬
*��    �ߣ�sjm
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
	
	if(Queue->Cnt >= Queue->Length)		//������
	{
		return QUEUE_FULL;			//���ض�����������
	}
	else
	{	
//		__set_PRIMASK(1);
		if((Queue->Index + Queue->Cnt) < Queue->Length)	//���Ҷ�βָ��
		{
			i = Queue->Index + Queue->Cnt;
		}
		else
		{
			i = Queue->Index + Queue->Cnt - Queue->Length;
		}
		buf[i] = dat;						//�������ݵ���β
		Queue->Cnt++;							//���ݸ�����1
//		__set_PRIMASK(0);
		return QUEUE_OK;					//���ز����ɹ���־
	}
} 

/*************************************************************
*�������ƣ�GetQueueDataNum
*�������ܣ���ö�����Ԫ�ظ���
*���������buf
*���������Ԫ�ظ���
*��    �ߣ�sjm
**************************************************************/
unsigned short GetQueueDataNum(struct QueuePara *Queue)
{
	return(Queue->Cnt);			//���ض��������ݸ���
}
/*************************************************************
*�������ƣ�ClearQueue
*�������ܣ���ն���
*���������buf
*�����������
*��    �ߣ�sjm
**************************************************************/
void ClearQueue(struct QueuePara *Queue)
{
	Queue->Index = 0;				//����ͷָ�븴λ
	Queue->Cnt = 0;				//�������ݸ�������
}

