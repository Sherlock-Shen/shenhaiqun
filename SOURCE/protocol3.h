/*************************************************************
*�ļ����ƣ�protocol3.h
*�������ڣ�2018��11��16��
*������ڣ�
*��    �ߣ�shq
**************************************************************/
#include "global.h"
#ifndef _PROTOCOL3_H_
#define _PROTOCOL3_H_

// �Ժ�̨˽��Э��ṹ��
struct BACKSTAGE_PROTOCOL_TYPE3      				// 
{
	u8 DataBuffer[300];	// ���ݽ��ջ���
};

extern struct BACKSTAGE_PROTOCOL_TYPE3 BackstageProtocol3;
extern struct BACKSTAGE_PROTOCOL_TYPE3 BackstageProtocol3bak;

void IEC_104_Resolve(u8 receivedata);	// Э�����
void IEC_104_CommandProcess(u8 USARTxChannel);
void IEC_104_CommandProcessPrepare(void);

#endif
