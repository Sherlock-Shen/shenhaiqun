// 2017��03��28��Ӳ���汾 10kV�������ܶ�·��ǰ����APP����	   ����ʼ��ַ0x8010000  ����0x80000 	��λ��ַ0x8010000 �ϼ�512K
/* Includes ------------------------------------------------------------------*/
#include "vision.h"
#include "Encryption.h"
#include "GPS.h"
#include "protocol1.h"
#include "protocol2.h"
#include "protocol3.h"
#include "protocol4.h"
#include "protocol5.h"
#include "protocol6.h"
#include "2812_FFT.h"
#include "ATT7022.h"
#include "control.h"
#include "AD7606.h" 
#include "ds1302.h"
#include "global.h"
#include "queue.h"
#include "flash.h"
#include "math.h"
#include "spi.h"
#include "sysinit.h"
#include "stm32f4xx.h"

#define ApplicationAddress    0x8000000
#define ATT7022_K		          1495       //K=2.592*10^10/(HFconst*EC*2^23)=2.592*10^10/(231*20000*2^23)= 0.00066881
#define ATT7022_EPTK	        0.05       // 1/20wh
#define CV_BAT                111
typedef  void (*pFunction)(void);
union Lineloss1 LineSet;
union REVERDefine PReverse;
struct LineLossEvent LineLossNum;
struct LineLockEvent LineLockNum;
/* Public variables -------------------------------------------------------------*/
s16 PcurveUA[1200];	// A�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
s16 PcurveUB[1200];	// B�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
s16 PcurveUC[1200];	// C�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
s16 PcurveIA[1200];	// A�ౣ����������¼������������ǰ4�����ڣ�����������8������
s16 PcurveIB[1200];	// B�ౣ����������¼������������ǰ4�����ڣ�����������8������
s16 PcurveIC[1200];	// C�ౣ����������¼������������ǰ4�����ڣ�����������8������
s16 PcurveU0[1200];	// ���򱣻���ѹ����¼������������ǰ4�����ڣ�����������8������
s16 PcurveI0[1200];	// ���򱣻���������¼������������ǰ4�����ڣ�����������8������

/*******************************ATT7022E��ȡ����ֵ*********************************/
s32 ATT7022_UaRms,ATT7022_UbRms,ATT7022_UcRms;	// ��ȱ�оƬ ��ѹ��Чֵ
s32 ATT7022_IaRms,ATT7022_IbRms,ATT7022_IcRms;	// ��ȱ�оƬ ������Чֵ
s32 ATT7022_Pa,ATT7022_Pb,ATT7022_Pc;			      // ��ȱ�оƬ �����й�
s32 ATT7022_Qa,ATT7022_Qb,ATT7022_Qc;			      // ��ȱ�оƬ �����޹�
s32 ATT7022_Sa,ATT7022_Sb,ATT7022_Sc,ATT7022_St;// ��ȱ�оƬ �������ڹ���
s32 ATT7022_Pfa,ATT7022_Pfb,ATT7022_Pfc;		    // ��ȱ�оƬ ���๦������
s32 ATT7022_Pt,ATT7022_Qt,ATT7022_Pft;			    // �й����޹����������� 
s32 ATT7022_Ept,ATT7022_Eqt,ATT7022_Est;			  // �й����ܣ��޹����ܣ����ڵ���
u32 ATT7022_Freq;
/*******************************ATT7022E��ȡУ�����*********************************/
u32 ZYXSA,ZYXSB,ZYXSC;	// �й�����ϵ��
u32 XWXSA,XWXSB,XWXSC;	// ��λ����ϵ��
u32 WGXSA,WGXSB,WGXSC;	// �޹�����ϵ��
u32 DYXSA,DYXSB,DYXSC;	// ��ѹ����ϵ��
u32 DLXSA,DLXSB,DLXSC;	// ��������ϵ��
u32 DYPYA,DYPYB,DYPYC;	// ��ѹ����ϵ��
u32 DLPYA,DLPYB,DLPYC;	// ��������ϵ��
u32 DYXSA1,DYXSB1,DYXSC1;	// ��ѹ���У׼
u32 DLXSA1,DLXSB1,DLXSC1;	// �������У׼
u32 ATT7022_sum = 0;
u8 saveATT7022_flag = 0;
u8 Start_Debug = 0;
u8 TESTFlag = 0;
u8 GPS_Flag = 0;
u8 TestFlagCnt = 0;
u8 Battery_Readflag = 0;
u8 MachineInformation[13];		// ���������������(2)��(2)�������(7)
u8 Send_Flag = 0;
u8 timer_renzheng=0;

u16  Para_peak1_period_S = 0;		// ����ͳ��ʱ������
u16  Para_peak1_period_E = 0;
u16  Para_peak2_period_S = 0;
u16  Para_peak2_period_E = 0;
u16  Para_valley_period_S = 0;
u16  Para_valley_period_E = 0;

s16 AD_IA_val = 0;               
s16 AD_IB_val = 0;
s16 AD_IC_val = 0;
s16 AD_PIA_val = 0;
s16 AD_PIB_val = 0;
s16 AD_PIC_val = 0;
s16 AD_U0_val = 0;
s16 AD_I0_val = 0;

u32 WpLast = 0;			        // ��һ�ε����й�����
u32 WqLast = 0;			        // ��һ�ε����޹�
u32 Wp_all_Last = 0;		    // ��һ�����й�����
u32 WpFX_all_Last = 0;	    // ��һ���ܷ����й�����
u32 Wp_peak_Last = 0;		    // ��һ�η��й�����
u32 WpFX_peak_Last = 0;	    // ��һ�η巴���й�����
u32 Wp_valley_Last = 0;	    // ��һ�ι��й�����
u32 WpFX_valley_Last = 0;	  // ��һ�ιȷ����й�����
u32 Wp_level_Last = 0;	    // ��һ��ƽ�й�����
u32 WpFX_level_Last = 0;	  // ��һ��ƽ�����й�����

u64 Wq_1all_Last = 0;		    // ��һ��I�������޹�
u64 Wq_2all_Last = 0;		    // ��һ��II�������޹�
u64 Wq_3all_Last = 0;		    // ��һ��III�������޹�
u64 Wq_4all_Last = 0;		    // ��һ��IV�������޹�

u8 BreakerStateLast = 0;	  // ��һ�ο���λ��״̬
u16 PowerOffCnt = 0;			  // ���������
u8 YabanSave_Flag = 0;			// ѹ�屣���־
u16 BattryCnt = 0;
u8 BattryTimeOut = 0;
u8 ParaInitReadyFlag = 0;		  // ������ʼ��׼����

u8 MeasureIAError = 0;			  // A��������ϱ�־
u8 MeasureIBError = 0;			  // B��������ϱ�־
u8 MeasureICError = 0;			  // C��������ϱ�־
u16 ZeroPointIA = 0;				  // һ�������������	
u16 ZeroPointIA10 = 0;				// ʮ�������������	
u16 ZeroPointIB = 0;				  // һ�������������	
u16 ZeroPointIB10 = 0;				// ʮ�������������	
u16 ZeroPointIC = 0;				  // һ�������������	
u16 ZeroPointIC10 = 0;				// ʮ�������������	

u8 AlarmRecord_flag = 0;
u16 SwitchCnt = 0;            // ��������
u16 SelfCheckErrCode = 0;		  // �Լ���ϴ���
u8 SwitchFlag = 0;				    // ����״̬
u8 StartCntFlag = 0;          // ��բ������־
u8 ReOnFlag = 0;					    // �غ�բ��־
u8 ReOnTimeOut = 0;					  // �غ�բʱ�䵽���־
u8 ReOnLockCnt = 10;				  // �غ�բ����ʱ��10��
u8 ReOnLockCnt1 = 0;				  // ��բ���غ�բ���ʱ��15��
u16 ReOnTimeDelay = 0;				// �غ�բ��ʱ10ms
u16 ReOnDelay = 0;					  // �غ�բ��ʱʱ��
u8 ReOnTimes = 0;	            // �غ�բ����
u8 DLQZD_flag = 0;				    // ��·�������Ϣ��־
u8 ProtectFlag = 0;           // ������־
u16 ProtectTimesLock = 0;	    // ����������������բλ��ֻ����һ�Σ���բ�������һ�α���
u8 ProtectTimesLockCnt = 0;   // ����ʱ���������
u16 AlarmTimesLock = 0;	      // ����������������բλ��ֻ����һ�Σ���բ�������һ�α���
u8 AlarmTimesLockCnt = 0;		  // ����ʱ���������
u8 Switchon_Lock = 0;	        // ��բ����
u8 Switchon_Lock1 = 0;	      // ��������ֹ��բ
u8 Switchon_Lock2 = 0;	      // ��ѹ������բ
u8 Switchon_Lock2_Flag = 0;
u8 Switchoff_Lock = 0;        // ��բ����
u8 Switchoff_Lock1 = 0;        // ��բ����1
u8 SigresetLock = 0;          // ���������ʱ��ʼ��־
u8 AD_Index = 0;		          // AD�������
u16 AD_StartPoint = 0;       // AD�����ٽ��
u8 ProtectRecord_flag = 0;		// ������¼��־�����ڼ�¼�����������Ƿ�������
u16 Max_current_after_cnt = 5000;       // ��������ټ�ʱ
u8 PTDX_Flag = 0;				                // PT���߱�־��1=����
u8 CTDX_Flag = 0;				                // CT���߱�־��1=����
u8 PTDX_Enable = 0;				              // PT���߼������1=����
u8 CTDX_Enable = 0;				              // CT���߼������1=����
u8 MeasureReady = 0;                    // ������ɱ�־
u8  GpsOn_Flag = 0;
u16 qualif_volt = 1000;                 // ��ѹ�ϸ���
u16 CtrlMeasureCnt = 0;	                // ���Ʋ�������
u8 CtrlMeasureFlag = 0;	                // ���Ʋ�����־
u8 AUTHORIZATIONcode[16]= {0};		//��Ȩ��
u8 ClearTJXX_Flag = 0;
// DMA���ͻ���
u8 DMATXbuffer1[300]= {0}; 
u8 DMATXbuffer2[820]= {0}; 
u8 DMATXbuffer3[100]= {0}; 
u8 DMATXbuffer4[300]= {0};
u8 DMATXbuffer5[420]= {0};
u8 DMATXbuffer6[100]= {0};
// DMA���ջ���

u8 DMARXbuffer1[50] = {0};
u8 DMARXbuffer2[50] = {0};
u8 DMARXbuffer3[50] = {0};
u8 DMARXbuffer4[50] = {0};
u8 DMARXbuffer5[50] = {0};
u8 DMARXbuffer6[50] = {0};

// DMA��������
u16 DMAADCbuffer1[100][9] = {0};

u16 SOE[16][11]={0};
// DMA��ȡ��ǰָ��
u16 DMAReadIndex1 = 0;
u16 DMAReadIndex2 = 0;
u16 DMAReadIndex3 = 0;
u16 DMAReadIndex4 = 0;
u16 DMAReadIndex5 = 0;
u16 DMAReadIndex6 = 0;

u8 Usart1RxReady = 0;// ����1������ɱ�־
u8 Usart1bakRxReady = 0;// ����1������ɱ�־
u8 Usart2RxReady = 0;// ����2������ɱ�־
u8 Usart2bakRxReady = 0;// ����2������ɱ�־
u8 Usart3RxReady = 0;// ����3������ɱ�־
u8 Usart3bakRxReady = 0;// ����3������ɱ�־
u8 Usart4RxReady = 0;// ����4������ɱ�־
u8 Usart4bakRxReady = 0;// ����4������ɱ�־
u8 Usart5RxReady = 0;// ����5������ɱ�־
u8 Usart5bakRxReady = 0;// ����5������ɱ�־
u8 Usart6RxReady = 0;// ����6������ɱ�־
u8 Usart6bakRxReady = 0;// ����6������ɱ�־

u8 Usart2TxReady = 0;// ����2׼�����ͱ�־
u8 timer_enable=0;	  //ʹ�ܼ�ʱ��
u8 delay_time_101=0;
// SOEʹ�ܱ�־ ����
const u8 SOE_Enable[20]=
{
	0xFF, 	// ��ַ���ұߵĵ�λ��ʼ 8  7  6  5  4  3  2  1 
	0xFF, 	// 		                  16 15 14 13 12 11 10 9   
	0xFF, 	// 	                    24 23 22 21 20 19 18 17
	0xFF, 	//	                    32 31 30 29 28 27 26 25
	0xFF, 	//	                    40 39 38 37 36 35 34 33	
	0xFF, 	//	                    48 47 46 45 44 43 42 41	
	0xFF, 	//	                    56 55 54 53 52 51 50 49		
	0x00, 			
	0x00, 		
	0x00, 		
	0x00, 		
	0x00, 		
	0x00, 	
	0x00, 	
	0x00, 	
	0x00, 	
	0x00, 	
	0x00, 	
	0x00, 	
	0x00 	  
};
u8 RealCurveFlag = 0;
u8 SOE_Flag = 0;
u8 SOEPad_Flag = 0;
u8 ModulusFlag =0;
u8 SetSystemFlag =0;
u8 SetProtectFlag =0;
u8 SetAlarmFlag =0;
u8 SetAutoswitchFlag =0;
u8 Set101ParaFlag = 0;
u8 SigresetFlag =0;
u8 OperateNumber=0;
u8 GroundNumber=0;
u8 AlarmNumber=0;
u8 ProtectNumber=0;
u8 ZeroTimeArrived = 0;
u8 KZQUpdataSucced=0;
u8 RecoverKZQBackupFlag = 0;
u8 Select101Addr = 0;	// Э����� ����ѡ����Ȧ��ַ
u8 EVENT_101Flag = 0;		  // 101Э���¼���־
u8 WifiConnectState = 0;
u8 ControlLock = 0;
u16 DLQ_EventRecord_Index;
u16 DLQ_EventRecord_Order[500];
u16 DLQ_SOERecord_Index;
u16 DLQ_SOERecord_Order[300];
u8 Battery_Voltage_Index;
u16 Battery_Voltage_Order[120];
u8 Curve_Index;
u16 Curve_Order[100];
u8 SendLock_Cnt = 0;
u16 Battery_RunTime = 0; // �������ʱ��
u16 Control_Hold = 0;
u16 CodeTimeout = 0;
s16 Temp_value,Humi_value,status;
u32 Lock_IdCode = 0;
u32 Reset_Value = 0;
u16 JudgePoint = 0;
u8 JudgePoint_Flag = 0;
u8 HumiTest_Flag = 0;
u16 Freez_15min_cnt = 900;

struct PhaseMode MyCurve;
//struct PhaseMode1 MyCurve1;
struct Measure          MeasureData;				 // ����ֵ
struct Measure1         MeasureCurve;		     // ��������
struct time_now         TimeNow;		 // ��ǰʱ��
struct UpdateVersion    MyVersion;	         // ���г���ͱ��ݳ���汾���޸�ʱ��
union  DLQZDXX          DLQZDinformation;
union  PROTECT_DELAY1   ProtectDelay;	       // ������ʱ��ʱ
union  PROTECT_TIMEOUT1 ProtectTimeout;	     // ����ʱ�䵽���־
union  ALARM_DELAY1     AlarmDelay;          // ������ʱ��ʱ
union  ALARM_TIMEOUT1   AlarmTimeout;        // ����ʱ�䵽���־
union  KZQMEASUREDATA   KZQMeasureData;	     // ��·��ʵʱ����
union  RECORD_ACTION    Record_on;	         // ��բ��¼
union  RECORD_ACTION    Record_off;	         // ��բ��¼
union  RECORD_ACTION    Record_protect1;	   // ������¼
union  RECORD_ACTION    Record_protect2;	   // ������¼
union  RECORD_ACTION    Record_alarm;	  	   // ������¼
union  RECORD_ACTION    Record_autofeed;     // �����Զ�����¼
union  SYSTEM_PARA1     SystemSet;	         // ϵͳ��������
union  PROTECT_PARA1    ProtectSet;	         // ������������
union  PROTECT_PARA1    ProtectSetReceive;
union  ALARM_PARA1      AlarmSet;	           // ������������
union  AUTO_SWITCHOFF1  AutoswitchSet;	     // �Զ����в�������
union  Modulus1         ProtectModulus;      // ��������ϵ��
union  Rule101Para1     Measure101Para;      // 101��Լ����

union YXWORDDefine      YXChange;	      // ң�ű�λ
union YXWORDDefine      YXChange1;    	// ң�ű�λ
union YXWORDDefine      YXChange2;	    // ң�ű�λ
union YXWORDDefine      YXChangeHold;   // ң�ű�λ����

union YXDefine          lockbz;

u8  YXChangeShake[16];        // ң�ű�λ����
u8	LED_State = 0;
u8	Led_Reverse_Flag = 0;
u8  Last_Breakerstate = 0;

struct ControlFlag{
	  u8 in4:1;	
		u8 in5:1;
		u8 rsv0:1;
		u8 rsv1:1;
		u8 rsv2:1;
		u8 rsv3:1;
		u8 flag1:1;
		u8 flag2:1;
};
union ControlState		
{
	struct ControlFlag bit;
	u8 word;
};
union ControlState Control;	

/* Private variables ---------------------------------------------------------*/
extern GPIO_InitTypeDef GPIO_InitStructure;
extern USART_InitTypeDef USART_InitStructure;
ErrorStatus HSEStartUpStatus;
volatile FLASH_Status FLASHStatus = FLASH_BUSY;

/* Private function prototypes -----------------------------------------------*/
void Delay(u32 nCount);

void Task1(void);
void Task2(void);
void Task3(void);
void Task4(void);
void Task5(void);
void Task6(void);
void Task7(void);
void Task8(void);
void Task9(void);
void Task10(void);
void Task11(void);
void Task12(void);
void Task13(void);
void Task14(void);
void Task15(void);
void Task16(void);
void Task17(void);

OS_TID t_Task1;                        /* assigned task id of task: Task1  */
static U64 stk1[2048/8];
OS_TID t_Task2;                        /* assigned task id of task: Task2  */
static U64 stk2[2048/8];
OS_TID t_Task3;                        /* assigned task id of task: Task3  */
static U64 stk3[2048/8];
OS_TID t_Task4;                        /* assigned task id of task: Task4  */
static U64 stk4[2048/8];
OS_TID t_Task5;                        /* assigned task id of task: Task5  */
static U64 stk5[2048/8];
OS_TID t_Task6;                        /* assigned task id of task: Task6  */
static U64 stk6[2048/8];
OS_TID t_Task7;                        /* assigned task id of task: Task7  */
static U64 stk7[2048/8];
OS_TID t_Task8;                        /* assigned task id of task: Task8  */
static U64 stk8[2048/8];
OS_TID t_Task9;                        /* assigned task id of task: Task9  */
static U64 stk9[2048/8];
OS_TID t_Task10;                       /* assigned task id of task: Task10 */
static U64 stk10[2048/8];
OS_TID t_Task11;                       /* assigned task id of task: Task11 */
static U64 stk11[2048/8];
OS_TID t_Task12;                       /* assigned task id of task: Task12 */
static U64 stk12[2048/8];
OS_TID t_Task13;                       /* assigned task id of task: Task13 */
static U64 stk13[2048/8];
OS_TID t_Task14;                       /* assigned task id of task: Task14 */
static U64 stk14[2048/8];
OS_TID t_Task15;                       /* assigned task id of task: Task15 */
static U64 stk15[2048/8];
OS_TID t_Task16;                       /* assigned task id of task: Task16 */
static U64 stk16[2048/8];
OS_TID t_Task17;                       /* assigned task id of task: Task17 */
static U64 stk17[2048/8];

/**************************************************************************************
* FunctionName   : TaskRemarks()
* Description    : �����־����
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TaskRemarks(void)
{
	TimeNow.msec++;
	if(TimeNow.msec>999)
	  TimeNow.msec = 0;
	else
	{
		if (TimeNow.second==59 && TimeNow.minute==59 && TimeNow.hour==23)
    {
      ZeroTimeArrived = 1;
    }
		else if(TimeNow.second==59 && TimeNow.minute==59 && TimeNow.hour==11)
    {	
      ZeroTimeArrived = 2;
	  }		
	}
}

void delay_us(u32 us)
{
	u32 time;
	us *= 168;
	time = DWT->CYCCNT;
	while((DWT->CYCCNT - time) < us);
}

void USART1_Configuration(long baudrate)
{
  USART_InitStructure.USART_BaudRate = baudrate;               /*���ò�����*/
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*��������λΪ8*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*����ֹͣλΪ1λ*/
  USART_InitStructure.USART_Parity = USART_Parity_No;        /*����żУ��*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*��Ӳ������*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*���ͺͽ���*/
  /*���ô���1  */
  USART_Init(USART1, &USART_InitStructure);		 
}

//static u8 DHT11_ReadValue(void)
//{
//	u8 value=0,i;
//	u16 count;
//	status=SUCCESS;//�趨��־Ϊ����״̬
//	for(i=8;i>0;i--)
//	{
//		value<<=1;		//��λ����
//		count=0;		//ÿһλ����ǰ����һ��50US�ĵ͵�ƽ����
//		while(HTDATA_IN==0&&count++<2200);
//		if(count>=2200)
//		{
//			status=ERROR;		 //�趨�����־
//			return 0;			 //����ִ�й��̷���������˳�����
//		}
//		Delay(1250);		//26-28US�ߵ�ƽ��ʾ��λ��0��Ϊ70US�ߵ�ƽ��ʾ��λΪ1
//		if(HTDATA_IN!=0)
//		{				         //���������ʾ��λ��1
//			count=0;
//			while(HTDATA_IN!=0&&count++<1900);//�ȴ�ʣ��40US�ĸߵ�ƽ
//			value++;
//		 }
//	}
//	return(value);
//}

