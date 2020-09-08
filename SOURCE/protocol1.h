/*************************************************************
*�ļ����ƣ�protocol1.h
*�������ڣ�2018��11��16��
*������ڣ�
*��    �ߣ�shq
**************************************************************/
#include "global.h"
#ifndef _PROTOCOL1_H_
#define _PROTOCOL1_H_

// Ӳ����Դ�͸�λ���Ŷ���
#define GPRS_POW_ON			  GPIO_ResetBits(GPIOF,GPIO_Pin_1) 	/* ��GPRSģ���Դ */
#define GPRS_POW_OFF		  GPIO_SetBits(GPIOF,GPIO_Pin_1) 		/* �ر�GPRSģ���Դ */

#define GPRS_PWRKEY_ON		GPIO_SetBits(GPIOF,GPIO_Pin_0) 		/* ��PWRKEY */
#define GPRS_PWRKEY_OFF		GPIO_ResetBits(GPIOF,GPIO_Pin_0) 	/* �ر�PWRKEY */

#define balance            Measure101Para.para.balance //��ƽ��ģʽ
#define Addr101            Measure101Para.para.addr_len //1��2��
#define Reason101          Measure101Para.para.reason_len //1����2��
#define DataFloat          Measure101Para.para.data_type
#define Ctrl_offset        Measure101Para.para.Remote_Ctrl
#define Reset_offset       Measure101Para.para.Remote_Reset
#define Max_Time  15

#define TotalLen68         (Addr101*2+Reason101-3)//68��֡��ƫ�Ƹ���
#define TotalLen10         Addr101+3//10֡��ƫ����
#define FrontReason        (Addr101-1)//����ԭ��ǰ

#define CURRENT_POSITIVE_ENERGY          0x6401      //��ǰ�����й�����
                                                     //��ǰ�����޹�����
																										 //��ǰһ�����޹�����
																										 //��ǰ�������޹�����
#define CURRENT_REVERSE_ENERGY           0x6405			 //��ǰ�����й�����
																										 //��ǰ�����޹�����
																										 //��ǰ�������޹�����
																										 //��ǰ�������޹�����
#define FREEZE_15min_POSITIVE_ENERGY     0x6409      //15���Ӷ��� �����й�����
                                                     //15���Ӷ��� �����޹�����
																										 //15���Ӷ��� һ�����޹�����
																										 //15���Ӷ��� �������޹�����
																										 //15���Ӷ��� �����й�����
																										 //15���Ӷ��� �����޹�����
																										 //15���Ӷ��� �������޹�����
																										 //15���Ӷ��� �������޹�����
#define FREEZE_day_POSITIVE_ENERGY       0x6411      //�ն��� �����й�����
                                                     //�ն��� �����޹�����
																										 //�ն��� һ�����޹�����
																										 //�ն��� �������޹�����
																										 //�ն��� �����й�����
																										 //�ն��� �����޹�����
																										 //�ն��� �������޹�����
																										 //�ն��� �������޹�����
#define FREEZE_change_POSITIVE_ENERGY    0x6419      //�����仯���� �����й�����
                                                     //�����仯���� �����޹�����
																										 //�����仯���� һ�����޹�����
																										 //�����仯���� �������޹�����
																										 //�����仯���� �����й�����
																										 //�����仯���� �����޹�����
																										 //�����仯���� �������޹�����
																										 //�����仯���� �������޹�����
#define TELEMETRY_LINELOSS_FREQ          0x4001			 //Ƶ��	
#define TELEMETRY_LINELOSS_CURRENT       0x4002			 //A�����
                                                     //B�����
                                                     //C�����
                                                     //A���ѹ
                                                     //B���ѹ
                                                     //C���ѹ
#define TELEMETRY_LINELOSS_POWER         0x4008      //A���й�����
                                                     //B���й�����
                                                     //C���й�����
                                                     //���й�����
                                                     //A���޹�����
                                                     //B���޹�����
                                                     //C���޹�����
                                                     //���޹�����
                                                     //A�����ڹ���
                                                     //B�����ڹ���
                                                     //C�����ڹ���
                                                     //�����ڹ���
                                                     //A�๦������
                                                     //B�๦������
                                                     //C�๦������
                                                     //�ܹ�������																						 

// �Ժ�̨˽��Э��ṹ��
struct BACKSTAGE_PROTOCOL_TYPE1      				// 
{
	u8 DataBuffer[850];	// ���ݽ��ջ���
};

