/*************************************************************
*文件名称：GPS.h
*创建日期：2018.11.16| 13 November 2018 
*完成日期：
*作    者：S.H.Q.
**************************************************************/
#include "global.h"
#ifndef _GPS_H_
#define _GPS_H_

struct GPS_INFO
{
	double time;
	u8     state;
	double longitude; //纬度
	u8     NS;
	double latitude; //经度
	u8     EW;
	float  speed; //速度
	float  direction; //航向
	u32    date;
	u8 DataBuffer[100];
};

extern struct GPS_INFO Gps;
extern struct GPS_INFO GpsBak;
extern u8 GPS_FLAG;
extern u8 ProtocolResolve_Gps(u8 receivedata);	// 协议解析
void CommandProcess_Gps(void);
void CommandProcess_GpsPrepare(void);

#endif
