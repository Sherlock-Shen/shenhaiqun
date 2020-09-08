
#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "stm32f4xx.h"
#include "sysinit.h"
#include <stdio.h>
#include <stdlib.h>
#include <RTL.h>
//#define DebugUART1
//#define DebugUART5
#define Four_Faith_MODEL
//#define SIM800C_MODEL
//#define WD_MODEL

#define ID_Address        0x1FFF7A10 // 103-0x1ffff7e8 407
#define Bank1_SRAM1_ADDR ((u32)(0x60000000))
#define RARED_UN          100
#define CONTROL_ON_DELAY  50  //合闸控制延时
#define CONTROL_OFF_DELAY 50  //分闸控制延时

#define DOWN_U_MODULUS    2060
#define DOWN_I_MODULUS    16932
#define DOWN_I0_MODULUS   6654
#define DOWN_U0_MODULUS   6300
#define UP_U_MODULUS      14170
#define UP_I_MODULUS      16620
#define DOWN_I10_MODULUS  1330

/*程序版本*/
#define VERSION_YEAR	20 // 日期为硬件版本
#define VERSION_MONTH	9
#define VERSION_DATE	7
#define VERSION_TIME	8  // 软件修改次数

#define PROTOCOL_VERSION_YEAR	19		// 通讯协议版本号 
#define PROTOCOL_VERSION_MONTH	12
#define PROTOCOL_VERSION_DATE	5

/**************************存储空间定义***************************/
#define FLASH_KZQUPDATE_ADDR	      0x000000			// 控制器程序升级
#define FLASH_KZQUPDATE_LENGTH	    0x080000			// 控制器程序本地升级 数据长度 512k
#define FLASH_KZQBACKUP_ADDR	      0x080000			// 控制器程序备份恢复 //新扇区
#define FLASH_KZQBACKUP_LENGTH	    0x080000			// 控制器程序备份恢复 数据长度 512k

#define FLASH_EventRecord_ADDR			0x100000		  // 事件记录变量存储地址 //新扇区
#define FLASH_EventRecord_LENGTH		0x000080		  // 事件记录变量 数据长度最长128字节，实际传送100字节
#define FLASH_EventRecord_MAX			  500					  // 事件记录变量 最多512条，实际使用500条
//一次8条曲线 2400*8+100 = 19300字节放曲线信息，为了擦写方便，使用5个扇区20480个字节
#define FLASH_P1SCURVE_ADDR         0x110000      //三相和零序录波曲线    //新扇区
#define FLASH_P1SCURVE_LENGTH       0x005000
#define FLASH_P1SCURVE_MAX          100           //最多存放100条
//
#define FLASH_SOERecord_ADDR      	0x304000		  // SOE记录变量存储地址  //新扇区
#define FLASH_SOERecord_LENGTH	    12			      // SOE记录变量  数据长度
#define FLASH_SOERecord_MAX         300           // SOE记录变量  最大容量

#define FLASH_BATVOLTAGE_ADDR      	0x305000		  // 电池电压存储地址  //新扇区
#define FLASH_BATVOLTAGE_LENGTH	    3			        // 电池电压存储长度
#define FLASH_BATVOLTAGE_MAX        120           // 电池电压  最大容量
#define FLASH_BATTERY_RUNTIME_ADDR  0x305168      // 电池运行时间存储地址，两个字节

#define FLASH_101PARA_ADDR      	  0x306000		  // 101规约参数存储地址  //新扇区
#define FLASH_101PARA_LENGTH	      256			      // 101规约参数存储长度256字节
#define FLASH_101PARABAK_ADDR      	0x307000		  // 101规约参数备份存储地址  //新扇区
#define FLASH_101PARABAK_LENGTH	    256			      // 101规约参数备份存储长度256字节

#define FLASH_RESETYY_ADDR      		0x308000  	  // 复位原因

#define FLASH_SYSTEMSET_ADDR      	0x700000		  // 系统参数设置  //新扇区
#define FLASH_SYSTEMSET_LENGTH	    256			      // 系统参数设置存储长度256字节
#define FLASH_PROTECTSET_ADDR      	0x701000		  // 保护参数设置  //新扇区
#define FLASH_PROTECTSET_LENGTH	    256			      // 保护参数设置存储长度256字节
#define FLASH_ALARMSET_ADDR      	  0x702000		  // 报警参数设置  //新扇区
#define FLASH_ALARMSET_LENGTH	      256			      // 报警参数设置存储长度256字节
#define FLASH_AUTOFFSET_ADDR      	0x703000		  // 自动解列参数设置  //新扇区
#define FLASH_AUTOFFSET_LENGTH	    256			      // 自动解列设置存储长度256字节
#define FLASH_MEASURECF_ADDR      	0x704000		  // 测量系数     //新扇区         
#define FLASH_MEASURECF_LENGTH	    256			      // 测量系数存储长度256字节
#define FLASH_CORRECTCF_ADDR        0x705000		  // 校表系数     //新扇区
#define FLASH_CORRECTCF_LENGTH	    256			      // 校表系数存储长度256字节

#define FLASH_SYSTEMSETBAK_ADDR     0x600000		  // 系统参数设置备份  //新扇区
#define FLASH_SYSTEMSETBAK_LENGTH	  256			      // 系统参数设置备份存储长度256字节
#define FLASH_PROTECTSETBAK_ADDR    0x601000		  // 保护参数设置备份  //新扇区
#define FLASH_PROTECTSETBAK_LENGTH	256			      // 保护参数设置备份存储长度256字节
#define FLASH_ALARMSETBAK_ADDR      0x602000		  // 报警参数设置备份  //新扇区
#define FLASH_ALARMSETBAK_LENGTH	  256			      // 报警参数设置备份存储长度256字节
#define FLASH_AUTOFFSETBAK_ADDR     0x603000		  // 自动解列设置备份  //新扇区
#define FLASH_AUTOFFSETBAK_LENGTH	  256			      // 自动解列设置备份存储长度256字节
#define FLASH_MEASURECFBAK_ADDR     0x604000		  // 测量系数备份      //新扇区         
#define FLASH_MEASURECFBAK_LENGTH	  256			      // 测量系数备份存储长度256字节
#define FLASH_CORRECTCFBAK_ADDR     0x605000		  // 校表系数备份     //新扇区
#define FLASH_CORRECTCFBAK_LENGTH	  256			      // 校表系数存储备份长度256字节
#define FLASH_MANUNUNBER_ADDR       0x606000		  // 出厂编号，操作次数  //新扇区
#define FLASH_MANUNUNBER_LENGTH	    256			      // 出厂编号存储长度256字节
#define FLASH_ENERGYVL_ADDR      	  0x608000		  // 掉电保存电量，压板投退 //新扇区
#define FLASH_ENERGYVL_LENGTH	      256			      // 掉电保存电量存储长度256字节


#define FLASH_UPDATEFLAG_ADDR	      0x720000		  // 控制器升级标志    //新扇区
#define FLASH_UPDATEFLAG_LENGTH	    10			      // 控制器升级标志     数据长度 10Byte

/**************************KZQ发送命令定义***************************/
//KZQ_CMD.datatype
#define		KZQTYPE_INFORMATION	          0x00		// 实时信息
#define		KZQTYPE_RECORD_ON		          0x01		// 合闸记录
#define		KZQTYPE_RECORD_OFF		        0x02		// 分闸记录
#define		KZQTYPE_RECORD_PROTECT	      0x03	  // 保护记录
#define		KZQTYPE_PARA_PROTECT	        0x04		// 保护参数
#define		KZQTYPE_PARA_SYSTEM	          0x05		// 系统参数
#define		KZQTYPE_PARA_ALARM		        0x06	  // 报警参数
#define		KZQTYPE_RECORD_ALARM	        0x07		// 报警记录
#define   KZQTYPE_MEASURE_VALUE	        0x08		// 测量系数
#define		KZQTYPE_RECORD_GROUND	        0x09		// 接地记录
#define		KZQTYPE_PARA_AUTOSWITCH		    0x0A	  // 自动解列参数
#define   KZQTYPE_BATTERY_ON            0x0B    // 电池电压
#define   KZQTYPE_GPS_TIME              0x0C    // GPS时间

#define		KZQTYPE_SWITCHON_A	          0x10		// A相合闸曲线 
#define		KZQTYPE_SWITCHON_B	          0x11		// B相合闸曲线 
#define		KZQKTYPE_SWITCHON_C	          0x12		// C相合闸曲线 
#define		KZQTYPE_SWITCHOFF_A	          0x13 		// A相分闸曲线 
#define		KZQTYPE_SWITCHOFF_B	          0x14 		// B相分闸曲线 
#define		KZQTYPE_SWITCHOFF_C	          0x15 		// C相分闸曲线 
#define		KZQTYPE_CURVE_PROTECT_A  	    0x16	  // A相保护曲线
#define		KZQTYPE_CURVE_PROTECT_B	      0x17	  // B相保护曲线
#define		KZQTYPE_CURVE_PROTECT_C	      0x18	  // C相保护曲线
#define		KZQTYPE_CURVE_PROTECT_O	      0x1B	  // 零序接地曲线

/**************************断路器事件记录类型***************************/
// 操作类型定义
#define   TYPE_ACT_PAD_OFF                0x11    //遥控分闸
#define   TYPE_ACT_PAD_ON                 0x12    //遥控合闸
#define   TYPE_ACT_FAR_OFF                0x13    //远方分闸
#define   TYPE_ACT_FAR_ON                 0x14    //远方合闸
#define   TYPE_ACT_MAN_OFF                0x15    //手动分闸
#define   TYPE_ACT_MAN_ON                 0x16    //手动合闸
#define   TYPE_ACT_REON                   0x17    //保护重合闸
#define   TYPE_ACT_REVERT                 0x18    //信号复归

// 保护类型定义
#define   TYPE_PROTECT_REON               0x21    // 重合闸
#define   TYPE_PROTECT_SECOND_REON        0x22    // 二次重合闸
#define   TYPE_PROTECT_FAST_OFF           0x23    // 速断保护
#define   TYPE_PROTECT_DELAY_FAST_OFF     0x24    // 延时速断保护
#define   TYPE_PROTECT_MAX_CURRENT        0x25    // 过流保护
#define   TYPE_PROTECT_MAX_CURRENT_AFTER  0x26    // 过流后加速保护
#define   TYPE_PROTECT_GND                0x27    // 接地保护
#define   TYPE_PROTECT_PHASE_LOSS         0x28    // 缺相保护
#define   TYPE_PROTECT_ZERO_MAX_VOLTAGE		0x29	  // 零序过压保护
#define   TYPE_PROTECT_ZERO_MAX_CURRENT		0x2A	  // 零序过流保护
#define   TYPE_PROTECT_ZERO_FAST_OFF		  0x2B	  // 零序速断保护
#define   TYPE_PROTECT_MAX_VOLTAGE        0x2C    // 过压保护
#define   TYPE_PROTECT_MAX_FREQ           0x2D    // 高频保护
#define   TYPE_PROTECT_LOW_FREQ           0x2E    // 低频保护

//馈线自动化保护定义
#define   TYPE_PROTECT_LOSS_POWER         0x31    // 失电分闸
#define   TYPE_PROTECT_GET_POWER          0x32    // 得电合闸
#define   TYPE_PROTECT_SINGLE_LOSS        0x33    // 失电合闸
#define   TYPE_PROTECT_ERR_FASTOFF        0x34    // 合到故障快速分闸
#define   TYPE_PROTECT_SWITCHOFF_LOCK     0x35    // 分闸闭锁
#define   TYPE_PROTECT_SWITCHON_LOCK      0x36    // 合闸闭锁
#define   TYPE_PROTECT_ZERO_VOLTAGE1      0x37    // 零序电压保护
#define   TYPE_PROTECT_ZERO_VOLTAGE2      0x38    // 零序电压报警
#define   TYPE_PROTECT_PTBREAK_ALARM      0x39    // PT断线告警

