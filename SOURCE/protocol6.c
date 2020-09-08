//###########################################################################
//
// ����6ͨѶЭ���ļ�
// ������ģ��ͨѶ
// 
//
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  13.06| 30 June 2013 | S.J.M. | 
//###########################################################################
#include "protocol6.h"
#include "queue.h"

struct BACKSTAGE_PROTOCOL_TYPE6 BackstageProtocol6;
struct BACKSTAGE_PROTOCOL_TYPE6 BackstageProtocol6bak;

static unsigned char MyAddress = 0x01;				// ������ַ
static unsigned char sum = 0;
static volatile unsigned char ProtocolResolveState = 0;	// Э���������״̬��
static unsigned char channel = 0;			// ����ͨ��

unsigned char DataDirection;	// ���ݹ���ת������ 0x00=����1,0x04=����5
unsigned char SerialPortCnt;	// �������ݵ�ȡ����
/****************************************************************************
*
*��    �ƣ�unsigned char ProtocolResolve6(unsigned char receivedata)
*
*��    �ܣ�����ͨѶЭ�����
*
*��ڲ���������6����
*
*���ز�����
*
****************************************************************************/
unsigned char ProtocolResolve6(unsigned char receivedata)
{
	
	static unsigned char addr = 0;
	static unsigned char lengthL = 0;
	static unsigned short lengthcnt = 0;
	int i = 0;
	switch(ProtocolResolveState)
	{
		case 0:
			if(receivedata == 0xFE)			// �ж�����ͷ
			{
				if(Usart6RxReady == 0)
				{
					channel = 0;
					ProtocolResolveState = 1;
				}
				else if(Usart6bakRxReady == 0)
				{
					channel = 1;
					ProtocolResolveState = 1;
				}
			}
			break;
		case 1:
			if(receivedata == 0xEF)			// ȷ������ͷ,������ʼ��
			{
				sum = 0xED;					// sumҲ���к�У��
				addr = 0;
				ProtocolResolveState = 2;
			}
			else
			{
				ProtocolResolveState = 0;
				return 0;
			}
			break;
		case 2:
			if(channel == 0)
				BackstageProtocol6.Device = receivedata;			// ���յ����豸����
			else
				BackstageProtocol6bak.Device = receivedata;			// ���յ����豸����
			if(receivedata == 0x50)				// ǰ��������
			{
			 	sum += receivedata;
				ProtocolResolveState = 3;	
			}
			else if(receivedata == 0x60)			// ģ��������
			{
				sum += receivedata;
				ProtocolResolveState = 3;
			}
			else							// �豸�����뱾������
			{
				ProtocolResolveState = 0;
				return 0;
			}
			break;
		case 3:
			addr = receivedata;				// ���յ������ݵ�ַ
			MyAddress = SystemSet.para.address;
			sum += receivedata;
			ProtocolResolveState = 4;
			break;
		case 4:							                                                                                                                          	// ����
			if(channel == 0)
				BackstageProtocol6.command = receivedata;
			else
				BackstageProtocol6bak.command = receivedata;
			sum += receivedata;
			if(receivedata != 0xFE)
			{
			 	ProtocolResolveState = 5;	
			}
			else							// ��������ָ��
			{
				ProtocolResolveState = 5;	
			}
			break;
		case 5:
			lengthL = receivedata;			// ���ݳ��ȵ��ֽ�
			sum += receivedata;
			ProtocolResolveState = 6;
			break;
		case 6:								// �������ݳ���
			if(channel == 0)
			{
				BackstageProtocol6.length = ((unsigned short)receivedata<<8)+(unsigned short)lengthL;
				if(BackstageProtocol6.length > 255)	// ���ݳ��ȳ�����Ƴ��ȣ�����
				{
					 ProtocolResolveState = 0;
				}
				else if(BackstageProtocol6.length < 3)	// û��������
				{
					ProtocolResolveState = 8;
				}
				else
					ProtocolResolveState = 7;
			}
			else
			{
				BackstageProtocol6bak.length = ((unsigned short)receivedata<<8)+(unsigned short)lengthL;
				if(BackstageProtocol6bak.length > 255)	// ���ݳ��ȳ�����Ƴ��ȣ�����
				{
					 ProtocolResolveState = 0;
				}
				else if(BackstageProtocol6bak.length < 3)	// û��������
				{
					ProtocolResolveState = 8;
				}
				else
					ProtocolResolveState = 7;
			}
				

			sum += receivedata;
			lengthcnt = 0;
			
			break;
		case 7:								// ��������
			if(channel == 0)
			{
				BackstageProtocol6.DataBuffer[lengthcnt] = receivedata;
			}
			else
			{
				BackstageProtocol6bak.DataBuffer[lengthcnt] = receivedata;
			}
			
			sum += receivedata;
			ProtocolResolveState=7;
			lengthcnt++;
			if(lengthcnt > 255)			// ���ݼ�飬��ֹ���
			{
				ProtocolResolveState = 0;
			}
			if(channel == 0)
			{
				if((lengthcnt+2) == BackstageProtocol6.length)		// ���ݽ��ս���
				{
					ProtocolResolveState = 8;
				}
			}
			else
			{
				if((lengthcnt+2) == BackstageProtocol6bak.length)		// ���ݽ��ս���
				{
					ProtocolResolveState = 8;
				}
			}
			break;
		case 8:
			if((sum&0x00ff) == receivedata)	// ��У��
			{
				ProtocolResolveState = 9;
			}
			else							// ��У��ʧ��
			{
 				ProtocolResolveState = 0;
				return 0;
			}
			break;
		case 9:
			if(receivedata == 0x16)			// ֡β
			{
				ProtocolResolveState = 0;
				if(((addr == MyAddress && BackstageProtocol6.Device == 0x50) || (addr == 0xFF && BackstageProtocol6.Device == 0x50)) && channel == 0)// ����������߹㲥������ش���
				{
					Usart6RxReady = 1;
					isr_evt_set (0x0020, t_Task4);
				}
				else if(((addr == MyAddress && BackstageProtocol6bak.Device == 0x50) || (addr == 0xFF && BackstageProtocol6bak.Device == 0x50)) && channel == 1)
				{
					Usart6bakRxReady = 1;
					isr_evt_set (0x0020, t_Task4);
				}
				else				 // ����ǰ�������ת��������4������������ǰ����
				{
					if(channel == 0)
					{
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0xFE);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0xEF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.Device);  	// �豸����
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],addr);		// �豸��ַ
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.command);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.length&0x00FF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.length>>8);
						for(i=0;i<(BackstageProtocol6.length-2);i++)
						{
							InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.DataBuffer[i]);	// 
						}
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],sum&0x00FF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0x16);
						DataDirection = 5; // ��������ת��������6
						SerialPortCnt++;	// ת����������					
					}
					else
					{
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0xFE);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0xEF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.Device);  	// �豸����
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],addr);		// �豸��ַ
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.command);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.length&0x00FF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.length>>8);
						for(i=0;i<(BackstageProtocol6bak.length-2);i++)
						{
							InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.DataBuffer[i]);	// 
						}
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],sum&0x00FF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0x16);
						DataDirection = 5; // ��������ת��������6
						SerialPortCnt++;	// ת����������		
					}
				}
				
				
			}
			else
			{
				ProtocolResolveState = 0;	
			}
			break;
		default:
			ProtocolResolveState=0;
			break;

	}
	return 0x0D;
}

