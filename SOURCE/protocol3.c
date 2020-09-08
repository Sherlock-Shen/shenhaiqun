//###########################################################################
//
// 串口3通讯协议文件
// 以太网模块通讯口
// 
//
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  09.10| 19 may 2018 | S.H.Q. |
//###################################+########################################
#include "protocol3.h"
#include "queue.h"
#include "flash.h"
#include "ds1302.h"

/* 结构体定义 ----------------------------------------------------------------*/
struct BACKSTAGE_PROTOCOL_TYPE3 BackstageProtocol3;
struct BACKSTAGE_PROTOCOL_TYPE3 BackstageProtocol3bak;
static volatile u8 ProtocolResolveState = 0;	// 协议解析函数状态机
static u8 channel = 0;			// 数据通道
//union Control_Type control;	
u8  remain,datdone,RxCounter=0;

void IEC104_Staid(u8 linkcontrol,u8 USARTxChannel)//固定帧	地址域1个字节
{
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x04);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],linkcontrol);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
}
/****************************************************************************
*
*名    称：u8 IEC_104_Resolve(u8 receivedata)
*
*功    能：以太网通讯协议解析
*
*入口参数：串口3数据
*
*返回参数：
*
****************************************************************************/
void IEC_104_Resolve(u8 receivedata)
{
	switch(ProtocolResolveState)
	{
		case 0:  //接收到新的数据帧
			if(receivedata == 0x68)			// 判断数据头，主站向子站发送的启动字符
			{
				if(Usart3RxReady == 0)		//未接收完成
				{
					channel = 0;
					BackstageProtocol3.DataBuffer[0] = receivedata;//这个和bak两个像是一个用来实时处理，另一个像是备份，channel=0对应实时处理，channel=1对应备份
					RxCounter=1;
					remain=5;								//还剩四个字符没有接收
					ProtocolResolveState = 1;//接收到一个字符
				}
				else if(Usart3bakRxReady == 0)
				{
					channel = 1;
					BackstageProtocol3bak.DataBuffer[0] = receivedata;
					RxCounter=1;
					remain=5;
					ProtocolResolveState = 1;
				}
			}
			else 
				ProtocolResolveState = 0;
			break;
		case 1:													      //之前已经接收到数据0x68
      if(channel == 0)                    //存在BackstageProtocol1中的数据
			{ 
				remain--;			                    //剩余数量减1
				BackstageProtocol3.DataBuffer[RxCounter++] = receivedata;
				if(remain==0)								      //如果已经接收到第六个字符了
			  {
					if(BackstageProtocol3.DataBuffer[1]==4)//长度字符
					{
						if(((BackstageProtocol3.DataBuffer[2]&0x3)==1)&&((BackstageProtocol3.DataBuffer[4]&1)==0))//S型
						{
							datdone=1;									//表示数据为S型
							Usart3RxReady=1;						//接收完毕
							ProtocolResolveState = 0;		//接收完毕，可以接收下一个命令
						}
						else if(((BackstageProtocol3.DataBuffer[2]&0x3)==3)&&((BackstageProtocol3.DataBuffer[4]&1)==0))//U型
						{
							datdone=2;									//表示数据为U型
							Usart3RxReady=1;						//接收完毕
							ProtocolResolveState = 0;		//接收完毕，可以接收下一个命令
						}
						else													//都不对，准备重新接收
							ProtocolResolveState = 0;
					}
					else if((BackstageProtocol3.DataBuffer[1]>4)&&(BackstageProtocol3.DataBuffer[1]<=253)&&((BackstageProtocol3.DataBuffer[2]&1)==0)&&((BackstageProtocol3.DataBuffer[4]&1)==0))//I型
					{
						remain=BackstageProtocol3.DataBuffer[1]-4;
//						datdone=3;									//表示数据为I型	
						ProtocolResolveState = 2;			//尚未接收完毕，继续接收
					}
					else													  //都不对，准备重新接收
						ProtocolResolveState = 0;
			  }
			}			
			else
			{
				remain--;			                    //剩余数量减1
				BackstageProtocol3bak.DataBuffer[RxCounter++] = receivedata;
				if(remain==0)								      //如果已经接收到第六个字符了
			  {
					if(BackstageProtocol3bak.DataBuffer[1]<=4)
					{
						if(((BackstageProtocol3bak.DataBuffer[2]&0x3)==1)&&((BackstageProtocol3bak.DataBuffer[4]&1)==0))//S型
						{
							datdone=1;
							Usart3bakRxReady=1;					//接收完毕
							ProtocolResolveState = 0;		//接收完毕，可以接收下一个命令
						}
						else if(((BackstageProtocol3bak.DataBuffer[2]&0x3)==3)&&((BackstageProtocol3bak.DataBuffer[4]&1)==0))//U型
						{
							datdone=2;
							Usart3bakRxReady=1;					//接收完毕
							ProtocolResolveState = 0;		//接收完毕，可以接收下一个命令
						}
						else 
							ProtocolResolveState = 0;
					}
					else if((BackstageProtocol3bak.DataBuffer[1]>4)&&((BackstageProtocol3bak.DataBuffer[2]&1)==0)&&((BackstageProtocol3bak.DataBuffer[4]&1)==0))//I型
					{
						remain=BackstageProtocol3bak.DataBuffer[1]-4;
//						datdone=3;	
						ProtocolResolveState = 2;			//尚未接收完毕，继续接收			
					}
					else													  //都不对，准备重新接收
						ProtocolResolveState = 0;
			  }
			}
			break;
		case 2://I型数据继续接收
		  if(channel == 0) 
			{
				remain--;
				BackstageProtocol3.DataBuffer[RxCounter++] = receivedata;
			  if(remain==0)
			  {
					Usart3RxReady=1;						   //接收完毕
					datdone=3;										 //表示数据为I型	
					isr_evt_set (0x0004, t_Task4); //这个任务4专门用来处理这些协议，除了这个之外还有其他的一些协议
					ProtocolResolveState=0;
				}																						
			}																										
			else
			{
				remain--;
				BackstageProtocol3bak.DataBuffer[RxCounter++] = receivedata;
			  if(remain==0)
			  {
					Usart3bakRxReady=1;						//接收完毕
					datdone=3;										//表示数据为I型	
					isr_evt_set (0x0004, t_Task4);   							 //这个任务4专门用来处理这些协议，除了这个之外还有其他的一些协议
				  ProtocolResolveState=0;
			  }	  
			}	
			break;
		default:
			ProtocolResolveState=0;
			break;
	}
}

