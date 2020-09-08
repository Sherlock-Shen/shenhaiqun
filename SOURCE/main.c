// 2017年03月28日硬件版本 10kV户外智能断路器前置器APP程序	   程序开始地址0x8010000  长度0x80000 	复位地址0x8010000 合计512K
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
s16 PcurveUA[1200];	// A相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
s16 PcurveUB[1200];	// B相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
s16 PcurveUC[1200];	// C相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
s16 PcurveIA[1200];	// A相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
s16 PcurveIB[1200];	// B相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
s16 PcurveIC[1200];	// C相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
s16 PcurveU0[1200];	// 零序保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
s16 PcurveI0[1200];	// 零序保护电流曲线录波，保护动作前4个周期，保护动作后8个周期

/*******************************ATT7022E读取测量值*********************************/
s32 ATT7022_UaRms,ATT7022_UbRms,ATT7022_UcRms;	// 电度表芯片 电压有效值
s32 ATT7022_IaRms,ATT7022_IbRms,ATT7022_IcRms;	// 电度表芯片 电流有效值
s32 ATT7022_Pa,ATT7022_Pb,ATT7022_Pc;			      // 电度表芯片 分相有功
s32 ATT7022_Qa,ATT7022_Qb,ATT7022_Qc;			      // 电度表芯片 分相无功
s32 ATT7022_Sa,ATT7022_Sb,ATT7022_Sc,ATT7022_St;// 电度表芯片 分相视在功率
s32 ATT7022_Pfa,ATT7022_Pfb,ATT7022_Pfc;		    // 电度表芯片 分相功率因数
s32 ATT7022_Pt,ATT7022_Qt,ATT7022_Pft;			    // 有功，无功，功率因数 
s32 ATT7022_Ept,ATT7022_Eqt,ATT7022_Est;			  // 有功电能，无功电能，视在电能
u32 ATT7022_Freq;
/*******************************ATT7022E读取校表参数*********************************/
u32 ZYXSA,ZYXSB,ZYXSC;	// 有功增益系数
u32 XWXSA,XWXSB,XWXSC;	// 相位增益系数
u32 WGXSA,WGXSB,WGXSC;	// 无功增益系数
u32 DYXSA,DYXSB,DYXSC;	// 电压增益系数
u32 DLXSA,DLXSB,DLXSC;	// 电流增益系数
u32 DYPYA,DYPYB,DYPYC;	// 电压增益系数
u32 DLPYA,DLPYB,DLPYC;	// 电流增益系数
u32 DYXSA1,DYXSB1,DYXSC1;	// 电压零点校准
u32 DLXSA1,DLXSB1,DLXSC1;	// 电流零点校准
u32 ATT7022_sum = 0;
u8 saveATT7022_flag = 0;
u8 Start_Debug = 0;
u8 TESTFlag = 0;
u8 GPS_Flag = 0;
u8 TestFlagCnt = 0;
u8 Battery_Readflag = 0;
u8 MachineInformation[13];		// 本体控制器生产年(2)月(2)出厂编号(7)
u8 Send_Flag = 0;
u8 timer_renzheng=0;

u16  Para_peak1_period_S = 0;		// 电量统计时段设置
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

u32 WpLast = 0;			        // 上一次当日有功电能
u32 WqLast = 0;			        // 上一次当日无功
u32 Wp_all_Last = 0;		    // 上一次总有功电能
u32 WpFX_all_Last = 0;	    // 上一次总反向有功电能
u32 Wp_peak_Last = 0;		    // 上一次峰有功电能
u32 WpFX_peak_Last = 0;	    // 上一次峰反向有功电能
u32 Wp_valley_Last = 0;	    // 上一次谷有功电能
u32 WpFX_valley_Last = 0;	  // 上一次谷反向有功电能
u32 Wp_level_Last = 0;	    // 上一次平有功电能
u32 WpFX_level_Last = 0;	  // 上一次平反向有功电能

u64 Wq_1all_Last = 0;		    // 上一次I象限总无功
u64 Wq_2all_Last = 0;		    // 上一次II象限总无功
u64 Wq_3all_Last = 0;		    // 上一次III象限总无功
u64 Wq_4all_Last = 0;		    // 上一次IV象限总无功

u8 BreakerStateLast = 0;	  // 上一次开关位置状态
u16 PowerOffCnt = 0;			  // 掉电检测计数
u8 YabanSave_Flag = 0;			// 压板保存标志
u16 BattryCnt = 0;
u8 BattryTimeOut = 0;
u8 ParaInitReadyFlag = 0;		  // 参数初始化准备好

u8 MeasureIAError = 0;			  // A相测量故障标志
u8 MeasureIBError = 0;			  // B相测量故障标志
u8 MeasureICError = 0;			  // C相测量故障标志
u16 ZeroPointIA = 0;				  // 一倍电流测量零点	
u16 ZeroPointIA10 = 0;				// 十倍电流测量零点	
u16 ZeroPointIB = 0;				  // 一倍电流测量零点	
u16 ZeroPointIB10 = 0;				// 十倍电流测量零点	
u16 ZeroPointIC = 0;				  // 一倍电流测量零点	
u16 ZeroPointIC10 = 0;				// 十倍电流测量零点	

u8 AlarmRecord_flag = 0;
u16 SwitchCnt = 0;            // 操作次数
u16 SelfCheckErrCode = 0;		  // 自检故障代码
u8 SwitchFlag = 0;				    // 开关状态
u8 StartCntFlag = 0;          // 合闸计数标志
u8 ReOnFlag = 0;					    // 重合闸标志
u8 ReOnTimeOut = 0;					  // 重合闸时间到达标志
u8 ReOnLockCnt = 10;				  // 重合闸闭锁时间10秒
u8 ReOnLockCnt1 = 0;				  // 合闸后重合闸充电时间15秒
u16 ReOnTimeDelay = 0;				// 重合闸计时10ms
u16 ReOnDelay = 0;					  // 重合闸延时时间
u8 ReOnTimes = 0;	            // 重合闸次数
u8 DLQZD_flag = 0;				    // 断路器诊断信息标志
u8 ProtectFlag = 0;           // 保护标志
u16 ProtectTimesLock = 0;	    // 保护次数闭锁，合闸位置只闭锁一次，分闸后进行下一次保护
u8 ProtectTimesLockCnt = 0;   // 保护时间闭锁计数
u16 AlarmTimesLock = 0;	      // 报警次数闭锁，合闸位置只闭锁一次，分闸后进行下一次报警
u8 AlarmTimesLockCnt = 0;		  // 报警时间闭锁计数
u8 Switchon_Lock = 0;	        // 合闸闭锁
u8 Switchon_Lock1 = 0;	      // 两侧带电禁止合闸
u8 Switchon_Lock2 = 0;	      // 残压闭锁合闸
u8 Switchon_Lock2_Flag = 0;
u8 Switchoff_Lock = 0;        // 分闸闭锁
u8 Switchoff_Lock1 = 0;        // 分闸闭锁1
u8 SigresetLock = 0;          // 闭锁复归计时开始标志
u8 AD_Index = 0;		          // AD采样序号
u16 AD_StartPoint = 0;       // AD采样临界点
u8 ProtectRecord_flag = 0;		// 保护记录标志，用于记录保护缓冲区是否有数据
u16 Max_current_after_cnt = 5000;       // 过流后加速计时
u8 PTDX_Flag = 0;				                // PT断线标志，1=断线
u8 CTDX_Flag = 0;				                // CT断线标志，1=断线
u8 PTDX_Enable = 0;				              // PT断线检测允许，1=允许
u8 CTDX_Enable = 0;				              // CT断线检测允许，1=允许
u8 MeasureReady = 0;                    // 测量完成标志
u8  GpsOn_Flag = 0;
u16 qualif_volt = 1000;                 // 电压合格率
u16 CtrlMeasureCnt = 0;	                // 控制测量计数
u8 CtrlMeasureFlag = 0;	                // 控制测量标志
u8 AUTHORIZATIONcode[16]= {0};		//授权码
u8 ClearTJXX_Flag = 0;
// DMA发送缓冲
u8 DMATXbuffer1[300]= {0}; 
u8 DMATXbuffer2[820]= {0}; 
u8 DMATXbuffer3[100]= {0}; 
u8 DMATXbuffer4[300]= {0};
u8 DMATXbuffer5[420]= {0};
u8 DMATXbuffer6[100]= {0};
// DMA接收缓冲

u8 DMARXbuffer1[50] = {0};
u8 DMARXbuffer2[50] = {0};
u8 DMARXbuffer3[50] = {0};
u8 DMARXbuffer4[50] = {0};
u8 DMARXbuffer5[50] = {0};
u8 DMARXbuffer6[50] = {0};

// DMA采样缓冲
u16 DMAADCbuffer1[100][9] = {0};

u16 SOE[16][11]={0};
// DMA读取当前指针
u16 DMAReadIndex1 = 0;
u16 DMAReadIndex2 = 0;
u16 DMAReadIndex3 = 0;
u16 DMAReadIndex4 = 0;
u16 DMAReadIndex5 = 0;
u16 DMAReadIndex6 = 0;

u8 Usart1RxReady = 0;// 串口1接收完成标志
u8 Usart1bakRxReady = 0;// 串口1接收完成标志
u8 Usart2RxReady = 0;// 串口2接收完成标志
u8 Usart2bakRxReady = 0;// 串口2接收完成标志
u8 Usart3RxReady = 0;// 串口3接收完成标志
u8 Usart3bakRxReady = 0;// 串口3接收完成标志
u8 Usart4RxReady = 0;// 串口4接收完成标志
u8 Usart4bakRxReady = 0;// 串口4接收完成标志
u8 Usart5RxReady = 0;// 串口5接收完成标志
u8 Usart5bakRxReady = 0;// 串口5接收完成标志
u8 Usart6RxReady = 0;// 串口6接收完成标志
u8 Usart6bakRxReady = 0;// 串口6接收完成标志

u8 Usart2TxReady = 0;// 串口2准备发送标志
u8 timer_enable=0;	  //使能计时器
u8 delay_time_101=0;
// SOE使能标志 配置
const u8 SOE_Enable[20]=
{
	0xFF, 	// 地址从右边的低位开始 8  7  6  5  4  3  2  1 
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
u8 Select101Addr = 0;	// 协议控制 操作选择线圈地址
u8 EVENT_101Flag = 0;		  // 101协议事件标志
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
u16 Battery_RunTime = 0; // 电池运行时间
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
struct Measure          MeasureData;				 // 测量值
struct Measure1         MeasureCurve;		     // 测量曲线
struct time_now         TimeNow;		 // 当前时间
struct UpdateVersion    MyVersion;	         // 运行程序和备份程序版本和修改时间
union  DLQZDXX          DLQZDinformation;
union  PROTECT_DELAY1   ProtectDelay;	       // 保护延时计时
union  PROTECT_TIMEOUT1 ProtectTimeout;	     // 保护时间到达标志
union  ALARM_DELAY1     AlarmDelay;          // 报警延时计时
union  ALARM_TIMEOUT1   AlarmTimeout;        // 报警时间到达标志
union  KZQMEASUREDATA   KZQMeasureData;	     // 断路器实时数据
union  RECORD_ACTION    Record_on;	         // 合闸记录
union  RECORD_ACTION    Record_off;	         // 分闸记录
union  RECORD_ACTION    Record_protect1;	   // 保护记录
union  RECORD_ACTION    Record_protect2;	   // 保护记录
union  RECORD_ACTION    Record_alarm;	  	   // 报警记录
union  RECORD_ACTION    Record_autofeed;     // 馈线自动化记录
union  SYSTEM_PARA1     SystemSet;	         // 系统参数设置
union  PROTECT_PARA1    ProtectSet;	         // 保护参数设置
union  PROTECT_PARA1    ProtectSetReceive;
union  ALARM_PARA1      AlarmSet;	           // 报警参数设置
union  AUTO_SWITCHOFF1  AutoswitchSet;	     // 自动解列参数设置
union  Modulus1         ProtectModulus;      // 保护测量系数
union  Rule101Para1     Measure101Para;      // 101规约参数

union YXWORDDefine      YXChange;	      // 遥信变位
union YXWORDDefine      YXChange1;    	// 遥信变位
union YXWORDDefine      YXChange2;	    // 遥信变位
union YXWORDDefine      YXChangeHold;   // 遥信变位保持

union YXDefine          lockbz;

u8  YXChangeShake[16];        // 遥信变位消抖
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
* Description    : 任务标志处理
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
  USART_InitStructure.USART_BaudRate = baudrate;               /*设置波特率*/
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*设置数据位为8*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
  USART_InitStructure.USART_Parity = USART_Parity_No;        /*无奇偶校验*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/
  /*配置串口1  */
  USART_Init(USART1, &USART_InitStructure);		 
}

