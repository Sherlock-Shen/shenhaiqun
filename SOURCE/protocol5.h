/*************************************************************
*文件名称：protocol5.h
*创建日期：2018年11月13日
*完成日期：
*作    者：shq
**************************************************************/
#include "global.h"
#ifndef _PROTOCOL5_H_
#define _PROTOCOL5_H_

struct BACKSTAGE_PROTOCOL_TYPE5      				// 
{
	u8 command;			// 控制命令
	u16 length;				// 数据长度
	u8 DeviceNum;				// 本机地址
	u8 DataBuffer[650];	// 数据接收缓冲
};

extern u32 UpdateLength;
extern void Heart_Jump(void);
extern u8 ProtocolResolve5(u8 receivedata);	// 协议解析
extern u8 WifiConnectCtrl(void);
extern u8 ProtocolResolveWIFIError(u8 receivedata);
extern void CommandProcess5(void);
extern void CommandProcess5Prepare(void);
#endif
