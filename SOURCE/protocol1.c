//###########################################################################
//
// ����1ͨѶЭ���ļ�
// GPRSģ��ͨѶ��
// 
//
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  09.10| 19 may 2018 | S.H.Q. |
//###################################+########################################
#include "protocol1.h"
#include "Encryption.h"
#include "queue.h"
#include "flash.h"
#include "ds1302.h"
#include "string.h"
#include "GPS.h"

u8 Soesend_cnt = 0;
u8 List_Name[30];//Ŀ¼���� ��ʷ�洢 ��ֹ����֡����-0x1fff7a10
u8 List_ID[4]={0};//Ԥ��0���
u8 List_NameNum = 0;
u8 Doc_Name[20];
u8 Doc_NameNum = 0;
u8 Doc_ID[4]={0};//�ļ�ID Ԥ�� ���� 0���
u8 JHDoc_ID[4]={0};//�����ļ�ID д�ļ�ʱ��Ҫ�ж�
u16 SN_ID = 0;
u16 TestAddr;//������·ͼ��
//��־���ȼ� ��ʾ����ͼ���
u8 InitSign = 0;
u8 InitComplete = 0;//��ʼ������
u8 ControlSign = 0;//ң�ز���
u8 TotalSign = 0;//���ٻ�
u8 SetSign = 0;//����
u8 DocSign = 0;//�ļ�
u8 TestSign = 0;//���� ��ƽ��ʹ�� ƽ�ⲻʹ��
u8 EnergySign = 0;//�ٻ�����������
u8 ClockSign = 0;
u8 ResetSign = 0;
u8 DocSameSign = 0;
u8 WriteDocSign = 0;
u8 SoftUpSign = 0;
//д�ļ� ��С
u32 DocLen = 0;//�ļ��ܴ�С
u8 DocErr = 0;//�ļ����ж�
u8 DocByte[240];//�ļ�����
u8 DocByteNum = 0;//�ļ���С
u8 CheckTime_Flag = 0;
u8 Clearloss_Flag = 0;
u8 Clearevent_Flag = 0;
u8 Rand_Flag = 0;
u8 LockSet_Flag = 0;
//������ д���� ��Ӧ����
u16 LockSetAddr[20];
u8  LockSetValue[20][24];
u8  LockSetType[20],LockSetNum[20];//ֻ����4�ֽ�һ�µĲ�������
u8  LockSetTotal;//�趨�ܸ���
//Ŀ¼���� 
const char List_Fixd[]="LINELOSS/FIXD";
const char List_Rand[]="LINELOSS/RAND";
const char List_Frzd[]="LINELOSS/FRZD";
const char List_Sharpd[]="LINELOSS/SHARPD";
const char List_Monthd[]="LINELOSS/MONTHD";
const char List_Eventd[]="LINELOSS/EVENTD";
//�ļ�����
const char Doc_Fixd[]="fixd";
const char Doc_Rand[]="rand";
const char Doc_Frzd[]="frzd";
const char Doc_Sharpd[]="sharpd";
const char Doc_Monthd[]="monthd";
const char Doc_Eventd[]="eventd";
/* �ṹ�嶨�� ----------------------------------------------------------------*/
struct BACKSTAGE_PROTOCOL_TYPE1 BackstageProtocol1;
struct BACKSTAGE_PROTOCOL_TYPE1 BackstageProtocol1bak;
static volatile u8 ProtocolResolveState = 0;	// Э���������״̬��
static u8 ycls[120];//�������ݻ���
static s32 ycls1[30];//ʵʱ���ݻ���
struct cp56time2a DocTime[6];
union Control_Field control_field;//control1;	
union Control_Type control;//control1;	
union Set_Type settype,Updatetype;	
static u8 channel = 0;			// ����ͨ��
static u16 gyjsq,RxCounter1=0;
static u8 datdone1=0;
static u16 Last_Number = 0;
static u32 GPRSUpdateLength = 0;

#ifdef SIM800C_MODEL
static u16 GprsConnectState = 0; 	// ����״̬��
static u16 GprsDelay = 0;					// ģ����ʱ
static u16 GprsTimeout = 0;				// ģ��ʱ�������־
static u16 GprsDataMode = 0;		  // GPRS͸��ģʽ
u16 GprsWdg = 0;			            // GPRSģ���Զ��忴�Ź�
u8  IPswitchFlag = 0;		          // IP��ַ�����л���־
#endif

void GetAddr(void);
//�ļ�Ŀ¼����
u8 SearchList(void)
{
	u8 i,flag=1,m=0;
	for(i=0;i<List_NameNum;i++)
	{
		if(i<strlen(List_Fixd))
		{
			if(List_Name[i]!=List_Fixd[i]) flag=0;
		}
		else break;
	}
	if(flag)
		m=1;
	else
	{
		flag=1;
		for(i=0;i<List_NameNum;i++)
		{
			if(i<strlen(List_Rand))
			{
				if(List_Name[i]!=List_Rand[i]) flag=0;
			}
			else break;
		}
		if(flag)
			m=2;
		else 
		{
			flag=1;
			for(i=0;i<List_NameNum;i++)
			{
				if(i<strlen(List_Frzd))
				{
					if(List_Name[i]!=List_Frzd[i]) flag=0;
				}
				else break;
			}
			if(flag)
				m=3;
			else
			{
				flag=1;
				for(i=0;i<List_NameNum;i++)
				{
					if(i<strlen(List_Sharpd))
					{
						if(List_Name[i]!=List_Sharpd[i]) flag=0;
					}
					else break;
				}
				if(flag)
					m=4;
				else
				{
					flag=1;
					for(i=0;i<List_NameNum;i++)
					{
						if(i<strlen(List_Monthd))
						{
							if(List_Name[i]!=List_Monthd[i]) flag=0;
						}
						else break;
					}
					if(flag)
						m=5;
					else
					{
						flag=1;
						for(i=0;i<List_NameNum;i++)
						{
							if(i<strlen(List_Eventd))
							{
								if(List_Name[i]!=List_Eventd[i]) flag=0;
							}
							else break;
						}
						if(flag)
							m=6;
					}
				}
			}
		}
	}
	return m;			
}

u8 SearchDoc(void)
{
	u8 i,flag=1,m=0;
	for(i=0;i<Doc_NameNum;i++)
	{
		if(i<strlen(Doc_Fixd))
		{
			if(Doc_Name[i]!=Doc_Fixd[i]) flag=0;
		}
		else break;
	}
	if(flag)
		m=1;
	else
	{
		flag=1;
		for(i=0;i<Doc_NameNum;i++)
		{
			if(i<strlen(Doc_Rand))
			{
				if(Doc_Name[i]!=Doc_Rand[i]) flag=0;
			}
			else break;
		}
		if(flag)
			m=2;
		else 
		{
			flag=1;
			for(i=0;i<Doc_NameNum;i++)
			{
				if(i<strlen(Doc_Frzd))
				{
					if(Doc_Name[i]!=Doc_Frzd[i]) flag=0;
				}
				else break;
			}
			if(flag)
				m=3;
			else
			{
				flag=1;
				for(i=0;i<Doc_NameNum;i++)
				{
					if(i<strlen(Doc_Sharpd))
					{
						if(Doc_Name[i]!=Doc_Sharpd[i]) flag=0;
					}
					else break;
				}
				if(flag)
					m=4;
				else
				{
					flag=1;
					for(i=0;i<Doc_NameNum;i++)
					{
						if(i<strlen(Doc_Monthd))
						{
							if(Doc_Name[i]!=Doc_Monthd[i]) flag=0;
						}
						else break;
					}
					if(flag)
						m=5;
					else
					{
						flag=1;
						for(i=0;i<Doc_NameNum;i++)
						{
							if(i<strlen(Doc_Eventd))
							{
								if(Doc_Name[i]!=Doc_Eventd[i]) flag=0;
							}
							else break;
						}
						if(flag)
							m=6;
					}
				}
			}
		}
	}
	return m;			
}

u8 UsartAdd(u8 *p,u8 sti,u16 zhnum)
{
	u16 i;
	u8 zhadd=0;
	for(i=0;i<zhnum;i++)
		zhadd+=p[i+sti];
	return zhadd;
}

/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolve1(u8 receivedata)
*
*��    �ܣ�����ͨѶЭ�����
*
*��ڲ���������1����
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolve1(u8 receivedata)
{
	//InsertDataToQueue(&QueueTX1,BufferTX1,receivedata);
	switch(ProtocolResolveState)
	{
		case 0:  //���յ��µ�����֡
			if(receivedata == 0x10)			// �ж�����ͷ����վ����վ���͵������ַ�
			{
				if(Usart1RxReady == 0)		//δ�������
				{
					channel = 0;
					BackstageProtocol1.DataBuffer[0] = receivedata;//�����bak��������һ������ʵʱ������һ�����Ǳ��ݣ�channel=0��Ӧʵʱ����channel=1��Ӧ����
					RxCounter1=1;
					gyjsq=TotalLen10;								//��ʣTotalLen10-1 ���ַ�û�н���
					ProtocolResolveState = 1;//���յ�һ���ַ�
				}
				else if(Usart1bakRxReady == 0)
				{
					channel = 1;
					BackstageProtocol1bak.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=TotalLen10;
					ProtocolResolveState = 1;
				}
			}
			else if (receivedata == 0x68)//���٣���վ����վ�������������ַ�
			{
				if(Usart1RxReady == 0)
				{
					channel = 0;
					BackstageProtocol1.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=3;
					ProtocolResolveState = 2;
				}
				else if(Usart1bakRxReady == 0)
				{
					channel = 1;
					BackstageProtocol1bak.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=3;
					ProtocolResolveState = 2;
				}
			}
			else 
				ProtocolResolveState = 0;
			break;
		case 1://֮ǰ�Ѿ����յ�����0x10
      if(channel == 0)//����BackstageProtocol1�е�����
			{
				gyjsq--;//ʣ��������1
				BackstageProtocol1.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)								//����Ѿ����յ�������ַ���
			  {
					if(BackstageProtocol1.DataBuffer[Addr101+2]==UsartAdd(BackstageProtocol1.DataBuffer,1,Addr101+1))//����101��Լ������ȵ�4�����ݵ��ڵ�2�͵�3������֮�ͣ����ĸ���У��λ
					{
							datdone1=2;
							Usart1RxReady=1;
							isr_evt_set (0x0001, t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
					}
					ProtocolResolveState = 0;
			  }
			}
			else
			{
				gyjsq--;
				BackstageProtocol1bak.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)								//	//�ж�
			  {
					if(BackstageProtocol1bak.DataBuffer[Addr101+2]==UsartAdd(BackstageProtocol1bak.DataBuffer,1,Addr101+1))
					{
							datdone1=2;
							Usart1bakRxReady=1;
							isr_evt_set (0x0001, t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
					}
					ProtocolResolveState = 0;
			  }
			}
			break;
		case 2://��һ�����֮ǰ�Ѿ����յ�0x68����������ֻ��4���ַ�������һ����5���ַ�
			if(channel == 0)
			{
				gyjsq--;
				BackstageProtocol1.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)
				{
					if(BackstageProtocol1.DataBuffer[1]==BackstageProtocol1.DataBuffer[2]&&BackstageProtocol1.DataBuffer[3]==0x68)
					{																											//����1�͵�4�����ݶ���0x68����2�͵�3��������ͬ��101��Լ������ȷʵ���
						gyjsq=BackstageProtocol1.DataBuffer[1]+2;           //��2�����ݱ�ʾ����֮��Ҫ���͵����ݳ���
						ProtocolResolveState = 3;						                //���뵽������ģʽ
					}
					else if(BackstageProtocol1.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol1.DataBuffer[1]+(BackstageProtocol1.DataBuffer[2]<<8)+2;
						ProtocolResolveState = 4;                          	
					}
					else
						ProtocolResolveState = 0;                           //���������Ϊ�ǽ��ճ����˴���
				}
			}
			else
			{
				gyjsq--;
				BackstageProtocol1bak.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)
				{
					if(BackstageProtocol1bak.DataBuffer[1]==BackstageProtocol1bak.DataBuffer[2]&&BackstageProtocol1bak.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol1bak.DataBuffer[1]+2;
						ProtocolResolveState = 3;						
					}
					else if(BackstageProtocol1bak.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol1bak.DataBuffer[1]+(BackstageProtocol1bak.DataBuffer[2]<<8)+2;
						ProtocolResolveState = 4;                          	
					}
					else
					ProtocolResolveState = 0;
			  }
			}
			break;
		case 3://ģʽ2����������ĵ�����ģʽ��׼��������������������
		  if(channel == 0) //������������ж��Ƿ���BackstageProtocol1�л��Ƿ���BackstageProtocol1bak��
			{
				gyjsq--;
				BackstageProtocol1.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol1.DataBuffer[RxCounter1-1]==0x16)//�ж����һ���Ƿ�Ϊ0x16�������֡β
				  {
						  if(BackstageProtocol1.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol1.DataBuffer,4,BackstageProtocol1.DataBuffer[1]))
						  {																								 //������2�����ݵ��ڵ�5������13������֮�ͣ���ʵ���ǳ��������֡β�Լ�У��λ����������֮�͵���У��λ
								Usart1RxReady=1;
							  datdone1=1;	
								isr_evt_set (0x0001, t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
						  }																								 //�ɴ�Ҳ���Կ����������������ɣ�����ȥ������ȥ������Щ���ݣ�0x10������������һ��ͨ�ż�����֣�0x68��������ͨ�ŵĿ�ʼ
				  }																										 //�����CommandProcess1��������Ӧ�þ��Ǵ�����Щ���ݵ�
				  ProtocolResolveState=0;
			  }	  
			}	
			else
			{
				gyjsq--;
				BackstageProtocol1bak.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol1bak.DataBuffer[RxCounter1-1]==0x16)
				  {
						if(BackstageProtocol1bak.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol1bak.DataBuffer,4,BackstageProtocol1bak.DataBuffer[1]))
						{
							Usart1bakRxReady=1;
							datdone1=1;	
							isr_evt_set (0x0001, t_Task4);
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
				BackstageProtocol1.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol1.DataBuffer[RxCounter1-1]==0x16)//�ж����һ���Ƿ�Ϊ0x16�������֡β
				  {
						  if(BackstageProtocol1.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol1.DataBuffer,4,BackstageProtocol1.DataBuffer[1]|(BackstageProtocol1.DataBuffer[2]<<8)))
						  {																								 //������2�����ݵ��ڵ�5������13������֮�ͣ���ʵ���ǳ��������֡β�Լ�У��λ����������֮�͵���У��λ
								Usart1RxReady=1;
							  datdone1=1;	
								isr_evt_set (0x0001, t_Task4);   							 //�������4ר������������ЩЭ�飬�������֮�⻹��������һЩЭ��
						  }																								 //�ɴ�Ҳ���Կ����������������ɣ�����ȥ������ȥ������Щ���ݣ�0x10������������һ��ͨ�ż�����֣�0x68��������ͨ�ŵĿ�ʼ
				  }																										 //�����CommandProcess1��������Ӧ�þ��Ǵ�����Щ���ݵ�
				  ProtocolResolveState=0;
			  }	  
			}	
			else
			{
				gyjsq--;
				BackstageProtocol1bak.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol1bak.DataBuffer[RxCounter1-1]==0x16)
				  {
						if(BackstageProtocol1bak.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol1bak.DataBuffer,4,BackstageProtocol1bak.DataBuffer[1]|(BackstageProtocol1bak.DataBuffer[2]<<8)))
						{
							Usart1bakRxReady=1;
							datdone1=1;	
							isr_evt_set (0x0001, t_Task4);
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

/****************************************************************************
*
*��    �ƣ�void GprsSendStr(u8 *str)
*
*��    �ܣ�GPRSģ�鷢���ַ���
*
*��ڲ������ַ����׵�ַ
*
*���ز�����
*
****************************************************************************/
void GprsSendStr(char *str)
{
	char *strcopy;
	strcopy = str;
	while((*strcopy) != '\0')
	{
		InsertDataToQueue(&QueueTX1,BufferTX1,*strcopy);
		strcopy++;
	}	
}

/****************************************************************************
*
*��    �ƣ�void GprsSendByte(u8 byte)
*
*��    �ܣ�GPRSģ�鷢��һ���ֽ�
*
*��ڲ�����һ���ֽ�����
*
*���ز�����
*
****************************************************************************/
void GprsSendByte(u8 byte)
{
	InsertDataToQueue(&QueueTX1,BufferTX1,byte);
}	 

#ifdef Four_Faith_MODEL
/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolveEncryptionData(u8 receivedata)
*
*��    �ܣ�GPRS����ͨѶЭ�����
*
*��ڲ�������������
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolveEncryptionData(u8 receivedata)
{
	if(Measure101Para.para.encrypt == 0)
	{
		if(ProtocolResolve1(receivedata)==0x0D)					// ����������
		{
			KZQMeasureData.para.SelfCheck.bit.GPRS_state = 0;
		}
	}
	else if(Measure101Para.para.encrypt == 1)
	{
		Encryption_Handle(receivedata,1);
	}
	return 0;
}
#endif

#ifdef SIM800C_MODEL
/****************************************************************************
*
*��    �ƣ�u8 GprsConnectCtrl(void)
*
*��    �ܣ�GPRS���ӿ���
*
*��ڲ�������
*
*���ز�����
*
****************************************************************************/
u8 GprsConnectCtrl(void)
{
	GprsWdg++;		// ���Ź�����
	if(GprsWdg>300)
	{
		//GprsConnectState = 0;	 // ���������û�з��ش������ݣ��ر�ģ���Դ��������
		GprsWdg = 0;
	}
		
	// ������ʱ	
	if(GprsTimeout==0)
	{
		if(GprsDelay>0)
		{
			 GprsDelay--;
		}
		else
		{
			 GprsTimeout = 1;
		}	
	}
	// ����״̬��ת
	switch(GprsConnectState)
	{
		case 0:		// �ر�ģ���Դ
			GPRS_POW_OFF;
			GPRS_PWRKEY_OFF;
			GprsConnectState = 1;
			GprsDataMode = 0;
			GprsDelay = 1; 	// ��ʱ
			GprsTimeout = 0;
		  KZQMeasureData.para.SelfCheck.bit.GPRS_state = 1;
			break;
		case 1:		// ��ģ���Դ������PWRKEY
			if(GprsTimeout == 1)
			{
				GPRS_POW_ON;
				GPRS_PWRKEY_OFF;
				GprsConnectState = 2;
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
			}
			break;
		case 2:		// ����PWRKEY
			if(GprsTimeout == 1)
			{
				GPRS_POW_ON;
				GPRS_PWRKEY_ON;
				GprsConnectState = 3;
				GprsDelay = 1; 	// ��ʱ
				GprsTimeout = 0;
			}
			break;
		
		case 3:		// ȡ������
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_ATE);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 1; 	// ��ʱ
				GprsTimeout = 0;
				GprsConnectState = 4;
			}
			break;
		case 4:		// ��ͨ�Ź���
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_CFUN);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 1; 	// ��ʱ
				GprsTimeout = 0;
				GprsConnectState = 5;
			}
			break;
		case 5:		// ѡ��͸��ģʽ
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_MODE);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 1; 	// ��ʱ
				GprsTimeout = 0;
				GprsConnectState = 6;
			}
			break;
		case 6:		// ��ȡ�ź�ǿ��
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_CSQ);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 7;
			}		
			break;
		case 7:		// �Ƿ���GPRS����
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_CGATT1);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 8;
			}		
		case 8:		// �Ƿ�ע������
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_CGREG1);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 9;
			}		
			break;
		case 9:		// �ر��ƶ��������ص�IP INITIAL
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_SHUT);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 10;
			}		
			break;
		case 10:   // ����APN IP INITIAL
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_CSTT);		
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 11;
			}
			break;
		case 11:   // �����ƶ�����
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_CIICR);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 12;
			}
			break;
		case 12:   // ��ñ���IP
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_CIFSR);
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 13;
			}
			break;
		case 13:   // ������ģʽ IP INITIAL
			if(GprsTimeout == 1)
			{
				//GprsSendStr(SIM900B_SERVER);		
				//GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 14;
			}
			break;
		case 14:
			if(GprsTimeout == 1)
			{
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
				if(IPswitchFlag == 0)
				{
					IPswitchFlag = 1;
					GprsConnectState = 15;
				}
				else
				{
					IPswitchFlag = 0;
					GprsConnectState = 16;
				}
				//GprsConnectState = 14;
			}
			break;
		case 15:
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_START);
				GprsSendByte(34);	// " IP1
				GprsSendByte(((SystemSet.para.IP1[0]>>8)/100)+0x30);
				GprsSendByte((((SystemSet.para.IP1[0]>>8)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.IP1[0]>>8)%10)+0x30);
				GprsSendByte(46);	// .
				GprsSendByte(((SystemSet.para.IP1[0]&0x00FF)/100)+0x30);
				GprsSendByte((((SystemSet.para.IP1[0]&0x00FF)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.IP1[0]&0x00FF)%10)+0x30);
				GprsSendByte(46);	// .
				GprsSendByte(((SystemSet.para.IP1[1]>>8)/100)+0x30);
				GprsSendByte((((SystemSet.para.IP1[1]>>8)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.IP1[1]>>8)%10)+0x30);
				GprsSendByte(46);	// .
				GprsSendByte(((SystemSet.para.IP1[1]&0x00FF)/100)+0x30);
				GprsSendByte((((SystemSet.para.IP1[1]&0x00FF)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.IP1[1]&0x00FF)%10)+0x30);
				GprsSendByte(34);	// "
				GprsSendByte(44);	// ,
				GprsSendByte(34);	// " �˿�
				if(SystemSet.para.port1>9999)
					GprsSendByte((SystemSet.para.port1/10000)+0x30);
				if(SystemSet.para.port1>999)
					GprsSendByte((((SystemSet.para.port1)/1000)%10)+0x30);
				if(SystemSet.para.port1>99)
					GprsSendByte((((SystemSet.para.port1)/100)%10)+0x30);
				GprsSendByte((((SystemSet.para.port1)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.port1)%10)+0x30);
				GprsSendByte(34);	// "
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 17;
				KZQMeasureData.para.SelfCheck.bit.GPRS_state = 1;
				GprsDataMode = 1;  // ����͸��ģʽ
			}
			break;
		case 16:
			if(GprsTimeout == 1)
			{
				GprsSendStr(SIM900B_START);
				GprsSendByte(34);	// " IP2
			  GprsSendByte(((SystemSet.para.IP2[0]>>8)/100)+0x30);
				GprsSendByte((((SystemSet.para.IP2[0]>>8)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.IP2[0]>>8)%10)+0x30);
				GprsSendByte(46);	// .
				GprsSendByte(((SystemSet.para.IP2[0]&0x00FF)/100)+0x30);
				GprsSendByte((((SystemSet.para.IP2[0]&0x00FF)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.IP2[0]&0x00FF)%10)+0x30);
				GprsSendByte(46);	// .
				GprsSendByte(((SystemSet.para.IP2[1]>>8)/100)+0x30);
				GprsSendByte((((SystemSet.para.IP2[1]>>8)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.IP2[1]>>8)%10)+0x30);
				GprsSendByte(46);	// .
				GprsSendByte(((SystemSet.para.IP2[1]&0x00FF)/100)+0x30);
				GprsSendByte((((SystemSet.para.IP2[1]&0x00FF)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.IP2[1]&0x00FF)%10)+0x30);
				GprsSendByte(34);	// "
				GprsSendByte(44);	// ,
				GprsSendByte(34);	// " �˿�
				if(SystemSet.para.port2>9999)
					GprsSendByte((SystemSet.para.port2/10000)+0x30);
				if(SystemSet.para.port2>999)
					GprsSendByte((((SystemSet.para.port2)/1000)%10)+0x30);
				if(SystemSet.para.port2>99)
					GprsSendByte((((SystemSet.para.port2)/100)%10)+0x30);
				GprsSendByte((((SystemSet.para.port2)/10)%10)+0x30);
				GprsSendByte(((SystemSet.para.port2)%10)+0x30);
				GprsSendByte(34);	// "
				GprsSendStr(SIM900B_ENTER);
				GprsDelay = 2; 	// ��ʱ
				GprsTimeout = 0;
	 			GprsConnectState = 17;
				KZQMeasureData.para.SelfCheck.bit.GPRS_state = 1;
				GprsDataMode = 1;  // ����͸��ģʽ
			}
			break;
		case 17:
			if(GprsTimeout == 1)
			{
				GprsDelay = 1; 	// ��ʱ
				GprsTimeout = 0;
 				GprsConnectState = 17; 		
			}
			break;
		
		default:
			GprsConnectState = 0;
			break;
	}
	return 0;
}

/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolveGprsData(u8 receivedata)
*
*��    �ܣ�GPRS����ͨѶЭ�����
*
*��ڲ�������������
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolveGprsData(u8 receivedata)
{
	
	if(GprsDataMode == 1)
	{
		
		if(Measure101Para.para.res3 == 0)
	  {
			if(ProtocolResolve1(receivedata)==0x0D)					// ����������
			{
				KZQMeasureData.para.SelfCheck.bit.GPRS_state = 0;
			}
	  }
		else if(Measure101Para.para.res3 == 1)
		{
			Encryption_Handle(receivedata,1);
		}
	}
	
	return 0;
}