//static u8 DHT11_ReadValue(void)
//{
//	u8 value=0,i;
//	u16 count;
//	status=SUCCESS;//设定标志为正常状态
//	for(i=8;i>0;i--)
//	{
//		value<<=1;		//高位在先
//		count=0;		//每一位数据前会有一个50US的低电平结束
//		while(HTDATA_IN==0&&count++<2200);
//		if(count>=2200)
//		{
//			status=ERROR;		 //设定错误标志
//			return 0;			 //函数执行过程发生错误就退出函数
//		}
//		Delay(1250);		//26-28US高电平表示该位是0，为70US高电平表示该位为1
//		if(HTDATA_IN!=0)
//		{				         //进入这里表示该位是1
//			count=0;
//			while(HTDATA_IN!=0&&count++<1900);//等待剩余40US的高电平
//			value++;
//		 }
//	}
//	return(value);
//}

////读温度和湿度函数，读一次的数据，共五字节，读出成功函数返回OK，错误返回ERROR
//u8 DHT11_ReadTempAndHumi(void)
//{
//	u8 i=0,check_value=0,value_array[5];
//	u16 count=0;
//	HTDATA_OUT_0;//拉低数据线大于18ms发送开始信号   
//	os_dly_wait(1); //需大于800微秒	   
//	HTDATA_OUT_1;  //释放数据线，用于检测低电平的应答信号
//	//延时20-40us,等待一段时间后检测应答信号，应答信号是从机拉低数据线80US
////   os_tmr_create(10,1);  //10个时钟节拍，一个时钟节拍为1ms
//// 	while(usdelay_flag==0);
//// 	usdelay_flag = 0;
//	Delay(1500);//480等于40us
//	if(HTDATA_IN!=0)
//	{
//	  return 0;//没应答信号
//	}
//	else
//	{
//		//有应答信号
//		while(HTDATA_IN==0&&count++<3500);//等待应答信号结束
//		if(count>3500)//检测计数器是否超过了设定的范围
//		{	
//			HTDATA_OUT_1;  //释放数据线
//			return 0;//退出函数
//		}
//	  count=0;
//		HTDATA_OUT_1;//释放数据线
//		while(HTDATA_IN!=0&&count++<3500);//应答信号后会有一个80US的高电平结束
//		if(count>3500)//检测计数器是否超过了设定的范围
//		{
//			HTDATA_OUT_1;
//			return 0;//退出函数
//		}
//		//读出湿度温度值
//		for(i=0;i<5;i++)
//		{	
//			value_array[i]=DHT11_ReadValue();
//			if(status==ERROR)//调用ReadValue()读数据出错会设定status为ERROR
//			{
//			  HTDATA_OUT_1;  //释放数据线
//				return 0;
//			}
//			//读出的最后一个值是校验值不需加上去
//			if(i!=4)
//			{
//				//读出的五个字节数据中的前四字节数据和等于第五字节数据表示成功
//				check_value+=value_array[i];
//			}
//		}
//		//在没用发生函数调用失败时进行校验
//		if(check_value==value_array[4])
//		{
//		  Humi_value=(value_array[0]<<8)+value_array[1];
//		  Temp_value=(value_array[2]<<8)+value_array[3];
//			//if(Temp_value & 0x8000)
//			//  Temp_value = -(Temp_value & 0x7FFF);
//		  HTDATA_OUT_1;  //释放数据线
//		  return 1 ;   //正确的 读出dht11输出的数据
//		}
//		 else
//		{
//			//校验数据出错
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
//	HTCLK_OUT_1;  //ACK时钟
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
//	HTCLK_OUT_1;  //ACK时钟
//	HTDATA_OUT_0; //响应传感器拉低
//	delay_us(5);
//	return(data);
//}

////读温度和湿度函数，读出成功函数返回OK，错误返回ERROR
//u8 DHT11_ReadTempAndHumi1(void)
//{
//	u8 value_array[4];
//	//唤醒传感器
//	HTCLK_OUT_1;
//	HTDATA_OUT_1;
//	Delay(3);
//	HTDATA_OUT_0;        //起始信号
//	delay_us(5);
//  ReadHumi_Send(0xB8);
//	HTDATA_OUT_0; //唤醒时传感器不会回应ACK，主动拉低
//	HTCLK_OUT_0;
//	os_dly_wait(1); //需大于800微秒	   
//	HTCLK_OUT_1;
//	delay_us(5);
//	HTDATA_OUT_1;   //停止信号
//	os_dly_wait(10);
//	
//	//发送读指令
//	HTDATA_OUT_0;        //起始信号
//	delay_us(5);
//	ReadHumi_Send(0xB8);  //地址
//	if(HTDATA_IN == 1)
//	  return 0;
//	delay_us(35);
//	ReadHumi_Send(0x03);  //功能码
//	if(HTDATA_IN == 1)
//	  return 0;
//	ReadHumi_Send(0x00);  //起始地址
//	if(HTDATA_IN == 1)
//	  return 0;
//	ReadHumi_Send(0x04);  //寄存器长度
//	if(HTDATA_IN == 1)
//	  return 0;
//	HTDATA_OUT_1;   //停止信号
//	
//	//读返回温湿度数据
//	os_dly_wait(2);  //发送读指令后，主机需等待至少1.5ms，再发送读取时序
//	HTDATA_OUT_0;        //起始信号
//	delay_us(5);
//	ReadHumi_Send(0xB8);  //地址
//	if(HTDATA_IN == 1)
//	  return 0;
//	delay_us(35);
//	ReadHumi_Data();  //返回功能码0x03
//	ReadHumi_Data();  //返回数据长度0x04
//	value_array[0] = ReadHumi_Data();//湿度高位
//	value_array[1] = ReadHumi_Data();//湿度低位
//	value_array[2] = ReadHumi_Data();//温度高位
//	value_array[3] = ReadHumi_Data();//温度低位
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
		  //唤醒传感器
			HTCLK_OUT_1;
			HTDATA_OUT_1;
		  Step_Value = 1;
		  break;
		case 1:
			HTDATA_OUT_0;        //起始信号
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
		  HTDATA_OUT_1;  //故意拉高
		  Step_Value = 5;
		  break;
		case 5:
			HTCLK_OUT_1;  //ACK时钟
		  Step_Value = 6;
		  break;
		case 6:
			HTCLK_OUT_0;  //需大于800微秒	  
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
			HTDATA_OUT_1;   //停止信号	
		  Step_Value = 10;
		  break;
		//发送读指令
		case 10:
			HTDATA_OUT_0;        //起始信号
		  address_data = 0xB8; //地址
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
		  HTDATA_OUT_1;  //故意拉高
		  Step_Value = 14;
		  break;
		case 14:
			HTCLK_OUT_1;  //ACK时钟
		  if(HTDATA_IN == 1)
			{
        Step_Value = 0;
				return 0;
			}
			else
			{
				Step_Value = 15;
			  address_data = 0x03;  //功能码
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
		  HTDATA_OUT_1;  //故意拉高
		  Step_Value = 18;
		  break;
		case 18:
			HTCLK_OUT_1;  //ACK时钟
		  //Delay(2);
		  if(HTDATA_IN == 1)
      {
        Step_Value = 0;
				return 0;
			}
			else
			{
				Step_Value = 19;
			  address_data = 0x00;  //起始地址
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
		  HTDATA_OUT_1;  //故意拉高
		  Step_Value = 22;
		  break;
		case 22:
			HTCLK_OUT_1;  //ACK时钟
		  //Delay(2);
		  if(HTDATA_IN == 1)
      {
        Step_Value = 0;
				return 0;
			}
			else
			{
				Step_Value = 23;
			  address_data = 0x04;  //寄存器长度
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
		  HTDATA_OUT_1;  //故意拉高
		  Step_Value = 26;
		  break;
		case 26:
			HTCLK_OUT_1;  //ACK时钟
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
			HTDATA_OUT_1;   //停止信号
		  Step_Value = 30;
		  break;
		case 30:
			HTCLK_OUT_0;   //等待>1.5ms
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
      HTCLK_OUT_1;  //恢复时序
		  Step_Value = 33;
		  break;			
    //读返回温湿度数据	
    case 33:
      HTDATA_OUT_0;        //起始信号
		  address_data = 0xB9; //地址
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
		  HTDATA_OUT_1;  //故意拉高
		  Step_Value = 37;
		  break;
		case 37:
			HTCLK_OUT_1;  //ACK时钟
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
		  HTDATA_OUT_1;  //故意拉高
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
					HTDATA_OUT_0; //响应传感器拉低
				}
				else
				{
					Step_Value = 0;
				  return 0;
				}
			}
			function =  function<<1;  //功能码
		  break;
	  case 41:
			HTDATA_OUT_0; //响应传感器拉低
      HTCLK_OUT_1;  //ACK时钟
		  Step_Value = 42;
		  datanum = 0;
		  i=0;
		  break;
		case 42:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //故意拉高
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
				  HTDATA_OUT_0; //响应传感器拉低
				}
				else
				{
					Step_Value = 0;
				  return 0;
				}
			}
			datanum =  datanum<<1;  //数据个数
			break;
	  case 45:
			HTDATA_OUT_0; //响应传感器拉低
      HTCLK_OUT_1;  //ACK时钟
		  Step_Value = 46;
		  value_array[0] = 0;
		  i=0;
		  break;
		case 46:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //故意拉高
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
				HTDATA_OUT_0; //响应传感器拉低
				break;
			}
			value_array[0] =  value_array[0]<<1;  //湿度高位
			break;
	  case 49:
			HTDATA_OUT_0; //响应传感器拉低
      HTCLK_OUT_1;  //ACK时钟
		  Step_Value = 50;
		  value_array[1] = 0;
		  i=0;
		  break;
		case 50:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //故意拉高
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
				HTDATA_OUT_0; //响应传感器拉低
				break;
			}
			value_array[1] =  value_array[1]<<1;  //湿度低位
			break;
	  case 53:
			HTDATA_OUT_0; //响应传感器拉低
      HTCLK_OUT_1;  //ACK时钟
		  Step_Value = 54;
		  value_array[2] = 0;
		  i=0;
		  break;
		case 54:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //故意拉高
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
				HTDATA_OUT_0; //响应传感器拉低
				break;
			}
			value_array[2] =  value_array[2]<<1;  //温度高位
			break;
	  case 57:
			HTDATA_OUT_0; //响应传感器拉低
      HTCLK_OUT_1;  //ACK时钟
		  Step_Value = 58;
		  value_array[3] = 0;
		  i=0;
		  break;
		case 58:
			HTCLK_OUT_0;
		  //Delay(2);
		  HTDATA_OUT_1;  //故意拉高
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
				HTDATA_OUT_0; //响应传感器拉低
				Humi_value = value_array[1] | (value_array[0]<<8);
  	    Temp_value = value_array[3] | (value_array[2]<<8);
				Step_Value = 0;
		    return 0x0D;
			}
			value_array[3] =  value_array[3]<<1;  //温度低位
			break;
		default:
			break;
	}
	return 0x01;
}