void IEC_104_CommandProcessPrepare(void)
{
	u16 i = 0;	
	for(i=0;i<RxCounter;i++)
		BackstageProtocol3.DataBuffer[i] = BackstageProtocol3bak.DataBuffer[i];	
}

//###########################################################################
//
//1 级数据包括事件和高优先级报文
//2 级数据包括循环传送或低优先级报文
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  2019.04.09| 9 April 2019 | S.H.Q. |
//###########################################################################

void IEC_104_CommandProcess(u8 USARTxChannel)
{
//	static u8 Read_Flag = 0;
//	u16 sjaddr;//数据地址
//	u8 i;
	if(datdone == 2)//U型
	{
		if(BackstageProtocol3.DataBuffer[2]==0x7)	  			//启动命令  
		{
			IEC104_Staid(0xB,USARTxChannel);  							//启动确认           	
		}	
		else if(BackstageProtocol3.DataBuffer[2]==0x13)		//停止命令
		{
			IEC104_Staid(0x23,USARTxChannel);               //停止确认		
		}
		else if(BackstageProtocol3.DataBuffer[2]==0x43) 	//测试命令
		{			
			IEC104_Staid(0x83,USARTxChannel);								//测试确认
		}
	}
	else if(datdone == 1)//S型
	{
	
	}
//	else if(datdone == 3)
//	{
//		if((BackstageProtocol3.DataBuffer[5]==(SystemSet.para.address&0xFF))||BackstageProtocol3.DataBuffer[5]==0xFF)
//		{
//			if(BackstageProtocol3.DataBuffer[6]==100) //总召唤命令
//			{
//				 IEC101_Staid_All(USARTxChannel);
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==101)   //总的召唤/冻结电能计数量
//			{
//				if(BackstageProtocol3.DataBuffer[12]==1)       //召唤电能脉冲计数量
//				{
//				  IEC101_Staid_Enegy(1,USARTxChannel);
//          os_dly_wait (10);					
//				}
//				else if(BackstageProtocol3.DataBuffer[12]==41) //冻结电能脉冲计数量
//				{
//				  IEC101_Staid_Enegy(41,USARTxChannel);
//					os_dly_wait (10);	
//				}
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==102) //类型标识＜102＞:=读数据命令
//			{
//				sjaddr=(u16)(BackstageProtocol3.DataBuffer[11]<<8)+BackstageProtocol3.DataBuffer[10];  //低前高后
//				switch(sjaddr)
//				{
//					case 0x4801: 
//					{
//						Read_Flag |= system_data;
//						IEC101_Staid(0xA0,USARTxChannel);           //0 1 1ACD(要求访问一级数据) 0DFC(数据流控制位) 0000(功能码0为确认)  
//					}
//						break;
//					case 0x4802: 
//					{
//						Read_Flag |= protect_data;
//						IEC101_Staid(0xA0,USARTxChannel);           //0 1 1ACD(要求访问一级数据) 0DFC(数据流控制位) 0000(功能码0为确认) 
//					}
//						break;
//					case 0x4803: 
//					{
//						Read_Flag |= alarm_data;
//						IEC101_Staid(0xA0,USARTxChannel);           //0 1 1ACD(要求访问一级数据) 0DFC(数据流控制位) 0000(功能码0为确认)  
//					}
//					  break;
//					case 0x4804: 
//					{
//						Read_Flag |= autoswitch_data;
//						IEC101_Staid(0xA0,USARTxChannel);           //0 1 1ACD(要求访问一级数据) 0DFC(数据流控制位) 0000(功能码0为确认)  
//					}
//						break;
//					default:
//						break;
//				}
//			}
//			else  if(BackstageProtocol3.DataBuffer[6]==103) //时间同步
//			{
//				 TimeNow.second  = ((u16)(BackstageProtocol3.DataBuffer[12] | (BackstageProtocol3.DataBuffer[13]<<8))/1000)&0x3F;  
//				 TimeNow.minute  = BackstageProtocol3.DataBuffer[14]&0x3F;
//				 TimeNow.hour    = BackstageProtocol3.DataBuffer[15]&0x1F;
//				 TimeNow.date    = BackstageProtocol3.DataBuffer[16]&0x1F;
//				 TimeNow.month   = BackstageProtocol3.DataBuffer[17]&0x0F;
//				 TimeNow.year    = BackstageProtocol3.DataBuffer[18]&0x7F;
//				 correct_time();
//				 Time_Synchronization(USARTxChannel);
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==107)	//召唤历史记录(顺序事件记录)
//			{
//        History_Data(USARTxChannel);
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==46) //遥控命令
//			{
//				control.byte = BackstageProtocol3.DataBuffer[12];
//				if((control.bit.S_E==1) && (BackstageProtocol3.DataBuffer[8]==6))     //<6>:=激活
//				{
//					Remote_Control(USARTxChannel,7);                                    //<7>:=激活确认
//					if(control.bit.QCS_RCS == 2)
//						Select101Addr=1;
//					else if(control.bit.QCS_RCS == 1)
//						Select101Addr=2;
//				}
//				else if((control.bit.S_E==0) && (BackstageProtocol3.DataBuffer[8]==6))//<6>:=激活
//				{
//					Remote_Control(USARTxChannel,7);                                    //<7>:=激活确认
//					if((control.bit.QCS_RCS == 2) && (Select101Addr==1))
//					{
//						BreakOn101Flag=2;
//						os_dly_wait (100);
//						Remote_Control(USARTxChannel,10);                                 //<10>:=激活结束
//					}
//					else if((control.bit.QCS_RCS == 1) && (Select101Addr==2))
//					{
//						BreakOff101Flag=2;
//						os_dly_wait (100);
//						Remote_Control(USARTxChannel,10);                                 //<10>:=激活结束
//					}
//				}
//				else if((control.bit.S_E==1) && (BackstageProtocol3.DataBuffer[8]==8))//<8>:=停止激活
//				{
//					Remote_Control(USARTxChannel,9);                                    //<9>:=停止激活确认
//					Select101Addr=0;
//				}
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==48)//设置参数命令
//			{
//				sjaddr=(u16)(BackstageProtocol3.DataBuffer[11]<<8)+BackstageProtocol3.DataBuffer[10];  //低前高后，信息体地址
//				switch(sjaddr)
//				{
//					case 0x4801: 
//					{
//						for(i=0;i<((sizeof(struct SYSTEM_PARA))/2);i++)
//							SystemSet.word[i]=(BackstageProtocol3.DataBuffer[13+2*i]<<8)+BackstageProtocol3.DataBuffer[12+2*i];
//						SetSystemFlag=1;
//						SetParameter(USARTxChannel,((sizeof(struct SYSTEM_PARA))/2));
//					}
//						break;
//					case 0x4802:
//					{
//						for(i=0;i<((sizeof(struct PROTECT_PARA))/2);i++)
//							ProtectSet.word[i]=(BackstageProtocol3.DataBuffer[13+2*i]<<8)+BackstageProtocol3.DataBuffer[12+2*i];
//						SetProtectFlag=1;
//						SetParameter(USARTxChannel,((sizeof(struct PROTECT_PARA))/2));
//					}
//						break;
//					case 0x4803:
//					{
//						for(i=0;i<((sizeof(struct ALARM_PARA))/2);i++)
//							AlarmSet.word[i]=(BackstageProtocol3.DataBuffer[13+2*i]<<8)+BackstageProtocol3.DataBuffer[12+2*i];
//						SetAlarmFlag=1;
//						SetParameter(USARTxChannel,((sizeof(struct ALARM_PARA))/2));
//					}
//						break;	
//          case 0x4804:
//					{
//						for(i=0;i<((sizeof(struct AUTO_SWITCHOFF))/2);i++)
//							AutoswitchSet.word[i]=(BackstageProtocol3.DataBuffer[13+2*i]<<8)+BackstageProtocol3.DataBuffer[12+2*i];
//						SetAutoswitchFlag=1;
//						SetParameter(USARTxChannel,((sizeof(struct AUTO_SWITCHOFF))/2));
//					}
//						break;						
//					default:
//						break;
//				}
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==108) //发送授权码数据
//		  {
//				if(CodeTimeout == 0)
//				{
//				  for(i=0;i<16;i++)
//            AUTHORIZATIONcode[i]=BackstageProtocol3.DataBuffer[12+i];
//					CodeTimeout = BackstageProtocol3.DataBuffer[12+14] | (BackstageProtocol3.DataBuffer[12+15]<<8); //授权秒数
//				}					
//		  }			
//		}
//  }
}