/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolveGprsState(u8 receivedata)
*
*��    �ܣ�GPRS״̬ͨѶЭ�����
*
*��ڲ�������������
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolveGprsState(u8 receivedata)
{
	static u8 ProtocolStatusState = 0;
	ProtocolResolveGprsError(receivedata);
	GprsWdg = 0;	// ���Ź�����
	switch(ProtocolStatusState)
	{
		case 0:
			if(receivedata == 'S')
			{
				ProtocolStatusState = 1;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 1:
			if(receivedata == 'T')
			{
				ProtocolStatusState = 2;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 2:
			if(receivedata == 'A')
			{
				ProtocolStatusState = 3;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 3:
			if(receivedata == 'T')
			{
				ProtocolStatusState = 4;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 4:
			if(receivedata == 'E')
			{
				ProtocolStatusState = 5;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 5:
			if(receivedata == ':')
			{
				ProtocolStatusState = 6;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 6:
			if(receivedata == ' ')
			{
				ProtocolStatusState = 7;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 7:
			if(receivedata == 'C')			// CONNECT OK
			{
				ProtocolStatusState = 8;
			}
			else if(receivedata == 'T')		// TCP CONNECTING or TCP CLOSED
			{
				ProtocolStatusState = 17;
			}
			else if(receivedata == 'P')		// PDP DEACT
			{
				ProtocolStatusState = 22;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 8:
			if(receivedata == 'O')
			{
				ProtocolStatusState = 9;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 9:
			if(receivedata == 'N')
			{
				ProtocolStatusState = 10;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 10:
			if(receivedata == 'N')
			{
				ProtocolStatusState = 11;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 11:
			if(receivedata == 'E')
			{
				ProtocolStatusState = 12;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 12:
			if(receivedata == 'C')
			{
				ProtocolStatusState = 13;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 13:
			if(receivedata == 'T')
			{
				ProtocolStatusState = 14;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 14:
			if(receivedata == ' ')
			{
				ProtocolStatusState = 15;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 15:
			if(receivedata == 'O')
			{
				ProtocolStatusState = 16;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 16:
			if(receivedata == 'K')			// CONNECT OK ��������
			{
				ProtocolStatusState = 0;
				KZQMeasureData.para.SelfCheck.bit.GPRS_state = 0;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 17:
			if(receivedata == 'C')
			{
				ProtocolStatusState = 18;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 18:
			if(receivedata == 'P')
			{
				ProtocolStatusState = 19;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 19:
			if(receivedata == ' ')
			{
				ProtocolStatusState = 20;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 20:
			if(receivedata == 'C')
			{
				ProtocolStatusState = 21;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 21:
			if(receivedata == 'O')		// TCP CONNECTING  ��������
			{
				ProtocolStatusState = 0;
				
			}
			else if(receivedata == 'L')	// TCP CLOSED	   ���ӹر�
			{
				ProtocolStatusState = 0;
				GprsConnectState = 0;	// ��������
				KZQMeasureData.para.SelfCheck.bit.GPRS_state = 1;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 22:
			if(receivedata == 'D')
			{
				ProtocolStatusState = 23;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 23:
			if(receivedata == 'P')
			{
				ProtocolStatusState = 24;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 24:
			if(receivedata == ' ')
			{
				ProtocolStatusState = 25;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 25:
			if(receivedata == 'D')
			{
				ProtocolStatusState = 26;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 26:
			if(receivedata == 'E')
			{
				ProtocolStatusState = 27;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 27:
			if(receivedata == 'A')
			{
				ProtocolStatusState = 28;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 28:
			if(receivedata == 'C')
			{
				ProtocolStatusState = 29;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		case 29:
			if(receivedata == 'T')		   // PDP DEACT
			{
				ProtocolStatusState = 0;
				GprsConnectState = 0;	// ģ�鸴λ����
				KZQMeasureData.para.SelfCheck.bit.GPRS_state = 1;
			}
			else
			{
				ProtocolStatusState = 0;
			}
			break;
		default:	
			ProtocolStatusState = 0;
			break;
	}
	return 0;
}

/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolveGprsError(u8 receivedata)
*
*��    �ܣ�GPRSͨѶ����Э�����
*
*��ڲ�������������
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolveGprsError(u8 receivedata)
{
	static u8 ProtocolErrorState = 0;
	switch(ProtocolErrorState)
	{
		case 0:
			if(receivedata == 'E')
			{
				ProtocolErrorState = 1;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 1:
			if(receivedata == 'R')
			{
				ProtocolErrorState = 2;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 2:
			if(receivedata == 'R')
			{
				ProtocolErrorState = 3;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 3:
			if(receivedata == 'O')
			{
				ProtocolErrorState = 4;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 4:
			if(receivedata == 'R')
			{
				ProtocolErrorState = 0;
				GprsConnectState = 0;	// ģ�鸴λ����
				KZQMeasureData.para.SelfCheck.bit.GPRS_state = 1;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		default:
			ProtocolErrorState = 0;
			break;

	}
	return 0;
}
#endif

void InsertTeam(u8 USARTxChannel,u8 *sum,u16 zhi)
{
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],zhi);
	*sum+=zhi;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],zhi>>8);
	*sum+=(zhi>>8);
}
void InsertByte(u8 USARTxChannel,u8 *sum,u8 zhi)
{
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],zhi);
	*sum+=zhi;
}

void IEC101_Staid(u8 linkcontrol,u8 USARTxChannel)//�̶�֡	��ַ��1���ֽ�
{
	u8 sum = 0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x10);
	InsertByte(USARTxChannel,&sum,linkcontrol);
	if(Addr101==2)
	InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void IEC101_Staid_All(u8 USARTxChannel,u8 Reason)//���ٻ�ȷ��	��ַ��1���ֽ�
{
	u8 sum = 0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	{
		if(Reason==7)
		InsertByte(USARTxChannel,&sum,0x28);//ȷ��
		else InsertByte(USARTxChannel,&sum,0x08);//���ٻ�����
  }
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,100);//���ͱ�ʶ100�����ٻ�
	
	InsertByte(USARTxChannel,&sum,0x81);  //��Ϣ����Ŀ
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,Reason);//<7>:=����ȷ�� a-����
  else 
		InsertByte(USARTxChannel,&sum,Reason);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	InsertByte(USARTxChannel,&sum,20); //QOI=<20> :=���ٻ�
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Test_101(u8 USARTxChannel)
{
	u8 sum = 0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],10+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],10+TotalLen68);
  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,104);//���ͱ�ʶ104��������·
	
	InsertByte(USARTxChannel,&sum,0x81);  //��Ϣ����Ŀ
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,7);//7<7>:=����ȷ�� a-����
  else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	
	InsertTeam(USARTxChannel,&sum,0);
	InsertTeam(USARTxChannel,&sum,TestAddr);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void IEC101_Staid_Enegy(u8 number,u8 USARTxChannel,u8 Reason)//������ٻ��������������ȷ��֡	��ַ��1���ֽ�
{
	u8 sum = 0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,101);//���ͱ�ʶ101��������ٻ��������������
	InsertByte(USARTxChannel,&sum,0x81);  //��Ϣ����Ŀ
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,Reason);//<7>:=����ȷ�� a-����
  else 
		InsertByte(USARTxChannel,&sum,Reason);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
	InsertByte(USARTxChannel,&sum,number); //QCC=<45> :=������ܣ�QCC=<5> :=�ٻ�����

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void InsertFloat(u8 USARTxChannel,u8 *sum,s32 zhi,u16 div)//С����
{
	union FloatToChar Floatm;
	u8 i;
	Floatm.f=(float)zhi/div;
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,sum,Floatm.byte[i]);
}
void IntToFloat(u8 m,s32 value,u16 div)//С����
{	
	union FloatToChar Floatm;
	ycls1[m] = value;
	Floatm.f=(float)value/div;
	ycls[4*m]=Floatm.byte[0];
	ycls[4*m+1]=Floatm.byte[1];
	ycls[4*m+2]=Floatm.byte[2];
	ycls[4*m+3]=Floatm.byte[3];
}

void IEC101_Enegy_Send(u8 USARTxChannel)//�����������������	��ַ��1���ֽ�
{
	u8 sum = 0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);//5*num+8
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);
  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,206);//���ͱ�ʶ206 ���� ������
//	InsertByte(USARTxChannel,&sum,15);//���ͱ�ʶ��15����=�������������
	InsertByte(USARTxChannel,&sum,0x88);  //��Ϣ����Ŀ
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,37);  //��Ӧ���������ٻ�
  else 
		InsertByte(USARTxChannel,&sum,37);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0x6401);
	
	InsertFloat(USARTxChannel,&sum,MeasureData.Wp_all,1);
  InsertByte(USARTxChannel,&sum,0);
	InsertFloat(USARTxChannel,&sum,MeasureData.Wq_1all+MeasureData.Wq_2all,1);
	InsertByte(USARTxChannel,&sum,0);
	InsertFloat(USARTxChannel,&sum,MeasureData.Wq_1all,1);
	InsertByte(USARTxChannel,&sum,0);
	InsertFloat(USARTxChannel,&sum,MeasureData.Wq_4all,1);
	InsertByte(USARTxChannel,&sum,0);
	
	InsertFloat(USARTxChannel,&sum,MeasureData.WpFX_all,1);
  InsertByte(USARTxChannel,&sum,0);
	InsertFloat(USARTxChannel,&sum,MeasureData.Wq_3all+MeasureData.Wq_4all,1);
	InsertByte(USARTxChannel,&sum,0);
	InsertFloat(USARTxChannel,&sum,MeasureData.Wq_2all,1);
	InsertByte(USARTxChannel,&sum,0);
	InsertFloat(USARTxChannel,&sum,MeasureData.Wq_3all,1);
	InsertByte(USARTxChannel,&sum,0);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void SetParameter(u8 USARTxChannel,u8 num)
{
	u8 i,sum = 0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],2*num+8+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],2*num+8+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,48);//���ͱ�ʶ48���趨����
	InsertByte(USARTxChannel,&sum,num|0x80);
	
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,0x7);//
  else 
		InsertByte(USARTxChannel,&sum,0x7);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	if(num==((sizeof(struct SYSTEM_PARA))/2))
	{
		InsertTeam(USARTxChannel,&sum,0x4801);  //��Ϣ���ַ
		for(i=0;i<((sizeof(struct SYSTEM_PARA))/2);i++)
	  {
			InsertTeam(USARTxChannel,&sum,SystemSet.word[i]);	    
		}
	}
	else if(num==((sizeof(struct PROTECT_PARA))/2))
	{
		InsertTeam(USARTxChannel,&sum,0x4802);  //��Ϣ���ַ
		for(i=0;i<((sizeof(struct PROTECT_PARA))/2);i++)
	  {
			InsertTeam(USARTxChannel,&sum,ProtectSet.word[i]);	    
		}		
	}
	else if(num==((sizeof(struct ALARM_PARA))/2))
	{
		InsertTeam(USARTxChannel,&sum,0x4803);  //��Ϣ���ַ
		for(i=0;i<((sizeof(struct ALARM_PARA))/2);i++)
	  {
			InsertTeam(USARTxChannel,&sum,AlarmSet.word[i]);	    
		}	
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void ReadParameter(u8 USARTxChannel,u8 num)
{
	u8 i,sum = 0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],2*num+8+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],2*num+8+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,102);//���ͱ�ʶ102������������
	InsertByte(USARTxChannel,&sum,num|0x80);
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,0x7);//
  else 
		InsertByte(USARTxChannel,&sum,0x7);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);

	if(num==((sizeof(struct SYSTEM_PARA))/2))
	{
		InsertTeam(USARTxChannel,&sum,0x4801);  //��Ϣ���ַ
		for(i=0;i<((sizeof(struct SYSTEM_PARA))/2);i++)
	  {
			InsertTeam(USARTxChannel,&sum,SystemSet.word[i]);	    
		}
	}
	else if(num==((sizeof(struct PROTECT_PARA))/2))
	{
		InsertTeam(USARTxChannel,&sum,0x4802);  //��Ϣ���ַ
		for(i=0;i<((sizeof(struct PROTECT_PARA))/2);i++)
	  {
			InsertTeam(USARTxChannel,&sum,ProtectSet.word[i]);	    
		}		
	}
	else if(num==((sizeof(struct ALARM_PARA))/2))
	{
		InsertTeam(USARTxChannel,&sum,0x4803);  //��Ϣ���ַ
		for(i=0;i<((sizeof(struct ALARM_PARA))/2);i++)
	  {
			InsertTeam(USARTxChannel,&sum,AlarmSet.word[i]);	    
		}	
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Time_Synchronization(u8 USARTxChannel,u8 reason)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],15+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],15+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
  else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,103);//���ͱ�ʶ103��ʱ��ͬ������
	InsertByte(USARTxChannel,&sum,1);
	
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,reason);// 5��ȡ 6-���� 7��Ӧ
  else 
		InsertByte(USARTxChannel,&sum,reason);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	
	InsertTeam(USARTxChannel,&sum,0);
	
	InsertTeam(USARTxChannel,&sum,(u16)(TimeNow.second*1000));
  InsertByte(USARTxChannel,&sum,TimeNow.minute);
	InsertByte(USARTxChannel,&sum,TimeNow.hour);
	InsertByte(USARTxChannel,&sum,TimeNow.date);
	InsertByte(USARTxChannel,&sum,TimeNow.month);
	InsertByte(USARTxChannel,&sum,TimeNow.year);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void History_Data(u8 USARTxChannel)
{
  u8 bufread[120] = {0};//���ͻ���
	u8 i,j,temp1,temp2,sum=0;
  CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_SOERecord_ADDR,2);
  if((bufread[0] | (bufread[1]<<8))!=0xFFFF)
	{
		if((bufread[0] | (bufread[1]<<8))>300)
		{
			for(i=0;i<30;i++)
			{		
        sum=0;				
			  SPI_Flash_Read(bufread,FLASH_SOERecord_ADDR+i*120,120);
				
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],128+TotalLen68);//L=128
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],128+TotalLen68);
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
				if(balance == 1)
				  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
			  else 
				  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
				if(Addr101==2)
					InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
				else 
					InsertByte(USARTxChannel,&sum,SystemSet.para.address);
				InsertByte(USARTxChannel,&sum,107);//
				InsertByte(USARTxChannel,&sum,0xF8);
				
				if(Reason101==2)
					InsertTeam(USARTxChannel,&sum,0x7);//
				else 
					InsertByte(USARTxChannel,&sum,0x7);
				if(Addr101==2)
					InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
				else 
					InsertByte(USARTxChannel,&sum,SystemSet.para.address);
				InsertTeam(USARTxChannel,&sum,0);
	      for(j=0;j<120;j++)
				{	
					InsertByte(USARTxChannel,&sum,bufread[j]);
				}
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	      InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
				
				os_dly_wait (10);
			}
		}
		else
		{
			temp1 = (DLQ_SOERecord_Index+1)/10;
			temp2 = (DLQ_SOERecord_Index+1)%10;
			for(i=0;i<temp1;i++)
			{
				sum=0;				
			  SPI_Flash_Read(bufread,FLASH_SOERecord_ADDR+i*120,120);
				
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],128+TotalLen68);//L=128
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],128+TotalLen68);
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
				if(balance == 1)
				  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
				else 
				  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
				if(Addr101==2)
					InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
				else 
					InsertByte(USARTxChannel,&sum,SystemSet.para.address);
				InsertByte(USARTxChannel,&sum,107);//
				InsertByte(USARTxChannel,&sum,0xF8);
				
				if(Reason101==2)
					InsertTeam(USARTxChannel,&sum,0x7);//
				else 
					InsertByte(USARTxChannel,&sum,0x7);
				if(Addr101==2)
					InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
				else 
					InsertByte(USARTxChannel,&sum,SystemSet.para.address);
				InsertTeam(USARTxChannel,&sum,0);
	      for(j=0;j<120;j++)
				{	
					InsertByte(USARTxChannel,&sum,bufread[j]);
				}
				InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	      InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
				
				os_dly_wait (10);
			}
			
			sum=0;				
			SPI_Flash_Read(bufread,FLASH_SOERecord_ADDR+temp1*120,temp2*FLASH_SOERecord_LENGTH);
			
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],temp2*FLASH_SOERecord_LENGTH+8+TotalLen68);
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],temp2*FLASH_SOERecord_LENGTH+8+TotalLen68);
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
			if(balance == 1)
			  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
		  else 
			  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
			if(Addr101==2)
				InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
			else 
				InsertByte(USARTxChannel,&sum,SystemSet.para.address);
			InsertByte(USARTxChannel,&sum,107);//
			InsertByte(USARTxChannel,&sum,0xF8);
			
			if(Reason101==2)
				InsertTeam(USARTxChannel,&sum,0x7);//
			else 
				InsertByte(USARTxChannel,&sum,0x7);
			if(Addr101==2)
				InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
			else 
				InsertByte(USARTxChannel,&sum,SystemSet.para.address);
			InsertTeam(USARTxChannel,&sum,0);
			for(j=0;j<temp2*FLASH_SOERecord_LENGTH;j++)
			{	
				InsertByte(USARTxChannel,&sum,bufread[j]);
			}
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
		}
	}
	else
	{
    IEC101_Staid(0x89,USARTxChannel); 
	}
}

void Read_RealData(u8 USARTxChannel)
{
	u8 sum=0,i=0;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
//	if(DataFloat == 1)
//	{
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],123+TotalLen68);//23*5+8
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],123+TotalLen68);
//	}
//	else
//	{
//	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],54+TotalLen68);//23*2+8
//	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],54+TotalLen68);
//	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	//if(DataFloat == 1)
	  InsertByte(USARTxChannel,&sum,13);      //���ͱ�ʶ��9����=����ֵ���̸����� 13 
  //else
  //  InsertByte(USARTxChannel,&sum,0x15);    //���ͱ�ʶ��9����=����ֵ 21 	
	
	InsertByte(USARTxChannel,&sum,0x97);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)0x80|����
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,0x14);//
	else 
		InsertByte(USARTxChannel,&sum,0x14);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0x4001);//��Ϣ��ַ

	IntToFloat(Measure101Para.para.AC_power,0,1);
	IntToFloat(Measure101Para.para.Bat_volt,MeasureData.V_BAT,100);
	IntToFloat(Measure101Para.para.UAB,MeasureData.Display_UAB_val,1000);
	IntToFloat(Measure101Para.para.UCB,MeasureData.Display_UBC_val,1000);
	
//	IntToFloat(Measure101Para.para.UAB,9999,1000);
//	IntToFloat(Measure101Para.para.UCB,10008,1000);
	
	IntToFloat(Measure101Para.para.U0,MeasureData.Display_U0_val,1000);
	IntToFloat(Measure101Para.para.Sig_dBm,0,1);
	IntToFloat(Measure101Para.para.freq,MeasureData.freq,100);
	IntToFloat(Measure101Para.para.res7,0,1);
	IntToFloat(Measure101Para.para.res8,0,1);
	IntToFloat(Measure101Para.para.IA,MeasureData.Display_IA_val*3,25);
	IntToFloat(Measure101Para.para.IB,MeasureData.Display_IB_val*3,25);
	IntToFloat(Measure101Para.para.IC,MeasureData.Display_IC_val*3,25);
	
//	IntToFloat(Measure101Para.para.IA,15001,25);
//	IntToFloat(Measure101Para.para.IB,14998,25);
//	IntToFloat(Measure101Para.para.IC,15006,25);
	
	if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
		IntToFloat(Measure101Para.para.I0,MeasureData.Display_I0_val,1000);
	else
		IntToFloat(Measure101Para.para.I0,MeasureData.Protect_10I0_val,100);
	
	IntToFloat(Measure101Para.para.P,(s32)MeasureData.P_val*12,1);
	IntToFloat(Measure101Para.para.Q,MeasureData.Q_val*12,1);
	IntToFloat(Measure101Para.para.cos,MeasureData.cosALL,1000);
	IntToFloat(Measure101Para.para.res9,0,1);
	IntToFloat(Measure101Para.para.res10,0,1);
	IntToFloat(Measure101Para.para.res11,0,1);

	for(i=0;i<23;i++)
	{
		InsertByte(USARTxChannel,&sum,ycls[4*i]);
		InsertByte(USARTxChannel,&sum,ycls[4*i+1]);
		InsertByte(USARTxChannel,&sum,ycls[4*i+2]);
		InsertByte(USARTxChannel,&sum,ycls[4*i+3]);
		InsertByte(USARTxChannel,&sum,0);
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
} 

void Read_RealData_Old(u8 USARTxChannel)
{
	u8 i,sum=0;
	u8 j = 13;
	u32 longitude = 0;
	u32 latitude = 0;
	
	longitude = Gps.longitude*100000;
	latitude = Gps.latitude*100000;
	
	KZQMeasureData.word[j++]=MeasureData.Display_UAB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UBC_val;
  KZQMeasureData.word[j++]=MeasureData.Display_UCA_val;	
	KZQMeasureData.word[j++]=MeasureData.Display_UA_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UC_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UA1_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UB1_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UC1_val;	
	KZQMeasureData.word[j++]=MeasureData.Display_degUA_val; 
	KZQMeasureData.word[j++]=MeasureData.Display_degUB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degUC_val;
	KZQMeasureData.word[j++]=MeasureData.Display_U0_val;
	if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
	  KZQMeasureData.word[j++]=MeasureData.Display_I0_val/10;
	else 
		KZQMeasureData.word[j++]=MeasureData.Protect_10I0_val;
  KZQMeasureData.word[j++]=MeasureData.freq;
	KZQMeasureData.word[j++]=MeasureData.Display_degU0_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degI0_val;
	KZQMeasureData.word[j++]=MeasureData.Display_IA_val;
	KZQMeasureData.word[j++]=MeasureData.Display_IB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_IC_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degIA_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degIB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degIC_val;	
	KZQMeasureData.word[j++]=(u16)MeasureData.P_val;
	KZQMeasureData.word[j++]=MeasureData.Q_val;
	KZQMeasureData.word[j++]=MeasureData.cosALL;	
	KZQMeasureData.word[j++]=MeasureData.Wp;
	KZQMeasureData.word[j++]=MeasureData.Wq;
	KZQMeasureData.word[j++]=MeasureData.cos_W;	
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[0];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[1];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[2];
  KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[3];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[4];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[5];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[6];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[7];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[8];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[0];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[1];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[2];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[3];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[4];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[5];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[6];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[7];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[8];	
	KZQMeasureData.word[j++]=MeasureData.Protect_UAB_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_UBC_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_UCA_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degUAB_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IA_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IB_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IC_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IA10_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IB10_val;	
	KZQMeasureData.word[j++]=MeasureData.Protect_IC10_val;	
	KZQMeasureData.word[j++]=zxdjdfs;
	KZQMeasureData.word[j++]=jdxb;
	KZQMeasureData.word[j++]=dycfzc;
	KZQMeasureData.word[j++]=jdxz;
	KZQMeasureData.word[j++]=jdyf;	
	KZQMeasureData.word[j++]=hgxb;
	KZQMeasureData.word[j++]=hggdy;
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_1all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_1all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_peak&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_peak>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_2all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_2all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_valley&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_valley>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_3all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_3all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_level&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_level>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_4all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_4all>>16);	
	KZQMeasureData.word[j++]=MeasureData.UA_val;
	KZQMeasureData.word[j++]=MeasureData.UB_val;
	KZQMeasureData.word[j++]=MeasureData.UC_val;
	KZQMeasureData.word[j++]=MeasureData.IA_val;
	KZQMeasureData.word[j++]=MeasureData.IB_val;
	KZQMeasureData.word[j++]=MeasureData.IC_val;
	KZQMeasureData.word[j++]=MeasureData.cosA;
	KZQMeasureData.word[j++]=MeasureData.cosB;
	KZQMeasureData.word[j++]=MeasureData.cosC;	
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_peak&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_peak>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_valley&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_valley>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_level&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_level>>16);
	KZQMeasureData.word[j++]= TimeNow.year;
	KZQMeasureData.word[j++]= TimeNow.month;
	KZQMeasureData.word[j++]= TimeNow.date;
	KZQMeasureData.word[j++]= TimeNow.hour;
	KZQMeasureData.word[j++]= TimeNow.minute;
	KZQMeasureData.word[j++]= TimeNow.second;
	KZQMeasureData.word[j]= MeasureData.V_BAT;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],226);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],226);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(SOE_Flag!=0  || SendLock_Cnt == 0xFF || TotalSign!=0)
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	else
		InsertByte(USARTxChannel,&sum,0x08);//ȷ��
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],SystemSet.para.address);
	sum+=SystemSet.para.address;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9);       //���ͱ�ʶ��9����=����ֵ
	sum+=9;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0xF4);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	sum+=0xF4;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);    //����ԭ�򣺼���ȷ��
	sum+=0x07;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],SystemSet.para.address);
	sum+=SystemSet.para.address;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x01);    //��ϢԪ�ص�ַ��2byte�� 
	sum+=0x01;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);    //<7>:=����ȷ��
	sum+=0x07;
	for(i=13;i<(sizeof(struct KZQMEASURE)/2);i++)
	{
	    InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],KZQMeasureData.word[i]);
	    sum+=KZQMeasureData.word[i];
	    InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],KZQMeasureData.word[i]>>8);
	    sum+=KZQMeasureData.word[i]>>8;
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],longitude&0xFF);
	sum+=longitude&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],longitude>>8);
	sum+=longitude>>8;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],longitude>>16);
	sum+=longitude>>16;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],longitude>>24);
	sum+=longitude>>24;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Gps.NS & 0xFF);
	sum+=Gps.NS&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],latitude&0xFF);
	sum+=latitude&0xFF;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],latitude>>8);
	sum+=latitude>>8;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],latitude>>16);
	sum+=latitude>>16;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],latitude>>24);
	sum+=latitude>>24;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Gps.EW&0xFF);
	sum+=Gps.EW&0xFF;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
} 

