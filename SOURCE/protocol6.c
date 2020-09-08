//###########################################################################
//
// 串口6通讯协议文件
// 与网络模块通讯
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

static unsigned char MyAddress = 0x01;				// 本机地址
static unsigned char sum = 0;
static volatile unsigned char ProtocolResolveState = 0;	// 协议解析函数状态机
static unsigned char channel = 0;			// 数据通道

unsigned char DataDirection;	// 数据共享转发方向 0x00=串口1,0x04=串口5
unsigned char SerialPortCnt;	// 串口数据调取次数
/****************************************************************************
*
*名    称：unsigned char ProtocolResolve6(unsigned char receivedata)
*
*功    能：串行通讯协议解析
*
*入口参数：串口6数据
*
*返回参数：
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
			if(receivedata == 0xFE)			// 判断数据头
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
			if(receivedata == 0xEF)			// 确认数据头,参数初始化
			{
				sum = 0xED;					// sum也进行和校验
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
				BackstageProtocol6.Device = receivedata;			// 接收到的设备类型
			else
				BackstageProtocol6bak.Device = receivedata;			// 接收到的设备类型
			if(receivedata == 0x50)				// 前置器数据
			{
			 	sum += receivedata;
				ProtocolResolveState = 3;	
			}
			else if(receivedata == 0x60)			// 模拟屏数据
			{
				sum += receivedata;
				ProtocolResolveState = 3;
			}
			else							// 设备类型与本机不符
			{
				ProtocolResolveState = 0;
				return 0;
			}
			break;
		case 3:
			addr = receivedata;				// 接收到的数据地址
			MyAddress = SystemSet.para.address;
			sum += receivedata;
			ProtocolResolveState = 4;
			break;
		case 4:							                                                                                                                          	// 命令
			if(channel == 0)
				BackstageProtocol6.command = receivedata;
			else
				BackstageProtocol6bak.command = receivedata;
			sum += receivedata;
			if(receivedata != 0xFE)
			{
			 	ProtocolResolveState = 5;	
			}
			else							// 程序升级指令
			{
				ProtocolResolveState = 5;	
			}
			break;
		case 5:
			lengthL = receivedata;			// 数据长度低字节
			sum += receivedata;
			ProtocolResolveState = 6;
			break;
		case 6:								// 计算数据长度
			if(channel == 0)
			{
				BackstageProtocol6.length = ((unsigned short)receivedata<<8)+(unsigned short)lengthL;
				if(BackstageProtocol6.length > 255)	// 数据长度超出设计长度，出错
				{
					 ProtocolResolveState = 0;
				}
				else if(BackstageProtocol6.length < 3)	// 没有数据区
				{
					ProtocolResolveState = 8;
				}
				else
					ProtocolResolveState = 7;
			}
			else
			{
				BackstageProtocol6bak.length = ((unsigned short)receivedata<<8)+(unsigned short)lengthL;
				if(BackstageProtocol6bak.length > 255)	// 数据长度超出设计长度，出错
				{
					 ProtocolResolveState = 0;
				}
				else if(BackstageProtocol6bak.length < 3)	// 没有数据区
				{
					ProtocolResolveState = 8;
				}
				else
					ProtocolResolveState = 7;
			}
				

			sum += receivedata;
			lengthcnt = 0;
			
			break;
		case 7:								// 接收数据
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
			if(lengthcnt > 255)			// 数据检查，防止溢出
			{
				ProtocolResolveState = 0;
			}
			if(channel == 0)
			{
				if((lengthcnt+2) == BackstageProtocol6.length)		// 数据接收结束
				{
					ProtocolResolveState = 8;
				}
			}
			else
			{
				if((lengthcnt+2) == BackstageProtocol6bak.length)		// 数据接收结束
				{
					ProtocolResolveState = 8;
				}
			}
			break;
		case 8:
			if((sum&0x00ff) == receivedata)	// 和校验
			{
				ProtocolResolveState = 9;
			}
			else							// 和校验失败
			{
 				ProtocolResolveState = 0;
				return 0;
			}
			break;
		case 9:
			if(receivedata == 0x16)			// 帧尾
			{
				ProtocolResolveState = 0;
				if(((addr == MyAddress && BackstageProtocol6.Device == 0x50) || (addr == 0xFF && BackstageProtocol6.Device == 0x50)) && channel == 0)// 本机命令或者广播命令，返回处理
				{
					Usart6RxReady = 1;
					isr_evt_set (0x0020, t_Task4);
				}
				else if(((addr == MyAddress && BackstageProtocol6bak.Device == 0x50) || (addr == 0xFF && BackstageProtocol6bak.Device == 0x50)) && channel == 1)
				{
					Usart6bakRxReady = 1;
					isr_evt_set (0x0020, t_Task4);
				}
				else				 // 其他前置器命令，转发到串口4，发送至其他前置器
				{
					if(channel == 0)
					{
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0xFE);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0xEF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.Device);  	// 设备类型
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],addr);		// 设备地址
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.command);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.length&0x00FF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.length>>8);
						for(i=0;i<(BackstageProtocol6.length-2);i++)
						{
							InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6.DataBuffer[i]);	// 
						}
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],sum&0x00FF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0x16);
						DataDirection = 5; // 返回数据转发到串口6
						SerialPortCnt++;	// 转发次数递增					
					}
					else
					{
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0xFE);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0xEF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.Device);  	// 设备类型
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],addr);		// 设备地址
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.command);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.length&0x00FF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.length>>8);
						for(i=0;i<(BackstageProtocol6bak.length-2);i++)
						{
							InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],BackstageProtocol6bak.DataBuffer[i]);	// 
						}
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],sum&0x00FF);
						InsertDataToQueue(QueueTXChannel[3],BufferTXChannel[3],0x16);
						DataDirection = 5; // 返回数据转发到串口6
						SerialPortCnt++;	// 转发次数递增		
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

