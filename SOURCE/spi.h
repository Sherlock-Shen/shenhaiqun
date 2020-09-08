#ifndef __SPI_H
#define __SPI_H
#include "global.h"
// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

#define SPI_FLAG_RXNE           SPI_I2S_FLAG_RXNE
#define SPI_FLAG_TXE            SPI_I2S_FLAG_TXE

//SPI1初始化
void SPI1_Init(void);
//SPI1读写一字节数据
u8 SPI1_ReadWrite(u8 writedat);
//SPI2初始化
void SPI2_Init(void);
//SPI2读写一字节数据
u8 SPI2_ReadWrite(u8 writedat);

extern void SSN_L(void);
extern void SSN_H(void);
#endif