struct CTRL_FIELD			// ������C
{
	u8 FC:4;	    // ������
	u8 FCV:1;		  // ֡������Чλ
	u8 FCB:1;	    // ֡����λ
	u8 PRM:1;		  // ������־λ
	u8 DIR:1;	    // ���䷽��λ
};
union Control_Field
{
	struct CTRL_FIELD bit;
	u8 byte;
};

struct CTRL_BITS			// ң�ؿ�����
{
	u8 QCS_RCS:2;	// ����Ϊ0��;1��  //˫��Ϊ0,3:������;1,��;2,��
	u8 QU:5;		  // 1:���������;2:������;3:�������
	u8 S_E:1;	    // ����״̬��=0Ϊִ�У�=1Ϊѡ��
};
union Control_Type
{
	struct CTRL_BITS bit;
	u8 byte;
};
struct Set_BITS
{
	u8 CONT:1;//0-�޺��� 1-�к���
	u8 RES:5;//����
	u8 CR:1;//0-δ�� 1-ȡ��Ԥ��
	u8 S_E:1;//0-�̻� 1-Ԥ��
};
union Set_Type
{
	struct Set_BITS bit;
	u8 byte;
};
union FloatToChar
{
	float f;
	u8 byte[4];
};
struct cp56time2a{
	u16 msec;
	u8 min:6;
	u8 res1:1;
	u8 iv:1;
	u8 hour:5;
	u8 res2:1;
	u8 su:1;
	u8 mday:5;
	u8 wday:3;
	u8 month:4;
	u8 res3:4;
	u8 year:7;
	u8 res4:1;
};

extern union Control_Field control_field;//control1;	
extern union Control_Type control;//control1;	
extern union Set_Type settype,Updatetype;	

extern u8 Soesend_cnt;
extern u8 List_Name[30];//Ŀ¼���� ��ʷ�洢 ��ֹ����֡����-0x1fff7a10
extern u8 List_ID[4];//Ԥ��0���
extern u8 List_NameNum;
extern u8 Doc_Name[20];
extern u8 Doc_NameNum;
extern u8 Doc_ID[4];//�ļ�ID Ԥ�� ���� 0���
extern u8 JHDoc_ID[4];//�����ļ�ID д�ļ�ʱ��Ҫ�ж�
extern u16 SN_ID;
extern u16 TestAddr;//������·ͼ��
//��־���ȼ� ��ʾ����ͼ���
extern u8 InitSign;
extern u8 InitComplete;//��ʼ������
extern u8 ControlSign;//ң�ز���
extern u8 TotalSign;//���ٻ�
extern u8 SetSign;//����
extern u8 DocSign;//�ļ�
extern u8 TestSign;//���� ��ƽ��ʹ�� ƽ�ⲻʹ��
extern u8 EnergySign;//�ٻ�����������
extern u8 ClockSign;
extern u8 ResetSign;
extern u8 DocSameSign;
extern u8 WriteDocSign;
extern u8 SoftUpSign;
//д�ļ� ��С
extern u32 DocLen;//�ļ��ܴ�С
extern u8 DocErr;//�ļ����ж�
extern u8 DocByte[240];//�ļ�����
extern u8 DocByteNum;//�ļ���С
extern u8 CheckTime_Flag;
extern u8 Clearloss_Flag;
extern u8 Clearevent_Flag;
extern u8 Rand_Flag;
extern u8 LockSet_Flag;

//������ д���� ��Ӧ����
extern u16 LockSetAddr[20];
extern u8  LockSetValue[20][24];
extern u8  LockSetType[20];
extern u8  LockSetNum[20];//ֻ����4�ֽ�һ�µĲ�������
extern u8  LockSetTotal;//�趨�ܸ���

extern struct BACKSTAGE_PROTOCOL_TYPE1 BackstageProtocol1;
extern struct BACKSTAGE_PROTOCOL_TYPE1 BackstageProtocol1bak;