void Read_YX(u8 USARTxChannel)//���ٻ�ң��
{
	u8 buffer[40] = {0};
	u8 sum=0,i=0;
	
	buffer[Measure101Para.para.far_positoin] = FAR_IN6^0x01;
	buffer[Measure101Para.para.local_positoin] = LOCAL_IN7^0x01;//�͵�
	if(DYXH1 == 1 && DYXH2 == 1)	
	{
	  buffer[Measure101Para.para.AC_loss] = 0x01;               //����ʧ��澯
	}
	else
	  buffer[Measure101Para.para.AC_loss] = 0;
	buffer[Measure101Para.para.Bat_low] = KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage;//���Ƿѹ�澯
	buffer[Measure101Para.para.low_airlock] = 0;
	buffer[Measure101Para.para.lock_on] = Switchon_Lock | Switchon_Lock1 | Switchon_Lock2; //������բ
	buffer[Measure101Para.para.lock_off] = Switchoff_Lock | Switchoff_Lock1;                //������բ  
	buffer[Measure101Para.para.break_on] = HW_IN2^0x01;//��λ
	buffer[Measure101Para.para.break_off] = FW_IN1^0x01;//��λ
	buffer[Measure101Para.para.wcn_state] = WCN_IN3^0x01;//δ����
	buffer[Measure101Para.para.max_current1_alarm] = 0;//10
	buffer[Measure101Para.para.max_current2_alarm] = 0;
	buffer[Measure101Para.para.max_load_alarm] = KZQMeasureData.para.AlarmFlag.bit.max_load;//�����ɸ澯
	buffer[Measure101Para.para.zero_current1_alarm] = KZQMeasureData.para.AlarmFlag.bit.zero_max_current;
	buffer[Measure101Para.para.ground_current_alarm] = KZQMeasureData.para.AlarmFlag.bit.ground;//����ӵع��ϸ澯
	buffer[Measure101Para.para.max_current1_protec] = KZQMeasureData.para.ProtectFlag1.bit.fast_off;//����I��
	buffer[Measure101Para.para.max_current2_protec] = KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off;//����II��
	buffer[Measure101Para.para.zero_current1_protec] = KZQMeasureData.para.ProtectFlag1.bit.zero_max_current;//�����������
	buffer[Measure101Para.para.zero_currentafter_protec] = 0;
	buffer[Measure101Para.para.reon] = KZQMeasureData.para.ProtectFlag2.bit.reon_act; //�غ�բ
	buffer[Measure101Para.para.max_currentafter_protec] = KZQMeasureData.para.ProtectFlag1.bit.max_current_after;//��������ٱ���//20
	buffer[Measure101Para.para.max_voltage] = KZQMeasureData.para.ProtectFlag2.bit.max_voltage;
	buffer[Measure101Para.para.high_freq] = KZQMeasureData.para.ProtectFlag2.bit.max_freq;
	buffer[Measure101Para.para.low_freq] = KZQMeasureData.para.ProtectFlag2.bit.low_freq;
	if(ProtectFlag==1 || (KZQMeasureData.para.AlarmFlag.word & 0x01ff))  //�¹���
	  buffer[Measure101Para.para.event_all] = 0x01;
	else
		 buffer[Measure101Para.para.event_all] = 0;
	buffer[Measure101Para.para.max_current3_protec] = KZQMeasureData.para.ProtectFlag1.bit.max_current; //����III��
	if(KZQMeasureData.para.SelfCheck.word & 0x3f0f)
	{
	  buffer[Measure101Para.para.break_err] = 0x01;                                           //װ���쳣�澯
	}
	else
	  buffer[Measure101Para.para.break_err] = 0; 
	
	 if(AutoswitchSet.para.segment_contact_mode == 0x01)	                  //�ֶε㹤��ģʽ
	{
	  buffer[Measure101Para.para.segment_mode] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.segment_mode] = 0;                              
  if(AutoswitchSet.para.segment_contact_mode == 0x02)	                  //����㹤��ģʽ
	{
	  buffer[Measure101Para.para.contact_mode] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.contact_mode] = 0;                           
	if(ProtectTimeout.para.novoltage_value_A == 1)	                      //��Դ����ѹ
	{
	  buffer[Measure101Para.para.novoltage_A] = 0x01;       
	}
	else
	  buffer[Measure101Para.para.novoltage_A] = 0;    
  if(ProtectTimeout.para.novoltage_value_B == 1)	                      //���ز���ѹ
	{
	  buffer[Measure101Para.para.novoltage_B] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.novoltage_B] = 0;    
	if(KZQMeasureData.para.ProtectFlag1.bit.loss_power)	                  //ʧѹ��բ
	{
	  buffer[Measure101Para.para.powerloss_off] = 0x01;    
	}
	else
	  buffer[Measure101Para.para.powerloss_off] = 0;    
	buffer[Measure101Para.para.bat_active] = KZQMeasureData.para.AlarmFlag.bit.bat_active;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);//40+8=48
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,0x01);      //���ͱ�ʶ��1����=����ң��0/1 =3˫��ң�� 1/2
	InsertByte(USARTxChannel,&sum,0xA8);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)0x80|���� 40
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,0x14);//
	else 
		InsertByte(USARTxChannel,&sum,0x14);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	
	InsertTeam(USARTxChannel,&sum,0x0001);//��Ϣ��ַ
  for(i=0;i<40;i++)
	{
		InsertByte(USARTxChannel,&sum,buffer[i]); 
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Remote_Control(u8 USARTxChannel,u8 reason,u8 TI,u16 controladdr)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	{
		if(control.bit.S_E==1)
			InsertByte(USARTxChannel,&sum,0x08);//ȷ�ϣ�ң��ѡ����Ҫ��һ������
		else
	    InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	}

	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
  InsertByte(USARTxChannel,&sum,TI);//���ͱ�ʶ��46����=˫��ң������ 45����
	InsertByte(USARTxChannel,&sum,0x81);//�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,reason);//
	else 
		InsertByte(USARTxChannel,&sum,reason);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	
	InsertTeam(USARTxChannel,&sum,controladdr);
	InsertByte(USARTxChannel,&sum,control.byte);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void HCOS(u8 USARTxChannel,u8 p[][10],u8 num)  //��ʱ���
{
	u8 sum=0,i,j;
	u16 dqms;
	control_field.bit.FCB ^= 0x01;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],10*num+6+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],10*num+6+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
  InsertByte(USARTxChannel,&sum,30);//���ͱ�ʶ��46����=˫��ң������ 45����
	InsertByte(USARTxChannel,&sum,num);//�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,3);//
	else 
		InsertByte(USARTxChannel,&sum,3);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	for(i=0;i<num;i++)
	{
		InsertTeam(USARTxChannel,&sum,p[i][8]+1);          //ң�ŵ�ַ
		InsertByte(USARTxChannel,&sum,p[i][9]);
		dqms=(p[i][5]*1000+(p[i][6]<<8)+p[i][7])%60000;  //��
		InsertTeam(USARTxChannel,&sum,dqms);
		for(j=0;j<5;j++)
			InsertByte(USARTxChannel,&sum,p[i][4-j]);
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);	
}

void HSOE(u8 USARTxChannel,u8 p[][10],u8 num) //����ʱ���
{
	u8 sum=0,i;
	control_field.bit.FCB ^= 0x01;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],3*num+6+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],3*num+6+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	
  InsertByte(USARTxChannel,&sum,1);//������Ϣ
	
	InsertByte(USARTxChannel,&sum,num);//�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,3);//
	else 
		InsertByte(USARTxChannel,&sum,3);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	for(i=0;i<num;i++)
	{
		InsertTeam(USARTxChannel,&sum,p[i][8]+1);
		InsertByte(USARTxChannel,&sum,p[i][9]);
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);	
}

void Record_SOE_Send(u8 USARTxChannel)
{
	u8 soedat[10][10],i,j;
	u8 soenum,soedatanum;
	soedatanum = GetQueueDataNum(&QueueSOE);
	if((soedatanum%10)==0 && soedatanum > 9)
	{
		soenum = soedatanum/10;
		if(soedatanum<100)
		{
			for(j=0;j<soenum;j++)
			{
				for(i=0;i<10;i++)
					soedat[j][i] = GetDataFromQueue(&QueueSOE,BufferSOE);
			}
		}
		else
		{
			for(j=0;j<10;j++)
			{
				for(i=0;i<10;i++)
					soedat[j][i] = GetDataFromQueue(&QueueSOE,BufferSOE);
			}
		}
		HCOS(USARTxChannel,soedat,soenum);
		os_dly_wait (30);
		HSOE(USARTxChannel,soedat,soenum);
	}
	else if(soedatanum>0)
	{
		for(i=0;i<(soedatanum%10);i++)
		  GetDataFromQueue(&QueueSOE,BufferSOE);					
	}
	soedatanum = GetQueueDataNum(&QueueSOE);
	if(soedatanum == 0)
	  SOE_Flag = 0;
}

void Message_Send(u8 USARTxChannel)
{
	u8 sum=0,i=0;
	os_dly_wait (100);
	if(KZQMeasureData.para.ProtectFlag2.bit.protect_flag)
	{
		KZQMeasureData.para.ProtectFlag2.bit.protect_flag = 0;
		
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],67);
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],67);
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
		
		InsertByte(USARTxChannel,&sum,0x81);
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		InsertByte(USARTxChannel,&sum,22);
		InsertByte(USARTxChannel,&sum,59);
		InsertByte(USARTxChannel,&sum,0x03);
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		InsertByte(USARTxChannel,&sum,0);
		InsertByte(USARTxChannel,&sum,0);
		InsertByte(USARTxChannel,&sum,3);
		
		if((ProtectRecord_flag&0x0003) == 1)
		{
			for(i=0;i<(sizeof(struct RECORDpara)/2);i++)
			{
				InsertTeam(USARTxChannel,&sum,Record_protect1.word[i]);
			}				
		}
		else if((ProtectRecord_flag&0x0003) == 2)
		{
			for(i=0;i<(sizeof(struct RECORDpara)/2);i++)
			{
				InsertTeam(USARTxChannel,&sum,Record_protect2.word[i]);
			}	
		}
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	}
	else if(KZQMeasureData.para.RequestFlag1.bit.record_alarm)
	{
		KZQMeasureData.para.RequestFlag1.bit.record_alarm = 0;
		
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],67);
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],67);
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
		
		InsertByte(USARTxChannel,&sum,0x81);
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		InsertByte(USARTxChannel,&sum,22);
		InsertByte(USARTxChannel,&sum,59);
		InsertByte(USARTxChannel,&sum,0x03);
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		InsertByte(USARTxChannel,&sum,0);
		InsertByte(USARTxChannel,&sum,0);
		InsertByte(USARTxChannel,&sum,4);
		
		for(i=0;i<(sizeof(struct RECORDpara)/2);i++)
		{
			InsertTeam(USARTxChannel,&sum,Record_alarm.word[i]);
		}
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	}
}

//��λң�� ÿ�������
#define dypd 2000 //10000*0.2 ���� ���3V�ϴ�
#define pqpd 3600//100*5*3*0.2*12
#define ipd  3000//5000*0.2*3
#define pfpd 200//1000*0.2
#define i0pd 4000//20000*0.2
static u8 lsyc[170]; //7*23����ʱ���� 161

u8 dp2sj(u8 address,s32 ycdat2,u16 ycpd,u16 div,u8 ycnum)
{
	union FloatToChar Floatm;

	if(abs(ycls1[address]-ycdat2)>=ycpd)
	{
		ycls1[address]=ycdat2;
		
		Floatm.f=(float)ycdat2/div;
		lsyc[ycnum*7]=address+1;
		lsyc[ycnum*7+1]=0x40;
		lsyc[ycnum*7+2]=Floatm.byte[0];
		lsyc[ycnum*7+3]=Floatm.byte[1];
		lsyc[ycnum*7+4]=Floatm.byte[2];
		lsyc[ycnum*7+5]=Floatm.byte[3];
		lsyc[ycnum*7+6]=0;
		ycnum++;
	}
	return ycnum;
}
u8 ZH2SJ(void)
{
	u8 ycnum=0;
	ycnum=dp2sj(Measure101Para.para.AC_power,0,dypd,1,ycnum);
	ycnum=dp2sj(Measure101Para.para.Bat_volt,MeasureData.V_BAT,100,100,ycnum);
	ycnum=dp2sj(Measure101Para.para.UAB,MeasureData.Display_UAB_val,dypd,1000,ycnum);
	ycnum=dp2sj(Measure101Para.para.UCB,MeasureData.Display_UBC_val,dypd,1000,ycnum);
	ycnum=dp2sj(Measure101Para.para.U0,MeasureData.Display_U0_val,dypd,1000,ycnum);
	ycnum=dp2sj(Measure101Para.para.Sig_dBm,0,dypd,1,ycnum);
	ycnum=dp2sj(Measure101Para.para.freq,MeasureData.freq,100,100,ycnum);
	ycnum=dp2sj(Measure101Para.para.res7,0,dypd,1,ycnum);
	ycnum=dp2sj(Measure101Para.para.res8,0,dypd,1,ycnum);
	ycnum=dp2sj(Measure101Para.para.IA,MeasureData.Display_IA_val*3,ipd,25,ycnum);
	ycnum=dp2sj(Measure101Para.para.IB,MeasureData.Display_IB_val*3,ipd,25,ycnum);
	ycnum=dp2sj(Measure101Para.para.IC,MeasureData.Display_IC_val*3,ipd,25,ycnum);
	
	if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
		ycnum=dp2sj(Measure101Para.para.I0,MeasureData.Display_I0_val,i0pd,1000,ycnum);
	else
		ycnum=dp2sj(Measure101Para.para.I0,MeasureData.Protect_10I0_val,400,100,ycnum);
	
	ycnum=dp2sj(Measure101Para.para.P,(s32)MeasureData.P_val*12,pqpd,1,ycnum);
	ycnum=dp2sj(Measure101Para.para.Q,MeasureData.Q_val*12,pqpd,1,ycnum);
	ycnum=dp2sj(Measure101Para.para.cos,MeasureData.cosALL,pfpd,1000,ycnum);
	ycnum=dp2sj(Measure101Para.para.res9,0,dypd,1,ycnum);
	ycnum=dp2sj(Measure101Para.para.res10,0,dypd,1,ycnum);
	ycnum=dp2sj(Measure101Para.para.res11,0,dypd,1,ycnum);
	return ycnum;
}	

u8 H2SJ(u8 USARTxChannel)
{
	u8 numyc=0,sum=0,i;
	numyc=ZH2SJ();
	if(numyc!=0)
	{
		control_field.bit.FCB ^= 0x01;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],7*numyc+6+TotalLen68);//23*2+8
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],7*numyc+6+TotalLen68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
		if(balance == 1)
		{
			InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	  }
		else 
		  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

		if(Addr101==2)
			InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
		else 
			InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		//if(DataFloat == 1)
		  InsertByte(USARTxChannel,&sum,13);      //���ͱ�ʶ��9����=����ֵ���̸����� 13 
		//else
		//  InsertByte(USARTxChannel,&sum,0x15);    

		InsertByte(USARTxChannel,&sum,numyc);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)0x80|����	������
		if(Reason101==2)
			InsertTeam(USARTxChannel,&sum,3);//ͻ��
		else 
			InsertByte(USARTxChannel,&sum,3);
		if(Addr101==2)
			InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
		else 
			InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		for(i=0;i<numyc*7;i++)
		{
			InsertByte(USARTxChannel,&sum,lsyc[i]);
		}
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	}
	else if(balance == 0)  //��ƽ��ģʽ
	  IEC101_Staid(0x09,USARTxChannel);           //0 1 0ACD(Ҫ�����λ) 0DFC(����������λ) (��Ϊ0����ʹ��) 1001(������9Ϊ�����ٻ�������)  
	
	return numyc;
}
//��ʼ������
void InitEnd(u8 USARTxChannel)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);//23*2+8
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,70);      //���ͱ�ʶ��70����=��ʼ������ 
	InsertByte(USARTxChannel,&sum,0x81);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)0x80|����	������
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,4);//��ʼ�����
	else 
		InsertByte(USARTxChannel,&sum,4);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);
	InsertByte(USARTxChannel,&sum,0);       //��ʼ��ԭ��COI��0����=���ص�Դ����
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
//��λ����
void ResetLianlu(u8 USARTxChannel)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);//23*2+8
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,105);      //���ͱ�ʶ��105����=��λ��������
	InsertByte(USARTxChannel,&sum,0x81);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)0x80|����	������
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,7);//����ȷ��
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);
	InsertByte(USARTxChannel,&sum,1);//�����ܸ�λ
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void RealCurve_Confirm(u8 USARTxChannel,u8 command,u8 reason)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	{
		InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
  }
	else 
	  InsertByte(USARTxChannel,&sum,0x08);//ȷ�ϣ����ٻ�һ������
  if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,command);//���ͱ�ʶ��52����=ʵʱ��������
  InsertByte(USARTxChannel,&sum,0x81);//�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	InsertByte(USARTxChannel,&sum,reason);//<7>:=����ȷ��<9>:=ֹͣ����ȷ��<10>:=�������
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);

	InsertByte(USARTxChannel,&sum,0);
	InsertByte(USARTxChannel,&sum,0);
	InsertByte(USARTxChannel,&sum,RealCurveFlag);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void RealCurve_Data(u8 USARTxChannel,u8 command)
{
	static u8 datanum = 0;
	static u8 dataframe = 0;
	u8 i,sum=0;
	u8 point_value =0;
	s16 *point=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],18+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],18+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x88);//��������Ӧ����֡
	sum+=0x88;
  if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],command);       //���ͱ�ʶ��54����=ʵʱ����
	sum+=command;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x81);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	sum+=0x81;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);    //����ԭ�򣺼���ȷ��
	sum+=0x07;
  if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x01);    //��ϢԪ�ص�ַ��2byte�� 
	sum+=0x01;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);    //���Ͳ��κ�

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);  //���͵ڼ�֡

  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.year&0xff);
	sum += TimeNow.year&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.month&0xff);
	sum += TimeNow.month&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.date&0xff);
	sum += TimeNow.date&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.hour&0xff);
	sum += TimeNow.hour&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.minute&0xff);
	sum += TimeNow.minute&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.second&0xff);
	sum += TimeNow.second&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.msec>>8);
	sum += TimeNow.msec>>8;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.msec&0xff);
	sum += TimeNow.msec&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	os_dly_wait (100);
		
	for(point_value=0;point_value<96;point_value++)
	{
		sum=0;
		if(((point_value +1) % 12) == 0)
			dataframe = 12;
		else
		  dataframe = (point_value +1) % 12;
		if(point_value == 0)
		{
			point = &PcurveUA[AD_StartPoint];
		  datanum = 1;
		}
		else if(point_value == 12)
		{
			point = &PcurveUB[AD_StartPoint];
		  datanum = 2;
		}
		else if(point_value == 24)
		{
			point = &PcurveUC[AD_StartPoint];
		  datanum = 3;
		}
		else if(point_value == 36)
		{
			point = &PcurveIA[AD_StartPoint];
		  datanum = 4;
		}
		else if(point_value == 48)
		{
			point = &PcurveIB[AD_StartPoint];
		  datanum = 5;
		}
		else if(point_value == 60)
		{
			point = &PcurveIC[AD_StartPoint];
		  datanum = 6;
		}
		else if(point_value == 72)
		{
			point = &PcurveU0[AD_StartPoint];
		  datanum = 7;
		}
		else if(point_value == 84)
		{
			point = &PcurveI0[AD_StartPoint];
		  datanum = 8;
		}
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],210+TotalLen68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],210+TotalLen68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x88);//��������Ӧ����֡
		sum+=0x88;
		if(Addr101==2)
			InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
		else 
			InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],command);       //���ͱ�ʶ��54����=ʵʱ����
		sum+=command;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x81);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
		sum+=0x81;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);    //����ԭ�򣺼���ȷ��
		sum+=0x07;
		if(Addr101==2)
			InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
		else 
			InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x01);    //��ϢԪ�ص�ַ��2byte�� 
		sum+=0x01;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
		
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],datanum);    //���Ͳ��κ�
		sum+=datanum;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],dataframe);  //���͵ڼ�֡
		sum+=dataframe;
		for(i=0;i<100;i++)
		{
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],*point&0xff);
			sum+=*point&0xff;
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],*point>>8);
			sum+=*point>>8;
			point++;
			if(point > (&PcurveUA[1199]) && point_value < 12)
				point = &PcurveUA[0];
			else if(point > (&PcurveUB[1199]) && point_value < 24 && point_value >= 12)
				point = &PcurveUB[0];
			else if(point > (&PcurveUC[1199]) && point_value < 36 && point_value >= 24)
				point = &PcurveUC[0];
			else if(point > (&PcurveIA[1199]) && point_value < 48 && point_value >= 36)
				point = &PcurveIA[0];
			else if(point > (&PcurveIB[1199]) && point_value < 60 && point_value >= 48)
				point = &PcurveIB[0];
			else if(point > (&PcurveIC[1199]) && point_value < 72 && point_value >= 60)
				point = &PcurveIC[0];
			else if(point > (&PcurveU0[1199]) && point_value < 84 && point_value >= 72)
				point = &PcurveU0[0];
			else if(point > (&PcurveI0[1199]) && point_value < 96 && point_value >= 84)
				point = &PcurveI0[0];
		}
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
		os_dly_wait (100);
	}
	RealCurveFlag = 0;
	AD_StartPoint = 0;
	MeasureData.Psaveflag = 0;
	RealCurve_Confirm(USARTxChannel,53,10);                //�������
}

void InstantCurve_Data(u8 USARTxChannel,u8 command)
{
	static u8 datanum = 0;
	static u8 dataframe = 0;
	u8 point_value =0;
	s16 *point=0;
	u8 i,sum=0;

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],18+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],18+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x88);//��������Ӧ����֡
	sum+=0x88;
  if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],command);       //���ͱ�ʶ��54����=ʵʱ����
	sum+=command;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x81);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	sum+=0x81;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);    //����ԭ�򣺼���ȷ��
	sum+=0x07;
  if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x01);    //��ϢԪ�ص�ַ��2byte�� 
	sum+=0x01;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);    //���Ͳ��κ�

	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);  //���͵ڼ�֡

  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.year&0xff);
	sum += TimeNow.year&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.month&0xff);
	sum += TimeNow.month&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.date&0xff);
	sum += TimeNow.date&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.hour&0xff);
	sum += TimeNow.hour&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.minute&0xff);
	sum += TimeNow.minute&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.second&0xff);
	sum += TimeNow.second&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.msec>>8);
	sum += TimeNow.msec>>8;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],TimeNow.msec&0xff);
	sum += TimeNow.msec&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	os_dly_wait (100);
		
	for(point_value=0;point_value<96;point_value++)
	{
		sum=0; 
		if(((point_value +1) % 12) == 0)
			dataframe = 12;
		else
		  dataframe = (point_value +1) % 12;
		if(point_value == 0)
		{
			point = PcurveUA_bak;
		  datanum = 1;
		}
		else if(point_value == 12)
		{
			point = PcurveUB_bak;
		  datanum = 2;
		}
		else if(point_value == 24)
		{
			point = PcurveUC_bak;
		  datanum = 3;
		}
		else if(point_value == 36)
		{
			point = PcurveIA_bak;
		  datanum = 4;
		}
		else if(point_value == 48)
		{
			point = PcurveIB_bak;
		  datanum = 5;
		}
		else if(point_value == 60)
		{
			point = PcurveIC_bak;
		  datanum = 6;
		}
		else if(point_value == 72)
		{
			point = PcurveU0_bak;
		  datanum = 7;
		}
		else if(point_value == 84)
		{
			point = PcurveI0_bak;
		  datanum = 8;
		}
		
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],210+TotalLen68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],210+TotalLen68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x88);//��������Ӧ����֡
		sum+=0x88;
		if(Addr101==2)
			InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
		else 
			InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],command);       //���ͱ�ʶ��54����=ʵʱ����
		sum+=command;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x81);    //�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
		sum+=0x81;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x07);    //����ԭ�򣺼���ȷ��
		sum+=0x07;
		if(Addr101==2)
			InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
		else 
			InsertByte(USARTxChannel,&sum,SystemSet.para.address);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x01);    //��ϢԪ�ص�ַ��2byte�� 
		sum+=0x01;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0);
		
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],datanum);    //���Ͳ��κ�
		sum+=datanum;
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],dataframe);  //���͵ڼ�֡
		sum+=dataframe;
	
		for(i=0;i<100;i++)
		{
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],*point&0xff);
			sum+=*point&0xff;			
			InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],*point>>8);
			sum+=*point>>8;	
			point++;				
		}
		
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
		os_dly_wait (100);
	}
}

