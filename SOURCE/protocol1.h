/*************************************************************
*文件名称：protocol1.h
*创建日期：2018年11月16日
*完成日期：
*作    者：shq
**************************************************************/
#include "global.h"
#ifndef _PROTOCOL1_H_
#define _PROTOCOL1_H_

// 硬件电源和复位引脚定义
#define GPRS_POW_ON			  GPIO_ResetBits(GPIOF,GPIO_Pin_1) 	/* 打开GPRS模块电源 */
#define GPRS_POW_OFF		  GPIO_SetBits(GPIOF,GPIO_Pin_1) 		/* 关闭GPRS模块电源 */

#define GPRS_PWRKEY_ON		GPIO_SetBits(GPIOF,GPIO_Pin_0) 		/* 打开PWRKEY */
#define GPRS_PWRKEY_OFF		GPIO_ResetBits(GPIOF,GPIO_Pin_0) 	/* 关闭PWRKEY */

#define balance            Measure101Para.para.balance //非平衡模式
#define Addr101            Measure101Para.para.addr_len //1或2个
#define Reason101          Measure101Para.para.reason_len //1个或2个
#define DataFloat          Measure101Para.para.data_type
#define Ctrl_offset        Measure101Para.para.Remote_Ctrl
#define Reset_offset       Measure101Para.para.Remote_Reset
#define Max_Time  15

#define TotalLen68         (Addr101*2+Reason101-3)//68总帧长偏移个数
#define TotalLen10         Addr101+3//10帧长偏移量
#define FrontReason        (Addr101-1)//传送原因前

#define CURRENT_POSITIVE_ENERGY          0x6401      //当前正向有功电能
                                                     //当前正向无功电能
																										 //当前一象限无功电能
																										 //当前四象限无功电能
#define CURRENT_REVERSE_ENERGY           0x6405			 //当前反向有功电能
																										 //当前反向无功电能
																										 //当前二象限无功电能
																										 //当前三象限无功电能
#define FREEZE_15min_POSITIVE_ENERGY     0x6409      //15分钟冻结 正向有功电能
                                                     //15分钟冻结 正向无功电能
																										 //15分钟冻结 一象限无功电能
																										 //15分钟冻结 四象限无功电能
																										 //15分钟冻结 反向有功电能
																										 //15分钟冻结 反向无功电能
																										 //15分钟冻结 二象限无功电能
																										 //15分钟冻结 三象限无功电能
#define FREEZE_day_POSITIVE_ENERGY       0x6411      //日冻结 正向有功电能
                                                     //日冻结 正向无功电能
																										 //日冻结 一象限无功电能
																										 //日冻结 四象限无功电能
																										 //日冻结 反向有功电能
																										 //日冻结 反向无功电能
																										 //日冻结 二象限无功电能
																										 //日冻结 三象限无功电能
#define FREEZE_change_POSITIVE_ENERGY    0x6419      //潮流变化冻结 正向有功电能
                                                     //潮流变化冻结 正向无功电能
																										 //潮流变化冻结 一象限无功电能
																										 //潮流变化冻结 四象限无功电能
																										 //潮流变化冻结 反向有功电能
																										 //潮流变化冻结 反向无功电能
																										 //潮流变化冻结 二象限无功电能
																										 //潮流变化冻结 三象限无功电能
#define TELEMETRY_LINELOSS_FREQ          0x4001			 //频率	
#define TELEMETRY_LINELOSS_CURRENT       0x4002			 //A相电流
                                                     //B相电流
                                                     //C相电流
                                                     //A相电压
                                                     //B相电压
                                                     //C相电压
#define TELEMETRY_LINELOSS_POWER         0x4008      //A相有功功率
                                                     //B相有功功率
                                                     //C相有功功率
                                                     //总有功功率
                                                     //A相无功功率
                                                     //B相无功功率
                                                     //C相无功功率
                                                     //总无功功率
                                                     //A相视在功率
                                                     //B相视在功率
                                                     //C相视在功率
                                                     //总视在功率
                                                     //A相功率因数
                                                     //B相功率因数
                                                     //C相功率因数
                                                     //总功率因数																						 

// 对后台私有协议结构体
struct BACKSTAGE_PROTOCOL_TYPE1      				// 
{
	u8 DataBuffer[850];	// 数据接收缓冲
};