// 报警类型定义
#define   TYPE_ALARM_MAX_VOLTAGE          0x41    //过压报警
#define   TYPE_ALARM_MIN_VOLTAGE          0x42    //欠压报警
#define   TYPE_ALARM_MAX_HAR              0x43    //过谐波报警
#define   TYPE_ALARM_VOL_QUA_LOW          0x44    //电压合格率低报警
#define   TYPE_ALARM_MAX_LOAD             0x45    //过负荷报警
#define   TYPE_ALARM_ZERO_MAX_VOLTAGE     0x46    //零序过压报警
#define   TYPE_ALARM_ZERO_MAX_CURRENT     0x47    //零序过流报警
#define   TYPE_ALARM_BAT_LOW              0x48    //电池电压低报警
#define   TYPE_ALARM_GND                  0x49    //接地报警

//KZQ_CMD.flag
#define		KZQFLAG_CMD			        0x0001		// 控制命令
#define		KZQFLAG_SYSTEM_SET		  0x0002		// 系统参数设置
#define		KZQFLAG_PROTECT_SET	    0x0004		// 保护参数设置
#define		KZQFLAG_ALARM_SET		    0x0008		// 报警参数设置
#define		KZQFLAG_MACHINE_SET	    0x0010		// 出厂信息设置
#define		KZQFLAG_INFORMATION	    0x0020		// 实时信息
#define		KZQFLAG_GPS_time        0x0040		// GPS时间
#define		KZQFLAG_HOSTPC_time     0x0080		// 后台校时
#define		KZQFLAG_AUTOSWITCH_SET	0x0100		// 自动解列参数设置

//KZQ_CMD.action
#define		KZQCMD_BREAKER_ON		    0x01		// 合闸           
#define		KZQCMD_BREAKER_OFF		  0x02		// 分闸
#define   KZQCMD_BATTERY_ON       0x03    //电池继电器合
#define   KZQCMD_BATTERY_OFF      0x04    //电池继电器分
#define		KZQCMD_REVERT			      0x05		// 复归
#define		KZQCMD_CLEAR			      0x06		// 清空电量信息
#define		KZQCMD_RESET			      0x07		// 复位
#define		KZQCMD_FORCE_ON		      0x08		// 强制合闸
#define		KZQCMD_FORCE_OFF	      0x09		// 强制分闸

#define		KZQMODE_MODE1			      0x01		// 模式1 本地 mode
#define		KZQMODE_MODE2			      0x02		// 模式2 远方

/**************************SOE遥信地址***************************/
#define   YX_SHAKE_TIME			                   8	// 遥信去抖8ms

#define   SOE_ADDR_FAR                         Measure101Para.para.far_positoin            // 远方遥信地址
#define   SOE_ADDR_GND                         Measure101Para.para.local_positoin          // 就地遥信地址
#define		SOE_ADDR_POWER_LOST                  Measure101Para.para.AC_loss                 // 交流失电告警
#define		SOE_ADDR_ALARM_BAT_LOW_VOLTAGE       Measure101Para.para.Bat_low                 // 电池低电压报警动作事件
#define   SOE_ADDR_SWITCHON_LOCK               Measure101Para.para.lock_on                 // 开关闭锁合闸
#define   SOE_ADDR_SWITCHOFF_LOCK              Measure101Para.para.lock_off                // 开关闭锁分闸
#define   SOE_ADDR_IN2                         Measure101Para.para.break_on                // 合位遥信地址
#define   SOE_ADDR_IN1                         Measure101Para.para.break_off               // 分位遥信地址
#define   SOE_ADDR_IN3		                     Measure101Para.para.wcn_state	             // 弹簧未储能
#define   SOE_ADDR_ALARM_MAX_LOAD		           Measure101Para.para.max_load_alarm          // 过负荷报警动作事件
#define   SOE_ADDR_ALARM_GROUNDI0   		       Measure101Para.para.zero_current1_alarm	   // 零序电流报警动作事件
#define   SOE_ADDR_ALARM_GROUND		             Measure101Para.para.ground_current_alarm	   // 接地报警动作事件
#define   SOE_ADDR_FAST_OFF				             Measure101Para.para.max_current1_protec	   // 速断保护动作事件
#define   SOE_ADDR_DELAY_FAST_OFF		  	       Measure101Para.para.max_current2_protec	   // 延时速断保护动作事件
#define   SOE_ADDR_ZERO_MAX_CURRENT		         Measure101Para.para.zero_current1_protec	   // 零序过流保护动作事件
#define   SOE_ADDR_REON					               Measure101Para.para.reon               	   // 重合闸动作事件
#define   SOE_ADDR_MAX_CURRENT_AFTER		       Measure101Para.para.max_currentafter_protec // 过流后加速保护动作事件
#define   SOE_ADDR_MAX_VOLTAGE                 Measure101Para.para.max_voltage             // 过压保护
#define   SOE_ADDR_MAX_FREQ                    Measure101Para.para.high_freq               // 高频保护
#define   SOE_ADDR_LOW_FREQ                    Measure101Para.para.low_freq                // 低频保护
#define   SOE_ADDR_EVENT_ALL                   Measure101Para.para.event_all               // 事故总
#define   SOE_ADDR_MAX_CURRENT		             Measure101Para.para.max_current3_protec	   // 过流保护动作事件
#define		SOE_ADDR_SELF_ERROR                  Measure101Para.para.break_err               // 装置异常告警
#define		SOE_ADDR_SWITCH_CONTACT_1            Measure101Para.para.segment_mode            // 开关分段点工作模式
#define		SOE_ADDR_SWITCH_CONTACT_2            Measure101Para.para.contact_mode            // 开关联络点工作模式
#define		SOE_ADDR_SWITCH_POWERLOSS_1          Measure101Para.para.novoltage_A             // 开关电源侧无压
#define		SOE_ADDR_SWITCH_POWERLOSS_2          Measure101Para.para.novoltage_B             // 开关负荷侧无压
#define   SOE_ADDR_POWERLOSS_OFF               Measure101Para.para.powerloss_off           // 开关失压分闸
#define		SOE_ADDR_ALARM_BAT_ACTIVE            Measure101Para.para.bat_active              // 电池活化

#define   SOE_ADDR_IN1_pad		                     5	// 分位遥信地址
#define   SOE_ADDR_IN2_pad		                     6	// 合位遥信地址
#define   SOE_ADDR_IN3_pad		                     7	// 未储能遥信地址
#define   SOE_ADDR_IN4_pad		                     8	// 备用遥信地址
#define   SOE_ADDR_GND_pad		                     9	// 就地遥信地址
#define		SOE_ADDR_BHYB1_pad                       10 // 总保护压板地址
#define		SOE_ADDR_BHYB2_pad                       11 // 接地保护压板地址
#define		SOE_ADDR_BHYB3_pad                       12 // 相间保护压板地址
#define		SOE_ADDR_FAR_pad                         13 // 远方遥信地址
#define		SOE_ADDR_SIGRESET_pad                    14 // 信号复归地址

#define   SOE_ADDR_ZERO_MAX_VOLTAGE_pad		         15	// 零序过压保护动作事件
#define   SOE_ADDR_ZERO_MAX_CURRENT_pad		         16	// 零序过流保护动作事件
#define   SOE_ADDR_PHASE_LOSS_pad				           17	// 缺相保护动作事件
#define   SOE_ADDR_GROUND_pad				               18	// 接地保护动作事件
#define   SOE_ADDR_FAST_OFF_pad				             19	// 速断保护动作事件
#define   SOE_ADDR_DELAY_FAST_OFF_pad		  	       20	// 延时速断保护动作事件
#define   SOE_ADDR_MAX_CURRENT_pad			           21	// 过流保护动作事件
#define   SOE_ADDR_MAX_CURRENT_AFTER_pad		       22	// 过流后加速保护动作事件
#define   SOE_ADDR_REON_pad					               23	// 重合闸动作事件

#define   SOE_ADDR_ALARM_MAX_LOAD_pad		           24	// 过负荷报警动作事件
#define   SOE_ADDR_ALARM_MAX_VOLTAGE_pad		       25	// 过电压报警动作事件
#define   SOE_ADDR_ALARM_MIN_VOLTAGE_pad		       26	// 欠压报警动作事件
#define   SOE_ADDR_ALARM_MAX_HARMONIC_pad		       27	// 过谐波报警动作事件
#define   SOE_ADDR_ALARM_VOLTAGE_QUALIFRATE_pad    28	// 电压合格率报警动作事件
#define   SOE_ADDR_ALARM_BAT_LOW_VOLTAGE_pad	     29	// 电池低电压报警动作事件
#define   SOE_ADDR_ALARM_GROUND_pad   		         30	// 接地报警动作事件
#define   SOE_ADDR_ALARM_GROUNDU0_pad   		       31	// 零序电压报警动作事件
#define   SOE_ADDR_ALARM_GROUNDI0_pad   		       32	// 零序电流报警动作事件
#define   SOE_ADDR_MAX_VOLTAGE_pad                 33 // 过压保护
#define   SOE_ADDR_MAX_FREQ_pad                    34 // 高频保护
#define   SOE_ADDR_LOW_FREQ_pad                    35 // 低频保护

#define   SOE_ADDR_ACT_PAD_OFF_pad                 51 //本地分闸
#define   SOE_ADDR_ACT_PAD_ON_pad                  52 //本地合闸
#define   SOE_ADDR_ACT_FAR_OFF_pad                 53 //远方分闸
#define   SOE_ADDR_ACT_FAR_ON_pad                  54 //远方合闸
#define   SOE_ADDR_ACT_MAN_OFF_pad                 55 //手动分闸
#define   SOE_ADDR_ACT_MAN_ON_pad                  56 //手动合闸

#define   SOE_ADDR_PT_BREAK_pad                    60 // PT断线
#define   SOE_ADDR_LOSS_POWER_pad                  61 // 失电延时分闸
#define   SOE_ADDR_GETPOWER_A_pad                  62 // A侧得电延时合闸
#define   SOE_ADDR_GETPOWER_B_pad                  63 // B侧得电延时合闸
#define   SOE_ADDR_LOSSPOWER_A_pad                 64 // A侧失电延时合闸
#define   SOE_ADDR_LOSSPOWER_B_pad                 65 // B侧失电延时合闸
#define   SOE_ADDR_DOUBLEPOWER_LOCKON_pad          66 // 双侧有压闭锁合闸
#define   SOE_ADDR_VOLTAGE_TIME_pad                67 // 电压时间型失电分闸
#define   SOE_ADDR_VOLTAGE_CURRENT1_pad            68 // 电压电流型失电分闸无闭锁
#define   SOE_ADDR_VOLTAGE_CURRENT2_pad            69 // 电压电流型失电分闸有闭锁
#define   SOE_ADDR_SWITCHON_LOCKOFF_pad            70 // 合闸成功闭锁分闸
#define   SOE_ADDR_SIGRESET_LOCK_pad               71 // 闭锁复归
#define   SOE_ADDR_NOBREAK_CURRENT_pad             72 // 非遮断电流保护
#define   SOE_ADDR_RESVOLTAGE_LOCK_pad             73 // 残压闭锁
#define   SOE_ADDR_INRUSH_pad                      74 // 涌流
#define   SOE_ADDR_PHASE_ERR_pad                   75 // 相间故障
#define   SOE_ADDR_GROUND_ERR_pad                  76 // 接地故障
#define   SOE_ADDR_ZERO_VOLTAGE1_pad               77 // 零序电压报警
#define   SOE_ADDR_ZERO_VOLTAGE2_pad               78 // 零序电压保护