void SofeReset(void)
{
	u8 buf[12];
	buf[0] = TimeNow.year;
	buf[1] = TimeNow.month;
	buf[2] = TimeNow.date;
	buf[3] = TimeNow.hour;
	buf[4] = TimeNow.minute;
	buf[5] = TimeNow.second;
	buf[6] = GPRSUpdateLength & 0x000000FF;
	buf[7] = (GPRSUpdateLength & 0x0000FF00) >> 8;
	buf[8] = (GPRSUpdateLength & 0x00FF0000) >> 16;
	buf[9] = (GPRSUpdateLength & 0xFF000000) >> 24;
	CS2BZ=0;
	SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10); 	// �������ֽ���
	GPRSUpdateLength = 0;
	os_dly_wait (100);
	RecoverKZQBackupFlag = 1;
}

void ProgramUpdate_Confirm(u8 USARTxChannel,u8 command,u8 reason)
{
	u8 buf[10];
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x80);//ȷ��
	sum+=0x80;
  if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],command);
	sum+=command;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x81);//�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	sum+=0x81;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],reason);//<7>:=����ȷ��<9>:=ֹͣ����ȷ��<10>:=�������
	sum+=reason;
  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],VERSION_YEAR);
	sum+=VERSION_YEAR;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],VERSION_MONTH);
	sum+=VERSION_MONTH;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],VERSION_DATE);
	sum+=VERSION_DATE;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],VERSION_TIME);
	sum+=VERSION_TIME;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	Last_Number = 0;
	if(command == 130)
	{
		buf[0] = TimeNow.year;
		buf[1] = TimeNow.month;
		buf[2] = TimeNow.date;
		buf[3] = TimeNow.hour;
		buf[4] = TimeNow.minute;
		buf[5] = TimeNow.second;
		buf[6] = GPRSUpdateLength & 0x000000FF;
		buf[7] = (GPRSUpdateLength & 0x0000FF00) >> 8;
		buf[8] = (GPRSUpdateLength & 0x00FF0000) >> 16;
		buf[9] = (GPRSUpdateLength & 0xFF000000) >> 24;
		CS2BZ=0;
		SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10); 	// �������ֽ���
		GPRSUpdateLength = 0;
		os_dly_wait (100);
		RecoverKZQBackupFlag = 1;
	}
}

