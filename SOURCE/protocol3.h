/*************************************************************
*文件名称：protocol3.h
*创建日期：2018年11月16日
*完成日期：
*作    者：shq
**************************************************************/
#include "global.h"
#ifndef _PROTOCOL3_H_
#define _PROTOCOL3_H_

// 对后台私有协议结构体
struct BACKSTAGE_PROTOCOL_TYPE3      				// 
{
	u8 DataBuffer[300];	// 数据接收缓冲
};

extern struct BACKSTAGE_PROTOCOL_TYPE3 BackstageProtocol3;
extern struct BACKSTAGE_PROTOCOL_TYPE3 BackstageProtocol3bak;

void IEC_104_Resolve(u8 receivedata);	// 协议解析
void IEC_104_CommandProcess(u8 USARTxChannel);
void IEC_104_CommandProcessPrepare(void);

#endif