// 读取事件记录，SOE记录索引号
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

// 清空统计信息
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
	
	WpLast = 0;			// 上一次当日有功电能
	WqLast = 0;			// 上一次当日无功电能
	Wp_all_Last = 0;	// 上一次总有功电能
	Wp_peak_Last = 0;	// 上一次峰有功电能
	Wp_valley_Last = 0;	// 上一次谷有功电能
	Wp_level_Last = 0;	// 上一次平有功电能
	
	WpFX_all_Last = 0;	// 上一次总有功电能
	WpFX_peak_Last = 0;	// 上一次峰有功电能
	WpFX_valley_Last = 0;// 上一次谷有功电能
	WpFX_level_Last = 0;// 上一次平有功电能

	Wq_1all_Last = 0;	// 上一次Ⅰ象限总无功
	Wq_2all_Last = 0;	// 上一次Ⅱ象限总无功
	Wq_3all_Last = 0;	// 上一次Ⅲ象限总无功
	Wq_4all_Last = 0;	// 上一次Ⅳ象限总无功
	YabanSave_Flag = 1;//电量清零
}

void RecordSOE(u8 addr,u8 state)// SOE事件记录
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
//	SPI_Flash_Write(buf,FLASH_SOERecord_ADDR+(DLQ_SOERecord_Index*FLASH_SOERecord_LENGTH),FLASH_SOERecord_LENGTH);// 存入SOE记录
	
	SOE_Flag = 1;
}

void RecordSOE_Pad(u8 addr,u8 state)// SOE事件记录,平板
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

void YXChangeInit(void)// 遥信变位初始化
{
	YXChange.bit.D0  = FW_IN1;
	YXChange.bit.D1  = HW_IN2;
	YXChange.bit.D2  = WCN_IN3;
	YXChange.bit.D3  = LOCAL_IN7;                                            // 本地开关
	YXChange.bit.D4  = FAR_IN6;                                              // 远方开关
	YXChange.bit.D5  = MeasureData.YaBan1&0x01;                              // 保护投退压板
	YXChange.bit.D6  = MeasureData.YaBan2&0x01;                              // 接地保护投退压板
	YXChange.bit.D7  = MeasureData.YaBan3&0x01;                              // 相间保护投退压板
	YXChange.bit.D8  = SigresetFlag&0x01;                                    // 复归信号
	if(ProtectFlag==1 || (KZQMeasureData.para.AlarmFlag.word & 0x01ff))	// 事故输出
	{
    YXChange.bit.D9 = 0x01; 
	}
	else
	{
    YXChange.bit.D9 = 0x00; 
	}
			
	if(KZQMeasureData.para.SelfCheck.word & 0x3f0f)
	{
	  YXChange.bit.D10 = 0x01;                                         //装置异常告警
	}
	else
	   YXChange.bit.D10 = 0x00; 
  if(DYXH1 == 1 && DYXH2 == 1)	
	{
	  YXChange.bit.D11 = 0x01;                                          //交流失电告警
	}
	else
	  YXChange.bit.D11 = 0x00;
	if(Switchon_Lock | Switchon_Lock1 | Switchon_Lock2)	
	{
	  YXChange.bit.D12 = 0x01;                                          //开关闭锁合闸
	}
	else
	  YXChange.bit.D12 = 0x00;
	if(Switchoff_Lock | Switchoff_Lock1)	
	{
	  YXChange.bit.D13 = 0x01;                                          //开关闭锁分闸
	}
	else
	  YXChange.bit.D13 = 0x00;
	if(AutoswitchSet.para.segment_contact_mode == 0x01)	
	{
	  YXChange.bit.D14 = 0x01;                                          //分段点工作模式
	}
	else
	  YXChange.bit.D14 = 0x00;
	
	if(AutoswitchSet.para.segment_contact_mode == 0x02)	
	{
	  YXChange.bit.D15 = 0x01;                                          //联络点工作模式
	}
	else
	  YXChange.bit.D15 = 0x00;  
	
	if(ProtectTimeout.para.novoltage_value_A == 1)	
	{
	  YXChange1.bit.D0 = 0x01;                                          //电源侧无压
	}
	else
	  YXChange1.bit.D0 = 0x00;  
	if(ProtectTimeout.para.novoltage_value_B == 1)	
	{
	  YXChange1.bit.D1 = 0x01;                                          //负载侧无压
	}
	else
	  YXChange1.bit.D1 = 0x00;  
	if(KZQMeasureData.para.ProtectFlag1.bit.loss_power)	
	{
	  YXChange1.bit.D2 = 0x01;                                          //失压分闸
	}
	else
	  YXChange1.bit.D2 = 0x00;  
	
	YXChange1.bit.D3 = KZQMeasureData.para.AlarmFlag.bit.bat_active;    //电池活化
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

void SwapYXChange(void)// 遥信变位扫描
{
	u16 i = 0;
	static u8 self_error = 0;
	static u8 event_out = 0;
	static u8 power_lost = 0;
	
	if(YXChange.bit.D0 != FW_IN1)	// 分位
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

	if(YXChange.bit.D1 != HW_IN2)	// 合位
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

	if(YXChange.bit.D2 != WCN_IN3)	// 未储能位置
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
	
	if(YXChange.bit.D3 != LOCAL_IN7)	// 本地位置
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
	
	if(YXChange.bit.D4 != FAR_IN6)	// 远方位置
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
	
	if(YXChange.bit.D5 != (MeasureData.YaBan1 &0x01))	    // 保护压板
	{
		YXChange.bit.D5 = (MeasureData.YaBan1 &0x01);
		RecordSOE_Pad(SOE_ADDR_BHYB1_pad,YXChange.bit.D5);
	}
	if(YXChange.bit.D6 != (MeasureData.YaBan2 &0x01))	    // 接地保护压板
	{
		YXChange.bit.D6 = (MeasureData.YaBan2 &0x01);
		RecordSOE_Pad(SOE_ADDR_BHYB2_pad,YXChange.bit.D6);
	}
	if(YXChange.bit.D7 != (MeasureData.YaBan3 &0x01))	    // 相间保护压板
	{
		YXChange.bit.D7 = (MeasureData.YaBan3 &0x01);
		RecordSOE_Pad(SOE_ADDR_BHYB3_pad,YXChange.bit.D7);
	}

	if(SigresetFlag)	          // 复归信号  
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
		for(i=0;i<(sizeof(struct PROTECT_DELAY)/2);i++)	// 保护延时
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
	  event_out = 0x01;                                            //事故总
	}
	else
	  event_out = 0x00; 

	if(YXChange.bit.D9 != event_out)
	{
		YXChange.bit.D9 = event_out;
	  RecordSOE(SOE_ADDR_EVENT_ALL,YXChange.bit.D9);               //事故总
	}
	if(KZQMeasureData.para.SelfCheck.word & 0x3f0f)
	{
	  self_error = 0x01;                                           //装置异常告警
	}
	else
	  self_error = 0x00; 

	if(YXChange.bit.D10 != self_error)
	{
		YXChange.bit.D10 = self_error;
	  RecordSOE(SOE_ADDR_SELF_ERROR,YXChange.bit.D10);              //装置异常告警
	}
	if(DYXH1 == 1 && DYXH2 == 1)	
	{
	  power_lost = 0x01;                                            //交流失电告警
	}
	else
	  power_lost=0x00;   
  if(YXChange.bit.D11 != power_lost)
	{
		YXChange.bit.D11 = power_lost;
	  RecordSOE(SOE_ADDR_POWER_LOST,YXChange.bit.D11);               //交流失电告警
	}
	
	if(YXChange.bit.D12 != ((Switchon_Lock | Switchon_Lock1 | Switchon_Lock2)&0X01))	
	{
	  YXChange.bit.D12 = (Switchon_Lock | Switchon_Lock1 | Switchon_Lock2)&0X01;          
		RecordSOE(SOE_ADDR_SWITCHON_LOCK,YXChange.bit.D12);               //开关闭锁合闸
	}

	if(YXChange.bit.D13 != ((Switchoff_Lock | Switchoff_Lock1)&0x01))	
	{
	  YXChange.bit.D13 = (Switchoff_Lock | Switchoff_Lock1)&0x01;       //开关闭锁分闸
		RecordSOE(SOE_ADDR_SWITCHOFF_LOCK,YXChange.bit.D13);
	}

	if(YXChange.bit.D14 != (AutoswitchSet.para.segment_contact_mode&0x01))	
	{
	  YXChange.bit.D14 = AutoswitchSet.para.segment_contact_mode&0x01; //分段点工作模式
		RecordSOE(SOE_ADDR_SWITCH_CONTACT_1,YXChange.bit.D14);
	}

	if(YXChange.bit.D15 != ((AutoswitchSet.para.segment_contact_mode>>1)&0x01))	
	{
	  YXChange.bit.D15 = (AutoswitchSet.para.segment_contact_mode>>1)&0x01;//联络点工作模式
		RecordSOE(SOE_ADDR_SWITCH_CONTACT_2,YXChange.bit.D15);
	}

	if(YXChange1.bit.D0 != (ProtectTimeout.para.novoltage_value_A&0x01))	
	{
	  YXChange1.bit.D0 = ProtectTimeout.para.novoltage_value_A&0x01;    //电源侧无压
		RecordSOE(SOE_ADDR_SWITCH_POWERLOSS_1,YXChange1.bit.D0);
	}
  
	if(YXChange1.bit.D1 != (ProtectTimeout.para.novoltage_value_B&0x01))	
	{
	  YXChange1.bit.D1 = ProtectTimeout.para.novoltage_value_B&0x01;    //负载侧无压
		RecordSOE(SOE_ADDR_SWITCH_POWERLOSS_2,YXChange1.bit.D1);
	}

	if(YXChange1.bit.D2 != KZQMeasureData.para.ProtectFlag1.bit.loss_power)	
	{
	  YXChange1.bit.D2 = KZQMeasureData.para.ProtectFlag1.bit.loss_power;                                      
		RecordSOE(SOE_ADDR_POWERLOSS_OFF,YXChange1.bit.D2);               //失压分闸
	} 
	
	if(YXChange1.bit.D3 != KZQMeasureData.para.AlarmFlag.bit.bat_active)	// 电池活化
	{
		YXChange1.bit.D3 = KZQMeasureData.para.AlarmFlag.bit.bat_active;
		RecordSOE(SOE_ADDR_ALARM_BAT_ACTIVE,YXChange1.bit.D3);
	}
	
	if(YXChange2.bit.D0 != KZQMeasureData.para.ProtectFlag1.bit.zero_max_voltage)	// 零序过压保护动作
	{
		YXChange2.bit.D0 = KZQMeasureData.para.ProtectFlag1.bit.zero_max_voltage;
		RecordSOE_Pad(SOE_ADDR_ZERO_MAX_VOLTAGE_pad,YXChange2.bit.D0);
	}

	if(YXChange2.bit.D1 != KZQMeasureData.para.ProtectFlag1.bit.zero_max_current)	// 零序过流保护动作
	{
		YXChange2.bit.D1 = KZQMeasureData.para.ProtectFlag1.bit.zero_max_current;
		RecordSOE(SOE_ADDR_ZERO_MAX_CURRENT,YXChange2.bit.D1);
		RecordSOE_Pad(SOE_ADDR_ZERO_MAX_CURRENT_pad,YXChange2.bit.D1);
	}
	
	if(YXChange2.bit.D2 != KZQMeasureData.para.ProtectFlag1.bit.no_phase)	//缺相保护动作
	{
		YXChange2.bit.D2 = KZQMeasureData.para.ProtectFlag1.bit.no_phase;
		RecordSOE_Pad(SOE_ADDR_PHASE_LOSS_pad,YXChange2.bit.D2);
	}

	if(YXChange2.bit.D3 != KZQMeasureData.para.ProtectFlag1.bit.ground)  	//接地保护动作
	{
		YXChange2.bit.D3 = KZQMeasureData.para.ProtectFlag1.bit.ground;
		RecordSOE_Pad(SOE_ADDR_GROUND_pad,YXChange2.bit.D3);
	}

	if(YXChange2.bit.D4 != KZQMeasureData.para.ProtectFlag1.bit.fast_off)	// 速断保护动作
	{
		YXChange2.bit.D4 = KZQMeasureData.para.ProtectFlag1.bit.fast_off;
		RecordSOE(SOE_ADDR_FAST_OFF,YXChange2.bit.D4);
		RecordSOE_Pad(SOE_ADDR_FAST_OFF_pad,YXChange2.bit.D4);
	}

	if(YXChange2.bit.D5 != KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off)	// 延时速断保护动作
	{
		YXChange2.bit.D5 = KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off;
		RecordSOE(SOE_ADDR_DELAY_FAST_OFF,YXChange2.bit.D5);
		RecordSOE_Pad(SOE_ADDR_DELAY_FAST_OFF_pad,YXChange2.bit.D5);
	}

	if(YXChange2.bit.D6 != KZQMeasureData.para.ProtectFlag1.bit.max_current)	// 过流保护动作
	{
		YXChange2.bit.D6 = KZQMeasureData.para.ProtectFlag1.bit.max_current;
		RecordSOE(SOE_ADDR_MAX_CURRENT,YXChange2.bit.D6);
		RecordSOE_Pad(SOE_ADDR_MAX_CURRENT_pad,YXChange2.bit.D6);
	}

	if(YXChange2.bit.D7 != KZQMeasureData.para.ProtectFlag1.bit.max_current_after)	// 过流后加速保护动作
	{
		YXChange2.bit.D7 = KZQMeasureData.para.ProtectFlag1.bit.max_current_after;
		RecordSOE(SOE_ADDR_MAX_CURRENT_AFTER,YXChange2.bit.D7);
		RecordSOE_Pad(SOE_ADDR_MAX_CURRENT_AFTER_pad,YXChange2.bit.D7);
	}
	
	if(YXChange2.bit.D8 != KZQMeasureData.para.ProtectFlag2.bit.reon_act)	// 重合闸动作
	{
		YXChange2.bit.D8 = KZQMeasureData.para.ProtectFlag2.bit.reon_act;
		RecordSOE(SOE_ADDR_REON,YXChange2.bit.D8);
		RecordSOE_Pad(SOE_ADDR_REON_pad,YXChange2.bit.D8);
	}

	if(YXChange2.bit.D9 != KZQMeasureData.para.AlarmFlag.bit.max_load)	// 过负荷报警动作
	{
		YXChange2.bit.D9 = KZQMeasureData.para.AlarmFlag.bit.max_load;
		RecordSOE(SOE_ADDR_ALARM_MAX_LOAD,YXChange2.bit.D9);
		RecordSOE_Pad(SOE_ADDR_ALARM_MAX_LOAD_pad,YXChange2.bit.D9);
	}

	if(YXChange2.bit.D10 != KZQMeasureData.para.AlarmFlag.bit.max_voltage)	// 过电压报警动作
	{
		YXChange2.bit.D10 = KZQMeasureData.para.AlarmFlag.bit.max_voltage;
		RecordSOE_Pad(SOE_ADDR_ALARM_MAX_VOLTAGE_pad,YXChange2.bit.D10);
	}

	if(YXChange2.bit.D11 != KZQMeasureData.para.AlarmFlag.bit.min_voltage)	// 欠压报警动作
	{
		YXChange2.bit.D11 = KZQMeasureData.para.AlarmFlag.bit.min_voltage;
		RecordSOE_Pad(SOE_ADDR_ALARM_MIN_VOLTAGE_pad,YXChange2.bit.D11);
	}

	if(YXChange2.bit.D12 != KZQMeasureData.para.AlarmFlag.bit.max_harmonic)	// 过谐波报警动作
	{
		YXChange2.bit.D12 = KZQMeasureData.para.AlarmFlag.bit.max_harmonic;
		RecordSOE_Pad(SOE_ADDR_ALARM_MAX_HARMONIC_pad,YXChange2.bit.D12);
	}

	if(YXChange2.bit.D13 != KZQMeasureData.para.AlarmFlag.bit.voltage_qualifrate)	// 电压合格率报警动作
	{
		YXChange2.bit.D13 = KZQMeasureData.para.AlarmFlag.bit.voltage_qualifrate;
		RecordSOE_Pad(SOE_ADDR_ALARM_VOLTAGE_QUALIFRATE_pad,YXChange2.bit.D13);
	}

	if(YXChange2.bit.D14 != KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage)	// 电池低电压报警动作
	{
		YXChange2.bit.D14 = KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage;
		RecordSOE(SOE_ADDR_ALARM_BAT_LOW_VOLTAGE,YXChange2.bit.D14);
		RecordSOE_Pad(SOE_ADDR_ALARM_BAT_LOW_VOLTAGE_pad,YXChange2.bit.D14);
	}

	if(YXChange2.bit.D15 != KZQMeasureData.para.AlarmFlag.bit.ground)	// 接地报警动作
	{
		YXChange2.bit.D15 = KZQMeasureData.para.AlarmFlag.bit.ground;
		RecordSOE(SOE_ADDR_ALARM_GROUND,YXChange2.bit.D15);
		RecordSOE_Pad(SOE_ADDR_ALARM_GROUND_pad,YXChange2.bit.D15);
	}

	if(YXChange1.bit.D11 != KZQMeasureData.para.ProtectFlag2.bit.max_voltage)	// 过压保护动作
	{
		YXChange1.bit.D11 = KZQMeasureData.para.ProtectFlag2.bit.max_voltage;
		RecordSOE(SOE_ADDR_ALARM_GROUNDI0,YXChange1.bit.D11);
		RecordSOE_Pad(SOE_ADDR_MAX_VOLTAGE_pad,YXChange1.bit.D11);
	}
	
	if(YXChange1.bit.D12 != KZQMeasureData.para.ProtectFlag2.bit.max_freq)	// 高频保护动作
	{
		YXChange1.bit.D12 = KZQMeasureData.para.ProtectFlag2.bit.max_freq;
		RecordSOE(SOE_ADDR_ALARM_GROUNDI0,YXChange1.bit.D12);
		RecordSOE_Pad(SOE_ADDR_MAX_FREQ_pad,YXChange1.bit.D12);
	}
	
	if(YXChange1.bit.D13 != KZQMeasureData.para.ProtectFlag2.bit.low_freq)	// 低频保护动作
	{
		YXChange1.bit.D13 = KZQMeasureData.para.ProtectFlag2.bit.low_freq;
		RecordSOE(SOE_ADDR_ALARM_GROUNDI0,YXChange1.bit.D13);
		RecordSOE_Pad(SOE_ADDR_LOW_FREQ_pad,YXChange1.bit.D13);
	}
	
	if(YXChange1.bit.D14 != KZQMeasureData.para.AlarmFlag.bit.zero_max_voltage)	// 零序电压报警动作
	{
		YXChange1.bit.D14 = KZQMeasureData.para.AlarmFlag.bit.zero_max_voltage;
		RecordSOE_Pad(SOE_ADDR_ALARM_GROUNDU0_pad,YXChange1.bit.D14);
	}

	if(YXChange1.bit.D15 != KZQMeasureData.para.AlarmFlag.bit.zero_max_current)	// 零序电流报警动作
	{
		YXChange1.bit.D15 = KZQMeasureData.para.AlarmFlag.bit.zero_max_current;
		RecordSOE(SOE_ADDR_ALARM_GROUNDI0,YXChange1.bit.D15);
		RecordSOE_Pad(SOE_ADDR_ALARM_GROUNDI0_pad,YXChange1.bit.D15);
	}
}