void ProgramUpdate_ERROR(u8 USARTxChannel,u8 command)
{
	u8 sum=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],11);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],11);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x80);//ȷ��
	sum+=0x80;
  if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],command);
	sum+=command;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x81);//�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	sum+=0x81;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],13);//<7>:=����ȷ��<9>:=ֹͣ����ȷ��<10>:=�������
	sum+=13;
  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],MyVersion.GPRS_version[0]);
	sum+=MyVersion.GPRS_version[0];
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],MyVersion.GPRS_version[1]);
	sum+=MyVersion.GPRS_version[1];
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],MyVersion.GPRS_version[2]);
	sum+=MyVersion.GPRS_version[2];
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],MyVersion.GPRS_version[3]);
	sum+=MyVersion.GPRS_version[3];
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Last_Number&0xff);
	sum+=Last_Number&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Last_Number>>8);
	sum+=Last_Number>>8;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Program_Update_Receive(u8 USARTxChannel,u16 datalenth)
{
	u8 sum=0;
	u8 buf[800];
	u16 i,number;
	number = BackstageProtocol1.DataBuffer[11] | (BackstageProtocol1.DataBuffer[12] <<8);
	if(number==1)
		GPRSUpdateLength=0;
	for(i=0;i<datalenth;i++)
	{
	  buf[i] = BackstageProtocol1.DataBuffer[13+i];
		//InsertDataToQueue(&QueueTX2,BufferTX2,buf[i]);
	}
	CS2BZ=0;
	SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+GPRSUpdateLength,datalenth);
	GPRSUpdateLength += datalenth;
	os_dly_wait (30);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],11);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],11);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x80);//ȷ��
	sum+=0x80;
  if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],129);
	sum+=129;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x81);//�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	sum+=0x81;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],13);//<7>:=����ȷ��<9>:=ֹͣ����ȷ��<10>:=�������
	sum+=13;
  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],MyVersion.GPRS_version[0]);
	sum+=MyVersion.GPRS_version[0];
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],MyVersion.GPRS_version[1]);
	sum+=MyVersion.GPRS_version[1];
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],MyVersion.GPRS_version[2]);
	sum+=MyVersion.GPRS_version[2];
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],MyVersion.GPRS_version[3]);
	sum+=MyVersion.GPRS_version[3];
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(number+1)&0xff);
	sum+=(number+1)&0xff;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],(number+1)>>8);
	sum+=(number+1)>>8;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void CommandProcess1Prepare(void)
{
	u16 i = 0;	
	for(i=0;i<RxCounter1;i++)
		BackstageProtocol1.DataBuffer[i] = BackstageProtocol1bak.DataBuffer[i];	
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
//ң��仯��ʱ���� 5min����һ�� 
//�仯ң���������ͼ�ʱ����
void TimeYcYx(u8 USARTxChannel)
{
	if(balance == 1 && InitComplete==1) //��ʼ�����
	{
		Record_SOE_Send(USARTxChannel);
		if(Soesend_cnt==0)
		{
			H2SJ(USARTxChannel);
			Soesend_cnt=Max_Time;
		}
		else Soesend_cnt--;
	}
}

void CommandProcess1(u8 USARTxChannel)//�����ʵ����ר��ΪUSARTxChannel=0׼����
{
	static u8 Read_Flag = 0;
	static u8 CTRL;          //������C
	static u16 ADDR;         //��ַ��A
	static u8 TI = 0;        //���ͱ�ʶ
	static u8 COT = 0;       //����ԭ��
	static u16 object_addr;  //��Ϣ�����ַ
	static u16 datalenth = 0;
	u8 i;

	if(datdone1==2)        //�̶�֡��
	{
	  if(Addr101==2)
		  ADDR= BackstageProtocol1.DataBuffer[2]+(BackstageProtocol1.DataBuffer[3]<<8);
		else ADDR= BackstageProtocol1.DataBuffer[2];
		CTRL = BackstageProtocol1.DataBuffer[1];
  }
	else if(datdone1==1)  //�ɱ�֡��
	{
		CTRL = BackstageProtocol1.DataBuffer[4];
	  if(Addr101==2)
		  ADDR= BackstageProtocol1.DataBuffer[5]+(BackstageProtocol1.DataBuffer[6]<<8);
		else ADDR= BackstageProtocol1.DataBuffer[5];
		TI = BackstageProtocol1.DataBuffer[6+FrontReason];
		COT =BackstageProtocol1.DataBuffer[8+FrontReason];
		
		object_addr = (u16)(BackstageProtocol1.DataBuffer[11+TotalLen68]<<8)+BackstageProtocol1.DataBuffer[10+TotalLen68];//��ǰ�ߺ���Ϣ���ַ
		
		if(TI==45 || TI==46)
		{
			control.byte = BackstageProtocol1.DataBuffer[12+TotalLen68];
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
	
	if(ADDR==SystemSet.para.address||ADDR==0xFFFF||(ADDR==0xFF&&Addr101==1))
	{
		if(balance == 1) //ƽ��ģʽ
		{		
			if(datdone1==2)
			{
				if(BackstageProtocol1.DataBuffer[1]==0x40)	      //��վ��λԶ����·
				{
					IEC101_Staid(0x80,USARTxChannel);//��λ��·ȷ��
					os_dly_wait (30);	
					IEC101_Staid(0xC9,USARTxChannel);//������·
				}
				else if(BackstageProtocol1.DataBuffer[1]==0x42)   //��վ������·���Թ���
				{
					IEC101_Staid(0x80,USARTxChannel);
				}
				else if(BackstageProtocol1.DataBuffer[1]==0x49)	  //��վ������·״̬
				{
					IEC101_Staid(0x8B,USARTxChannel);
				}
				else if(BackstageProtocol1.DataBuffer[1]==0x0B)	  //��վ��Ӧ��·״̬
				{
					IEC101_Staid(0xC0,USARTxChannel);//��λ��վ��·
					InitSign=1;
				}
				else if(BackstageProtocol1.DataBuffer[1]==0x00)	  //��վ��Ӧȷ������
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
							Read_RealData_Old(USARTxChannel);//����ʵʱ����,�Ϻ���		
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
					if(BackstageProtocol1.DataBuffer[12+TotalLen68]==5)       //�ٻ��������������
					{
						IEC101_Staid_Enegy(5,USARTxChannel,7);
            os_dly_wait (30);	
						IEC101_Enegy_Send(USARTxChannel);
            EnergySign = 1;
					}
					else if(BackstageProtocol1.DataBuffer[12+TotalLen68]==0x45) //���᲻����λ�ĵ������������
					{
						IEC101_Staid_Enegy(0x45,USARTxChannel,7);
            EnergySign = 2;
					}
				}
				else if(TI==102) //���ͱ�ʶ��102��:=����������
				{
					switch(object_addr)
					{
						case 0x4801: 
						{
							ReadParameter(USARTxChannel,((sizeof(struct SYSTEM_PARA))/2));    //��ȡϵͳ����  							
						}
							break;
						case 0x4802: 
						{
							ReadParameter(USARTxChannel,((sizeof(struct PROTECT_PARA))/2));   //��ȡ��������		
						}
							break;
						case 0x4803: 
						{
							ReadParameter(USARTxChannel,((sizeof(struct ALARM_PARA))/2));     //��ȡ��������
						}
							break;
						case 0x4804: 
						{
							ReadParameter(USARTxChannel,((sizeof(struct AUTO_SWITCHOFF))/2)); //��ȡ�Զ������߲���
						}
							break;
						default:
							break;
					}
				}
				else if(TI==103)	//ʱ��ͬ��
				{
					if(COT==6)
					{	
						 TimeNow.second  = ((u16)(BackstageProtocol1.DataBuffer[12+TotalLen68] | (BackstageProtocol1.DataBuffer[13+TotalLen68]<<8))/1000)&0x3F;  
						 TimeNow.minute  = BackstageProtocol1.DataBuffer[14+TotalLen68]&0x3F;
						 TimeNow.hour    = BackstageProtocol1.DataBuffer[15+TotalLen68]&0x1F;
						 TimeNow.date    = BackstageProtocol1.DataBuffer[16+TotalLen68]&0x1F;
						 TimeNow.month   = BackstageProtocol1.DataBuffer[17+TotalLen68]&0x0F;
						 TimeNow.year    = BackstageProtocol1.DataBuffer[18+TotalLen68]&0x7F;
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
					TestAddr=BackstageProtocol1.DataBuffer[12+TotalLen68]+(BackstageProtocol1.DataBuffer[13+TotalLen68]<<8);
					Test_101(USARTxChannel);
				}
				else if(TI==105)	//��λ����
				{ 
					ResetLianlu(USARTxChannel);
					os_dly_wait (100);
					__set_FAULTMASK(1);
					NVIC_SystemReset();
				}
				else if(TI==107)	//�ٻ���ʷ��¼(˳���¼���¼)
				{
					History_Data(USARTxChannel);
				}
				else if(TI==108) //������Ȩ������
				{
					if(CodeTimeout == 0)
					{
						for(i=0;i<16;i++)
							AUTHORIZATIONcode[i]=BackstageProtocol1.DataBuffer[12+TotalLen68+i];
						CodeTimeout = BackstageProtocol1.DataBuffer[12+TotalLen68+14] | (BackstageProtocol1.DataBuffer[12+TotalLen68+15]<<8); //��Ȩ����
					}					
				}
				else if(TI==128) //���������ʼ����֡
				{
					MyVersion.GPRS_version[0] = BackstageProtocol1.DataBuffer[9+FrontReason];
					MyVersion.GPRS_version[1] = BackstageProtocol1.DataBuffer[10+FrontReason];
					MyVersion.GPRS_version[2] = BackstageProtocol1.DataBuffer[11+FrontReason];
					MyVersion.GPRS_version[3] = BackstageProtocol1.DataBuffer[12+FrontReason];
					ProgramUpdate_Confirm(USARTxChannel,128,13);           //����ȷ��
				}	
				else if(TI==129) //����
				{
					if((BackstageProtocol1.DataBuffer[11+FrontReason] | (BackstageProtocol1.DataBuffer[12+FrontReason] <<8)) == (Last_Number+1))
					{
						datalenth = (BackstageProtocol1.DataBuffer[1] | (BackstageProtocol1.DataBuffer[2]<<8))-9;
						if(datalenth < 801)
						{
							Program_Update_Receive(USARTxChannel,datalenth);//����	
							Last_Number = BackstageProtocol1.DataBuffer[11+FrontReason] | (BackstageProtocol1.DataBuffer[12+FrontReason] <<8);	
						}
						else
							ProgramUpdate_ERROR(USARTxChannel,0xFF);						
					}
					else
					{
						ProgramUpdate_ERROR(USARTxChannel,0xFF);
					}
				}	
				else if(TI==130) //�����������֡
				{
					MyVersion.GPRS_length_update = BackstageProtocol1.DataBuffer[10+FrontReason] | (BackstageProtocol1.DataBuffer[11+FrontReason]<<8) | (BackstageProtocol1.DataBuffer[12+FrontReason]<<16);
					if(MyVersion.GPRS_length_update == GPRSUpdateLength)
						ProgramUpdate_Confirm(USARTxChannel,130,13);           //����ȷ��
					else
					{
						ProgramUpdate_ERROR(USARTxChannel,0xFF);
					}
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
        else if(TI==48)//���ò�������
				{
					switch(object_addr)
					{
						case 0x4801: 
						{
							for(i=0;i<((sizeof(struct SYSTEM_PARA))/2);i++)
								SystemSet.word[i]=(BackstageProtocol1.DataBuffer[13+TotalLen68+2*i]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68+2*i];
							SetSystemFlag=1;
							SetParameter(USARTxChannel,((sizeof(struct SYSTEM_PARA))/2));
						}
							break;
						case 0x4802:
						{
							for(i=0;i<((sizeof(struct PROTECT_PARA))/2);i++)
								ProtectSet.word[i]=(BackstageProtocol1.DataBuffer[13+TotalLen68+2*i]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68+2*i];
							SetProtectFlag=1;
							SetParameter(USARTxChannel,((sizeof(struct PROTECT_PARA))/2));
						}
							break;
						case 0x4803:
						{
							for(i=0;i<((sizeof(struct ALARM_PARA))/2);i++)
								AlarmSet.word[i]=(BackstageProtocol1.DataBuffer[13+TotalLen68+2*i]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68+2*i];
							SetAlarmFlag=1;
							SetParameter(USARTxChannel,((sizeof(struct ALARM_PARA))/2));
						}
							break;	
						case 0x4804:
						{
							for(i=0;i<((sizeof(struct AUTO_SWITCHOFF))/2);i++)
								AutoswitchSet.word[i]=(BackstageProtocol1.DataBuffer[13+TotalLen68+2*i]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68+2*i];
							SetAutoswitchFlag=1;
							SetParameter(USARTxChannel,((sizeof(struct AUTO_SWITCHOFF))/2));
						}
							break;						
						default:
							break;
					}
				}
				else if(TI==52)        //�ٻ�ʵʱ����
				{
					if(MeasureData.Psaveflash == 0)
					{
						RealCurveFlag = 0x01;
						RealCurve_Confirm(USARTxChannel,52,7);                //����ȷ��	
					}					
				}
				else if(TI==53)        //�ٻ�ʵʱ���߼���
				{
					if(MeasureData.Psaveflash == 0)
					{
						if(RealCurveFlag != 0)
						{
							RealCurve_Data(USARTxChannel,53);
						}
					}
				}
				else if(TI==54)   //�������߼���
				{
					if(MeasureData.Psaveflash == 0)
					{
						InstantCurve_Data(USARTxChannel,54);
					}
				}
				else if(TI==55)   //��������ȷ��
				{
					SendLock_Cnt = 0;
					AD_StartPoint = 0;
					MeasureData.Psaveflag = 0;
	//				RealCurve_Confirm(USARTxChannel,55,10);                //�������	
				}								
				else if(TI==200)//�л�����
				{
					SN_ID=(u16)(BackstageProtocol1.DataBuffer[13+TotalLen68]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68];  
					Write_Loss_SN(USARTxChannel);
				}
				else if(TI==201)//������
				{	  
					Read_Loss_SN(USARTxChannel);
				}
				else if(TI==202)//������
				{
					Read_Loss_SetDeal(USARTxChannel);
					Read_Loss_Set(USARTxChannel);
				}
				else if(TI==203)//д����
				{	
					Write_Loss_SetGHDeal(USARTxChannel);
				}
				else if(TI==210)//�ļ�����
				{
					if(BackstageProtocol1.DataBuffer[13+TotalLen68]==1)//��Ŀ¼
					{
							Read_List_Deal();
							Read_List_OK(USARTxChannel,SearchList());
					}
					else if(BackstageProtocol1.DataBuffer[13+TotalLen68]==3)//���ļ�����
					{
							Read_Doc_Deal();
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
					else if(BackstageProtocol1.DataBuffer[13+TotalLen68]==6)//���ļ�ȷ��
					{
						//
					}
					else if(BackstageProtocol1.DataBuffer[13+TotalLen68]==7)//д�ļ�����
					{						
						Write_DocOK_Deal();
						Write_Doc_OK(USARTxChannel);
					}
					else if(BackstageProtocol1.DataBuffer[13+TotalLen68]==9)//д�ļ�
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
					Updatetype.byte = BackstageProtocol1.DataBuffer[12+TotalLen68];
					if(COT==6 && Updatetype.bit.S_E==1) //����
					{
						SoftUpdate(USARTxChannel,7);
						Last_Number = 0;
						LineLockNum.Write_Text=0;
						LineLockNum.Last_Text=0;
						LineLockNum.Next_Text=0;
					}
					else if(COT==8 && Updatetype.bit.S_E==0)//ȡ��
					{
						SoftUpdate(USARTxChannel,9);
						Last_Number = 0;
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
        if(BackstageProtocol1.DataBuffer[1]==0x40)	    //��λԶ����·
				{
					IEC101_Staid(0x20,USARTxChannel);             //��·��λȷ�ϣ���һ������
					InitSign=1;
				}
				else if(BackstageProtocol1.DataBuffer[1]==0x49)	//������·״̬
				{
					IEC101_Staid(0x0B,USARTxChannel);             //��·״̬
				}
				else if((BackstageProtocol1.DataBuffer[1] & 0x40) && (BackstageProtocol1.DataBuffer[1] & 0x0F)==0x0A)	//����1���û�����
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
							Read_RealData_Old(USARTxChannel);//����ʵʱ����//��߻��õ��Ϻ���
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
					else if(SOE_Flag!=0)//��λң��
					{
						Record_SOE_Send(USARTxChannel);  //SOE�¼�˳���¼  
						#ifdef SIM800C_MODEL
					    Message_Send(USARTxChannel);
					  #endif
					}        
					else if(Read_Flag & system_data)
					{
						Read_Flag &=~ system_data;             
						ReadParameter(USARTxChannel,((sizeof(struct SYSTEM_PARA))/2));    //��ȡϵͳ����  
					}
					else if(Read_Flag & protect_data)
					{
						Read_Flag &=~ protect_data;
						ReadParameter(USARTxChannel,((sizeof(struct PROTECT_PARA))/2));   //��ȡ��������		
					}
					else if(Read_Flag & alarm_data)
					{
						Read_Flag &=~ alarm_data;
						ReadParameter(USARTxChannel,((sizeof(struct ALARM_PARA))/2));     //��ȡ��������
					}
					else if(Read_Flag & autoswitch_data)
					{
						Read_Flag &=~ autoswitch_data;
						ReadParameter(USARTxChannel,((sizeof(struct AUTO_SWITCHOFF))/2)); //��ȡ�Զ������߲���
					}
					else if(SendLock_Cnt == 0xFF)
					{
						if(MeasureData.Psaveflash == 0)
						{
							RealCurve_Confirm(USARTxChannel,55,7); //�������߼���
						}
					}  
					else 
						IEC101_Staid(0x09,USARTxChannel);           //0 1 0ACD(Ҫ�����λ) 0DFC(����������λ) (��Ϊ0����ʹ��) 1001(������9Ϊ�����ٻ�������)  
				}
				else if((BackstageProtocol1.DataBuffer[1] & 0x40) && (BackstageProtocol1.DataBuffer[1] & 0x0F)==0x0B)	//����2���û�����
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
						Read_RealData_Old(USARTxChannel);//����ʵʱ����//��߻��õ��Ϻ���	
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
					if(BackstageProtocol1.DataBuffer[12+TotalLen68]==5)       //�ٻ��������������
					{
						EnergySign=1;
					}
					else if(BackstageProtocol1.DataBuffer[12+TotalLen68]==0x45) //���᲻����λ�ĵ������������
					{
						EnergySign=4;
					}
				}
				else if(TI==102) //���ͱ�ʶ��102��:=����������
				{
					switch(object_addr)
					{
						case 0x4801: 
						{
							Read_Flag |= system_data;
						}
							break;
						case 0x4802: 
						{
							Read_Flag |= protect_data;
						}
							break;
						case 0x4803: 
						{
							Read_Flag |= alarm_data; 
						}
							break;
						case 0x4804: 
						{
							Read_Flag |= autoswitch_data;
						}
							break;
						default:
							break;
					}
				}
				else if(TI==103)	//ʱ��ͬ��
				{
					if(COT==6)
					{
						TimeNow.second  = ((u16)(BackstageProtocol1.DataBuffer[12+TotalLen68] | (BackstageProtocol1.DataBuffer[13+TotalLen68]<<8))/1000)&0x3F;  
						TimeNow.minute  = BackstageProtocol1.DataBuffer[14+TotalLen68]&0x3F;
						TimeNow.hour    = BackstageProtocol1.DataBuffer[15+TotalLen68]&0x1F;
						TimeNow.date    = BackstageProtocol1.DataBuffer[16+TotalLen68]&0x1F;
						TimeNow.month   = BackstageProtocol1.DataBuffer[17+TotalLen68]&0x0F;
						TimeNow.year    = BackstageProtocol1.DataBuffer[18+TotalLen68]&0x7F;
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
					TestAddr=BackstageProtocol1.DataBuffer[12+TotalLen68]+(BackstageProtocol1.DataBuffer[13+TotalLen68]<<8);
					TestSign=1;
				}
				else if(TI==105)	//��λ����
				{
					ResetSign=1;
				}
				else if(TI==107)	//�ٻ���ʷ��¼(˳���¼���¼)
				{
					History_Data(USARTxChannel);
				}
				else if(TI==108) //������Ȩ������
				{
					if(CodeTimeout == 0)
					{
						for(i=0;i<16;i++)
							AUTHORIZATIONcode[i]=BackstageProtocol1.DataBuffer[12+TotalLen68+i];
						CodeTimeout = BackstageProtocol1.DataBuffer[12+TotalLen68+14] | (BackstageProtocol1.DataBuffer[12+TotalLen68+15]<<8); //��Ȩ����
					}					
				}
				else if(TI==128) //���������ʼ����֡
				{
					MyVersion.GPRS_version[0] = BackstageProtocol1.DataBuffer[9+FrontReason];
					MyVersion.GPRS_version[1] = BackstageProtocol1.DataBuffer[10+FrontReason];
					MyVersion.GPRS_version[2] = BackstageProtocol1.DataBuffer[11+FrontReason];
					MyVersion.GPRS_version[3] = BackstageProtocol1.DataBuffer[12+FrontReason];
					ProgramUpdate_Confirm(USARTxChannel,128,13);           //����ȷ��
				}	
				else if(TI==129) //����
				{
					if((BackstageProtocol1.DataBuffer[11+FrontReason] | (BackstageProtocol1.DataBuffer[12+FrontReason] <<8)) == (Last_Number+1))
					{
						datalenth = (BackstageProtocol1.DataBuffer[1] | (BackstageProtocol1.DataBuffer[2]<<8))-9;
						if(datalenth < 801)
						{
							Program_Update_Receive(USARTxChannel,datalenth);//����	
							Last_Number = BackstageProtocol1.DataBuffer[11+FrontReason] | (BackstageProtocol1.DataBuffer[12+FrontReason] <<8);	
						}
						else
							ProgramUpdate_ERROR(USARTxChannel,0xFF);						
					}
					else
					{
						ProgramUpdate_ERROR(USARTxChannel,0xFF);
					}
				}	
				else if(TI==130) //�����������֡
				{
					MyVersion.GPRS_length_update = BackstageProtocol1.DataBuffer[10+FrontReason] | (BackstageProtocol1.DataBuffer[11+FrontReason]<<8) | (BackstageProtocol1.DataBuffer[12+FrontReason]<<16);
					if(MyVersion.GPRS_length_update == GPRSUpdateLength)
						ProgramUpdate_Confirm(USARTxChannel,130,13);           //����ȷ��
					else
					{
						ProgramUpdate_ERROR(USARTxChannel,0xFF);
					}
				}	
				else if(TI==45 || TI==46)	//45����ң�أ�46˫��ң��
				{
					ControlSign=1;
				}
				else if(TI==48)//���ò�������
				{
				  switch(object_addr)
					{
						case 0x4801: 
						{
							for(i=0;i<((sizeof(struct SYSTEM_PARA))/2);i++)
								SystemSet.word[i]=(BackstageProtocol1.DataBuffer[13+TotalLen68+2*i]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68+2*i];
							SetSystemFlag=1;
							SetParameter(USARTxChannel,((sizeof(struct SYSTEM_PARA))/2));
						}
							break;
						case 0x4802:
						{
							for(i=0;i<((sizeof(struct PROTECT_PARA))/2);i++)
								ProtectSet.word[i]=(BackstageProtocol1.DataBuffer[13+TotalLen68+2*i]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68+2*i];
							SetProtectFlag=1;
							SetParameter(USARTxChannel,((sizeof(struct PROTECT_PARA))/2));
						}
							break;
						case 0x4803:
						{
							for(i=0;i<((sizeof(struct ALARM_PARA))/2);i++)
								AlarmSet.word[i]=(BackstageProtocol1.DataBuffer[13+TotalLen68+2*i]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68+2*i];
							SetAlarmFlag=1;
							SetParameter(USARTxChannel,((sizeof(struct ALARM_PARA))/2));
						}
							break;	
						case 0x4804:
						{
							for(i=0;i<((sizeof(struct AUTO_SWITCHOFF))/2);i++)
								AutoswitchSet.word[i]=(BackstageProtocol1.DataBuffer[13+TotalLen68+2*i]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68+2*i];
							SetAutoswitchFlag=1;
							SetParameter(USARTxChannel,((sizeof(struct AUTO_SWITCHOFF))/2));
						}
							break;						
						default:
							break;
					}
				}
				else if(TI==52)        //�ٻ�ʵʱ����
				{
					if(MeasureData.Psaveflash == 0)
					{
						RealCurveFlag = 0x01;
						RealCurve_Confirm(USARTxChannel,52,7);                //����ȷ��	
					}					
				}
				else if(TI==53)        //�ٻ�ʵʱ���߼���
				{
					if(MeasureData.Psaveflash == 0)
					{
						if(RealCurveFlag != 0)
						{
							RealCurve_Data(USARTxChannel,53);
						}
					}
				}
				else if(TI==54)   //�������߼���
				{
					if(MeasureData.Psaveflash == 0)
					{
						InstantCurve_Data(USARTxChannel,54);
					}
				}
				else if(TI==55)   //��������ȷ��
				{
					SendLock_Cnt = 0;
					AD_StartPoint = 0;
					MeasureData.Psaveflag = 0;
	//				RealCurve_Confirm(USARTxChannel,55,10);                //�������	
				}				
				else if(TI==200)//�л�����
				{
					SN_ID=(u16)(BackstageProtocol1.DataBuffer[13+TotalLen68]<<8)+BackstageProtocol1.DataBuffer[12+TotalLen68];
					SetSign=1;  
				}
				else if(TI==201)//������
				{
					SetSign=5; 
				}
				else if(TI==202)//������
				{
					SetSign=10;	
					Read_Loss_SetDeal(USARTxChannel);
				}
				else if(TI==203)//д����
				{
					Write_Loss_SetGHDeal(USARTxChannel);
				}
				else if(TI==210)//�ļ�����
				{
					if(BackstageProtocol1.DataBuffer[13+TotalLen68]==1)//��Ŀ¼
					{
						Read_List_Deal();
						DocSign=1;
					}
					else if(BackstageProtocol1.DataBuffer[13+TotalLen68]==3)//���ļ�����
					{
						Read_Doc_Deal();
						LineLockNum.Doc_Bz=SearchDoc();
						DocSign=10;//����
					}
					else if(BackstageProtocol1.DataBuffer[13+TotalLen68]==6)//���ļ�ȷ��
					{
				
					}
					else if(BackstageProtocol1.DataBuffer[13+TotalLen68]==7)//д�ļ�����
					{				
						Write_DocOK_Deal();
						WriteDocSign=1;
					}
					else if(BackstageProtocol1.DataBuffer[13+TotalLen68]==9)//д�ļ�
					{
						Write_Doc_Data_Deal();
						WriteDocSign=3;
					}
				}
				else if(TI==212)//�ļ�ͬ��
				{
					DocSameSign=1;
				}
				else if(TI==211) //������������ ���� ֹͣ
				{
					Updatetype.bit.S_E=BackstageProtocol1.DataBuffer[12+TotalLen68];
					if(COT==6 && Updatetype.bit.S_E==1) //����
					{
						SoftUpSign=1;
						Last_Number = 0;
						LineLockNum.Write_Text=0;
						LineLockNum.Last_Text=0;
						LineLockNum.Next_Text=0;
					}
					else if(COT==8&&Updatetype.bit.S_E==0)//ȡ��
					{
						SoftUpSign=10;
						Last_Number = 0;
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

//�ļ����� �ļ��洢 ���� �ڶ�Ƭ���ı��� ������ ������ �¼���  
//���ٻ� ң��
void ClearLock(void)        //����ģ������
{
//	CS2BZ=1;
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Lock);  //�����ϲ���Ҫ������������
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Fix);
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Rand);
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Frzd);
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Month);
//	CS2BZ=0;
	LineLockNum.Fix_Time=0;
	LineLockNum.Rand_Time=0;
	LineLockNum.Frzd_Time=0;
	LineLockNum.Sharp_Time=0;
	LineLockNum.Month_Time=0;
	LineLockNum.Fix_Place=0;
	LineLockNum.Rand_Place=0;
	LineLockNum.Frzd_Place=0;
	LineLockNum.Sharp_Place=0;
	LineLockNum.Month_Place=0;	
	SaveLock();
}
void ClearEvent(void)
{
//	CS2BZ=1;
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Event);   //�����ϲ���Ҫ������������
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Event_PAReverse);
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Event_PBReverse);
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Event_PCReverse);
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Event_PReverse);
//	SPI_Flash_Erase_Sector(Flash_LineLoss_Event_ClearLock);
//	CS2BZ=0;
	LineLossNum.PA_Reverse_Time=0;
	LineLossNum.PB_Reverse_Time=0;
	LineLossNum.PC_Reverse_Time=0;
	LineLossNum.P_Reverse_Time=0;
	LineLossNum.LineLoss_Zero_Time=0;
	LineLossNum.Event_Zero_Time=0;
	LineLossNum.Check_Time=0;
	
	LineLossNum.PA_Place=0;
	LineLossNum.PB_Place=0;
	LineLossNum.PC_Place=0;
	LineLossNum.P_Place=0;
	LineLossNum.LineLoss_Place=0;
	LineLossNum.Event_Place=0;
	LineLossNum.Check_Place=0;
	SaveEvent();
}
u8 FlashAdd(u8 *p,u16 k)
{
	u8 sum=0;
	u16 i;
	for(i=0;i<k;i++)
		sum+=p[i];
	return sum;
}
void SaveDocTime(void)
{
	u8 buffer[43],i;
	for(i=0;i<6;i++)
	{
		buffer[7*i]=DocTime[i].year;
		buffer[7*i+1]=DocTime[i].month;
		buffer[7*i+2]=DocTime[i].mday;
		buffer[7*i+3]=DocTime[i].hour;
		buffer[7*i+4]=DocTime[i].min;
		buffer[7*i+5]=DocTime[i].msec>>8;
		buffer[7*i+6]=DocTime[i].msec;
	}
	buffer[42]=FlashAdd(buffer,42);
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Event_DocTime,43);
	CS2BZ=0;
}
void ReadDocTime(void)
{
	u8 buffer[43],i;
	CS2BZ=1;
	SPI_Flash_Read(buffer,Flash_LineLoss_Event_DocTime,43);
	CS2BZ=0;
	if(buffer[42]==FlashAdd(buffer,42))
	{
		for(i=0;i<6;i++)
		{
			DocTime[i].year=buffer[7*i];
			DocTime[i].month=buffer[7*i+1];
			DocTime[i].mday=buffer[7*i+2];
			DocTime[i].hour=buffer[7*i+3];
			DocTime[i].min=buffer[7*i+4];
			DocTime[i].msec=(buffer[7*i+5]<<8)|buffer[7*i+6];
		}
	}
	else 
	{
		for(i=0;i<6;i++)
		{
			DocTime[i].year=TimeNow.year;
			DocTime[i].month=TimeNow.month;
			DocTime[i].mday=TimeNow.date;
			DocTime[i].hour=TimeNow.hour;
			DocTime[i].min=TimeNow.minute;
			DocTime[i].msec=(TimeNow.msec+TimeNow.second*1000)%60000;
		}
	}
}
void ChangeDocTime(u8 i)
{
	DocTime[i].year=TimeNow.year;
	DocTime[i].month=TimeNow.month;
	DocTime[i].mday=TimeNow.date;
	DocTime[i].hour=TimeNow.hour;
	DocTime[i].min=TimeNow.minute;
	DocTime[i].msec=(TimeNow.msec+TimeNow.second*1000)%60000;
	SaveDocTime();
}
void ReadLock(void)
{
	u8 buffer[40];
	CS2BZ=1;
	SPI_Flash_Read(buffer,Flash_LineLoss_Lock,21);
	CS2BZ=0;
	if(buffer[20]==FlashAdd(buffer,20))
	{
		LineLockNum.Fix_Time=(buffer[1]<<8)|buffer[0];
		LineLockNum.Rand_Time=(buffer[3]<<8)|buffer[2];
		LineLockNum.Frzd_Time=(buffer[5]<<8)|buffer[4];
		LineLockNum.Sharp_Time=(buffer[7]<<8)|buffer[6];
		LineLockNum.Month_Time=(buffer[9]<<8)|buffer[8];
		LineLockNum.Fix_Place=(buffer[11]<<8)|buffer[10];
		LineLockNum.Rand_Place=(buffer[13]<<8)|buffer[12];
		LineLockNum.Frzd_Place=(buffer[15]<<8)|buffer[14];
		LineLockNum.Sharp_Place=(buffer[17]<<8)|buffer[16];
		LineLockNum.Month_Place=(buffer[19]<<8)|buffer[18];		
	}
}
void ReadEvent(void)
{
	u8 buffer[40];
	CS2BZ=1;
	SPI_Flash_Read(buffer,Flash_LineLoss_Event,36);
	CS2BZ=0;
	if(buffer[35]==FlashAdd(buffer,35))
	{
		LineLossNum.PA_Reverse_Time=(buffer[3]<<24)|(buffer[2]<<16)|(buffer[1]<<8)|buffer[0];
		LineLossNum.PB_Reverse_Time=(buffer[7]<<24)|(buffer[6]<<16)|(buffer[5]<<8)|buffer[4];
		LineLossNum.PC_Reverse_Time=(buffer[11]<<24)|(buffer[10]<<16)|(buffer[9]<<8)|buffer[8];
		LineLossNum.P_Reverse_Time=(buffer[15]<<24)|(buffer[14]<<16)|(buffer[13]<<8)|buffer[12];
		LineLossNum.LineLoss_Zero_Time=(buffer[19]<<24)|(buffer[18]<<16)|(buffer[17]<<8)|buffer[16];
		LineLossNum.Event_Zero_Time=(buffer[23]<<24)|(buffer[22]<<16)|(buffer[21]<<8)|buffer[20];
		LineLossNum.Check_Time=(buffer[27]<<24)|(buffer[26]<<16)|(buffer[25]<<8)|buffer[24];
		LineLossNum.PA_Place=buffer[28];
		LineLossNum.PB_Place=buffer[29];
		LineLossNum.PC_Place=buffer[30];
		LineLossNum.P_Place=buffer[31];
		LineLossNum.LineLoss_Place=buffer[32];
		LineLossNum.Event_Place=buffer[33];
		LineLossNum.Check_Place=buffer[34];
	}	
}
void ShortToBuffer(u8 *p,u16 m)
{
	p[0]=m;
	p[1]=m>>8;
}
void UnitToBuffer(u8 *p,u32 m)
{
	p[0]=m;
	p[1]=m>>8;
	p[2]=m>>16;
	p[3]=m>>24;
}

void FloatToBuffer(u8 *p,float m)
{
	union FloatToChar Floatm;
	Floatm.f = m; 
	p[0]=Floatm.byte[0];
	p[1]=Floatm.byte[1];
	p[2]=Floatm.byte[2];
	p[3]=Floatm.byte[3];
}

void SaveLock(void)
{
	u8 buffer[40];
	ShortToBuffer(&buffer[0],LineLockNum.Fix_Time);
	ShortToBuffer(&buffer[2],LineLockNum.Rand_Time);
	ShortToBuffer(&buffer[4],LineLockNum.Frzd_Time);
	ShortToBuffer(&buffer[6],LineLockNum.Sharp_Time);
	ShortToBuffer(&buffer[8],LineLockNum.Month_Time);
	ShortToBuffer(&buffer[10],LineLockNum.Fix_Place);
	ShortToBuffer(&buffer[12],LineLockNum.Rand_Place);
	ShortToBuffer(&buffer[14],LineLockNum.Frzd_Place);
	ShortToBuffer(&buffer[16],LineLockNum.Sharp_Place);
	ShortToBuffer(&buffer[18],LineLockNum.Month_Place);
	buffer[20]=FlashAdd(buffer,20);
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Lock,21);
	CS2BZ=0;	
}
void SaveEvent(void)
{
	u8 buffer[40];
	UnitToBuffer(&buffer[0],LineLossNum.PA_Reverse_Time);
	UnitToBuffer(&buffer[4],LineLossNum.PB_Reverse_Time);
	UnitToBuffer(&buffer[8],LineLossNum.PC_Reverse_Time);
	UnitToBuffer(&buffer[12],LineLossNum.P_Reverse_Time);
	UnitToBuffer(&buffer[16],LineLossNum.LineLoss_Zero_Time);
	UnitToBuffer(&buffer[20],LineLossNum.Event_Zero_Time);
	UnitToBuffer(&buffer[24],LineLossNum.Check_Time);
	buffer[28]=LineLossNum.PA_Place;
	buffer[29]=LineLossNum.PB_Place;
	buffer[30]=LineLossNum.PC_Place;
	buffer[31]=LineLossNum.P_Place;
	buffer[32]=LineLossNum.LineLoss_Place;
	buffer[33]=LineLossNum.Event_Place;
	buffer[34]=LineLossNum.Check_Place;
	buffer[35]=FlashAdd(buffer,35);
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Event,36);
	CS2BZ=0;	
}
void Save_LineLoss_Fix(void)
{
	u8 buffer[Fix_Num];
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	
	FloatToBuffer(&buffer[6],MeasureData.Wp_all);
	FloatToBuffer(&buffer[10],MeasureData.Wq_1all+MeasureData.Wq_2all);
	FloatToBuffer(&buffer[14],MeasureData.Wq_1all);
	FloatToBuffer(&buffer[18],MeasureData.Wq_4all);
	FloatToBuffer(&buffer[22],MeasureData.WpFX_all);
	FloatToBuffer(&buffer[26],MeasureData.Wq_3all+MeasureData.Wq_4all);
	FloatToBuffer(&buffer[30],MeasureData.Wq_2all);
	FloatToBuffer(&buffer[34],MeasureData.Wq_3all);
	FloatToBuffer(&buffer[38],MeasureData.PA_val);
	FloatToBuffer(&buffer[42],MeasureData.PB_val);
	FloatToBuffer(&buffer[46],MeasureData.PC_val);
	FloatToBuffer(&buffer[50],MeasureData.P_val);
	if(LineLockNum.Fix_Place>=60) LineLockNum.Fix_Place=0;
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Fix + LineLockNum.Fix_Place*Fix_Num,Fix_Num);
	LineLockNum.Fix_Place++;
	if(LineLockNum.Fix_Place>=60) LineLockNum.Fix_Place=0;
	if(LineLockNum.Fix_Time<65000)LineLockNum.Fix_Time++;
	SaveLock();
	CS2BZ=0;	
	ChangeDocTime(0);	
}
void Save_LineLoss_Rand(void)
{
	u8 buffer[Rand_Num];
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	
	FloatToBuffer(&buffer[6],MeasureData.Wp_all);
	FloatToBuffer(&buffer[10],MeasureData.Wq_1all+MeasureData.Wq_2all);
	FloatToBuffer(&buffer[14],MeasureData.Wq_1all);
	FloatToBuffer(&buffer[18],MeasureData.Wq_4all);
	FloatToBuffer(&buffer[22],MeasureData.WpFX_all);
	FloatToBuffer(&buffer[26],MeasureData.Wq_3all+MeasureData.Wq_4all);
	FloatToBuffer(&buffer[30],MeasureData.Wq_2all);
	FloatToBuffer(&buffer[34],MeasureData.Wq_3all);
	FloatToBuffer(&buffer[38],MeasureData.PA_val);
	FloatToBuffer(&buffer[42],MeasureData.PB_val);
	FloatToBuffer(&buffer[46],MeasureData.PC_val);
	FloatToBuffer(&buffer[50],MeasureData.P_val);
	if(LineLockNum.Rand_Place>=3) LineLockNum.Rand_Place=0;
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Rand + LineLockNum.Rand_Place*Rand_Num,Rand_Num);
	LineLockNum.Rand_Place++;
	if(LineLockNum.Rand_Place>=3) LineLockNum.Rand_Place=0;
	if(LineLockNum.Rand_Time<65000)LineLockNum.Rand_Time++;
	SaveLock();
	CS2BZ=0;	
	ChangeDocTime(1);	
}
void Save_LineLoss_Frzd(void)
{
	u8 buffer[Frzd_Num];
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	
	FloatToBuffer(&buffer[6],MeasureData.Wp_all);
	FloatToBuffer(&buffer[10],MeasureData.Wq_1all+MeasureData.Wq_2all);
	FloatToBuffer(&buffer[14],MeasureData.Wq_1all);
	FloatToBuffer(&buffer[18],MeasureData.Wq_4all);
	FloatToBuffer(&buffer[22],MeasureData.WpFX_all);
	FloatToBuffer(&buffer[26],MeasureData.Wq_3all+MeasureData.Wq_4all);
	FloatToBuffer(&buffer[30],MeasureData.Wq_2all);
	FloatToBuffer(&buffer[34],MeasureData.Wq_3all);
	FloatToBuffer(&buffer[38],MeasureData.PA_val);
	FloatToBuffer(&buffer[42],MeasureData.PB_val);
	FloatToBuffer(&buffer[46],MeasureData.PC_val);
	FloatToBuffer(&buffer[50],MeasureData.P_val);
	if(LineLockNum.Frzd_Place>=62) LineLockNum.Frzd_Place=0;
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Frzd + LineLockNum.Frzd_Place*Frzd_Num,Frzd_Num);
	LineLockNum.Frzd_Place++;
	if(LineLockNum.Frzd_Place>=62) LineLockNum.Frzd_Place=0;
	if(LineLockNum.Frzd_Time<65000)LineLockNum.Frzd_Time++;
	SaveLock();
	CS2BZ=0;	
	ChangeDocTime(2);	
}
void Save_LineLoss_Sharp(void)
{
	u8 buffer[Sharp_Num];
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	
	FloatToBuffer(&buffer[6],MeasureData.Wp_all);
	FloatToBuffer(&buffer[10],MeasureData.WpFX_all);
	if(LineLockNum.Sharp_Place>=264) LineLockNum.Sharp_Place=0;
	CS2BZ=1;	
	SPI_Flash_Write(buffer,Flash_LineLoss_Sharp + LineLockNum.Sharp_Place*Sharp_Num,Sharp_Num);
	LineLockNum.Sharp_Place++;
	if(LineLockNum.Sharp_Place>=264) LineLockNum.Sharp_Place=0;
	if(LineLockNum.Sharp_Time<65000)LineLockNum.Sharp_Time++;
	SaveLock();
	CS2BZ=0;
	ChangeDocTime(3);	
}
void Save_LineLoss_Month(void)
{
	u8 buffer[Month_Num];
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	
	FloatToBuffer(&buffer[6],MeasureData.Wp_all);
	FloatToBuffer(&buffer[10],MeasureData.Wq_1all+MeasureData.Wq_2all);
	FloatToBuffer(&buffer[14],MeasureData.Wq_1all);
	FloatToBuffer(&buffer[18],MeasureData.Wq_4all);
	FloatToBuffer(&buffer[22],MeasureData.WpFX_all);
	FloatToBuffer(&buffer[26],MeasureData.Wq_3all+MeasureData.Wq_4all);
	FloatToBuffer(&buffer[30],MeasureData.Wq_2all);
	FloatToBuffer(&buffer[34],MeasureData.Wq_3all);
	if(LineLockNum.Month_Place>=12) LineLockNum.Month_Place=0;
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Month + LineLockNum.Month_Place*Month_Num,Month_Num);
	LineLockNum.Month_Place++;
	if(LineLockNum.Month_Place>=12) LineLockNum.Month_Place=0;
	if(LineLockNum.Month_Time<65000)LineLockNum.Month_Time++;
	SaveLock();
	CS2BZ=0;	
	ChangeDocTime(4);	
}
void Save_Event_PAReverse(u8 m) //������ָ� 0-���� 1-�ָ� ��1��0 
{
	u8 buffer[PAReverse_EndNum] ={0};
	u32 addr;
	if(m==0)
	{
		LineLossNum.PA_Reverse_Time++;
		if(LineLossNum.PA_Reverse_Time>99999999) LineLossNum.PA_Reverse_Time=99999999;
		buffer[0]=TimeNow.year;
		buffer[1]=TimeNow.month;
		buffer[2]=TimeNow.date;
		buffer[3]=TimeNow.hour;
		buffer[4]=TimeNow.minute;
		buffer[5]=TimeNow.second;
		ShortToBuffer(&buffer[6],TimeNow.msec);
		UnitToBuffer(&buffer[8],LineLossNum.PA_Reverse_Time);
		FloatToBuffer(&buffer[12],MeasureData.Wp_all);
		FloatToBuffer(&buffer[16],MeasureData.Wq_1all+MeasureData.Wq_2all);
		FloatToBuffer(&buffer[20],MeasureData.Wq_1all);
		FloatToBuffer(&buffer[24],MeasureData.Wq_4all);
		FloatToBuffer(&buffer[28],MeasureData.WpFX_all);
		FloatToBuffer(&buffer[32],MeasureData.Wq_3all+MeasureData.Wq_4all);
		FloatToBuffer(&buffer[36],MeasureData.Wq_2all);
		FloatToBuffer(&buffer[40],MeasureData.Wq_3all);
		if(LineLossNum.PA_Place>=10) LineLossNum.PA_Place=0;
		addr= Flash_LineLoss_Event_PAReverse + LineLossNum.PA_Place*PAReverse_EndNum;
		CS2BZ=1;
		SPI_Flash_Write(buffer,addr,PAReverse_EndNum);
		LineLossNum.PA_Place++;
		if(LineLossNum.PA_Place>=10) LineLossNum.PA_Place=0;
		
		SaveEvent();
		CS2BZ=0;			
	}
	else //�ָ��ܴ�������
	{
		buffer[0]=TimeNow.year;
		buffer[1]=TimeNow.month;
		buffer[2]=TimeNow.date;
		buffer[3]=TimeNow.hour;
		buffer[4]=TimeNow.minute;
		buffer[5]=TimeNow.second;
		ShortToBuffer(&buffer[6],TimeNow.msec);
		FloatToBuffer(&buffer[8],MeasureData.Wp_all);
		FloatToBuffer(&buffer[12],MeasureData.Wq_1all+MeasureData.Wq_2all);
		FloatToBuffer(&buffer[16],MeasureData.Wq_1all);
		FloatToBuffer(&buffer[20],MeasureData.Wq_4all);
		FloatToBuffer(&buffer[24],MeasureData.WpFX_all);
		FloatToBuffer(&buffer[28],MeasureData.Wq_3all+MeasureData.Wq_4all);
		FloatToBuffer(&buffer[32],MeasureData.Wq_2all);
		FloatToBuffer(&buffer[36],MeasureData.Wq_3all);
		if(LineLossNum.PA_Place==0) 
			addr= Flash_LineLoss_Event_PAReverse + 9*PAReverse_EndNum+PAReverse_ActNum;
		else 
			addr= Flash_LineLoss_Event_PAReverse + (LineLossNum.PA_Place-1)*PAReverse_EndNum+PAReverse_ActNum;
		CS2BZ=1;
		SPI_Flash_Write(buffer,addr,PAReverse_EndNum-PAReverse_ActNum);
		CS2BZ=0;
	}
	ChangeDocTime(5);	
}
void Save_Event_PBReverse(u8 m) //������ָ� 0-���� 1-�ָ� ��1��0 
{
	u8 buffer[PBReverse_EndNum] ={0};
	u32 addr;
	if(m==0)
	{
		LineLossNum.PB_Reverse_Time++;
		if(LineLossNum.PB_Reverse_Time>99999999) LineLossNum.PB_Reverse_Time=99999999;
		buffer[0]=TimeNow.year;
		buffer[1]=TimeNow.month;
		buffer[2]=TimeNow.date;
		buffer[3]=TimeNow.hour;
		buffer[4]=TimeNow.minute;
		buffer[5]=TimeNow.second;
		ShortToBuffer(&buffer[6],TimeNow.msec);
		UnitToBuffer(&buffer[8],LineLossNum.PB_Reverse_Time);
		FloatToBuffer(&buffer[12],MeasureData.Wp_all);
		FloatToBuffer(&buffer[16],MeasureData.Wq_1all+MeasureData.Wq_2all);
		FloatToBuffer(&buffer[20],MeasureData.Wq_1all);
		FloatToBuffer(&buffer[24],MeasureData.Wq_4all);
		FloatToBuffer(&buffer[28],MeasureData.WpFX_all);
		FloatToBuffer(&buffer[32],MeasureData.Wq_3all+MeasureData.Wq_4all);
		FloatToBuffer(&buffer[36],MeasureData.Wq_2all);
		FloatToBuffer(&buffer[40],MeasureData.Wq_3all);
		if(LineLossNum.PB_Place>=10) LineLossNum.PB_Place=0;
		addr= Flash_LineLoss_Event_PBReverse + LineLossNum.PB_Place*PBReverse_EndNum;
		CS2BZ=1;
		SPI_Flash_Write(buffer,addr,PBReverse_EndNum);
		LineLossNum.PB_Place++;
		if(LineLossNum.PB_Place>=10) LineLossNum.PB_Place=0;
		SaveEvent();
		CS2BZ=0;			
	}
	else //�ָ��ܴ�������
	{
		buffer[0]=TimeNow.year;
		buffer[1]=TimeNow.month;
		buffer[2]=TimeNow.date;
		buffer[3]=TimeNow.hour;
		buffer[4]=TimeNow.minute;
		buffer[5]=TimeNow.second;
		ShortToBuffer(&buffer[6],TimeNow.msec);
		FloatToBuffer(&buffer[8],MeasureData.Wp_all);
		FloatToBuffer(&buffer[12],MeasureData.Wq_1all+MeasureData.Wq_2all);
		FloatToBuffer(&buffer[16],MeasureData.Wq_1all);
		FloatToBuffer(&buffer[20],MeasureData.Wq_4all);
		FloatToBuffer(&buffer[24],MeasureData.WpFX_all);
		FloatToBuffer(&buffer[28],MeasureData.Wq_3all+MeasureData.Wq_4all);
		FloatToBuffer(&buffer[32],MeasureData.Wq_2all);
		FloatToBuffer(&buffer[36],MeasureData.Wq_3all);
		if(LineLossNum.PB_Place==0) 
			addr= Flash_LineLoss_Event_PBReverse + 9*PBReverse_EndNum+PBReverse_ActNum;
		else 
			addr= Flash_LineLoss_Event_PBReverse + (LineLossNum.PB_Place-1)*PBReverse_EndNum+PBReverse_ActNum;
		CS2BZ=1;
		SPI_Flash_Write(buffer,addr,PBReverse_EndNum-PBReverse_ActNum);
		CS2BZ=0;
	}
	ChangeDocTime(5);	
}
void Save_Event_PCReverse(u8 m) //������ָ� 0-���� 1-�ָ� ��1��0 
{
	u8 buffer[PCReverse_EndNum] ={0};
	u32 addr;
	if(m==0)
	{
		LineLossNum.PC_Reverse_Time++;
		if(LineLossNum.PC_Reverse_Time>99999999) LineLossNum.PC_Reverse_Time=99999999;
		buffer[0]=TimeNow.year;
		buffer[1]=TimeNow.month;
		buffer[2]=TimeNow.date;
		buffer[3]=TimeNow.hour;
		buffer[4]=TimeNow.minute;
		buffer[5]=TimeNow.second;
		ShortToBuffer(&buffer[6],TimeNow.msec);
		UnitToBuffer(&buffer[8],LineLossNum.PB_Reverse_Time);
		FloatToBuffer(&buffer[12],MeasureData.Wp_all);
		FloatToBuffer(&buffer[16],MeasureData.Wq_1all+MeasureData.Wq_2all);
		FloatToBuffer(&buffer[20],MeasureData.Wq_1all);
		FloatToBuffer(&buffer[24],MeasureData.Wq_4all);
		FloatToBuffer(&buffer[28],MeasureData.WpFX_all);
		FloatToBuffer(&buffer[32],MeasureData.Wq_3all+MeasureData.Wq_4all);
		FloatToBuffer(&buffer[36],MeasureData.Wq_2all);
		FloatToBuffer(&buffer[40],MeasureData.Wq_3all);
		if(LineLossNum.PC_Place>=10) LineLossNum.PC_Place=0;
		addr= Flash_LineLoss_Event_PCReverse + LineLossNum.PC_Place*PCReverse_EndNum;
		CS2BZ=1;
		SPI_Flash_Write(buffer,addr,PCReverse_EndNum);
		LineLossNum.PC_Place++;
		if(LineLossNum.PC_Place>=10) LineLossNum.PC_Place=0;
		SaveEvent();
		CS2BZ=0;			
	}
	else //�ָ��ܴ�������
	{
		buffer[0]=TimeNow.year;
		buffer[1]=TimeNow.month;
		buffer[2]=TimeNow.date;
		buffer[3]=TimeNow.hour;
		buffer[4]=TimeNow.minute;
		buffer[5]=TimeNow.second;
		ShortToBuffer(&buffer[6],TimeNow.msec);
		FloatToBuffer(&buffer[8],MeasureData.Wp_all);
		FloatToBuffer(&buffer[12],MeasureData.Wq_1all+MeasureData.Wq_2all);
		FloatToBuffer(&buffer[16],MeasureData.Wq_1all);
		FloatToBuffer(&buffer[20],MeasureData.Wq_4all);
		FloatToBuffer(&buffer[24],MeasureData.WpFX_all);
		FloatToBuffer(&buffer[28],MeasureData.Wq_3all+MeasureData.Wq_4all);
		FloatToBuffer(&buffer[32],MeasureData.Wq_2all);
		FloatToBuffer(&buffer[36],MeasureData.Wq_3all);
		if(LineLossNum.PC_Place==0) 
			addr= Flash_LineLoss_Event_PCReverse + 9*PCReverse_EndNum+PCReverse_ActNum;
		else 
			addr= Flash_LineLoss_Event_PCReverse + (LineLossNum.PC_Place-1)*PCReverse_EndNum+PCReverse_ActNum;
		CS2BZ=1;
		SPI_Flash_Write(buffer,addr,PCReverse_EndNum-PCReverse_ActNum);
		CS2BZ=0;
	}
	ChangeDocTime(5);	
}
void Save_Event_PReverse(void)  
{
	u8 buffer[PReverse_Num];
	u32 addr;
	LineLossNum.P_Reverse_Time++;
	if(LineLossNum.P_Reverse_Time>99999999) LineLossNum.P_Reverse_Time=99999999;
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	ShortToBuffer(&buffer[6],TimeNow.msec);
	UnitToBuffer(&buffer[8],LineLossNum.P_Reverse_Time);
	FloatToBuffer(&buffer[12],MeasureData.Wp_all);
	FloatToBuffer(&buffer[16],MeasureData.Wq_1all+MeasureData.Wq_2all);
	FloatToBuffer(&buffer[20],MeasureData.Wq_1all);
	FloatToBuffer(&buffer[24],MeasureData.Wq_4all);
	FloatToBuffer(&buffer[28],MeasureData.WpFX_all);
	FloatToBuffer(&buffer[32],MeasureData.Wq_3all+MeasureData.Wq_4all);
	FloatToBuffer(&buffer[36],MeasureData.Wq_2all);
	FloatToBuffer(&buffer[40],MeasureData.Wq_3all);
	if(LineLossNum.P_Place>=10) LineLossNum.P_Place=0;
	addr= Flash_LineLoss_Event_PReverse + LineLossNum.P_Place*PReverse_Num;
	CS2BZ=1;
	SPI_Flash_Write(buffer,addr,PReverse_Num);
	LineLossNum.P_Place++;
	if(LineLossNum.P_Place>=10) LineLossNum.P_Place=0;
	SaveEvent();
	CS2BZ=0;		
	ChangeDocTime(5);		
}

