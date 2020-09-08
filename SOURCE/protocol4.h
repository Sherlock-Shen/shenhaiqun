/*************************************************************
*文件名称：protocol4.h
*创建日期：2018年11月13日
*完成日期：
*作    者：shq
**************************************************************/
#ifndef _PROTOCOL4_H_
#define _PROTOCOL4_H_
#include "global.h"

struct BACKSTAGE_PROTOCOL_TYPE4      				// 
{
	u8 command;			// 控制命令
	u16 length;				// 数据长度
	u8 Device;				// 本机地址
	u8 DataBuffer[300];	// 数据接收缓冲
};

u8 ProtocolResolve4(u8 receivedata);	// 协议解析
void CommandProcess4(u8 USARTxChannel);
void CommandProcess4Prepare(void);

#endif
