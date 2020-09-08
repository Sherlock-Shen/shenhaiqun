//###########################################################################
//
// ����4ͨѶЭ���ļ�
// ����ģ��ͨ��
//
//ע�⣺��λ��Ϊ��ƽ��ģʽ��ͨ��ǰ�轫��ַ�򣬴���ԭ����Ϊ2���ֽڣ���ϵͳ�����ĵ�ַ��Ϊ1��
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  18.03| 13 mary 2018 | S.H.Q. |
//###########################################################################
#include "protocol4.h"
#include "protocol1.h"
#include "ds1302.h"
#include "queue.h"

#define Addr101_4                 2
#define Reason101_4               2
#define TotalLen10_4              Addr101_4+3//10֡��ƫ����
#define FrontReason_4             Addr101_4-1//����ԭ��ǰ
#define TotalLen68_4              Addr101_4*2+Reason101_4-3//68��֡��ƫ�Ƹ���

struct BACKSTAGE_PROTOCOL_TYPE4 BackstageProtocol4;
struct BACKSTAGE_PROTOCOL_TYPE4 BackstageProtocol4bak;

static volatile u8 ProtocolResolveState = 0;	// Э���������״̬��
static u8 channel = 0;			// ����ͨ��
static u16 gyjsq,RxCounter1=0;
static u8 datdone1=0;

/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolve4(u8 receivedata)
*
*��    �ܣ�����ͨѶЭ�����
*
*��ڲ���������4����
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolve4(u8 receivedata)
{
	switch(ProtocolResolveState)
	{
		case 0:  //���յ��µ�����֡
			if(receivedata == 0x10)			// �ж�����ͷ����վ����վ���͵������ַ�
			{
				if(Usart4RxReady == 0)		//δ�������
				{
					channel = 0;
					BackstageProtocol4.DataBuffer[0] = receivedata;//�����bak��������һ������ʵʱ������һ�����Ǳ��ݣ�channel=0��Ӧʵʱ����channel=1��Ӧ����
					RxCounter1=1;
					gyjsq=TotalLen10_4;								//��ʣTotalLen10_4-1 ���ַ�û�н���
					ProtocolResolveState = 1;//���յ�һ���ַ�
				}
				else if(Usart4bakRxReady == 0)
				{
					channel = 1;
					BackstageProtocol4bak.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=TotalLen10_4;
					ProtocolResolveState = 1;
				}
			}
			else if (receivedata == 0x68)//���٣���վ����վ�������������ַ�
			{
				if(Usart4RxReady == 0)
				{
					channel = 0;
					BackstageProtocol4.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=3;
					ProtocolResolveState = 2;
				}
				else if(Usart4bakRxReady == 0)
				{
					channel = 1;
					BackstageProtocol4bak.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=3;
					ProtocolResolveState = 2;
				}
			}
			else 
				ProtocolResolveState = 0;
			break;
		case 1://֮ǰ�Ѿ����յ�����0x10
      if(channel == 0)//����BackstageProtocol4�е�����
			{
				gyjsq--;//ʣ��������1
				BackstageProtocol4.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)								//����Ѿ����յ�������ַ���
			  {
					if(BackstageProtocol4.DataBuffer[Addr101_4+2]==UsartAdd(BackstageProtocol4.DataBuffer,1,Addr101_4+1))//����101��Լ������ȵ�4�����ݵ��ڵ�2�͵�3������֮�ͣ����ĸ���У��λ
					{
							datdone1=2;
							Usart4RxReady=1;
							isr_evt_set(0x0008,t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
					}
					ProtocolResolveState = 0;
			  }
			}
			else
			{
				gyjsq--;
				BackstageProtocol4bak.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)								//	//�ж�
			  {
					if(BackstageProtocol4bak.DataBuffer[Addr101_4+2]==UsartAdd(BackstageProtocol4bak.DataBuffer,1,Addr101_4+1))
					{
							datdone1=2;
							Usart4bakRxReady=1;
							isr_evt_set(0x0008,t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
					}
					ProtocolResolveState = 0;
			  }
			}
			break;
		case 2://��һ�����֮ǰ�Ѿ����յ�0x68����������ֻ��4���ַ�������һ����5���ַ�
			if(channel == 0)
			{
				gyjsq--;
				BackstageProtocol4.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)
				{
					if(BackstageProtocol4.DataBuffer[1]==BackstageProtocol4.DataBuffer[2]&&BackstageProtocol4.DataBuffer[3]==0x68)
					{																											//����1�͵�4�����ݶ���0x68����2�͵�3��������ͬ��101��Լ������ȷʵ���
						gyjsq=BackstageProtocol4.DataBuffer[1]+2;           //��2�����ݱ�ʾ����֮��Ҫ���͵����ݳ���
						ProtocolResolveState = 3;						                //���뵽������ģʽ
					}
					else if(BackstageProtocol4.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol4.DataBuffer[1]+(BackstageProtocol4.DataBuffer[2]<<8)+2;
						ProtocolResolveState = 4;                          	
					}
					else
						ProtocolResolveState = 0;                           //���������Ϊ�ǽ��ճ����˴���
				}
			}
			else
			{
				gyjsq--;
				BackstageProtocol4bak.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)
				{
					if(BackstageProtocol4bak.DataBuffer[1]==BackstageProtocol4bak.DataBuffer[2]&&BackstageProtocol4bak.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol4bak.DataBuffer[1]+2;
						ProtocolResolveState = 3;						
					}
					else if(BackstageProtocol4bak.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol4bak.DataBuffer[1]+(BackstageProtocol4bak.DataBuffer[2]<<8)+2;
						ProtocolResolveState = 4;                          	
					}
					else
					ProtocolResolveState = 0;
			  }
			}
			break;
		case 3://ģʽ2����������ĵ�����ģʽ��׼��������������������
		  if(channel == 0) //������������ж��Ƿ���BackstageProtocol4�л��Ƿ���BackstageProtocol4bak��
			{
				gyjsq--;
				BackstageProtocol4.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol4.DataBuffer[RxCounter1-1]==0x16)//�ж����һ���Ƿ�Ϊ0x16�������֡β
				  {
						  if(BackstageProtocol4.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol4.DataBuffer,4,BackstageProtocol4.DataBuffer[1]))
						  {																								 //������2�����ݵ��ڵ�5������13������֮�ͣ���ʵ���ǳ��������֡β�Լ�У��λ����������֮�͵���У��λ
								Usart4RxReady=1;
							  datdone1=1;	
								isr_evt_set(0x0008,t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
						  }																								 //�ɴ�Ҳ���Կ����������������ɣ�����ȥ������ȥ������Щ���ݣ�0x10������������һ��ͨ�ż�����֣�0x68��������ͨ�ŵĿ�ʼ
				  }																										 //�����CommandProcess1��������Ӧ�þ��Ǵ�����Щ���ݵ�
				  ProtocolResolveState=0;
			  }	  
			}	
			else
			{
				gyjsq--;
				BackstageProtocol4bak.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol4bak.DataBuffer[RxCounter1-1]==0x16)
				  {
						if(BackstageProtocol4bak.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol4bak.DataBuffer,4,BackstageProtocol4bak.DataBuffer[1]))
						{
							Usart4bakRxReady=1;
							datdone1=1;	
							isr_evt_set(0x0008,t_Task4);
						}
				  }
				  ProtocolResolveState=0;
			  }	  
			}	
			break;
		case 4:
		  if(channel == 0) 
			{
				gyjsq--;
				BackstageProtocol4.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol4.DataBuffer[RxCounter1-1]==0x16)//�ж����һ���Ƿ�Ϊ0x16�������֡β
				  {
						  if(BackstageProtocol4.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol4.DataBuffer,4,BackstageProtocol4.DataBuffer[1]|(BackstageProtocol4.DataBuffer[2]<<8)))
						  {																								 //������2�����ݵ��ڵ�5������13������֮�ͣ���ʵ���ǳ��������֡β�Լ�У��λ����������֮�͵���У��λ
								Usart4RxReady=1;
							  datdone1=1;	
								isr_evt_set(0x0008,t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
						  }																								 //�ɴ�Ҳ���Կ����������������ɣ�����ȥ������ȥ������Щ���ݣ�0x10������������һ��ͨ�ż�����֣�0x68��������ͨ�ŵĿ�ʼ
				  }																										 //�����CommandProcess1��������Ӧ�þ��Ǵ�����Щ���ݵ�
				  ProtocolResolveState=0;
			  }	  
			}	
			else
			{
				gyjsq--;
				BackstageProtocol4bak.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol4bak.DataBuffer[RxCounter1-1]==0x16)
				  {
						if(BackstageProtocol4bak.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol4bak.DataBuffer,4,BackstageProtocol4bak.DataBuffer[1]|(BackstageProtocol4bak.DataBuffer[2]<<8)))
						{
							Usart4bakRxReady=1;
							datdone1=1;	
							isr_evt_set(0x0008,t_Task4);
						}
				  }
				  ProtocolResolveState=0;
			  }	  
			}	
			break;
		default:
			ProtocolResolveState=0;
			break;
	}
	return 0x0D;
}