////���¶Ⱥ�ʪ�Ⱥ�������һ�ε����ݣ������ֽڣ������ɹ���������OK�����󷵻�ERROR
//u8 DHT11_ReadTempAndHumi(void)
//{
//	u8 i=0,check_value=0,value_array[5];
//	u16 count=0;
//	HTDATA_OUT_0;//���������ߴ���18ms���Ϳ�ʼ�ź�   
//	os_dly_wait(1); //�����800΢��	   
//	HTDATA_OUT_1;  //�ͷ������ߣ����ڼ��͵�ƽ��Ӧ���ź�
//	//��ʱ20-40us,�ȴ�һ��ʱ�����Ӧ���źţ�Ӧ���ź��Ǵӻ�����������80US
////   os_tmr_create(10,1);  //10��ʱ�ӽ��ģ�һ��ʱ�ӽ���Ϊ1ms
//// 	while(usdelay_flag==0);
//// 	usdelay_flag = 0;
//	Delay(1500);//480����40us
//	if(HTDATA_IN!=0)
//	{
//	  return 0;//ûӦ���ź�
//	}
//	else
//	{
//		//��Ӧ���ź�
//		while(HTDATA_IN==0&&count++<3500);//�ȴ�Ӧ���źŽ���
//		if(count>3500)//���������Ƿ񳬹����趨�ķ�Χ
//		{	
//			HTDATA_OUT_1;  //�ͷ�������
//			return 0;//�˳�����
//		}
//	  count=0;
//		HTDATA_OUT_1;//�ͷ�������
//		while(HTDATA_IN!=0&&count++<3500);//Ӧ���źź����һ��80US�ĸߵ�ƽ����
//		if(count>3500)//���������Ƿ񳬹����趨�ķ�Χ
//		{
//			HTDATA_OUT_1;
//			return 0;//�˳�����
//		}
//		//����ʪ���¶�ֵ
//		for(i=0;i<5;i++)
//		{	
//			value_array[i]=DHT11_ReadValue();
//			if(status==ERROR)//����ReadValue()�����ݳ�����趨statusΪERROR
//			{
//			  HTDATA_OUT_1;  //�ͷ�������
//				return 0;
//			}
//			//���������һ��ֵ��У��ֵ�������ȥ
//			if(i!=4)
//			{
//				//����������ֽ������е�ǰ���ֽ����ݺ͵��ڵ����ֽ����ݱ�ʾ�ɹ�
//				check_value+=value_array[i];
//			}
//		}
//		//��û�÷�����������ʧ��ʱ����У��
//		if(check_value==value_array[4])
//		{
//		  Humi_value=(value_array[0]<<8)+value_array[1];
//		  Temp_value=(value_array[2]<<8)+value_array[3];
//			//if(Temp_value & 0x8000)
//			//  Temp_value = -(Temp_value & 0x7FFF);
//		  HTDATA_OUT_1;  //�ͷ�������
//		  return 1 ;   //��ȷ�� ����dht11���������
//		}
//		 else
//		{
//			//У�����ݳ���
//			return 0;
//		}
//	}
//}

//void ReadHumi_Send(u8 address_data)
//{
//	u8 i;
//	for(i=0;i<8;i++)
//	{
//		HTCLK_OUT_0;
//		delay_us(5);
//		HTCLK_OUT_1;
//		if((address_data >> (7-i))&0x01)
//		{
//			HTDATA_OUT_1;
//		}
//		else
//		{
//			HTDATA_OUT_0;
//		}
//		delay_us(5);
//	}
//	HTCLK_OUT_0;
//	delay_us(5);
//	HTCLK_OUT_1;  //ACKʱ��
//	delay_us(5);
//}

//static u8 ReadHumi_Data(void)
//{
//	u8 i;
//	u8 data;
//	for(i=0;i<8;i++)
//	{
//		HTCLK_OUT_0;
//		delay_us(5);
//		HTCLK_OUT_1;
//		delay_us(5);
//		data = data<<1;
//		if(HTDATA_IN == 1)
//		  data |= 0x01;
//	}
//	HTCLK_OUT_0;
//	delay_us(5);
//	HTCLK_OUT_1;  //ACKʱ��
//	HTDATA_OUT_0; //��Ӧ����������
//	delay_us(5);
//	return(data);
//}

////���¶Ⱥ�ʪ�Ⱥ����������ɹ���������OK�����󷵻�ERROR
//u8 DHT11_ReadTempAndHumi1(void)
//{
//	u8 value_array[4];
//	//���Ѵ�����
//	HTCLK_OUT_1;
//	HTDATA_OUT_1;
//	Delay(3);
//	HTDATA_OUT_0;        //��ʼ�ź�
//	delay_us(5);
//  ReadHumi_Send(0xB8);
//	HTDATA_OUT_0; //����ʱ�����������ӦACK����������
//	HTCLK_OUT_0;
//	os_dly_wait(1); //�����800΢��	   
//	HTCLK_OUT_1;
//	delay_us(5);
//	HTDATA_OUT_1;   //ֹͣ�ź�
//	os_dly_wait(10);
//	
//	//���Ͷ�ָ��
//	HTDATA_OUT_0;        //��ʼ�ź�
//	delay_us(5);
//	ReadHumi_Send(0xB8);  //��ַ
//	if(HTDATA_IN == 1)
//	  return 0;
//	delay_us(35);
//	ReadHumi_Send(0x03);  //������
//	if(HTDATA_IN == 1)
//	  return 0;
//	ReadHumi_Send(0x00);  //��ʼ��ַ
//	if(HTDATA_IN == 1)
//	  return 0;
//	ReadHumi_Send(0x04);  //�Ĵ�������
//	if(HTDATA_IN == 1)
//	  return 0;
//	HTDATA_OUT_1;   //ֹͣ�ź�
//	
//	//��������ʪ������
//	os_dly_wait(2);  //���Ͷ�ָ���������ȴ�����1.5ms���ٷ��Ͷ�ȡʱ��
//	HTDATA_OUT_0;        //��ʼ�ź�
//	delay_us(5);
//	ReadHumi_Send(0xB8);  //��ַ
//	if(HTDATA_IN == 1)
//	  return 0;
//	delay_us(35);
//	ReadHumi_Data();  //���ع�����0x03
//	ReadHumi_Data();  //�������ݳ���0x04
//	value_array[0] = ReadHumi_Data();//ʪ�ȸ�λ
//	value_array[1] = ReadHumi_Data();//ʪ�ȵ�λ
//	value_array[2] = ReadHumi_Data();//�¶ȸ�λ
//	value_array[3] = ReadHumi_Data();//�¶ȵ�λ
//	Humi_value=(value_array[0]<<8)+value_array[1];
//	Temp_value=(value_array[2]<<8)+value_array[3];
//	return 1 ;
//}

u8 Humi_Handle_Step(void)
{
	static u8 value_array[4] = {0};
	static u8 Step_Value = 0;
	static u8 timecnt = 0;
	static u8 address_data;
	static u8 i = 0;
	static u8 function = 0;
	static u8 datanum = 0;
	switch(Step_Value)
	{
		case 0:
		  //���Ѵ�����
			HTCLK_OUT_1;
			HTDATA_OUT_1;
		  Step_Value = 1;
		  break;
		case 1:
			HTDATA_OUT_0;        //��ʼ�ź�
		  address_data = 0xB8;
		  i=0;
		  Step_Value = 2;
		  break;
		case 2:
			HTCLK_OUT_0;
		  if((address_data >> (7-i))&0x01)
			{
				HTDATA_OUT_1;
			}
			else
			{
				HTDATA_OUT_0;
			}
		  Step_Value = 3;
		  break;
		case 3:
			HTCLK_OUT_1;
		  if(i < 7)
			{
				i++;
				Step_Value = 2;
			}
			else
			{
				Step_Value = 4;
			}
		  break;
		case 4:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 5;
		  break;
		case 5:
			HTCLK_OUT_1;  //ACKʱ��
		  Step_Value = 6;
		  break;
		case 6:
			HTCLK_OUT_0;  //�����800΢��	  
		  //Delay(2);
		  HTDATA_OUT_0;
      timecnt = 5;		
		  Step_Value = 7;
		  break;
		case 7:
      timecnt--;
		  if(timecnt == 0)
		    Step_Value = 8;
		  break;
		case 8:
			HTCLK_OUT_1;
		  Step_Value = 9;
		  break;
		case 9:
			HTDATA_OUT_1;   //ֹͣ�ź�	
		  Step_Value = 10;
		  break;
		//���Ͷ�ָ��
		case 10:
			HTDATA_OUT_0;        //��ʼ�ź�
		  address_data = 0xB8; //��ַ
		  i=0;
		  Step_Value = 11;
		  break;
		case 11:
			HTCLK_OUT_0;
		  //Delay(2);
		  if((address_data >> (7-i))&0x01)
			{
				HTDATA_OUT_1;
			}
			else
			{
				HTDATA_OUT_0;
			}
		  Step_Value = 12;
		  break;
		case 12:
			HTCLK_OUT_1;
		  if(i < 7)
			{
				i++;
				Step_Value = 11;
			}
			else
			{
				Step_Value = 13;
			}
		  break;
		case 13:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 14;
		  break;
		case 14:
			HTCLK_OUT_1;  //ACKʱ��
		  if(HTDATA_IN == 1)
			{
        Step_Value = 0;
				return 0;
			}
			else
			{
				Step_Value = 15;
			  address_data = 0x03;  //������
		    i=0;
			}
		  break;
		case 15:
			HTCLK_OUT_0;
		  //Delay(2);
		  if((address_data >> (7-i))&0x01)
			{
				HTDATA_OUT_1;
			}
			else
			{
				HTDATA_OUT_0;
			}
		  Step_Value = 16;
		  break;
		case 16:
			HTCLK_OUT_1;
		  if(i < 7)
			{
				i++;
				Step_Value = 15;
			}
			else
			{
				Step_Value = 17;
			}
		  break;
		case 17:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 18;
		  break;
		case 18:
			HTCLK_OUT_1;  //ACKʱ��
		  //Delay(2);
		  if(HTDATA_IN == 1)
      {
        Step_Value = 0;
				return 0;
			}
			else
			{
				Step_Value = 19;
			  address_data = 0x00;  //��ʼ��ַ
		    i=0;
			}
		  break;
    case 19:
			HTCLK_OUT_0;
		  //Delay(2);
		  if((address_data >> (7-i))&0x01)
			{
				HTDATA_OUT_1;
			}
			else
			{
				HTDATA_OUT_0;
			}
		  Step_Value = 20;
		  break;
		case 20:
			HTCLK_OUT_1;
		  if(i < 7)
			{
				i++;
				Step_Value = 19;
			}
			else
			{
				Step_Value = 21;
			}
		  break;
		case 21:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 22;
		  break;
		case 22:
			HTCLK_OUT_1;  //ACKʱ��
		  //Delay(2);
		  if(HTDATA_IN == 1)
      {
        Step_Value = 0;
				return 0;
			}
			else
			{
				Step_Value = 23;
			  address_data = 0x04;  //�Ĵ�������
		    i=0;
			}
		  break;	
    case 23:
			HTCLK_OUT_0;
		  //Delay(2);
		  if((address_data >> (7-i))&0x01)
			{
				HTDATA_OUT_1;
			}
			else
			{
				HTDATA_OUT_0;
			}
		  Step_Value = 24;
		  break;
		case 24:
			HTCLK_OUT_1;
		  if(i < 7)
			{
				i++;
				Step_Value = 23;
			}
			else
			{
				Step_Value = 25;
			}
		  break;
		case 25:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 26;
		  break;
		case 26:
			HTCLK_OUT_1;  //ACKʱ��
		  //Delay(2);
		  if(HTDATA_IN == 1)
      {
        Step_Value = 0;
				return 0;
			}
			else
			  Step_Value = 27;
		  break;
		case 27:	
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_0;  
		  Step_Value = 28;
		  break;
		case 28:	
			HTCLK_OUT_1;
		  Step_Value = 29;
		  break;
    case 29:
			HTDATA_OUT_1;   //ֹͣ�ź�
		  Step_Value = 30;
		  break;
		case 30:
			HTCLK_OUT_0;   //�ȴ�>1.5ms
		  timecnt = 10;		
		  Step_Value = 31;
		  break;
		case 31:
			timecnt--;
		  if(timecnt == 0)
		    Step_Value = 32;
		  break;
		case 32:
			HTDATA_OUT_1;
      HTCLK_OUT_1;  //�ָ�ʱ��
		  Step_Value = 33;
		  break;			
    //��������ʪ������	
    case 33:
      HTDATA_OUT_0;        //��ʼ�ź�
		  address_data = 0xB9; //��ַ
		  i=0;
		  Step_Value = 34;
		  break;
		case 34:
			HTCLK_OUT_0;
		  //Delay(2);
		  if((address_data >> (7-i))&0x01)
			{
				HTDATA_OUT_1;
			}
			else
			{
				HTDATA_OUT_0;
			}
		  Step_Value = 35;
		  break;
		case 35:
			HTCLK_OUT_1;
		  if(i < 7)
			{
				i++;
				Step_Value = 34;
			}
			else
			{
				Step_Value = 36;
			}
		  break;
		case 36:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 37;
		  break;
		case 37:
			HTCLK_OUT_1;  //ACKʱ��
		  //Delay(2);
		  if(HTDATA_IN == 1)
      {
        Step_Value = 0;
				return 0;
			}
			else
			{
				Step_Value = 38;
				function = 0;
		    i=0;
			}
		  break;
    case 38:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 39;
		  break;
		case 39:
			HTCLK_OUT_1;
		  Step_Value = 40;
		  break;
		case 40:
			if(HTDATA_IN == 1)
		    function |= 0x01;
			HTCLK_OUT_0;
			if(i < 7)
			{
				i++;
				Step_Value = 39;
			}
			else
			{
				if(function == 0x03)
				{
					Step_Value = 41;
					HTDATA_OUT_0; //��Ӧ����������
				}
				else
				{
					Step_Value = 0;
				  return 0;
				}
			}
			function =  function<<1;  //������
		  break;
	  case 41:
			HTDATA_OUT_0; //��Ӧ����������
      HTCLK_OUT_1;  //ACKʱ��
		  Step_Value = 42;
		  datanum = 0;
		  i=0;
		  break;
		case 42:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 43;
		  break;
	  case 43:
			HTCLK_OUT_1;
		  Step_Value = 44;
		  break;
	  case 44:
			if(HTDATA_IN == 1)
			  datanum |= 0x01;
			HTCLK_OUT_0;
			if(i < 7)
			{
				i++;
				Step_Value = 43;
			}
			else
			{
        if(datanum == 0x04)
				{
				  Step_Value = 45;
				  HTDATA_OUT_0; //��Ӧ����������
				}
				else
				{
					Step_Value = 0;
				  return 0;
				}
			}
			datanum =  datanum<<1;  //���ݸ���
			break;
	  case 45:
			HTDATA_OUT_0; //��Ӧ����������
      HTCLK_OUT_1;  //ACKʱ��
		  Step_Value = 46;
		  value_array[0] = 0;
		  i=0;
		  break;
		case 46:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 47;
		  break;
		case 47:
			HTCLK_OUT_1;
		  Step_Value = 48;
		  break;
		case 48:
			if(HTDATA_IN == 1)
			  value_array[0] |= 0x01;
			HTCLK_OUT_0;
			if(i < 7)
			{
				i++;
				Step_Value = 47;
			}
			else
			{
				Step_Value = 49;
				HTDATA_OUT_0; //��Ӧ����������
				break;
			}
			value_array[0] =  value_array[0]<<1;  //ʪ�ȸ�λ
			break;
	  case 49:
			HTDATA_OUT_0; //��Ӧ����������
      HTCLK_OUT_1;  //ACKʱ��
		  Step_Value = 50;
		  value_array[1] = 0;
		  i=0;
		  break;
		case 50:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 51;
		  break;
		case 51:
			HTCLK_OUT_1;
		  Step_Value = 52;
		  break;
		case 52:
			if(HTDATA_IN == 1)
			  value_array[1] |= 0x01;
			HTCLK_OUT_0;
			if(i < 7)
			{
				i++;
				Step_Value = 51;
			}
			else
			{
				Step_Value = 53;
				HTDATA_OUT_0; //��Ӧ����������
				break;
			}
			value_array[1] =  value_array[1]<<1;  //ʪ�ȵ�λ
			break;
	  case 53:
			HTDATA_OUT_0; //��Ӧ����������
      HTCLK_OUT_1;  //ACKʱ��
		  Step_Value = 54;
		  value_array[2] = 0;
		  i=0;
		  break;
		case 54:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 55;
		  break;
		case 55:
			HTCLK_OUT_1;
		  Step_Value = 56;
		  break;
		case 56:
			if(HTDATA_IN == 1)
			  value_array[2] |= 0x01;
			HTCLK_OUT_0;
			if(i < 7)
			{
				i++;
				Step_Value = 55;
			}
			else
			{
				Step_Value = 57;
				HTDATA_OUT_0; //��Ӧ����������
				break;
			}
			value_array[2] =  value_array[2]<<1;  //�¶ȸ�λ
			break;
	  case 57:
			HTDATA_OUT_0; //��Ӧ����������
      HTCLK_OUT_1;  //ACKʱ��
		  Step_Value = 58;
		  value_array[3] = 0;
		  i=0;
		  break;
		case 58:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //��������
		  Step_Value = 59;
		  break;
		case 59:
			HTCLK_OUT_1;
		  Step_Value = 60;
		  break;
		case 60:
			if(HTDATA_IN == 1)
			  value_array[3] |= 0x01;
			HTCLK_OUT_0;
			if(i < 7)
			{
				i++;
				Step_Value = 59;
			}
			else
			{
				Step_Value = 61;
				HTDATA_OUT_0; //��Ӧ����������
				Humi_value = value_array[1] | (value_array[0]<<8);
  	    Temp_value = value_array[3] | (value_array[2]<<8);
				Step_Value = 0;
		    return 0x0D;
			}
			value_array[3] =  value_array[3]<<1;  //�¶ȵ�λ
			break;
		default:
			break;
	}
	return 0x01;
}

// ��ȡ�¼���¼��SOE��¼������
void ReadRecordIndexfromFlash(void)
{
	u8 pbuffer[2];
	u16	temp = 0;
	u16	templast = 0;	
	u16 i = 0;
	u8 flag = 0;
	CS2BZ=0;
	temp = 0;
	templast = 0;
	DLQ_EventRecord_Index = 0;
	flag = 0;
	for(i=0;i<FLASH_EventRecord_MAX;i++)
	{
		SPI_Flash_Read(pbuffer,FLASH_EventRecord_ADDR+(FLASH_EventRecord_LENGTH*i),2);
		temp = pbuffer[0] | (pbuffer[1]<<8);
		if(temp == 0xFFFF)
			temp = 0;
		DLQ_EventRecord_Order[i] = temp;
		if(abs(temp - templast) != 1 || temp < templast)
		{
			if(DLQ_EventRecord_Index == 0 && flag == 0)
			{
				if(i > 0)
				{
					DLQ_EventRecord_Index = i-1;
					flag = 1;				
				}
			}
		}
		templast = temp;	
	}
	
	temp = 0;
	templast = 0;
	DLQ_SOERecord_Index = 0;
	flag = 0;
	for(i=0;i<FLASH_SOERecord_MAX;i++)
	{
		SPI_Flash_Read(pbuffer,FLASH_SOERecord_ADDR+(FLASH_SOERecord_LENGTH*i),2);
		temp = pbuffer[0] | (pbuffer[1]<<8);
		if(temp == 0xFFFF)
			temp = 0;
		DLQ_SOERecord_Order[i] = temp;
		if(abs(temp - templast) != 1 || temp < templast)
		{
			if(DLQ_SOERecord_Index == 0 && flag == 0)
			{
				if(i > 0)
				{
					DLQ_SOERecord_Index = i-1;
					flag = 1;				
				}
			}
		}
		templast = temp;	
	}
	
	temp = 0;
	templast = 0;
	Battery_Voltage_Index = 0;
	flag = 0;
	for(i=0;i<FLASH_BATVOLTAGE_MAX;i++)
	{
		SPI_Flash_Read(pbuffer,FLASH_BATVOLTAGE_ADDR+(FLASH_BATVOLTAGE_LENGTH*i),2);
		temp = pbuffer[0] | (pbuffer[1]<<8);
		if(temp == 0xFFFF)
			temp = 0;
		Battery_Voltage_Order[i] = temp;
		if(abs(temp - templast) != 1 || temp < templast)
		{
			if(Battery_Voltage_Index == 0 && flag == 0)
			{
				if(i > 0)
				{
					Battery_Voltage_Index = i-1;
					flag = 1;				
				}
			}
		}
		templast = temp;	
	}
	
	temp = 0;
	SPI_Flash_Read(pbuffer,FLASH_BATTERY_RUNTIME_ADDR,2);
	temp = pbuffer[0] | (pbuffer[1]<<8);
	if(temp == 0xFFFF)
	  temp = 0;
	Battery_RunTime = temp;
	
	temp = 0;
	templast = 0;
	Curve_Index = 0;
	flag = 0;
	for(i=0;i<FLASH_P1SCURVE_MAX;i++)
	{
		SPI_Flash_Read(pbuffer,FLASH_P1SCURVE_ADDR+(FLASH_P1SCURVE_LENGTH*i),2);
		temp = pbuffer[0] | (pbuffer[1]<<8);
		if(temp == 0xFFFF)
			temp = 0;
		Curve_Order[i] = temp;
		if(abs(temp - templast) != 1 || temp < templast)
		{
			if(Curve_Index == 0 && flag == 0)
			{
				if(i > 0)
				{
					Curve_Index = i-1;
					flag = 1;				
				}
			}
		}
		templast = temp;	
	}
}

