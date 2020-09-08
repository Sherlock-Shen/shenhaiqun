/*************************************************************
*�ļ����ƣ�2812_FFT.h
*�������ڣ�2012��09��04��
*������ڣ�
*��    �ߣ�sjm
**************************************************************/

#ifndef _2812_FFT_H_
#define _2812_FFT_H_
#include "global.h"

struct Harmonic
{
	u16 V_all;
	u16 V3;
	u16 V5;
	u16 V7;
	u16 V9;
	u16 V11;
	u16 I_all;
	u16 I3;
	u16 I5;
	u16 I7;
	u16 I9;
	u16 I11;
};

extern void MeasureHarmonicPercent(void);
extern void Measure_FastHarmonic_Percent(void);
extern u16 FJBUA,FJBUB,FJBUC,FJBIA,FJBIB,FJBIC,FJBU0,FJBI0,FJBUAB;           //FFT����ķ�ֵ
extern u16 FDEGUA,FDEGUB,FDEGUC,FDEGIA,FDEGIB,FDEGIC,FDEGU0,FDEGI0,FDEGUAB,FDEGUPT1,FDEGUPT2;  //FFT������Ļ����νǶ�
extern u16   FXBZUA,FXBZUB,FXBZUC,FXBZIA,FXBZIB,FXBZIC,FXBZU0,FXBZI0,FXBZUAB;
extern u16 FXB2IA,FXB2IB,FXB2IC;

#endif