//线损Flash位置定义 事件定义
#define Flash_LineLoss_Set                0      //0
#define Flash_LineLoss_Lock               0x1000 //1  //线损模块冻结次数和位置信息
#define Flash_LineLoss_Event							0x2000 //2  //线损模块事件次数和位置信息
#define Flash_LineLoss_Fix                0x3000 //3- 54*60
#define Fix_Num                           54//存几处？
#define Flash_LineLoss_Rand								0x4000 //3- 54*3
#define Rand_Num                          54//存几处？
#define Flash_LineLoss_Frzd               0x5000 //62- 54*62
#define Frzd_Num                          54//存几处？
#define Flash_LineLoss_Sharp              0x6000//264 -14*264
#define Sharp_Num                         14//存几处？
#define Flash_LineLoss_Month              0x7000//12-38*12
#define Month_Num                         38//存几处？
#define Flash_LineLoss_Event_PAReverse    0x8000// 4+8*4+8+8*4  10处 恢复时记录后面2个 72
#define PAReverse_EndNum                  84//存几处？+8
#define PAReverse_ActNum                  44//存几处？
#define Flash_LineLoss_Event_PBReverse    0x9000// 8*4+8+8*4  10处 恢复时记录后面2个 72
#define PBReverse_EndNum                  84//存几处？
#define PBReverse_ActNum                  44//存几处？
#define Flash_LineLoss_Event_PCReverse    0xA000// 8*4+8+8*4  10处 恢复时记录后面2个 72
#define PCReverse_EndNum                  84//存几处？
#define PCReverse_ActNum                  44//存几处？
#define Flash_LineLoss_Event_PReverse     0xB000// 8*4  10处 恢复时记录后面2个 72
#define PReverse_Num                      44//存几处？
#define Flash_LineLoss_Event_ClearLock    0xC000// 8*4  10处 恢复时记录后面2个 72
#define ClearLock_Num                     44//存几处？
#define Flash_LineLoss_Event_ClearEvent   0xD000//不记录数据 只记录总次数 不占用空间 4*10
#define ClearEvent_Num                    12
#define Flash_LineLoss_Event_CheckTime    0xE000//不记录数据 只记录总次数 不占用空间
#define CheckTime_Num                     12
#define Flash_LineLoss_Event_DocTime      0xF000//不记录数据 只记录总次数 不占用空间  //文件最后一次修改时间
#define Doc_Num                           43 //6个文件+校验和

// /**************************READ_101Flag***************************/
#define  system_data        0x01  
#define  protect_data       0x02  
#define  alarm_data         0x04 
#define  autoswitch_data    0x08  
//线损模块 设置
struct Lineloss
{
	float LockDay;
	float LockHour;
	float P_Reverse_Limit;
	float P_Reverse_Time;
	float Pt_Reverse_Limit;
	float Pt_Reverse_Time;
};

union Lineloss1
{
	struct Lineloss para;
	u8 two_array[6][sizeof(float)];
};

struct REVER_BITS {     			// bits  
  u8  PA_Reverse:1;  		// 0  	单位遥信
	u8  PB_Reverse:1;  		// 1  	单位遥信
	u8  PC_Reverse:1;  		// 2  	单位遥信
	u8  P_Reverse:1;  		// 3  	单位遥信
  u8  D4:1;  		// 4  	单位遥信
	u8  D5:1;  		// 5  	单位遥信
	u8  D6:1;  		// 6  	单位遥信
	u8  D7:1;  		// 7  	单位遥信
};

union REVERDefine					// 遥信定义
{
	struct REVER_BITS bit;
	u8 byte;
};
struct LineLossEvent
{
	u32 PA_Reverse_Time;
	u32 PB_Reverse_Time;
	u32 PC_Reverse_Time;
	u32 P_Reverse_Time;
	u32 LineLoss_Zero_Time;
	u32 Event_Zero_Time;
	u32 Check_Time;//对时
	u8  PA_Place;
	u8  PB_Place;
	u8  PC_Place;
	u8  P_Place;
	u8  LineLoss_Place;
	u8  Event_Place;
	u8  Check_Place;
	u32 PA_Reverse_Delay;
	u32 PB_Reverse_Delay;
	u32 PC_Reverse_Delay;
	u32 P_Reverse_Delay1;
	u32 P_Reverse_Delay2;
};	
struct LineLockEvent
{
	u16 Fix_Time;
	u16 Rand_Time;
	u16 Frzd_Time;
	u16 Sharp_Time;
	u16 Month_Time;//存储次数
//	u32 Event_Time;//   事件分7种单独存储
	u16  Fix_Place;//60   使用冻结时判断吗？
	u16  Rand_Place;//3    命令冻结
	u16  Frzd_Place;//62   0点冻结
	u16  Sharp_Place;//264 整点冻结
	u16  Month_Place;//12  月冻结 冻结日+冻结时
	u16  Read_Place;//临时变量
	u16  Read_CS;   //读取次数
	u32  Read_Text; //读取位置
	u32  Write_Text; //写入位置
	u32  Next_Text;//下次写入首地址
	u32  Last_Text;//上次地址
	u8   Write_EndBz;//结束标志
	u8   Read_EndBz;//结束标志
	u8   Read_Num;//结束读取个数 //事件里借用为 文件报文长度
	u8 	 Event_Time;//总次数
	u8   Event_Bz;//1-7
	u8   Event_CS;//第几次读取
	u8   Doc_Bz;//文件标志
};	
extern struct LineLockEvent LineLockNum;
extern struct LineLossEvent LineLossNum;
extern union Lineloss1 LineSet;
extern union REVERDefine PReverse;

//----------------------------------------曲线结构---------------------------------------//
struct Curve
{
	s16 voltage[400];		// 电压曲线
	s16 current[400];		// 电流曲线
	s16 locus[400];		// 轨迹曲线
	s16 position[50];
};

union CurveDat
{
	s16 byte[1250];
	struct Curve word; 
};

struct ActionMode
{
	union CurveDat On;				// 合闸
	union CurveDat Off;				// 分闸
	union CurveDat Protect;			// 保护分闸
};

struct PhaseMode
{
	struct ActionMode CurveA;
	struct ActionMode CurveB;
	struct ActionMode CurveC;
};
extern struct PhaseMode MyCurve;




//struct Curve1
//{
//	s16 voltage[800];		// 电压曲线
//	s16 current[800];		// 电流曲线
//	s16 locus[800];		// 轨迹曲线
//};

//union CurveDat1
//{
//	s16 byte[2400];
//	struct Curve1 word; 
//};

//struct ActionMode1
//{
//	union CurveDat1 On;				// 合闸
//	union CurveDat1 Off;				// 分闸
//	union CurveDat1 Protect;			// 保护分闸
//};

//struct PhaseMode1
//{
//	struct ActionMode1 CurveA;
//	struct ActionMode1 CurveB;
//	struct ActionMode1 CurveC;
//};
//extern struct PhaseMode1 MyCurve1;


struct Modulus
{
	u16 UAB;
	u16 UBC;
	u16 UCA;
	u16 UA;
	u16 UB;
	u16 UC;
	u16 U0;
	u16 IA;
	u16 IB;
	u16 IC;
	u16 IA10;
	u16 IB10;
	u16 IC10;
	u16 I0;
	u16 UA1;
	u16 I010;
	u16 UC1;
};

union Modulus1
{
	struct Modulus para;
	u16 word[sizeof(struct Modulus)/2];
};

struct Rule101Para
{
	u8 balance;              
	u8 addr_len;
  u8 reason_len;
	u8 data_type;
	u8 encrypt;
	u8 Remote_Ctrl;       //开关遥控
	u8 Remote_Batact;     //电池活化
	u8 Remote_Reset;      //装置远方复位
	u8 res5;
	u8 res6;                         //10
	u8 AC_power;
	u8 Bat_volt;
	u8 UAB;
	u8 UCB;
	u8 U0;
	u8 Sig_dBm;
	u8 freq;
	u8 res7;
	u8 res8;
	u8 IA;                           //20
	u8 IB;
	u8 IC;
	u8 I0;
	u8 P;
	u8 Q;
	u8 cos;
	u8 res9;
	u8 res10;
	u8 res11;
	u8 far_positoin;                  //30
	u8 local_positoin;
	u8 AC_loss;
	u8 Bat_low;
	u8 low_airlock;
	u8 lock_on;
	u8 lock_off;
	u8 break_on;
	u8 break_off;
	u8 wcn_state;
	u8 max_current1_alarm;            //40
	u8 max_current2_alarm;
	u8 max_load_alarm;
	u8 zero_current1_alarm;
	u8 ground_current_alarm;
	u8 max_current1_protec;
	u8 max_current2_protec;
	u8 zero_current1_protec;
	u8 zero_currentafter_protec;
	u8 reon;
	u8 max_currentafter_protec;        //50
	u8 max_voltage;
	u8 high_freq;
	u8 low_freq;
	u8 event_all;
	u8 max_current3_protec;
	u8 break_err;
	u8 segment_mode;
	u8 contact_mode;
	u8 novoltage_A;
	u8 novoltage_B;                    //60
	u8 powerloss_off;                          
	u8 bat_active;                //电池活化           
};

union Rule101Para1
{
	struct Rule101Para para;
	u8 word[sizeof(struct Rule101Para)];
};

struct Measure		// 测量值
{
	u8 Pstartflag;	 // 保护曲线开始采样标志
	u8 Psaveflag;    // 保护曲线开始保存标志
	u8 Psaveflash;    // 保护曲线保存到falsh
	
	s16 UAB_val;		 // AB相电压有效值
	s16 UBC_val;		 // BC相电压有效值
	s16 UCA_val;		 // CA相电压有效值

	s16 UA_val;			 // A相电压有效值
	s16 UB_val;			 // B相电压有效值
	s16 UC_val;			 // C相电压有效值
	
	s16 UAB1_val;		 // AB相电压有效值
	s16 UBC1_val;		 // BC相电压有效值
	s16 UCA1_val;		 // CA相电压有效值

	s16 UA1_val;		 // A相电压有效值
	s16 UB1_val;		 // B相电压有效值
	s16 UC1_val;		 // C相电压有效值

	s16 IA_val;	     // A相电流有效值
	s16 IB_val;		   // B相电流有效值
	s16 IC_val;	     // C相电流有效值

	u16 Display_UAB_val;		 // AB相显示电压有效值
	u16 Display_UBC_val;		 // BC相显示电压有效值
	u16 Display_UCA_val;		 // CA相显示电压有效值
	
	u16 Display_degUAB_val; // AB相显示电压角度	

	u16 Display_UA_val;		 // A相显示电压有效值
	u16 Display_UB_val;		 // B相显示电压有效值
	u16 Display_UC_val;		 // C相显示电压有效值

	u16 Display_degUA_val;	 // A相显示电压角度
	u16 Display_degUB_val;	 // B相显示电压角度
	u16 Display_degUC_val;	 // C相显示电压角度

//	u16 Display_UAB1_val;	// AB相显示电压有效值
//	u16 Display_UBC1_val;	// BC相显示电压有效值
//	u16 Display_UCA1_val;	// CA相显示电压有效值	

	u16 Display_UA1_val;		 // A相显示电压有效值
	u16 Display_UB1_val;		 // B相显示电压有效值
	u16 Display_UC1_val;		 // C相显示电压有效值

	u16 Display_U0_val;		 // 零序电压显示有效值
  u16 Display_degU0_val;  // 零序电压角度显示

	u16 Display_IA_val;		 // A相显示电流有效值
	u16 Display_IB_val;		 // B相显示电流有效值
	u16 Display_IC_val;		 // C相显示电流有效值

  u16 Display_degIA_val;  // A相显示电流角度
	u16 Display_degIB_val;	 // B相显示电流角度
 	u16 Display_degIC_val;	 // C相显示电流角度

	u16 Display_I0_val;		 // 零序电流显示有效值
	u16 Display_degI0_val;	 // 零序电流显示角度

	u16 Display_IA10_val;	 // A相10倍显示电流有效值
	u16 Display_IB10_val;	 // B相10倍显示电流有效值
	u16 Display_IC10_val;	 // C相10倍显示电流有效值

	u16 Protect_UAB_val;		 // AB相保护电压有效值
	u16 Protect_UBC_val;		 // BC相保护电压有效值
	u16 Protect_UCA_val;		 // CA相保护电压有效值	

	u16 Protect_UA_val;		 // A相保护电压有效值
	u16 Protect_UB_val;		 // B相保护电压有效值
	u16 Protect_UC_val;		 // C相保护电压有效值

//	u16 Protect_UAB1_val;	// AB相保护电压有效值
//	u16 Protect_UBC1_val;	// BC相保护电压有效值
//	u16 Protect_UCA1_val;	// CA相保护电压有效值	

	u16 Protect_UA1_val;		  // A相保护电压有效值
	u16 Protect_UB1_val;		  // B相保护电压有效值
	u16 Protect_UC1_val;		  // C相保护电压有效值

	u16 Protect_U0_val;		  // 零序电压保护有效值

