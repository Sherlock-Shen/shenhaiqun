#ifndef _AD7606_H_
#define _AD7606_H_
#include "global.h"	
	
#define AD7606_CONVST_0            GPIO_ResetBits(GPIOD,GPIO_Pin_12)	//输出
#define AD7606_CONVST_1            GPIO_SetBits(GPIOD,GPIO_Pin_12)	//输出
#define AD7606_RESET_0             GPIO_ResetBits(GPIOD,GPIO_Pin_11)	//输出
#define AD7606_RESET_1             GPIO_SetBits(GPIOD,GPIO_Pin_11)	//输出


void AD7606_delay_US(u32 nCount); 
extern void AD7606START(void);
extern void AD7606Reset(void);
extern void AD7606Read(void);
void AD7606ReadBuffer(u16* pBuffer);
#endif


