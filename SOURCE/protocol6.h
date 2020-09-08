/*************************************************************
*文件名称：protocol6.h
*创建日期：2016年04月08日
*完成日期：
*作    者：sjm
**************************************************************/

#ifndef _PROTOCOL6_H_
#define _PROTOCOL6_H_
#include "global.h"

struct BACKSTAGE_PROTOCOL_TYPE6      				// 
{
	u8 command;			// 控制命令
	u16 length;				// 数据长度
	u8 Device;				// 本机地址
	u8 DataBuffer[300];	// 数据接收缓冲
};

 u8 ProtocolResolve6(u8 receivedata);	// 协议解析
 void CommandProcess6(void);
 void CommandProcess6Prepare(void);

#endif