#ifdef Four_Faith_MODEL
u8 ProtocolResolveEncryptionData(u8 receivedata);
#endif
#ifdef SIM800C_MODEL
// AT�����
#define SIM900B_AT			"AT"				/* ���Դ��� */
#define SIM900B_STATUS	"AT+CIPSTATUS"		/* TCP����״̬ */
#define SIM900B_IPR			"AT+IPR?"			/* ��ѯ������ */
#define SIM900B_CFUN		"AT+CFUN=1"			/* ��ͨ�Ź��� */
#define SIM900B_CLIP		"AT+CLIP=1"			/* ��ʾ������� */
#define SIM900B_ATH			"ATH"				/* �Ҷϵ绰 */
#define SIM900B_ATA			"ATA"				/* �����绰 */
#define SIM900B_CMGR		"AT+CMGR=1"			/* ��ȡ�¶��� */
#define SIM900B_CMGD		"AT+CMGD=1"			/* ɾ���ڼ������� */
#define SIM900B_CSCA		"AT+CSCA?"			/* ��ȡsms�������ĺ��� */
#define SIM900B_CIICR		"AT+CIICR"			/* �����ƶ�����*/
#define SIM900B_SHUT		"AT+CIPSHUT"		/* �ر��ƶ��������ص�IP INITIAL */
#define SIM900B_CLOSE		"AT+CIPCLOSE"		/* �ر�TCP���� */
#define SIM900B_SEND		"AT+CIPSEND="		/* ͨ��GPRS�������� */
#define SIM900B_CIFSR		"AT+CIFSR"			/* ��ñ���IP */
#define SIM900B_START		"AT+CIPSTART=\"TCP\"," /*\"122.193.249.075\",\"9560\"" */		/* ����TCP���� *///"AT+CIPSTART=\"TCP\",\"122.193.249.075\",\"9510\""
#define SIM900B_TCP			"TCP"				/* TCP���� */
#define SIM900B_UDP			"UDP"				/* UDP���� */
#define SIM900B_IP			"122.193.249.075"	/* IP��ַ */
#define SIM900B_PORT		"9510"				/* �˿ں� */
#define SIM900B_CSQ			"AT+CSQ"			/* ��ȡ�ź�ǿ�� */
#define SIM900B_CGREG1	"AT+CGREG?"			/* �Ƿ�ע������ */
#define SIM900B_ATE			"ATE0"				/* ȡ������ */
#define SIM900B_TCOUT		"+++"				/* �˳�͸��ģʽ */
#define SIM900B_ATO			"ATO"				/* ����͸��ģʽ */
#define SIM900B_CCFG		"AT+CIPCCFG=3,2,200,1" /* ����͸��ģʽ */
#define SIM900B_CMGF		"AT+CMGF=0"			/* ���ö���Ϣ��ʽΪPDU��ʽ */
#define SIM900B_CPIN		"AT+CPIN?"			/* ��ѯ�������� */
#define SIM900B_CGATT1	"AT+CGATT?"			/* �Ƿ���GPRS���� */
#define SIM900B_CGATT		"AT+CGATT=1"		/* ����GPRS���� */
#define SIM900B_CSTT1		"AT+CSTT=\"CMIOTNTGTJPDZDH.JS\""	/* ����APN */
#define SIM900B_CSTT		"AT+CSTT"	/* ����APN */
#define SIM900B_MODE		"AT+CIPMODE=1"		/* ѡ��͸��ģʽ */
#define SIM900B_CSGP		"AT+CIPCSGP=1,\"CMNET\",\"guest\",\"guest\""		/* �������ӷ�ʽ */
#define SIM900B_CIFSR		"AT+CIFSR"			/* ��ѯ����IP��ַ */
#define SIM900B_HEAD		"AT+CIPHEAD=1"		/* �������ݼ�IPͷ */
#define SIM900B_SERVER	"AT+CIPSERVER=1,2404"		/* ������ģʽ */
#define SIM900B_CIPSTATUS	"AT+CIPSTATUS" /* ���IP״̬ */
#define SIM900B_ENTER		"\r\n"				/* ��������� */
extern u16 GprsWdg;			        // GPRSģ���Զ��忴�Ź�
extern u8  IPswitchFlag;		    // IP��ַ�����л���־
u8  GprsConnectCtrl(void);
u8  ProtocolResolveGprsData(u8 receivedata);
u8  ProtocolResolveGprsState(u8 receivedata);
u8  ProtocolResolveGprsError(u8 receivedata);
#endif
#ifdef WD_MODEL
u8  ProtocolResolve1(u8 receivedata);
#endif
u8 UsartAdd(u8 *p,u8 sti,u16 zhnum);
void CommandProcess1(u8 USARTxChannel);
void CommandProcess1Prepare(void);
void GprsSendStr(char *str);
void GprsSendByte(u8 byte);
void ClearLock(void);
void ClearEvent(void);
void ReadLock(void);
void ReadEvent(void);
void SaveLock(void);
void SaveEvent(void);
void Save_LineLoss_Fix(void);
void Save_LineLoss_Rand(void);
void Save_LineLoss_Frzd(void);
void Save_LineLoss_Sharp(void);
void Save_LineLoss_Month(void);
void Save_Event_PAReverse(u8 m);
void Save_Event_PBReverse(u8 m);
void Save_Event_PCReverse(u8 m);
void Save_Event_PReverse(void); 
void Save_Event_ClearLoss(void);
void Save_Event_ClearEvent(void);
void Save_Event_CheckTime(void);
void Lock_Event(void);
void Read_Doc_Fix_Tou(u8 USARTxChannel);
void Read_Doc_Rand_Tou(u8 USARTxChannel);
void Read_Doc_Frzd_Tou(u8 USARTxChannel);
void Read_Doc_Sharp_Tou(u8 USARTxChannel);
void Read_Doc_Month_Tou(u8 USARTxChannel);
void Read_Doc_Event_Tou(u8 USARTxChannel);
void Read_Doc_Fix_OK(u8 USARTxChannel);
void Read_Doc_Rand_OK(u8 USARTxChannel);
void Read_Doc_Frzd_OK(u8 USARTxChannel);
void Read_Doc_Sharp_OK(u8 USARTxChannel);
void Read_Doc_Month_OK(u8 USARTxChannel);
void Read_Doc_Event_OK(u8 USARTxChannel);
void Read_Doc_Fix_Data(u8 USARTxChannel);
void Read_Doc_Rand_Data(u8 USARTxChannel);
void Read_Doc_Frzd_Data(u8 USARTxChannel);
void Read_Doc_Sharp_Data(u8 USARTxChannel);
void Read_Doc_Month_Data(u8 USARTxChannel);
void Read_Doc_Event_Data(u8 USARTxChannel);
void Read_Doc_Same(u8 USARTxChannel);
void ReadAllLossSave(void);
void Write_Loss_SetGHDeal(u8 USARTxChannel);
void Write_Loss_SetDeal(u8 USARTxChannel);
void Read_Loss_Set(u8 USARTxChannel);
void Read_Loss_SetDeal(u8 USARTxChnnel);
u8 ProtocolResolve1(u8 receivedata);
void Read_List_OK(u8 USARTxChannel,u8 Listn);
void Write_Loss_SN(u8 USARTxChannel);
void Read_Loss_SN(u8 USARTxChannel);
void TimeYcYx(u8 USARTxChannel);
void Read_List_Deal(void);
void Read_Doc_Deal(void);
void Read_Doc_Error(u8 USARTxChannel);
void Write_Loss_Set_YZ(u8 USARTxChannel);
void Write_Loss_Set_GH(u8 USARTxChannel,u8 reason);
void SoftUpdate(u8 USARTxChannel,u8 reason);
void Write_Doc_OK(u8 USARTxChannel);
void Write_Doc_Data_OK(u8 USARTxChannel,u8 m);//д�ļ�����ȷ��
void Write_DocOK_Deal(void);
void Write_Doc_Data_Deal(void);
void IEC101_Staid(u8 linkcontrol,u8 USARTxChannel);//�̶�֡	��ַ��1���ֽ�
void IEC101_Staid_All(u8 USARTxChannel,u8 Reason);//���ٻ�ȷ��	��ַ��1���ֽ�	
void Test_101(u8 USARTxChannel);
void IEC101_Staid_Enegy(u8 number,u8 USARTxChannel,u8 Reason);//������ٻ��������������ȷ��֡	��ַ��1���ֽ�
void IEC101_Enegy_Send(u8 USARTxChannel);//�����������������	��ַ��1���ֽ�
void Time_Synchronization(u8 USARTxChannel,u8 reason);
void Read_RealData(u8 USARTxChannel);
void Read_YX(u8 USARTxChannel);//���ٻ�ң��
void Remote_Control(u8 USARTxChannel,u8 reason,u8 TI,u16 controladdr);
void Record_SOE_Send(u8 USARTxChannel);	
void InitEnd(u8 USARTxChannel);
void ResetLianlu(u8 USARTxChannel);
void InsertValue(u8 k); //д 
void SaveLockSet(void);
void SofeReset(void);
void GetAddr(void);
u8 SearchList(void);
u8 SearchDoc(void);
u8 H2SJ(u8 USARTxChannel);
#endif
