/*************************************************************
*文件名称：protocol2.h
*创建日期：2018年11月16日
*完成日期：
*作    者：shq
**************************************************************/
#include "global.h"
#ifndef _PROTOCOL2_H_
#define _PROTOCOL2_H_

struct BACKSTAGE_PROTOCOL_TYPE2      				// 
{
	u8 command;			// 控制命令
	u16 length;				// 数据长度
	u8 Device;				// 设备类型
	u8 DeviceNum;				// 本机地址
	u8 AgeAddress;			//拷机地址
	u8 DataBuffer[300];	// 数据接收缓冲
};

 u8 ProtocolResolve2(u8 receivedata);	// 协议解析
 void CommandProcess2(void);
 void CommandProcess2Prepare(void);
 extern void PCurve_Save(void);
 extern void P1SCurve_Save(u8 position);
#endif