// ���ͳ����Ϣ
void ClearTJXX(void)
{
  u16 i = 0;
	CS2BZ=0;
	SPI_Flash_Erase(FLASH_EventRecord_ADDR,FLASH_EventRecord_LENGTH*FLASH_EventRecord_MAX);
	for(i=0;i<FLASH_EventRecord_MAX;i++)
	{
		DLQ_EventRecord_Order[i] = 0;
	}
	DLQ_EventRecord_Index = 0;

	SPI_Flash_Erase(FLASH_SOERecord_ADDR,FLASH_SOERecord_LENGTH*FLASH_SOERecord_MAX);
	for(i=0;i<FLASH_SOERecord_MAX;i++)
	{
		DLQ_SOERecord_Order[i] = 0;
	}
	DLQ_SOERecord_Index = 0;
	
	SPI_Flash_Erase(FLASH_BATVOLTAGE_ADDR,FLASH_BATVOLTAGE_LENGTH*FLASH_BATVOLTAGE_MAX);
	for(i=0;i<FLASH_BATVOLTAGE_MAX;i++)
	{
		Battery_Voltage_Order[i] = 0; 
	}
	Battery_Voltage_Index = 0;
	
	for(i=0;i<FLASH_P1SCURVE_MAX;i++)
	{
		Curve_Order[i] = 0;
		SPI_Flash_Erase(FLASH_P1SCURVE_ADDR+(i*FLASH_P1SCURVE_LENGTH),4096);
	}
	Curve_Index = 0;
	
	WpLast = 0;			// ��һ�ε����й�����
	WqLast = 0;			// ��һ�ε����޹�����
	Wp_all_Last = 0;	// ��һ�����й�����
	Wp_peak_Last = 0;	// ��һ�η��й�����
	Wp_valley_Last = 0;	// ��һ�ι��й�����
	Wp_level_Last = 0;	// ��һ��ƽ�й�����
	
	WpFX_all_Last = 0;	// ��һ�����й�����
	WpFX_peak_Last = 0;	// ��һ�η��й�����
	WpFX_valley_Last = 0;// ��һ�ι��й�����
	WpFX_level_Last = 0;// ��һ��ƽ�й�����

	Wq_1all_Last = 0;	// ��һ�΢��������޹�
	Wq_2all_Last = 0;	// ��һ�΢��������޹�
	Wq_3all_Last = 0;	// ��һ�΢��������޹�
	Wq_4all_Last = 0;	// ��һ�΢��������޹�
	YabanSave_Flag = 1;//��������
}

void RecordSOE(u8 addr,u8 state)// SOE�¼���¼
{
	u8 i = 0;
	u8 temp1 = 0;
	u8 temp2 = 0;
//	u8 buf[12];
//	u8 bufread[2];	
//	u16 recordorder = 0;
	u16 soenum;
	soenum = GetQueueDataNum(&QueueSOE);
	temp1 = (addr-1)/8;
	temp2 = (addr-1)%8;

	if((SOE_Enable[temp1] & (1<<temp2)) == 0)
		return;
	if((soenum%10) != 0)
	{
		for(i=0;i<(soenum%10);i++)
		  GetDataFromQueue(&QueueSOE,BufferSOE);					
	}
	
	if(GetQueueDataNum(&QueueSOE) > 630)  //==640
	{
		for(i=0;i<10;i++)
			GetDataFromQueue(&QueueSOE,BufferSOE);
	}

	InsertDataToQueue(&QueueSOE,BufferSOE,TimeNow.year);				  
	InsertDataToQueue(&QueueSOE,BufferSOE,TimeNow.month);
	InsertDataToQueue(&QueueSOE,BufferSOE,TimeNow.date);
	InsertDataToQueue(&QueueSOE,BufferSOE,TimeNow.hour);
	InsertDataToQueue(&QueueSOE,BufferSOE,TimeNow.minute);
	InsertDataToQueue(&QueueSOE,BufferSOE,TimeNow.second);
	InsertDataToQueue(&QueueSOE,BufferSOE,TimeNow.msec >> 8);
	InsertDataToQueue(&QueueSOE,BufferSOE,TimeNow.msec & 0x00FF);
	InsertDataToQueue(&QueueSOE,BufferSOE,addr);
	InsertDataToQueue(&QueueSOE,BufferSOE,state);
	
//	SPI_Flash_Read(bufread,FLASH_SOERecord_ADDR+(DLQ_SOERecord_Index*FLASH_SOERecord_LENGTH),2);
//	recordorder = bufread[0] | (bufread[1]<<8);
//	if(recordorder == 0xFFFF) recordorder = 0;
//	if(recordorder == DLQ_SOERecord_Order[DLQ_SOERecord_Index])
//	{
//		recordorder = DLQ_SOERecord_Order[DLQ_SOERecord_Index] +1;
//		if(recordorder > 60000 || recordorder == 0)recordorder = 1;
//		if(DLQ_SOERecord_Order[DLQ_SOERecord_Index] != 0)
//		{
//			if(DLQ_SOERecord_Index < (FLASH_SOERecord_MAX-1))
//				DLQ_SOERecord_Index++;
//			else
//				DLQ_SOERecord_Index = 0;				
//		}
//		DLQ_SOERecord_Order[DLQ_SOERecord_Index] = recordorder;
//	}
//	buf[0] = DLQ_SOERecord_Order[DLQ_SOERecord_Index] & 0x00FF;
//	buf[1] = DLQ_SOERecord_Order[DLQ_SOERecord_Index] >> 8;
//	buf[2] = TimeNow.year;
//  buf[3] = TimeNow.month;
//	buf[4] = TimeNow.date;
//	buf[5] = TimeNow.hour;
//	buf[6] = TimeNow.minute;
//	buf[7] = TimeNow.second;
//	buf[8] = TimeNow.msec >> 8;
//	buf[9] = TimeNow.msec & 0x00FF;
//	buf[10] = addr;
//	buf[11] = state;
//	SPI_Flash_Write(buf,FLASH_SOERecord_ADDR+(DLQ_SOERecord_Index*FLASH_SOERecord_LENGTH),FLASH_SOERecord_LENGTH);// ����SOE��¼
	
	SOE_Flag = 1;
}

void RecordSOE_Pad(u8 addr,u8 state)// SOE�¼���¼,ƽ��
{
	u8 i = 0;
	u16 soenum;
	soenum = GetQueueDataNum(&QueueSOE_Pad);
	if((soenum%10) != 0)
	{
		for(i=0;i<(soenum%10);i++)
		  GetDataFromQueue(&QueueSOE_Pad,BufferSOE_Pad);					
	}
	
	if(GetQueueDataNum(&QueueSOE_Pad) > 290) 
	{
		for(i=0;i<10;i++)
			GetDataFromQueue(&QueueSOE_Pad,BufferSOE_Pad);
	}
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,TimeNow.year);				  
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,TimeNow.month);
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,TimeNow.date);
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,TimeNow.hour);
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,TimeNow.minute);
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,TimeNow.second);
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,TimeNow.msec >> 8);
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,TimeNow.msec & 0x00FF);
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,addr);
	InsertDataToQueue(&QueueSOE_Pad,BufferSOE_Pad,state);
	SOEPad_Flag = 1;
}

void YXChangeInit(void)// ң�ű�λ��ʼ��
{
	YXChange.bit.D0  = FW_IN1;
	YXChange.bit.D1  = HW_IN2;
	YXChange.bit.D2  = WCN_IN3;
	YXChange.bit.D3  = LOCAL_IN7;                                            // ���ؿ���
	YXChange.bit.D4  = FAR_IN6;                                              // Զ������
	YXChange.bit.D5  = MeasureData.YaBan1&0x01;                              // ����Ͷ��ѹ��
	YXChange.bit.D6  = MeasureData.YaBan2&0x01;                              // �ӵر���Ͷ��ѹ��
	YXChange.bit.D7  = MeasureData.YaBan3&0x01;                              // ��䱣��Ͷ��ѹ��
	YXChange.bit.D8  = SigresetFlag&0x01;                                    // �����ź�
	if(ProtectFlag==1 || (KZQMeasureData.para.AlarmFlag.word & 0x01ff))	// �¹����
	{
    YXChange.bit.D9 = 0x01; 
	}
	else
	{
    YXChange.bit.D9 = 0x00; 
	}
			
	if(KZQMeasureData.para.SelfCheck.word & 0x3f0f)
	{
	  YXChange.bit.D10 = 0x01;                                         //װ���쳣�澯
	}
	else
	   YXChange.bit.D10 = 0x00; 
  if(DYXH1 == 1 && DYXH2 == 1)	
	{
	  YXChange.bit.D11 = 0x01;                                          //����ʧ��澯
	}
	else
	  YXChange.bit.D11 = 0x00;
	if(Switchon_Lock | Switchon_Lock1 | Switchon_Lock2)	
	{
	  YXChange.bit.D12 = 0x01;                                          //���ر�����բ
	}
	else
	  YXChange.bit.D12 = 0x00;
	if(Switchoff_Lock | Switchoff_Lock1)	
	{
	  YXChange.bit.D13 = 0x01;                                          //���ر�����բ
	}
	else
	  YXChange.bit.D13 = 0x00;
	if(AutoswitchSet.para.segment_contact_mode == 0x01)	
	{
	  YXChange.bit.D14 = 0x01;                                          //�ֶε㹤��ģʽ
	}
	else
	  YXChange.bit.D14 = 0x00;
	
	if(AutoswitchSet.para.segment_contact_mode == 0x02)	
	{
	  YXChange.bit.D15 = 0x01;                                          //����㹤��ģʽ
	}
	else
	  YXChange.bit.D15 = 0x00;  
	
	if(ProtectTimeout.para.novoltage_value_A == 1)	
	{
	  YXChange1.bit.D0 = 0x01;                                          //��Դ����ѹ
	}
	else
	  YXChange1.bit.D0 = 0x00;  
	if(ProtectTimeout.para.novoltage_value_B == 1)	
	{
	  YXChange1.bit.D1 = 0x01;                                          //���ز���ѹ
	}
	else
	  YXChange1.bit.D1 = 0x00;  
	if(KZQMeasureData.para.ProtectFlag1.bit.loss_power)	
	{
	  YXChange1.bit.D2 = 0x01;                                          //ʧѹ��բ
	}
	else
	  YXChange1.bit.D2 = 0x00;  
	
	YXChange1.bit.D3 = KZQMeasureData.para.AlarmFlag.bit.bat_active;    //��ػ
	YXChange1.bit.D4 = PReverse.bit.PA_Reverse;
	YXChange1.bit.D5 = PReverse.bit.PB_Reverse;
	YXChange1.bit.D6 = PReverse.bit.PC_Reverse;
	YXChange1.bit.D7 = PReverse.bit.P_Reverse;
	
	YXChange1.bit.D11 = KZQMeasureData.para.ProtectFlag2.bit.max_voltage;
	YXChange1.bit.D12 = KZQMeasureData.para.ProtectFlag2.bit.max_freq;
	YXChange1.bit.D13 = KZQMeasureData.para.ProtectFlag2.bit.low_freq;
	YXChange1.bit.D14 = KZQMeasureData.para.AlarmFlag.bit.zero_max_voltage;
  YXChange1.bit.D15 = KZQMeasureData.para.AlarmFlag.bit.zero_max_current;
	YXChange2.bit.D0  = KZQMeasureData.para.ProtectFlag1.bit.zero_max_voltage;
	YXChange2.bit.D1  = KZQMeasureData.para.ProtectFlag1.bit.zero_max_current;
	YXChange2.bit.D2  = KZQMeasureData.para.ProtectFlag1.bit.no_phase;
	YXChange2.bit.D3  = KZQMeasureData.para.ProtectFlag1.bit.ground;
	YXChange2.bit.D4  = KZQMeasureData.para.ProtectFlag1.bit.fast_off;
	YXChange2.bit.D5  = KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off;
	YXChange2.bit.D6  = KZQMeasureData.para.ProtectFlag1.bit.max_current;
	YXChange2.bit.D7  = KZQMeasureData.para.ProtectFlag1.bit.max_current_after;
	YXChange2.bit.D8  = KZQMeasureData.para.ProtectFlag2.bit.reon_act;
	YXChange2.bit.D9  = KZQMeasureData.para.AlarmFlag.bit.max_load;
	YXChange2.bit.D10 = KZQMeasureData.para.AlarmFlag.bit.max_voltage;
	YXChange2.bit.D11 = KZQMeasureData.para.AlarmFlag.bit.min_voltage;
	YXChange2.bit.D12 = KZQMeasureData.para.AlarmFlag.bit.max_harmonic;
	YXChange2.bit.D13 = KZQMeasureData.para.AlarmFlag.bit.voltage_qualifrate;
	YXChange2.bit.D14 = KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage;
	YXChange2.bit.D15 = KZQMeasureData.para.AlarmFlag.bit.ground;
}

void SwapYXChange(void)// ң�ű�λɨ��
{
	u16 i = 0;
	static u8 self_error = 0;
	static u8 event_out = 0;
	static u8 power_lost = 0;
	
	if(YXChange.bit.D0 != FW_IN1)	// ��λ
	{
		YXChange.bit.D0 = FW_IN1;
		if(YXChangeShake[0] == 0)
		{
			YXChangeShake[0] = YX_SHAKE_TIME;
			YXChangeHold.bit.D0 = YXChange.bit.D0;
		}
	}
	if(YXChangeShake[0]>0)
	{
		YXChangeShake[0]--;
		if(YXChangeShake[0]==0 && YXChangeHold.bit.D0 == YXChange.bit.D0)
		{
			RecordSOE(SOE_ADDR_IN1,YXChange.bit.D0^0x01);
			RecordSOE_Pad(SOE_ADDR_IN1_pad,YXChange.bit.D0);
		}
	}

	if(YXChange.bit.D1 != HW_IN2)	// ��λ
	{
		YXChange.bit.D1 = HW_IN2;
		if(YXChangeShake[1] == 0)
		{
			YXChangeShake[1] = YX_SHAKE_TIME;
			YXChangeHold.bit.D1 = YXChange.bit.D1;
		}
	}
	if(YXChangeShake[1]>0)
	{
		YXChangeShake[1]--;
		if(YXChangeShake[1]==0 && YXChangeHold.bit.D1 == YXChange.bit.D1)
		{
			if(YXChange.bit.D1 == 0)
			{
				StartCntFlag |= 0x10;
				if(AutoswitchSet.para.err_lockon_enable == 1)
				  StartCntFlag |= 0x01;
				if(AutoswitchSet.para.err_fastoff_enable == 1)
					StartCntFlag |= 0x02;
				if(AutoswitchSet.para.lock_switchoff_enable == 1)
					StartCntFlag |= 0x04;
				if(AutoswitchSet.para.err_current_enable == 1)
					StartCntFlag |= 0x08;
			}
			RecordSOE(SOE_ADDR_IN2,YXChange.bit.D1^0x01);
		}
	}

	if(YXChange.bit.D2 != WCN_IN3)	// δ����λ��
	{
		YXChange.bit.D2 = WCN_IN3;
		if(YXChangeShake[2] == 0)
		{
			YXChangeShake[2] = YX_SHAKE_TIME;
			YXChangeHold.bit.D2 = YXChange.bit.D2;
		}
	}
	if(YXChangeShake[2]>0)
	{
		YXChangeShake[2]--;
		if(YXChangeShake[2]==0 && YXChangeHold.bit.D2 == YXChange.bit.D2)
		{
			RecordSOE(SOE_ADDR_IN3,YXChange.bit.D2^0x01);
			RecordSOE_Pad(SOE_ADDR_IN3_pad,YXChange.bit.D2);
		}
	}
	
	if(YXChange.bit.D3 != LOCAL_IN7)	// ����λ��
	{
		YXChange.bit.D3 = LOCAL_IN7;
		if(YXChangeShake[3] == 0)
		{
			YXChangeShake[3] = YX_SHAKE_TIME;
			YXChangeHold.bit.D3 = YXChange.bit.D3;
		}
	}
	if(YXChangeShake[3]>0)
	{
		YXChangeShake[3]--;
		if(YXChangeShake[3]==0 && YXChangeHold.bit.D3 == YXChange.bit.D3)
		{
			RecordSOE(SOE_ADDR_GND,YXChange.bit.D3^0x01);
			RecordSOE_Pad(SOE_ADDR_GND_pad,YXChange.bit.D3^0x01);
		}
	}
	
	if(YXChange.bit.D4 != FAR_IN6)	// Զ��λ��
	{
		YXChange.bit.D4 = FAR_IN6;
		if(YXChangeShake[4] == 0)
		{
			YXChangeShake[4] = YX_SHAKE_TIME;
			YXChangeHold.bit.D4 = YXChange.bit.D4;
		}
	}
	if(YXChangeShake[4]>0)
	{
		YXChangeShake[4]--;
		if(YXChangeShake[4]==0 && YXChangeHold.bit.D4 == YXChange.bit.D4)
		{
			RecordSOE(SOE_ADDR_FAR,YXChange.bit.D4^0x01);
			RecordSOE_Pad(SOE_ADDR_FAR_pad,YXChange.bit.D4^0x01);
		}
	}
	
	if(YXChange.bit.D5 != (MeasureData.YaBan1 &0x01))	    // ����ѹ��
	{
		YXChange.bit.D5 = (MeasureData.YaBan1 &0x01);
		RecordSOE_Pad(SOE_ADDR_BHYB1_pad,YXChange.bit.D5);
	}
	if(YXChange.bit.D6 != (MeasureData.YaBan2 &0x01))	    // �ӵر���ѹ��
	{
		YXChange.bit.D6 = (MeasureData.YaBan2 &0x01);
		RecordSOE_Pad(SOE_ADDR_BHYB2_pad,YXChange.bit.D6);
	}
	if(YXChange.bit.D7 != (MeasureData.YaBan3 &0x01))	    // ��䱣��ѹ��
	{
		YXChange.bit.D7 = (MeasureData.YaBan3 &0x01);
		RecordSOE_Pad(SOE_ADDR_BHYB3_pad,YXChange.bit.D7);
	}

	if(SigresetFlag)	          // �����ź�  
	{
		YXChange.bit.D8 = (SigresetFlag &0x01);
		RecordSOE_Pad(SOE_ADDR_SIGRESET_pad,YXChange.bit.D8);
		
		StartCntFlag = 0;
		KZQMeasureData.para.ProtectFlag1.word = 0;
		KZQMeasureData.para.ProtectFlag2.word = 0;
		KZQMeasureData.para.AlarmFlag.word = 0;
		ProtectTimesLock = 0;
		AD_StartPoint = 0;
		RealCurveFlag = 0;
		SendLock_Cnt = 0;
		ProtectFlag = 0;
		MeasureData.Psaveflag = 0;
		for(i=0;i<(sizeof(struct PROTECT_DELAY)/2);i++)	// ������ʱ
		{
			ProtectDelay.word[i] = 0;
			ProtectTimeout.word[i] = 0;
		}
		for(i=0;i<(sizeof(struct ALARM_DELAY)/2);i++)
		{
			AlarmDelay.word[i] = 0;
			AlarmTimeout.word[i] = 0;
		}
		for(i=0;i<(sizeof(struct KZQMEASURE)/2);i++)
		{
			KZQMeasureData.word[i] = 0;
		}
		ReOnFlag = 0;
		under_voltage_on = 0;
		PTDX_Enable = 0;
		CTDX_Enable = 0;
		PTDX_Flag = 0;
		CTDX_Flag = 0;
		Switchon_Lock = 0;
		Switchon_Lock1 = 0;
		Switchon_Lock2 = 0;
		Switchoff_Lock = 0;
		Switchoff_Lock1 = 0;
		SigresetFlag = 0;
	}
	if(ProtectFlag==1 || (KZQMeasureData.para.AlarmFlag.word & 0x01ff))
	{
	  event_out = 0x01;                                            //�¹���
	}
	else
	  event_out = 0x00; 

	if(YXChange.bit.D9 != event_out)
	{
		YXChange.bit.D9 = event_out;
	  RecordSOE(SOE_ADDR_EVENT_ALL,YXChange.bit.D9);               //�¹���
	}
	if(KZQMeasureData.para.SelfCheck.word & 0x3f0f)
	{
	  self_error = 0x01;                                           //װ���쳣�澯
	}
	else
	  self_error = 0x00; 

	if(YXChange.bit.D10 != self_error)
	{
		YXChange.bit.D10 = self_error;
	  RecordSOE(SOE_ADDR_SELF_ERROR,YXChange.bit.D10);              //װ���쳣�澯
	}
	if(DYXH1 == 1 && DYXH2 == 1)	
	{
	  power_lost = 0x01;                                            //����ʧ��澯
	}
	else
	  power_lost=0x00;   
  if(YXChange.bit.D11 != power_lost)
	{
		YXChange.bit.D11 = power_lost;
	  RecordSOE(SOE_ADDR_POWER_LOST,YXChange.bit.D11);               //����ʧ��澯
	}
	
	if(YXChange.bit.D12 != ((Switchon_Lock | Switchon_Lock1 | Switchon_Lock2)&0X01))	
	{
	  YXChange.bit.D12 = (Switchon_Lock | Switchon_Lock1 | Switchon_Lock2)&0X01;          
		RecordSOE(SOE_ADDR_SWITCHON_LOCK,YXChange.bit.D12);               //���ر�����բ
	}

	if(YXChange.bit.D13 != ((Switchoff_Lock | Switchoff_Lock1)&0x01))	
	{
	  YXChange.bit.D13 = (Switchoff_Lock | Switchoff_Lock1)&0x01;       //���ر�����բ
		RecordSOE(SOE_ADDR_SWITCHOFF_LOCK,YXChange.bit.D13);
	}

	if(YXChange.bit.D14 != (AutoswitchSet.para.segment_contact_mode&0x01))	
	{
	  YXChange.bit.D14 = AutoswitchSet.para.segment_contact_mode&0x01; //�ֶε㹤��ģʽ
		RecordSOE(SOE_ADDR_SWITCH_CONTACT_1,YXChange.bit.D14);
	}

	if(YXChange.bit.D15 != ((AutoswitchSet.para.segment_contact_mode>>1)&0x01))	
	{
	  YXChange.bit.D15 = (AutoswitchSet.para.segment_contact_mode>>1)&0x01;//����㹤��ģʽ
		RecordSOE(SOE_ADDR_SWITCH_CONTACT_2,YXChange.bit.D15);
	}

	if(YXChange1.bit.D0 != (ProtectTimeout.para.novoltage_value_A&0x01))	
	{
	  YXChange1.bit.D0 = ProtectTimeout.para.novoltage_value_A&0x01;    //��Դ����ѹ
		RecordSOE(SOE_ADDR_SWITCH_POWERLOSS_1,YXChange1.bit.D0);
	}
  
	if(YXChange1.bit.D1 != (ProtectTimeout.para.novoltage_value_B&0x01))	
	{
	  YXChange1.bit.D1 = ProtectTimeout.para.novoltage_value_B&0x01;    //���ز���ѹ
		RecordSOE(SOE_ADDR_SWITCH_POWERLOSS_2,YXChange1.bit.D1);
	}

	if(YXChange1.bit.D2 != KZQMeasureData.para.ProtectFlag1.bit.loss_power)	
	{
	  YXChange1.bit.D2 = KZQMeasureData.para.ProtectFlag1.bit.loss_power;                                      
		RecordSOE(SOE_ADDR_POWERLOSS_OFF,YXChange1.bit.D2);               //ʧѹ��բ
	} 
	
	if(YXChange1.bit.D3 != KZQMeasureData.para.AlarmFlag.bit.bat_active)	// ��ػ
	{
		YXChange1.bit.D3 = KZQMeasureData.para.AlarmFlag.bit.bat_active;
		RecordSOE(SOE_ADDR_ALARM_BAT_ACTIVE,YXChange1.bit.D3);
	}
	
	if(YXChange2.bit.D0 != KZQMeasureData.para.ProtectFlag1.bit.zero_max_voltage)	// �����ѹ��������
	{
		YXChange2.bit.D0 = KZQMeasureData.para.ProtectFlag1.bit.zero_max_voltage;
		RecordSOE_Pad(SOE_ADDR_ZERO_MAX_VOLTAGE_pad,YXChange2.bit.D0);
	}

	if(YXChange2.bit.D1 != KZQMeasureData.para.ProtectFlag1.bit.zero_max_current)	// ���������������
	{
		YXChange2.bit.D1 = KZQMeasureData.para.ProtectFlag1.bit.zero_max_current;
		RecordSOE(SOE_ADDR_ZERO_MAX_CURRENT,YXChange2.bit.D1);
		RecordSOE_Pad(SOE_ADDR_ZERO_MAX_CURRENT_pad,YXChange2.bit.D1);
	}
	
	if(YXChange2.bit.D2 != KZQMeasureData.para.ProtectFlag1.bit.no_phase)	//ȱ�ౣ������
	{
		YXChange2.bit.D2 = KZQMeasureData.para.ProtectFlag1.bit.no_phase;
		RecordSOE_Pad(SOE_ADDR_PHASE_LOSS_pad,YXChange2.bit.D2);
	}

	if(YXChange2.bit.D3 != KZQMeasureData.para.ProtectFlag1.bit.ground)  	//�ӵر�������
	{
		YXChange2.bit.D3 = KZQMeasureData.para.ProtectFlag1.bit.ground;
		RecordSOE_Pad(SOE_ADDR_GROUND_pad,YXChange2.bit.D3);
	}

	if(YXChange2.bit.D4 != KZQMeasureData.para.ProtectFlag1.bit.fast_off)	// �ٶϱ�������
	{
		YXChange2.bit.D4 = KZQMeasureData.para.ProtectFlag1.bit.fast_off;
		RecordSOE(SOE_ADDR_FAST_OFF,YXChange2.bit.D4);
		RecordSOE_Pad(SOE_ADDR_FAST_OFF_pad,YXChange2.bit.D4);
	}

	if(YXChange2.bit.D5 != KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off)	// ��ʱ�ٶϱ�������
	{
		YXChange2.bit.D5 = KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off;
		RecordSOE(SOE_ADDR_DELAY_FAST_OFF,YXChange2.bit.D5);
		RecordSOE_Pad(SOE_ADDR_DELAY_FAST_OFF_pad,YXChange2.bit.D5);
	}

	if(YXChange2.bit.D6 != KZQMeasureData.para.ProtectFlag1.bit.max_current)	// ������������
	{
		YXChange2.bit.D6 = KZQMeasureData.para.ProtectFlag1.bit.max_current;
		RecordSOE(SOE_ADDR_MAX_CURRENT,YXChange2.bit.D6);
		RecordSOE_Pad(SOE_ADDR_MAX_CURRENT_pad,YXChange2.bit.D6);
	}

	if(YXChange2.bit.D7 != KZQMeasureData.para.ProtectFlag1.bit.max_current_after)	// ��������ٱ�������
	{
		YXChange2.bit.D7 = KZQMeasureData.para.ProtectFlag1.bit.max_current_after;
		RecordSOE(SOE_ADDR_MAX_CURRENT_AFTER,YXChange2.bit.D7);
		RecordSOE_Pad(SOE_ADDR_MAX_CURRENT_AFTER_pad,YXChange2.bit.D7);
	}
	
	if(YXChange2.bit.D8 != KZQMeasureData.para.ProtectFlag2.bit.reon_act)	// �غ�բ����
	{
		YXChange2.bit.D8 = KZQMeasureData.para.ProtectFlag2.bit.reon_act;
		RecordSOE(SOE_ADDR_REON,YXChange2.bit.D8);
		RecordSOE_Pad(SOE_ADDR_REON_pad,YXChange2.bit.D8);
	}

	if(YXChange2.bit.D9 != KZQMeasureData.para.AlarmFlag.bit.max_load)	// �����ɱ�������
	{
		YXChange2.bit.D9 = KZQMeasureData.para.AlarmFlag.bit.max_load;
		RecordSOE(SOE_ADDR_ALARM_MAX_LOAD,YXChange2.bit.D9);
		RecordSOE_Pad(SOE_ADDR_ALARM_MAX_LOAD_pad,YXChange2.bit.D9);
	}

	if(YXChange2.bit.D10 != KZQMeasureData.para.AlarmFlag.bit.max_voltage)	// ����ѹ��������
	{
		YXChange2.bit.D10 = KZQMeasureData.para.AlarmFlag.bit.max_voltage;
		RecordSOE_Pad(SOE_ADDR_ALARM_MAX_VOLTAGE_pad,YXChange2.bit.D10);
	}

	if(YXChange2.bit.D11 != KZQMeasureData.para.AlarmFlag.bit.min_voltage)	// Ƿѹ��������
	{
		YXChange2.bit.D11 = KZQMeasureData.para.AlarmFlag.bit.min_voltage;
		RecordSOE_Pad(SOE_ADDR_ALARM_MIN_VOLTAGE_pad,YXChange2.bit.D11);
	}

	if(YXChange2.bit.D12 != KZQMeasureData.para.AlarmFlag.bit.max_harmonic)	// ��г����������
	{
		YXChange2.bit.D12 = KZQMeasureData.para.AlarmFlag.bit.max_harmonic;
		RecordSOE_Pad(SOE_ADDR_ALARM_MAX_HARMONIC_pad,YXChange2.bit.D12);
	}

	if(YXChange2.bit.D13 != KZQMeasureData.para.AlarmFlag.bit.voltage_qualifrate)	// ��ѹ�ϸ��ʱ�������
	{
		YXChange2.bit.D13 = KZQMeasureData.para.AlarmFlag.bit.voltage_qualifrate;
		RecordSOE_Pad(SOE_ADDR_ALARM_VOLTAGE_QUALIFRATE_pad,YXChange2.bit.D13);
	}

	if(YXChange2.bit.D14 != KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage)	// ��ص͵�ѹ��������
	{
		YXChange2.bit.D14 = KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage;
		RecordSOE(SOE_ADDR_ALARM_BAT_LOW_VOLTAGE,YXChange2.bit.D14);
		RecordSOE_Pad(SOE_ADDR_ALARM_BAT_LOW_VOLTAGE_pad,YXChange2.bit.D14);
	}

	if(YXChange2.bit.D15 != KZQMeasureData.para.AlarmFlag.bit.ground)	// �ӵر�������
	{
		YXChange2.bit.D15 = KZQMeasureData.para.AlarmFlag.bit.ground;
		RecordSOE(SOE_ADDR_ALARM_GROUND,YXChange2.bit.D15);
		RecordSOE_Pad(SOE_ADDR_ALARM_GROUND_pad,YXChange2.bit.D15);
	}

	if(YXChange1.bit.D11 != KZQMeasureData.para.ProtectFlag2.bit.max_voltage)	// ��ѹ��������
	{
		YXChange1.bit.D11 = KZQMeasureData.para.ProtectFlag2.bit.max_voltage;
		RecordSOE(SOE_ADDR_ALARM_GROUNDI0,YXChange1.bit.D11);
		RecordSOE_Pad(SOE_ADDR_MAX_VOLTAGE_pad,YXChange1.bit.D11);
	}
	
	if(YXChange1.bit.D12 != KZQMeasureData.para.ProtectFlag2.bit.max_freq)	// ��Ƶ��������
	{
		YXChange1.bit.D12 = KZQMeasureData.para.ProtectFlag2.bit.max_freq;
		RecordSOE(SOE_ADDR_ALARM_GROUNDI0,YXChange1.bit.D12);
		RecordSOE_Pad(SOE_ADDR_MAX_FREQ_pad,YXChange1.bit.D12);
	}
	
	if(YXChange1.bit.D13 != KZQMeasureData.para.ProtectFlag2.bit.low_freq)	// ��Ƶ��������
	{
		YXChange1.bit.D13 = KZQMeasureData.para.ProtectFlag2.bit.low_freq;
		RecordSOE(SOE_ADDR_ALARM_GROUNDI0,YXChange1.bit.D13);
		RecordSOE_Pad(SOE_ADDR_LOW_FREQ_pad,YXChange1.bit.D13);
	}
	
	if(YXChange1.bit.D14 != KZQMeasureData.para.AlarmFlag.bit.zero_max_voltage)	// �����ѹ��������
	{
		YXChange1.bit.D14 = KZQMeasureData.para.AlarmFlag.bit.zero_max_voltage;
		RecordSOE_Pad(SOE_ADDR_ALARM_GROUNDU0_pad,YXChange1.bit.D14);
	}

	if(YXChange1.bit.D15 != KZQMeasureData.para.AlarmFlag.bit.zero_max_current)	// ���������������
	{
		YXChange1.bit.D15 = KZQMeasureData.para.AlarmFlag.bit.zero_max_current;
		RecordSOE(SOE_ADDR_ALARM_GROUNDI0,YXChange1.bit.D15);
		RecordSOE_Pad(SOE_ADDR_ALARM_GROUNDI0_pad,YXChange1.bit.D15);
	}
}

