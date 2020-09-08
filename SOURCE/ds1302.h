
#ifndef _ds1302_H
#define _ds1302_H
#include "global.h"

#define     time_i                GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6)
#define     time_o_0              GPIO_ResetBits(GPIOE,GPIO_Pin_6)         //Êä³ö
#define     time_o_1              GPIO_SetBits(GPIOE,GPIO_Pin_6)
#define 	  time_clk_0					  GPIO_ResetBits(GPIOE,GPIO_Pin_5)	       //Êä³ö
#define 	  time_clk_1					  GPIO_SetBits(GPIOE,GPIO_Pin_5)
#define 	  time_ce_0					    GPIO_ResetBits(GPIOC,GPIO_Pin_13)	       //Êä³ö
#define 	  time_ce_1					    GPIO_SetBits(GPIOC,GPIO_Pin_13)
extern void  read_time(void);
extern void  write_time(void);
extern void  correct_time(void);
extern void  ds1302_init(void);
#endif
