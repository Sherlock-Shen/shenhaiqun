//###########################################################################
//
// ����3ͨѶЭ���ļ�
// ��̫��ģ��ͨѶ��
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

/* �ṹ�嶨�� ----------------------------------------------------------------*/
struct BACKSTAGE_PROTOCOL_TYPE3 BackstageProtocol3;
struct BACKSTAGE_PROTOCOL_TYPE3 BackstageProtocol3bak;
static volatile u8 ProtocolResolveState = 0;	// Э���������״̬��
static u8 channel = 0;			// ����ͨ��
//union Control_Type control;	
u8  remain,datdone,RxCounter=0;

void IEC104_Staid(u8 linkcontrol,u8 USARTxChannel)//�̶�֡	��ַ��1���ֽ�
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
*��    �ƣ�u8 IEC_104_Resolve(u8 receivedata)
*
*��    �ܣ���̫��ͨѶЭ�����
*
*��ڲ���������3����
*
*���ز�����
*
****************************************************************************/
void IEC_104_Resolve(u8 receivedata)
{
	switch(ProtocolResolveState)
	{
		case 0:  //���յ��µ�����֡
			if(receivedata == 0x68)			// �ж�����ͷ����վ����վ���͵������ַ�
			{
				if(Usart3RxReady == 0)		//δ�������
				{
					channel = 0;
					BackstageProtocol3.DataBuffer[0] = receivedata;//�����bak��������һ������ʵʱ������һ�����Ǳ��ݣ�channel=0��Ӧʵʱ����channel=1��Ӧ����
					RxCounter=1;
					remain=5;								//��ʣ�ĸ��ַ�û�н���
					ProtocolResolveState = 1;//���յ�һ���ַ�
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
		case 1:													      //֮ǰ�Ѿ����յ�����0x68
      if(channel == 0)                    //����BackstageProtocol1�е�����
			{ 
				remain--;			                    //ʣ��������1
				BackstageProtocol3.DataBuffer[RxCounter++] = receivedata;
				if(remain==0)								      //����Ѿ����յ��������ַ���
			  {
					if(BackstageProtocol3.DataBuffer[1]==4)//�����ַ�
					{
						if(((BackstageProtocol3.DataBuffer[2]&0x3)==1)&&((BackstageProtocol3.DataBuffer[4]&1)==0))//S��
						{
							datdone=1;									//��ʾ����ΪS��
							Usart3RxReady=1;						//�������
							ProtocolResolveState = 0;		//������ϣ����Խ�����һ������
						}
						else if(((BackstageProtocol3.DataBuffer[2]&0x3)==3)&&((BackstageProtocol3.DataBuffer[4]&1)==0))//U��
						{
							datdone=2;									//��ʾ����ΪU��
							Usart3RxReady=1;						//�������
							ProtocolResolveState = 0;		//������ϣ����Խ�����һ������
						}
						else													//�����ԣ�׼�����½���
							ProtocolResolveState = 0;
					}
					else if((BackstageProtocol3.DataBuffer[1]>4)&&(BackstageProtocol3.DataBuffer[1]<=253)&&((BackstageProtocol3.DataBuffer[2]&1)==0)&&((BackstageProtocol3.DataBuffer[4]&1)==0))//I��
					{
						remain=BackstageProtocol3.DataBuffer[1]-4;
//						datdone=3;									//��ʾ����ΪI��	
						ProtocolResolveState = 2;			//��δ������ϣ���������
					}
					else													  //�����ԣ�׼�����½���
						ProtocolResolveState = 0;
			  }
			}			
			else
			{
				remain--;			                    //ʣ��������1
				BackstageProtocol3bak.DataBuffer[RxCounter++] = receivedata;
				if(remain==0)								      //����Ѿ����յ��������ַ���
			  {
					if(BackstageProtocol3bak.DataBuffer[1]<=4)
					{
						if(((BackstageProtocol3bak.DataBuffer[2]&0x3)==1)&&((BackstageProtocol3bak.DataBuffer[4]&1)==0))//S��
						{
							datdone=1;
							Usart3bakRxReady=1;					//�������
							ProtocolResolveState = 0;		//������ϣ����Խ�����һ������
						}
						else if(((BackstageProtocol3bak.DataBuffer[2]&0x3)==3)&&((BackstageProtocol3bak.DataBuffer[4]&1)==0))//U��
						{
							datdone=2;
							Usart3bakRxReady=1;					//�������
							ProtocolResolveState = 0;		//������ϣ����Խ�����һ������
						}
						else 
							ProtocolResolveState = 0;
					}
					else if((BackstageProtocol3bak.DataBuffer[1]>4)&&((BackstageProtocol3bak.DataBuffer[2]&1)==0)&&((BackstageProtocol3bak.DataBuffer[4]&1)==0))//I��
					{
						remain=BackstageProtocol3bak.DataBuffer[1]-4;
//						datdone=3;	
						ProtocolResolveState = 2;			//��δ������ϣ���������			
					}
					else													  //�����ԣ�׼�����½���
						ProtocolResolveState = 0;
			  }
			}
			break;
		case 2://I�����ݼ�������
		  if(channel == 0) 
			{
				remain--;
				BackstageProtocol3.DataBuffer[RxCounter++] = receivedata;
			  if(remain==0)
			  {
					Usart3RxReady=1;						   //�������
					datdone=3;										 //��ʾ����ΪI��	
					isr_evt_set (0x0004, t_Task4); //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
					ProtocolResolveState=0;
				}																						
			}																										
			else
			{
				remain--;
				BackstageProtocol3bak.DataBuffer[RxCounter++] = receivedata;
			  if(remain==0)
			  {
					Usart3bakRxReady=1;						//�������
					datdone=3;										//��ʾ����ΪI��	
					isr_evt_set (0x0004, t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
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
//1 �����ݰ����¼��͸����ȼ�����
//2 �����ݰ���ѭ�����ͻ�����ȼ�����
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  2019.04.09| 9 April 2019 | S.H.Q. |
//###########################################################################

void IEC_104_CommandProcess(u8 USARTxChannel)
{
//	static u8 Read_Flag = 0;
//	u16 sjaddr;//���ݵ�ַ
//	u8 i;
	if(datdone == 2)//U��
	{
		if(BackstageProtocol3.DataBuffer[2]==0x7)	  			//��������  
		{
			IEC104_Staid(0xB,USARTxChannel);  							//����ȷ��           	
		}	
		else if(BackstageProtocol3.DataBuffer[2]==0x13)		//ֹͣ����
		{
			IEC104_Staid(0x23,USARTxChannel);               //ֹͣȷ��		
		}
		else if(BackstageProtocol3.DataBuffer[2]==0x43) 	//��������
		{			
			IEC104_Staid(0x83,USARTxChannel);								//����ȷ��
		}
	}
	else if(datdone == 1)//S��
	{
	
	}
//	else if(datdone == 3)
//	{
//		if((BackstageProtocol3.DataBuffer[5]==(SystemSet.para.address&0xFF))||BackstageProtocol3.DataBuffer[5]==0xFF)
//		{
//			if(BackstageProtocol3.DataBuffer[6]==100) //���ٻ�����
//			{
//				 IEC101_Staid_All(USARTxChannel);
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==101)   //�ܵ��ٻ�/������ܼ�����
//			{
//				if(BackstageProtocol3.DataBuffer[12]==1)       //�ٻ��������������
//				{
//				  IEC101_Staid_Enegy(1,USARTxChannel);
//          os_dly_wait (10);					
//				}
//				else if(BackstageProtocol3.DataBuffer[12]==41) //����������������
//				{
//				  IEC101_Staid_Enegy(41,USARTxChannel);
//					os_dly_wait (10);	
//				}
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==102) //���ͱ�ʶ��102��:=����������
//			{
//				sjaddr=(u16)(BackstageProtocol3.DataBuffer[11]<<8)+BackstageProtocol3.DataBuffer[10];  //��ǰ�ߺ�
//				switch(sjaddr)
//				{
//					case 0x4801: 
//					{
//						Read_Flag |= system_data;
//						IEC101_Staid(0xA0,USARTxChannel);           //0 1 1ACD(Ҫ�����һ������) 0DFC(����������λ) 0000(������0Ϊȷ��)  
//					}
//						break;
//					case 0x4802: 
//					{
//						Read_Flag |= protect_data;
//						IEC101_Staid(0xA0,USARTxChannel);           //0 1 1ACD(Ҫ�����һ������) 0DFC(����������λ) 0000(������0Ϊȷ��) 
//					}
//						break;
//					case 0x4803: 
//					{
//						Read_Flag |= alarm_data;
//						IEC101_Staid(0xA0,USARTxChannel);           //0 1 1ACD(Ҫ�����һ������) 0DFC(����������λ) 0000(������0Ϊȷ��)  
//					}
//					  break;
//					case 0x4804: 
//					{
//						Read_Flag |= autoswitch_data;
//						IEC101_Staid(0xA0,USARTxChannel);           //0 1 1ACD(Ҫ�����һ������) 0DFC(����������λ) 0000(������0Ϊȷ��)  
//					}
//						break;
//					default:
//						break;
//				}
//			}
//			else  if(BackstageProtocol3.DataBuffer[6]==103) //ʱ��ͬ��
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
//			else if(BackstageProtocol3.DataBuffer[6]==107)	//�ٻ���ʷ��¼(˳���¼���¼)
//			{
//        History_Data(USARTxChannel);
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==46) //ң������
//			{
//				control.byte = BackstageProtocol3.DataBuffer[12];
//				if((control.bit.S_E==1) && (BackstageProtocol3.DataBuffer[8]==6))     //<6>:=����
//				{
//					Remote_Control(USARTxChannel,7);                                    //<7>:=����ȷ��
//					if(control.bit.QCS_RCS == 2)
//						Select101Addr=1;
//					else if(control.bit.QCS_RCS == 1)
//						Select101Addr=2;
//				}
//				else if((control.bit.S_E==0) && (BackstageProtocol3.DataBuffer[8]==6))//<6>:=����
//				{
//					Remote_Control(USARTxChannel,7);                                    //<7>:=����ȷ��
//					if((control.bit.QCS_RCS == 2) && (Select101Addr==1))
//					{
//						BreakOn101Flag=2;
//						os_dly_wait (100);
//						Remote_Control(USARTxChannel,10);                                 //<10>:=�������
//					}
//					else if((control.bit.QCS_RCS == 1) && (Select101Addr==2))
//					{
//						BreakOff101Flag=2;
//						os_dly_wait (100);
//						Remote_Control(USARTxChannel,10);                                 //<10>:=�������
//					}
//				}
//				else if((control.bit.S_E==1) && (BackstageProtocol3.DataBuffer[8]==8))//<8>:=ֹͣ����
//				{
//					Remote_Control(USARTxChannel,9);                                    //<9>:=ֹͣ����ȷ��
//					Select101Addr=0;
//				}
//			}
//			else if(BackstageProtocol3.DataBuffer[6]==48)//���ò�������
//			{
//				sjaddr=(u16)(BackstageProtocol3.DataBuffer[11]<<8)+BackstageProtocol3.DataBuffer[10];  //��ǰ�ߺ���Ϣ���ַ
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
//			else if(BackstageProtocol3.DataBuffer[6]==108) //������Ȩ������
//		  {
//				if(CodeTimeout == 0)
//				{
//				  for(i=0;i<16;i++)
//            AUTHORIZATIONcode[i]=BackstageProtocol3.DataBuffer[12+i];
//					CodeTimeout = BackstageProtocol3.DataBuffer[12+14] | (BackstageProtocol3.DataBuffer[12+15]<<8); //��Ȩ����
//				}					
//		  }			
//		}
//  }
}