void Save_Event_ClearLoss(void)
{
	u8 buffer[ClearLock_Num];
	u32 addr;
	
	ClearLock();
	LineLossNum.LineLoss_Zero_Time++;
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	ShortToBuffer(&buffer[6],TimeNow.msec);
	UnitToBuffer(&buffer[8],LineLossNum.LineLoss_Zero_Time);
	FloatToBuffer(&buffer[12],MeasureData.Wp_all);
	FloatToBuffer(&buffer[16],MeasureData.Wq_1all+MeasureData.Wq_2all);
	FloatToBuffer(&buffer[20],MeasureData.Wq_1all);
	FloatToBuffer(&buffer[24],MeasureData.Wq_4all);
	FloatToBuffer(&buffer[28],MeasureData.WpFX_all);
	FloatToBuffer(&buffer[32],MeasureData.Wq_3all+MeasureData.Wq_4all);
	FloatToBuffer(&buffer[36],MeasureData.Wq_2all);
	FloatToBuffer(&buffer[40],MeasureData.Wq_3all);
	if(LineLossNum.LineLoss_Place>=10) LineLossNum.LineLoss_Place=0;
	addr= Flash_LineLoss_Event_ClearLock + LineLossNum.LineLoss_Place*ClearLock_Num;
	CS2BZ=1;
	SPI_Flash_Write(buffer,addr,ClearLock_Num);
	LineLossNum.LineLoss_Place++;
	if(LineLossNum.LineLoss_Place>=10) LineLossNum.LineLoss_Place=0;
	SaveEvent();
	CS2BZ=0;
	ChangeDocTime(5);	
}

void Save_Event_ClearEvent(void)
{
	u8 buffer[ClearEvent_Num];
	
	ClearEvent();
	LineLossNum.Event_Zero_Time++;
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	ShortToBuffer(&buffer[6],TimeNow.msec);
	UnitToBuffer(&buffer[8],LineLossNum.Event_Zero_Time);
	
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Event_ClearEvent,ClearEvent_Num);
	LineLossNum.Event_Place++;
	SaveEvent();
	CS2BZ=0;		
	ChangeDocTime(5);		
}

void Save_Event_CheckTime(void)
{
	u8 buffer[CheckTime_Num];
	u32 addr;
	LineLossNum.Check_Time++;
	if(LineLossNum.Check_Time>99999999) LineLossNum.Check_Time=99999999;
	buffer[0]=TimeNow.year;
	buffer[1]=TimeNow.month;
	buffer[2]=TimeNow.date;
	buffer[3]=TimeNow.hour;
	buffer[4]=TimeNow.minute;
	buffer[5]=TimeNow.second;
	ShortToBuffer(&buffer[6],TimeNow.msec);
	UnitToBuffer(&buffer[8],LineLossNum.Check_Time);
	if(LineLossNum.Check_Place>=10) LineLossNum.Check_Place=0;
	addr= Flash_LineLoss_Event_CheckTime + LineLossNum.Check_Place*CheckTime_Num;
	CS2BZ=1;
	SPI_Flash_Write(buffer,addr,CheckTime_Num);
	LineLossNum.Check_Place++;
	if(LineLossNum.Check_Place>=10) LineLossNum.Check_Place=0;
	SaveEvent();
	CS2BZ=0;
	ChangeDocTime(5);		
}
//�����¼���¼
void Lock_Event(void)
{
	if(lockbz.bit.D1==1)//15���Ӷ���
	{
	  Save_LineLoss_Fix();
		lockbz.bit.D1 = 0;
	}
	
	if(TimeNow.minute==0&&TimeNow.hour==0)//�ն���
	{
		if(lockbz.bit.D2==0)
		{
			lockbz.bit.D2=1;
			Save_LineLoss_Frzd();
		}
	}
	else lockbz.bit.D2=0;
	
	if(TimeNow.minute==0)//���㶳��
	{
		if(lockbz.bit.D3==0)
		{
			lockbz.bit.D3=1;
			Save_LineLoss_Sharp();
		}
	}
	else lockbz.bit.D3=0;
	//�¶��� ������ ����Сʱ
	if(TimeNow.date==LineSet.para.LockDay && TimeNow.hour==LineSet.para.LockHour)
	{
		if(lockbz.bit.D4==0)
		{
			lockbz.bit.D4=1;
			Save_LineLoss_Month();
		}
	}
	else lockbz.bit.D4=0;

	if(Rand_Flag == 1)
	{
		Save_LineLoss_Rand();
		Rand_Flag = 0;
	}
	else if(Clearloss_Flag == 1)
	{
		Save_Event_ClearLoss();
		Clearloss_Flag = 0;
	}
	else if(Clearevent_Flag == 1)
	{
		Save_Event_ClearEvent();
		Clearevent_Flag = 0;
	}
	else if(CheckTime_Flag == 1)
	{
	  Save_Event_CheckTime();
		CheckTime_Flag = 0;
	}
	else if(LockSet_Flag == 1)
	{
		SaveLockSet();
		LockSet_Flag = 0;
	}
}

//�����Ĵ����ķ��ļ�ͷ��Ϣ �ļ�������Ϣ�������ݽ��в��
void UintToASCI8(u8 *p,float zhi)
{
	sprintf((char*)&p[0],"%8.2f",zhi);
}

void IntToASCI10(u8 *p,float zhi)//PA,PB,PC,Pz
{
	if(zhi < 0)
	  sprintf((char*)&p[0],"%10.4f",zhi);
	else
		sprintf((char*)&p[0],"+%9.4f",zhi);
}

void UintZToASCI8(u8 *p,u32 zhi)
{
	sprintf((char*)&p[0],"%8d",zhi);
}

//�ļ����䱨�Ķ�ʱ���� 
#define Fix_Tou     51 //
#define Fix_Text    200//
#define Rand_Tou    51 //
#define Rand_Text   200
#define Frzd_Tou    51 //
#define Frzd_Text   200
#define Sharp_Tou   53 //
#define Sharp_Text  52 //3��һ��
#define Month_Tou   53 //
#define Month_Text  136
void InsertString(u8 USARTxChannel,u8 *sum,u8 *s,u8 num)
{
	u8 i;
	for(i=0;i<num;i++)
	 InsertByte(USARTxChannel,sum,s[i]);
}
void Read_Doc_Fix_OK(u8 USARTxChannel)
{
	u8 sum=0,i;	
	u32 textlen = 0;
	if(LineLockNum.Fix_Time<60) textlen=Fix_Tou+Fix_Text*LineLockNum.Fix_Time;
	else textlen=Fix_Tou+Fix_Text*60;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],44+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],44+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2)
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2)
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,4); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	InsertByte(USARTxChannel,&sum,0);//0-�ɹ� 1-ʧ��
	InsertByte(USARTxChannel,&sum,24);//�ļ�������
	InsertString(USARTxChannel,&sum,(u8*)"LINELOSS/FIXD/fixd01.msg",24);
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,textlen);
	InsertTeam(USARTxChannel,&sum,textlen>>16);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
