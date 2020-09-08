/*************************************************************
*文件名称：Encryption.h
*创建日期：2018年11月16日
*完成日期：
*作    者：shq
**************************************************************/
#include "global.h"
#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H_

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define RL(x, y) (((x) << (y)) | ((x) >> (32 - (y))))
#define FF(a, b, c, d, x, s, ac) a = b + (RL((a + F(b, c, d) + x + ac), s))
#define GG(a, b, c, d, x, s, ac) a = b + (RL((a + G(b, c, d) + x + ac), s))
#define HH(a, b, c, d, x, s, ac) a = b + (RL((a + H(b, c, d) + x + ac), s))
#define II(a, b, c, d, x, s, ac) a = b + (RL((a + I(b, c, d) + x + ac), s))
#define PP(x) (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24)

/********************类型标识**********************/

//监视方向的过程信息
#define M_SP_NA_1 1    	  // 单点信息
#define M_DP_NA_1 3     	// 双点信息
//#define M_ST_NA_1 5     // 步位置信息
//#define M_BO_NA_1 7     // 32位串
#define M_ME_NA_1 9       // 被测值，归一化值
#define M_ME_NB_1 11      // 被测值，标度化值
#define M_ME_NC_1 13      // 被测值，短浮点数
//#define M_IT_NA_1 15    // 累计量
//#define M_PS_NA_1 20    // 带状态检出的单点信息
//#define M_ME_ND_1 21    //不带品质描述的归一化被测值
#define M_SP_TB_1 30      //带时标单点信息
#define M_DP_TB_1 31      //带时标双点信息
#define M_FT_NA_1 42      //故障事件信息
#define M_IT_NB_1 206     //累计量,短浮点数
#define M_IT_TC_1 207     //带时标累计量,短浮点数

// 主站→子站 在控制方向的过程信息
#define C_SC_NA_1	45		// 单点遥控命令
#define C_DC_NA_1	46		// 双点遥控命令
#define C_RC_NA_1	47		// 升降命令
#define C_SE_NA_1	48		// 设定命令，归一化值
#define C_SE_NB_1	49		// 设定命令，标度化值
#define C_SE_NC_1	50		// 设定命令，短浮点数
#define C_BO_NA_1	51		// 32位的位串

#define C_SC_TB_1	58		// 带时标单点遥控命令
#define C_DC_TB_1	59		// 带时标双点遥控命令
#define C_RC_TB_1	60		// 带时标升降命令
#define C_SE_TA_1	61		// 带时标设定命令，归一化值
#define C_SE_TB_1	62		// 带时标设定命令，标度化值
#define C_SE_TC_1	63		// 带时标设定命令，短浮点数
#define C_BO_TB_1	64		// 带时标32位的位串



//在监视方向的系统命令
#define M_EI_NA_1	70		// 初始化结束

// 主站→子站 在控制方向的系统信息
#define C_IC_NA_1	100					// 召唤命令
#define C_CI_NA_1	101					// 电能脉冲召唤命令
#define C_RD_NA_1	102					// 读数据命令
#define C_CS_NA_1	103					// 时钟同步命令
#define C_TS_NA_1	104					// 测试命令
#define C_RP_NA_1	105					// 复位进程命令
#define C_CD_NA_1	106					// 延时获得命令
#define C_EI_NA_1	70					// 初始化结束
// 在控制方向的参数
#define P_ME_NA_1	110					// 装载参数命令
#define P_AC_NA_1	113					// 激活参数


//文件传输
#define F_SC_NA_1	122					// 召唤目录，选择文件
#define F_SG_NA_1	126					// 目录,文件
#define F_FR_NA_1	210					// 文件传输
#define F_SR_NA_1	211					// 软件升级

//参数传输
#define C_SR_NA_1	200					// 切换定值区
#define C_RR_NA_1	201					// 读定值区
#define C_RS_NA_1	202					// 读参数和定值
#define C_WS_NA_1	203					// 写参数和定值

// 传送原因
#define PER_CYC		1		  // 周期、循环
#define BACK		  2		  // 背景扫描
#define SPONT		  3		  // 突发
#define INIT		  4		  // 初始化
#define REQ			  5		  // 请求或被请求
#define ACT			  6		  // 激活
#define ACTCON	  7		  // 激活确认
#define DEACT		  8		  // 停止激活
#define DEACTCON	9		  // 停止激活确认
#define ACTTERM		10		// 激活结束
#define RETREM		11		// 远程命令引起的返送信息
#define RETLOC		12		// 当地命令引起的返送信息
#define FILETRANS	13		// 文件传输

#define INTROGEN	20		// 响应总召唤
#define INTRO1		21		// 响应第1组召唤
#define INTRO2		22		// 响应第2组召唤
#define INTRO3		23		// 响应第3组召唤
#define INTRO4		24		// 响应第4组召唤
#define INTRO5		25		// 响应第5组召唤
#define INTRO6		26		// 响应第6组召唤
#define INTRO7		27		// 响应第7组召唤
#define INTRO8		28		// 响应第8组召唤
#define INTRO9		29		// 响应第9组召唤
#define INTRO10		30		// 响应第10组召唤
#define INTRO11		31		// 响应第11组召唤
#define INTRO12		32		// 响应第12组召唤
#define INTRO13		33		// 响应第13组召唤
#define INTRO14		34		// 响应第14组召唤
#define INTRO15		35		// 响应第15组召唤
#define INTRO16		36		// 响应第16组召唤

