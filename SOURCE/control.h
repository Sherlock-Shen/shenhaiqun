/*************************************************************
*�ļ����ƣ�control.h
*�������ڣ�2012��05��08��
*������ڣ�
*��    �ߣ�gy
**************************************************************/

#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "global.h"

extern u16 CtrlTimeDelay;
extern u16 StoreTimeDelay;
extern u8 CtrlTimeout;
extern u8 StoreTimeout;
extern u8 CtrlOutput(u8 *switch_flag);
extern void StoreOutput(void);
#endif
