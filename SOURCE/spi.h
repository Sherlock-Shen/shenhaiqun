#ifndef __SPI_H
#define __SPI_H
#include "global.h"
// SPI�����ٶ����� 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

#define SPI_FLAG_RXNE           SPI_I2S_FLAG_RXNE
#define SPI_FLAG_TXE            SPI_I2S_FLAG_TXE

//SPI1��ʼ��
void SPI1_Init(void);
//SPI1��дһ�ֽ�����
u8 SPI1_ReadWrite(u8 writedat);
//SPI2��ʼ��
void SPI2_Init(void);
//SPI2��дһ�ֽ�����
u8 SPI2_ReadWrite(u8 writedat);

extern void SSN_L(void);
extern void SSN_H(void);
#endif

