/*************************************************************
*�ļ����ƣ�protocol4.h
*�������ڣ�2018��11��13��
*������ڣ�
*��    �ߣ�shq
**************************************************************/
#ifndef _PROTOCOL4_H_
#define _PROTOCOL4_H_
#include "global.h"

struct BACKSTAGE_PROTOCOL_TYPE4      				// 
{
	u8 command;			// ��������
	u16 length;				// ���ݳ���
	u8 Device;				// ������ַ
	u8 DataBuffer[300];	// ���ݽ��ջ���
};

u8 ProtocolResolve4(u8 receivedata);	// Э�����
void CommandProcess4(u8 USARTxChannel);
void CommandProcess4Prepare(void);

#endif
