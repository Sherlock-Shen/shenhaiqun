/*************************************************************
*文件名称：control.h
*创建日期：2012年05月08日
*完成日期：
*作    者：gy
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