	u16 Protect_IA_val;		  // A相保护电流有效值
	u16 Protect_IB_val;		  // B相保护电流有效值
	u16 Protect_IC_val;		  // C相保护电流有效值

	u16 Protect_I0_val;		  // 零序电流保护有效值
	u16 Protect_10I0_val;		// 零序电流10倍保护有效值

	u16 Protect_IA10_val;	  // A相10倍保护电流有效值
	u16 Protect_IB10_val;	  // B相10倍保护电流有效值
	u16 Protect_IC10_val;	  // C相10倍保护电流有效值

	u16 Protect_IASD_val;		// A相保护电流有效值
	u16 Protect_IBSD_val;		// B相保护电流有效值
	u16 Protect_ICSD_val;		// C相保护电流有效值

	u16 Protect_IA10SD_val;	// A相10倍保护电流有效值
	u16 Protect_IB10SD_val;	// B相10倍保护电流有效值
	u16 Protect_IC10SD_val;	// C相10倍保护电流有效值

	float PA_val;		// A相有功功率 
	float PB_val;		// B相有功功率
	float PC_val;		// C相有功功率

	s16 QA_val;		// A相无功功率
	s16 QB_val;		// B相无功功率
	s16 QC_val;		// C相无功功率

	s16 cosA;		  // A相功率因数
	s16 cosB;		  // B相功率因数
	s16 cosC;		  // C相功率因数

	float P_val;		// 三相有功功率
	s16 Q_val;		// 三相无功功率
	s16 cosALL;		// 总功率因数
	
	u16 freq;	// 电度表频率
  u16 freq_cpu1;	// cpu计算频率1
	u16 freq_cpu2;	// cpu计算频率2
	
	s16 Wp;	          // 当日有功电量
	s16 Wq;	          // 当日无功电量
	s16 cos_W;		    // 当日电量功率因数

	float Wp_all;	      // 总正向有功电量
	float WpFX_all;	    // 总反向有功电量
	float Wp_peak;	    // 峰正向有功电量
	float WpFX_peak;	  // 峰反向有功电量
	float Wp_valley;	  // 谷正向有功电量
	float WpFX_valley;	// 谷反向有功电量
	float Wp_level;	    // 平正向有功电量
	float WpFX_level;	  // 平反向有功电量

	float Wq_1all;	    // I  象限总无功电量
	float Wq_2all;	    // II 象限总无功电量
	float Wq_3all;	    // III象限总无功电量
	float Wq_4all;	    // IV 象限总无功电量

	u16 XB_U[9];	    // 电压谐波
	u16 XB_I[9];	    // 电流谐波
	
	u16 DIS_XB_U[9];	// 电压谐波
	u16 DIS_XB_I[9];	// 电流谐波

	u16 datatype;		// 数据类型	
	u8 YaBan1;		  // 压板1
	u8 YaBan2;		  // 压板2
	u8 YaBan3;		  // 压板3
	u32 UABsum;
	u32 UBCsum;
	u32 UCAsum;
	u32 UAsum;
	u32 UBsum;
	u32 UCsum;
	u32 UA1sum;
	u32 UB1sum;
	u32 UC1sum;
	u32 U0sum;
	u32 IAsum;
	u32 IBsum;
	u32 ICsum;
	u32 I0sum;
	u32 PIAsum;
	u32 PIBsum;
	u32 PICsum;
	u32 PI0sum;
	u32 IASDsum;
	u32 IBSDsum;
	u32 ICSDsum;
	u32 PIASDsum;
	u32 PIBSDsum;
	u32 PICSDsum;

	u16 Att7022Flag;

	u32 UAB_temp;
	u32 UBC_temp;
	u32 UCA_temp;
	u32 UA_temp;
	u32 UB_temp;
	u32 UC_temp;
	u32 U0_temp;
	u32 UA1_temp;
	u32 UB1_temp;
	u32 UC1_temp;
	u32 IA_temp;
	u32 IB_temp;
	u32 IC_temp;
	u32 PIA_temp;
	u32 PIB_temp;
	u32 PIC_temp;
	u32 I0_10_temp;
	u32 I0_temp;
	u16 V_BAT;
	
	s16 SA_val;		// A相有功功率 
	s16 SB_val;		// B相有功功率
	s16 SC_val;		// C相有功功率
	s16 S_val;
};

struct Measure1		// 测量曲线值
{
	s16 IAX[100];
  s16 IBX[100];
  s16 ICX[100];
	s16 U0X[100];
	s16 I0X[100];
	s16 UAX[100];
	s16 UBX[100];
	s16 UCX[100];
	s16 UABX[100];
	s16 UBCX[100];	
	s16 UCAX[100];
  s16 UPT1X[100];	
	s16 UPT2X[100];

	s16 XB_IA[64];	// A相电流谐波
	s16 XB_IB[64];
	s16 XB_IC[64];
	s16 XB_UA[64];
	s16 XB_UB[64];
	s16 XB_UC[64];
	s16 XB_U0[64];
	s16 XB_I0[64];
	s16 XB_UAB[64];	// A相电压谐波
	s16 XB_UBC[64];
	s16 XB_UCA[64];
  s16 XB_UPT1[64];	
	s16 XB_UPT2[64];
	u32 V_BAT_SUM;

};

/**********************
*断路器诊断信息 4-1
长度  =  34
**********************/
struct DLQZD
{
	u16 open_distance[3];
	u16 overrun[3];
	u16 time_on[3];
	u16 speed_on[3];
	u16 meanwhile_on[3];
	u16 shake_on[3];
	u16 current_on;
	u16 time_off[3];
	u16 speed_off[3];
	u16 meanwhile_off[3];
	u16 burn_off[3];
	u16 current_off;
	u16 store_time;
	u16 store_current;
};
union DLQZDXX
{
	struct DLQZD para;
	u16 word[sizeof(struct DLQZD)/2];
};

struct time_now			// 系统时间
{
	u16 year;
	u16 month;
	u16 date;
	u16 hour;
	u16 minute;
	u16 second;
	u16 msec;
};

// 升级包数据版本
struct UpdateVersion
{
	u8  KZQ_version[4];			  // KZQ程序版本
	u8  IAP_version[4];			  // KZQ启动程序版本
	u8  GPRS_version[4];			// GPRS程序版本
	u32 KZQ_length_update;		// KZQ本地升级数据包长度
	u8  KZQ_time_update[6];	  // KZQ本地升级数据包修改时间
	u32 KZQ_length_backup;		// KZQ备份程序数据包长度
	u8  KZQ_time_backup[6];	  // KZQ备份程序数据包修改时间
	u16 Product_number;	      // 生产批号
	u16 Product_flow;	        // 生产流水
	u8  Product_date[3];	    // 生产日期
	u16 GPRS_length_update;	  // GPRS升级数据包长度
};

/***********************设置值*********************************/
// 系统参数设置
struct SYSTEM_PARA      				          // 系统参数
{
	u16  PT_Hvalue;					      // 0   PT变比一次侧
	u16  PT_Lvalue;					      // 1   PT变比二次侧
	u16  CT_Hvalue;			          // 2   CT变比一次侧
	u16  CT_Lvalue;			          // 3   CT变比二次侧
	u16  exchange_dycfzc;         // 4
	u16  wifi_gps_switch;         // 5
	u16  peak_valley_period[6];	  // 6   时段设置
	u16  password;				        // 12  密码
	u16  groundtype;	            // 13  中心点接地方式
	u16  com_protocol;				    // 14  规约号，88为test    
	u16  baud_rate;				        // 15  波特率
	u16  battery_delay;		        // 16  电池供电时长
	u16  telnumber1[3];           // 17  手机号码1
  u16  telnumber2[3];           // 20  手机号码2
	u16  address;					        // 23  本机地址
	u16  port1;						        // 24  目的端口号
	u16  port2;						        // 25  备用端口号
	u16  IP1[2];						      // 26	 目的IP地址
	u16  IP2[2];						      // 28  备用IP地址
	u16  control_time;            // 30  遥控保持时间
	u16  start_year;					    // 31  日期时间设置
	u16  start_month;             // 32
	u16  start_date;              // 33
	u16  start_hour;					    // 34
	u16  start_min;               // 35
	u16  start_sec;               // 36
};
union SYSTEM_PARA1
{
	struct SYSTEM_PARA para;
	u16    word[sizeof(struct SYSTEM_PARA)/2];
};

struct PROTECT_PARA       				      // 保护参数设置
{
  u16  max_voltage;  					          // 0   	过压
	u16  max_voltage_time;  				      // 1   	过压时间
	u16  max_voltage_enable;  			      // 2   	过压使能
  u16  zero_max_voltage;  				      // 3   	零序过压
	u16  zero_max_voltage_time;  		      // 4   	零序过压时间
	u16  zero_max_voltage_enable;  		    // 5   	零序过压使能
	u16  min_voltage;  					          // 6   	欠压
	u16  min_voltage_time;  				      // 7   	欠压时间
	u16  min_voltage_enable;  			      // 8   	欠压使能
	u16  fast_off;  						          // 9   	速断
	u16  fast_off_time;  				          // 10   速断时间
	u16  fast_off_voltage_lock;  		      // 11   速断低压闭锁
	u16  fast_off_direction_lock;  		    // 12   速断方向闭锁
	u16  fast_off_enable;  				        // 13   速断使能
	u16  delay_fast_off;  				        // 14   延时速断
	u16  delay_fast_off_time;  			      // 15   延时速断时间
	u16  delay_fast_off_voltage_lock;  	  // 16   延时速断低压闭锁
	u16  delay_fast_off_direction_lock;   // 17   延时速断方向闭锁
	u16  delay_fast_off_enable;  		      // 18   延时速断使能
	u16  max_current;  					          // 19   过流
	u16  max_current_time;  				      // 20   过流时间
	u16  max_current_time_mode;  		      // 21   过流时限
	u16  max_current_voltage_lock;  		  // 22   过流低压闭锁
	u16  max_current_direction_lock;  	  // 23   过流方向闭锁
	u16  max_current_enable;  			      // 24   过流使能
	u16  max_current_reon_time;  		      // 25   过流重合闸时间
	u16  max_current_reon_enable;  		    // 26   过流重合闸使能
	u16  max_current_after;  			        // 27   过流后加速
	u16  max_current_after_time;  		    // 28   过流后加速时间
	u16  max_current_after_enable;  		  // 29   过流后加速使能
	u16  max_load;  						          // 30   过负荷
	u16  max_load_time;  				          // 31  	过负荷时间
	u16  max_load_enable;  				        // 32  	过负荷使能
	u16  zero_fast_off;  				          // 33   零序速断
	u16  zero_fast_off_time;  			      // 34   零序速断时间
	u16  zero_fast_off_enable;  			    // 35   零序速断使能
	u16  zero_max_current;  				      // 36   零序过流
	u16  zero_max_current_time;  		      // 37   零序过流时间
	u16  zero_max_current_enable;  		    // 38   零序过流使能
	u16  zero_max_current_reon_time;  	  // 39   零序过流重合闸时间
	u16  zero_max_current_reon_enable;  	// 40   零序过流重合闸使能
	u16  zero_max_current_after;  		    // 41   零序过流后加速
	u16  zero_max_current_after_time;  	  // 42   零序过流后加速时间
	u16  zero_max_current_after_enable;   // 43   零序过流后加速使能
	u16  imbalance_current;  			        // 44   不平衡电流
	u16  imbalance_current_time;  		    // 45   不平衡电流时间
	u16  imbalance_current_enable;  		  // 46   不平衡电流使能
	u16  fast_off_bak_enable;			        // 47	  速断后备使能
	u16  max_freq;  			                // 48   高频保护
	u16  max_freq_time;  		              // 49   高频保护延时
	u16  max_freq_enable;				          // 50   高频保护投退
	u16  low_freq;  			                // 48   低频保护
	u16  low_freq_time;  		              // 49   低频保护延时
	u16  low_freq_enable;				          // 50   低频保护投退
	u16  reon_Un_enable;				          // 54   重合闸检无压使能
	u16  reon_synchron_enable;  		      // 55   重合闸检同期使能
	u16  min_voltage_lock;  				      // 56   低压闭锁
	u16  reon_requency;                   // 57   重合闸次数  
	u16  once_reon_time;                  // 58   一次重合闸时间
	u16  once_reon_enable;  		          // 59   一次重合闸使能
	u16  secondary_reon_time;             // 60   二次重合闸时间
	u16  phaseloss_protect_time;          // 61   缺相保护延时时间
	u16  phaseloss_protect_enable;        // 62   缺相保护延时使能
};
union PROTECT_PARA1
{
	struct PROTECT_PARA para;
	u16 word[sizeof(struct PROTECT_PARA)/2];
};