// 读取版本信息
void ReadVersionfromFlash(void)
{
	u8 pbuffer[10] = {0};
  static u32 cpuid[3];
	u8 i = 0;
  cpuid[0] = *(vu32*)ID_Address;       //设备唯一ID加密算法
  cpuid[1] = *(vu32*)(ID_Address+4);
  cpuid[2] = *(vu32*)(ID_Address+8);
  Lock_IdCode = cpuid[0] + cpuid[1] + cpuid[2];
  CS2BZ=0;
  // 控制器升级数据
	SPI_Flash_Read(pbuffer,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10);
 	for(i=0;i<6;i++)
 	{
 		MyVersion.KZQ_time_update[i] = pbuffer[i];
 	}
	MyVersion.KZQ_length_update = (u32)pbuffer[6] | ((u32)pbuffer[7]<<8) | ((u32)pbuffer[8]<<16) | ((u32)pbuffer[9]<<24);
	// 控制器备份升级数据
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
// 设置KZQ升级标志 type为升级类型  0x01=本地升级，0x02=远方升级,0x03=恢复备份
void SetUpdateFlag(u8 type)
{
	u8 pbuffer[FLASH_UPDATEFLAG_LENGTH] = {0};
	pbuffer[0] = 0x55;
	pbuffer[1] = 0xAA;
	pbuffer[2] = type;
	CS2BZ=0;
	SPI_Flash_Write(pbuffer,FLASH_UPDATEFLAG_ADDR,FLASH_UPDATEFLAG_LENGTH);
}
// 读取程序升级标志信息，程序升级则置标志0x55 AA,复位重启后烧写完成则改写为0x5A A5,否则烧写不成功
u8 ReadUpdateFlagfromFlash(void)
{
	u8 pbuffer[FLASH_UPDATEFLAG_LENGTH] = {0};
	CS2BZ=0;
	SPI_Flash_Read(pbuffer,FLASH_UPDATEFLAG_ADDR,FLASH_UPDATEFLAG_LENGTH);
	MyVersion.IAP_version[0]=pbuffer[3];
	MyVersion.IAP_version[1]=pbuffer[4];
	MyVersion.IAP_version[2]=pbuffer[5];
	MyVersion.IAP_version[3]=pbuffer[6];
	if(pbuffer[0]==0x55 && pbuffer[1]==0xAA) // 未烧写
	{
		pbuffer[0]=0xFF;
		pbuffer[1]=0xFF;
		SPI_Flash_Write(pbuffer,FLASH_UPDATEFLAG_ADDR,FLASH_UPDATEFLAG_LENGTH);
		return 0x0E;
	}
	else if(pbuffer[0]==0x5A && pbuffer[1]==0xA5)  // 烧写成功
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

void JumpToApp(void)	// 程序跳到应用程序
{
	u32 JumpAddress;
	pFunction Jump_To_Application;	
  RCC_DeInit();        //关闭外设
  __set_FAULTMASK(1);  //关闭总中断  
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
  IWDG_SetReload(Reload);                                  /* 喂狗时间 2000*6.4ms = 12800ms */
  IWDG_ReloadCounter();                                 /* 喂狗*/
  IWDG_Enable();                                        /* 使能*/
}

/*----------------------------------------------------------------------------
  Task  'init': Initialize
 *---------------------------------------------------------------------------*/
__task void Task (void) 
{
	IWDG_Configuration(2000);  
	t_Task1  = os_tsk_create_user(Task1, 1, &stk1,  sizeof(stk1));   /* start task 'Task1'   */
	t_Task2  = os_tsk_create_user(Task2, 1, &stk2,  sizeof(stk2));   /* start task 'Task2'  遥信变位扫描，分合闸灯控制 */
	t_Task3  = os_tsk_create_user(Task3, 1, &stk3,  sizeof(stk3));   /* start task 'Task3'  1秒计时器                  */
	t_Task4  = os_tsk_create_user(Task4, 1, &stk4,  sizeof(stk4));   /* start task 'Task4'  通讯协议处理任务           */
	t_Task5  = os_tsk_create_user(Task5, 1, &stk5,  sizeof(stk5));   /* start task 'Task5'  串口数据发送置标志         */
	t_Task6  = os_tsk_create_user(Task6, 1, &stk6,  sizeof(stk6));   /* start task 'Task6'  串口数据发送处理           */		
	t_Task7  = os_tsk_create_user(Task7, 1, &stk7,  sizeof(stk7));   /* start task 'Task7'  前置器程序更新控制         */
	t_Task8  = os_tsk_create_user(Task8, 1, &stk8,  sizeof(stk8));   /* start task 'Task8'  读温度和湿度函数           */
	t_Task9  = os_tsk_create_user(Task9, 1, &stk9,  sizeof(stk9));   /* start task 'Task9'  GPS时间校准，电池数据监测  */
	t_Task10 = os_tsk_create_user(Task10,1, &stk10, sizeof(stk10));  /* start task 'Task10'  */	
	t_Task11 = os_tsk_create_user(Task11,1, &stk11, sizeof(stk11));  /* start task 'Task11'  */
  t_Task12 = os_tsk_create_user(Task12,3, &stk12, sizeof(stk12));  /* start task 'Task12'  */
  t_Task13 = os_tsk_create_user(Task13,1, &stk13, sizeof(stk13));  /* start task 'Task13'  */
  t_Task14 = os_tsk_create_user(Task14,1, &stk14, sizeof(stk14));  /* start task 'Task14'  */
  t_Task15 = os_tsk_create_user(Task15,1, &stk15, sizeof(stk15));  /* start task 'Task15'  */
  t_Task16 = os_tsk_create_user(Task16,1, &stk16, sizeof(stk16));  /* start task 'Task16'  */
  t_Task17 = os_tsk_create_user(Task17,1, &stk17, sizeof(stk17));  /* start task 'Task17' 看门狗监控     1000ms      */
	os_tsk_delete_self ();
}

// 参数初始化
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
	else if(*reon_flag == 1 )		// 重合闸使能并启动
	{
		if( ReOnTimes >= ProtectSet.para.reon_requency )
		{
			*reon_flag = 0;
			state = 0;
		}
		switch(state)
		{
			case 0:						// 复归延
				state = 1;
				if(ReOnDelay<65000 && ReOnDelay>29)
					ReOnTimeDelay = ReOnDelay;	// 设定延时0.3s~650s
				else
					ReOnTimeDelay = 50;	// 500ms延时
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
					if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && KZQMeasureData.para.DLQflag.bit.store == 1)	// 分位，已储能
					{						
						for(i=0;i<8;i++)		// 检查有无保护延时动作标志
						{
							if(ProtectDelay.word[i] != 0)	// 复归无效，则说明故障依然存在，放弃重合闸
							{
								return 0;
							}
						}
						if(SwitchFlag == 0)
						{
							SwitchFlag = 4;		// 故障解除，若开关处于分闸状态执行重合闸命令
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
	else if( *reon_flag == 2 )  //第二次重合闸
	{ 
		if( ReOnTimes >= ProtectSet.para.reon_requency )
		{
			*reon_flag = 0;
			state = 0;
		}			
		switch(state)
		{
			case 0:
				ReOnTimeDelay = 100;	//1s延时
			  ReOnTimeOut = 0;
			  state = 1;
				break;
			case 1:
				if(ReOnTimeOut == 1)
				{
				  ReOnTimeDelay = 1500;	//15s延时
			    ReOnTimeOut = 0;
			    state = 2;
				}
				break;
			case 2:						// 复归延时
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
						ReOnTimeDelay = ReOnDelay;	// 设定延时2s~650s
					else
						ReOnTimeDelay = 200;	//2s延时
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
				  if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && KZQMeasureData.para.DLQflag.bit.store == 1)	// 分位，已储能
				  {
						for(i=0;i<8;i++)		// 检查有无保护延时动作标志
						{
							if(ProtectDelay.word[i] != 0)	// 复归无效，则说明故障依然存在，放弃重合闸
							{
								return 0;
							}
						}
						if(SwitchFlag == 0)
						{
							SwitchFlag = 4;		// 故障解除，若开关处于分闸状态执行重合闸命令
							RecordSwitchOn(TYPE_ACT_REON);
						}
				  }
				}
				break;
			default:
				break;
		}
	}
	else if( *reon_flag == 3 )  //第三次重合闸
	{
		if( ReOnTimes >= ProtectSet.para.reon_requency )
		{
			*reon_flag = 0;
			state = 0;
		}
		switch(state)
		{
			case 0:
				ReOnTimeDelay = 100;	//1s延时
			  ReOnTimeOut = 0;
			  state = 1;
				break;
			case 1:
				if(ReOnTimeOut == 1)
				{
				  ReOnTimeDelay = 1500;	//15s延时
			    ReOnTimeOut = 0;
			    state = 2;
				}
				break;
			case 2:						// 复归延时
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
						ReOnTimeDelay = ReOnDelay;	// 设定延时2s~650s
					else
						ReOnTimeDelay = 200;	//2s延时
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
					if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && KZQMeasureData.para.DLQflag.bit.store == 1)	// 分位，已储能)	
					{
						for(i=0;i<8;i++)		// 检查有无保护延时动作标志
						{
							if(ProtectDelay.word[i] != 0)	// 复归无效，则说明故障依然存在，放弃重合闸
							{
								return 0;
							}
						}
						if(SwitchFlag == 0)
						{
							SwitchFlag = 4;		// 故障解除，若开关处于分闸状态执行重合闸命令
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
	InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// 发送数据头
	sum += 0xFE;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// 发送数据头
	sum += 0xEF;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x01);			// 发送设备类型
	sum += 0x01;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x01);		// 发送设备地址
	sum += 0x01;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xBA);				// 发送特征码
	sum += 0xBA;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x05);				// 发送长度低位 
	sum += 0x05;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// 发送长度高位  

	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				

	InsertDataToQueue(&QueueTX2,BufferTX2,0x4B);				// 发送长度高位  
	sum += 0x4B;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// 发送长度低位 
	sum += 0x00;
	InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// 发送校验和
	InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// 发送帧尾
}

void Fault_101Curve_Send(void)
{
	u8 sum=0;
	InsertDataToQueue(&QueueTX1,BufferTX1,0x68);				// 发送数据头
	InsertDataToQueue(&QueueTX1,BufferTX1,9);				    //长度
	InsertDataToQueue(&QueueTX1,BufferTX1,9);		
  InsertDataToQueue(&QueueTX1,BufferTX1,0x68);				// 发送数据头	
	InsertDataToQueue(&QueueTX1,BufferTX1,0x80);
  sum+=0x80;	
	InsertDataToQueue(&QueueTX1,BufferTX1,SystemSet.para.address);
	sum+=SystemSet.para.address;
	InsertDataToQueue(&QueueTX1,BufferTX1,56);//类型标识＜56＞∶=故障曲线突发
	sum+=56;
	InsertDataToQueue(&QueueTX1,BufferTX1,0x81);//可变结构限定词（信息元素数目)
	sum+=0x81;
	InsertDataToQueue(&QueueTX1,BufferTX1,3);//<3>:=突发<7>:=激活确认<9>:=停止激活确认<10>:=激活结束
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
	SPI2_ReadWrite(0x55);														  //帧头
	for(i=0;i<6;i++)
	{
		SPI2_ReadWrite(Get_Ter_Random[i]);	
		crc_yhf^=Get_Ter_Random[i];
	}
	crc_yhf=~crc_yhf;
	SPI2_ReadWrite(crc_yhf);	
	crc_yhf=0;					//很重要
	SSN_H();											   //拉高片选
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
	HTCLK_OUT_1;  //I方C通信方式
	OUT_COM_OFF;		// 关闭输出公共端
  HE_OFF;
	FEN_OFF;
	KZCN_OFF;
	OUT4_OFF;      //关闭继电器1
	OUT5_OFF;      //关闭继电器2
	OUT6_ON;       //out6为公用端，高电平保持常开
	BATT_OFF;      //电池控制继电器关闭
	//SC1161Y_OFF;   // SC1161Y芯片关闭
	SC1161Y_ON;    // SC1161Y芯片开启
	SPI_FLASH_CS_H;
	SPI_FLASH_CS2_H;
	ParaInit();			// 参数初始化
	while(ATT7022_SIG != 0)// 等待电度表就绪信号变为低电平，对其初始化
	{
		delay7022++;
		if(delay7022 > 100000)	// 超时退出
			break;
	}
	ATT7022_Init();		  // 电度表初始化
	if(Read_ATT7022() != 0x0D)		  // 读取ATT7022校表参数
	{
		Read_ATT7022BAK();
	}
	Write_ATT7022();
	ds1302_init();	
  AD7606Reset();      // ad7606复位

	// 断路器上一次位置状态
	if(HW_IN2 == 0 && FW_IN1 == 1 )
	{
	 	BreakerStateLast = 1;		// 合闸状态	
	}
	else if( FW_IN1 == 0 && HW_IN2 == 1 )
	{
		BreakerStateLast = 2;	    // 分闸状态
	}
	else BreakerStateLast = 0;
	
	test_before();
	
  os_sys_init (Task); /* Initialize RTX and start init     */ 
	while(1);
}

/**************************************************************************************
* FunctionName   : Task1()  50ms
* Description    : 任务1    
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task1(void)
{ 
	static u16 energy_cnt = 0;
	os_dly_wait (100);
	ReadPowerOffPara();	// 读取电量
	ReadMachineInformation();// 读取出厂编号和制造年月,操作次数
  if(SwitchCnt == 0xFFFF)
	  SwitchCnt = 0;

	if(ReadSystemPara() != 0x0D)	// 读系统参数
	{
		if(ReadSystemParaBAK() == 0x0D)	// 正常系统参数出错，备份系统参数正确，则重新保存正常系统参数
		{
			SaveSystemPara();
		}
	}	
	if(ReadProtectPara() != 0x0D)	// 读取保护参数
	{
		if(ReadProtectParaBAK() == 0x0D)	// 正常保护参数出错，备份保护参数正确，则重新保存正常保护参数
		{
			SaveProtectPara();
		}
	}
	if(ReadAlarmPara() != 0x0D)	// 读取报警参数
	{
		if(ReadAlarmParaBAK() == 0x0D)	// 正常报警参数出错，备份报警参数正确，则重新保存正常报警参数
		{
			SaveAlarmPara();
		}
	}
	if(ReadAutoSwitchPara() != 0x0D)	// 读取自动解列参数设置
	{
		if(ReadAutoSwitchParaBAK() == 0x0D)	// 正常自动解列参数出错，备份自动解列参数正确，则重新保存正常自动解列参数
		{
			SaveAutoSwitchPara();
		}
	}
	if(ReadModulus() != 0x0D)	// 读取测量系数
	{
		if(ReadModulusBAK() == 0x0D)	// 正常测量系数出错，备份测量系数正确，则重新保存正常测量系数
		{
			SaveModulus();
		}
	}
	
	if(Read101Para() != 0x0D)	// 读取101规约参数
	{
		if(Read101ParaBAK() == 0x0D)	// 正常101规约参数出错，备份101规约参数正确，则重新保存正常101规约参数
		{
			Save101Para();
		}
	}	
	
	ReadVersionfromFlash();	 // 读取升级程序版本信息
	KZQUpdataSucced = ReadUpdateFlagfromFlash();
	ReadRecordIndexfromFlash();
	ReadAllLossSave();       //读取线损各项设置参数
	os_dly_wait (2000);
	while(1)
	{
		if(PowerOffCnt > 20)	// 检测时间为(0.02 && 0.05)秒
		{
			//YabanSave_Flag = 1;			            // 保存电量(不太可靠)
			KZQMeasureData.para.ProtectFlag2.bit.poweroff = 1;
		}
		
		if(WCN_IN3 == 1)				            // 储能
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
* FunctionName   : Task2()	 遥信变位扫描，分合闸灯控制   10ms
* Description    : 任务2	
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
* FunctionName   : Task3()	   1秒计数 		 1000ms
* Description    : 任务3 
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
	DJZZKZ_OFF;       //电机正转控制关
	DJFZKZ_OFF;       //电机反转控制关
	GPRS_POW_OFF;
	GPRS_PWRKEY_OFF;
	WIFI_OFF;
	GPS_OFF;
	BATTERY_CHARGE_OFF;//电池充电关闭

	while(1)
	{
    if(GPS_Flag)
		{
			if(gps_cnt == 0)
			{
				USART5_Configuration(9600);
			  WIFI_OFF;    //wifi关
			  GPS_ON;      //gps开
				GpsOn_Flag = 1;
			}
			gps_cnt++;
      if(gps_cnt > 60 || KZQMeasureData.para.SelfCheck.bit.GPS_state)
			{
			  WifiConnectState = 0;	     // wifi模块重新软启动
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
		
		if(ReOnLockCnt < 10)	// 重合闸闭锁计时
		{
			ReOnLockCnt++;
		}
		//控制字投入，开关在合后状态
		if(ProtectSet.para.once_reon_enable == 1 && KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 重合闸充电计时
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
			delay_time_101++;//这个用来计数超时

		if(SystemSet.para.wifi_gps_switch == 1)	
		{
			SystemSet.para.wifi_gps_switch = 0xFF;
			SetSystemFlag=1;
			USART5_Configuration(9600);
			WIFI_OFF;    //wifi关
			GPS_ON;      //gps开
			GpsOn_Flag = 1;
		}
		else if(SystemSet.para.wifi_gps_switch == 0)	
		{
			SystemSet.para.wifi_gps_switch = 0xFF;
			SetSystemFlag=1;
			WifiConnectState = 0;	     // wifi模块重新软启动
		}
		else if(SystemSet.para.wifi_gps_switch == 2)	
		{
			SystemSet.para.wifi_gps_switch = 0xFF;
			SetSystemFlag=1;
			WIFI_OFF;    //wifi关
			GPS_OFF;     //gps关
		}
		
		if(Freez_15min_cnt == 0)
		{
			Freez_15min_cnt =900;
			lockbz.bit.D1=1;
		}
		Freez_15min_cnt--;
		
		//线损模块事件记录判断
		if(MeasureData.PA_val<0 && abs(MeasureData.PA_val)>LineSet.para.P_Reverse_Limit)
		{
			if(PReverse.bit.PA_Reverse == 0)
			  LineLossNum.PA_Reverse_Delay++;
			else
				LineLossNum.PA_Reverse_Delay = 0;
			if(LineLossNum.PA_Reverse_Delay>LineSet.para.P_Reverse_Time)
		    PReverse.bit.PA_Reverse=1;
		}
		else  //不触发功率反向事件
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
		else //不触发功率反向事件
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
		else //不触发功率反向事件
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
		else //不触发潮流方向改变事件
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
* FunctionName   : Task4()	   通讯协议处理任务 		 100ms
* Description    : 任务4 
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
							CommandProcess1Prepare();                 	// 命令预处理
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
				if(Usart2RxReady == 1)                        // 命令处理
				{
					CommandProcess2();
					Usart2RxReady = 0;
				}
				else if(Usart2bakRxReady == 1)
				{
					CommandProcess2Prepare();	                  // 命令预处理
					CommandProcess2();
					Usart2bakRxReady = 0;
				}
				if(Usart3RxReady == 1)                        // 命令处理
				{
					IEC_104_CommandProcess(2);
					Usart3RxReady = 0;
				}
				else if(Usart3bakRxReady == 1)
				{
					IEC_104_CommandProcessPrepare();	          // 命令预处理
					IEC_104_CommandProcess(2);
					Usart3bakRxReady = 0;
				}
				if(Usart4RxReady == 1)
				{
					Usart4RxReady = 0;
					CommandProcess4(3);			// 命令处理
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
					//没有初始化和遥控（优先级压制）
					if(y_104==0)
					{
						if((Super_Sign[1]==0)&&(Super_Sign[2]==0) && Callend_Flag)
						{
							//*********************遥信遥测主动上送******************************
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
						
						if(delay_time_101 > 15)  //101才有重发机制
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
						TimeYcYx(0);            //变位遥信遥测
					}
					else if(Measure101Para.para.encrypt == 1)
					{
						if(y_104==0)
						{
							//没有初始化和遥控（优先级压制）
							if((Super_Sign[1]==0)&&(Super_Sign[2]==0) && Callend_Flag)
							{
								//*********************遥信遥测主动上送******************************
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
* FunctionName   : Task5()	   串口数据发送置标志 		300ms
* Description    : 任务5
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
				case 0x01:		// 操作合闸					
					KZQMeasureData.para.RequestFlag1.bit.curve_on_A = 1;	// 置位A相合闸曲线
					KZQMeasureData.para.RequestFlag1.bit.curve_on_B = 1;	// 置位B相合闸曲线
					KZQMeasureData.para.RequestFlag1.bit.curve_on_C = 1;	// 置位C相合闸曲线
					break;
				case 0x02:		// 操作分闸
					KZQMeasureData.para.RequestFlag1.bit.curve_off_A = 1;	// 置位A相分闸曲线
					KZQMeasureData.para.RequestFlag1.bit.curve_off_B = 1;	// 置位B相分闸曲线
					KZQMeasureData.para.RequestFlag1.bit.curve_off_C = 1;	// 置C相分闸曲线
					break;
				case 0x03:		// 保护分闸
					KZQMeasureData.para.RequestFlag1.bit.curve_protect_A = 1;	// 置位A相保护曲线
					KZQMeasureData.para.RequestFlag1.bit.curve_protect_B = 1;	// 置位B相保护曲线
					KZQMeasureData.para.RequestFlag2.bit.curve_protect_C = 1;	// 置位C相保护曲线
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
				SaveSystemPara();		// 保存系统参数
				SaveSystemParaBAK();	// 保存系统参数备份
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
			  SaveProtectPara();				// 保存保护参数
			  SaveProtectParaBAK();			// 保存保护参数备份
			}
			SetProtectFlag=0;
		}
		else if(SetAlarmFlag==1)
		{
			if(AlarmParaCompare() != 0x0D)
			{
			  SaveAlarmPara();        // 保存报警参数
			  SaveAlarmParaBAK();		// 保存报警参数
			}
			SetAlarmFlag=0;
		}
		else if(SetAutoswitchFlag==1)
		{
			if(AutoswitchParaCompare() != 0x0D)
			{
			  SaveAutoSwitchPara();    // 保存自动解列参数
			  SaveAutoSwitchParaBAK(); // 保存自动解列参数	
			}
      SetAutoswitchFlag=0;			
		}
		else if(ModulusFlag==1)
		{
			if(ModulusParaCompare() != 0x0D)
			{
			  SaveModulus();  // 保存测量系数
			  SaveModulusBAK();
			}
			ModulusFlag=0;
		}
		else if(Set101ParaFlag==1)
		{
			if(Measure101ParaCompare() != 0x0D)
			{
			  Save101Para();  // 保存101规约参数
			  Save101ParaBAK();
			}
			Set101ParaFlag=0;
		}
		else if(YabanSave_Flag==1)
		{
			SavePowerOffPara();	// 保存压板数据
			YabanSave_Flag=0;
		}
		else if(KZQMeasureData.para.RequestFlag1.bit.record_on)
		{
			SaveRecordOn();// 保存合闸记录
			KZQMeasureData.para.RequestFlag1.bit.record_on = 0;
		}
		else if(KZQMeasureData.para.RequestFlag1.bit.record_off)
		{
			SaveRecordOff();  // 保存分闸记录
			KZQMeasureData.para.RequestFlag1.bit.record_off = 0;
		}
		else if(KZQMeasureData.para.RequestFlag1.bit.record_protect)
		{
			if((ProtectRecord_flag&0x0003) == 1)
			{
				SaveRecordProtect1(); // 保存保护记录						
			}
			else if((ProtectRecord_flag&0x0003) == 2)
			{
				SaveRecordProtect2(); // 保存保护记录
			}
			KZQMeasureData.para.RequestFlag1.bit.record_protect = 0;
		}
		else if(KZQMeasureData.para.RequestFlag1.bit.record_autofeed)
		{
			SaveRecordAutofeed();// 保存馈线自动化记录
			KZQMeasureData.para.RequestFlag1.bit.record_autofeed = 0;
		}
		else if(AlarmRecord_flag == 1)// 保存报警记录
		{
			AlarmRecord_flag = 0;
			SaveRecordAlarm();
		}
		else if(ClearTJXX_Flag == 1)  //清空记录信息
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
					//MeasureData.Psaveflag = 0;  //开启这条指令，曲线内存数据将迅速更新
				}
		  }
		}
		
		if(MeasureData.Psaveflag==0)
			PCurve_Readflag = 0;
		
    Lock_Event();   //冻结事件记录
		
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
	
		if(TESTFlag == 0)	// 出厂检测状态
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
			WIFI_OFF;    //wifi关
			GPS_ON;      //gps开
			GpsOn_Flag = 1;
		}
		if(switch_flag == 1)
		  switch_cnt++;
		if(switch_cnt > 2000) //600s后开wifi
		{
			switch_cnt = 0;
		  switch_flag = 0;
			WifiConnectState = 0;	     // wifi模块重新软启动
		}
		
		os_evt_set (0x0010, t_Task17);
		os_dly_wait (300);
	}
}

/**************************************************************************************
* FunctionName   : Task6()    串口数据发送处理  15ms
* Description    : 任务6 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task6(void)
{ u16 i = 0;
	u16 NumTemp = 0;
 	os_dly_wait (2000);
	while(1)
	{
		/****************串口1数据发送处理*****************/	
		if(GetQueueDataNum(&QueueTX1) > 0 )									// 判断发送队列是否为空
		{
			NumTemp = GetQueueDataNum(&QueueTX1);
			 
			 if(DMA_GetCurrDataCounter(DMA2_Stream7) == 0)					// DMA缓冲区空
			 {
				 if(NumTemp > 299)
				 {	
						for(i=0;i<300;i++)
						{
					   DMATXbuffer1[i] = GetDataFromQueue(&QueueTX1,BufferTX1);	
						}
						DMA2_Stream7->NDTR = 300;							// 本次传送字节数为300
						DMA_Cmd(DMA2_Stream7, ENABLE);	 					// 使能DMA发送
				 }
				 else
				 {
						
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer1[i] = GetDataFromQueue(&QueueTX1,BufferTX1);	
						}
						DMA2_Stream7->NDTR = NumTemp;							// 本次传送字节数为NumTemp
						DMA_Cmd(DMA2_Stream7, ENABLE);	 					// 使能DMA发送
				 }
			 }
		}
	/****************串口2数据发送处理*****************/	
		if(GetQueueDataNum(&QueueTX2) > 0 && Usart2TxReady == 1)									// 判断发送队列是否为空
		{
			 NumTemp = GetQueueDataNum(&QueueTX2);
			 
			 if(DMA_GetCurrDataCounter(DMA1_Stream6) == 0)					// DMA缓冲区空
			 {
				 if(NumTemp > 819)
				 {
						T485_2_TX;
						for(i=0;i<820;i++)
						{
								 DMATXbuffer2[i] = GetDataFromQueue(&QueueTX2,BufferTX2);	
						}
						DMA1_Stream6->NDTR = 820;							// 本次传送字节数为820
						DMA_Cmd(DMA1_Stream6, ENABLE);	 					// 使能DMA发送
				 }
				 else
				 {
						T485_2_TX;
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer2[i] = GetDataFromQueue(&QueueTX2,BufferTX2);	
						}
						
						DMA1_Stream6->NDTR = NumTemp;							// 本次传送字节数为NumTemp
						DMA_Cmd(DMA1_Stream6, ENABLE);	 					// 使能DMA发送
				 }
			 }
		}
	/****************串口3数据发送处理*****************/	
		if(GetQueueDataNum(&QueueTX3) > 0 )									// 判断发送队列是否为空
		{
			 NumTemp = GetQueueDataNum(&QueueTX3);
			 
			 if(DMA_GetCurrDataCounter(DMA1_Stream3) == 0)					// DMA缓冲区空
			 {
				 if(NumTemp > 99)
				 {
						for(i=0;i<100;i++)
						{
								 DMATXbuffer3[i] = GetDataFromQueue(&QueueTX3,BufferTX3);	
						}
						DMA1_Stream3->NDTR = 100;							// 本次传送字节数为100
						DMA_Cmd(DMA1_Stream3, ENABLE);	 					// 使能DMA发送
				 }
				 else
				 {
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer3[i] = GetDataFromQueue(&QueueTX3,BufferTX3);	
						}
						DMA1_Stream3->NDTR = NumTemp;							// 本次传送字节数为NumTemp
						DMA_Cmd(DMA1_Stream3, ENABLE);	 					// 使能DMA发送
				 }
			 }
		}
	/****************串口4数据发送处理*****************/	
		if(GetQueueDataNum(&QueueTX4) > 0)									// 判断发送队列是否为空
		{
			 NumTemp = GetQueueDataNum(&QueueTX4);
			 if(DMA_GetCurrDataCounter(DMA1_Stream4) == 0)					// DMA缓冲区空
			 {
				 if(NumTemp > 299)
				 {
						for(i=0;i<300;i++)
						{
								 DMATXbuffer4[i] = GetDataFromQueue(&QueueTX4,BufferTX4);	
						}
						DMA1_Stream4->NDTR = 300;							// 本次传送字节数为300
						DMA_Cmd(DMA1_Stream4, ENABLE);	 					// 使能DMA发送
				 }
				 else
				 {
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer4[i] = GetDataFromQueue(&QueueTX4,BufferTX4);	
						}
						DMA1_Stream4->NDTR = NumTemp;							// 本次传送字节数为NumTemp
						DMA_Cmd(DMA1_Stream4, ENABLE);	 					// 使能DMA发送
				 }
			 }
		}
	/****************串口5数据发送处理*****************/	
		if(GetQueueDataNum(&QueueTX5) > 0 )									// 判断发送队列是否为空
		{
			 NumTemp = GetQueueDataNum(&QueueTX5);
			 if(DMA_GetCurrDataCounter(DMA1_Stream7) == 0)					// DMA缓冲区空
			 {
				 if(NumTemp > 419)
				 {
						for(i=0;i<420;i++)
						{
								 DMATXbuffer5[i] = GetDataFromQueue(&QueueTX5,BufferTX5);	
						}
						DMA1_Stream5->NDTR = 420;							    // 本次传送字节数为420
						DMA_Cmd(DMA1_Stream5, ENABLE);	 					// 使能DMA发送
				 }
				 else
				 {
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer5[i] = GetDataFromQueue(&QueueTX5,BufferTX5);	
						}
						DMA1_Stream7->NDTR = NumTemp;							// 本次传送字节数为NumTemp
						DMA_Cmd(DMA1_Stream7, ENABLE);	 					// 使能DMA发送
				 }
			 }
		}
	/****************串口6数据发送处理*****************/	
		if(GetQueueDataNum(&QueueTX6) > 0)									// 判断发送队列是否为空
		{
			 NumTemp = GetQueueDataNum(&QueueTX6);
			 if(DMA_GetCurrDataCounter(DMA2_Stream6) == 0)					// DMA缓冲区空
			 {
				 if(NumTemp > 99)
				 {
						for(i=0;i<100;i++)
						{
								 DMATXbuffer6[i] = GetDataFromQueue(&QueueTX6,BufferTX6);	
						}
						DMA2_Stream6->NDTR = 100;							// 本次传送字节数为100
						DMA_Cmd(DMA2_Stream6, ENABLE);	 					// 使能DMA发送
				 }
				 else
				 {
						for(i=0;i<NumTemp;i++)
						{
								DMATXbuffer6[i] = GetDataFromQueue(&QueueTX6,BufferTX6);	
						}
						DMA2_Stream6->NDTR = NumTemp;							// 本次传送字节数为NumTemp
						DMA_Cmd(DMA2_Stream6, ENABLE);	 					// 使能DMA发送
				 }
			 }
		}

		/****************串口2数据接收处理*****************/
		if(DMA_GetCurrDataCounter(DMA1_Stream5) == 50 && Usart2TxReady == 0)	// 判断串口是否可以发送数据	485接口
		{
			Usart2TxReady = 1;	
		}
	
		os_evt_set (0x0020, t_Task17);
	  os_dly_wait (15);
	}
}