void CommandProcess4Prepare(void)
{
	u16 i = 0;	
	for(i=0;i<RxCounter1;i++)
		BackstageProtocol4.DataBuffer[i] = BackstageProtocol4bak.DataBuffer[i];	
}

void Read_Loss_SetDeal_4(u8 USARTxChnnel)
{
	u8 i,j=0;
	LockSetTotal=(BackstageProtocol4.DataBuffer[7+FrontReason_4]&0x7F);
	if(LockSetTotal == 0)
	{
		for(i=0;i<9;i++)
		{
		  LockSetType[i]= 4;
		  LockSetAddr[i]= 0x8001+i;
		}
	}
	else
	{
		for(i=0;i<LockSetTotal;i++)
		{
			LockSetType[i]= 38;
			LockSetAddr[i]=BackstageProtocol4.DataBuffer[12+j+TotalLen68_4]+(BackstageProtocol4.DataBuffer[13+j+TotalLen68_4]<<8);//2
			j=j+2;
		}
  }
}

void Write_Loss_SetDeal_4(u8 USARTxChannel)
{
	u8 i,j=0,k;
	LockSetTotal=(BackstageProtocol4.DataBuffer[7+FrontReason_4]&0x7F);
	for(i=0;i<LockSetTotal;i++)
	{
		LockSetAddr[i]=BackstageProtocol4.DataBuffer[13+j+TotalLen68_4]+(BackstageProtocol4.DataBuffer[14+j+TotalLen68_4]<<8);//2
		LockSetType[i]=BackstageProtocol4.DataBuffer[15+j+TotalLen68_4];//1
		LockSetNum[i]=BackstageProtocol4.DataBuffer[16+j+TotalLen68_4];//1
		for(k=0;k<LockSetNum[i];k++)
		{
			LockSetValue[i][k]=BackstageProtocol4.DataBuffer[17+TotalLen68_4+k+j];
		}
		j=j+LockSetNum[i]+4;
	}
}