struct PROTECT_DELAY
{
	u16  zero_max_voltage;  			// 1   	零序过压时间
	u16  zero_max_current;  			// 2    零序过流时间
	u16  no_phase;                // 3    缺相保护时间
  u16  ground;  				        // 4   	接地时间
	u16  fast_off;  					    // 5   	速断时间
	u16  delay_fast_off;  				// 6   	延时速断时间
	u16  max_current;  				    // 7    过流时间
	u16  max_current_after;  			// 8    过流后加速时间	
	
	u16  voltage_value_A;         // 9    A侧有压延时   
	u16  voltage_value_B;         // 10   B侧有压延时
	u16  novoltage_value_A;       // 11   A侧无压延时   
	u16  novoltage_value_B;       // 12   B侧无压延时   
	u16  loss_power;  				    // 13   失电延时
	u16  get_power;  		          // 14   得电延时
	u16  single_loss;  				    // 15   单侧失电延时
	u16  err_current;  		        // 16   故障检测延时
	u16  err_fastoff;  			      // 17   快速跳闸
	u16  zero_voltage;  			    // 18   零序电压延时
	u16  nonbreak_current;  		  // 19   非遮断电流保护
	
	u16  max_voltage;             // 20   过压时间
  u16  max_freq;                // 21   高频时间
	u16  low_freq;                // 22   低频时间
};

union PROTECT_DELAY1
{
	struct PROTECT_DELAY para;
	u16 word[sizeof(struct PROTECT_DELAY)/2];
};

struct PROTECT_TIMEOUT
{	
	u16  zero_max_voltage;  			// 1   	零序过压时间
	u16  zero_max_current;  			// 2    零序过流时间
	u16  no_phase;                // 3    缺相保护时间
  u16  ground;  				        // 4   	接地时间
	u16  fast_off;  					    // 5   	速断时间
	u16  delay_fast_off;  				// 6   	延时速断时间
	u16  max_current;  				    // 7    过流时间
	u16  max_current_after;  			// 8    过流后加速时间	
	
	u16  voltage_value_A;         // 9    A侧有压延时   
	u16  voltage_value_B;         // 10   B侧有压延时
	u16  novoltage_value_A;       // 11   A侧无压延时   
	u16  novoltage_value_B;       // 12   B侧无压延时   
	u16  loss_power;  				    // 13   失电延时
	u16  get_power;  		          // 14   得电延时
	u16  single_loss;  				    // 15   单侧失电延时
	u16  err_current;  		        // 16   故障检测延时
	u16  err_fastoff;  			      // 17   快速跳闸
	u16  zero_voltage;  			    // 18   零序电压延时
	u16  nonbreak_current;  		  // 19   非遮断电流保护
	
	u16  max_voltage;             // 20   过压时间
  u16  max_freq;                // 21   高频时间
	u16  low_freq;                // 22   低频时间
};

union PROTECT_TIMEOUT1
{
	struct PROTECT_TIMEOUT para;
	u16 word[sizeof(struct PROTECT_TIMEOUT)/2];
};

// 报警参数设置
struct ALARM_PARA      				//报警参数
{
	u16  alarmU0;                     // 0  零序过压报警
  u16  alarmU0_time;                // 1  零序过压报警延时时间
  u16	 alarmU0_enable;              // 2  零序过压报警使能
  u16  alarmI0;                     // 3  零序过流报警
  u16  alarmI0_time;                // 4  零序过流报警延时时间
  u16	 alarmI0_enable;              // 5  零序过流报警使能
  u16  max_load;				            // 6  过负荷报警
	u16  max_load_time;			          // 7  过负荷报警延时时间
	u16  max_load_enable;             // 8  过负荷报警使能
	u16  max_voltage;				          // 9  过压报警
	u16  max_voltage_time;            // 10 过压报警延时时间
	u16  max_voltage_enable;          // 11 过压报警使能
	u16  min_voltage;                 // 12 低电压报警
  u16  min_voltage_time;            // 13 低电压报警延时时间
  u16	 min_voltage_enable;          // 14 低电压报警使能
	u16  max_harmonic;                // 15 过谐波报警
  u16  max_harmonic_time;           // 16 过谐波报警延时时间
  u16	 max_harmonic_enable;         // 17 过谐波报警使能
	u16  voltage_qualifrate;          // 18 电压合格率报警
	u16  qualificate_voltage_min;     // 19 电压合格率下限
	u16  qualificate_voltage_max;     // 20 电压合格率上限
	u16  voltage_qualifrate_enable;   // 21 电压合格率使能
	u16  battery_low_voltage;         // 22 电池低电压报警

};
union ALARM_PARA1
{
	struct   ALARM_PARA para;
	u16 word[sizeof(struct ALARM_PARA)/2];
};

struct ALARM_DELAY
{
  u16  max_load;	              //0 过负荷						 
	u16  max_voltage;			        //1 过压	
	u16  min_voltage;             //2 欠压
	u16  max_harmonic;            //3 过谐波
	u16  voltage_qualifrate;      //4 电压合格率
	u16  battery_low_voltage;     //5 电池低电压
	u16  groundU0;  			        //6 接地电压   
  u16  groundI0;  			        //7 接地电流
	u16  ground;  			          //8 接地
};

union ALARM_DELAY1
{
	struct ALARM_DELAY para;
	u16 word[sizeof(struct ALARM_DELAY)/2];
};

struct ALARM_TIMEOUT
{
  u16  max_load;	              //0 过负荷						 
	u16  max_voltage;			        //1 过压	
	u16  min_voltage;             //2 欠压
	u16  max_harmonic;            //3 过谐波
	u16  voltage_qualifrate;      //4 电压合格率
	u16  battery_low_voltage;     //5 电池低电压
	u16  groundU0;  			        //6 接地电压   
  u16  groundI0;  			        //7 接地电流
	u16  ground;  			          //8 接地
};
union ALARM_TIMEOUT1
{
	struct ALARM_TIMEOUT para;
	u16 word[sizeof(struct ALARM_TIMEOUT)/2];
};

// 自动解列参数设置
struct AUTO_SWITCHOFF 
{     			    	
  u16  segment_contact_mode;   // 1  不依赖通信馈线自动化，分段，联络模式
	u16  voltage_value;          // 2  有压定值
	u16  voltage_value_time;     // 3  有压定值延时0-99s
	u16  novoltage_value;        // 4  失压定值
	u16  novoltage_value_time;   // 5  失压定值延时0-99s
  u16  losspower_enable;       // 6  失压延时分闸使能		
	u16  loss_power_time;  		   // 7  失电分闸时间 0-99s
	u16  getpower_enable_A;  		 // 8  A侧得电合闸使能
	u16  getpower_enable_B;  		 // 9  B侧得电合闸使能
	u16  get_power_time;  		   // 10 得电延时时间 0-99s
	u16  single_loss_enable_A;   // 11 A侧失电延时使能
	u16  single_loss_enable_B;   // 12 B侧失电延时使能
	u16  single_loss_time;       // 13 单侧失电延时时间0-99s
	u16  lock_switchoff_enable;  // 14 合闸成功闭锁失压分
	u16  lock_switchoff_time;    // 15 合闸成功闭锁失压分时间0-99s
	u16  err_lockon_enable;      // 16 合到故障闭锁合闸使能
	u16  err_current_time;  		 // 17 故障检测时限
	u16  err_fastoff_enable;     // 18 合到故障快速跳闸使能
	u16  err_fastoff_time;       // 19 合到故障快速跳闸时间0-99s

	u16  nonbreak_current_enable;// 20 非遮断电流使能
	u16  nonbreak_current_value; // 21 非遮断电流定值
	u16  inrush_current_enable;  // 22 涌流识别使能
	u16  inrush_current_value;   // 23 涌流识别定值
	u16  lock_resvoltage_enable; // 24 残压闭锁
	u16  PTbreak_alarm_enable;   // 25 PT断线告警
  u16  phase_err_value;  			 // 26 相间故障电流定值
	u16  ground_err_value;       // 27 接地故障电流定值
  u16  err_current_enable;     // 28 合到故障电流判据
	u16  zero_voltage_enable;    // 29 零序电压保护
};

union AUTO_SWITCHOFF1
{
	struct AUTO_SWITCHOFF para;
	u16 word[sizeof(struct AUTO_SWITCHOFF)/2];
};

struct STATE1_BITS {     				// bits  
  u16  breaker:2;  			// 0~1  断路器
	u16  handcart:2;  			// 2~3  手车
	u16  store:2;  				// 4~5  储能
	u16  knife:2;  				// 6~7  地刀  本体中没有地刀遥信，用作保护和报警记录用
	u16  directionA:1;  			// 8   	A相电流方向 1=反向
	u16  directionB:1;  			// 9   	B相电流方向 1=反向
	u16  directionC:1;  			// 10  	C相电流方向 1=反向
	u16  rsv3:1;  				// 11  	保留
	u16  rsv4:1;  				// 12  	保留
	u16  rsv5:1;  				// 13  	保留
	u16  disconnection_on:1;  	// 14  	开关合线圈断线
	u16  disconnection_off:1;  	// 15  	开关分线圈断线
};
union STATE								// 断路器状态 从本体实时数据读取
{
	struct STATE1_BITS bit;
	u16 word;
};

struct PROTECT1_BITS {     			 // bits  
  u16  zero_max_voltage:1;  		   // 0   零序过压
	u16  zero_max_current:1;  		   // 1   零序过流
	u16  no_phase:1;                 // 2   缺相
	u16  ground:1;  		    	       // 3   接地
	u16  fast_off:1;  				       // 4   速断
	u16  delay_fast_off:1;  		     // 5   延时速断
	u16  max_current:1;  			       // 6   过流
	u16  max_current_after:1;  	     // 7   过流后加速
	
	u16  loss_power:1;  			       // 8   失电分闸
	u16  get_power:1;  		           // 9   得电合闸
	u16  single_loss:1;  			       // 10  失电合闸
	u16  err_fastoff:1;  		         // 11  合到故障快速分闸
	u16  switchoff_lock:1;  		     // 12  分闸闭锁
  u16  switchon_lock:1;  		       // 13  合闸闭锁
	u16  zero_voltage:1;  		       // 14  零序电压保护
	u16  ptbreak_alarm:1;  		       // 15  PT断线告警
};

union PROTECT1							// 保护参数1
{
	struct PROTECT1_BITS bit;
	u16 word;
};

struct PROTECT2_BITS {     				// bits  
	u16  poweroff:1;  			// 0  电源失电
	u16  close_err:1;  		  // 1  合不上
	u16  open_err:1;  			// 2  分不开
	u16  lock:1;  					// 3  闭锁
	u16  reon_err:1;  			// 4  重合闸故障
	u16  reon_fail:1;  		  // 5  重合闸失败
	u16  reon_act:1;  			// 6  重合闸动作标志
  u16  protect_flag:1;    // 7  保护标志
	
	u16  open_flag:1;  			// 8  分闸标志
	u16  close_flag:1;  		// 9  合闸标志
  u16  have_err:1;  			// 10 故障电流
	u16  max_voltage:1;  		// 11 过压
	u16  max_freq:1;  		  // 12 高频
	u16  low_freq:1;  			// 13 低频
  u16  rsv1:1;            // 14 保留
	u16  rsv2:1;            // 15 保留
};

union PROTECT2							// 保护参数2
{
	struct PROTECT2_BITS bit;
	u16 word;
};