struct CTRL_FIELD			// 控制域C
{
	u8 FC:4;	    // 功能码
	u8 FCV:1;		  // 帧计数有效位
	u8 FCB:1;	    // 帧计数位
	u8 PRM:1;		  // 启动标志位
	u8 DIR:1;	    // 传输方向位
};
union Control_Field
{
	struct CTRL_FIELD bit;
	u8 byte;
};

struct CTRL_BITS			// 遥控控制字
{
	u8 QCS_RCS:2;	// 单点为0分;1合  //双点为0,3:不允许;1,分;2,合
	u8 QU:5;		  // 1:短脉冲输出;2:长脉冲;3:连续输出
	u8 S_E:1;	    // 命令状态，=0为执行；=1为选择；
};
union Control_Type
{
	struct CTRL_BITS bit;
	u8 byte;
};
struct Set_BITS
{
	u8 CONT:1;//0-无后续 1-有后续
	u8 RES:5;//保留
	u8 CR:1;//0-未用 1-取消预置
	u8 S_E:1;//0-固化 1-预置
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
extern u8 List_Name[30];//目录名称 历史存储 防止其他帧插入-0x1fff7a10
extern u8 List_ID[4];//预留0填充
extern u8 List_NameNum;
extern u8 Doc_Name[20];
extern u8 Doc_NameNum;
extern u8 Doc_ID[4];//文件ID 预留 保留 0填充
extern u8 JHDoc_ID[4];//激活文件ID 写文件时需要判断
extern u16 SN_ID;
extern u16 TestAddr;//测试链路图像
//标志优先级 表示进入低几步
extern u8 InitSign;
extern u8 InitComplete;//初始化步骤
extern u8 ControlSign;//遥控步骤
extern u8 TotalSign;//总召唤
extern u8 SetSign;//设置
extern u8 DocSign;//文件
extern u8 TestSign;//测试 不平衡使用 平衡不使用
extern u8 EnergySign;//召唤电能量步骤
extern u8 ClockSign;
extern u8 ResetSign;
extern u8 DocSameSign;
extern u8 WriteDocSign;
extern u8 SoftUpSign;
//写文件 大小
extern u32 DocLen;//文件总大小
extern u8 DocErr;//文件包判断
extern u8 DocByte[240];//文件内容
extern u8 DocByteNum;//文件大小
extern u8 CheckTime_Flag;
extern u8 Clearloss_Flag;
extern u8 Clearevent_Flag;
extern u8 Rand_Flag;
extern u8 LockSet_Flag;

//读参数 写参数 响应报文
extern u16 LockSetAddr[20];
extern u8  LockSetValue[20][24];
extern u8  LockSetType[20];
extern u8  LockSetNum[20];//只做了4字节一下的参数设置
extern u8  LockSetTotal;//设定总个数

extern struct BACKSTAGE_PROTOCOL_TYPE1 BackstageProtocol1;
extern struct BACKSTAGE_PROTOCOL_TYPE1 BackstageProtocol1bak;

#ifdef Four_Faith_MODEL
u8 ProtocolResolveEncryptionData(u8 receivedata);
#endif
#ifdef SIM800C_MODEL
// AT命令定义
#define SIM900B_AT			"AT"				/* 测试串口 */
#define SIM900B_STATUS	"AT+CIPSTATUS"		/* TCP连接状态 */
#define SIM900B_IPR			"AT+IPR?"			/* 查询波特率 */
#define SIM900B_CFUN		"AT+CFUN=1"			/* 打开通信功能 */
#define SIM900B_CLIP		"AT+CLIP=1"			/* 显示来电号码 */
#define SIM900B_ATH			"ATH"				/* 挂断电话 */
#define SIM900B_ATA			"ATA"				/* 接听电话 */
#define SIM900B_CMGR		"AT+CMGR=1"			/* 读取新短信 */
#define SIM900B_CMGD		"AT+CMGD=1"			/* 删除第几条短信 */
#define SIM900B_CSCA		"AT+CSCA?"			/* 读取sms服务中心号码 */
#define SIM900B_CIICR		"AT+CIICR"			/* 激活移动场景*/
#define SIM900B_SHUT		"AT+CIPSHUT"		/* 关闭移动场景返回到IP INITIAL */
#define SIM900B_CLOSE		"AT+CIPCLOSE"		/* 关闭TCP连接 */
#define SIM900B_SEND		"AT+CIPSEND="		/* 通过GPRS发送数据 */
#define SIM900B_CIFSR		"AT+CIFSR"			/* 获得本地IP */
#define SIM900B_START		"AT+CIPSTART=\"TCP\"," /*\"122.193.249.075\",\"9560\"" */		/* 启动TCP连接 *///"AT+CIPSTART=\"TCP\",\"122.193.249.075\",\"9510\""
#define SIM900B_TCP			"TCP"				/* TCP连接 */
#define SIM900B_UDP			"UDP"				/* UDP连接 */
#define SIM900B_IP			"122.193.249.075"	/* IP地址 */
#define SIM900B_PORT		"9510"				/* 端口号 */
#define SIM900B_CSQ			"AT+CSQ"			/* 读取信号强度 */
#define SIM900B_CGREG1	"AT+CGREG?"			/* 是否注册网络 */
#define SIM900B_ATE			"ATE0"				/* 取消回显 */
#define SIM900B_TCOUT		"+++"				/* 退出透传模式 */
#define SIM900B_ATO			"ATO"				/* 进入透传模式 */
#define SIM900B_CCFG		"AT+CIPCCFG=3,2,200,1" /* 配置透传模式 */
#define SIM900B_CMGF		"AT+CMGF=0"			/* 设置短消息格式为PDU方式 */
#define SIM900B_CPIN		"AT+CPIN?"			/* 查询输入密码 */
#define SIM900B_CGATT1	"AT+CGATT?"			/* 是否附着GPRS网络 */
#define SIM900B_CGATT		"AT+CGATT=1"		/* 附着GPRS网络 */
#define SIM900B_CSTT1		"AT+CSTT=\"CMIOTNTGTJPDZDH.JS\""	/* 设置APN */
#define SIM900B_CSTT		"AT+CSTT"	/* 设置APN */
#define SIM900B_MODE		"AT+CIPMODE=1"		/* 选择透传模式 */
#define SIM900B_CSGP		"AT+CIPCSGP=1,\"CMNET\",\"guest\",\"guest\""		/* 设置连接方式 */
#define SIM900B_CIFSR		"AT+CIFSR"			/* 查询本地IP地址 */
#define SIM900B_HEAD		"AT+CIPHEAD=1"		/* 接收数据加IP头 */
#define SIM900B_SERVER	"AT+CIPSERVER=1,2404"		/* 服务器模式 */
#define SIM900B_CIPSTATUS	"AT+CIPSTATUS" /* 获得IP状态 */
#define SIM900B_ENTER		"\r\n"				/* 命令结束符 */
extern u16 GprsWdg;			        // GPRS模块自定义看门狗
extern u8  IPswitchFlag;		    // IP地址连接切换标志
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
void Write_Doc_Data_OK(u8 USARTxChannel,u8 m);//写文件激活确认
void Write_DocOK_Deal(void);
void Write_Doc_Data_Deal(void);
void IEC101_Staid(u8 linkcontrol,u8 USARTxChannel);//固定帧	地址域1个字节
void IEC101_Staid_All(u8 USARTxChannel,u8 Reason);//总召唤确认	地址域1个字节	
void Test_101(u8 USARTxChannel);
void IEC101_Staid_Enegy(u8 number,u8 USARTxChannel,u8 Reason);//冻结和召唤电能脉冲计数量确认帧	地址域1个字节
void IEC101_Enegy_Send(u8 USARTxChannel);//电能脉冲计数量发送	地址域1个字节
void Time_Synchronization(u8 USARTxChannel,u8 reason);
void Read_RealData(u8 USARTxChannel);
void Read_YX(u8 USARTxChannel);//总召唤遥信
void Remote_Control(u8 USARTxChannel,u8 reason,u8 TI,u16 controladdr);
void Record_SOE_Send(u8 USARTxChannel);	
void InitEnd(u8 USARTxChannel);
void ResetLianlu(u8 USARTxChannel);
void InsertValue(u8 k); //写 
void SaveLockSet(void);
void SofeReset(void);
void GetAddr(void);
u8 SearchList(void);
u8 SearchDoc(void);
u8 H2SJ(u8 USARTxChannel);
#endif
