/*************************************************************
*�ļ����ƣ�protocol6.h
*�������ڣ�2016��04��08��
*������ڣ�
*��    �ߣ�sjm
**************************************************************/

#ifndef _PROTOCOL6_H_
#define _PROTOCOL6_H_
#include "global.h"

struct BACKSTAGE_PROTOCOL_TYPE6      				// 
{
	u8 command;			// ��������
	u16 length;				// ���ݳ���
	u8 Device;				// ������ַ
	u8 DataBuffer[300];	// ���ݽ��ջ���
};

 u8 ProtocolResolve6(u8 receivedata);	// Э�����
 void CommandProcess6(void);
 void CommandProcess6Prepare(void);

#endif