// 数据请求类型1
struct REQUEST1_BITS {     		 // bits  
  u16  record_on:1;  			     // 0  合闸操作记录
	u16  record_off:1;  			   // 1  分闸操作记录
	u16  record_protect:1;   	   // 2  保护记录
	u16  record_alarm:1;  	     // 3  报警记录
	u16  record_autofeed:1;  	   // 4  馈线自动化记录
	u16  reon_act:1;             // 5  重合闸记录
	u16  curve_current_forward:1;// 6  
	u16  curve_current_back:1;   // 7  
	u16  curve_on_A:1;  			   // 8 A相合闸曲线
	u16  curve_on_B:1;  			   // 9 B相合闸曲线
	u16  curve_on_C:1;  			   // 10C相合闸曲线
	u16  curve_off_A:1;  		     // 11A相分闸曲线
	u16  curve_off_B:1;  		     // 12B相分闸曲线
	u16  curve_off_C:1;  		     // 13C相分闸曲线
	u16  curve_protect_A:1;      // 14A相保护曲线
	u16  curve_protect_B:1;      // 15B相保护曲线
};

union REQUEST1							// 数据请求类型1
{
	struct REQUEST1_BITS bit;
	u16 word;
};
// 数据请求类型2
struct REQUEST2_BITS {     		// bits  
  u16  curve_protect_C:1;  	  // 0  	C相保护曲线
	u16  curve_position_on:1;   // 1  	合闸位置曲线
	u16  curve_position_off:1;  // 2  	分闸位置曲线
	u16  curve_ground_O:1;  	  // 3  	零序接地曲线
	u16  curve_PUA:1;  		      // 4  	A相保护电压曲线
  u16  curve_PUB:1;  		      // 5    B相保护电压曲线
	u16  curve_PUC:1;  		      // 6  	C相保护电压曲线
  u16  curve_PU0:1;  		      // 7    保护零序电压曲线
	u16  curve_PIA:1;  		      // 8  	A相保护电流曲线
	u16  curve_PIB:1;  		      // 9  	B相保护电流曲线
	u16  curve_PIC:1;  		      // 10  	C相保护电流曲线
	u16  curve_PI0:1;  		      // 11   保护零序电流曲线
	u16  rsv12:1;  		          // 12   保留
	u16  rsv13:1;  		          // 13   保留
	u16  rsv14:1;  		          // 14  	保留
	u16  rsv15:1;  		          // 15  	保留
};

union REQUEST2							// 数据请求类型2
{
	struct REQUEST2_BITS bit;
	u16 word;
};
// 数据请求类型3
struct REQUEST3_BITS {     		// bits  
	u16  curve_P10s_IB:1;  		// 0  	B相10秒保护电流有效值曲线
  u16  curve_P10s_IC:1;  		// 1  	C相10秒保护电流有效值曲线
	u16  rsv2:1;  		// 2    保留
	u16  rsv3:1;      // 3    保留
  u16  rsv4:1;  		// 4  	保留
	u16  rsv5:1;  		// 5  	保留
	u16  rsv6:1;  		// 6  	保留
	u16  rsv7:1;  		// 7  	保留
	u16  rsv8:1;  		// 8   	保留
	u16  rsv9:1;  		// 9   	保留
	u16  rsv10:1;  		// 10  	保留
	u16  rsv11:1;  		// 11  	保留
	u16  rsv12:1;  		// 12  	保留
	u16  rsv13:1;  		// 13  	保留
	u16  rsv14:1;  		// 14  	保留
	u16  rsv15:1;  		// 15  	保留
};

union REQUEST3							// 数据请求类型3
{
	struct REQUEST3_BITS bit;
	u16 word;
};

// 本体遥信2
struct BTYX2_BITS {     			// bits  
  u16  rsv0:1;  		// 0  	空N12
	u16  BHYX1:1;  	  // 1  	保护1N6
	u16  rsv1:1;  		// 2  	空N13
	u16  HQYX1:1;  	  // 3  	合闸断线遥信1
  u16  BHYX2:1;  	  // 4  	保护2N7
	u16  rsv2:1;  		// 5  	空N14
	u16  FQYX2:1;  	  // 6  	分闸断线遥信2
	u16  BHYX3:1;  	  // 7  	保护3N8
	u16  POW5V:1;  	  // 8   	5V2电源N15
	u16  BSYX3:1;  	  // 9   	闭锁遥信3
	u16  BHYX4:1;  	  // 10  	保护4N9
	u16  POW12V:1;  	// 11  	12V电源N16
	u16  HBYX4:1;  	  // 12  	后备动作遥信4
	u16  PTDX:1;  		// 13  	PT断线
	u16  CTDX:1;  		// 14  	CT断线
	u16  rsv5:1;  		// 15  	保留
};

union BTYX2							// 本体遥信2
{
	struct BTYX2_BITS bit;
	u16 word;
};
// 本体遥信3
struct BTYX3_BITS {     			// bits  
  u16  voltage_lossA:1;  		// 0  	A相失压	  1=失压
	u16  voltage_lossB:1;  		// 1  	B相失压
	u16  voltage_lossC:1;  		// 2  	C相失压
	u16  voltage_sequence:1;  	// 3  	电压相序错	1=相序错
  u16  current_sequence:1;  	// 4  	电流相序错
	u16  current_directionA:1;  	// 5  	A相电流方向	1=反向
	u16  current_directionB:1;  	// 6  	B相电流方向
	u16  current_directionC:1;  	// 7  	C相电流方向
	u16  rsv8:1;  		// 8   	保留
	u16  rsv9:1;  		// 9   	保留
	u16  rsv10:1;  		// 10  	保留
	u16  rsv11:1;  		// 11  	保留
	u16  rsv12:1;  		// 12  	保留
	u16  rsv13:1;  		// 13  	保留
	u16  rsv14:1;  		// 14  	保留
	u16  rsv15:1;  		// 15  	保留
};

union BTYX3							// 本体遥信3
{
	struct BTYX3_BITS bit;
	u16 word;
};

struct SELF_CHECK_BITS {    // bits  
	u16  ext_ram:1;  				  // 0	  外部内存故障
	u16  int_ram:1;  				  // 1	  内部内存出错
	u16  att7022:1;  				  // 2	  电度表芯片故障
	u16  para_set:1;  				// 3	  定值出错
	u16  GPRS_state:2;  		  // 4,5  GPRS状态
	u16  rsv1:1;  						// 6  	保留
	u16  WIFI_state:1;  			// 7  	wifi状态
	u16  currentA:1;  				// 8  	A相电流测量通道出错
	u16  currentB:1;  				// 9  	B相电流测量通道出错
	u16  currentC:1;  				// 10  	C相电流测量通道出错
	u16  voltageA:1;  				// 11  	A相电压测量故障
	u16  voltageB:1;  				// 12  	B相电压测量故障
	u16  voltageC:1;  				// 13  	C相电压测量故障
	u16  Temp_Humi:1;  				// 14  	温湿度状态
	u16  GPS_state:1;  				// 15  	GPS状态
};
union SELF_CHECK_STATE				// 本体自检状态
{
	struct SELF_CHECK_BITS bit;
	u16 word;
};

struct ALARM_BITS {     			// bits  
	u16  max_load:1;  					// 0   	过载
	u16  max_voltage:1;  				// 1   	过压
	u16  min_voltage:1;  				// 2  	欠压
	u16  max_harmonic:1;  			// 3  	过谐波
	u16  voltage_qualifrate:1;  // 4  	电压合格率
	u16  battery_low_voltage:1; // 5  	电池欠压
	u16  ground:1;  				    // 6  	接地
	u16  zero_max_voltage:1;  	// 7  	零序过压
	u16  zero_max_current:1;  	// 8  	零序过流
	u16  bat_active:1;  				// 9  	电池活化
	u16  rsv10:1;  				      // 10  	保留
	u16  rsv11:1;  				      // 11  	保留
	u16  rsv12:1;  				      // 12  	保留
	u16  rsv13:1;  				      // 13  	保留
	u16  rsv14:1;  				      // 14  	保留
	u16  rsv15:1;  				      // 15  	保留
};
union ALARM_STATE					// 报警状态
{
	struct ALARM_BITS bit;
	u16 word;
};

// 遥信 字节
struct YX_BITS {     			// bits  
  u8  D0:1;  		// 0  	单位遥信
	u8  D1:1;  		// 1  	单位遥信
	u8  D2:1;  		// 2  	单位遥信
	u8  D3:1;  		// 3  	单位遥信
  u8  D4:1;  		// 4  	单位遥信
	u8  D5:1;  		// 5  	单位遥信
	u8  D6:1;  		// 6  	单位遥信
	u8  D7:1;  		// 7  	单位遥信
};

union YXDefine					// 遥信定义
{
	struct YX_BITS bit;
	u8 byte;
};

// 遥信	字
struct YXWORD_BITS {     			// bits  
  u8  D0:1;  		// 0  	单位遥信
	u8  D1:1;  		// 1  	单位遥信
	u8  D2:1;  		// 2  	单位遥信
	u8  D3:1;  		// 3  	单位遥信
  u8  D4:1;  		// 4  	单位遥信
	u8  D5:1;  		// 5  	单位遥信
	u8  D6:1;  		// 6  	单位遥信
	u8  D7:1;  		// 7  	单位遥信
  u8  D8:1;  		// 8  	单位遥信
	u8  D9:1;  		// 9  	单位遥信
	u8  D10:1;  		// 10  	单位遥信
	u8  D11:1;  		// 11  	单位遥信
  u8  D12:1;  		// 12  	单位遥信
	u8  D13:1;  		// 13  	单位遥信
	u8  D14:1;  		// 14  	单位遥信
	u8  D15:1;  		// 15  	单位遥信
};

union YXWORDDefine					// 遥信定义
{
	struct YXWORD_BITS bit;
	u16 word;
};

// 断路器本体测控实时数据
struct KZQMEASURE
{
	/*遥信*/
	union STATE DLQflag;				// 断路器标志
	union PROTECT1 ProtectFlag1;		// 保护标志1
	union PROTECT2 ProtectFlag2;		// 保护标志2
	union REQUEST1 RequestFlag1;		// 数据请求标志1
	union REQUEST2 RequestFlag2;		// 数据请求标志2
	union REQUEST3 RequestFlag3;		// 数据请求标志3
	u16 BTYX1;
	union BTYX2 YX2;
	union BTYX3 YX3;
	union SELF_CHECK_STATE SelfCheck;	// 自检标志
	u16 ErrCode;
	union ALARM_STATE AlarmFlag;		// 报警标志
	union YXWORDDefine FXState;			// 分相断路器状态
	/*遥测*/
	u16 Uab;				 	// 线电压//13
	u16 Ubc;
	u16 Uca;
	u16 PTUa;				 	// PT相电压
	u16 PTUb;
	u16 PTUc;
	u16 Ua;				 	// 相电压
	u16 Ub;
	u16 Uc;
	u16 degUA;
	u16 degUB;
	u16 degUC;
	u16 U0;				 	// 零序电压
	u16 I0;					// 零序电流
	u16 Freq;				// 电网频率 27
	u16 degU0;
	u16 degI0;
	u16 Ia;					// 线电流
	u16 Ib;
	u16 Ic;
	u16 degIA;
	u16 degIB;
	u16 degIC;
	s16	P_all;				// 三相有功
  s16 Q_all;				// 三相无功
	u16 COS_all;			// 功率因数
	u16 Wp;					// 当日有功电量
	u16 Wq;					// 当日无功电量
	u16 COS_W;				// 当日电量功率因数
	u16 XBU_all;			// 电压谐波   42
	u16 XBU2;
	u16 XBU3;
	u16 XBU5;
	u16 XBU7;
	u16 XBU11;
	u16 XBU13;
	u16 XBU17;
	u16 XBU19;
	u16 XBI_all;			// 电流谐波
	u16 XBI2;
	u16 XBI3;
	u16 XBI5;
	u16 XBI7;
	u16 XBI11;
	u16 XBI13;
	u16 XBI17;
	u16 XBI19;
	u16 PUab;				// 保护线电压
	u16 PUbc;
	u16 PUca;
	u16 PdegUab;
	u16 PIa;					// 保护线电流    64
	u16 PIb;
	u16 PIc;
	u16 P10Ia;				// 10倍保护线电流
	u16 P10Ib;
	u16 P10Ic;                    //69
  u16 NJDFS;  
	u16 JDXB;
  u16 DYCFZC;   
  u16 JDXZ;
  u16 JDYF;
	u16 HGXB;
	u16 HGGDY;                      //76
	u16 Wp_all_L;		// 正向总有功电量
	u16 Wp_all_H;		// 正向总有功电量
	u16 Wq_1all_L;		// Ⅰ象限总无功电量
	u16 Wq_1all_H;		// Ⅰ象限总无功电量
	u16 Wp_peak_L;		// 正向峰有功电量
	u16 Wp_peak_H;		// 正向峰有功电量
	u16 Wq_2all_L;		// Ⅱ象限总无功电量
	u16 Wq_2all_H;		// Ⅱ象限总无功电量
	u16 Wp_valley_L;	// 正向谷有功电量
	u16 Wp_valley_H;	// 正向谷有功电量
	u16 Wq_3all_L;		// Ⅲ象限总无功电量
	u16 Wq_3all_H;		// Ⅲ象限总无功电量
	u16 Wp_level_L;	// 正向平有功电量
	u16 Wp_level_H;	// 正向平有功电量
	u16 Wq_4all_L;		// Ⅳ象限总无功电量
	u16 Wq_4all_H;		// Ⅳ象限总无功电量
	u16 EMUAB;         //93
	u16 EMUBC;
	u16 EMUCA;
	u16 EMIA;
	u16 EMIB;
	u16 EMIC;
	u16 COSA;
	u16 COSB;
	u16 COSC;
	u16 WpFX_all_L;		// 反向总有功电量
	u16 WpFX_all_H;		// 反向总有功电量
	u16 WpFX_peak_L;		// 反向峰有功电量
	u16 WpFX_peak_H;		// 反向峰有功电量
	u16 WpFX_valley_L;	// 反向谷有功电量
	u16 WpFX_valley_H;	// 反向谷有功电量
	u16 WpFX_level_L;	// 反向平有功电量
	u16 WpFX_level_H;	// 反向平有功电量
	u16 year;          //110
	u16 month;
	u16 date;
	u16 hour;
	u16 minute;
	u16 second;
	u16 battery_vol;
};
union KZQMEASUREDATA						// 断路器实时数据
{
	struct KZQMEASURE para;
  u16 word[sizeof(struct KZQMEASURE)/2];
};