//�ļ����ݷ���
void InsertByte1(u8 USARTxChannel,u8 *sum,u8 zhi,u8 *sum1)
{
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],zhi);
	*sum+=zhi;
	*sum1+=zhi;
}
void InsertString1(u8 USARTxChannel,u8 *sum,u8 *s,u8 num,u8 *sum1)
{
	u8 i;
	for(i=0;i<num;i++)
	{
	 InsertByte1(USARTxChannel,sum,s[i],sum1);
	}
}
void Read_Doc_Fix_Tou(u8 USARTxChannel)
{
	u8 i,sum=0,sum1=0,buf1[30];	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Fix_Tou+20+TotalLen68);//19+Fix_Tou+1
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Fix_Tou+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,0);//�ļ���ַƫ���ֽ�
	InsertTeam(USARTxChannel,&sum,0);
	if(LineLockNum.Fix_Time==0)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign=0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
		DocSign=20;
	}
	InsertString1(USARTxChannel,&sum,(u8*)"fixd01.msg, v1.0",16,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	for(i=0;i<12;i++)
		sprintf((char*)&buf1[2*i],"%02X",*(vu8*)(ID_Address+i));
	for(i=0;i<24;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
	if(LineLockNum.Fix_Time<60)sprintf((char*)buf1,",%3d,%2d",LineLockNum.Fix_Time,4);
	else sprintf((char*)buf1,",%3d,%2d",60,4);
	for(i=0;i<7;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
  InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	LineLockNum.Read_CS=0;
	LineLockNum.Read_Place=LineLockNum.Fix_Place;
	LineLockNum.Read_Text=0;
}

u8 JudgeEnd(u32 total,u16 nowplace,u16 readplace,u16 maxplace)
{
	if(total>=maxplace)
	{
		if(nowplace==readplace) 
		{
			return 1;
		}
		else 
			return 0;
	}
	else if(readplace==0) 
		return 1;
	else return 0;
}

float Char4ToFloat(u8 *p)
{
	union FloatToChar Floatm;
	Floatm.byte[0] = p[0];
	Floatm.byte[1] = p[1];
	Floatm.byte[2] = p[2];
	Floatm.byte[3] = p[3];
	return Floatm.f;
}

u32 Char4ToUnit32(u8 *p)
{
	u32 returnp;
	returnp=(p[3]<<24)+(p[2]<<16)+(p[1]<<8)+p[0];
	return returnp;
}


void Read_Doc_Fix_Data(u8 USARTxChannel)
{
	u8 i,j,sum=0,sum1=0,buf1[30];	
	u8 buffer[Fix_Num],endp = 0;
	
  endp=JudgeEnd(LineLockNum.Fix_Time,LineLockNum.Fix_Place,LineLockNum.Read_Place,60);
	LineLockNum.Read_Text=(LineLockNum.Read_CS-1)*Fix_Text+Fix_Tou;	
	CS2BZ=1;
	SPI_Flash_Read(buffer,Flash_LineLoss_Fix + LineLockNum.Read_Place*Fix_Num,Fix_Num);
	CS2BZ=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Fix_Text+20+TotalLen68);//Fix_Tou+12+8+1
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Fix_Text+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);  //
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ� 12
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 13
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text);//�ļ���ַƫ���ֽ� 
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text>>16);
	if(endp==1)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign = 0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
	}
	sprintf((char*)buf1,"%02d,",12);
	for(i=0;i<3;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//3
	sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d,",2000+buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);//20
	for(i=0;i<20;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//20
	for(j=0;j<8;j++)
	{
		sprintf((char*)buf1,"%04x,",FREEZE_15min_POSITIVE_ENERGY+j);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		UintToASCI8(buf1,Char4ToFloat(&buffer[6+4*j]));
		buf1[8]=',';
		for(i=0;i<9;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
	}
	for(j=0;j<4;j++)
	{
		sprintf((char*)buf1,"%04x,",TELEMETRY_LINELOSS_POWER+j);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		IntToASCI10(buf1,Char4ToFloat(&buffer[38+4*j]));
		if(j==3) buf1[10]=0x0D;
		else buf1[10]=',';
		for(i=0;i<11;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//11
	}
  InsertByte1(USARTxChannel,&sum,0x0A,&sum1);	
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Read_Doc_Rand_OK(u8 USARTxChannel)
{
	u8 sum=0,i;	
	u32 textlen = 0;
	if(LineLockNum.Rand_Time<3) textlen=Rand_Tou+Rand_Text*LineLockNum.Rand_Time;
	else textlen=Rand_Tou+Rand_Text*3;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],44+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],44+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,4); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	InsertByte(USARTxChannel,&sum,0);//0-�ɹ� 1-ʧ��
	InsertByte(USARTxChannel,&sum,24);//�ļ�������
	InsertString(USARTxChannel,&sum,(u8*)"LINELOSS/RAND/rand01.msg",24);
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,textlen);
	InsertTeam(USARTxChannel,&sum,textlen>>16);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
void Read_Doc_Error(u8 USARTxChannel)
{
	u8 sum=0,i;	
	u32 textlen=0;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],20+Doc_NameNum+TotalLen68);//22+8
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],20+Doc_NameNum+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,4); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	InsertByte(USARTxChannel,&sum,1);//0-�ɹ� 1-ʧ��
	InsertByte(USARTxChannel,&sum,Doc_NameNum);//�ļ�������
	for(i=0;i<Doc_NameNum;i++)
		InsertByte(USARTxChannel,&sum,Doc_Name[i]);
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,textlen);
	InsertTeam(USARTxChannel,&sum,textlen>>16);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Read_Doc_Rand_Tou(u8 USARTxChannel)
{
	u8 i,sum=0,sum1=0,buf1[30];	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Rand_Tou+20+TotalLen68);//19+Rand_Tou+1
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Rand_Tou+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,0);//�ļ���ַƫ���ֽ�
	InsertTeam(USARTxChannel,&sum,0);
	if(LineLockNum.Rand_Time==0)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign=0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
		DocSign=20;
	}
	
	InsertString1(USARTxChannel,&sum,(u8*)"rand01.msg, v1.0",16,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	for(i=0;i<12;i++)
		sprintf((char*)&buf1[2*i],"%02X",*(vu8*)(ID_Address+i));
	for(i=0;i<24;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
	if(LineLockNum.Rand_Time<60)sprintf((char*)buf1,",%3d,%2d",LineLockNum.Rand_Time,4);
	else sprintf((char*)buf1,",%3d,%2d",3,4);
	for(i=0;i<7;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
  InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	LineLockNum.Read_CS=0;
	LineLockNum.Read_Place=LineLockNum.Rand_Place;
	LineLockNum.Read_Text=0;
}

void Read_Doc_Rand_Data(u8 USARTxChannel)
{
	u8 i,j,sum=0,sum1=0,buf1[30];	
	u8 buffer[Rand_Num],endp = 0;

	endp=JudgeEnd(LineLockNum.Rand_Time,LineLockNum.Rand_Place,LineLockNum.Read_Place,3);
	LineLockNum.Read_Text=(LineLockNum.Read_CS-1)*Rand_Text+Rand_Tou;	
	CS2BZ=1;
	SPI_Flash_Read(buffer,Flash_LineLoss_Rand + LineLockNum.Read_Place*Rand_Num,Rand_Num);
	CS2BZ=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Rand_Text+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Rand_Text+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
  	InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ� 12
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 13
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text);//�ļ���ַƫ���ֽ� 
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text>>16);
	if(endp==1)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign = 0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
	}
	sprintf((char*)buf1,"%02d,",12);
	for(i=0;i<3;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//3
	sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d,",2000+buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);//20
	for(i=0;i<20;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//20
	for(j=0;j<8;j++)
	{
		sprintf((char*)buf1,"%04x,",CURRENT_POSITIVE_ENERGY+j);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		UintToASCI8(buf1,Char4ToFloat(&buffer[6+4*j]));
		buf1[8]=',';
		for(i=0;i<9;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
	}
	for(j=0;j<4;j++)
	{
		sprintf((char*)buf1,"%04x,",TELEMETRY_LINELOSS_POWER+j);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		IntToASCI10(buf1,Char4ToFloat(&buffer[38+4*j]));
		if(j==3) buf1[10]=0x0D;
		else buf1[10]=',';
		for(i=0;i<11;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//11
	}
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);	
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
} 

void Read_Doc_Frzd_OK(u8 USARTxChannel)
{
	u8 sum=0,i;	
	u32 textlen = 0;
	if(LineLockNum.Frzd_Time<62) textlen=Frzd_Tou+Frzd_Text*LineLockNum.Frzd_Time;
	else textlen=Frzd_Tou+Frzd_Text*62;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],44+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],44+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,4); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	InsertByte(USARTxChannel,&sum,0);//0-�ɹ� 1-ʧ��
	InsertByte(USARTxChannel,&sum,24);//�ļ�������
	InsertString(USARTxChannel,&sum,(u8*)"LINELOSS/FRZD/frzd01.msg",24);
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,textlen);
	InsertTeam(USARTxChannel,&sum,textlen>>16);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Read_Doc_Frzd_Tou(u8 USARTxChannel)
{
	u8 i,sum=0,sum1=0,buf1[30];	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Frzd_Tou+20+TotalLen68);//19+Frzd_Tou+1
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Frzd_Tou+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,0);//�ļ���ַƫ���ֽ�
	InsertTeam(USARTxChannel,&sum,0);
	
	if(LineLockNum.Frzd_Time==0)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign=0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
		DocSign=20;
	}
	
	InsertString1(USARTxChannel,&sum,(u8*)"frzd01.msg, v1.0",16,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	for(i=0;i<12;i++)
		sprintf((char*)&buf1[2*i],"%02X",*(vu8*)(ID_Address+i));
	for(i=0;i<24;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
	if(LineLockNum.Frzd_Time<62)sprintf((char*)buf1,",%3d,%2d",LineLockNum.Frzd_Time,4);
	else sprintf((char*)buf1,",%3d,%2d",62,4);
	for(i=0;i<7;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
  InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	LineLockNum.Read_CS=0;
	LineLockNum.Read_Place=LineLockNum.Frzd_Place;
	LineLockNum.Read_Text=0;
}
void Read_Doc_Frzd_Data(u8 USARTxChannel)
{
	u8 i,j,sum=0,sum1=0,buf1[30];	
	u8 buffer[Frzd_Num],endp = 0;

	endp=JudgeEnd(LineLockNum.Frzd_Time,LineLockNum.Frzd_Place,LineLockNum.Read_Place,62);	
	LineLockNum.Read_Text=(LineLockNum.Read_CS-1)*Frzd_Text+Frzd_Tou;	
	CS2BZ=1;
	SPI_Flash_Read(buffer,Flash_LineLoss_Frzd + LineLockNum.Read_Place*Frzd_Num,Frzd_Num);
	CS2BZ=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Frzd_Text+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Frzd_Text+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ� 12
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 13
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text);//�ļ���ַƫ���ֽ� 
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text>>16);
	if(endp==1)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign = 0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
	}
	sprintf((char*)buf1,"%02d,",12);
	for(i=0;i<3;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//3
	sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d,",2000+buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);//20
	for(i=0;i<20;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//20
	for(j=0;j<8;j++)
	{
		sprintf((char*)buf1,"%04x,",FREEZE_day_POSITIVE_ENERGY+j);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		UintToASCI8(buf1,Char4ToFloat(&buffer[6+4*j]));
		buf1[8]=',';
		for(i=0;i<9;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
	}
	for(j=0;j<4;j++)
	{
		sprintf((char*)buf1,"%04x,",TELEMETRY_LINELOSS_POWER+j);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		IntToASCI10(buf1,Char4ToFloat(&buffer[38+4*j]));
		if(j==3) buf1[10]=0x0D;
		else buf1[10]=',';
		for(i=0;i<11;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//11
	}
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);	
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
} 

void Read_Doc_Sharp_OK(u8 USARTxChannel)
{
	u8 sum=0,i;	
	u32 textlen = 0;
	if(LineLockNum.Sharp_Time<264) textlen=Sharp_Tou+Sharp_Text*LineLockNum.Sharp_Time;
	else textlen=Sharp_Tou+Sharp_Text*264;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,4); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	InsertByte(USARTxChannel,&sum,0);//0-�ɹ� 1-ʧ��
	InsertByte(USARTxChannel,&sum,28);//�ļ�������
	InsertString(USARTxChannel,&sum,(u8*)"LINELOSS/SHARPD/sharpd01.msg",28);
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,textlen);
	InsertTeam(USARTxChannel,&sum,textlen>>16);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Read_Doc_Sharp_Tou(u8 USARTxChannel)
{
	u8 i,sum=0,sum1=0,buf1[30];	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Sharp_Tou+20+TotalLen68);//19+Sharp_Tou+1
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Sharp_Tou+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,0);//�ļ���ַƫ���ֽ�
	InsertTeam(USARTxChannel,&sum,0);
	if(LineLockNum.Sharp_Time==0)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign=0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
		DocSign=20;
	}

	InsertString1(USARTxChannel,&sum,(u8*)"sharpd01.msg, v1.0",18,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	for(i=0;i<12;i++)
		sprintf((char*)&buf1[2*i],"%02X",*(vu8*)(ID_Address+i));
	for(i=0;i<24;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
	if(LineLockNum.Sharp_Time<264)sprintf((char*)buf1,",%3d,%2d",LineLockNum.Sharp_Time,4);
	else sprintf((char*)buf1,",%3d,%2d",264,4);
	for(i=0;i<7;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
  InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	LineLockNum.Read_CS=0;
	LineLockNum.Read_Place=LineLockNum.Sharp_Place;
	LineLockNum.Read_Text=0;
}
void Read_Doc_Sharp_Data(u8 USARTxChannel)
{
	u8 i,j,sum=0,sum1=0,buf1[30];	
	u8 buffer[Sharp_Num*3];

	//endp=JudgeEnd(LineLockNum.Sharp_Time,LineLockNum.Sharp_Place,LineLockNum.Read_Place,264);
	LineLockNum.Read_Text=(LineLockNum.Read_CS-1)*Sharp_Text*3+Sharp_Tou;	
	CS2BZ=1;
	if(LineLockNum.Read_Place == 262)
	{
	  SPI_Flash_Read(buffer,Flash_LineLoss_Sharp + LineLockNum.Read_Place*Sharp_Num,Sharp_Num*2);
		SPI_Flash_Read(&buffer[Sharp_Num*2],Flash_LineLoss_Sharp,Sharp_Num);
	}
	else if(LineLockNum.Read_Place == 263)
	{
	  SPI_Flash_Read(buffer,Flash_LineLoss_Sharp + LineLockNum.Read_Place*Sharp_Num,Sharp_Num);
		SPI_Flash_Read(&buffer[Sharp_Num],Flash_LineLoss_Sharp,Sharp_Num*2);
	}
	else
		SPI_Flash_Read(buffer,Flash_LineLoss_Sharp + LineLockNum.Read_Place*Sharp_Num,Sharp_Num*3);
	CS2BZ=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(LineLockNum.Read_EndBz==1)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Sharp_Text*LineLockNum.Read_Num+20+TotalLen68);//Fix_Tou+12+8+1
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Sharp_Text*LineLockNum.Read_Num+20+TotalLen68);
	}
	else 
	{
	  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Sharp_Text*3+20+TotalLen68);//22+8
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Sharp_Text*3+20+TotalLen68);
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ� 12
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 13
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text);//�ļ���ַƫ���ֽ� 
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text>>16);
	if(LineLockNum.Read_EndBz==1)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign = 0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
	}
	for(j=0;j<LineLockNum.Read_Num;j++)
	{
		sprintf((char*)buf1,"%2d,",2);
		for(i=0;i<3;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//3
		sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d,",2000+buffer[Sharp_Num*j],buffer[Sharp_Num*j+1],
		buffer[Sharp_Num*j+2],buffer[Sharp_Num*j+3],buffer[Sharp_Num*j+4],buffer[Sharp_Num*j+5]);//20
		for(i=0;i<20;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//20
		sprintf((char*)buf1,"%04x,",CURRENT_POSITIVE_ENERGY);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		UintToASCI8(buf1,Char4ToFloat(&buffer[6+Sharp_Num*j]));
		buf1[8]=',';
		for(i=0;i<9;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
		
		sprintf((char*)buf1,"%04x,",CURRENT_REVERSE_ENERGY);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		UintToASCI8(buf1,Char4ToFloat(&buffer[10+Sharp_Num*j]));
    buf1[8]=0x0D;
		buf1[9]=0x0A;
		for(i=0;i<10;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
	}	
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
} 

void Read_Doc_Month_OK(u8 USARTxChannel)
{
	u8 sum=0,i;	
	u32 textlen = 0;
	if(LineLockNum.Month_Time<12) textlen=Month_Tou+Month_Text*LineLockNum.Month_Time;
	else textlen=Month_Tou+Month_Text*12;
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,4); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	InsertByte(USARTxChannel,&sum,0);//0-�ɹ� 1-ʧ��
	InsertByte(USARTxChannel,&sum,28);//�ļ�������
	InsertString(USARTxChannel,&sum,(u8*)"LINELOSS/MONTHD/monthd01.msg",28);
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,textlen);
	InsertTeam(USARTxChannel,&sum,textlen>>16);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Read_Doc_Month_Tou(u8 USARTxChannel)
{
	u8 i,sum=0,sum1=0,buf1[30];	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Month_Tou+20+TotalLen68);//19+Month_Tou+1
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Month_Tou+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��
	
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,0);//�ļ���ַƫ���ֽ�
	InsertTeam(USARTxChannel,&sum,0);
	if(LineLockNum.Month_Time==0)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign=0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
		DocSign=20;
	}
	
	InsertString1(USARTxChannel,&sum,(u8*)"monthd01.msg, v1.0",18,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	for(i=0;i<12;i++)
		sprintf((char*)&buf1[2*i],"%02X",*(vu8*)(ID_Address+i));
	for(i=0;i<24;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
	if(LineLockNum.Month_Time<12)sprintf((char*)buf1,",%3d,%2d",LineLockNum.Month_Time,4);
	else sprintf((char*)buf1,",%3d,%2d",12,4);
	for(i=0;i<7;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
  InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	LineLockNum.Read_CS=0;
	LineLockNum.Read_Place=LineLockNum.Month_Place;
	LineLockNum.Read_Text=0;
}

void Read_Doc_Month_Data(u8 USARTxChannel)
{
	u8 i,j,sum=0,sum1=0,buf1[30];	
	u8 buffer[Month_Num],endp = 0;

	endp=JudgeEnd(LineLockNum.Month_Time,LineLockNum.Month_Place,LineLockNum.Read_Place,12);
	LineLockNum.Read_Text=(LineLockNum.Read_CS-1)*Month_Text+Month_Tou;	
	CS2BZ=1;
	SPI_Flash_Read(buffer,Flash_LineLoss_Month + LineLockNum.Read_Place*Month_Num,Month_Num);
	CS2BZ=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Month_Text+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Month_Text+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ� 12
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 13
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text);//�ļ���ַƫ���ֽ� 
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text>>16);
	if(endp==1)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign = 0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
	}
	sprintf((char*)buf1,"%02d,",8);
	for(i=0;i<3;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//3
	sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d,",2000+buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);//20
	for(i=0;i<20;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//20
	for(j=0;j<8;j++)
	{
		sprintf((char*)buf1,"%04x,",CURRENT_POSITIVE_ENERGY+j);
		for(i=0;i<5;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
		UintToASCI8(buf1,Char4ToFloat(&buffer[6+4*j]));
		if(j==7) buf1[8]=0x0D;
		else buf1[8]=',';
		for(i=0;i<9;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
	}	
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);	
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
} 

u8 GetAllEvent(void)
{
	u8 k=0;
	if(LineLossNum.PA_Reverse_Time>10) k=10;
	else k=LineLossNum.PA_Reverse_Time;
	if(LineLossNum.PB_Reverse_Time>10) k+=10;
	else k+=LineLossNum.PB_Reverse_Time;
	if(LineLossNum.PC_Reverse_Time>10) k+=10;
	else k+=LineLossNum.PC_Reverse_Time;
	if(LineLossNum.P_Reverse_Time>10) k+=10;
	else k+=LineLossNum.P_Reverse_Time;
	if(LineLossNum.LineLoss_Zero_Time>10) k+=10;
	else k+=LineLossNum.LineLoss_Zero_Time;
	if(LineLossNum.Event_Zero_Time>10) k+=10;
	else k+=LineLossNum.Event_Zero_Time;
	if(LineLossNum.Check_Time>10) k+=10;
	else k+=LineLossNum.Check_Time;
	return k;
}

#define Event_Tou         29   //�¼���¼��һ֡��Ϣ����
#define Event_Text_Pf     287  //��2֡����  //PA,PB,PC������Ϣ����
#define Event_Text_Pf1    150  //��1֡������ //��������Ϣ
#define Event_Text_Pf2    137  //��2֡������ //���������Ϣ
#define Event_Text_Pz     151  //4,5 ��������ı䣬����ģ��������Ϣ����
#define Event_Text_Clear  37   //6,7 �¼����㣬Уʱ��Ϣ����

void GetAddr(void)//�жϴ����Ƿ����
{
	u8 p[7],zhi;
	if(LineLossNum.PA_Reverse_Time>10)p[0]=10;
	else p[0]=LineLossNum.PA_Reverse_Time;
	if(LineLossNum.PB_Reverse_Time>10) p[1]=10;
	else p[1]=LineLossNum.PB_Reverse_Time;
	if(LineLossNum.PC_Reverse_Time>10)p[2]=10;
	else p[2]=LineLossNum.PC_Reverse_Time;
	if(LineLossNum.P_Reverse_Time>10) p[3]=10;
	else p[3]=LineLossNum.P_Reverse_Time;
	if(LineLossNum.LineLoss_Zero_Time>10) p[4]=10;
	else p[4]=LineLossNum.LineLoss_Zero_Time;
	if(LineLossNum.Event_Zero_Time>10) p[5]=10;
	else p[5]=LineLossNum.Event_Zero_Time;
	if(LineLossNum.Check_Time>10) p[6]=10;
	else p[6]=LineLossNum.Check_Time;
	
	LineLockNum.Read_CS++;
	
	if(LineLockNum.Read_CS<=(p[0]*2))
	{
			LineLockNum.Event_CS = LineLockNum.Read_CS;
		  LineLockNum.Event_Bz =1;
		  LineLockNum.Read_Text = Event_Tou + (LineLockNum.Read_CS-1)/2 * Event_Text_Pf + ((LineLockNum.Read_CS-1)%2)* Event_Text_Pf1;//��ʼλ��
		  if(LineLockNum.Read_CS%2) LineLockNum.Read_Num = Event_Text_Pf1;
			else LineLockNum.Read_Num = Event_Text_Pf2;
		 	if(LineLossNum.PA_Place<(LineLockNum.Event_CS+1)/2) LineLockNum.Read_Place=LineLossNum.PA_Place+10-(LineLockNum.Event_CS+1)/2;
			else LineLockNum.Read_Place=LineLossNum.PA_Place-(LineLockNum.Event_CS+1)/2;

	}
	else if(LineLockNum.Read_CS<=((p[0]+p[1])*2))
	{
			LineLockNum.Event_CS = LineLockNum.Read_CS-2*p[0];
		  LineLockNum.Event_Bz =2;
		  LineLockNum.Read_Text = Event_Tou + (LineLockNum.Read_CS-1)/2 * Event_Text_Pf + ((LineLockNum.Read_CS-1)%2)* Event_Text_Pf1;//��ʼλ��
		  if(LineLockNum.Read_CS%2) LineLockNum.Read_Num = Event_Text_Pf1;
			else LineLockNum.Read_Num = Event_Text_Pf2;
			if(LineLossNum.PB_Place<(LineLockNum.Event_CS+1)/2) LineLockNum.Read_Place=LineLossNum.PB_Place+10-(LineLockNum.Event_CS+1)/2;
			else LineLockNum.Read_Place=LineLossNum.PB_Place-(LineLockNum.Event_CS+1)/2;
	}
	else if(LineLockNum.Read_CS<=((p[0]+p[1]+p[2])*2))
	{
			LineLockNum.Event_CS = LineLockNum.Read_CS-2*(p[0]+p[1]);
		  LineLockNum.Event_Bz =3;
		  LineLockNum.Read_Text = Event_Tou + (LineLockNum.Read_CS-1)/2 * Event_Text_Pf + ((LineLockNum.Read_CS-1)%2)* Event_Text_Pf1;//��ʼλ��
		  if(LineLockNum.Read_CS%2) LineLockNum.Read_Num = Event_Text_Pf1;
			else LineLockNum.Read_Num = Event_Text_Pf2;
			if(LineLossNum.PC_Place<(LineLockNum.Event_CS+1)/2) LineLockNum.Read_Place=LineLossNum.PC_Place+10-(LineLockNum.Event_CS+1)/2;
			else LineLockNum.Read_Place=LineLossNum.PC_Place-(LineLockNum.Event_CS+1)/2;
	}
	else if(LineLockNum.Read_CS<=((p[0]+p[1]+p[2])*2)+p[3])
	{
			LineLockNum.Event_CS = LineLockNum.Read_CS-2*(p[0]+p[1]+p[2]);
		  LineLockNum.Event_Bz =4;
		  LineLockNum.Read_Text = Event_Tou + (p[0]+p[1]+p[2])* Event_Text_Pf + (LineLockNum.Event_CS-1)* Event_Text_Pz;//��ʼλ��
		  LineLockNum.Read_Num = Event_Text_Pz;
			if(LineLossNum.P_Place<LineLockNum.Event_CS) LineLockNum.Read_Place=LineLossNum.P_Place+10-LineLockNum.Event_CS;
			else LineLockNum.Read_Place=LineLossNum.P_Place-LineLockNum.Event_CS;
	}
  else if(LineLockNum.Read_CS<=((p[0]+p[1]+p[2])*2+p[3]+p[4]))
	{
	  	LineLockNum.Event_CS = LineLockNum.Read_CS-2*(p[0]+p[1]+p[2])-p[3];
		  LineLockNum.Event_Bz =5;
		  LineLockNum.Read_Text = Event_Tou + (p[0]+p[1]+p[2])* Event_Text_Pf +p[3]*Event_Text_Pz+ (LineLockNum.Event_CS-1)* Event_Text_Pz;//��ʼλ��
		  LineLockNum.Read_Num = Event_Text_Pz;
		  if(LineLossNum.LineLoss_Place<LineLockNum.Event_CS) LineLockNum.Read_Place=LineLossNum.LineLoss_Place+10-LineLockNum.Event_CS;
			else LineLockNum.Read_Place=LineLossNum.LineLoss_Place-LineLockNum.Event_CS;
	}
	else if(LineLockNum.Read_CS<=((p[0]+p[1]+p[2])*2+p[3]+p[4]+p[5]))
	{
		  LineLockNum.Event_CS = LineLockNum.Read_CS-2*(p[0]+p[1]+p[2])-p[3]-p[4];
		  LineLockNum.Event_Bz =6;
		  LineLockNum.Read_Text = Event_Tou + (p[0]+p[1]+p[2])* Event_Text_Pf +(p[3]+p[4])*Event_Text_Pz+ (LineLockNum.Event_CS-1)* Event_Text_Clear;//��ʼλ��
		  LineLockNum.Read_Num = Event_Text_Clear;
		  if(LineLossNum.Event_Place<LineLockNum.Event_CS) LineLockNum.Read_Place=LineLossNum.Event_Place+10-LineLockNum.Event_CS;
			else LineLockNum.Read_Place=LineLossNum.Event_Place-LineLockNum.Event_CS;
	}
	else
	{
			LineLockNum.Event_CS = LineLockNum.Read_CS-2*(p[0]+p[1]+p[2])-p[3]-p[4]-p[5];
		  LineLockNum.Event_Bz =7;
		  LineLockNum.Read_Text = Event_Tou + (p[0]+p[1]+p[2])* Event_Text_Pf +(p[3]+p[4])*Event_Text_Pz+ (p[5]+LineLockNum.Event_CS-1)* Event_Text_Clear;//��ʼλ��
		  LineLockNum.Read_Num = Event_Text_Clear;
			if(LineLossNum.Check_Place<LineLockNum.Event_CS) LineLockNum.Read_Place=LineLossNum.Check_Place+10-LineLockNum.Event_CS;
			else LineLockNum.Read_Place=LineLossNum.Check_Place-LineLockNum.Event_CS;
	}
	zhi=(p[0]+p[1]+p[2])*2+p[3]+p[4]+p[5]+p[6];
	if(zhi==LineLockNum.Read_CS)
		LineLockNum.Read_EndBz =1;
	else 	LineLockNum.Read_EndBz =0;
}	

u32 GetTotal(void)
{
	u8 p[7];
	u32 total=0;
	if(LineLossNum.PA_Reverse_Time>10)p[0]=10;
	else p[0]=LineLossNum.PA_Reverse_Time;
	if(LineLossNum.PB_Reverse_Time>10) p[1]=10;
	else p[1]=LineLossNum.PB_Reverse_Time;
	if(LineLossNum.PC_Reverse_Time>10)p[2]=10;
	else p[2]=LineLossNum.PC_Reverse_Time;
	if(LineLossNum.P_Reverse_Time>10) p[3]=10;
	else p[3]=LineLossNum.P_Reverse_Time;
	if(LineLossNum.LineLoss_Zero_Time>10) p[4]=10;
	else p[4]=LineLossNum.LineLoss_Zero_Time;
	if(LineLossNum.Event_Zero_Time>10) p[5]=10;
	else p[5]=LineLossNum.Event_Zero_Time;
	if(LineLossNum.Check_Time>10) p[6]=10;
	else p[6]=LineLossNum.Check_Time;
	total = Event_Tou + (p[0]+p[1]+p[2])* Event_Text_Pf +(p[3]+p[4])*Event_Text_Pz+ (p[5]+p[6])* Event_Text_Clear;
	return total;
}
void Read_Doc_Event_OK(u8 USARTxChannel)
{
	u8 sum=0,i;	
	u32 textlen = 0;
	textlen=GetTotal();
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],48+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,4); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	InsertByte(USARTxChannel,&sum,0);//0-�ɹ� 1-ʧ��
	InsertByte(USARTxChannel,&sum,28);//�ļ�������
	InsertString(USARTxChannel,&sum,(u8*)"LINELOSS/EVENTD/eventd01.msg",28);
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,textlen);
	InsertTeam(USARTxChannel,&sum,textlen>>16);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