#define REQCOGCN	37		// 响应计数量总召唤
#define REQCO1		38		// 响应第1组计数量召唤
#define REQCO2		39		// 响应第2组计数量召唤
#define REQCO3		40		// 响应第3组计数量召唤
#define REQCO4		41		// 响应第4组计数量召唤

//限定词
#define QOI_GenCall	20		    //总召命令限定词
#define QRP_RstCall 1

//规约参数
#define ASDU_TYPE_YX	  0x01	//ASDU类型，01-YX帧
#define	ASDU_TYPE_YXBW	0x03	//ASDU类型，03-YXBW帧
#define	ASDU_TYPE_YC	  0x09//ASDU类型，09-YC帧



/*---报文应用类型编号---*/
/*--下面属于信息安全扩展区--*/
/*---time时间：6个字节---*/
/*---random随机数：8个字节---*/
/*---sign签名:65个字节---*/

#define Msg_None 0x00 //纯报文
#define Msg_S    0x01 //纯报文+签名
#define Msg_R    0x02 //纯报文+随机数
#define Msg_RS   0x03 //纯报文+随机数+签名
#define Msg_T    0x04 //纯报文+时间
#define Msg_TS   0x05 //纯报文+时间+签名
#define Msg_TR   0x06 //纯报文+时间+随机数
#define Msg_TRS  0x07 //纯报文+时间+随机数+签名
#define NMsg_TRS 0x08 //无报文+时间+随机数+签名
#define Reback   0x1f //业务安全处理结果返回
/*--------安全扩展应用类型--------*/
#define Wg_Req    0x20 //网关对终端的认证请求
#define Ter_Wg    0x21 //终端确认并请求网关认证
#define Wg_Ter    0x22 //网关对终端的认证请求的响应
#define Ter_Tw    0x23 //终端返回认证结果

#define Zz_Req    0x50 //主站对终端的认证请求
#define Ter_Zz    0x51 //终端确认并请求主站认证
#define Zz_Ter    0x52 //主站对终端的认证请求的响应
#define Ter_Tz    0x53 //终端返回认证结果
#define Zz_Rnm    0x54 //主站获取终端序列号
#define Ter_Rnm   0x55 //终端返回序列号

#define Zz_Rse    0x60 //主站获取终端密钥版本
#define Ter_Rse   0x61 //终端返回密钥版本
#define Zz_Upd    0x62 //主站执行远程密钥更新
#define Ter_Rup   0x63 //终端返回更新结果
#define Zz_Rre    0x64 //主站执行远程密钥恢复
#define Ter_Rre   0x65 //终端返回恢复结果

#define Zz_Rz_L   0x50 //现场与终端认证应用类型低边界
#define Zz_Rz_H   0x5f //现场与终端认证应用类型高边界

#define Zz_My_L   0x60 //主站与终端密钥管理应用类型低边界
#define Zz_My_H   0x6f //主站与终端密钥管理应用类型高边界

#define Zz_Zs_L   0x70 //现场与终端证书管理应用类型低边界
#define Zz_Zs_H   0x7f //现场与终端证书管理应用类型高边界

#define YC_Number  30  //定义最大遥测个数
#define YX_Number  48  //定义最大遥信个数

// 对后台私有协议结构体
struct BACKSTAGE_ENCRYPTION_TYPE1      				// 
{
	u8 DataBuffer[512];	// 数据接收缓冲
};

typedef union
{
	float floatdata;
	u8 bytedata[4];
	s32 longdata;
}FLOAT_BYTE;

struct YC_data
{
	FLOAT_BYTE yc_meas[YC_Number];
	u32 yc_state;
	u8  yc_num;
};
struct FIX_data
{
	FLOAT_BYTE fixpara[64];
};

struct RUN_data
{
	FLOAT_BYTE runpara[32];
};

extern u8 y_104;
extern u8 Index_1[8];
extern u8 Index_2[11];
extern u8 Index_3[10];
extern u8 Index_4[11];
extern u8 Index_5[13];
extern u8 Index_6[11];
extern u8 Index_7[15];
extern u8 Index_8[12];
extern u8 cpu_style[];
extern u8 frequence[];
extern u8 communcation_modle[];
extern char Constpara9[];

extern u8 zz_ok,wg_ok,yw_ok;	             //主站、网关、运维认证标志位
extern u8 In_Tag_Style[10];          //固有参数TAG类型
extern u8 In_Params_Len[10];			  //固有参数数据长度
extern u8 Wg_ca1[6];				//网关验证芯片读取码1
extern u8 Wg_ca2[6];				//网关验证芯片读取码2
extern u8 ID_Spi[8];
extern u8 Get_Ter_Random[6];
extern u8 Sec_ca1[6];
extern u8 Sec_ca2[6];	
extern u8 Recv_JM[4];
extern u8 Send_JM[4];
extern u8 ucaFlashBuf[1025];
extern u8 Super_Sign[15];
extern u8 Callend_Flag;
extern u8 index_which;    //用来表示当前请求的是哪个目录
extern u8 renzheng_flag;
extern u32 ulFlieLength;

void Send_Again(u8 USARTxChannel);	//重发
u8 SOE_Send_PD(void);
u8 getfloat_yc_state(void);  // 20%的变化
void Active_Upload(u8 USARTxChannel);	
void Active_Upload1(u8 USARTxChannel);	
u8 Ter_WriteRead_Spi1(void);
void Encryption_Handle(u8 receivedata,u8 USARTxnumber);
void CommandProcess_101_Encryption(u8 USARTxChannel);
void CommandProcess_Encryption_Prepare(u8 USARTxnumber);
#endif