/**********************
*记录
*长度 = 29
**********************/
struct RECORDpara		// 详细记录
{
	u16 year;	// 年		0	byte		
	u16 month;	// 月			byte
	u16 date;	// 日			byte
	u16 hour;	// 时			byte
	u16 min;		// 分			byte
	u16 sec;		// 秒		5	byte
	u16 msec;	// 毫秒	6
	u16 type;			// 7保护类型
	u16 setvalue;	// 8设置值
	u16 setvaluetime;//9设定延时
	u16 Protect_UA_val;				// 10	A相电压
	u16 Protect_UB_val;				// 11	B相电压
	u16 Protect_UC_val;				// 12	C相电压
	u16 Protect_IA_val;				// 		A相电流
	u16 Protect_IB_val;				// 		B相电流
	u16 Protect_IC_val;				// 		C相电流
	u16 P;					          // 		有功功率
	u16 Q;					          // 		无功功率
	u16 COS;				          // 		功率因数
	u16 Protect_U0_val;				// 19 零序电压
	u16 Protect_I0_val;				// 20 零序电流
	u16 degU0;
	u16 degI0;
	u16 JDXZ;
	u16 DYCFZC;
	u16 JDXB;
	u16 Uqrate;			
  u16 Uharmonic;
	u16 Iharmonic;
	u16 Freq;                 // 频率
};

union RECORD_ACTION			
{
	struct RECORDpara para;
	u16 word[sizeof(struct RECORDpara)/2];
};

extern s16 PcurveUA[1200];	// A相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveUB[1200];	// B相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveUC[1200];	// C相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveIA[1200];	// A相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveIB[1200];	// B相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveIC[1200];	// C相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveU0[1200];	// 零序保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveI0[1200];	// 零序保护电流曲线录波，保护动作前4个周期，保护动作后8个周期

extern s16 PcurveUA_bak[1200];	// A相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveUB_bak[1200];	// B相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveUC_bak[1200];	// C相保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveIA_bak[1200];	// A相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveIB_bak[1200];	// B相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveIC_bak[1200];	// C相保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveU0_bak[1200];	// 零序保护电压曲线录波，保护动作前4个周期，保护动作后8个周期
extern s16 PcurveI0_bak[1200];	// 零序保护电流曲线录波，保护动作前4个周期，保护动作后8个周期
/*******************************ATT7022E读取校表参数*********************************/
extern u32 ZYXSA,ZYXSB,ZYXSC;	// 有功增益系数
extern u32 XWXSA,XWXSB,XWXSC;	// 相位增益系数
extern u32 WGXSA,WGXSB,WGXSC;	// 无功增益系数
extern u32 DYXSA,DYXSB,DYXSC;	// 电压增益系数
extern u32 DLXSA,DLXSB,DLXSC;	// 电流增益系数
extern u32 DYPYA,DYPYB,DYPYC;	// 电压增益系数
extern u32 DLPYA,DLPYB,DLPYC;	// 电流增益系数
extern u32 DYXSA1,DYXSB1,DYXSC1;	// 电压增益系数
extern u32 DLXSA1,DLXSB1,DLXSC1;	// 电流增益系数
extern s32 ATT7022_Ept,ATT7022_Eqt,ATT7022_Est;			  // 有功电能，无功电能，视在电能
extern u32 ATT7022_sum;
extern u8 saveATT7022_flag;
extern u8 Start_Debug;
extern u8 TESTFlag;
extern u8 GPS_Flag;
extern u8 TestFlagCnt;
extern u8 Battery_Readflag;
extern u8 MachineInformation[13];	// 本体控制器生产年(2)月(2)出厂编号(7)

extern u16  Para_peak1_period_S;		// 电量统计时段设置
extern u16  Para_peak1_period_E;
extern u16  Para_peak2_period_S;
extern u16  Para_peak2_period_E;
extern u16  Para_valley_period_S;
extern u16  Para_valley_period_E;

extern s16 AD_IA_val;               
extern s16 AD_IB_val;
extern s16 AD_IC_val;
extern s16 AD_PIA_val;
extern s16 AD_PIB_val;
extern s16 AD_PIC_val;
extern s16 AD_U0_val;
extern s16 AD_I0_val;

extern u32 WpLast;			      // 上一次当日有功电能
extern u32 WqLast;			      // 上一次当日无功
extern u32 Wp_all_Last;		    // 上一次总有功电能
extern u32 WpFX_all_Last;	    // 上一次总反向有功电能
extern u32 Wp_peak_Last;		  // 上一次峰有功电能
extern u32 WpFX_peak_Last;	  // 上一次峰反向有功电能
extern u32 Wp_valley_Last;	  // 上一次谷有功电能
extern u32 WpFX_valley_Last;	// 上一次谷反向有功电能
extern u32 Wp_level_Last;	    // 上一次平有功电能
extern u32 WpFX_level_Last;	  // 上一次平反向有功电能

extern u64 Wq_1all_Last;		  // 上一次I象限总无功
extern u64 Wq_2all_Last;		  // 上一次II象限总无功
extern u64 Wq_3all_Last;		  // 上一次III象限总无功
extern u64 Wq_4all_Last;		  // 上一次IV象限总无功
extern u16 PowerOffCnt;			  // 掉电检测计数
extern u8 BattryTimeOut;
extern u8 YabanSave_Flag;			  // 压板保存标志
extern u8 ParaInitReadyFlag;		// 参数初始化准备好

extern u8 MeasureIAError;			  // A相测量故障标志
extern u8 MeasureIBError;			  // B相测量故障标志
extern u8 MeasureICError;			  // C相测量故障标志
extern u16 ZeroPointIA;				  // 一倍电流测量零点	
extern u16 ZeroPointIA10;				// 十倍电流测量零点	
extern u16 ZeroPointIB;				  // 一倍电流测量零点	
extern u16 ZeroPointIB10;				// 十倍电流测量零点	
extern u16 ZeroPointIC;				  // 一倍电流测量零点	
extern u16 ZeroPointIC10;				// 十倍电流测量零点	
extern u16 CtrlMeasureCnt;		  // 控制测量计数
extern u8  CtrlMeasureFlag;		  // 控制测量标志
extern u8 StartCntFlag;         // 合闸计数标志
extern u16 SwitchCnt;           // 操作次数
extern u16 SelfCheckErrCode;		// 自检故障代码
extern u8 AlarmRecord_flag;
extern u8 SwitchFlag;				    // 开关状态
extern u8 ReOnFlag;					    // 重合闸标志
extern u16 ReOnTimeDelay;				// 重合闸计时10ms
extern u8 ReOnTimeOut;					// 重合闸时间到达标志
extern u8 ReOnLockCnt;				  // 重合闸闭锁时间10秒
extern u8 ReOnLockCnt1;				  // 合故障重合闸闭锁时间10秒
extern u16 ReOnDelay;					  // 重合闸延时时间
extern u8 ReOnTimes;	          // 重合闸次数
extern u8 DLQZD_flag;				    // 断路器诊断信息标志
extern u8 ProtectFlag;          // 保护标志
extern u16 ProtectTimesLock;	    // 保护次数闭锁，合闸位置只闭锁一次，分闸后进行下一次保护
extern u8 ProtectTimesLockCnt;  // 保护时间闭锁计数
extern u16 AlarmTimesLock;	      // 报警次数闭锁，合闸位置只闭锁一次，分闸后进行下一次报警
extern u8 AlarmTimesLockCnt;		// 报警时间闭锁计数
extern u8 Switchon_Lock;	      // 合闸闭锁
extern u8 Switchon_Lock1;	      // 两侧带电禁止合闸
extern u8 Switchon_Lock2;	      // 残压闭锁合闸
extern u8 Switchon_Lock2_Flag;
extern u8 Switchoff_Lock;       // 分闸闭锁
extern u8 Switchoff_Lock1;       // 分闸闭锁
extern u8 SigresetLock;         // 闭锁复归计时开始标志
extern u8 AD_Index;		          // AD采样序号
extern u16 AD_StartPoint;		    // AD采样临界点
extern u8 ProtectRecord_flag;		// 保护记录标志，用于记录保护缓冲区是否有数据
extern u16 Max_current_after_cnt; // 过流后加速计时
extern u8  PTDX_Flag;				    // PT断线标志，1=断线
extern u8  CTDX_Flag;				    // CT断线标志，1=断线
extern u8  PTDX_Enable;				  // PT断线检测允许，1=允许
extern u8  CTDX_Enable;				  // CT断线检测允许，1=允许
extern u8  MeasureReady;				  // 测量完成标志
extern u8  GpsOn_Flag;
extern u16 qualif_volt;           // 电压合格率
extern u8 AUTHORIZATIONcode[16];	//授权码
extern u8 ClearTJXX_Flag;
// DMA发送缓冲
extern u8 DMATXbuffer1[300]; 
extern u8 DMATXbuffer2[820]; 
extern u8 DMATXbuffer3[100]; 
extern u8 DMATXbuffer4[300];
extern u8 DMATXbuffer5[420]; 
extern u8 DMATXbuffer6[100]; 
// DMA接收缓冲
extern u8 DMARXbuffer1[50];
extern u8 DMARXbuffer2[50];
extern u8 DMARXbuffer3[50];
extern u8 DMARXbuffer4[50];
extern u8 DMARXbuffer5[50];
extern u8 DMARXbuffer6[50];
extern u16 DMAADCbuffer1[100][9];
extern u16 DMAReadIndex1;
extern u16 DMAReadIndex2;
extern u16 DMAReadIndex3;
extern u16 DMAReadIndex4;
extern u16 DMAReadIndex5;
extern u16 DMAReadIndex6;