// ��ȡ�汾��Ϣ
void ReadVersionfromFlash(void)
{
	u8 pbuffer[10] = {0};
  static u32 cpuid[3];
	u8 i = 0;
  cpuid[0] = *(vu32*)ID_Address;       //�豸ΨһID�����㷨
  cpuid[1] = *(vu32*)(ID_Address+4);
  cpuid[2] = *(vu32*)(ID_Address+8);
  Lock_IdCode = cpuid[0] + cpuid[1] + cpuid[2];
  CS2BZ=0;
  // ��������������
	SPI_Flash_Read(pbuffer,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10);
 	for(i=0;i<6;i++)
 	{
 		MyVersion.KZQ_time_update[i] = pbuffer[i];
 	}
	MyVersion.KZQ_length_update = (u32)pbuffer[6] | ((u32)pbuffer[7]<<8) | ((u32)pbuffer[8]<<16) | ((u32)pbuffer[9]<<24);
	// ������������������
	SPI_Flash_Read(pbuffer,FLASH_KZQBACKUP_ADDR+FLASH_KZQBACKUP_LENGTH-10,10);
	for(i=0;i<6;i++)
	{
		MyVersion.KZQ_time_backup[i] = pbuffer[i];
	}
	MyVersion.KZQ_length_backup = (u32)pbuffer[6] | ((u32)pbuffer[7]<<8) | ((u32)pbuffer[8]<<16) | ((u32)pbuffer[9]<<24);
	
	MyVersion.IAP_version[0] = VERSION_YEAR;
	MyVersion.IAP_version[1] = VERSION_MONTH;
	MyVersion.IAP_version[2] = VERSION_DATE;
	MyVersion.IAP_version[3] = VERSION_TIME;
	MyVersion.KZQ_version[0] = VERSION_YEAR;
	MyVersion.KZQ_version[1] = VERSION_MONTH;
	MyVersion.KZQ_version[2] = VERSION_DATE;
	MyVersion.KZQ_version[3] = VERSION_TIME;	
	
	MyVersion.Product_number =  *(__IO uint16_t*)0x080E0000;
	MyVersion.Product_flow =    *(__IO uint16_t*)0x080E0002;
	MyVersion.Product_date[0] =  *(__IO uint8_t*)0x080E0004;
	MyVersion.Product_date[1] =  *(__IO uint8_t*)0x080E0005;
	MyVersion.Product_date[2] =  *(__IO uint8_t*)0x080E0006;
	
	sprintf(&Constpara9[9],"%03u",MyVersion.Product_number);
	Constpara9[12]= '2';
	Constpara9[13]= '0';
	sprintf(&Constpara9[14],"%02u",MyVersion.Product_date[0]);
	sprintf(&Constpara9[16],"%02u",MyVersion.Product_date[1]);
	sprintf(&Constpara9[18],"%02u",MyVersion.Product_date[2]);
	sprintf(&Constpara9[20],"%04u",MyVersion.Product_flow);
}
// ����KZQ������־ typeΪ��������  0x01=����������0x02=Զ������,0x03=�ָ�����
void SetUpdateFlag(u8 type)
{
	u8 pbuffer[FLASH_UPDATEFLAG_LENGTH] = {0};
	pbuffer[0] = 0x55;
	pbuffer[1] = 0xAA;
	pbuffer[2] = type;
	CS2BZ=0;
	SPI_Flash_Write(pbuffer,FLASH_UPDATEFLAG_ADDR,FLASH_UPDATEFLAG_LENGTH);
}
// ��ȡ����������־��Ϣ�������������ñ�־0x55 AA,��λ��������д������дΪ0x5A A5,������д���ɹ�
u8 ReadUpdateFlagfromFlash(void)
{
	u8 pbuffer[FLASH_UPDATEFLAG_LENGTH] = {0};
	CS2BZ=0;
	SPI_Flash_Read(pbuffer,FLASH_UPDATEFLAG_ADDR,FLASH_UPDATEFLAG_LENGTH);
	MyVersion.IAP_version[0]=pbuffer[3];
	MyVersion.IAP_version[1]=pbuffer[4];
	MyVersion.IAP_version[2]=pbuffer[5];
	MyVersion.IAP_version[3]=pbuffer[6];
	if(pbuffer[0]==0x55 && pbuffer[1]==0xAA) // δ��д
	{
		pbuffer[0]=0xFF;
		pbuffer[1]=0xFF;
		SPI_Flash_Write(pbuffer,FLASH_UPDATEFLAG_ADDR,FLASH_UPDATEFLAG_LENGTH);
		return 0x0E;
	}
	else if(pbuffer[0]==0x5A && pbuffer[1]==0xA5)  // ��д�ɹ�
	{
		pbuffer[0]=0xFF;
		pbuffer[1]=0xFF;
		SPI_Flash_Write(pbuffer,FLASH_UPDATEFLAG_ADDR,FLASH_UPDATEFLAG_LENGTH);
		return 0x0D;
	}
	else
	{
		return 0;
  }
}

void JumpToApp(void)	// ��������Ӧ�ó���
{
	u32 JumpAddress;
	pFunction Jump_To_Application;	
  RCC_DeInit();        //�ر�����
  __set_FAULTMASK(1);  //�ر����ж�  
	/* Jump to user application */ 
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	{
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
		JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);       
		Jump_To_Application = (pFunction) JumpAddress;       /* Initialize user application's Stack Pointer */        
		__set_MSP(*(__IO uint32_t*) ApplicationAddress); 
		Jump_To_Application(); 		
	}
}

void IWDG_Configuration(u16 Reload)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);         /* Register write functions allow dogs*/
  IWDG_SetPrescaler(IWDG_Prescaler_256);                /*  40K/256=156.25HZ(6.4ms) */ 
  IWDG_SetReload(Reload);                                  /* ι��ʱ�� 2000*6.4ms = 12800ms */
  IWDG_ReloadCounter();                                 /* ι��*/
  IWDG_Enable();                                        /* ʹ��*/
}

/*----------------------------------------------------------------------------
  Task  'init': Initialize
 *---------------------------------------------------------------------------*/
__task void Task (void) 
{
	IWDG_Configuration(2000);  
	t_Task1  = os_tsk_create_user(Task1, 1, &stk1,  sizeof(stk1));   /* start task 'Task1'   */
	t_Task2  = os_tsk_create_user(Task2, 1, &stk2,  sizeof(stk2));   /* start task 'Task2'  ң�ű�λɨ�裬�ֺ�բ�ƿ��� */
	t_Task3  = os_tsk_create_user(Task3, 1, &stk3,  sizeof(stk3));   /* start task 'Task3'  1���ʱ��                  */
	t_Task4  = os_tsk_create_user(Task4, 1, &stk4,  sizeof(stk4));   /* start task 'Task4'  ͨѶЭ�鴦������           */
	t_Task5  = os_tsk_create_user(Task5, 1, &stk5,  sizeof(stk5));   /* start task 'Task5'  �������ݷ����ñ�־         */
	t_Task6  = os_tsk_create_user(Task6, 1, &stk6,  sizeof(stk6));   /* start task 'Task6'  �������ݷ��ʹ���           */		
	t_Task7  = os_tsk_create_user(Task7, 1, &stk7,  sizeof(stk7));   /* start task 'Task7'  ǰ����������¿���         */
	t_Task8  = os_tsk_create_user(Task8, 1, &stk8,  sizeof(stk8));   /* start task 'Task8'  ���¶Ⱥ�ʪ�Ⱥ���           */
	t_Task9  = os_tsk_create_user(Task9, 1, &stk9,  sizeof(stk9));   /* start task 'Task9'  GPSʱ��У׼��������ݼ��  */
	t_Task10 = os_tsk_create_user(Task10,1, &stk10, sizeof(stk10));  /* start task 'Task10'  */	
	t_Task11 = os_tsk_create_user(Task11,1, &stk11, sizeof(stk11));  /* start task 'Task11'  */
  t_Task12 = os_tsk_create_user(Task12,3, &stk12, sizeof(stk12));  /* start task 'Task12'  */
  t_Task13 = os_tsk_create_user(Task13,1, &stk13, sizeof(stk13));  /* start task 'Task13'  */
  t_Task14 = os_tsk_create_user(Task14,1, &stk14, sizeof(stk14));  /* start task 'Task14'  */
  t_Task15 = os_tsk_create_user(Task15,1, &stk15, sizeof(stk15));  /* start task 'Task15'  */
  t_Task16 = os_tsk_create_user(Task16,1, &stk16, sizeof(stk16));  /* start task 'Task16'  */
  t_Task17 = os_tsk_create_user(Task17,1, &stk17, sizeof(stk17));  /* start task 'Task17' ���Ź����     1000ms      */
	os_tsk_delete_self ();
}

// ������ʼ��
void ParaInit(void)
{
	u16 i = 0;
	for(i=0;i<(sizeof(struct PROTECT_DELAY)/2);i++)
	{
		ProtectDelay.word[i] = 0;
		ProtectTimeout.word[i] = 0;
	}
	for(i=0;i<(sizeof(struct ALARM_DELAY)/2);i++)
	{
		AlarmDelay.word[i] = 0;
		AlarmTimeout.word[i] = 0;
	}
  MeasureData.Pstartflag = 0;
	MeasureData.Psaveflag = 0;
	MeasureData.datatype = 0;
	MeasureData.UABsum = 0;
	MeasureData.UBCsum = 0;
	MeasureData.UCAsum = 0;
	MeasureData.UAsum = 0;
	MeasureData.UBsum = 0;
	MeasureData.UCsum = 0;
	MeasureData.U0sum = 0;
	MeasureData.IAsum = 0;
	MeasureData.IBsum = 0;
	MeasureData.ICsum = 0;
	MeasureData.I0sum = 0;
	MeasureData.PIAsum = 0;
	MeasureData.PIBsum = 0;
	MeasureData.PICsum = 0;
  MeasureData.PI0sum = 0;
	
	MeasureData.UAB_val = 0;
	MeasureData.UBC_val = 0;
	MeasureData.UCA_val = 0;
	MeasureData.IA_val = 0;
	MeasureData.IB_val = 0;
	MeasureData.IC_val = 0;	

	MeasureData.Protect_UAB_val = 0;
	MeasureData.Protect_UBC_val = 0;
	MeasureData.Protect_UCA_val = 0;
	MeasureData.Protect_IA_val = 0;
	MeasureData.Protect_IB_val = 0;
	MeasureData.Protect_IC_val = 0;
	MeasureData.Protect_IA10_val = 0;
	MeasureData.Protect_IB10_val = 0;
	MeasureData.Protect_IC10_val = 0;
	
	MeasureData.Wp_all = 0;	
	MeasureData.Wp_peak = 0;
	MeasureData.Wp_valley = 0;
	MeasureData.Wp_level = 0;

	MeasureData.WpFX_all = 0;	
	MeasureData.WpFX_peak = 0;
	MeasureData.WpFX_valley = 0;
	MeasureData.WpFX_level = 0;

	MeasureData.Wq_1all = 0;	 
	MeasureData.Wq_2all = 0;	
	MeasureData.Wq_3all = 0;
	MeasureData.Wq_4all = 0;	
	
	for(i=0;i<1250;i++)
	{
		MyCurve.CurveA.On.byte[i] = 0;
		MyCurve.CurveA.Off.byte[i] = 0;
		MyCurve.CurveA.Protect.byte[i] = 0;

		MyCurve.CurveB.On.byte[i] = 0;
		MyCurve.CurveB.Off.byte[i] = 0;
		MyCurve.CurveB.Protect.byte[i] = 0;
		
		MyCurve.CurveC.On.byte[i] = 0;
		MyCurve.CurveC.Off.byte[i] = 0;
		MyCurve.CurveC.Protect.byte[i] = 0;
	}

	for(i=0;i<16;i++)
	{
		YXChangeShake[i] = 0;
	}

	MeasureData.Display_UAB_val = 0;
	MeasureData.Display_UBC_val = 0;
	MeasureData.Display_UCA_val = 0;
	MeasureData.Display_UA_val = 0;
	MeasureData.Display_UB_val = 0;
	MeasureData.Display_UC_val = 0;
	MeasureData.Display_U0_val = 0;
	MeasureData.Display_IA_val = 0;
	MeasureData.Display_IB_val = 0;
	MeasureData.Display_IC_val = 0;
	MeasureData.Display_IA_val = 0;
	MeasureData.Display_IB_val = 0;
	MeasureData.Display_IC_val = 0;
	MeasureData.Display_I0_val = 0;
	ParaInitReadyFlag = 1;
}