void Write_Loss_SetGHDeal_4(u8 USARTxChannel)
{
	u8 i,j;
	settype.byte=BackstageProtocol4.DataBuffer[12+TotalLen68_4];
	
	if(settype.bit.S_E==1)//Ԥ��
	{
		Write_Loss_SetDeal_4(USARTxChannel);
		if(balance == 1)
		  Write_Loss_Set_YZ(USARTxChannel);
		else 
	  	SetSign=20;
	}
	else if(settype.bit.CR==1)//ȡ��
	{	
		for(i=0;i<sizeof(LockSetAddr);i++)
		{
			LockSetAddr[i]=0;//2
			LockSetType[i]=0;//1
			LockSetNum[i]=0;//1
			for(j=0;j<sizeof(LockSetValue[i]);j++)
				LockSetValue[i][j]=0;
		}
		if(balance == 1)
		  Write_Loss_Set_GH(USARTxChannel,9);
		else 
		  SetSign=25;
	}
	else if(settype.bit.S_E==0)//�̻�
	{
		for(i=0;i<LockSetTotal;i++)
		{
			InsertValue(i);
		}
		LockSet_Flag = 1;
		if(balance == 1)
		  Write_Loss_Set_GH(USARTxChannel,7);
		else 
		  SetSign=30;
	}
}

void Read_List_Deal_4(void)
{
	u8 i;
//	for(i=0;i<4;i++)
//		List_ID[i]=BackstageProtocol4.DataBuffer[14+TotalLen68_4+i];
	for(i=0;i<BackstageProtocol4.DataBuffer[18+TotalLen68_4];i++)
	 List_Name[i]=BackstageProtocol4.DataBuffer[19+TotalLen68_4+i];
	List_NameNum=BackstageProtocol4.DataBuffer[18+TotalLen68_4];
}
void Read_Doc_Deal_4(void)
{
	u8 i;
	for(i=0;i<BackstageProtocol4.DataBuffer[14+TotalLen68_4];i++)
	 Doc_Name[i]=BackstageProtocol4.DataBuffer[15+TotalLen68_4+i];
	Doc_NameNum=BackstageProtocol4.DataBuffer[14+TotalLen68_4];
}