extern u8 Usart1RxReady; // 串口1接收完成标志
extern u8 Usart1bakRxReady; // 串口1接收完成标志
extern u8 Usart2RxReady; // 串口2接收完成标志
extern u8 Usart2bakRxReady; // 串口2接收完成标志
extern u8 Usart3RxReady; // 串口3接收完成标志
extern u8 Usart3bakRxReady; // 串口3接收完成标志
extern u8 Usart4RxReady; // 串口4接收完成标志
extern u8 Usart4bakRxReady; // 串口4接收完成标志
extern u8 Usart5RxReady; // 串口5接收完成标志
extern u8 Usart5bakRxReady; // 串口5接收完成标志
extern u8 Usart6RxReady; // 串口6接收完成标志
extern u8 Usart6bakRxReady; // 串口6接收完成标志
extern u8 Usart2TxReady;// 串口2准备发送标志
extern u8 delay_time_101;
extern u8 timer_enable;	  //使能计时器
extern const  u8 SOE_Enable[20];
extern u8 RealCurveFlag;
extern u8 SOEPad_Flag;
extern u8 SOE_Flag;
extern u8 ModulusFlag;
extern u8 SetSystemFlag;
extern u8 SetProtectFlag;
extern u8 SetAlarmFlag;
extern u8 SetAutoswitchFlag;
extern u8 Set101ParaFlag;
extern u8 SigresetFlag;
extern u8 OperateNumber;
extern u8 GroundNumber;
extern u8 AlarmNumber;
extern u8 ProtectNumber;
extern u8 ZeroTimeArrived;
extern u8 KZQUpdataSucced;
extern u8 RecoverKZQBackupFlag;
extern u8 Select101Addr;
extern u8 EVENT_101Flag;  // 101协议事件标志
extern u8 WifiConnectState;
extern u8 ControlLock;
extern u8 Battery_Voltage_Index;
extern u16 DLQ_EventRecord_Index;
extern u16 DLQ_EventRecord_Order[500];
extern u16 DLQ_SOERecord_Index;
extern u16 DLQ_SOERecord_Order[300];
extern u16 Battery_Voltage_Order[120];
extern u8 Curve_Index;
extern u16 Curve_Order[100];
extern u8 SendLock_Cnt;
extern u16 Battery_RunTime; // 电池运行时间
extern u16 CodeTimeout;
extern s16 Temp_value,Humi_value;
extern u32 Lock_IdCode;
extern u32 Reset_Value;
extern u16 JudgePoint;
extern u8 JudgePoint_Flag;
extern u8 HumiTest_Flag;
extern u8 Send_Flag;
extern u16 Freez_15min_cnt;

extern struct Measure          MeasureData;			     // 测量值
extern struct Measure1         MeasureCurve;		     // 测量曲线
extern struct time_now         TimeNow;	     // 当前时间
extern struct UpdateVersion    MyVersion;	           // 运行程序和备份程序版本和修改时间
extern union  PROTECT_DELAY1   ProtectDelay;	       // 保护延时计时
extern union  PROTECT_TIMEOUT1 ProtectTimeout;	     // 保护时间到达标志
extern union  ALARM_DELAY1     AlarmDelay;           // 报警延时计时
extern union  ALARM_TIMEOUT1   AlarmTimeout;         // 报警时间到达标志
extern union  KZQMEASUREDATA   KZQMeasureData;	     // 断路器实时数据
extern union  RECORD_ACTION    Record_on;	           // 合闸记录
extern union  RECORD_ACTION    Record_off;	         // 分闸记录
extern union  RECORD_ACTION    Record_protect1;	     // 保护记录
extern union  RECORD_ACTION    Record_protect2;	     // 保护记录
extern union  RECORD_ACTION    Record_alarm;	  	   // 报警记录
extern union  RECORD_ACTION    Record_autofeed;      // 馈线自动化记录
extern union  SYSTEM_PARA1     SystemSet;	           // 系统参数设置
extern union  PROTECT_PARA1    ProtectSet;	         // 保护参数设置
extern union  PROTECT_PARA1    ProtectSetReceive;
extern union  ALARM_PARA1      AlarmSet;	           // 报警参数设置
extern union  AUTO_SWITCHOFF1  AutoswitchSet;	       // 自动解列参数设置
extern union  Modulus1         ProtectModulus;		   // 保护测量系数
extern union  DLQZDXX          DLQZDinformation;     // 断路器诊断信息
extern union  Rule101Para1     Measure101Para;      // 101规约参数
extern union YXDefine          lockbz;

extern OS_TID t_Task1;                        /* assigned task id of task: Task1   */
extern OS_TID t_Task2;                        /* assigned task id of task: Task2  */
extern OS_TID t_Task3;                        /* assigned task id of task: Task3  */
extern OS_TID t_Task4;                        /* assigned task id of task: Task4  */
extern OS_TID t_Task5;                        /* assigned task id of task: Task5  */
extern OS_TID t_Task6;                        /* assigned task id of task: Task6  */
extern OS_TID t_Task7;                        /* assigned task id of task: Task7  */
extern OS_TID t_Task8;                        /* assigned task id of task: Task8  */
extern OS_TID t_Task9;                        /* assigned task id of task: Task9  */
extern OS_TID t_Task10;                        /* assigned task id of task: Task10  */
extern OS_TID t_Task11;                        /* assigned task id of task: Task11  */
extern OS_TID t_Task12;                        /* assigned task id of task: Task12  */
extern OS_TID t_Task13;                        /* assigned task id of task: Task13  */
extern OS_TID t_Task14;                        /* assigned task id of task: Task14  */
extern OS_TID t_Task15;                        /* assigned task id of task: Task15  */
extern OS_TID t_Task16;                        /* assigned task id of task: Task16  */
extern OS_TID t_Task17;                        /* assigned task id of task: Task17  */

extern struct QueuePara QueueTX1;			// 串口1发送队列控制变量
extern u8 BufferTX1[512];		// 串口1发送缓冲

extern struct QueuePara QueueTX2;			// 串口2发送队列控制变量
extern u8 BufferTX2[820];		// 串口2发送缓冲

extern struct QueuePara QueueTX3;			// 串口3发送队列控制变量
extern u8 BufferTX3[512];		// 串口3发送缓冲

extern struct QueuePara QueueTX4;			// 串口4发送队列控制变量
extern u8 BufferTX4[512];		// 串口4发送缓冲

extern struct QueuePara QueueTX5;			// 串口5发送队列控制变量
extern u8 BufferTX5[512];		// 串口5发送缓冲

extern struct QueuePara QueueTX6;			// 串口6发送队列控制变量
extern u8 BufferTX6[512];		// 串口6发送缓冲

extern struct QueuePara QueueSOE;			// SOE发送队列控制变量
extern u8 BufferSOE[640];		// SOE发送缓冲

extern u16 SOE[16][11];

extern struct QueuePara QueueSOE_Pad;	// SOE_Pad发送队列控制变量
extern u8 BufferSOE_Pad[300];// SOE_Pad发送缓冲

extern struct QueuePara *QueueTXChannel[6];
extern u8 *BufferTXChannel[6];

extern u8 ProtectParaChecked;		// 保护参数检查每天只读取flash一次，零点复位
extern u8 SystemParaChecked;		// 系统参数检查每天只读取flash一次，零点复位
extern u8 AlarmParaChecked;     // 报警参数检查每天只读取flash一次，零点复位
extern u8 AutoswitchParaChecked;// 自动解列参数检查每天只读取flash一次，零点复位
extern u8 CorrectParaChecked;   // 校表参数检查每天只读取flash一次，零点复位
extern u8 MeasureParaChecked;   // 测量系数检查每天只读取flash一次，零点复位
extern u8 Measure101ParaChecked;// 101规约参数检查每天只读取flash一次，零点复位
extern u8 PeriodMode[24];			  // 时段模式，用于电量分时统计

/************************** 接地 **************************/
extern u16  u0fd;         // u0fd=FJBU0/3
extern u16  xxpdxs;       // 相序判断系数
extern s16  iuxw;         // U0用来判断的相位，I0与U0的相位差。
extern u8   jdyf;         // 接地与否
extern u8   zxdjdfs;      // 中性点接地方式
extern s16  u0pdxw;       // 电压与电流之间的相位
extern u8   jdxb;         // 接地相别
extern u8   dycfzc;       // 电源侧/负载侧
extern u8   dycfzc2;      // 电源侧/负载侧2
extern u8   jdxz;         // 接地性质
extern u8   hgxb;         // 弧光相别
extern u16  hggdy;        // 弧光过电压
/***************************************/
extern u8 under_voltage_on;
extern u8 RamTest(void);
extern void SaveCruve(u8 page,s16 *curve,s16 point);// 保存保护曲线
extern void ReadCruve(u8 page,s16 *curve);// 读取保护曲线
extern void SaveProtectPara(void); // 保存保护参数
extern u8 ReadProtectPara(void);// 读取保护参数
extern void SaveProtectParaBAK(void); // 保存保护参数备份
extern u8 ReadProtectParaBAK(void);// 读取保护参数备份
extern void SaveModulus(void);
extern u8 ReadModulus(void);
extern void SaveModulusBAK(void);
extern u8 ReadModulusBAK(void);
extern u8 SystemParaCompare(void);
extern u8 ProtectParaCompare(void);
extern u8 AlarmParaCompare(void);
extern u8 AutoswitchParaCompare(void);
extern u8 ModulusParaCompare(void);
extern void SaveSystemPara(void);// 保存系统设置参数
extern u8 ReadSystemPara(void);// 读取系统设置参数
extern void SaveSystemParaBAK(void);// 保存系统设置参数备份
extern u8 ReadSystemParaBAK(void);// 读取系统设置参数备份
extern void SaveAlarmPara(void);// 保存报警设置参数
extern u8 ReadAlarmPara(void);// 读取报警设置参数
extern void SaveAlarmParaBAK(void);// 保存报警设置参数备份
extern u8 ReadAlarmParaBAK(void);// 读取报警设置参数备份
extern u8 Read101Para(void);
extern u8 Read101ParaBAK(void);
extern void Save101Para(void);
extern void Save101ParaBAK(void);
extern u8 Measure101ParaCompare(void);

extern void SaveAutoSwitchPara(void);// 保存自动解列参数
extern u8 ReadAutoSwitchPara(void);// 读取自动解列参数
extern void SaveAutoSwitchParaBAK(void);// 保存自动解列参数备份
extern u8 ReadAutoSwitchParaBAK(void);// 读取自动解列参数备份

extern void SaveMachineInformation(void);
extern void SaveATT7022(void);
extern u8 Read_ATT7022(void);
extern void Read_ATT7022BAK(void);
extern void Write_ATT7022(void);
extern void Check_ATT7022(void);
extern void RecordSwitchOn(u8 switch_type);	   	// 记录合闸操作记录信息
extern void RecordSwitchOff(u8 switch_type);	   	// 记录分闸操作记录信息
extern void RecordProtectStart(u8 protect_type);	// 记录保护操作记录信息
extern void RecordAlarm(u8 type,s16 set_value,s16 set_delay);
extern void SaveRecordAlarm(void);
extern void RecordAutofeed(u8 switch_type);	// 馈线自动化记录信息
extern void JDPD(void);

extern void SaveRecordOn(void);  // 保存合闸记录
extern void SaveRecordOff(void);  // 保存分闸记录
extern void SaveRecordProtect1(void); // 保存保护记录1
extern void SaveRecordProtect2(void);	 // 保存保护记录2
extern void SaveRecordAutofeed(void);	 // 保存馈线自动化记录
extern u32 SqrtINT32U(u32 radicand); 	/* 32 位数的开方 */

extern void CheckSystemPara(void);          // 检查系统参数设置
extern void CheckProtectPara(void);			    // 检查保护参数设置
extern void CheckAlarmPara(void);           // 检查报警参数设置
extern void CheckAutoSwitchPara(void);      // 检查自动解列参数设置
extern void CheckModulus(void);             // 检查测量参数
extern void CheckMeasure101Para(void);      // 检查101规约参数
extern void SavePowerOffPara(void);
extern void TaskRemarks(void);
extern void ClearTJXX(void);
extern void RecordSOE(u8 addr,u8 state);// SOE事件记录
extern void RecordSOE_Pad(u8 addr,u8 state);
extern void UART1_Configuration(s32 baudrate);
extern void ReadVersionfromFlash(void);
extern void ReadInformation(u8 mode);
extern void Delay(u32 nCount);
extern void ReadPowerOffPara(void);
extern void ReadMachineInformation(void);
extern void MeasureDataCalcAndProcess(void);
extern u8 Humi_Handle_Step(void);
#endif

