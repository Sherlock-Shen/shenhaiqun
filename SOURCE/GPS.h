/*************************************************************
*�ļ����ƣ�GPS.h
*�������ڣ�2018.11.16| 13 November 2018 
*������ڣ�
*��    �ߣ�S.H.Q.
**************************************************************/
#include "global.h"
#ifndef _GPS_H_
#define _GPS_H_

struct GPS_INFO
{
	double time;
	u8     state;
	double longitude; //γ��
	u8     NS;
	double latitude; //����
	u8     EW;
	float  speed; //�ٶ�
	float  direction; //����
	u32    date;
	u8 DataBuffer[100];
};

extern struct GPS_INFO Gps;
extern struct GPS_INFO GpsBak;
extern u8 GPS_FLAG;
extern u8 ProtocolResolve_Gps(u8 receivedata);	// Э�����
void CommandProcess_Gps(void);
void CommandProcess_GpsPrepare(void);

#endif
