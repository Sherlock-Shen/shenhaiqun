/*************************************************************
*�ļ����ƣ�protocol2.h
*�������ڣ�2018��11��16��
*������ڣ�
*��    �ߣ�shq
**************************************************************/
#include "global.h"
#ifndef _PROTOCOL2_H_
#define _PROTOCOL2_H_

struct BACKSTAGE_PROTOCOL_TYPE2      				// 
{
	u8 command;			// ��������
	u16 length;				// ���ݳ���
	u8 Device;				// �豸����
	u8 DeviceNum;				// ������ַ
	u8 AgeAddress;			//������ַ
	u8 DataBuffer[300];	// ���ݽ��ջ���
};

 u8 ProtocolResolve2(u8 receivedata);	// Э�����
 void CommandProcess2(void);
 void CommandProcess2Prepare(void);
 extern void PCurve_Save(void);
 extern void P1SCurve_Save(u8 position);
#endif
