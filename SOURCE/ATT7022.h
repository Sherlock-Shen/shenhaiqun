/*************************************************************
*文件名称：ATT7022.h
*创建日期：2013年08月29日
*完成日期：
*作    者：sjm
**************************************************************/

#ifndef _ATT7022_H_
#define _ATT7022_H_
#include "global.h"

/***********************电度表芯片******************************/
#define     ATT7022_SEL		      GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12)     // 电度表芯片三相三线制和三相四线制转换信号，0-三相三线，1-三相四线
#define     ATT7022_SIG		      GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_13)     // 电度表芯片就绪信号，上电为高，等待25ms变为低后可以对其初始化
#define     ATT_SDO		          GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)    
#define     ATT_CS_1  	    	  GPIO_SetBits(GPIOG,GPIO_Pin_15)
#define     ATT_CS_0  	    	  GPIO_ResetBits(GPIOG,GPIO_Pin_15)
#define     ATT_SCLK_1		      GPIO_SetBits(GPIOG,GPIO_Pin_14)
#define     ATT_SCLK_0		      GPIO_ResetBits(GPIOG,GPIO_Pin_14)
#define     ATT_SDI_1		        GPIO_SetBits(GPIOB,GPIO_Pin_4)  
#define     ATT_SDI_0		        GPIO_ResetBits(GPIOB,GPIO_Pin_4)  

//  ----- Measurement Reg ----
#define  PA            0x01
#define  PB            0x02
#define  PC            0x03
#define  PT            0x04
#define  QA	           0x05
#define  QB            0x06
#define  QC            0x07
#define  QT            0x08
#define  SA	           0x09
#define  SB            0x0A
#define  SC            0x0B
#define  ST            0x0C
#define  UARMS         0x0D
#define  UBRMS         0x0E
#define  UCRMS         0x0F
#define  IARMS         0x10
#define  IBRMS         0x11
#define  ICRMS         0x12
#define  ITRMS	       0x13
#define  PFA           0x14
#define  PFB           0x15
#define  PFC           0x16
#define  PFT           0x17
#define  PGA           0x18
#define  PGB           0x19
#define  PGC           0x1A
#define  INTFLAG       0x1B
#define  FREQ          0x1C
#define  EFLAG         0x1D
#define  EPA	         0x1E
#define  EPB           0x1F
#define  EPC           0x20
#define  EPT           0x21
#define  EQA           0x22
#define  EQB           0x23
#define  EQC           0x24
#define  EQT           0x25

#define  YUAUB         0x26
#define  YUAUC         0x27
#define  YUBUC         0x28

#define  I0RMS         0x29
#define  TPSD	         0x2A
#define  URMST	       0x2B
#define  SFLAG         0x2C
#define  BCKREG	       0x2D
#define  COMCHKSUM     0x2E


#define  SAMPLEIA      0x2F
#define  SAMPLEIB	     0x30
#define  SAMPLEIC      0x31
#define  SAMPLEUA      0x32
#define  SAMPLEUB	     0x33
#define  SAMPLEUC      0x34
#define  ESA           0x35
#define  ESB           0x36
#define  ESC     	     0x37
#define  EST           0x38

#define  FSTCNTA       0x39
#define  FSTCNTB       0x3A
#define  FSTCNTC 	     0x3B
#define  FSTCNTT       0x3C
#define  PFLAG         0x3D
#define  CHKSUM        0x3E
#define  SAMPLEI0	     0x3F


#define  LINEPA	       0x40
#define  LINEPB	       0x41
#define  LINEPC	       0x42
#define  LINEPT	       0x43
#define  LINEEPA	     0x44
#define  LINEEPB	     0x45
#define  LINEEPC	     0x46
#define  LINEEPT	     0x47

#define  LINEUARRMS	   0x48
#define  LINEUBRRMS	   0x49
#define  LINEUCRRMS	   0x4A
#define  LINEIARRMS	   0x4B
#define  LINEIBRRMS	   0x4C
#define  LINEICRRMS	   0x4D

#define  LEFLAG  	     0x4E

#define  PTRWAVEBUFF   0x7E
#define  WAVEBUFF  	   0x7F


extern void WriteAT7052(u8 Command, u32 dat);
extern u32 ReadAT7052(u8 Command);
extern void XBAT7022(void);

extern void ATT7022_Clear(void);
extern void ATT7022_WREnable(void);
extern void ATT7022_WRDisable(void);
extern void ATT7022_RDEnable(void);
extern void ATT7022_RDDisable(void);
extern void ATT7022_Config(void);
extern void ATT7022_Reset(void);
extern void ATT7022_Init(void);
#endif