u8 CtrlReOn(u8 *reon_flag)
{
	static u8 state = 0;
  u8	i = 0;
	 
  if( ProtectSet.para.reon_requency > 3 )
	{
		ProtectSet.para.reon_requency= 3;
	}
	if(*reon_flag == 0)
	{
		state = 0;
		return 0;	
	}
	else if(*reon_flag == 1 )		// �غ�բʹ�ܲ�����
	{
		if( ReOnTimes >= ProtectSet.para.reon_requency )
		{
			*reon_flag = 0;
			state = 0;
		}
		switch(state)
		{
			case 0:						// ������
				state = 1;
				if(ReOnDelay<65000 && ReOnDelay>29)
					ReOnTimeDelay = ReOnDelay;	// �趨��ʱ0.3s~650s
				else
					ReOnTimeDelay = 50;	// 500ms��ʱ
				ReOnTimeOut = 0;
				ProtectTimesLock = 0;	
				ProtectFlag = 0;					
				break;
			case 1:						
				if(ReOnTimeOut == 1)
				{
					ReOnTimes = 1;
					if( ReOnTimes >= ProtectSet.para.reon_requency )
					  *reon_flag = 0;
					else
					  *reon_flag = 2;
					state = 0;
					ReOnTimeOut = 0;
					if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && KZQMeasureData.para.DLQflag.bit.store == 1)	// ��λ���Ѵ���
					{						
						for(i=0;i<8;i++)		// ������ޱ�����ʱ������־
						{
							if(ProtectDelay.word[i] != 0)	// ������Ч����˵��������Ȼ���ڣ������غ�բ
							{
								return 0;
							}
						}
						if(SwitchFlag == 0)
						{
							SwitchFlag = 4;		// ���Ͻ���������ش��ڷ�բ״ִ̬���غ�բ����
						  RecordSwitchOn(TYPE_ACT_REON);					
						}
					}
				}
				break;
			default:
          state = 0;				
				break;
		}
	}
	else if( *reon_flag == 2 )  //�ڶ����غ�բ
	{ 
		if( ReOnTimes >= ProtectSet.para.reon_requency )
		{
			*reon_flag = 0;
			state = 0;
		}			
		switch(state)
		{
			case 0:
				ReOnTimeDelay = 100;	//1s��ʱ
			  ReOnTimeOut = 0;
			  state = 1;
				break;
			case 1:
				if(ReOnTimeOut == 1)
				{
				  ReOnTimeDelay = 1500;	//15s��ʱ
			    ReOnTimeOut = 0;
			    state = 2;
				}
				break;
			case 2:						// ������ʱ
				if(ReOnLockCnt1 >= 15 && ReOnTimeOut == 1)
				{
					*reon_flag = 0;
					state = 0;
				}
        else if(KZQMeasureData.para.DLQflag.bit.breaker == 2)
				{					
					state = 3;
					ReOnDelay = ProtectSet.para.once_reon_time;	
					if(ReOnDelay<65000 && ReOnDelay>200)
						ReOnTimeDelay = ReOnDelay;	// �趨��ʱ2s~650s
					else
						ReOnTimeDelay = 200;	//2s��ʱ
					ReOnTimeOut = 0;
					ProtectTimesLock = 0;
					ProtectFlag = 0;	
			  }				
				break;
			case 3:					
				if(ReOnTimeOut == 1)
				{
					ReOnTimes = 2;
					if( ReOnTimes >= ProtectSet.para.reon_requency )
					  *reon_flag = 0;
					else
					  *reon_flag = 3;
					state = 0;
					ReOnTimeOut = 0;
				  if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && KZQMeasureData.para.DLQflag.bit.store == 1)	// ��λ���Ѵ���
				  {
						for(i=0;i<8;i++)		// ������ޱ�����ʱ������־
						{
							if(ProtectDelay.word[i] != 0)	// ������Ч����˵��������Ȼ���ڣ������غ�բ
							{
								return 0;
							}
						}
						if(SwitchFlag == 0)
						{
							SwitchFlag = 4;		// ���Ͻ���������ش��ڷ�բ״ִ̬���غ�բ����
							RecordSwitchOn(TYPE_ACT_REON);
						}
				  }
				}
				break;
			default:
				break;
		}
	}
	else if( *reon_flag == 3 )  //�������غ�բ
	{
		if( ReOnTimes >= ProtectSet.para.reon_requency )
		{
			*reon_flag = 0;
			state = 0;
		}
		switch(state)
		{
			case 0:
				ReOnTimeDelay = 100;	//1s��ʱ
			  ReOnTimeOut = 0;
			  state = 1;
				break;
			case 1:
				if(ReOnTimeOut == 1)
				{
				  ReOnTimeDelay = 1500;	//15s��ʱ
			    ReOnTimeOut = 0;
			    state = 2;
				}
				break;
			case 2:						// ������ʱ
				if(ReOnLockCnt1 >= 15 && ReOnTimeOut == 1)
				{
					*reon_flag = 0;
					state = 0;
				}
        else if(KZQMeasureData.para.DLQflag.bit.breaker == 2)
				{				
					state = 3;
					ReOnDelay = ProtectSet.para.once_reon_time;	
					if(ReOnDelay<65000 && ReOnDelay>200)
						ReOnTimeDelay = ReOnDelay;	// �趨��ʱ2s~650s
					else
						ReOnTimeDelay = 200;	//2s��ʱ
					ReOnTimeOut = 0;
					ProtectTimesLock = 0;
					ProtectFlag = 0;
				}					
				break;
			case 3:					
				if(ReOnTimeOut == 1)
				{
					ReOnTimes = 0;
					*reon_flag = 0;
					state = 0;
					ReOnTimeOut = 0;
					if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && KZQMeasureData.para.DLQflag.bit.store == 1)	// ��λ���Ѵ���)	
					{
						for(i=0;i<8;i++)		// ������ޱ�����ʱ������־
						{
							if(ProtectDelay.word[i] != 0)	// ������Ч����˵��������Ȼ���ڣ������غ�բ
							{
								return 0;
							}
						}
						if(SwitchFlag == 0)
						{
							SwitchFlag = 4;		// ���Ͻ���������ش��ڷ�բ״ִ̬���غ�բ����
							RecordSwitchOn(TYPE_ACT_REON);
						}
					}
				}
				break;
			default:
				break;
		}
	}
	return 0;
}
			
void Fault_Curve_Send(void)
{
	u8 sum=0;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
	sum += 0xFE;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
	sum += 0xEF;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x01);			// �����豸����
	sum += 0x01;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x01);		// �����豸��ַ
	sum += 0x01;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xBA);				// ����������
	sum += 0xBA;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x05);				// ���ͳ��ȵ�λ 
	sum += 0x05;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ  

	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				

	InsertDataToQueue(&QueueTX2,BufferTX2,0x4B);				// ���ͳ��ȸ�λ  
	sum += 0x4B;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȵ�λ 
	sum += 0x00;
	InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
	InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
}

void Fault_101Curve_Send(void)
{
	u8 sum=0;
	InsertDataToQueue(&QueueTX1,BufferTX1,0x68);				// ��������ͷ
	InsertDataToQueue(&QueueTX1,BufferTX1,9);				    //����
	InsertDataToQueue(&QueueTX1,BufferTX1,9);		
  InsertDataToQueue(&QueueTX1,BufferTX1,0x68);				// ��������ͷ	
	InsertDataToQueue(&QueueTX1,BufferTX1,0x80);
  sum+=0x80;	
	InsertDataToQueue(&QueueTX1,BufferTX1,SystemSet.para.address);
	sum+=SystemSet.para.address;
	InsertDataToQueue(&QueueTX1,BufferTX1,56);//���ͱ�ʶ��56����=��������ͻ��
	sum+=56;
	InsertDataToQueue(&QueueTX1,BufferTX1,0x81);//�ɱ�ṹ�޶��ʣ���ϢԪ����Ŀ)
	sum+=0x81;
	InsertDataToQueue(&QueueTX1,BufferTX1,3);//<3>:=ͻ��<7>:=����ȷ��<9>:=ֹͣ����ȷ��<10>:=�������
	sum+=3;
	InsertDataToQueue(&QueueTX1,BufferTX1,SystemSet.para.address);
	sum+=SystemSet.para.address;
	
	InsertDataToQueue(&QueueTX1,BufferTX1,0);
	InsertDataToQueue(&QueueTX1,BufferTX1,0);
	
	InsertDataToQueue(&QueueTX1,BufferTX1,0x00);
  sum+=0x00;
	InsertDataToQueue(&QueueTX1,BufferTX1,sum&0xff);
	InsertDataToQueue(&QueueTX1,BufferTX1,0x16);
}

void Battery_Voltage_Save(void)
{
	u8 buf[3];
	u8 bufread[2];	
	u16 recordorder = 0;
	CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_BATVOLTAGE_ADDR+(Battery_Voltage_Index*FLASH_BATVOLTAGE_LENGTH),2);
	recordorder = bufread[0] | (bufread[1]<<8);
	if(recordorder == 0xFFFF)recordorder = 0;
	if(recordorder == Battery_Voltage_Order[Battery_Voltage_Index])
	{
		recordorder = Battery_Voltage_Order[Battery_Voltage_Index] +1;
		if(recordorder > 60000 || recordorder == 0)recordorder = 1;
		if(Battery_Voltage_Order[Battery_Voltage_Index] != 0)
		{
			if(Battery_Voltage_Index < (FLASH_BATVOLTAGE_MAX-1))
				Battery_Voltage_Index++;
			else
				Battery_Voltage_Index = 0;				
		}
		Battery_Voltage_Order[Battery_Voltage_Index] = recordorder;
	}
	buf[0] = Battery_Voltage_Order[Battery_Voltage_Index] & 0x00FF;
	buf[1] = Battery_Voltage_Order[Battery_Voltage_Index] >> 8;
	if(MeasureData.V_BAT>2400)
		buf[2] = 100;
	else
		buf[2] = MeasureData.V_BAT/24;
	CS2BZ=0;
	SPI_Flash_Write(buf,FLASH_BATVOLTAGE_ADDR+(Battery_Voltage_Index*FLASH_BATVOLTAGE_LENGTH),FLASH_BATVOLTAGE_LENGTH);
}	

void test_before(void)
{
	u8 crc_yhf=0,i=0;
	SSN_L();	
	SPI2_ReadWrite(0x55);														  //֡ͷ
	for(i=0;i<6;i++)
	{
		SPI2_ReadWrite(Get_Ter_Random[i]);	
		crc_yhf^=Get_Ter_Random[i];
	}
	crc_yhf=~crc_yhf;
	SPI2_ReadWrite(crc_yhf);	
	crc_yhf=0;					//����Ҫ
	SSN_H();											   //����Ƭѡ
	Ter_WriteRead_Spi1();

}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	u32 delay7022 = 0;
  System_Init();
	Reset_Value = RCC->CSR;
	//RCC_ClearFlag();
	HTCLK_OUT_1;  //I��Cͨ�ŷ�ʽ
	OUT_COM_OFF;		// �ر����������
  HE_OFF;
	FEN_OFF;
	KZCN_OFF;
	OUT4_OFF;      //�رռ̵���1
	OUT5_OFF;      //�رռ̵���2
	OUT6_ON;       //out6Ϊ���öˣ��ߵ�ƽ���ֳ���
	BATT_OFF;      //��ؿ��Ƽ̵����ر�
	//SC1161Y_OFF;   // SC1161YоƬ�ر�
	SC1161Y_ON;    // SC1161YоƬ����
	SPI_FLASH_CS_H;
	SPI_FLASH_CS2_H;
	ParaInit();			// ������ʼ��
	while(ATT7022_SIG != 0)// �ȴ���ȱ�����źű�Ϊ�͵�ƽ�������ʼ��
	{
		delay7022++;
		if(delay7022 > 100000)	// ��ʱ�˳�
			break;
	}
	ATT7022_Init();		  // ��ȱ��ʼ��
	if(Read_ATT7022() != 0x0D)		  // ��ȡATT7022У�����
	{
		Read_ATT7022BAK();
	}
	Write_ATT7022();
	ds1302_init();	
  AD7606Reset();      // ad7606��λ

	// ��·����һ��λ��״̬
	if(HW_IN2 == 0 && FW_IN1 == 1 )
	{
	 	BreakerStateLast = 1;		// ��բ״̬	
	}
	else if( FW_IN1 == 0 && HW_IN2 == 1 )
	{
		BreakerStateLast = 2;	    // ��բ״̬
	}
	else BreakerStateLast = 0;
	
	test_before();
	
  os_sys_init (Task); /* Initialize RTX and start init     */ 
	while(1);
}