void CommandProcess4(u8 USARTxChannel)
{
	static u8 CTRL;          //������C
	static u16 ADDR;         //��ַ��A
	static u8 TI = 0;        //���ͱ�ʶ
	static u8 COT = 0;       //����ԭ��
	static u16 object_addr;  //��Ϣ�����ַ

	if(datdone1==2)        //�̶�֡��
	{
	  if(Addr101_4==2)
		  ADDR= BackstageProtocol4.DataBuffer[2]+(BackstageProtocol4.DataBuffer[3]<<8);
		else ADDR= BackstageProtocol4.DataBuffer[2];
		CTRL = BackstageProtocol4.DataBuffer[1];
  }
	else if(datdone1==1)  //�ɱ�֡��
	{
		CTRL = BackstageProtocol4.DataBuffer[4];
	  if(Addr101_4==2)
		  ADDR= BackstageProtocol4.DataBuffer[5]+(BackstageProtocol4.DataBuffer[6]<<8);
		else ADDR= BackstageProtocol4.DataBuffer[5];
		TI = BackstageProtocol4.DataBuffer[6+FrontReason_4];
		COT =BackstageProtocol4.DataBuffer[8+FrontReason_4];
		
		object_addr = (u16)(BackstageProtocol4.DataBuffer[11+TotalLen68_4]<<8)+BackstageProtocol4.DataBuffer[10+TotalLen68_4];//��ǰ�ߺ���Ϣ���ַ
		
		if(TI==45 || TI==46)
		{
			control.byte = BackstageProtocol4.DataBuffer[12+TotalLen68_4];
		}
					
		if(balance == 1)    //ƽ��ģʽ
		{
			control_field.bit.DIR = 0x01;   //�ն˷���
			control_field.bit.PRM = 0x01;   //����վ
			control_field.bit.FCV = 0x01;   //֡����λ��Ч
			control_field.bit.FC  = 0x03;   //����/ȷ���û�����
			if((CTRL&0x0f) == 0x03)  //����/ȷ���û�����
			{
        if(CTRL&0x10)          //FCVλΪ1
				{
					control_field.bit.FCB ^= 0x01;
				}
			}
			if((control.bit.S_E==1) && (COT==6) && (Select101Addr == 1)) //<6>:=���� //Ԥ��
			{
				IEC101_Staid(0x81,USARTxChannel);                                               //<1>:=���Ͽ�
				Select101Addr=0;
			}		
      else
		    IEC101_Staid(0x80,USARTxChannel);                                               //<0>:=�Ͽ�
		}			
		else                //��ƽ��ģʽ
		{
		  IEC101_Staid(0x20,USARTxChannel);                                               //<0>:=�Ͽ�
		}	
	}
	
	if(ADDR==SystemSet.para.address||ADDR==0xFFFF||(ADDR==0xFF&&Addr101_4==1))
	{
		if(balance == 1) //ƽ��ģʽ
		{		
			if(datdone1==2)
			{
				if(BackstageProtocol4.DataBuffer[1]==0x40)	      //��վ��λԶ����·
				{
					IEC101_Staid(0x80,USARTxChannel);//��λ��·ȷ��
					os_dly_wait (30);	
					IEC101_Staid(0xC9,USARTxChannel);//������·
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x42)   //��վ������·���Թ���
				{
					IEC101_Staid(0x80,USARTxChannel);
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x49)	  //��վ������·״̬
				{
					IEC101_Staid(0x8B,USARTxChannel);
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x0B)	  //��վ��Ӧ��·״̬
				{
					IEC101_Staid(0xC0,USARTxChannel);//��λ��վ��·
					InitSign=1;
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x00)	  //��վ��Ӧȷ������
				{
					if(InitSign!=0)            //��ʼ��������
					{	
						if(InitSign==1)
						{
							InitEnd(USARTxChannel);//��ʼ������
							InitSign=2;
						}
						else
						  InitSign=0;		
					}
					else if(TotalSign!=0)//���ٻ���־
					{
						if(TotalSign==1)
						{
							Read_YX(USARTxChannel);
							TotalSign=2;
						}
						else if(TotalSign==2)
						{	
							Read_RealData(USARTxChannel);	
							TotalSign=3;
						}
						else if(TotalSign==3)
						{
							IEC101_Staid_All(USARTxChannel,10);
							TotalSign=4;
						}
						else 
						{
							TotalSign=0;
						  InitComplete=1;
						}
					}
					else if(ControlSign!=0)//���� 45������ƣ�46˫�����
					{
						if(ControlSign==1) 
						{
							Remote_Control(USARTxChannel,10,TI,object_addr);//���ƽ���
							ControlSign=2;
						}
						else ControlSign=0;
					}
					else if(EnergySign!=0)
					{
						if(EnergySign==1)
						{
						  IEC101_Staid_Enegy(5,USARTxChannel,10);
						}
						else if(EnergySign==2)
						{
						  IEC101_Staid_Enegy(0x45,USARTxChannel,10);
							Rand_Flag = 1;
						}
						EnergySign = 0;
					}
					else if(DocSign!=0)//�ļ�
					{	
						if(DocSign==1)//   �����ļ���һ֡
						{
							if(LineLockNum.Doc_Bz==1)
							{
								Read_Doc_Fix_Tou(USARTxChannel);														
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								Read_Doc_Rand_Tou(USARTxChannel);	
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								Read_Doc_Frzd_Tou(USARTxChannel);
							}							
							else if(LineLockNum.Doc_Bz==4)
							{
								Read_Doc_Sharp_Tou(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								 Read_Doc_Month_Tou(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								Read_Doc_Event_Tou(USARTxChannel);							
							}
						}					
						else if(DocSign==20)
						{
							if(LineLockNum.Doc_Bz==1)
							{
							  if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=59;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Fix_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=2;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Rand_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=61;
								else LineLockNum.Read_Place--;
								  LineLockNum.Read_CS++;
								Read_Doc_Frzd_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==4)
							{
								if(LineLockNum.Sharp_Time >= 264)
								{
									if(LineLockNum.Read_Place<3 && (LineLockNum.Read_Place+261)>=LineLockNum.Sharp_Place)
									{
										LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place+=261;
										if(LineLockNum.Read_Place == LineLockNum.Sharp_Place)
											LineLockNum.Read_EndBz=1;
										else
											LineLockNum.Read_EndBz=0;
									}
									else if(LineLockNum.Read_Place<3 && (LineLockNum.Read_Place+261)<LineLockNum.Sharp_Place)
									{									
										if(LineLockNum.Read_Place == 1)
										{
										  LineLockNum.Read_Num = 2;
										}
										else if(LineLockNum.Read_Place == 0)
										{
											if(LineLockNum.Sharp_Place == 262)
												LineLockNum.Read_Num = 2;
											else if(LineLockNum.Sharp_Place == 263)
												LineLockNum.Read_Num = 1;
										}
										LineLockNum.Read_Place=LineLockNum.Sharp_Place;
										LineLockNum.Read_EndBz=1;
									}
									else if(LineLockNum.Read_Place>3)
									{
										LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place-=3;
										LineLockNum.Read_EndBz=0;
									}
								}
								else
								{
									if(LineLockNum.Read_Place<3)
									{
										LineLockNum.Read_Num = LineLockNum.Read_Place;
										LineLockNum.Read_Place = 0;
										LineLockNum.Read_EndBz=1;
									}
                  else	
									{
                    LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place-=3;
										if(LineLockNum.Read_Place == 0)
											LineLockNum.Read_EndBz = 1;
										else
										  LineLockNum.Read_EndBz=0;
									}										
								}
								LineLockNum.Read_CS++;
								Read_Doc_Sharp_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=11;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Month_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								GetAddr();
								Read_Doc_Event_Data(USARTxChannel);
							}
						}			
					}
				}
		  }
		  else if(datdone1==1)
		  {
				if(TI==100)	//���ٻ�
				{
					IEC101_Staid_All(USARTxChannel,7);
					TotalSign=1;
					Soesend_cnt=Max_Time;
				}
				else if(TI==101 && COT==6)   //�ܵ��ٻ�/������ܼ�����
				{
					if(BackstageProtocol4.DataBuffer[12+TotalLen68_4]==5)       //�ٻ��������������
					{
						IEC101_Staid_Enegy(5,USARTxChannel,7);
            os_dly_wait (30);	
						IEC101_Enegy_Send(USARTxChannel);
						EnergySign = 1;
					}
					else if(BackstageProtocol4.DataBuffer[12+TotalLen68_4]==0x45) //���᲻����λ�ĵ������������
					{
						IEC101_Staid_Enegy(0x45,USARTxChannel,7);
						EnergySign = 2;
					}
				}
				else if(TI==103)	//ʱ��ͬ��
				{
					if(COT==6)
					{	
						 TimeNow.second  = ((u16)(BackstageProtocol4.DataBuffer[12+TotalLen68_4] | (BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8))/1000)&0x3F;  
						 TimeNow.minute  = BackstageProtocol4.DataBuffer[14+TotalLen68_4]&0x3F;
						 TimeNow.hour    = BackstageProtocol4.DataBuffer[15+TotalLen68_4]&0x1F;
						 TimeNow.date    = BackstageProtocol4.DataBuffer[16+TotalLen68_4]&0x1F;
						 TimeNow.month   = BackstageProtocol4.DataBuffer[17+TotalLen68_4]&0x0F;
						 TimeNow.year    = BackstageProtocol4.DataBuffer[18+TotalLen68_4]&0x7F;
						 correct_time();
						 Time_Synchronization(USARTxChannel,7);//����
						 CheckTime_Flag = 1;
					}
					else if(COT==5)
					{	
						Time_Synchronization(USARTxChannel,5);//��ǰʱ��
					}
				}	
				else if(TI==104)	//������·
				{	  
					TestAddr=BackstageProtocol4.DataBuffer[12+TotalLen68_4]+(BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8);
					Test_101(USARTxChannel);
				}
				else if(TI==105)	//��λ����
				{ 
					ResetLianlu(USARTxChannel);
					os_dly_wait (100);
					__set_FAULTMASK(1);
					NVIC_SystemReset();
				}
				else if(TI==45 || TI==46)	//45����ң�أ�46˫��ң��
				{						
					if((control.bit.S_E==1) && (COT==6))     //<6>:=���� //Ԥ��
					{
						if(Select101Addr == 0)
						{				
						  Remote_Control(USARTxChannel,7,TI,object_addr);                                    //<7>:=����ȷ��
						  Select101Addr=1;
						}
					}
					else if((control.bit.S_E==0) && (COT==6))//<6>:=����
					{							
						Remote_Control(USARTxChannel,7,TI,object_addr);                                    //<7>:=����ȷ��

						if(object_addr==(0x6001 + Ctrl_offset)) 
						{
							if((control.bit.QCS_RCS == 2) && (Select101Addr==1))  //QCS_RCS��1����
							{
								if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
								{
									RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
									SwitchFlag = 1;
								}
							}

							if((control.bit.QCS_RCS == 1) && (Select101Addr==1))  //QCS_RCS��0����
							{
								if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
								{
									RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
									SwitchFlag = 2;
								}
							}
						}
						
				    if(object_addr==0x4001)                            //45����ң��
						{
							if((control.bit.QCS_RCS == 1) && (Select101Addr==1))  //QCS_RCS��1����
							{
								if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
								{
									RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
									SwitchFlag = 1;
								}
							}
						}
						else if(object_addr==0x4002) 
						{
							if((control.bit.QCS_RCS == 0) && (Select101Addr==1))  //QCS_RCS��0����
							{
								if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
								{
									RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
									SwitchFlag = 2;
								}
							}
						}
						else if(object_addr==0x4003 || object_addr==(0x6001 + Reset_offset)) 
						{
							if(Select101Addr==1)
							{
								SigresetFlag=1;
							}
						}
						
						if(object_addr==0x6001)         //����ģ��ң������
						{
							if(Select101Addr==1) 
							{
								Clearloss_Flag = 1;
							}
						}
						else if(object_addr==0x6002)
						{
							if(Select101Addr==1) 
							{
								Clearevent_Flag = 1;
							}
						}
						ControlSign=1;
					}
					else if((control.bit.S_E==0) && (COT==8))//<8>:=ֹͣ����
					{							
						Remote_Control(USARTxChannel,9,TI,object_addr);                                    //<9>:=ֹͣ����ȷ��
						Select101Addr=0;
					}
					control.byte = 0;
				}
				else if(TI==200)//�л�����
				{
					SN_ID=(u16)(BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8)+BackstageProtocol4.DataBuffer[12+TotalLen68_4];  
					Write_Loss_SN(USARTxChannel);
				}
				else if(TI==201)//������
				{	  
					Read_Loss_SN(USARTxChannel);
				}
				else if(TI==202)//������
				{
					Read_Loss_SetDeal_4(USARTxChannel);
					Read_Loss_Set(USARTxChannel);
				}
				else if(TI==203)//д����
				{	
					Write_Loss_SetGHDeal_4(USARTxChannel);
				}
				else if(TI==210)//�ļ�����
				{
					if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==1)//��Ŀ¼
					{
							Read_List_Deal_4();
							Read_List_OK(USARTxChannel,SearchList());
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==3)//���ļ�����
					{
							Read_Doc_Deal_4();
							LineLockNum.Doc_Bz=SearchDoc();
							DocSign=1;//����
							if(LineLockNum.Doc_Bz==1)
							{
								Read_Doc_Fix_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								Read_Doc_Rand_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								Read_Doc_Frzd_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==4)
							{
								Read_Doc_Sharp_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								Read_Doc_Month_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								Read_Doc_Event_OK(USARTxChannel);							
							}
							else 
							{
								Read_Doc_Error(USARTxChannel);
								DocSign=0;
							}
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==6)//���ļ�ȷ��
					{
						//
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==7)//д�ļ�����
					{						
//						Write_DocOK_Deal();
//						Write_Doc_OK(USARTxChannel);
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==9)//д�ļ�
					{
						Write_Doc_Data_Deal();
						Write_Doc_Data_OK(USARTxChannel,DocErr);
					}
				}
				else if(TI==212)//�ļ�ͬ��
				{	
					Read_Doc_Same(USARTxChannel);
				}
				else if(TI==211) //������������ ���� ֹͣ
				{
					Updatetype.byte = BackstageProtocol4.DataBuffer[12+TotalLen68_4];
					if(COT==6 && Updatetype.bit.S_E==1) //����
					{
						SoftUpdate(USARTxChannel,7);
						LineLockNum.Write_Text=0;
						LineLockNum.Last_Text=0;
						LineLockNum.Next_Text=0;
					}
					else if(COT==8 && Updatetype.bit.S_E==0)//ȡ��
					{
						SoftUpdate(USARTxChannel,9);
					}
					else if(COT==6&&Updatetype.bit.S_E==0)//��������
					{
						SoftUpdate(USARTxChannel,10);
						os_dly_wait (2000);	//2S�Ժ�λ���³���
						SofeReset();
					}
				}	
		  }
    }
	  else //��ƽ��ģʽ
	  {
		  if(datdone1==2)
		  {
        if(BackstageProtocol4.DataBuffer[1]==0x40)	    //��λԶ����·
				{
					IEC101_Staid(0x20,USARTxChannel);             //��·��λȷ�ϣ���һ������
					InitSign=1;
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x49)	//������·״̬
				{
					IEC101_Staid(0x0B,USARTxChannel);             //��·״̬
				}
				else if((BackstageProtocol4.DataBuffer[1] & 0x40) && (BackstageProtocol4.DataBuffer[1] & 0x0F)==0x0A)	//����1���û�����
				{
					if(InitSign!=0)             //��ʼ��������
					{	
						if(InitSign==1)
						{
							InitEnd(USARTxChannel); //��ʼ������
						}
						InitSign=0;
					}
					else if(TotalSign!=0)//���ٻ���־
					{
						if(TotalSign==1)
						{
							IEC101_Staid_All(USARTxChannel,7);
							TotalSign=2;
						}
						else if(TotalSign==2)
						{
							Read_YX(USARTxChannel);
							TotalSign=3;
						}
						else if(TotalSign==3)
						{
							Read_RealData(USARTxChannel);
							TotalSign=4;
						}
						else if(TotalSign==4)
						{
							IEC101_Staid_All(USARTxChannel,10);
							TotalSign=0;
						}
						else TotalSign=0;
					}
					else if(ControlSign!=0)//���� 45������ƣ�46˫�����
					{	
						if(ControlSign==1) 
						{
							if((control.bit.S_E==1) && (COT==6))     //<6>:=���� //Ԥ��
							{
								ControlSign=0;
								if(Select101Addr == 0)
								{
									Remote_Control(USARTxChannel,7,TI,object_addr);                                 //<7>:=����ȷ��
									Select101Addr=1;
								}
								else
								{
									IEC101_Staid(0x21,USARTxChannel);                                               //<1>:=���Ͽ�
									Select101Addr = 0;
								}
							}
							else if((control.bit.S_E==0) && (COT==6))//<6>:=���� //ִ��
							{	
                ControlSign=2;								
								if(object_addr==(0x6001 + Ctrl_offset))                                      //46˫��ң��
								{
									if((control.bit.QCS_RCS == 2) && (Select101Addr==1))  //QCS_RCS��1����
									{
										if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
										{
											RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
											SwitchFlag = 1;
										}
									}

									if((control.bit.QCS_RCS == 1) && (Select101Addr==1))  //QCS_RCS��0����
									{
										if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
										{
											RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
											SwitchFlag = 2;
										}
									}
								}
								else if(object_addr==0x4001)                            //45����ң��
								{
									if((control.bit.QCS_RCS == 1) && (Select101Addr==1))  //QCS_RCS��1����
									{
										if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
										{
											RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
											SwitchFlag = 1;
										}
									}
								}
								else if(object_addr==0x4002) 
								{
									if((control.bit.QCS_RCS == 0) && (Select101Addr==1))  //QCS_RCS��0����
									{
										if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
										{
											RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
											SwitchFlag = 2;
										}
									}
								}
								else if(object_addr==0x4003 || object_addr==(0x6001 + Reset_offset)) 
								{
									if(Select101Addr==1)
									{
										SigresetFlag=1;
									}
								}
								
								if(object_addr==0x6001)              //����ģ��ң������
								{
									if(Select101Addr==1) 
									{
										Clearloss_Flag = 1;
									}
								}
								else if(object_addr==0x6002)
								{
									if(Select101Addr==1) 
									{
										Clearevent_Flag = 1;
									}
								}
								Remote_Control(USARTxChannel,7,TI,object_addr);                                //<7>:=ң��ִ��ȷ��
							}
							else if((control.bit.S_E==0) && (COT==0))//<0>:=ֹͣ����
							{
								Remote_Control(USARTxChannel,9,TI,object_addr);                                //<9>:=ֹͣ����ȷ��
								Select101Addr=0;
								ControlSign=0;
							}
							control.byte = 0;
						}
						else if(ControlSign==2)
						{
							Remote_Control(USARTxChannel,10,TI,object_addr);//ң��ִ�н���
							ControlSign=0;
						}
						else ControlSign=0;
					}
					else if(ResetSign!=0)//��λ��·
					{
						if(ResetSign==1)
						{
							ResetLianlu(USARTxChannel);
							os_dly_wait (100);
							__set_FAULTMASK(1);
							NVIC_SystemReset();
							ResetSign=0;
						}
						else ResetSign=0;
					}
					else if(SetSign!=0)//��λ��·
					{
						if(SetSign==1) //�л�SN
						{
							Write_Loss_SN(USARTxChannel);
							SetSign=0;
						}
						else if(SetSign==5)//��SN
						{
							Read_Loss_SN(USARTxChannel);
							SetSign=0;
						}
						else if(SetSign==10)//������
						{
							Read_Loss_Set(USARTxChannel);
							SetSign=0;
						}
						else if(SetSign==20)//Ԥ��
						{
							Write_Loss_Set_YZ(USARTxChannel);
							SetSign=0;
						}
						else if(SetSign==25)//ȡ��
						{
							Write_Loss_Set_GH(USARTxChannel,9);
							SetSign=0;
						}
						else if(SetSign==30)//�̻�
						{
							Write_Loss_Set_GH(USARTxChannel,7);
							SetSign=0;
						}
						else SetSign=0;
					}
					else if(EnergySign!=0)
					{
							if(EnergySign==1)
							{
								EnergySign=2;
								IEC101_Staid_Enegy(5,USARTxChannel,7);
							}
							else if(EnergySign==2)
							{
								EnergySign=3;
								IEC101_Enegy_Send(USARTxChannel);
							}
							else if(EnergySign==3)
							{
								EnergySign=0;
								IEC101_Staid_Enegy(5,USARTxChannel,10);
							}
							else if(EnergySign==4)
							{
								EnergySign=5;
                IEC101_Staid_Enegy(0x45,USARTxChannel,7);
							}
							else if(EnergySign==5)
							{
								EnergySign=0;
                IEC101_Staid_Enegy(0x45,USARTxChannel,10);
								Rand_Flag = 1;
							}		
							else EnergySign=0;
					}
//					else if(SOE_Flag!=0)//��λң��
//					{
//						Record_SOE_Send(USARTxChannel);  //SOE�¼�˳���¼  
//					}        
					else 
						IEC101_Staid(0x09,USARTxChannel);           //0 1 0ACD(Ҫ�����λ) 0DFC(����������λ) (��Ϊ0����ʹ��) 1001(������9Ϊ�����ٻ�������)  
				}
				else if((BackstageProtocol4.DataBuffer[1] & 0x40) && (BackstageProtocol4.DataBuffer[1] & 0x0F)==0x0B)	//����2���û�����
				{
					KZQMeasureData.para.SelfCheck.bit.GPRS_state = 2;

					if(ClockSign!=0)
					{
						if(ClockSign==1)
						{
							ClockSign=0;
							Time_Synchronization(USARTxChannel,7);//����
							CheckTime_Flag = 1;
						}
						else if(ClockSign==5)
						{	
							ClockSign=0;
							Time_Synchronization(USARTxChannel,5);//��ǰʱ��
						}
						else ClockSign=0;
					}
					else if(TestSign!=0)//��·����
					{
						if(TestSign==1)
						{
							Test_101(USARTxChannel);
							TestSign=0;
						}
						else TestSign=0;
					}
					else if(DocSign!=0)//�ļ�
					{	
						if(DocSign==1) //��Ŀ¼����ȷ��
						{
							LineLockNum.Read_CS=0;
							Read_List_OK(USARTxChannel,SearchList());
							DocSign=0;	
						}
						else if(DocSign==10)//���ļ�����ȷ��
						{	
							DocSign=11;
							if(LineLockNum.Doc_Bz==1)
							{
								Read_Doc_Fix_OK(USARTxChannel);														
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								Read_Doc_Rand_OK(USARTxChannel);	
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								Read_Doc_Frzd_OK(USARTxChannel);
							}							
							else if(LineLockNum.Doc_Bz==4)
							{
								Read_Doc_Sharp_OK(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								 Read_Doc_Month_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								Read_Doc_Event_OK(USARTxChannel);							
							}
							else 
							{
								Read_Doc_Error(USARTxChannel);
								DocSign=0;
							}
						}
						else if(DocSign==11)//   �����ļ���һ֡
						{
							if(LineLockNum.Doc_Bz==1)
							{
								Read_Doc_Fix_Tou(USARTxChannel);														
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								Read_Doc_Rand_Tou(USARTxChannel);	
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								Read_Doc_Frzd_Tou(USARTxChannel);
							}							
							else if(LineLockNum.Doc_Bz==4)
							{
								Read_Doc_Sharp_Tou(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								 Read_Doc_Month_Tou(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								Read_Doc_Event_Tou(USARTxChannel);							
							}
						}
						else if(DocSign==20)
						{			
							if(LineLockNum.Doc_Bz==1)
							{
							  if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=59;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Fix_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=2;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Rand_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=61;
								else LineLockNum.Read_Place--;
								  LineLockNum.Read_CS++;
								Read_Doc_Frzd_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==4)
							{
								if(LineLockNum.Sharp_Time >= 264)
								{
									if(LineLockNum.Read_Place<3 && (LineLockNum.Read_Place+261)>=LineLockNum.Sharp_Place)
									{
										LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place+=261;
										if(LineLockNum.Read_Place == LineLockNum.Sharp_Place)
											LineLockNum.Read_EndBz=1;
										else
											LineLockNum.Read_EndBz=0;
									}
									else if(LineLockNum.Read_Place<3 && (LineLockNum.Read_Place+261)<LineLockNum.Sharp_Place)
									{									
										if(LineLockNum.Read_Place == 1)
										{
										  LineLockNum.Read_Num = 2;
										}
										else if(LineLockNum.Read_Place == 0)
										{
											if(LineLockNum.Sharp_Place == 262)
												LineLockNum.Read_Num = 2;
											else if(LineLockNum.Sharp_Place == 263)
												LineLockNum.Read_Num = 1;
										}
										LineLockNum.Read_Place=LineLockNum.Sharp_Place;
										LineLockNum.Read_EndBz=1;
									}
									else if(LineLockNum.Read_Place>3)
									{
										LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place-=3;
										LineLockNum.Read_EndBz=0;
									}
								}
								else
								{
									if(LineLockNum.Read_Place<3)
									{
										LineLockNum.Read_Num = LineLockNum.Read_Place;
										LineLockNum.Read_Place = 0;
										LineLockNum.Read_EndBz=1;
									}
                  else	
									{
                    LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place-=3;
										if(LineLockNum.Read_Place == 0)
											LineLockNum.Read_EndBz = 1;
										else
										  LineLockNum.Read_EndBz=0;
									}										
								}
								LineLockNum.Read_CS++;
								Read_Doc_Sharp_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=11;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Month_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								GetAddr();
								Read_Doc_Event_Data(USARTxChannel);
							}
						}
					}
					else if(WriteDocSign!=0)
					{
						if(WriteDocSign==1)
						{
							Write_Doc_OK(USARTxChannel);
							WriteDocSign=0;
						}
						else if(WriteDocSign==2)
						{
							Write_Doc_Data_OK(USARTxChannel,DocErr);
							WriteDocSign=0;
						}
						else WriteDocSign=0;
					}
					else if(SoftUpSign!=0)
					{
						if(SoftUpSign==1)//����
						{
							 SoftUpdate(USARTxChannel, 7);
							 SoftUpSign=0;
						}
						else if(SoftUpSign==5)// ����
						{
							 SoftUpdate(USARTxChannel, 10);
							 SoftUpSign=0;
							 os_dly_wait (2000);	//2S�Ժ�λ���³���
						   SofeReset();
						}
						else if(SoftUpSign==10)//��ֹ
						{
							 SoftUpdate(USARTxChannel, 9);
							 SoftUpSign=0;
						}
						else SoftUpSign=0;
					}
					else if(DocSameSign!=0)
					{
							if(DocSameSign==1)
							{
								DocSameSign=0;
								Read_Doc_Same(USARTxChannel);
							}
							else DocSameSign=0;
					}
					else
					{
						//���ж��Ƿ���1������ ң�ű�λ ���������һֱ�ص�08����һ�����ݻ�28			
						H2SJ(USARTxChannel); //�ر仯ң��
					}
				}
		  }
		  else if(datdone1==1)//����0x68��ͷ
		  {
				if(TI==100)	//���ٻ�
				{		
					TotalSign=1;
				}
				else if(TI==101 && COT==6)   //�ܵ��ٻ�/������ܼ�����
				{
					if(BackstageProtocol4.DataBuffer[12+TotalLen68_4]==5)       //�ٻ��������������
					{
						EnergySign=1;
					}
					else if(BackstageProtocol4.DataBuffer[12+TotalLen68_4]==0x45) //���᲻����λ�ĵ������������
					{
						EnergySign=4;
					}
				}
				else if(TI==103)	//ʱ��ͬ��
				{
					if(COT==6)
					{
						TimeNow.second  = ((u16)(BackstageProtocol4.DataBuffer[12+TotalLen68_4] | (BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8))/1000)&0x3F;  
						TimeNow.minute  = BackstageProtocol4.DataBuffer[14+TotalLen68_4]&0x3F;
						TimeNow.hour    = BackstageProtocol4.DataBuffer[15+TotalLen68_4]&0x1F;
						TimeNow.date    = BackstageProtocol4.DataBuffer[16+TotalLen68_4]&0x1F;
						TimeNow.month   = BackstageProtocol4.DataBuffer[17+TotalLen68_4]&0x0F;
						TimeNow.year    = BackstageProtocol4.DataBuffer[18+TotalLen68_4]&0x7F;
						correct_time();
						ClockSign=1;
					}
					else if(COT==5)
					{	
						ClockSign=5;
					}
				}	
				else if(TI==104)	//������·
				{
					TestAddr=BackstageProtocol4.DataBuffer[12+TotalLen68_4]+(BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8);
					TestSign=1;
				}
				else if(TI==105)	//��λ����
				{
					ResetSign=1;
				}
				else if(TI==45 || TI==46)	//45����ң�أ�46˫��ң��
				{
					ControlSign=1;
				}
				else if(TI==200)//�л�����
				{
					SN_ID=(u16)(BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8)+BackstageProtocol4.DataBuffer[12+TotalLen68_4];
					SetSign=1;  
				}
				else if(TI==201)//������
				{
					SetSign=5; 
				}
				else if(TI==202)//������
				{
					SetSign=10;	
					Read_Loss_SetDeal_4(USARTxChannel);
				}
				else if(TI==203)//д����
				{
					Write_Loss_SetGHDeal_4(USARTxChannel);
				}
				else if(TI==210)//�ļ�����
				{
					if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==1)//��Ŀ¼
					{
						Read_List_Deal_4();
						DocSign=1;
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==3)//���ļ�����
					{
						Read_Doc_Deal_4();
						LineLockNum.Doc_Bz=SearchDoc();
						DocSign=10;//����
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==6)//���ļ�ȷ��
					{
				
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==7)//д�ļ�����
					{				
						//Write_DocOK_Deal();
						//WriteDocSign=1;
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==9)//д�ļ�
					{
						//Write_Doc_Data_Deal();
						//WriteDocSign=3;
					}
				}
				else if(TI==212)//�ļ�ͬ��
				{
					DocSameSign=1;
				}
				else if(TI==211) //������������ ���� ֹͣ
				{
					Updatetype.bit.S_E=BackstageProtocol4.DataBuffer[12+TotalLen68_4];
					if(COT==6 && Updatetype.bit.S_E==1) //����
					{
						SoftUpSign=1;
						LineLockNum.Write_Text=0;
						LineLockNum.Last_Text=0;
						LineLockNum.Next_Text=0;
					}
					else if(COT==8&&Updatetype.bit.S_E==0)//ȡ��
					{
						SoftUpSign=10;
					}
					else if(COT==6&&Updatetype.bit.S_E==0)//��������
					{
						SoftUpSign=5;
					}
				}	
		  }
    }
	}
}
