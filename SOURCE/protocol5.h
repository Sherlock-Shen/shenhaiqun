/*************************************************************
*�ļ����ƣ�protocol5.h
*�������ڣ�2018��11��13��
*������ڣ�
*��    �ߣ�shq
**************************************************************/
#include "global.h"
#ifndef _PROTOCOL5_H_
#define _PROTOCOL5_H_

struct BACKSTAGE_PROTOCOL_TYPE5      				// 
{
	u8 command;			// ��������
	u16 length;				// ���ݳ���
	u8 DeviceNum;				// ������ַ
	u8 DataBuffer[650];	// ���ݽ��ջ���
};

extern u32 UpdateLength;
extern void Heart_Jump(void);
extern u8 ProtocolResolve5(u8 receivedata);	// Э�����
extern u8 WifiConnectCtrl(void);
extern u8 ProtocolResolveWIFIError(u8 receivedata);
extern void CommandProcess5(void);
extern void CommandProcess5Prepare(void);
#endif