/**************************************************************************************
* FunctionName   : Task1()  50ms
* Description    : ����1    
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task1(void)
{ 
	static u16 energy_cnt = 0;
	os_dly_wait (100);
	ReadPowerOffPara();	// ��ȡ����
	ReadMachineInformation();// ��ȡ������ź���������,��������
  if(SwitchCnt == 0xFFFF)
	  SwitchCnt = 0;

	if(ReadSystemPara() != 0x0D)	// ��ϵͳ����
	{
		if(ReadSystemParaBAK() == 0x0D)	// ����ϵͳ������������ϵͳ������ȷ�������±�������ϵͳ����
		{
			SaveSystemPara();
		}
	}	
	if(ReadProtectPara() != 0x0D)	// ��ȡ��������
	{
		if(ReadProtectParaBAK() == 0x0D)	// �������������������ݱ���������ȷ�������±���������������
		{
			SaveProtectPara();
		}
	}
	if(ReadAlarmPara() != 0x0D)	// ��ȡ��������
	{
		if(ReadAlarmParaBAK() == 0x0D)	// �������������������ݱ���������ȷ�������±���������������
		{
			SaveAlarmPara();
		}
	}
	if(ReadAutoSwitchPara() != 0x0D)	// ��ȡ�Զ����в�������
	{
		if(ReadAutoSwitchParaBAK() == 0x0D)	// �����Զ����в������������Զ����в�����ȷ�������±��������Զ����в���
		{
			SaveAutoSwitchPara();
		}
	}
	if(ReadModulus() != 0x0D)	// ��ȡ����ϵ��
	{
		if(ReadModulusBAK() == 0x0D)	// ��������ϵ���������ݲ���ϵ����ȷ�������±�����������ϵ��
		{
			SaveModulus();
		}
	}
	
	if(Read101Para() != 0x0D)	// ��ȡ101��Լ����
	{
		if(Read101ParaBAK() == 0x0D)	// ����101��Լ������������101��Լ������ȷ�������±�������101��Լ����
		{
			Save101Para();
		}
	}	
	
	ReadVersionfromFlash();	 // ��ȡ��������汾��Ϣ
	KZQUpdataSucced = ReadUpdateFlagfromFlash();
	ReadRecordIndexfromFlash();
	ReadAllLossSave();       //��ȡ����������ò���
	os_dly_wait (2000);
	while(1)
	{
		if(PowerOffCnt > 20)	// ���ʱ��Ϊ(0.02 && 0.05)��
		{
			//YabanSave_Flag = 1;			            // �������(��̫�ɿ�)
			KZQMeasureData.para.ProtectFlag2.bit.poweroff = 1;
		}
		
		if(WCN_IN3 == 1)				            // ����
		{
			KZQMeasureData.para.DLQflag.bit.store = 1;
			if(energy_cnt>0)
			{
				DLQZDinformation.para.store_time = energy_cnt/2;  //store_time/10 s
				MeasureData.datatype |= 0x80;
				energy_cnt = 0;
			}
		}
		else
		{
			KZQMeasureData.para.DLQflag.bit.store = 2;
			if(energy_cnt<1000)
			{
				energy_cnt++;
			}
		}
		os_dly_wait (50);
		os_evt_set (0x0001, t_Task17);
	}
}

/**************************************************************************************
* FunctionName   : Task2()	 ң�ű�λɨ�裬�ֺ�բ�ƿ���   10ms
* Description    : ����2	
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task2(void)
{
	u8 poscnt1=0,poscnt2=0,last_DJ_IN5=0,last_DJ_IN4=0;
	u16 Endflag = 0;
	os_dly_wait (2000);
	os_evt_set (0x0002, t_Task17);
	os_dly_wait (2000);
	Control.word = 3;
	while(1)
	{
		if(Endflag>0) Endflag--;
		if(Endflag==1) 
		{
			DJFZKZ_OFF;
			DJZZKZ_OFF;
		}		 
		if(LED_Judge == 0 && LED_State == 0x01)
		{
		  HZLED_ON;FZLED_OFF;
			if(last_DJ_IN5 == DJ_IN5)	
			  poscnt1++;
			if(poscnt1>5)
			{
			  Control.bit.in5 = DJ_IN5;
				Control.bit.flag1 = 1;
				poscnt1 = 0;
			}
			last_DJ_IN5 = DJ_IN5;
		}
		else if(LED_Judge == 0 && LED_State == 0x02)
		{
			HZLED_OFF;FZLED_ON;
			if(last_DJ_IN4 == DJ_IN4)	
			  poscnt2++;
			if(poscnt2>5)
			{
			  Control.bit.in4 = DJ_IN4;
				Control.bit.flag2 = 1;
				poscnt2 = 0;
			}
			last_DJ_IN4 = DJ_IN4;
		}
	
		if(Endflag==0)
		{
			if(Control.word == 0xC2)
			{   
					DJZZKZ_ON;
					DJFZKZ_OFF;
					Endflag=3000;
				  Control.word = 3;
			}
			else if(Control.word == 0xC0)
			{
					DJZZKZ_OFF;
					DJFZKZ_ON;
					Endflag=3000;
				  Control.word = 3;
			}					
		}
		os_dly_wait (10);
	  os_evt_set (0x0002, t_Task17);		
	}
}

/**************************************************************************************
* FunctionName   : Task3()	   1����� 		 1000ms
* Description    : ����3 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task3(void)
{
	static u8 gps_cnt=0;
	static u16 sigreset_endcnt=0;
	static u8 timer0 = 0;
	u8 i = 0;
	os_dly_wait (2000);
	DJZZKZ_OFF;       //�����ת���ƹ�
	DJFZKZ_OFF;       //�����ת���ƹ�
	GPRS_POW_OFF;
	GPRS_PWRKEY_OFF;
	WIFI_OFF;
	GPS_OFF;
	BATTERY_CHARGE_OFF;//��س��ر�

	while(1)
	{
    if(GPS_Flag)
		{
			if(gps_cnt == 0)
			{
				USART5_Configuration(9600);
			  WIFI_OFF;    //wifi��
			  GPS_ON;      //gps��
				GpsOn_Flag = 1;
			}
			gps_cnt++;
      if(gps_cnt > 60 || KZQMeasureData.para.SelfCheck.bit.GPS_state)
			{
			  WifiConnectState = 0;	     // wifiģ������������
				GPS_Flag = 0;
			}
		}
		else
			gps_cnt = 0;
		
		WifiConnectCtrl();
		#ifdef SIM800C_MODEL
		GprsConnectCtrl();
		#endif
		
		if(CodeTimeout>0) 
		{
			CodeTimeout--;
			if(CodeTimeout == 0)
			{
			  for(i=0;i<16;i++)
          AUTHORIZATIONcode[i]= 0;
				ControlLock = 0;
			}
		}
		
		if(Send_Flag == 1 && UpdateLength == 0)
		{
			timer0++;
			if(timer0 > 20)
			{
				timer0 = 0;
				Heart_Jump();
			}
		}
		else
			timer0 = 0;
		
		if(ReOnLockCnt < 10)	// �غ�բ������ʱ
		{
			ReOnLockCnt++;
		}
		//������Ͷ�룬�����ںϺ�״̬
		if(ProtectSet.para.once_reon_enable == 1 && KZQMeasureData.para.DLQflag.bit.breaker == 1)	// �غ�բ����ʱ
		{
			if(ReOnLockCnt1 < 15)
			  ReOnLockCnt1++;
		}
		else
		{
			ReOnLockCnt1 = 0;	
		}

		if(SigresetLock == 1 || KZQMeasureData.para.AlarmFlag.word || ProtectFlag == 1)
		{
		  sigreset_endcnt++;
			if(sigreset_endcnt > 300)
			{
				SigresetFlag=1;
				sigreset_endcnt = 0;
				SigresetLock = 0;
			}
		}
		
		if(Select101Addr || ControlSign)
		{
			if(Control_Hold>0)
				Control_Hold--;
			else
			{
				ControlSign = 0;
				Select101Addr=0;
				Control_Hold = SystemSet.para.control_time;
			}
		}
		else
			Control_Hold = SystemSet.para.control_time;
		
		if(renzheng_flag)
			timer_renzheng++;
		
		if(timer_renzheng>120)
		{
			zz_ok=0;
			wg_ok=0;
			timer_renzheng=0;
			renzheng_flag = 0;
		}

		if(Soesend_cnt > 0)	
		{
			Soesend_cnt--;
		}
		if(timer_enable)
			delay_time_101++;//�������������ʱ

		if(SystemSet.para.wifi_gps_switch == 1)	
		{
			SystemSet.para.wifi_gps_switch = 0xFF;
			SetSystemFlag=1;
			USART5_Configuration(9600);
			WIFI_OFF;    //wifi��
			GPS_ON;      //gps��
			GpsOn_Flag = 1;
		}
		else if(SystemSet.para.wifi_gps_switch == 0)	
		{
			SystemSet.para.wifi_gps_switch = 0xFF;
			SetSystemFlag=1;
			WifiConnectState = 0;	     // wifiģ������������
		}
		else if(SystemSet.para.wifi_gps_switch == 2)	
		{
			SystemSet.para.wifi_gps_switch = 0xFF;
			SetSystemFlag=1;
			WIFI_OFF;    //wifi��
			GPS_OFF;     //gps��
		}
		
		if(Freez_15min_cnt == 0)
		{
			Freez_15min_cnt =900;
			lockbz.bit.D1=1;
		}
		Freez_15min_cnt--;
		
		//����ģ���¼���¼�ж�
		if(MeasureData.PA_val<0 && abs(MeasureData.PA_val)>LineSet.para.P_Reverse_Limit)
		{
			if(PReverse.bit.PA_Reverse == 0)
			  LineLossNum.PA_Reverse_Delay++;
			else
				LineLossNum.PA_Reverse_Delay = 0;
			if(LineLossNum.PA_Reverse_Delay>LineSet.para.P_Reverse_Time)
		    PReverse.bit.PA_Reverse=1;
		}
		else  //���������ʷ����¼�
		{
			PReverse.bit.PA_Reverse=0;
			LineLossNum.PA_Reverse_Delay = 0;
		}
			
		if(MeasureData.PB_val<0 && abs(MeasureData.PB_val)>LineSet.para.P_Reverse_Limit)
	  {
			if(PReverse.bit.PB_Reverse == 0)
		    LineLossNum.PB_Reverse_Delay++;
			else
				LineLossNum.PB_Reverse_Delay = 0;
			if(LineLossNum.PB_Reverse_Delay>LineSet.para.P_Reverse_Time)
		    PReverse.bit.PB_Reverse=1;
	  }
		else //���������ʷ����¼�
		{
			PReverse.bit.PB_Reverse=0;
			LineLossNum.PB_Reverse_Delay = 0;
		}
		if(MeasureData.PC_val<0 && abs(MeasureData.PC_val)>LineSet.para.P_Reverse_Limit)
	  {
			if(PReverse.bit.PC_Reverse == 0)
		    LineLossNum.PC_Reverse_Delay++;
			else
				LineLossNum.PC_Reverse_Delay = 0;
			if(LineLossNum.PC_Reverse_Delay>LineSet.para.P_Reverse_Time)
		    PReverse.bit.PC_Reverse=1;	
	  }
		else //���������ʷ����¼�
		{
			PReverse.bit.PC_Reverse=0;
			LineLossNum.PC_Reverse_Delay = 0;
		}
		if(MeasureData.P_val<0 && abs(MeasureData.P_val)>LineSet.para.Pt_Reverse_Limit)
	  {
			if(PReverse.bit.P_Reverse == 0)
		    LineLossNum.P_Reverse_Delay1++;
			else
				LineLossNum.P_Reverse_Delay1 = 0;
			LineLossNum.P_Reverse_Delay2 = 0;
			if(LineLossNum.P_Reverse_Delay1>LineSet.para.Pt_Reverse_Time)
		    PReverse.bit.P_Reverse=1;	
	  }
		else if(MeasureData.P_val>0 && MeasureData.P_val>LineSet.para.Pt_Reverse_Limit)
	  {
			if(PReverse.bit.P_Reverse == 1)
		    LineLossNum.P_Reverse_Delay2++;
			else
				LineLossNum.P_Reverse_Delay2 = 0;
			LineLossNum.P_Reverse_Delay1 = 0;
			if(LineLossNum.P_Reverse_Delay2>LineSet.para.Pt_Reverse_Time)
		    PReverse.bit.P_Reverse=0;	
	  }
		else //��������������ı��¼�
		{
			PReverse.bit.P_Reverse=0;
			LineLossNum.P_Reverse_Delay1 = 0;
			LineLossNum.P_Reverse_Delay2 = 0;
		}
		
		os_dly_wait (1000); 
		os_evt_set (0x0004, t_Task17);
	}
}

/**************************************************************************************
* FunctionName   : Task4()	   ͨѶЭ�鴦������ 		 100ms
* Description    : ����4 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task4(void)
{ OS_RESULT xResult;
	os_dly_wait (2000);
	while(1)
	{ 	
		xResult = os_evt_wait_or(0xFF,100);
		switch(xResult)
		{
			case OS_R_EVT:
				if(Usart1RxReady == 1)
				{
					if(SystemSet.para.com_protocol == 88)
					{
					  CommandProcess_101_Encryption(3);
					}
					else
					{
						if(Measure101Para.para.encrypt == 0)
						{
							CommandProcess1(0);
						}
						else if(Measure101Para.para.encrypt == 1)
						{
							CommandProcess_101_Encryption(0);
						}
				  }
					Usart1RxReady = 0;
				}
				else if(Usart1bakRxReady == 1)
				{
					if(SystemSet.para.com_protocol == 88)
					{
					  CommandProcess_Encryption_Prepare(1);
					  CommandProcess_101_Encryption(3);
					}
					else
					{
						if(Measure101Para.para.encrypt == 0)
						{
							CommandProcess1Prepare();                 	// ����Ԥ����
							CommandProcess1(0);
						}
						else if(Measure101Para.para.encrypt == 1)
						{
							CommandProcess_Encryption_Prepare(1);
							CommandProcess_101_Encryption(0);
						}
				  }
					Usart1bakRxReady = 0;
				}
				if(Usart2RxReady == 1)                        // �����
				{
					CommandProcess2();
					Usart2RxReady = 0;
				}
				else if(Usart2bakRxReady == 1)
				{
					CommandProcess2Prepare();	                  // ����Ԥ����
					CommandProcess2();
					Usart2bakRxReady = 0;
				}
				if(Usart3RxReady == 1)                        // �����
				{
					IEC_104_CommandProcess(2);
					Usart3RxReady = 0;
				}
				else if(Usart3bakRxReady == 1)
				{
					IEC_104_CommandProcessPrepare();	          // ����Ԥ����
					IEC_104_CommandProcess(2);
					Usart3bakRxReady = 0;
				}
				if(Usart4RxReady == 1)
				{
					Usart4RxReady = 0;
					CommandProcess4(3);			// �����
				}
				else if(Usart4bakRxReady == 1)
				{
					Usart4bakRxReady = 0;
					CommandProcess4Prepare();
					CommandProcess4(3);
				}
				if(Usart5RxReady == 1)
				{
					Usart5RxReady = 0;
					if(GpsOn_Flag == 0)
					{
					  CommandProcess5();
					}
					else if(GpsOn_Flag == 1)
					{
					  CommandProcess_Gps();
					}
				}
				else if(Usart5bakRxReady == 1)
				{
					Usart5bakRxReady = 0;
					if(GpsOn_Flag == 0)
					{
					  CommandProcess5Prepare();
					  CommandProcess5();
					}
					else if(GpsOn_Flag == 1)
					{
					  CommandProcess_GpsPrepare();
					  CommandProcess_Gps();
					}
				}
				break;
			case OS_R_TMO:

				if(SystemSet.para.com_protocol == 88)
				{
					//û�г�ʼ����ң�أ����ȼ�ѹ�ƣ�
					if(y_104==0)
					{
						if((Super_Sign[1]==0)&&(Super_Sign[2]==0) && Callend_Flag)
						{
							//*********************ң��ң����������******************************
							if(Super_Sign[3]==0)
							{
								if(SOE_Send_PD() == 0x0D)
								{
									Super_Sign[3]=1;
									Active_Upload1(3);
								}
							}

							if(Soesend_cnt == 0 && Super_Sign[4] == 0)	
							{
								if(getfloat_yc_state())
								{
									Super_Sign[7]=1;
									Active_Upload1(3);
								}
								Soesend_cnt=Max_Time;	
							}
						}
						
						if(delay_time_101 > 15)  //101�����ط�����
							Send_Again(3);
					}
					else
					{
						Active_Upload(3);
					}
			  }
				else
				{
					if(Measure101Para.para.encrypt == 0)
					{
						TimeYcYx(0);            //��λң��ң��
					}
					else if(Measure101Para.para.encrypt == 1)
					{
						if(y_104==0)
						{
							//û�г�ʼ����ң�أ����ȼ�ѹ�ƣ�
							if((Super_Sign[1]==0)&&(Super_Sign[2]==0) && Callend_Flag)
							{
								//*********************ң��ң����������******************************
								if(Super_Sign[3]==0)
								{
									if(SOE_Send_PD() == 0x0D)
									{
										Super_Sign[3]=1;
										Active_Upload1(0);
									}
								}
								
								if(Soesend_cnt == 0 && Super_Sign[4] == 0)	
								{
									if(getfloat_yc_state())
									{
										Super_Sign[7]=1;
										Active_Upload1(0);
									}
									Soesend_cnt=Max_Time;	
								}
							}
							if(delay_time_101 > 15)
								Send_Again(0);
						}
						else
						{
							Active_Upload(0);
						}
					}
			  }
				break;
			default:
			  break;
		}
	  os_evt_set (0x0008, t_Task17);
	}
}

/**************************************************************************************
* FunctionName   : Task5()	   �������ݷ����ñ�־ 		300ms
* Description    : ����5
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task5(void)
{
	static u8 switch_flag = 0;
	static u16 switch_cnt = 0;
	static u16 switch_cnt1 = 0;
	static u8 PCurve_Readflag = 0;
	os_dly_wait (2000);
	while(1)
	{	
		if(DLQZD_flag != 0 && CtrlMeasureFlag == 0)
		{
			MeasureData.datatype |= 0x80;			
			switch(DLQZD_flag)
			{
				case 0x01:		// ������բ					
					KZQMeasureData.para.RequestFlag1.bit.curve_on_A = 1;	// ��λA���բ����
					KZQMeasureData.para.RequestFlag1.bit.curve_on_B = 1;	// ��λB���բ����
					KZQMeasureData.para.RequestFlag1.bit.curve_on_C = 1;	// ��λC���բ����
					break;
				case 0x02:		// ������բ
					KZQMeasureData.para.RequestFlag1.bit.curve_off_A = 1;	// ��λA���բ����
					KZQMeasureData.para.RequestFlag1.bit.curve_off_B = 1;	// ��λB���բ����
					KZQMeasureData.para.RequestFlag1.bit.curve_off_C = 1;	// ��C���բ����
					break;
				case 0x03:		// ������բ
					KZQMeasureData.para.RequestFlag1.bit.curve_protect_A = 1;	// ��λA�ౣ������
					KZQMeasureData.para.RequestFlag1.bit.curve_protect_B = 1;	// ��λB�ౣ������
					KZQMeasureData.para.RequestFlag2.bit.curve_protect_C = 1;	// ��λC�ౣ������
					break;
				default:
				  break;
			}	
			DLQZD_flag = 0;	
		}
		
		if(SetSystemFlag==1)
		{
			if(SystemParaCompare() != 0x0D)
			{
				SaveSystemPara();		// ����ϵͳ����
				SaveSystemParaBAK();	// ����ϵͳ��������
				write_time();
				Control_Hold = SystemSet.para.control_time;
				BattryCnt = SystemSet.para.battery_delay<<1;
			}
			SetSystemFlag=0;
		}
		else if(SetProtectFlag==1)
		{
			if(ProtectParaCompare() != 0x0D)
			{
			  SaveProtectPara();				// ���汣������
			  SaveProtectParaBAK();			// ���汣����������
			}
			SetProtectFlag=0;
		}
		else if(SetAlarmFlag==1)
		{
			if(AlarmParaCompare() != 0x0D)
			{
			  SaveAlarmPara();        // ���汨������
			  SaveAlarmParaBAK();		// ���汨������
			}
			SetAlarmFlag=0;
		}
		else if(SetAutoswitchFlag==1)
		{
			if(AutoswitchParaCompare() != 0x0D)
			{
			  SaveAutoSwitchPara();    // �����Զ����в���
			  SaveAutoSwitchParaBAK(); // �����Զ����в���	
			}
      SetAutoswitchFlag=0;			
		}
		else if(ModulusFlag==1)
		{
			if(ModulusParaCompare() != 0x0D)
			{
			  SaveModulus();  // �������ϵ��
			  SaveModulusBAK();
			}
			ModulusFlag=0;
		}
		else if(Set101ParaFlag==1)
		{
			if(Measure101ParaCompare() != 0x0D)
			{
			  Save101Para();  // ����101��Լ����
			  Save101ParaBAK();
			}
			Set101ParaFlag=0;
		}
		else if(YabanSave_Flag==1)
		{
			SavePowerOffPara();	// ����ѹ������
			YabanSave_Flag=0;
		}
		else if(KZQMeasureData.para.RequestFlag1.bit.record_on)
		{
			SaveRecordOn();// �����բ��¼
			KZQMeasureData.para.RequestFlag1.bit.record_on = 0;
		}
		else if(KZQMeasureData.para.RequestFlag1.bit.record_off)
		{
			SaveRecordOff();  // �����բ��¼
			KZQMeasureData.para.RequestFlag1.bit.record_off = 0;
		}
		else if(KZQMeasureData.para.RequestFlag1.bit.record_protect)
		{
			if((ProtectRecord_flag&0x0003) == 1)
			{
				SaveRecordProtect1(); // ���汣����¼						
			}
			else if((ProtectRecord_flag&0x0003) == 2)
			{
				SaveRecordProtect2(); // ���汣����¼
			}
			KZQMeasureData.para.RequestFlag1.bit.record_protect = 0;
		}
		else if(KZQMeasureData.para.RequestFlag1.bit.record_autofeed)
		{
			SaveRecordAutofeed();// ���������Զ�����¼
			KZQMeasureData.para.RequestFlag1.bit.record_autofeed = 0;
		}
		else if(AlarmRecord_flag == 1)// ���汨����¼
		{
			AlarmRecord_flag = 0;
			SaveRecordAlarm();
		}
		else if(ClearTJXX_Flag == 1)  //��ռ�¼��Ϣ
		{
			ClearTJXX_Flag = 0;
			ClearTJXX();
		}
		else if(MeasureData.Psaveflag==1)
		{
			if(PCurve_Readflag == 0)
			{
			  PCurve_Save();
		    JudgePoint_Flag = 1;	
				PCurve_Readflag = 1;
			}
			if(MeasureData.Psaveflash)
			{
				P1SCurve_Save(switch_cnt1);
				switch_cnt1++;
				if(switch_cnt1 > 8)
				{
					switch_cnt1 = 0;
					MeasureData.Psaveflash = 0;
					//MeasureData.Psaveflag = 0;  //��������ָ������ڴ����ݽ�Ѹ�ٸ���
				}
		  }
		}
		
		if(MeasureData.Psaveflag==0)
			PCurve_Readflag = 0;
		
    Lock_Event();   //�����¼���¼
		
		if(YXChange1.bit.D4 != PReverse.bit.PA_Reverse)	
		{
			YXChange1.bit.D4 = PReverse.bit.PA_Reverse;
			if(YXChange1.bit.D4==1) Save_Event_PAReverse(0);
			else Save_Event_PAReverse(1);
		}
		if(YXChange1.bit.D5 != PReverse.bit.PB_Reverse)	
		{
			YXChange1.bit.D5 = PReverse.bit.PB_Reverse;
			if(YXChange1.bit.D5==1) Save_Event_PBReverse(0);
			else Save_Event_PBReverse(1);
		}
		if(YXChange1.bit.D6 != PReverse.bit.PC_Reverse)	
		{
			YXChange1.bit.D6 = PReverse.bit.PC_Reverse;
			if(YXChange1.bit.D6==1) Save_Event_PCReverse(0);
			else Save_Event_PCReverse(1);
		}
		if(YXChange1.bit.D7 != PReverse.bit.P_Reverse)	
		{
			YXChange1.bit.D7 = PReverse.bit.P_Reverse;
			if(YXChange1.bit.D7==1) Save_Event_PReverse();
		}
	
		if(TESTFlag == 0)	// �������״̬
	  {
			if(SwitchFlag == 0 && WCN_IN3 == 1)
			{
				OUT_COM_OFF;
			}
		}

		if(TimeNow.hour == 11 && TimeNow.minute == 55 && TimeNow.second == 59 && switch_flag == 0)
		{
		  switch_flag = 1;
			USART5_Configuration(9600);
			WIFI_OFF;    //wifi��
			GPS_ON;      //gps��
			GpsOn_Flag = 1;
		}
		if(switch_flag == 1)
		  switch_cnt++;
		if(switch_cnt > 2000) //600s��wifi
		{
			switch_cnt = 0;
		  switch_flag = 0;
			WifiConnectState = 0;	     // wifiģ������������
		}
		
		os_evt_set (0x0010, t_Task17);
		os_dly_wait (300);
	}
}

/**************************************************************************************
* FunctionName   : Task6()    �������ݷ��ʹ���  15ms
* Description    : ����6 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task6(void)
{ u16 i = 0;
	u16 NumTemp = 0;
 	os_dly_wait (2000);
	while(1)
	{
		/****************����1���ݷ��ʹ���*****************/	
		if(GetQueueDataNum(&QueueTX1) > 0 )									// �жϷ��Ͷ����Ƿ�Ϊ��
		{
			NumTemp = GetQueueDataNum(&QueueTX1);
			 
			 if(DMA_GetCurrDataCounter(DMA2_Stream7) == 0)					// DMA��������
			 {
				 if(NumTemp > 299)
				 {	
						for(i=0;i<300;i++)
						{
					   DMATXbuffer1[i] = GetDataFromQueue(&QueueTX1,BufferTX1);	
						}
						DMA2_Stream7->NDTR = 300;							// ���δ����ֽ���Ϊ300
						DMA_Cmd(DMA2_Stream7, ENABLE);	 					// ʹ��DMA����
				 }
				 else
				 {
						
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer1[i] = GetDataFromQueue(&QueueTX1,BufferTX1);	
						}
						DMA2_Stream7->NDTR = NumTemp;							// ���δ����ֽ���ΪNumTemp
						DMA_Cmd(DMA2_Stream7, ENABLE);	 					// ʹ��DMA����
				 }
			 }
		}
	/****************����2���ݷ��ʹ���*****************/	
		if(GetQueueDataNum(&QueueTX2) > 0 && Usart2TxReady == 1)									// �жϷ��Ͷ����Ƿ�Ϊ��
		{
			 NumTemp = GetQueueDataNum(&QueueTX2);
			 
			 if(DMA_GetCurrDataCounter(DMA1_Stream6) == 0)					// DMA��������
			 {
				 if(NumTemp > 819)
				 {
						T485_2_TX;
						for(i=0;i<820;i++)
						{
								 DMATXbuffer2[i] = GetDataFromQueue(&QueueTX2,BufferTX2);	
						}
						DMA1_Stream6->NDTR = 820;							// ���δ����ֽ���Ϊ820
						DMA_Cmd(DMA1_Stream6, ENABLE);	 					// ʹ��DMA����
				 }
				 else
				 {
						T485_2_TX;
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer2[i] = GetDataFromQueue(&QueueTX2,BufferTX2);	
						}
						
						DMA1_Stream6->NDTR = NumTemp;							// ���δ����ֽ���ΪNumTemp
						DMA_Cmd(DMA1_Stream6, ENABLE);	 					// ʹ��DMA����
				 }
			 }
		}
	/****************����3���ݷ��ʹ���*****************/	
		if(GetQueueDataNum(&QueueTX3) > 0 )									// �жϷ��Ͷ����Ƿ�Ϊ��
		{
			 NumTemp = GetQueueDataNum(&QueueTX3);
			 
			 if(DMA_GetCurrDataCounter(DMA1_Stream3) == 0)					// DMA��������
			 {
				 if(NumTemp > 99)
				 {
						for(i=0;i<100;i++)
						{
								 DMATXbuffer3[i] = GetDataFromQueue(&QueueTX3,BufferTX3);	
						}
						DMA1_Stream3->NDTR = 100;							// ���δ����ֽ���Ϊ100
						DMA_Cmd(DMA1_Stream3, ENABLE);	 					// ʹ��DMA����
				 }
				 else
				 {
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer3[i] = GetDataFromQueue(&QueueTX3,BufferTX3);	
						}
						DMA1_Stream3->NDTR = NumTemp;							// ���δ����ֽ���ΪNumTemp
						DMA_Cmd(DMA1_Stream3, ENABLE);	 					// ʹ��DMA����
				 }
			 }
		}
	/****************����4���ݷ��ʹ���*****************/	
		if(GetQueueDataNum(&QueueTX4) > 0)									// �жϷ��Ͷ����Ƿ�Ϊ��
		{
			 NumTemp = GetQueueDataNum(&QueueTX4);
			 if(DMA_GetCurrDataCounter(DMA1_Stream4) == 0)					// DMA��������
			 {
				 if(NumTemp > 299)
				 {
						for(i=0;i<300;i++)
						{
								 DMATXbuffer4[i] = GetDataFromQueue(&QueueTX4,BufferTX4);	
						}
						DMA1_Stream4->NDTR = 300;							// ���δ����ֽ���Ϊ300
						DMA_Cmd(DMA1_Stream4, ENABLE);	 					// ʹ��DMA����
				 }
				 else
				 {
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer4[i] = GetDataFromQueue(&QueueTX4,BufferTX4);	
						}
						DMA1_Stream4->NDTR = NumTemp;							// ���δ����ֽ���ΪNumTemp
						DMA_Cmd(DMA1_Stream4, ENABLE);	 					// ʹ��DMA����
				 }
			 }
		}
	/****************����5���ݷ��ʹ���*****************/	
		if(GetQueueDataNum(&QueueTX5) > 0 )									// �жϷ��Ͷ����Ƿ�Ϊ��
		{
			 NumTemp = GetQueueDataNum(&QueueTX5);
			 if(DMA_GetCurrDataCounter(DMA1_Stream7) == 0)					// DMA��������
			 {
				 if(NumTemp > 419)
				 {
						for(i=0;i<420;i++)
						{
								 DMATXbuffer5[i] = GetDataFromQueue(&QueueTX5,BufferTX5);	
						}
						DMA1_Stream5->NDTR = 420;							    // ���δ����ֽ���Ϊ420
						DMA_Cmd(DMA1_Stream5, ENABLE);	 					// ʹ��DMA����
				 }
				 else
				 {
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer5[i] = GetDataFromQueue(&QueueTX5,BufferTX5);	
						}
						DMA1_Stream7->NDTR = NumTemp;							// ���δ����ֽ���ΪNumTemp
						DMA_Cmd(DMA1_Stream7, ENABLE);	 					// ʹ��DMA����
				 }
			 }
		}
	/****************����6���ݷ��ʹ���*****************/	
		if(GetQueueDataNum(&QueueTX6) > 0)									// �жϷ��Ͷ����Ƿ�Ϊ��
		{
			 NumTemp = GetQueueDataNum(&QueueTX6);
			 if(DMA_GetCurrDataCounter(DMA2_Stream6) == 0)					// DMA��������
			 {
				 if(NumTemp > 99)
				 {
						for(i=0;i<100;i++)
						{
								 DMATXbuffer6[i] = GetDataFromQueue(&QueueTX6,BufferTX6);	
						}
						DMA2_Stream6->NDTR = 100;							// ���δ����ֽ���Ϊ100
						DMA_Cmd(DMA2_Stream6, ENABLE);	 					// ʹ��DMA����
				 }
				 else
				 {
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer6[i] = GetDataFromQueue(&QueueTX6,BufferTX6);	
						}
						DMA2_Stream6->NDTR = NumTemp;							// ���δ����ֽ���ΪNumTemp
						DMA_Cmd(DMA2_Stream6, ENABLE);	 					// ʹ��DMA����
				 }
			 }
		}

		/****************����2���ݽ��մ���*****************/
		if(DMA_GetCurrDataCounter(DMA1_Stream5) == 50 && Usart2TxReady == 0)	// �жϴ����Ƿ���Է�������	485�ӿ�
		{
			Usart2TxReady = 1;	
		}
	
		os_evt_set (0x0020, t_Task17);
	  os_dly_wait (15);
	}
}