/**************************************************************************************
* FunctionName   : Task7()  前置器程序更新控制  1000ms
* Description    : 任务7 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task7(void)
{
	os_dly_wait (2000);
	while(1)
	{
		switch(RecoverKZQBackupFlag)  	// 前置器程序更新控制
		{
			case 0:	// 无更新
				
				break;
			case 1:	// 本地更新
				RecoverKZQBackupFlag = 0;
				SetUpdateFlag(0x01);	// 本地更新
			  os_dly_wait (100);
				__set_FAULTMASK(1);
				NVIC_SystemReset();
        //JumpToApp();
			  break;
			case 2:	// 备份更新
				RecoverKZQBackupFlag = 0;
				SetUpdateFlag(0x02);	// 备份更新
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
* FunctionName   : Task8()读温度和湿度函数，读一次的数据，共五字节；
* Description    : 任务8   10*1000ms = 10s 读一次
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
* Description    : 任务9    1000ms
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
* Description    : 任务10 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task10(void)
{
	u8 ledtimecnt=0;
	os_dly_wait (2000);
	while(1)
	{
		if(TESTFlag == 0)	// 出厂检测状态
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
				os_dly_wait (10);          //延迟10ms去抖
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
* Description    : 任务11 
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
		if(CTDX_Enable == 0)	// CT断线检测条件不具备
		{
			if(MeasureData.Protect_IA_val>500 && MeasureData.Protect_IB_val>500 && MeasureData.Protect_IC_val>500)	// 
			{
				CTDX_Enable = 1;
				CTDX_Flag = 0;
				ctdx_cnt = 0;
			}
		}
		if(CTDX_Enable == 1)	// CT正常，启动断线检测
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

		if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && ProtectTimesLock != 0)	// 保护状态，合位，若故障清除，则解除跳闸次数闭锁	
		{
			ProtectTimesLockCnt++;
			if(ProtectTimesLockCnt>3)
			{
				ProtectTimesLockCnt = 0;
				ProtectTimesLock = 0;
			}
		}

		if(AlarmTimesLock != 0)	// 报警状态
		{
			AlarmTimesLockCnt++;
			if(AlarmTimesLockCnt>3)
			{
				AlarmTimesLockCnt = 0;
				AlarmTimesLock = 0;
			}
		}
		
	  if(TESTFlag == 0)	// 出厂检测状态
	  {
			if( DYXH1 == 1 && DYXH2 == 1 && BattryCnt == 0 ) //变压器断电，电池断电倒计时
			{
				OUT6_ON;
				BATT_ON;  
				BattryCnt = SystemSet.para.battery_delay<<1; //系统参数更新计时BattryCnt
				if(BattryCnt == 0) 
					BattryCnt = 1;
				BattryTimeOut = 0; 
			}
			else if(DYXH1 == 0 || DYXH2 == 0) //变压器上电
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
				BattryCnt = 1;     //防止继电器重新吸合
			}
		}

		if(Battery_Readflag == 0 && TESTFlag == 0)
		  BATTERY_CHARGE_ON;
		if(ZeroTimeArrived == 1)	// 午夜零点时刻做的事情
		{
			ZeroTimeArrived = 0;
			Battery_RunTime++; //电池运行天数
			buf[0] = Battery_RunTime & 0x00FF;
			buf[1] = Battery_RunTime >> 8;
			CS2BZ=0;
			SPI_Flash_Write(buf,FLASH_BATTERY_RUNTIME_ADDR,2);// 写入flash
			Battery_Voltage_Save();	
									
			WpLast = 0;			// 当天清空电量
			WqLast = 0;	
			YabanSave_Flag = 1;			// 保存当前电量
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
    if(TESTFlag == 0)	// 出厂检测状态
	  {
			if((abs(MeasureData.IA_val - MeasureData.Protect_IA_val) > 300 && MeasureIAError == 0) ||
				 (abs(MeasureData.IB_val - MeasureData.Protect_IB_val) > 300 && MeasureIBError == 0) ||
				 (abs(MeasureData.IC_val - MeasureData.Protect_IC_val) > 300 && MeasureICError == 0) ||
					abs(MeasureData.UA_val - MeasureData.Protect_UA_val) > 500 ||
					abs(MeasureData.UB_val - MeasureData.Protect_UB_val) > 500 ||
					abs(MeasureData.UC_val - MeasureData.Protect_UC_val) > 500)	// 电度表芯片自检
			{
				SelfCheckAtt7022Cnt++;
				if(SelfCheckAtt7022Cnt > 5)
				{
					SelfCheckAtt7022Cnt = 0;
					KZQMeasureData.para.SelfCheck.bit.att7022 = 1;
					if(SelfCheckErrCode == 0)
						SelfCheckErrCode = 0x0300;
					ATT7022_Init();		  // 电度表初始化
					if(Read_ATT7022() != 0x0D)		  // 读取ATT7022校表参数
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
		
		if(minute_cnt < 120)	// 一分钟检查保护参数
		{
			minute_cnt++;
		}
		else
		{
			CheckSystemPara();          // 检查系统参数设置
			CheckProtectPara();			    // 检查保护参数设置
			CheckAlarmPara();           // 检查报警参数设置
			CheckAutoSwitchPara();      // 检查自动解列参数设置
			CheckModulus();             // 检查测量系数
			CheckMeasure101Para();      // 检查101规约参数
			if(saveATT7022_flag == 0)
			  Check_ATT7022();
			minute_cnt = 0;
		}
		
		if(SwitchFlag == 0 && CtrlMeasureFlag == 0)
	  {
		  //读取时间芯片
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
	    // 读取电度表芯片信息
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
				MeasureData.IB_val=ATT7022_IbRms/503;//电流电压如果按说明书校准就会溢出
			
			ATT7022_IcRms = ReadAT7052(ICRMS);
				MeasureData.IC_val=ATT7022_IcRms/503;

			ATT7022_Pa = ReadAT7052(PA);
			if(ATT7022_Pa>0x800000)
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// 下进线方式
					KZQMeasureData.para.DLQflag.bit.directionA = 0;	// 正向
				else
					KZQMeasureData.para.DLQflag.bit.directionA = 1;
				MeasureData.Att7022Flag |= 0x0020;				
				MeasureData.PA_val = (float)(ATT7022_Pa-0x1000000)/ATT7022_K;
			}
			else
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// 下进线方式
					KZQMeasureData.para.DLQflag.bit.directionA = 1;	// 反向
				else
					KZQMeasureData.para.DLQflag.bit.directionA = 0;
				MeasureData.Att7022Flag &= ~0x0020;				
				MeasureData.PA_val = (float)ATT7022_Pa/ATT7022_K;
			}
			if(SystemSet.para.exchange_dycfzc == 1) MeasureData.PA_val = -MeasureData.PA_val;	// 下进线，电流负方向为正
			ATT7022_Pb = ReadAT7052(PB);
			if(ATT7022_Pb>0x800000)
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// 下进线方式
					KZQMeasureData.para.DLQflag.bit.directionB = 0;	// 正向
				else
					KZQMeasureData.para.DLQflag.bit.directionB = 1;
				MeasureData.Att7022Flag |= 0x0040;			
				MeasureData.PB_val = (float)(ATT7022_Pb-0x1000000)/ATT7022_K;
			}
			else
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// 下进线方式
					KZQMeasureData.para.DLQflag.bit.directionB = 1;	// 反向
				else
					KZQMeasureData.para.DLQflag.bit.directionB = 0;
				MeasureData.Att7022Flag &= ~0x0040;			
				MeasureData.PB_val = (float)ATT7022_Pb/ATT7022_K;
			}
			if(SystemSet.para.exchange_dycfzc == 1) MeasureData.PB_val = -MeasureData.PB_val;	// 下进线，电流负方向为正
			ATT7022_Pc = ReadAT7052(PC);
			if(ATT7022_Pc>0x800000)
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// 下进线方式
					KZQMeasureData.para.DLQflag.bit.directionC = 0;	// 正向
				else
					KZQMeasureData.para.DLQflag.bit.directionC = 1;
				MeasureData.Att7022Flag |= 0x0080;			
				MeasureData.PC_val = (float)(ATT7022_Pc-0x1000000)/ATT7022_K;
			}
			else
			{
				if(SystemSet.para.exchange_dycfzc == 1)	// 下进线方式
					KZQMeasureData.para.DLQflag.bit.directionC = 1;	// 反向
				else
					KZQMeasureData.para.DLQflag.bit.directionC = 0;
				MeasureData.Att7022Flag &= ~0x0080;			
				MeasureData.PC_val = (float)ATT7022_Pc/ATT7022_K;
			}
			if(SystemSet.para.exchange_dycfzc == 1) MeasureData.PC_val = -MeasureData.PC_val;	// 下进线，电流负方向为正
			ATT7022_Qa = ReadAT7052(QA);
			if(ATT7022_Qa>0x800000)
			{
				MeasureData.QA_val = (ATT7022_Qa-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.QA_val = ATT7022_Qa/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.QA_val = -MeasureData.QA_val;	// 下进线，电流负方向为正
			ATT7022_Qb = ReadAT7052(QB);
			if(ATT7022_Qb>0x800000)
			{
				MeasureData.QB_val = (ATT7022_Qb-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.QB_val = ATT7022_Qb/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.QB_val = -MeasureData.QB_val;	// 下进线，电流负方向为正
			ATT7022_Qc = ReadAT7052(QC);
			if(ATT7022_Qc>0x800000)
			{
				MeasureData.QC_val = (ATT7022_Qc-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.QC_val = ATT7022_Qc/ATT7022_K;
			
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.QC_val = -MeasureData.QC_val;	// 下进线，电流负方向为正
			//增加S 读取
			ATT7022_Sa = ReadAT7052(SA);
			if(ATT7022_Sa>0x800000)
			{
				MeasureData.SA_val = (ATT7022_Sa-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.SA_val = ATT7022_Sa/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.SA_val = -MeasureData.SA_val;	// 下进线，电流负方向为正
			
			ATT7022_Sb = ReadAT7052(SB);
			if(ATT7022_Sb>0x800000)
			{
				MeasureData.SB_val = (ATT7022_Sb-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.SB_val = ATT7022_Sb/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.SB_val = -MeasureData.SB_val;	// 下进线，电流负方向为正
			ATT7022_Sc = ReadAT7052(SC);
			if(ATT7022_Sc>0x800000)
			{
				MeasureData.SC_val = (ATT7022_Sc-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.SC_val = ATT7022_Sc/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.SC_val = -MeasureData.SC_val;	// 下进线，电流负方向为正
			ATT7022_Sa = ReadAT7052(ST);
			if(ATT7022_St>0x800000)
			{
				MeasureData.S_val = (ATT7022_St-0x1000000)/ATT7022_K;
			}
			else
				MeasureData.S_val = ATT7022_St/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.S_val = -MeasureData.S_val;	// 下进线，电流负方向为正
			
			ATT7022_Pfa = ReadAT7052(PFA);
			if(ATT7022_Pfa>0x800000)
			{
				MeasureData.cosA = (ATT7022_Pfa-0x1000000)/8388.608;
			}
			else
				MeasureData.cosA = ATT7022_Pfa/8388.608;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.cosA = -MeasureData.cosA;	// 下进线，电流负方向为正
			ATT7022_Pfb = ReadAT7052(PFB);
			if(ATT7022_Pfb>0x800000)
			{
				MeasureData.cosB = (ATT7022_Pfb-0x1000000)/8388.608;
			}
			else
				MeasureData.cosB = ATT7022_Pfb/8388.608;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.cosB = -MeasureData.cosB;	// 下进线，电流负方向为正
			ATT7022_Pfc = ReadAT7052(PFC);
			if(ATT7022_Pfc>0x800000)
			{
				MeasureData.cosC = (ATT7022_Pfc-0x1000000)/8388.608;
			}
			else
				MeasureData.cosC = ATT7022_Pfc/8388.608;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.cosC = -MeasureData.cosC;	// 下进线，电流负方向为正

			ATT7022_Pt = ReadAT7052(PT);
			if(ATT7022_Pt>0x800000)
			{
				MeasureData.P_val = (float)(ATT7022_Pt-0x1000000)*2/ATT7022_K;
			}
			else
				MeasureData.P_val = (float)ATT7022_Pt*2/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.P_val = -MeasureData.P_val;	// 下进线，电流负方向为正

			ATT7022_Qt = ReadAT7052(QT);
			if(ATT7022_Qt>0x800000)
			{
				MeasureData.Q_val = (ATT7022_Qt - 0x1000000)*2/ATT7022_K;
			}
			else
				MeasureData.Q_val = ATT7022_Qt*2/ATT7022_K;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.Q_val = -MeasureData.Q_val;	// 下进线，电流负方向为正

			ATT7022_Pft = ReadAT7052(PFT);
			if(ATT7022_Pft>0x800000)
			{
				ATT7022_Pft = ATT7022_Pft - 0x1000000;
				MeasureData.cosALL = ATT7022_Pft/8388.608;
			}
			else 
				MeasureData.cosALL = ATT7022_Pft/8388.608;
			if(SystemSet.para.exchange_dycfzc == 1)MeasureData.cosALL = -MeasureData.cosALL; // 下进线，电流负方向为正

			ATT7022_Ept = ReadAT7052(EPT);
			WpLast += ATT7022_Ept;
			MeasureData.Wp = (float)WpLast*(float)ATT7022_EPTK;
			if(KZQMeasureData.para.DLQflag.bit.directionA == 0)Wp_all_Last += ATT7022_Ept;			// 上一次总有功电能
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
				if(MeasureData.Q_val > 0)	// 第一象限
				{
				Wq_1all_Last += ATT7022_Eqt;			// 上一次总无功电能	
				}
				else // 第四象限
				{
				Wq_4all_Last += ATT7022_Eqt;			// 上一次总无功电能	
				}
			}
			else
			{
				if(MeasureData.Q_val > 0)	// 第二象限
				{
				Wq_2all_Last += ATT7022_Eqt;			// 上一次总无功电能	
				}
				else // 第三象限
				{
				Wq_3all_Last += ATT7022_Eqt;			// 上一次总无功电能	
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

			if(PeriodMode[TimeNow.hour] == 1)	// 峰时段电量统计
			{
				if(KZQMeasureData.para.DLQflag.bit.directionA == 0)Wp_peak_Last += ATT7022_Ept;		// 上一次峰有功电能
				else WpFX_peak_Last += ATT7022_Ept;
			}
			else if(PeriodMode[TimeNow.hour] == 2)// 谷时段电量统计
			{
				if(KZQMeasureData.para.DLQflag.bit.directionA == 0)Wp_valley_Last += ATT7022_Ept;		// 上一次谷有功电能
				else WpFX_valley_Last += ATT7022_Ept;
			}
			else if(PeriodMode[TimeNow.hour] == 3)// 平时段电量统计
			{
				if(KZQMeasureData.para.DLQflag.bit.directionA == 0)Wp_level_Last += ATT7022_Ept;		// 上一次平有功电能
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
		  EXIT_WATCHDOG_0;		//外部看门狗喂狗
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
* Description    : 任务12 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
__task void Task12(void)
{
	static u8 breaker_changecnt = 0;
	os_dly_wait (2000);
	YXChangeInit();     // 遥信变位初始化
	while(1)
	{		
		if(KZQMeasureData.para.DLQflag.bit.breaker != 0)	// 判断开关位置异常变动
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
				if(SwitchFlag == 0)	// 非程序控制
				{
					if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && BreakerStateLast != 0)	// 手动合闸事件
					{
						if(KZQMeasureData.para.RequestFlag1.bit.record_on == 0)
						{
							RecordSOE_Pad(SOE_ADDR_ACT_MAN_ON_pad,1);
						  RecordSwitchOn(TYPE_ACT_MAN_ON);// 手动合闸记录
						  Record_on.para.type |= 0x80;
						  SaveRecordOn();// 保存合闸记录
						}
					}
					else if(KZQMeasureData.para.DLQflag.bit.breaker == 2 && BreakerStateLast != 0) // 手动分闸事件
					{
						if(KZQMeasureData.para.RequestFlag1.bit.record_off == 0)
						{
							RecordSOE_Pad(SOE_ADDR_ACT_MAN_OFF_pad,1);
						  RecordSwitchOff(TYPE_ACT_MAN_OFF);  // 手动分闸记录
						  Record_off.para.type |= 0x80;
						  SaveRecordOff();  // 保存分闸记录
						}
					}
				}
				BreakerStateLast = KZQMeasureData.para.DLQflag.bit.breaker;
				breaker_changecnt = 0;
			}
		}
		
		if(DYXH1 == 1 && DYXH2 == 1)  //掉电
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
		CtrlReOn(&ReOnFlag);          //重合闸判断
		if(TESTFlag == 0)
	    StoreOutput();                //储能判断
		SwapYXChange();	   	          // SOE遥信变位扫描
		os_evt_set (0x0800, t_Task17);
		os_dly_wait (1);
	}
}

/**************************************************************************************
* FunctionName   : Task13()	  ADC转换读取  10ms
* Description    : 任务13 
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
* Description    : 任务14 
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
* Description    : 任务15 
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
* Description    : 任务16 
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
* FunctionName   : Task17()	    看门狗监控   500ms
* Description    : 任务17 
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