void Read_Doc_Event_Data(u8 USARTxChannel)
{
	u8 i,j,sum=0,sum1=0,buf1[30];	
	u8 buffer[PAReverse_EndNum];//��ֽ�
	CS2BZ=1;
	if(LineLockNum.Event_Bz==1)
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_PAReverse + LineLockNum.Read_Place*PAReverse_EndNum,PAReverse_EndNum);
	else if(LineLockNum.Event_Bz==2)
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_PBReverse + LineLockNum.Read_Place*PBReverse_EndNum,PBReverse_EndNum);
	else if(LineLockNum.Event_Bz==3)
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_PCReverse + LineLockNum.Read_Place*PCReverse_EndNum,PCReverse_EndNum);
	else if(LineLockNum.Event_Bz==4)
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_PReverse + LineLockNum.Read_Place*PReverse_Num,PReverse_Num);
	else if(LineLockNum.Event_Bz==5)
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_ClearLock + LineLockNum.Read_Place*ClearLock_Num,ClearLock_Num);
	else if(LineLockNum.Event_Bz==6)
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_ClearEvent + LineLockNum.Read_Place*ClearEvent_Num,ClearEvent_Num);
	else if(LineLockNum.Event_Bz==7)
		SPI_Flash_Read(buffer,Flash_LineLoss_Event_CheckTime + LineLockNum.Read_Place*CheckTime_Num,CheckTime_Num);
	CS2BZ=0;
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],LineLockNum.Read_Num+20+TotalLen68);//22+8
  InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],LineLockNum.Read_Num+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ� 12
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 13
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text);//�ļ���ַƫ���ֽ� 
	InsertTeam(USARTxChannel,&sum,LineLockNum.Read_Text>>16);
	if(LineLockNum.Read_EndBz==1)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign = 0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
	}
	if(LineLockNum.Event_Bz<4)
	{
		
		if(LineLockNum.Event_CS%2)//ǰ��֡
		{
			sprintf((char*)buf1,"%2d,",LineLockNum.Event_Bz);//3
			for(i=0;i<3;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
			sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d.%03d,",2000+buffer[0],buffer[1],buffer[2],
			        buffer[3],buffer[4],buffer[5],((buffer[7]<<8)+buffer[6])%1000);//24
			for(i=0;i<24;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//24
			UintZToASCI8(buf1,Char4ToUnit32(&buffer[8]));//8-11
			buf1[8]=',';
			for(i=0;i<9;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
			sprintf((char*)buf1,"%1d,",8);
			for(i=0;i<2;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//2
			for(j=0;j<8;j++)
			{
				sprintf((char*)buf1,"%04x,",CURRENT_POSITIVE_ENERGY+j);
				for(i=0;i<5;i++)
					InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
				UintToASCI8(buf1,Char4ToFloat(&buffer[12+4*j]));
				buf1[8]=',';
				for(i=0;i<9;i++)
					InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
			}	
		}
		else//���֡
		{
			sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d.%03d,",2000+buffer[44],buffer[45],buffer[46],
							buffer[47],buffer[48],buffer[49],((buffer[51]<<8)+buffer[50])%1000);//24
			for(i=0;i<24;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//24
			for(j=0;j<8;j++)
			{
				sprintf((char*)buf1,"%04x,",CURRENT_POSITIVE_ENERGY+j);
				for(i=0;i<5;i++)
					InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
				UintToASCI8(buf1,Char4ToFloat(&buffer[52+4*j]));
				if(j==7) buf1[8]=0x0D;
				else buf1[8]=',';
				for(i=0;i<9;i++)
					InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
			}
			InsertByte1(USARTxChannel,&sum,0x0A,&sum1);			
		}
	}
	else if(LineLockNum.Event_Bz<6)
	{
		sprintf((char*)buf1,"%2d,",LineLockNum.Event_Bz);//3
		for(i=0;i<3;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
		sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d.%03d,",2000+buffer[0],buffer[1],buffer[2],
						buffer[3],buffer[4],buffer[5],((buffer[7]<<8)+buffer[6])%1000);//24
		for(i=0;i<24;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//24
		UintZToASCI8(buf1,Char4ToUnit32(&buffer[8]));//8-11
		buf1[8]=',';
		for(i=0;i<9;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
		sprintf((char*)buf1,"%1d,",8);
		for(i=0;i<2;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//2
		for(j=0;j<8;j++)
		{
			sprintf((char*)buf1,"%04x,",CURRENT_POSITIVE_ENERGY+j);
			for(i=0;i<5;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//5
			UintToASCI8(buf1,Char4ToFloat(&buffer[12+4*j]));
			if(j==7) buf1[8]=0x0D;
			else buf1[8]=',';
			for(i=0;i<9;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//9
		}	
		InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	}
	else
	{
		sprintf((char*)buf1,"%2d,",LineLockNum.Event_Bz);//3
		for(i=0;i<3;i++)
				InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
		sprintf((char*)buf1,"%04d-%02d-%02d %02d:%02d:%02d.%03d,",2000+buffer[0],buffer[1],buffer[2],
						buffer[3],buffer[4],buffer[5],((buffer[7]<<8)+buffer[6])%1000);//24
		for(i=0;i<24;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//24
		UintZToASCI8(buf1,Char4ToUnit32(&buffer[8]));//8-11
		buf1[8]=0x0D;
		buf1[9]=0x0A;
		for(i=0;i<10;i++)
			InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);//10
	}
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
} 

void Read_Doc_Event_Tou(u8 USARTxChannel)
{
	u8 i,sum=0,sum1=0,buf1[30];	
	LineLockNum.Event_Time=GetAllEvent();
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Event_Tou+20+TotalLen68);//19+Event_Tou+1
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],Event_Tou+20+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�12
	InsertByte(USARTxChannel,&sum,5); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 13
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,0);//�ļ���ַƫ���ֽ�
	InsertTeam(USARTxChannel,&sum,0);
	if(LineLockNum.Event_Time==0)
	{
		InsertByte(USARTxChannel,&sum,0);//�޺���
		DocSign=0;
	}
	else 
	{
		InsertByte(USARTxChannel,&sum,1);//�к���
		DocSign=20;
	}
	for(i=0;i<12;i++)
		sprintf((char*)&buf1[2*i],"%02X",*(vu8*)(ID_Address+i));
	for(i=0;i<24;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
	sprintf((char*)buf1,",%2d",LineLockNum.Event_Time);
	for(i=0;i<3;i++)
		InsertByte1(USARTxChannel,&sum,buf1[i],&sum1);
  InsertByte1(USARTxChannel,&sum,0x0D,&sum1);
	InsertByte1(USARTxChannel,&sum,0x0A,&sum1);
	InsertByte(USARTxChannel,&sum,sum1);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
	LineLockNum.Read_CS=0;
	LineLockNum.Read_Text=0;
}

//�ļ�ͬ������
void Read_Doc_Same(u8 USARTxChannel)
{
	u8 sum=0;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);//1+8
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,212);      //�ļ�����
	InsertByte(USARTxChannel,&sum,0);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,0);//��������
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void InsertValue(u8 k) //д 
{
	union FloatToChar Floatm;
	u8 i;
	for(i=0;i<4;i++)
	{
		Floatm.byte[i]=LockSetValue[k][i];
	}
	switch(LockSetAddr[k])
	{
		case 0x8021:LineSet.para.LockDay=Floatm.f;break;
		case 0x8022:LineSet.para.LockHour=Floatm.f;break;
		case 0x8023:LineSet.para.P_Reverse_Limit=Floatm.f;break;
		case 0x8024:LineSet.para.P_Reverse_Time=Floatm.f;break;
		case 0x8025:LineSet.para.Pt_Reverse_Limit=Floatm.f;break;
		case 0x8026:LineSet.para.Pt_Reverse_Time=Floatm.f;break;
	}
}

u8 InsertValue1(u8 k) //��ȡ
{
	u8 m=0;
	union FloatToChar Floatm;
	u8 i;	
	
	if(LockSetAddr[k]>=0x8021&&LockSetAddr[k]<=0x8026)
	{
		switch(LockSetAddr[k])
		{
			case 0x8021:Floatm.f=LineSet.para.LockDay;break;
			case 0x8022:Floatm.f=LineSet.para.LockHour;break;
			case 0x8023:Floatm.f=LineSet.para.P_Reverse_Limit;break;
			case 0x8024:Floatm.f=LineSet.para.P_Reverse_Time;break;
			case 0x8025:Floatm.f=LineSet.para.Pt_Reverse_Limit;break;
			case 0x8026:Floatm.f=LineSet.para.Pt_Reverse_Time;break;
		}
		for(i=0;i<4;i++)
		{
			LockSetValue[k][i]=Floatm.byte[i];
		}
		m=4;
	}
	return m;
}

u8 Fixd_Value(u8 k) //��ȡ
{
	u8 m=0;
	
	if(k == 0)
	{
		m = 9;
	}
	else if(k == 1)
	{
		m = 8;
	}
	else if(k == 2)
	{
		m = 4;
	}
	else if(k == 3)
	{
		m = 8;
	}
	else if(k == 4)
	{
		m = 8;
	}
	else if(k == 5)
	{
		m = 4;
	}
	else if(k == 6)
	{
		m = 18;
	}
	else if(k == 7)
	{
		m = 15;
	}
	else if(k == 8)
	{
		m = 24;
	}
	return m;
}

void Read_Loss_Set(u8 USARTxChannel)
{
	u8 i,sum=0,m,j;	
	u8 len=0;
	if(LockSetTotal == 0)
	{
		strcpy((char*)LockSetValue[0],"LINELOSS ");
		len+=9;
		strcpy((char*)LockSetValue[1],"ARM/RTX ");
		len+=8;
		strcpy((char*)LockSetValue[2],"XDDL");
		len+=4;
		strcpy((char*)LockSetValue[3],"HV01.01 ");
		len+=8;
		strcpy((char*)LockSetValue[4],"SV01.05 ");
		len+=8;
		strcpy((char*)LockSetValue[5],"A55A");
		len+=4;
		strcpy((char*)LockSetValue[6],"DL/T634.5101-2002 ");
		len+=18;
		strcpy((char*)LockSetValue[7],"ZW-20-LINELOSS ");
		len+=15;
		for(i=0;i<12;i++)
			sprintf((char*)&LockSetValue[8][2*i],"%02X",*(vu8*)(ID_Address+i));
		len+=24;
	}
	else
	{
		for(i=0;i<LockSetTotal;i++)
		{
			m=InsertValue1(i);
			len+=m; //�䳤
			len+=4;//�̶�4��
		}
  }
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+len+TotalLen68);//1+8+6
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+len+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,202);      //������
	if(LockSetTotal == 0)
	  InsertByte(USARTxChannel,&sum,9);    //
	else
	  InsertByte(USARTxChannel,&sum,LockSetTotal);    //
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��ֵ���� SN
	InsertByte(USARTxChannel,&sum,0);//��������
	
	if(LockSetTotal == 0)
	{
		for(i=0;i<9;i++)
		{
			InsertTeam(USARTxChannel,&sum,LockSetAddr[i]);//2
			InsertByte(USARTxChannel,&sum,LockSetType[i]);//1
			m = Fixd_Value(i);
			InsertByte(USARTxChannel,&sum,m);//4 ����
			for(j=0;j<m;j++)
				InsertByte(USARTxChannel,&sum,LockSetValue[i][j]);
		}
	}
	else
	{
		for(i=0;i<LockSetTotal;i++)
		{
			InsertTeam(USARTxChannel,&sum,LockSetAddr[i]);//2
			InsertByte(USARTxChannel,&sum,LockSetType[i]);//1
			m=InsertValue1(i);//1
			InsertByte(USARTxChannel,&sum,m);//4 ����
			for(j=0;j<m;j++)
				InsertByte(USARTxChannel,&sum,LockSetValue[i][j]);
		}
  }
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

void Read_Loss_SetDeal(u8 USARTxChnnel)
{
	u8 i,j=0;
	LockSetTotal=(BackstageProtocol1.DataBuffer[7+FrontReason]&0x7F);
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
			LockSetAddr[i]=BackstageProtocol1.DataBuffer[12+j+TotalLen68]+(BackstageProtocol1.DataBuffer[13+j+TotalLen68]<<8);//2
			j=j+2;
		}
  }
}

//д���� Ԥ�� �̻������� ������Ӧ �� ����һֱ ֻ�ı� Ϊ����
void Write_Loss_Set_YZ(u8 USARTxChannel)
{
	u8 i,k,j=0,sum=0;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	for(i=0;i<LockSetTotal;i++)
		j=j+LockSetNum[i]+4;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+j+TotalLen68);//1+8+6
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+j+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,203);      //������
	InsertByte(USARTxChannel,&sum,LockSetTotal);    //
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��ֵ���� SN
	InsertByte(USARTxChannel,&sum,settype.byte);//��������
	
	for(i=0;i<LockSetTotal;i++)
	{
		InsertTeam(USARTxChannel,&sum,LockSetAddr[i]);	
		InsertByte(USARTxChannel,&sum,LockSetType[i]);
		InsertByte(USARTxChannel,&sum,LockSetNum[i]);
		for(k=0;k<LockSetNum[i];k++)		
			InsertByte(USARTxChannel,&sum,LockSetValue[i][k]);	
	}
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
void Write_Loss_SetDeal(u8 USARTxChannel)
{
	u8 i,j=0,k;
	LockSetTotal=(BackstageProtocol1.DataBuffer[7+FrontReason]&0x7F);
	for(i=0;i<LockSetTotal;i++)
	{
		LockSetAddr[i]=BackstageProtocol1.DataBuffer[13+j+TotalLen68]+(BackstageProtocol1.DataBuffer[14+j+TotalLen68]<<8);//2
		LockSetType[i]=BackstageProtocol1.DataBuffer[15+j+TotalLen68];//1
		LockSetNum[i]=BackstageProtocol1.DataBuffer[16+j+TotalLen68];//1
		for(k=0;k<LockSetNum[i];k++)
		{
			LockSetValue[i][k]=BackstageProtocol1.DataBuffer[17+TotalLen68+k+j];
		}
		j=j+LockSetNum[i]+4;
	}
}

void Write_Loss_Set_GH(u8 USARTxChannel,u8 reason)
{
	u8 sum=0;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);//1+8+6
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,203);      //������
	InsertByte(USARTxChannel,&sum,0);    //
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,reason);//
	else 
		InsertByte(USARTxChannel,&sum,reason);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��ֵ���� SN
	InsertByte(USARTxChannel,&sum,settype.byte);//��������
		
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
//�л�SN��
void Write_Loss_SN(u8 USARTxChannel)
{
	u8 sum=0;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],10+TotalLen68);//1+8+6
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],10+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,200);      //������
	InsertByte(USARTxChannel,&sum,1);    //
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ���ַ SN
	InsertTeam(USARTxChannel,&sum,SN_ID);//��ֵ���� SN
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
//��SN��
void Read_Loss_SN(u8 USARTxChannel)
{
	u8 sum=0;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],14+TotalLen68);//1+8+6
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],14+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,201);      //������
	InsertByte(USARTxChannel,&sum,1);    //
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);//
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ���ַ SN
	InsertTeam(USARTxChannel,&sum,SN_ID);//��ֵ���� SN
	InsertTeam(USARTxChannel,&sum,0);
	InsertTeam(USARTxChannel,&sum,1);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
void SaveLockSet(void)
{
	u8 buffer[40];
	FloatToBuffer(&buffer[0],LineSet.para.LockDay);
	FloatToBuffer(&buffer[4],LineSet.para.LockHour);
	FloatToBuffer(&buffer[8],LineSet.para.P_Reverse_Limit);
	FloatToBuffer(&buffer[12],LineSet.para.P_Reverse_Time);
	FloatToBuffer(&buffer[16],LineSet.para.Pt_Reverse_Limit);
	FloatToBuffer(&buffer[20],LineSet.para.Pt_Reverse_Time);
	buffer[24]=FlashAdd(buffer,24);
	CS2BZ=1;
	SPI_Flash_Write(buffer,Flash_LineLoss_Set,25);
	CS2BZ=0;	
}
void ReadLockSet(void)
{
	u8 buffer[40],i;
	CS2BZ=1;
	SPI_Flash_Read(buffer,Flash_LineLoss_Set,25);
	CS2BZ=0;
	if(buffer[24]==FlashAdd(buffer,24))
	{
		for(i=0;i<6;i++)
		{
		  LineSet.two_array[i][0]=buffer[4*i];
			LineSet.two_array[i][1]=buffer[4*i+1];
			LineSet.two_array[i][2]=buffer[4*i+2];
			LineSet.two_array[i][3]=buffer[4*i+3];			
		}
		if(LineSet.para.LockDay>31) LineSet.para.LockDay=1;
		if(LineSet.para.LockHour>23) LineSet.para.LockHour=0;
		if(LineSet.para.P_Reverse_Limit>9999) LineSet.para.P_Reverse_Limit=2.5;
		if(LineSet.para.P_Reverse_Time>9999) LineSet.para.P_Reverse_Time=60;
		if(LineSet.para.Pt_Reverse_Limit>9999) LineSet.para.Pt_Reverse_Limit=2.5;
		if(LineSet.para.Pt_Reverse_Time>9999) LineSet.para.Pt_Reverse_Time=60;
	}	
}

void Write_Loss_SetGHDeal(u8 USARTxChannel)
{
	u8 i,j;
	settype.byte=BackstageProtocol1.DataBuffer[12+TotalLen68];
	
	if(settype.bit.S_E==1)//Ԥ��
	{
		Write_Loss_SetDeal(USARTxChannel);
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

void ReadAllLossSave(void)
{
	ReadLockSet();
	ReadLock();
	ReadEvent();
	ReadDocTime();
}
//��Ŀ¼
void Read_List_OK(u8 USARTxChannel,u8 Listn)//�Ƿ����ļ� ,Ŀ¼�� 0��Ч 1-6��Ч
{
	u8 sum=0,i;	
	u32 textlen=0;
	if(Listn==1)
	{
		if(LineLockNum.Fix_Time<60) textlen=Fix_Tou+Fix_Text*LineLockNum.Fix_Time;
		else textlen=Fix_Tou+Fix_Text*60;
	}
	else if(Listn==2)
	{
		if(LineLockNum.Rand_Time<3) textlen=Rand_Tou+Rand_Text*LineLockNum.Rand_Time;
		else textlen=Rand_Tou+Rand_Text*3;
	}
	else if(Listn==3)
	{
		if(LineLockNum.Frzd_Time<62) textlen=Frzd_Tou+Frzd_Text*LineLockNum.Frzd_Time;
		else textlen=Frzd_Tou+Frzd_Text*62;
	}
	else if(Listn==4)
	{
		if(LineLockNum.Sharp_Time<264) textlen=Sharp_Tou+Sharp_Text*LineLockNum.Sharp_Time;
		else textlen=Sharp_Tou+Sharp_Text*264;
	}
	else if(Listn==5)
	{
		if(LineLockNum.Month_Time<12) textlen=Month_Tou+Month_Text*LineLockNum.Month_Time;
	  else textlen=Month_Tou+Month_Text*12;
	}
	else if(Listn==6)
	{
		textlen=GetTotal();
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(Listn>=1&&Listn<=3)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],40+TotalLen68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],40+TotalLen68);
	}
	else if(Listn>=4&&Listn<=6)
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],42+TotalLen68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],42+TotalLen68);
	}
	else 
	{
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],17+TotalLen68);
		InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],17+TotalLen68);
	}		
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);    //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,5);//
	else 
		InsertByte(USARTxChannel,&sum,5);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,2); //1-��Ŀ¼ 2-Ŀ¼ 3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ��
	if(Listn==0||Listn>6) 
		InsertByte(USARTxChannel,&sum,1);
	else 
		InsertByte(USARTxChannel,&sum,0);//0-�ɹ� 1-ʧ��
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,List_ID[i]);
	InsertByte(USARTxChannel,&sum,0);//0-�޺���  1-�к���
	if(Listn==0||Listn>6)
		InsertByte(USARTxChannel,&sum,0); 
	else
	{		
		InsertByte(USARTxChannel,&sum,1); //�ļ�����
		if(Listn==1)
		{
			InsertByte(USARTxChannel,&sum,10);     //�ļ�������
			InsertString(USARTxChannel,&sum,(u8*)"fixd01.msg",10);//����
		}
		else if(Listn==2)
		{
			InsertByte(USARTxChannel,&sum,10);     //�ļ�������
			InsertString(USARTxChannel,&sum,(u8*)"rand01.msg",10);//����
		}
		else if(Listn==3)
		{
			InsertByte(USARTxChannel,&sum,10);     //�ļ�������
			InsertString(USARTxChannel,&sum,(u8*)"frzd01.msg",10);//����
		}
		else if(Listn==4)
		{
			InsertByte(USARTxChannel,&sum,12);     //�ļ�������
			InsertString(USARTxChannel,&sum,(u8*)"sharpd01.msg",12);//����
		}
		else if(Listn==5)
		{
			InsertByte(USARTxChannel,&sum,12);     //�ļ�������
			InsertString(USARTxChannel,&sum,(u8*)"monthd01.msg",12);//����
		}
		else //if(Listn==6)
		{
			InsertByte(USARTxChannel,&sum,12);     //�ļ�������
			InsertString(USARTxChannel,&sum,(u8*)"eventd01.msg",12);//����
		}
		InsertByte(USARTxChannel,&sum,0); //����
		InsertTeam(USARTxChannel,&sum,textlen);//��С
		InsertTeam(USARTxChannel,&sum,textlen>>16);
		InsertTeam(USARTxChannel,&sum,DocTime[Listn-1].msec);//ʱ��
		InsertByte(USARTxChannel,&sum,DocTime[Listn-1].min);
		InsertByte(USARTxChannel,&sum,DocTime[Listn-1].hour);
		InsertByte(USARTxChannel,&sum,DocTime[Listn-1].mday);
		InsertByte(USARTxChannel,&sum,DocTime[Listn-1].month);
		InsertByte(USARTxChannel,&sum,DocTime[Listn-1].year);
	}
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

//����2��������ʱ����Ĭ��0 ��Ҫ�Ļ� ��Ҫ�Լ� ȷ�� ��ͬĿ¼ ��ͬ�ļ���ID��
void Read_List_Deal(void)
{
	u8 i;
//	for(i=0;i<4;i++)
//		List_ID[i]=BackstageProtocol1.DataBuffer[14+TotalLen68+i];
	for(i=0;i<BackstageProtocol1.DataBuffer[18+TotalLen68];i++)
	 List_Name[i]=BackstageProtocol1.DataBuffer[19+TotalLen68+i];
	List_NameNum=BackstageProtocol1.DataBuffer[18+TotalLen68];
}
void Read_Doc_Deal(void)
{
	u8 i;
	for(i=0;i<BackstageProtocol1.DataBuffer[14+TotalLen68];i++)
	 Doc_Name[i]=BackstageProtocol1.DataBuffer[15+TotalLen68+i];
	Doc_NameNum=BackstageProtocol1.DataBuffer[14+TotalLen68];
}

void SoftUpdate(u8 USARTxChannel,u8 reason) //7-���� 9-��ֹ 10-����
{
	u8 sum=0;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);//1+8+6
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],9+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,211);      //��������
	InsertByte(USARTxChannel,&sum,0);        //VSQ
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,reason);//
	else 
		InsertByte(USARTxChannel,&sum,reason);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//������ַ
	InsertByte(USARTxChannel,&sum,Updatetype.byte);//CTYPE
		
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}

//д�ļ�ֻ������������
void Write_Doc_OK(u8 USARTxChannel)//д�ļ�����ȷ��
{
	u8 sum=0,i;	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],20+Doc_NameNum+TotalLen68);//22+8
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],20+Doc_NameNum+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);        //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);      //���ӷ��򣬼���ȷ��
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ�

	InsertByte(USARTxChannel,&sum,8); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 7-д�ļ����� 8-д�ļ�ȷ�� 9д�ļ� 10-д�ļ�ȷ��
	InsertByte(USARTxChannel,&sum,0);//0-�ɹ� 1-ʧ��
	InsertByte(USARTxChannel,&sum,Doc_NameNum);//�ļ�������
	if(Doc_NameNum > 20)
		Doc_NameNum = 20;
	for(i=0;i<Doc_NameNum;i++)
		InsertByte(USARTxChannel,&sum,Doc_Name[i]);
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,DocLen);
	InsertTeam(USARTxChannel,&sum,DocLen>>16);
	
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
void Write_Doc_Data_OK(u8 USARTxChannel,u8 m)//д�ļ�����ȷ��
{
	u8 sum=0,i;		
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],19+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],19+TotalLen68);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x68);
	if(balance == 1)
	  InsertByte(USARTxChannel,&sum,control_field.byte);//ȷ��
	else 
	  InsertByte(USARTxChannel,&sum,0x28);//ȷ��

	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertByte(USARTxChannel,&sum,210);      //�ļ�����
	InsertByte(USARTxChannel,&sum,1);        //��Ч
	if(Reason101==2) 
		InsertTeam(USARTxChannel,&sum,7);      //���ӷ��򣬼���ȷ��
	else 
		InsertByte(USARTxChannel,&sum,7);
	if(Addr101==2) 
		InsertTeam(USARTxChannel,&sum,SystemSet.para.address);
	else 
		InsertByte(USARTxChannel,&sum,SystemSet.para.address);
	InsertTeam(USARTxChannel,&sum,0);//��Ϣ��ַ
	InsertByte(USARTxChannel,&sum,2);//�ļ�����//���¸������ݰ� 12

	InsertByte(USARTxChannel,&sum,10); //3-���� 4-����ȷ�� 5-���ļ���Ӧ 6-���ļ�ȷ�� 7-д�ļ����� 8-д�ļ�ȷ�� 9д�ļ� 10-д�ļ�ȷ��
	for(i=0;i<4;i++)
		InsertByte(USARTxChannel,&sum,Doc_ID[i]);
	InsertTeam(USARTxChannel,&sum,LineLockNum.Write_Text);
	InsertTeam(USARTxChannel,&sum,LineLockNum.Write_Text>>16);
	InsertByte(USARTxChannel,&sum,m);//m=0 �ɹ�  1-δ֪����  2-У��ʹ���  3-�ļ����Ȳ���Ӧ 4-�ļ�ID�뼤��ID��һ��
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],sum&0xff);
	InsertDataToQueue(QueueTXChannel[USARTxChannel],BufferTXChannel[USARTxChannel],0x16);
}
void Write_DocOK_Deal(void)
{
	u8 i;
	Doc_NameNum=BackstageProtocol1.DataBuffer[14+TotalLen68];
	if(Doc_NameNum > 20)
		Doc_NameNum = 20;
	for(i=0;i<Doc_NameNum;i++)
		Doc_Name[i]=BackstageProtocol1.DataBuffer[15+TotalLen68+i];
	for(i=0;i<4;i++)
		JHDoc_ID[i]=Doc_ID[i]=BackstageProtocol1.DataBuffer[15+Doc_NameNum+TotalLen68+i];
	DocLen=Char4ToFloat(&BackstageProtocol1.DataBuffer[19+Doc_NameNum+TotalLen68]);
	
}
void Write_Doc_Data_Deal(void)
{
	u8 i,jxaddr,jxaddr1;
	for(i=0;i<4;i++)
		Doc_ID[i]=BackstageProtocol1.DataBuffer[14+TotalLen68+i];
	
	LineLockNum.Last_Text =LineLockNum.Write_Text;
	LineLockNum.Write_Text=Char4ToFloat(&BackstageProtocol1.DataBuffer[18+TotalLen68]);
	if(LineLockNum.Write_Text==0)GPRSUpdateLength=0;
	
	LineLockNum.Write_EndBz=BackstageProtocol1.DataBuffer[22+TotalLen68];
	
	DocByteNum=BackstageProtocol1.DataBuffer[1]-20-TotalLen68;
	for(i=0;i<DocByteNum;i++)
	{
		DocByte[i]=BackstageProtocol1.DataBuffer[23+TotalLen68+i];
	}
	jxaddr=BackstageProtocol1.DataBuffer[23+DocByteNum+TotalLen68];
	jxaddr1=UsartAdd(DocByte,0,DocByteNum);
	DocErr=0;
	if(jxaddr!=jxaddr1) DocErr=2;//У�����
	else if(LineLockNum.Write_EndBz==0 && DocLen!=DocByteNum+LineLockNum.Write_Text)
	{
		DocErr=3;
	}
	else if(JHDoc_ID[0]!=Doc_ID[0]||JHDoc_ID[1]!=Doc_ID[1]||JHDoc_ID[2]!=Doc_ID[2]||JHDoc_ID[3]!=Doc_ID[3])
	{
			DocErr=4;
	}
	if(DocErr==0)
	{
		CS2BZ=0;
		SPI_Flash_Write(DocByte,FLASH_KZQUPDATE_ADDR+GPRSUpdateLength,DocByteNum);
		GPRSUpdateLength += DocByteNum;
	}
}