/**************************************************************************************
* FunctionName   : Task7()  ǰ����������¿���  1000ms
* Description    : ����7 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task7(void)
{
	os_dly_wait (2000);
	while(1)
	{
		switch(RecoverKZQBackupFlag)  	// ǰ����������¿���
		{
			case 0:	// �޸���
				
				break;
			case 1:	// ���ظ���
				RecoverKZQBackupFlag = 0;
				SetUpdateFlag(0x01);	// ���ظ���
			  os_dly_wait (100);
				__set_FAULTMASK(1);
				NVIC_SystemReset();
        //JumpToApp();
			  break;
			case 2:	// ���ݸ���
				RecoverKZQBackupFlag = 0;
				SetUpdateFlag(0x02);	// ���ݸ���
			  os_dly_wait (100);
			  //JumpToApp();
				__set_FAULTMASK(1);
				NVIC_SystemReset();
			  break;
			default:
				break;
		}
		os_evt_set (0x0040, t_Task17);
		os_dly_wait (1000);
	}
}

/**************************************************************************************
* FunctionName   : Task8()���¶Ⱥ�ʪ�Ⱥ�������һ�ε����ݣ������ֽڣ�
* Description    : ����8   10*1000ms = 10s ��һ��
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task8(void)
{
	u8 timeflag = 0;
	os_dly_wait (2000);
	while(1)
	{
		os_evt_set (0x0080, t_Task17);
    if(timeflag > 10)
		{
			HumiTest_Flag = 1;
			timeflag = 0;
		}
		timeflag++;
		os_dly_wait(1000);
	}
}

/**************************************************************************************
* FunctionName   : Task9()  
* Description    : ����9    1000ms
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task9(void)
{
	os_dly_wait (2000);
	while(1)  
	{
		os_evt_set (0x0100, t_Task17);
	  os_dly_wait (1000); 
  }	
}

/**************************************************************************************
* FunctionName   : Task10()    
* Description    : ����10 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task10(void)
{
	u8 ledtimecnt=0;
	os_dly_wait (2000);
	while(1)
	{
		if(TESTFlag == 0)	// �������״̬
	  {
		  if(LED_Judge)
		  {
			  os_dly_wait (10);
				if(LED_Judge)
				{
					if(KZQMeasureData.para.DLQflag.bit.breaker == Last_Breakerstate)
						ledtimecnt++;	
					if(ledtimecnt >5)
					{
						if(KZQMeasureData.para.DLQflag.bit.breaker==1){HZLED_ON;FZLED_OFF;}
						else if(KZQMeasureData.para.DLQflag.bit.breaker==2){HZLED_OFF;FZLED_ON;}
						else {HZLED_OFF;FZLED_OFF;}
						ledtimecnt = 0;
					}
					Control.word = 3;
				}
				Last_Breakerstate  = KZQMeasureData.para.DLQflag.bit.breaker;
			}
			else 
			{
				os_dly_wait (10);          //�ӳ�10msȥ��
				if(LED_Judge == 0)
				{
					if(Led_Reverse_Flag)
					{
						LED_State = 0x01;
						Led_Reverse_Flag = 0;
					}
					else if(Led_Reverse_Flag == 0)
					{
						LED_State = 0x02;
						Led_Reverse_Flag = 1;
					}
					os_dly_wait (500);
				}
			}
		}
		else
		{
			TestFlagCnt++;
			if(TestFlagCnt>60)
				TESTFlag = 0;
			os_dly_wait (1000);
		}		

		os_evt_set (0x0200, t_Task17);
	}
}

/**************************************************************************************
* FunctionName   : Task11()	    500ms
* Description    : ����11 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task11(void)
{
	static u8 minute_cnt = 0;
	static u8 second_cnt = 0;
	static u8 last_sectime = 0;
	static u8 SelfCheckAtt7022Cnt = 0;
	static u8 ctdx_cnt = 0;
	static u8 SW_flag = 0;
	u8 year,month,date,hour,minute,sec,buf[2];
	u32 time;
	u32 ATT7022_ID = 0;
	os_dly_wait (2000);
	while(1)
	{
		if(CTDX_Enable == 0)	// CT���߼���������߱�
		{
			if(MeasureData.Protect_IA_val>500 && MeasureData.Protect_IB_val>500 && MeasureData.Protect_IC_val>500)	// 
			{
				CTDX_Enable = 1;
				CTDX_Flag = 0;
				ctdx_cnt = 0;
			}
		}
		if(CTDX_Enable == 1)	// CT�������������߼��
		{
			if(MeasureData.Protect_IA_val<300 || MeasureData.Protect_IB_val<300 || MeasureData.Protect_IC_val<300)	// 
			{
				if(MeasureData.Protect_IA_val>500 || MeasureData.Protect_IB_val>500 || MeasureData.Protect_IC_val>500)	// 
				{
					if(KZQMeasureData.para.DLQflag.bit.breaker == 1)
					{
						if(ctdx_cnt > 2)
						{
							CTDX_Enable = 0;
							CTDX_Flag = 1;
						}
						else
							ctdx_cnt++;
					}
					else
						ctdx_cnt = 0;
				}
				else
					ctdx_cnt = 0;
			}
			else
				ctdx_cnt = 0;
		}

		if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && ProtectTimesLock != 0)	// ����״̬����λ�������������������բ��������	
		{
			ProtectTimesLockCnt++;
			if(ProtectTimesLockCnt>3)
			{
				ProtectTimesLockCnt = 0;
				ProtectTimesLock = 0;
			}
		}

		if(AlarmTimesLock != 0)	// ����״̬
		{
			AlarmTimesLockCnt++;
			if(AlarmTimesLockCnt>3)
			{
				AlarmTimesLockCnt = 0;
				AlarmTimesLock = 0;
			}
		}
		
	  if(TESTFlag == 0)	// �������״̬
	  {
			if( DYXH1 == 1 && DYXH2 == 1 && BattryCnt == 0 ) //��ѹ���ϵ磬��ضϵ絹��ʱ
			{
				OUT6_ON;
				BATT_ON;  
				BattryCnt = SystemSet.para.battery_delay<<1; //ϵͳ�������¼�ʱ��BattryCnt
				if(BattryCnt == 0) 
					BattryCnt = 1;
				BattryTimeOut = 0; 
			}
			else if(DYXH1 == 0 || DYXH2 == 0) //��ѹ���ϵ�
			{
				OUT6_ON;
				BATT_ON;  
				BattryCnt = 0;
				BattryTimeOut = 0;
			}

			if( BattryCnt > 0 )
			{
				BattryCnt--;
				if( BattryCnt == 0 )
				{
					BattryTimeOut = 1;
				}
			}
			
			if( BattryTimeOut == 1 )
			{
				BATT_OFF;
				BattryCnt = 1;     //��ֹ�̵�����������
			}
		}

		if(Battery_Readflag == 0 && TESTFlag == 0)
		  BATTERY_CHARGE_ON;
		if(ZeroTimeArrived == 1)	// ��ҹ���ʱ����������
		{
			ZeroTimeArrived = 0;
			Battery_RunTime++; //�����������
			buf[0] = Battery_RunTime & 0x00FF;
			buf[1] = Battery_RunTime >> 8;
			CS2BZ=0;
			SPI_Flash_Write(buf,FLASH_BATTERY_RUNTIME_ADDR,2);// д��flash
			Battery_Voltage_Save();	
									
			WpLast = 0;			// ������յ���
			WqLast = 0;	
			YabanSave_Flag = 1;			// ���浱ǰ����
			SystemParaChecked = 0;
			ProtectParaChecked = 0;
			AlarmParaChecked = 0;
			AutoswitchParaChecked = 0;
			MeasureParaChecked = 0;
			CorrectParaChecked = 0;
			Measure101ParaChecked = 0;
			KZQMeasureData.para.SelfCheck.bit.para_set = 0;
		}
		else if(ZeroTimeArrived == 2)
		{
			ZeroTimeArrived = 0;
			year = Gps.date%100;
			month = (Gps.date/100)%100;
			date = Gps.date/10000;
			time = (u32)Gps.time;
			sec = time%100;
			minute = (time/100)%100;
			hour = time/10000;
			if(year > 18 && year < 99)
					if(month > 0 && month < 13)
						if(date > 0 && date < 32)
							if(hour<24)
								if(minute<60)
									if(sec<60)
									{
										TimeNow.year = year;
										TimeNow.month = month;
										if(hour >= 16)
										{
											TimeNow.date = date+1;
										  TimeNow.hour = hour-16;
										}
										else
										{
											TimeNow.date = date;
											TimeNow.hour = hour+8;
										}
										TimeNow.minute = minute;
										TimeNow.second = sec;
										correct_time();
									}
		}
    if(TESTFlag == 0)	// �������״̬
	  {
			if((abs(MeasureData.IA_val - MeasureData.Protect_IA_val) > 300 && MeasureIAError == 0) ||
				 (abs(MeasureData.IB_val - MeasureData.Protect_IB_val) > 300 && MeasureIBError == 0) ||
				 (abs(MeasureData.IC_val - MeasureData.Protect_IC_val) > 300 && MeasureICError == 0) ||
					abs(MeasureData.UA_val - MeasureData.Protect_UA_val) > 500 ||
					abs(MeasureData.UB_val - MeasureData.Protect_UB_val) > 500 ||
					abs(MeasureData.UC_val - MeasureData.Protect_UC_val) > 500)	// ��ȱ�оƬ�Լ�
			{
				SelfCheckAtt7022Cnt++;
				if(SelfCheckAtt7022Cnt > 5)
				{
					SelfCheckAtt7022Cnt = 0;
					KZQMeasureData.para.SelfCheck.bit.att7022 = 1;
					if(SelfCheckErrCode == 0)
						SelfCheckErrCode = 0x0300;
					ATT7022_Init();		  // ��ȱ��ʼ��
					if(Read_ATT7022() != 0x0D)		  // ��ȡATT7022У�����
					{
						Read_ATT7022BAK();
					}
					Write_ATT7022();
				}	
			}
			else
			{
				SelfCheckAtt7022Cnt = 0;
				KZQMeasureData.para.SelfCheck.bit.att7022 = 0;
			}
	  }
		
		if(minute_cnt < 120)	// һ���Ӽ�鱣������
		{
			minute_cnt++;
		}
		else
		{
			CheckSystemPara();          // ���ϵͳ��������
			CheckProtectPara();			    // ��鱣����������
			CheckAlarmPara();           // ��鱨����������
			CheckAutoSwitchPara();      // ����Զ����в�������
			CheckModulus();             // ������ϵ��
			CheckMeasure101Para();      // ���101��Լ����
			if(saveATT7022_flag == 0)
			  Check_ATT7022();
			minute_cnt = 0;
		}
		
		if(SwitchFlag == 0 && CtrlMeasureFlag == 0)
	  {
		  //��ȡʱ��оƬ
		  read_time();	
			
			if(second_cnt == 0)
			{
				last_sectime = TimeNow.second;
			}
			second_cnt++;
			if(second_cnt > 5)
			{
				second_cnt = 0;
				if((last_sectime - TimeNow.second) == 0)
				  ds1302_init();
			}
	    // ��ȡ��ȱ�оƬ��Ϣ
		  ATT7022_ID = ReadAT7052(0x2A);
	    ATT7022_ID = 200 - 0.726*ATT7022_ID;
			MeasureData.Att7022Flag &= ~(0x001F);
			MeasureData.Att7022Flag |= (ReadAT7052(SFLAG)&0x0000001F);

			ATT7022_Freq = ReadAT7052(FREQ);
			MeasureData.freq = ATT7022_Freq/81.92;
			if(MeasureData.freq<4000 || MeasureData.freq>6000)
				MeasureData.freq = 0;
			else
				TIM_SetAutoreload(TIM4,(84000000/MeasureData.freq)-1);

	    ATT7022_UaRms = ReadAT7052(UARMS);
	    MeasureData.UA_val=ATT7022_UaRms/290;
			MeasureData.UAB_val=(long)MeasureData.UA_val*1732/1000;
			ATT7022_UbRms = ReadAT7052(UBRMS);
			MeasureData.UB_val=ATT7022_UbRms/290;
			MeasureData.UBC_val=(long)MeasureData.UB_val*1732/1000;
			ATT7022_UcRms = ReadAT7052(UCRMS);
			MeasureData.UC_val=ATT7022_UcRms/290;
			MeasureData.UCA_val=(long)MeasureData.UC_val*1732/1000;

			ATT7022_IaRms = ReadAT7052(IARMS);
				MeasureData.IA_val=ATT7022_IaRms/503; //Irms = (Vrms/2^13)/N ;N=58
			
			ATT7022_IbRms = ReadAT7052(IBRMS);
				MeasureData.IB_val=ATT7022_IbRms/503;//������ѹ�����˵����У׼�ͻ����
			
			ATT7022_IcRms = ReadAT7052(ICRMS);
				MeasureData.IC_val=ATT7022_IcRms/503;

			ATT7022_Pa = ReadAT7052(PA);
			if(ATT7022_Pa>0x800000)
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// �½��߷�ʽ
					KZQMeasureData.para.DLQflag.bit.directionA = 0;	// ����
				else
					KZQMeasureData.para.DLQflag.bit.directionA = 1;
				MeasureData.Att7022Flag |= 0x0020;				
				MeasureData.PA_val = (float)(ATT7022_Pa-0x1000000)/ATT7022_K;
			}
			else
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// �½��߷�ʽ
					KZQMeasureData.para.DLQflag.bit.directionA = 1;	// ����
				else
					KZQMeasureData.para.DLQflag.bit.directionA = 0;
				MeasureData.Att7022Flag &= ~0x0020;				
				MeasureData.PA_val = (float)ATT7022_Pa/ATT7022_K;
			}
			if(SystemSet.para.exchange_dycfzc == 1) MeasureData.PA_val = -MeasureData.PA_val;	// �½��ߣ�����������Ϊ��
			ATT7022_Pb = ReadAT7052(PB);
			if(ATT7022_Pb>0x800000)
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// �½��߷�ʽ
					KZQMeasureData.para.DLQflag.bit.directionB = 0;	// ����
				else
					KZQMeasureData.para.DLQflag.bit.directionB = 1;
				MeasureData.Att7022Flag |= 0x0040;			
				MeasureData.PB_val = (float)(ATT7022_Pb-0x1000000)/ATT7022_K;
			}
			else
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// �½��߷�ʽ
					KZQMeasureData.para.DLQflag.bit.directionB = 1;	// ����
				else
					KZQMeasureData.para.DLQflag.bit.directionB = 0;
				MeasureData.Att7022Flag &= ~0x0040;			
				MeasureData.PB_val = (float)ATT7022_Pb/ATT7022_K;
			}
			if(SystemSet.para.exchange_dycfzc == 1) MeasureData.PB_val = -MeasureData.PB_val;	// �½��ߣ�����������Ϊ��
			ATT7022_Pc = ReadAT7052(PC);
			if(ATT7022_Pc>0x800000)
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// �½��߷�ʽ
					KZQMeasureData.para.DLQflag.bit.directionC = 0;	// ����
				else
					KZQMeasureData.para.DLQflag.bit.directionC = 1;
				MeasureData.Att7022Flag |= 0x0080;			
				MeasureData.PC_val = (float)(ATT7022_Pc-0x1000000)/ATT7022_K;
			}
			else
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// �½��߷�ʽ
					KZQMeasureData.para.DLQflag.bit.directionC = 1;	// ����
				else
					KZQMeasureData.para.DLQflag.bit.directionC = 0;
				MeasureData.Att7022Flag &= ~0x0080;			
				MeasureData.PC_val = (float)ATT7022_Pc/ATT7022_K;
			}
			if(SystemSet.para.exchange_dycfzc == 1) MeasureData.PC_val = -MeasureData.PC_val;	// �½��ߣ�����������Ϊ��
			ATT7022_Qa = ReadAT7052(QA);
			if(ATT7022_Qa>0x800000)
			{
				MeasureData.QA_val = (ATT7022_Qa-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.QA_val = ATT7022_Qa/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.QA_val = -MeasureData.QA_val;	// �½��ߣ�����������Ϊ��
			ATT7022_Qb = ReadAT7052(QB);
			if(ATT7022_Qb>0x800000)
			{
				MeasureData.QB_val = (ATT7022_Qb-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.QB_val = ATT7022_Qb/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.QB_val = -MeasureData.QB_val;	// �½��ߣ�����������Ϊ��
			ATT7022_Qc = ReadAT7052(QC);
			if(ATT7022_Qc>0x800000)
			{
				MeasureData.QC_val = (ATT7022_Qc-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.QC_val = ATT7022_Qc/ATT7022_K;
			
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.QC_val = -MeasureData.QC_val;	// �½��ߣ�����������Ϊ��
			//����S ��ȡ
			ATT7022_Sa = ReadAT7052(SA);
			if(ATT7022_Sa>0x800000)
			{
				MeasureData.SA_val = (ATT7022_Sa-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.SA_val = ATT7022_Sa/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.SA_val = -MeasureData.SA_val;	// �½��ߣ�����������Ϊ��
			
			ATT7022_Sb = ReadAT7052(SB);
			if(ATT7022_Sb>0x800000)
			{
				MeasureData.SB_val = (ATT7022_Sb-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.SB_val = ATT7022_Sb/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.SB_val = -MeasureData.SB_val;	// �½��ߣ�����������Ϊ��
			ATT7022_Sc = ReadAT7052(SC);
			if(ATT7022_Sc>0x800000)
			{
				MeasureData.SC_val = (ATT7022_Sc-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.SC_val = ATT7022_Sc/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.SC_val = -MeasureData.SC_val;	// �½��ߣ�����������Ϊ��
			ATT7022_Sa = ReadAT7052(ST);
			if(ATT7022_St>0x800000)
			{
				MeasureData.S_val = (ATT7022_St-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.S_val = ATT7022_St/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.S_val = -MeasureData.S_val;	// �½��ߣ�����������Ϊ��
			
			ATT7022_Pfa = ReadAT7052(PFA);
			if(ATT7022_Pfa>0x800000)
			{
				MeasureData.cosA = (ATT7022_Pfa-0x1000000)/8388.608;
			}
			else
				MeasureData.cosA = ATT7022_Pfa/8388.608;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.cosA = -MeasureData.cosA;	// �½��ߣ�����������Ϊ��
			ATT7022_Pfb = ReadAT7052(PFB);
			if(ATT7022_Pfb>0x800000)
			{
				MeasureData.cosB = (ATT7022_Pfb-0x1000000)/8388.608;
			}
			else
				MeasureData.cosB = ATT7022_Pfb/8388.608;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.cosB = -MeasureData.cosB;	// �½��ߣ�����������Ϊ��
			ATT7022_Pfc = ReadAT7052(PFC);
			if(ATT7022_Pfc>0x800000)
			{
				MeasureData.cosC = (ATT7022_Pfc-0x1000000)/8388.608;
			}
			else
				MeasureData.cosC = ATT7022_Pfc/8388.608;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.cosC = -MeasureData.cosC;	// �½��ߣ�����������Ϊ��

			ATT7022_Pt = ReadAT7052(PT);
			if(ATT7022_Pt>0x800000)
			{
				MeasureData.P_val = (float)(ATT7022_Pt-0x1000000)*2/ATT7022_K;
			}
			else
				MeasureData.P_val = (float)ATT7022_Pt*2/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.P_val = -MeasureData.P_val;	// �½��ߣ�����������Ϊ��

			ATT7022_Qt = ReadAT7052(QT);
			if(ATT7022_Qt>0x800000)
			{
				MeasureData.Q_val = (ATT7022_Qt - 0x1000000)*2/ATT7022_K;
			}
			else
				MeasureData.Q_val = ATT7022_Qt*2/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.Q_val = -MeasureData.Q_val;	// �½��ߣ�����������Ϊ��

			ATT7022_Pft = ReadAT7052(PFT);
			if(ATT7022_Pft>0x800000)
			{
				ATT7022_Pft = ATT7022_Pft - 0x1000000;
				MeasureData.cosALL = ATT7022_Pft/8388.608;
			}
			else 
				MeasureData.cosALL = ATT7022_Pft/8388.608;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.cosALL = -MeasureData.cosALL; // �½��ߣ�����������Ϊ��

			ATT7022_Ept = ReadAT7052(EPT);
			WpLast += ATT7022_Ept;
			MeasureData.Wp = (float)WpLast*(float)ATT7022_EPTK;
			if(KZQMeasureData.para.DLQflag.bit.directionA == 0)Wp_all_Last += ATT7022_Ept;			// ��һ�����й�����
			else WpFX_all_Last += ATT7022_Ept; 
			MeasureData.Wp_all = (float)Wp_all_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.Wp_all > 100000000)Wp_all_Last = 0;
			MeasureData.WpFX_all = (float)WpFX_all_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.WpFX_all > 100000000)WpFX_all_Last = 0;

			ATT7022_Eqt = ReadAT7052(EQT);
			WqLast += ATT7022_Eqt;
			MeasureData.Wq = (float)WqLast*(float)ATT7022_EPTK;
			if(KZQMeasureData.para.DLQflag.bit.directionA == 0)
			{
				if(MeasureData.Q_val > 0)	// ��һ����
				{
				Wq_1all_Last += ATT7022_Eqt;			// ��һ�����޹�����	
				}
				else // ��������
				{
				Wq_4all_Last += ATT7022_Eqt;			// ��һ�����޹�����	
				}
			}
			else
			{
				if(MeasureData.Q_val > 0)	// �ڶ�����
				{
				Wq_2all_Last += ATT7022_Eqt;			// ��һ�����޹�����	
				}
				else // ��������
				{
				Wq_3all_Last += ATT7022_Eqt;			// ��һ�����޹�����	
				}
			} 
			MeasureData.Wq_1all = (float)Wq_1all_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.Wq_1all > 100000000)Wq_1all_Last = 0;
			MeasureData.Wq_2all = (float)Wq_2all_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.Wq_2all > 100000000)Wq_2all_Last = 0;
			MeasureData.Wq_3all = (float)Wq_3all_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.Wq_3all > 100000000)Wq_3all_Last = 0;
			MeasureData.Wq_4all = (float)Wq_4all_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.Wq_4all > 100000000)Wq_4all_Last = 0;

			ATT7022_Est = ReadAT7052(EST);

			if(PeriodMode[TimeNow.hour] == 1)	// ��ʱ�ε���ͳ��
			{
				if(KZQMeasureData.para.DLQflag.bit.directionA == 0)Wp_peak_Last += ATT7022_Ept;		// ��һ�η��й�����
				else WpFX_peak_Last += ATT7022_Ept;
			}
			else if(PeriodMode[TimeNow.hour] == 2)// ��ʱ�ε���ͳ��
			{
				if(KZQMeasureData.para.DLQflag.bit.directionA == 0)Wp_valley_Last += ATT7022_Ept;		// ��һ�ι��й�����
				else WpFX_valley_Last += ATT7022_Ept;
			}
			else if(PeriodMode[TimeNow.hour] == 3)// ƽʱ�ε���ͳ��
			{
				if(KZQMeasureData.para.DLQflag.bit.directionA == 0)Wp_level_Last += ATT7022_Ept;		// ��һ��ƽ�й�����
				else WpFX_level_Last += ATT7022_Ept;
			}
			MeasureData.Wp_peak = (float)Wp_peak_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.Wp_peak > 100000000)Wp_peak_Last = 0;
			MeasureData.WpFX_peak = (float)WpFX_peak_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.WpFX_peak > 100000000)WpFX_peak_Last = 0;
			MeasureData.Wp_valley = (float)Wp_valley_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.Wp_valley > 100000000)Wp_valley_Last = 0;
			MeasureData.WpFX_valley = (float)WpFX_valley_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.WpFX_valley > 100000000)WpFX_valley_Last = 0;
			MeasureData.Wp_level = (float)Wp_level_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.Wp_level > 100000000)Wp_level_Last = 0;
			MeasureData.WpFX_level = (float)WpFX_level_Last*(float)ATT7022_EPTK/1000;
			if(MeasureData.WpFX_level > 100000000)WpFX_level_Last = 0;
		}
		
		if(SW_flag == 0)
		{
		  SW_flag = 1;
		  EXIT_WATCHDOG_0;		//�ⲿ���Ź�ι��
		}
		else
		{
		  SW_flag = 0;
		  EXIT_WATCHDOG_1;
		}
		os_evt_set (0x0400, t_Task17);
		os_dly_wait (500);
	}
}

/**************************************************************************************
* FunctionName   : Task12()	    // 1ms
* Description    : ����12 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task12(void)
{
	static u8 breaker_changecnt = 0;
	os_dly_wait (2000);
	YXChangeInit();     // ң�ű�λ��ʼ��
	while(1)
	{		
		if(KZQMeasureData.para.DLQflag.bit.breaker != 0)	// �жϿ���λ���쳣�䶯
		{
			if(BreakerStateLast != KZQMeasureData.para.DLQflag.bit.breaker)
				breaker_changecnt ++;
			if(BreakerStateLast != KZQMeasureData.para.DLQflag.bit.breaker && breaker_changecnt == 10)
			{
				if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && BreakerStateLast == 2)
				{
					SwitchCnt ++;
					Max_current_after_cnt = 0;
				}
				if(SwitchFlag == 0)	// �ǳ������
				{
					if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && BreakerStateLast != 0)	// �ֶ���բ�¼�
					{
						if(KZQMeasureData.para.RequestFlag1.bit.record_on == 0)
						{
							RecordSOE_Pad(SOE_ADDR_ACT_MAN_ON_pad,1);
						  RecordSwitchOn(TYPE_ACT_MAN_ON);// �ֶ���բ��¼
						  Record_on.para.type |= 0x80;
						  SaveRecordOn();// �����բ��¼
						}
					}
					else if(KZQMeasureData.para.DLQflag.bit.breaker == 2 && BreakerStateLast != 0) // �ֶ���բ�¼�
					{
						if(KZQMeasureData.para.RequestFlag1.bit.record_off == 0)
						{
							RecordSOE_Pad(SOE_ADDR_ACT_MAN_OFF_pad,1);
						  RecordSwitchOff(TYPE_ACT_MAN_OFF);  // �ֶ���բ��¼
						  Record_off.para.type |= 0x80;
						  SaveRecordOff();  // �����բ��¼
						}
					}
				}
				BreakerStateLast = KZQMeasureData.para.DLQflag.bit.breaker;
				breaker_changecnt = 0;
			}
		}
		
		if(DYXH1 == 1 && DYXH2 == 1)  //����
		{
			if(PowerOffCnt < 20000)
				PowerOffCnt++;
		}
		else
		{
			PowerOffCnt = 0;
		}
		if(SwitchFlag == 0)
      MeasureDataCalcAndProcess();
		CtrlReOn(&ReOnFlag);          //�غ�բ�ж�
		if(TESTFlag == 0)
	    StoreOutput();                //�����ж�
		SwapYXChange();	   	          // SOEң�ű�λɨ��
		os_evt_set (0x0800, t_Task17);
		os_dly_wait (1);
	}
}

/**************************************************************************************
* FunctionName   : Task13()	  ADCת����ȡ  10ms
* Description    : ����13 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task13(void)
{
	u8 i = 0;
	s32 temp = 0;
	static u8 Avr_cnt = 0;
	static u32 UptA_avr = 0;
	static u32 UptB_avr = 0;
	static u32 UptC_avr = 0;
	static u32 UA_avr = 0;
	static u32 UB_avr = 0;
	static u32 UC_avr = 0;
	static u32 I0_10_avr = 0;
	static u32 Ug_avr = 0;
	static u32 VBAT_avr = 0;
	
	static u32 Final_UptA_avr = 0;
	static u32 Final_UptB_avr = 0;
	static u32 Final_UptC_avr = 0;
	static u32 Final_UA_avr = 0;
	static u32 Final_UB_avr = 0;
	static u32 Final_UC_avr = 0;
//	static u32 Final_I0_10_avr = 0;
//	static u32 Final_Ug_avr = 0;

	static u32  MeasureData_UA1sum = 0;
	static u32  MeasureData_UB1sum = 0;
	static u32  MeasureData_UC1sum = 0;
	static u32  MeasureData_UAsum  = 0;
	static u32  MeasureData_UBsum  = 0;
	static u32  MeasureData_UCsum  = 0;
	//static u32  MeasureData_PI0sum  = 0;
	static u32  MeasureData_UABsum = 0;
	static u32  MeasureData_UBCsum = 0;
	static u32  MeasureData_UCAsum = 0;
	
	os_dly_wait (2000);
	while(1)
	{
		for(i=0;i<100;i++)
		{
			UptA_avr += DMAADCbuffer1[i][0];
			UptB_avr += DMAADCbuffer1[i][1];
			UptC_avr += DMAADCbuffer1[i][2];
			UA_avr   += DMAADCbuffer1[i][3];
			UB_avr   += DMAADCbuffer1[i][4];
		  UC_avr   += DMAADCbuffer1[i][5];
			I0_10_avr+= DMAADCbuffer1[i][6];
		  Ug_avr   += DMAADCbuffer1[i][7];
		}
		Avr_cnt++;
		if(Avr_cnt > 19)
		{
			Avr_cnt = 0;
			Final_UptA_avr = UptA_avr  / 2000;
			Final_UptB_avr = UptB_avr  / 2000;
			Final_UptC_avr = UptC_avr  / 2000;
			Final_UA_avr =   UA_avr    / 2000;
			Final_UB_avr =   UB_avr    / 2000;
			Final_UC_avr =   UC_avr    / 2000;
//			Final_I0_10_avr =I0_10_avr / 2000;
//			Final_Ug_avr =   Ug_avr    / 2000;
			
			UptA_avr = 0;
			UptB_avr = 0;
			UptC_avr = 0;
			UA_avr = 0;
			UB_avr = 0;
			UC_avr = 0;
			I0_10_avr = 0;
			Ug_avr = 0;
		}
		
//		if(abs(Final_Ug_avr-2048)>200)
//		{
//			Final_Ug_avr = 2048;		
//		}
//		
//		if(Final_UptA_avr < 2000 || Final_UptA_avr > 2200)
//		{
//			Final_UptA_avr = Final_Ug_avr;
//		}
//		if(Final_UptB_avr < 2000 || Final_UptB_avr > 2200)
//		{
//			Final_UptB_avr = Final_Ug_avr;
//		}
//		if(Final_UptC_avr < 2000 || Final_UptC_avr > 2200)
//		{
//			Final_UptC_avr = Final_Ug_avr;
//		}
//		if(Final_UA_avr < 2000 || Final_UA_avr > 2200)
//		{
//			Final_UA_avr = Final_Ug_avr;
//		}
//		if(Final_UB_avr < 2000 || Final_UB_avr > 2200)
//		{
//			Final_UB_avr = Final_Ug_avr;
//		}
//		if(Final_UC_avr < 2000 || Final_UC_avr > 2200)
//		{
//			Final_UC_avr = Final_Ug_avr;
//		}
//		if(Final_I0_10_avr < 2000 || Final_I0_10_avr > 2200)
//		{
//			Final_I0_10_avr = Final_Ug_avr;
//		}
	
	  MeasureData_UA1sum = 0;
	  MeasureData_UB1sum = 0;
	  MeasureData_UC1sum = 0;
	  MeasureData_UAsum  = 0;
	  MeasureData_UBsum  = 0;
	  MeasureData_UCsum  = 0;
	  //MeasureData_PI0sum  = 0;
	  MeasureData_UABsum = 0;
	  MeasureData_UBCsum = 0;
	  MeasureData_UCAsum = 0;
		VBAT_avr = 0;
		
		for(i=0;i<100;i++)
		{
			temp = (DMAADCbuffer1[i][0]-Final_UptA_avr);
			MeasureData_UA1sum += temp*temp;
			temp = (DMAADCbuffer1[i][1]-Final_UptB_avr);
			MeasureData_UB1sum += temp*temp;
			temp = (DMAADCbuffer1[i][2]-Final_UptC_avr);
			MeasureData_UC1sum += temp*temp;
			temp = (DMAADCbuffer1[i][3]-Final_UA_avr);
			MeasureData_UAsum += temp*temp;
			temp = (DMAADCbuffer1[i][4]-Final_UB_avr);
			MeasureData_UBsum += temp*temp;
			temp = (DMAADCbuffer1[i][5]-Final_UC_avr);
			MeasureData_UCsum += temp*temp;
			temp = (DMAADCbuffer1[i][3]-Final_UA_avr)-(DMAADCbuffer1[i][4]-Final_UB_avr);
			MeasureData_UABsum += temp*temp;
			temp = (DMAADCbuffer1[i][4]-Final_UB_avr)-(DMAADCbuffer1[i][5]-Final_UC_avr);
			MeasureData_UBCsum += temp*temp;
			temp = (DMAADCbuffer1[i][5]-Final_UC_avr)-(DMAADCbuffer1[i][3]-Final_UA_avr);
			MeasureData_UCAsum += temp*temp;
			//temp = (DMAADCbuffer1[i][6]-Final_I0_10_avr);
			//MeasureData_PI0sum += temp*temp;
			
			VBAT_avr += DMAADCbuffer1[i][8];
		}
		
		MeasureData.UA1sum = MeasureData_UA1sum;
	  MeasureData.UB1sum = MeasureData_UB1sum;
	  MeasureData.UC1sum = MeasureData_UC1sum;
	  MeasureData.UAsum  = MeasureData_UAsum;
	  MeasureData.UBsum  = MeasureData_UBsum;
	  MeasureData.UCsum  = MeasureData_UCsum;
	  //MeasureData.PI0sum  = MeasureData_PI0sum;
	  MeasureData.UABsum = MeasureData_UABsum;
	  MeasureData.UBCsum = MeasureData_UBCsum;
	  MeasureData.UCAsum = MeasureData_UCAsum;
		
		MeasureData.V_BAT = VBAT_avr/(u8)(CV_BAT+(s8)(((s32)269900 - (s32)VBAT_avr) / (s32)10000)*3);
		os_evt_set (0x1000, t_Task17);
		os_dly_wait (10);
	}
}

/**************************************************************************************
* FunctionName   : Task14()	    
* Description    : ����14 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task14(void)
{
	u8 over_u0_cnt = 0;
	u8 over_i0_cnt1 = 0;
	u8 over_i0_cnt2 = 0;
	u32 i0_avg = 0;
	u16 i = 0,j = 0;
	s16 U0bak_value[8];
	s16 I0bak_value[8];
//  u8 k = 0;
//	s16 point1 = 0;
//	u8 bufread[1200];
//	s16 U0_value[100];
//	s16 I0_value[100];
//	s32 total_value[10];
	s32 dycfzc_value = 0;
	os_dly_wait (2000);
	while(1)
	{
		if(JudgePoint_Flag==1)
		{
			JudgePoint_Flag = 0;
			
			for(i=0;i<12;i++)
			{
				 for(j=0;j<100;j++)
				 {
					 PcurveU0_bak[i*100+j]=PcurveU0_bak[i*100+j]-PcurveU0_bak[j];
				 }
			}
			
			if(jdxz == 2)
			{
			  for(i=200;i<1000;i++)
				{
					if(abs(PcurveU0_bak[i])>100)
					{
						if(abs(PcurveU0_bak[i+1])>100)
						{
							for(j=0;j<10;j++)
							{
								if(abs(PcurveU0_bak[i-j-1])<25)
								{
									JudgePoint = i-j-1;
									break;
								}
							}
							if(j >= 10)
							{
								JudgePoint = i-11;
							}
							break;
						}
					}
				}	
			}
			else
			{
				for(i=200;i<1000;i++)
				{
					if(abs(PcurveU0_bak[i])>800)
					{
						if(abs(PcurveU0_bak[i+1])>800)
						{
							for(j=0;j<10;j++)
							{
								if(abs(PcurveU0_bak[i-j-1])<200)
								{
									JudgePoint = i-j-1;
									break;
								}
							}
							if(j >= 10)
							{
								JudgePoint = i-11;
							}
							break;
						}
					}
				}
		  }
			over_u0_cnt = 0;
			i0_avg = 0;
			for(i=0;i<8;i++)
			{
				U0bak_value[i] = 0;
				I0bak_value[i] = 0;
				if(PcurveU0_bak[JudgePoint+i] > 0)
				  over_u0_cnt++;
				i0_avg+=abs(PcurveI0_bak[JudgePoint+i]);
			}
			if(over_u0_cnt > 5)
			{
				for(i=0;i<8;i++)
				{
					if(PcurveU0_bak[JudgePoint+i] > 0)
					  U0bak_value[i] = PcurveU0_bak[JudgePoint+i];	
				}
			}
			else
			{
				for(i=0;i<8;i++)
				{
					if(PcurveU0_bak[JudgePoint+i] < 0)
					  U0bak_value[i] = PcurveU0_bak[JudgePoint+i];	
				}
			}
      over_i0_cnt1 = 0;
			over_i0_cnt2 = 0;
			for(i=0;i<8;i++)
			{
				if(abs(PcurveI0_bak[JudgePoint+i])*8 > i0_avg)
					I0bak_value[i] = PcurveI0_bak[JudgePoint+i];	
				if(I0bak_value[i] > 0)
				  over_i0_cnt1++;
				else if(I0bak_value[i] < 0)
					over_i0_cnt2++;
			}
			if(over_i0_cnt1 > over_i0_cnt2)
			{
				for(i=0;i<8;i++)
				{
					if(I0bak_value[i] < 0)
					  I0bak_value[i] = 0;	
				}
			}
			else
			{
				for(i=0;i<8;i++)
				{
					if(I0bak_value[i] > 0)
					  I0bak_value[i] = 0;	
				}
			}
			dycfzc_value = 0;
			for(i=0;i<8;i++)
			{
				dycfzc_value += (s32)U0bak_value[i]*(s32)I0bak_value[i]; 
			}
			if(dycfzc_value > 0)
			{
				dycfzc = 1;
			}
			else
			{
				dycfzc = 2;
			}
		}	
		
//		for(i=0;i<10;i++)
//		{
//			SPI_Flash_Read(bufread,FLASH_P1SCURVE_ADDR+i*FLASH_P1SCURVE_LENGTH+15100,1200);
//			for(j=10;j<600;j++)
//			{
//				point1 = bufread[2*j] | (bufread[2*j+1]<<8);
//				if(abs(point1)>800)
//				{
//					point1 = bufread[2*j+2] | (bufread[2*j+3]<<8);
//					if(abs(point1) > 800)
//					{
//						for(k=0;k<10;k++)
//						{
//						  point1 = bufread[2*j-2*k-2] | (bufread[2*j-2*k-1]<<8);
//						  if(abs(point1) < 200)
//						  {
//						    JudgePoint = j-k-1+300;
//						    break;
//						  }
//					  }
//						if(k >= 10)
//						{
//							JudgePoint = i-11;
//						}
//						break;
//					}
//				}
//			}
//			total_value[i] = 0;
//			//JudgePoint = 500;
//			SPI_Flash_Read(bufread,FLASH_P1SCURVE_ADDR+i*FLASH_P1SCURVE_LENGTH+JudgePoint*2+14500,20);
//			for(j=0;j<10;j++)
//			{
//				U0_value[10*i+j] = bufread[2*j] | (bufread[2*j+1]<<8);
//				//U0_value[j] = bufread[2*j] | (bufread[2*j+1]<<8);
//			}
//			
//			SPI_Flash_Read(bufread,FLASH_P1SCURVE_ADDR+i*FLASH_P1SCURVE_LENGTH+JudgePoint*2+16900,20);
//			for(j=0;j<10;j++)
//			{
//				I0_value[10*i+j] = bufread[2*j] | (bufread[2*j+1]<<8);
//				//I0_value[j] = bufread[2*j] | (bufread[2*j+1]<<8);
//			}
//			
//			over_u0_cnt = 0;
//			i0_avg = 0;
//			for(j=0;j<8;j++)
//			{
//				U0bak_value[j] = 0;
//				I0bak_value[j] = 0;
//				if(U0_value[10*i+j] > 0)
//				  over_u0_cnt++;
//				i0_avg+=abs(I0_value[10*i+j]);
//			}
//			if(over_u0_cnt > 5)
//			{
//				for(j=0;j<8;j++)
//				{
//					if(U0_value[10*i+j] > 0)
//					  U0bak_value[j] = U0_value[10*i+j];	
//				}
//			}
//			else
//			{
//				for(j=0;j<8;j++)
//				{
//					if(U0_value[10*i+j] < 0)
//					  U0bak_value[j] = U0_value[10*i+j];	
//				}
//			}
//			
//			over_i0_cnt1 = 0;
//			over_i0_cnt2 = 0;			
//			for(j=0;j<8;j++)
//			{
//				if(abs(I0_value[10*i+j])*8 > i0_avg)
//					I0bak_value[j] = I0_value[10*i+j];	
//				if(I0bak_value[j] > 0)
//				  over_i0_cnt1++;
//				else if(I0bak_value[j] < 0)
//					over_i0_cnt2++;
//			}
//			if(over_i0_cnt1 > over_i0_cnt2)
//			{
//				for(j=0;j<8;j++)
//				{
//					if(I0bak_value[j] < 0)
//					  I0bak_value[j] = 0;	
//				}
//			}
//			else
//			{
//				for(j=0;j<8;j++)
//				{
//					if(I0bak_value[j] > 0)
//					  I0bak_value[j] = 0;	
//				}
//			}
//			
//			for(j=0;j<8;j++)
//			{
//				total_value[i] += U0bak_value[j] * I0bak_value[j];
//			}
//    }
		MeasureHarmonicPercent();
		os_evt_set (0x2000, t_Task17);
		os_dly_wait (100);
	}
}

/**************************************************************************************
* FunctionName   : Task15()	    
* Description    : ����15 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task15(void)
{
	os_dly_wait (2000);
	while(1)
	{
		os_evt_set (0x4000, t_Task17);
		os_dly_wait (100);
	}
}

/**************************************************************************************
* FunctionName   : Task16()	    
* Description    : ����16 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task16(void)
{
	os_dly_wait (2000);
	while(1)
	{
		//InsertDataToQueue(&QueueTX2,BufferTX2,0x88);
		os_evt_set(0x8000, t_Task17);
		os_dly_wait(100);
	}
}

/**************************************************************************************
* FunctionName   : Task17()	    ���Ź����   500ms
* Description    : ����17 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task17(void)
{
	OS_RESULT xResult;
  while(1)
	{
		os_dly_wait(100);
		xResult = os_evt_wait_and(0xFFFF,500);
		switch(xResult)
		{
			case OS_R_EVT:
		    IWDG_ReloadCounter();
			  break;
			case OS_R_TMO:
				break;
			default:
			  break;
		}  
	}
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 nCount)
{
  for(; nCount != 0; nCount--);
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
