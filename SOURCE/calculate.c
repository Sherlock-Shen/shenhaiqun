//###########################################################################
//
// 数据计算处理文件
//
// 
//
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  12.08| 27 Aug 2012 | S.J.M. | 
//###########################################################################

#include "global.h"
#include "2812_FFT.h"
#include "math.h"
#include "AD7606.h"  

static u32 Display_UAB_sum=0;
static u32 Display_UBC_sum=0;
static u32 Display_UCA_sum=0;
static u32 Display_UA_sum=0;
static u32 Display_UB_sum=0;
static u32 Display_UC_sum=0;
static u32 Display_U0_sum=0;
static u32 Display_IA_sum=0;
static u32 Display_IB_sum=0;
static u32 Display_IC_sum=0;
static u32 Display_IA10_sum=0;
static u32 Display_IB10_sum=0;
static u32 Display_IC10_sum=0;
static u32 Display_I0_sum=0;
static u32 Display_UA1_sum=0;
//static u32 Display_UB1_sum=0;
static u32 Display_UC1_sum=0;
u8 under_voltage_on = 0;

/***************new add groud protect***/
s16 iuxw;         // U0用来判断的相位，I0与U0的相位差。
u8  jdyf;         // 接地与否
u8   zxdjdfs;     // 中性点接地方式
s16 u0pdxw;       // 电压与电流之间的相位
u8   jdxb;        // 接地相别
u8   dycfzc;      // 电源侧/负载侧
u8   dycfzc2;     // 电源侧/负载侧2
u8   jdxz;        // 接地性质
u8   hgxb;        // 弧光相别
u16  hggdy;       // 弧光过电压
/***************************************/

const float CosValList[91] = {1.0,
0.9998476951563913,0.9993908270190958,0.9986295347545738,0.9975640502598242,0.9961946980917455,
0.9945218953682733,0.992546151641322,0.9902680687415704,0.9876883405951378,0.984807753012208,
0.981627183447664,0.9781476007338057,0.9743700647852352,0.9702957262759965,0.9659258262890683,
0.9612616959383189,0.9563047559630355,0.9510565162951535,0.9455185755993168,0.9396926207859084,
0.9335804264972017,0.9271838545667874,0.9205048534524404,0.9135454576426009,0.9063077870366499,
0.898794046299167,0.8910065241883679,0.882947592858927,0.8746197071393957,0.8660254037844387,
0.8571673007021123,0.848048096156426,0.838670567945424,0.8290375725550417,0.8191520442889918,
0.8090169943749474,0.7986355100472928,0.7880107536067219,0.7771459614569709,0.766044443118978,
0.754709580222772,0.7431448254773942,0.7313537016191705,0.7193398003386512,0.7071067811865476,
0.6946583704589974,0.6819983600624985,0.6691306063588582,0.6560590289905074,0.6427876096865394,
0.6293203910498375,0.6156614753256583,0.6018150231520484,0.5877852522924731,0.5735764363510462,
0.5591929034707468,0.5446390350150272,0.5299192642332049,0.5150380749100544,0.5000000000000001,
0.4848096202463371,0.46947156278589086,0.4539904997395468,0.43837114678907746,0.42261826174069944,
0.4067366430758004,0.3907311284892737,0.3746065934159122,0.35836794954530015,0.3420201433256688,
0.32556815445715675,0.30901699437494745,0.29237170472273677,0.27563735581699916,0.25881904510252074,
0.24192189559966767,0.22495105434386514,0.20791169081775923,0.19080899537654491,0.17364817766693041,
0.15643446504023092,0.13917310096006546,0.12186934340514749,0.10452846326765346,0.08715574274765836,
0.06975647374412523,0.052335956242943966,0.03489949670250108,0.0174524064372836,0.0 
}; 

u32 SqrtINT32U(u32 radicand) 	/* 32 位数的开方*/
{
  u32 rem = 0, root = 0, divisor = 0;
  u16  i;
  for(i=0; i<16; i++)
  {
    root <<= 1;
    rem = ((rem << 2) + (radicand>>30));
    radicand <<= 2;
    divisor = (root << 1) + 1;
    if(divisor <= rem)
    {
      rem -= divisor;
      root++;
    }
  }
  return root;
}

 
/****************************************************************************
*
*名    称：u16 CosValConvert(float val)
*
*功    能：将功率因数（余弦值）转换成时间（角度）
*
*入口参数：余弦值
*
*返回参数：时间，放大十倍，单位ms
*
****************************************************************************/
u16 CosValConvert(float val)
{
	u8 i = 0;
	u8 temp = 0;
	u16 time = 0;
	if(val < 0)
		val = -val;
	for(i=0;i<90;i++)
	{
		if(val<=CosValList[i] && val>CosValList[i+1])
			temp = i;
	}
	time = CosValList[temp]*1000;
	return time;
}

void MeasureDataCalcAndProcess(void)
{
	static u8 state = 0;
	static u8 restart_cnt = 0;	
	u8 i = 0;
	static u8 voltage_flag1 = 0;
	static u8 voltage_flag2 = 0;
	static u8 fastoffcnt = 0;	// 电流速切滤波
	static u8 fastoffcnt1 = 0;// 电流速切滤波1
	static u8 fastoffcnt2 = 0;// 电流速切滤波2
	static u8 fastoffcnt3 = 0;// 电流速切滤波3
	static u8 Overcurrent_flag = 0;
	static u8 getpower_flag = 0;
	static u8 losspower_flag = 0;
	if(MeasureReady == 0)
	{
		restart_cnt++;
	}
	else if(MeasureReady == 1)
	{
		restart_cnt = 0;
	}
	
	if(restart_cnt > 20)
	{
    AD7606Reset();      // ad7606复位
	}

	switch(state)
	{
		case 0:			// 清零
			if(MeasureReady == 1)
			{
				MeasureData.UAB_temp = 0;
				MeasureData.UBC_temp = 0;
		 		MeasureData.UCA_temp = 0;				
				MeasureData.UA_temp = 0;
				MeasureData.UB_temp = 0;
		 		MeasureData.UC_temp = 0;
		 		MeasureData.U0_temp = 0;
		 		MeasureData.IA_temp = 0;
		 		MeasureData.IB_temp = 0;
		 		MeasureData.IC_temp = 0;
		 		MeasureData.PIA_temp = 0;
		 		MeasureData.PIB_temp = 0;
		 		MeasureData.PIC_temp = 0;
		 		MeasureData.I0_temp = 0;		
				MeasureData.UA1_temp = 0;
				MeasureData.UB1_temp = 0;
		 		MeasureData.UC1_temp = 0;
				
				MeasureData.UAB_temp = SqrtINT32U(MeasureData.UABsum/100);
				MeasureData.Protect_UAB_val = ((float)MeasureData.UAB_temp*(float)ProtectModulus.para.UAB)/(DOWN_U_MODULUS);
				
				MeasureData.UBC_temp = SqrtINT32U(MeasureData.UBCsum/100);
				MeasureData.Protect_UBC_val = ((float)MeasureData.UBC_temp*(float)ProtectModulus.para.UBC)/(DOWN_U_MODULUS);
				
				MeasureData.UCA_temp = SqrtINT32U(MeasureData.UCAsum/100);
				MeasureData.Protect_UCA_val = ((float)MeasureData.UCA_temp*(float)ProtectModulus.para.UCA)/(DOWN_U_MODULUS);
				
				MeasureData.UA_temp = SqrtINT32U(MeasureData.UAsum/100);
				MeasureData.Protect_UA_val = ((float)MeasureData.UA_temp*(float)ProtectModulus.para.UA)/DOWN_U_MODULUS;
				
				MeasureData.UB_temp = SqrtINT32U(MeasureData.UBsum/100);
				MeasureData.Protect_UB_val = ((float)MeasureData.UB_temp*(float)ProtectModulus.para.UB)/(DOWN_U_MODULUS);
				
				MeasureData.UC_temp = SqrtINT32U(MeasureData.UCsum/100);
				MeasureData.Protect_UC_val = ((float)MeasureData.UC_temp*(float)ProtectModulus.para.UC)/(DOWN_U_MODULUS);
				
				MeasureData.U0_temp = SqrtINT32U(MeasureData.U0sum/100);
				MeasureData.Protect_U0_val = ((float)MeasureData.U0_temp*(float)ProtectModulus.para.U0)/DOWN_U0_MODULUS;
				
				MeasureData.I0_temp = SqrtINT32U(MeasureData.I0sum/100);
				MeasureData.Protect_I0_val = (((float)MeasureData.I0_temp*(float)ProtectModulus.para.I0)/DOWN_I0_MODULUS);
								
				MeasureData.IA_temp = SqrtINT32U(MeasureData.IAsum/100);
				MeasureData.Protect_IA_val = (((float)MeasureData.IA_temp*(float)ProtectModulus.para.IA)/UP_I_MODULUS); 
				
				MeasureData.PIA_temp = SqrtINT32U(MeasureData.PIAsum/100);
				MeasureData.Protect_IA10_val = (((float)MeasureData.PIA_temp*(float)ProtectModulus.para.IA10)/DOWN_I_MODULUS);
				
				MeasureData.IB_temp = SqrtINT32U(MeasureData.IBsum/100);
				MeasureData.Protect_IB_val = (((float)MeasureData.IB_temp*(float)ProtectModulus.para.IB)/UP_I_MODULUS);
				
				MeasureData.PIB_temp = SqrtINT32U(MeasureData.PIBsum/100);
				MeasureData.Protect_IB10_val = (((float)MeasureData.PIB_temp*(float)ProtectModulus.para.IB10)/DOWN_I_MODULUS);
				
				MeasureData.IC_temp = SqrtINT32U(MeasureData.ICsum/100);
				MeasureData.Protect_IC_val = (((float)MeasureData.IC_temp*(float)ProtectModulus.para.IC)/UP_I_MODULUS);
				
				MeasureData.PIC_temp = SqrtINT32U(MeasureData.PICsum/100);
				MeasureData.Protect_IC10_val = (((float)MeasureData.PIC_temp*(float)ProtectModulus.para.IC10)/DOWN_I_MODULUS);

				MeasureData.IA_temp = SqrtINT32U(MeasureData.IASDsum/50);
				MeasureData.Protect_IASD_val = (((float)MeasureData.IA_temp*(float)ProtectModulus.para.IA)/UP_I_MODULUS);
				
				MeasureData.PIA_temp = SqrtINT32U(MeasureData.PIASDsum/50);
				MeasureData.Protect_IA10SD_val = (((float)MeasureData.PIA_temp*(float)ProtectModulus.para.IA10)/DOWN_I_MODULUS);
				
				MeasureData.IB_temp = SqrtINT32U(MeasureData.IBSDsum/50);
				MeasureData.Protect_IBSD_val = (((float)MeasureData.IB_temp*(float)ProtectModulus.para.IB)/UP_I_MODULUS);
				
				MeasureData.PIB_temp = SqrtINT32U(MeasureData.PIBSDsum/50);
				MeasureData.Protect_IB10SD_val = (((float)MeasureData.PIB_temp*(float)ProtectModulus.para.IB10)/DOWN_I_MODULUS);
				
				MeasureData.IC_temp = SqrtINT32U(MeasureData.ICSDsum/50);
				MeasureData.Protect_ICSD_val = (((float)MeasureData.IC_temp*(float)ProtectModulus.para.IC)/UP_I_MODULUS);
				
				MeasureData.PIC_temp = SqrtINT32U(MeasureData.PICSDsum/50);
				MeasureData.Protect_IC10SD_val = (((float)MeasureData.PIC_temp*(float)ProtectModulus.para.IC10)/DOWN_I_MODULUS);
				
				MeasureData.I0_10_temp = SqrtINT32U(MeasureData.PI0sum/100);
				MeasureData.Protect_10I0_val = (((float)MeasureData.I0_10_temp*(float)ProtectModulus.para.I010)/DOWN_I10_MODULUS);

				MeasureData.UA1_temp = SqrtINT32U(MeasureData.UA1sum/100);
				MeasureData.Protect_UA1_val = ((float)MeasureData.UA1_temp*(float)ProtectModulus.para.UA1)/UP_U_MODULUS;
				
				MeasureData.UC1_temp = SqrtINT32U(MeasureData.UB1sum/100);
				MeasureData.Protect_UC1_val = ((float)MeasureData.UC1_temp*(float)ProtectModulus.para.UC1)/UP_U_MODULUS;
				
				if(MeasureData.Protect_UA_val > AlarmSet.para.min_voltage || 
			    MeasureData.Protect_UB_val > AlarmSet.para.min_voltage || 
			    MeasureData.Protect_UC_val > AlarmSet.para.min_voltage)
				{
					under_voltage_on = 1;
				}
				//A侧有压延时
				if(MeasureData.Protect_UA_val > AutoswitchSet.para.voltage_value*57)
				{
					if(ProtectDelay.para.voltage_value_A == 0 && ProtectTimeout.para.voltage_value_A == 0)
					{
						ProtectDelay.para.voltage_value_A = AutoswitchSet.para.voltage_value_time;  //有压延时时间
					}
			  }
				else	// 延时过程中失压消除
				{
					if(ProtectDelay.para.voltage_value_A > 0 && ProtectTimeout.para.voltage_value_A == 0)
					{
					  ProtectDelay.para.voltage_value_A = 0;
					}
				}
        //B侧有压延时
				if(MeasureData.Protect_UB_val > AutoswitchSet.para.voltage_value*57)
				{
					if(ProtectDelay.para.voltage_value_B == 0 && ProtectTimeout.para.voltage_value_B == 0)
					{
						ProtectDelay.para.voltage_value_B = AutoswitchSet.para.voltage_value_time;  //有压延时时间
					}
			  }
				else	// 延时过程中失压消除
				{
					if(ProtectDelay.para.voltage_value_B > 0 && ProtectTimeout.para.voltage_value_B == 0)
					{
					  ProtectDelay.para.voltage_value_B = 0;
					}
				}
				//A侧无压延时
				if(MeasureData.Protect_UA_val < AutoswitchSet.para.novoltage_value*57)
				{
					if(ProtectDelay.para.novoltage_value_A == 0 && ProtectTimeout.para.novoltage_value_A == 0)
					{
						ProtectDelay.para.novoltage_value_A = AutoswitchSet.para.novoltage_value_time;  //无压延时时间
					}
			  }
				else	// 延时过程中失压消除
				{
					if(ProtectDelay.para.novoltage_value_A > 0 && ProtectTimeout.para.novoltage_value_A == 0)
					{
					  ProtectDelay.para.novoltage_value_A = 0;
					}
				}
        //B侧无压延时
				if(MeasureData.Protect_UB_val < AutoswitchSet.para.novoltage_value*57)
				{
					if(ProtectDelay.para.novoltage_value_B == 0 && ProtectTimeout.para.novoltage_value_B == 0)
					{
						ProtectDelay.para.novoltage_value_B = AutoswitchSet.para.novoltage_value_time;  //无压延时时间
					}
			  }
				else	// 延时过程中失压消除
				{
					if(ProtectDelay.para.novoltage_value_B > 0 && ProtectTimeout.para.novoltage_value_B == 0)
					{
					  ProtectDelay.para.novoltage_value_B = 0;
					}
				}
				
				if(ProtectTimeout.para.voltage_value_A == 1 && ProtectTimeout.para.voltage_value_B == 1)
				{
					voltage_flag1 = 0x11;
				  voltage_flag2 = 0x11;
				}
				else if(ProtectTimeout.para.voltage_value_A == 1 && ProtectTimeout.para.novoltage_value_B == 1)
				{
					if(voltage_flag1 == 0x11)
						voltage_flag1 = 0x66;
					else if(voltage_flag1 == 0x44)
						voltage_flag1 = 0x33;
					voltage_flag2 = 0x22;
				}
				else if(ProtectTimeout.para.novoltage_value_A == 1 && ProtectTimeout.para.voltage_value_B == 1)
				{							
					if(voltage_flag1 == 0x11)
						voltage_flag1 = 0x55;
					else if(voltage_flag1 == 0x44)
						voltage_flag1 = 0x22;
					voltage_flag2 = 0x33;
				}
				else if(ProtectTimeout.para.novoltage_value_A == 1 && ProtectTimeout.para.novoltage_value_B == 1)
				{
					voltage_flag1 = 0x44;
				  if(voltage_flag2 == 0x11 || voltage_flag2 == 0x22 || voltage_flag2 == 0x33)
						voltage_flag2 = 0x44;
				}
				
				Display_UAB_sum+=MeasureData.Protect_UAB_val;
				Display_UBC_sum+=MeasureData.Protect_UBC_val;
				Display_UCA_sum+=MeasureData.Protect_UCA_val;
				Display_UA_sum+=MeasureData.Protect_UA_val;
				Display_UB_sum+=MeasureData.Protect_UB_val;
				Display_UC_sum+=MeasureData.Protect_UC_val;
				Display_U0_sum+=MeasureData.Protect_U0_val;
				Display_IA_sum+=MeasureData.Protect_IA_val;
				Display_IB_sum+=MeasureData.Protect_IB_val;
				Display_IC_sum+=MeasureData.Protect_IC_val;
				Display_IA10_sum+=MeasureData.Protect_IA10_val;
				Display_IB10_sum+=MeasureData.Protect_IB10_val;
				Display_IC10_sum+=MeasureData.Protect_IC10_val;
				Display_I0_sum+=MeasureData.Protect_I0_val;
				Display_UA1_sum+=MeasureData.Protect_UA1_val;
				//Display_UB1_sum+=MeasureData.Protect_UB1_val;
				Display_UC1_sum+=MeasureData.Protect_UC1_val;

				MeasureData.Display_UAB_val = Display_UAB_sum>>4;
				MeasureData.Display_UBC_val = Display_UBC_sum>>4;
				MeasureData.Display_UCA_val = Display_UCA_sum>>4;
				MeasureData.Display_UA_val = Display_UA_sum>>2;
				MeasureData.Display_UB_val = Display_UB_sum>>2;
				MeasureData.Display_UC_val = Display_UC_sum>>2;
				MeasureData.Display_U0_val = Display_U0_sum>>2;
				MeasureData.Display_IA_val = Display_IA_sum>>2;
				MeasureData.Display_IB_val = Display_IB_sum>>2;
				MeasureData.Display_IC_val = Display_IC_sum>>2;
				MeasureData.Display_IA10_val = Display_IA10_sum>>2;
				MeasureData.Display_IB10_val = Display_IB10_sum>>2;
				MeasureData.Display_IC10_val = Display_IC10_sum>>2;
				MeasureData.Display_I0_val = Display_I0_sum>>2;
				MeasureData.Display_UA1_val = Display_UA1_sum>>2;
				//MeasureData.Display_UB1_val = Display_UB1_sum>>2;
				MeasureData.Display_UC1_val = Display_UC1_sum>>2;

				Display_UAB_sum-=MeasureData.Display_UAB_val;
				Display_UBC_sum-=MeasureData.Display_UBC_val;
				Display_UCA_sum-=MeasureData.Display_UCA_val;
				Display_UA_sum-=MeasureData.Display_UA_val;
				Display_UB_sum-=MeasureData.Display_UB_val;
				Display_UC_sum-=MeasureData.Display_UC_val;
				Display_U0_sum-=MeasureData.Display_U0_val;
				Display_IA_sum-=MeasureData.Display_IA_val;
				Display_IB_sum-=MeasureData.Display_IB_val;
				Display_IC_sum-=MeasureData.Display_IC_val;
				Display_IA10_sum-=MeasureData.Display_IA10_val;
				Display_IB10_sum-=MeasureData.Display_IB10_val;
				Display_IC10_sum-=MeasureData.Display_IC10_val;
				Display_I0_sum-=MeasureData.Display_I0_val;
				Display_UA1_sum-=MeasureData.Display_UA1_val;
				//Display_UB1_sum-=MeasureData.Display_UB1_val;
				Display_UC1_sum-=MeasureData.Display_UC1_val;

        MeasureData.Display_degUAB_val = FDEGUAB;
				MeasureData.Display_degUA_val = FDEGUA; 
				MeasureData.Display_degUB_val = FDEGUB;
				MeasureData.Display_degUC_val = FDEGUC;
				MeasureData.Display_degU0_val = FDEGU0;
				MeasureData.Display_degIA_val = FDEGIA;
				MeasureData.Display_degIB_val = FDEGIB;
				MeasureData.Display_degIC_val = FDEGIC;
				MeasureData.Display_degI0_val = FDEGI0;
        if(FDEGUA < 18000)
				  MeasureData.Display_UB1_val = abs(FDEGUA+24000-FDEGUB);//第二个PT侧C相电压存两个PT相位差
        else
					MeasureData.Display_UB1_val = abs(FDEGUA-12000-FDEGUB);
				
				JDPD();//接地判断
				
				if((ZeroPointIA > (MeasureData.IA_temp/2)) && (MeasureData.Protect_IA_val > 500))	// 有直流分量
				{
					MeasureIAError = 1;
				}
				else if((ZeroPointIA10 > (MeasureData.PIA_temp/2)) && (MeasureData.Protect_IA10_val > 50))	
				{
					MeasureIAError = 1;
				}
				else
				{
					if(abs(MeasureData.Protect_IA_val - MeasureData.Protect_IA10_val*10) > 500)
					{
						if(MeasureData.Protect_IA_val < 5000 || MeasureData.Protect_IA10_val < 500)
						{
							MeasureIAError = 1;
						}
						else
							MeasureIAError = 0;
					}
					else
						MeasureIAError = 0;
				}
				KZQMeasureData.para.SelfCheck.bit.currentA = MeasureIAError;
				if((ZeroPointIB > (MeasureData.IB_temp/2)) && (MeasureData.Protect_IB_val > 500))	// 有直流分量
				{
					MeasureIBError = 1;
				}
				else if((ZeroPointIB10 > (MeasureData.PIB_temp/2)) && (MeasureData.Protect_IB10_val > 50))	
				{
					MeasureIBError = 1;
				}
				else
				{
					if(abs(MeasureData.Protect_IB_val - MeasureData.Protect_IB10_val*10) > 500)
					{
						if(MeasureData.Protect_IB_val < 5000 || MeasureData.Protect_IB10_val < 500)
						{
							MeasureIBError = 1;
						}
						else
							MeasureIBError = 0;
					}
					else
						MeasureIBError = 0;
				}
				KZQMeasureData.para.SelfCheck.bit.currentB = MeasureIBError;
				if((ZeroPointIC > (MeasureData.IC_temp/2)) && (MeasureData.Protect_IC_val > 500))	// 有直流分量
				{
					MeasureICError = 1;
				}
				else if((ZeroPointIC10 > (MeasureData.PIC_temp/2)) && (MeasureData.Protect_IC10_val > 50))	
				{
					MeasureICError = 1;
				}
				else
				{
					if(abs(MeasureData.Protect_IC_val - MeasureData.Protect_IC10_val*10) > 500)
					{
						if(MeasureData.Protect_IC_val < 5000 || MeasureData.Protect_IC10_val < 500)
						{
							MeasureICError = 1;
						}
						else
							MeasureICError = 0;
					}
					else
						MeasureICError = 0;
				}
				KZQMeasureData.para.SelfCheck.bit.currentC = MeasureICError;
				
//				MeasureIAError = 0;
//				MeasureIBError = 0;
//				MeasureICError = 0;

				MeasureReady =0;
				if(RamTest()==0x02)	// 内部内存出错，停止保护判断
					break;
				
				if(KZQMeasureData.para.DLQflag.bit.breaker == 2 && ParaInitReadyFlag == 1 && AutoswitchSet.para.segment_contact_mode)
					state = 1;
				else if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && ParaInitReadyFlag == 1 && AutoswitchSet.para.segment_contact_mode)
					state = 2;
				else if(ParaInitReadyFlag == 1 && MeasureData.YaBan1 == 1)//断路器位置不判了，总保护压板使能
					state = 3; 
				else if(ParaInitReadyFlag == 1)//断路器位置不判了，报警判断
					state = 4;				
				else
				  state = 0;
			}
			break;
		case 1:
			//电压型馈线自动化
			if((AutoswitchSet.para.segment_contact_mode & 0x03) && KZQMeasureData.para.DLQflag.bit.breaker == 2) // 断路器分位
			{
				if(AutoswitchSet.para.segment_contact_mode == 0x02)
				{
					if(ProtectTimeout.para.voltage_value_A == 1 && ProtectTimeout.para.voltage_value_B == 1)//联络模式两侧有电，闭锁合闸
					{
						Switchon_Lock1 = 1;//合闸闭锁
					}
					else
						Switchon_Lock1 = 0;//合闸解除闭锁
				}
        else if(AutoswitchSet.para.segment_contact_mode == 0x01)		
				  Switchon_Lock1 = 0;//合闸解除闭锁						

			  //电源侧，负载侧得电延时合闸
				if(AutoswitchSet.para.getpower_enable_A == 0x01 && AutoswitchSet.para.getpower_enable_B == 0x00)
				{
					
					if(voltage_flag1 == 0x33)// A侧得压合闸
					{
						if(ProtectDelay.para.get_power == 0 && ProtectTimeout.para.get_power == 0)
						{
							getpower_flag = 0x01;
							ProtectDelay.para.get_power = AutoswitchSet.para.get_power_time;  //得电延时时间
						}
					}
				}
				else if(AutoswitchSet.para.getpower_enable_A == 0x00 && AutoswitchSet.para.getpower_enable_B == 0x01)
				{				
					if(voltage_flag1 == 0x22)// B侧得压合闸
					{
						if(ProtectDelay.para.get_power == 0 && ProtectTimeout.para.get_power == 0)
						{
							getpower_flag = 0x02;
							ProtectDelay.para.get_power = AutoswitchSet.para.get_power_time;  //得电延时时间
						}
					}
				}
				else if(AutoswitchSet.para.getpower_enable_A == 0x01 && AutoswitchSet.para.getpower_enable_B == 0x01)
				{							
					if(voltage_flag1 == 0x22 || voltage_flag1 == 0x33)// A侧或B侧得压合闸
					{
						if(ProtectDelay.para.get_power == 0 && ProtectTimeout.para.get_power == 0)
						{
							if(voltage_flag1 == 0x33)// A侧得压合闸
								getpower_flag = 0x01;
							else if(voltage_flag1 == 0x22)// B侧得压合闸
						    getpower_flag = 0x02;
							ProtectDelay.para.get_power = AutoswitchSet.para.get_power_time;  //得电延时时间
							ProtectTimeout.para.get_power = 0;
						}
					}
				}
								
				//单侧电压失电，在一定时间内检测到故障残压，闭锁合闸
				if(AutoswitchSet.para.lock_resvoltage_enable ==1)
				{
					if(ProtectDelay.para.single_loss > 0 && ProtectTimeout.para.single_loss == 0)
					{
						if(voltage_flag1 == 0x55)// A侧失压合闸
						{
							if(MeasureData.Protect_UA_val > 1400)//残压固定为25%
							{
								Switchon_Lock2_Flag = 1;  //合闸闭锁标志
							}
						}
						else if(voltage_flag1 == 0x66)// B侧失压合闸
						{
							if(MeasureData.Protect_UB_val > 1400)//残压固定为25%
							{
								Switchon_Lock2_Flag = 1;  //合闸闭锁标志
							}
						}
					}
				}
				
				//双侧电压规定时间后，单侧电压消失延时合闸
				if(AutoswitchSet.para.single_loss_enable_A ==1 && AutoswitchSet.para.single_loss_enable_B ==0)
				{					
					if(voltage_flag1 == 0x55)// A侧失压合闸
					{
						if(ProtectDelay.para.single_loss == 0 && ProtectTimeout.para.single_loss == 0)
						{
							losspower_flag = 0x01;
							ProtectDelay.para.single_loss = AutoswitchSet.para.single_loss_time;
						}
					}
				}
				else if(AutoswitchSet.para.single_loss_enable_A ==0 && AutoswitchSet.para.single_loss_enable_B ==1)
				{						
					if(voltage_flag1 == 0x66)// B侧失压合闸
					{
						if(ProtectDelay.para.single_loss == 0 && ProtectTimeout.para.single_loss == 0)
						{
							losspower_flag = 0x02;
							ProtectDelay.para.single_loss = AutoswitchSet.para.single_loss_time;
						}
					}
				}
				else if(AutoswitchSet.para.single_loss_enable_A ==1 && AutoswitchSet.para.single_loss_enable_B ==1)
				{									
					if(voltage_flag1 == 0x55 || voltage_flag1 == 0x66)// A侧或B侧失压合闸
					{
						if(ProtectDelay.para.single_loss == 0 && ProtectTimeout.para.single_loss == 0)
						{
							if(voltage_flag1 == 0x55)
							  losspower_flag = 0x01;
							else if(voltage_flag1 == 0x66)
							  losspower_flag = 0x02;
							ProtectDelay.para.single_loss = AutoswitchSet.para.single_loss_time;
						}
					}
				}
			}

		  if(ProtectTimeout.para.get_power != 0)
			{
				if(Switchon_Lock == 0 && Switchon_Lock1 == 0 && Switchon_Lock2 == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2) // 断路器分位
				{
					KZQMeasureData.para.ProtectFlag1.bit.get_power=1;
					RecordAutofeed(TYPE_PROTECT_GET_POWER);
					if(SwitchFlag == 0)
					{
						SwitchFlag = 1;					// 置合闸标志
						if(getpower_flag == 0x01)
						  RecordSOE_Pad(SOE_ADDR_GETPOWER_A_pad,1);
						else if(getpower_flag == 0x02)
						  RecordSOE_Pad(SOE_ADDR_GETPOWER_B_pad,1);
					}
				}
				getpower_flag = 0;
				voltage_flag1 = 0;
				ProtectTimeout.para.get_power = 0;
			}
      else if(ProtectTimeout.para.single_loss != 0)
			{
				if(Switchon_Lock == 0 && Switchon_Lock1 == 0 && Switchon_Lock2 == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2)// 断路器分位
				{
					KZQMeasureData.para.ProtectFlag1.bit.single_loss=1;
					RecordAutofeed(TYPE_PROTECT_SINGLE_LOSS);
					if(SwitchFlag == 0)
					{
						SwitchFlag = 1;					// 置合闸标志
						if(losspower_flag == 0x01)
						  RecordSOE_Pad(SOE_ADDR_LOSSPOWER_A_pad,1);
						else if(losspower_flag == 0x02)
						  RecordSOE_Pad(SOE_ADDR_LOSSPOWER_B_pad,1);
					}
				}
				losspower_flag = 0;
				voltage_flag1 = 0;			
		    ProtectTimeout.para.single_loss = 0;
		  }
			state = 0; 
		  break;
		case 2:
			//电压型馈线自动化
			if((AutoswitchSet.para.segment_contact_mode & 0x03) && KZQMeasureData.para.DLQflag.bit.breaker == 1) // 断路器合位
			{		
				if(AutoswitchSet.para.inrush_current_enable == 1)
				{
					if(fastoffcnt > 0)
					  Measure_FastHarmonic_Percent();		// FFT
					if(FXB2IA > AutoswitchSet.para.inrush_current_value*100 || FXB2IB > AutoswitchSet.para.inrush_current_value*100 ||
						 FXB2IC > AutoswitchSet.para.inrush_current_value*100)
					{
						Switchoff_Lock1 = 1;  //闭锁分闸1标志
					}
					else
					{
						fastoffcnt2 ++;
						if(fastoffcnt2 > 2)
						{
							Switchoff_Lock1 = 0;
							fastoffcnt2 = 0;
						}
					}
				}
				
				//非遮断电流保护
				if(AutoswitchSet.para.nonbreak_current_enable == 1)
				{
					if(AutoswitchSet.para.nonbreak_current_value < 1000)
					{
						if(MeasureData.Protect_IA_val > (AutoswitchSet.para.nonbreak_current_value*10) ||
							 MeasureData.Protect_IB_val > (AutoswitchSet.para.nonbreak_current_value*10) ||
							 MeasureData.Protect_IC_val > (AutoswitchSet.para.nonbreak_current_value*10))
						{
							Switchoff_Lock1 = 1;  //闭锁分闸1标志
						}
						else
						{
							fastoffcnt3 ++;
							if(fastoffcnt3 > 2)
							{
								Switchoff_Lock1 = 0;
								fastoffcnt3 = 0;
							}
						}
					}
					else
					{
						if(MeasureData.Protect_IA10_val > AutoswitchSet.para.nonbreak_current_value ||
							 MeasureData.Protect_IB10_val > AutoswitchSet.para.nonbreak_current_value ||
							 MeasureData.Protect_IC10_val > AutoswitchSet.para.nonbreak_current_value)
						{
							Switchoff_Lock1 = 1;  //闭锁分闸1标志
						}
						else
						{
							fastoffcnt3 ++;
							if(fastoffcnt3 > 2)
							{
								Switchoff_Lock1 = 0;
								fastoffcnt3 = 0;
							}
						}
					}
				}
					
			  if(StartCntFlag & 0x10) //合闸之后
				{
					if(ProtectDelay.para.err_current == 0 && ProtectTimeout.para.err_current == 0)
					{
						ProtectDelay.para.err_current = AutoswitchSet.para.err_current_time;
					}
					
					if(AutoswitchSet.para.phase_err_value <1000)
					{
						if(MeasureData.Protect_IA_val > (AutoswitchSet.para.phase_err_value*10) ||
							 MeasureData.Protect_IB_val > (AutoswitchSet.para.phase_err_value*10) ||
							 MeasureData.Protect_IC_val > (AutoswitchSet.para.phase_err_value*10))
						   {
							   KZQMeasureData.para.ProtectFlag2.bit.have_err = 1;
						     Overcurrent_flag |= 0x01;
							 }
					}
					else
					{
						if(MeasureData.Protect_IA10_val > AutoswitchSet.para.phase_err_value ||
							 MeasureData.Protect_IB10_val > AutoswitchSet.para.phase_err_value ||
							 MeasureData.Protect_IC10_val > AutoswitchSet.para.phase_err_value)
							 {
							   KZQMeasureData.para.ProtectFlag2.bit.have_err = 1;
						     Overcurrent_flag |= 0x01;
							 }
					}
					
					if(AutoswitchSet.para.ground_err_value <2100)
					{
					  if(MeasureData.Protect_I0_val > (AutoswitchSet.para.ground_err_value*10))
						{
						  KZQMeasureData.para.ProtectFlag2.bit.have_err = 1;
						  Overcurrent_flag |= 0x02;
						}
					}
					else
					{
						if(MeasureData.Protect_10I0_val > AutoswitchSet.para.ground_err_value)
						{
						  KZQMeasureData.para.ProtectFlag2.bit.have_err = 1;
						  Overcurrent_flag |= 0x02;
						}
					}
								
          if(StartCntFlag & 0x08) //合到故障电流判据
					{
						if(voltage_flag2 == 0x44)
						{
							KZQMeasureData.para.ProtectFlag2.bit.have_err = 1;
							if(StartCntFlag & 0x01)
							{
								if(Overcurrent_flag)
								{
								  Switchon_Lock = 1;
									KZQMeasureData.para.ProtectFlag1.bit.switchon_lock=1;
									KZQMeasureData.para.RequestFlag1.bit.record_autofeed = 1;
									RecordAutofeed(TYPE_PROTECT_SWITCHON_LOCK);
									if(Overcurrent_flag &0x01)
									{
										RecordSOE_Pad(SOE_ADDR_PHASE_ERR_pad,1); // 相间故障
									}
									if(Overcurrent_flag &0x02)
									{
										RecordSOE_Pad(SOE_ADDR_GROUND_ERR_pad,1);// 接地故障
									}
									StartCntFlag &= 0xEF;
									Overcurrent_flag = 0;
								}
							}
							if(StartCntFlag & 0x02)
							{
								if(ProtectDelay.para.err_fastoff == 0 && ProtectTimeout.para.err_fastoff == 0)
								{
									ProtectDelay.para.err_fastoff = AutoswitchSet.para.err_fastoff_time;
								}
							}
					  }
					}
					else
					{
						if(voltage_flag2 == 0x44)
						{
							KZQMeasureData.para.ProtectFlag2.bit.have_err = 1;
							if(StartCntFlag & 0x01)
							{
								Switchon_Lock = 1;
								KZQMeasureData.para.ProtectFlag1.bit.switchon_lock=1;
								KZQMeasureData.para.RequestFlag1.bit.record_autofeed = 1;
								RecordAutofeed(TYPE_PROTECT_SWITCHON_LOCK);
								StartCntFlag &= 0xEF;
							}
							if(StartCntFlag & 0x02)
							{
								if(ProtectDelay.para.err_fastoff == 0 && ProtectTimeout.para.err_fastoff == 0)
								{
									ProtectDelay.para.err_fastoff = AutoswitchSet.para.err_fastoff_time;	
								}
							}
					  }
					}
				}
				
				//双侧失压，无流，延时时间t分闸
				if(AutoswitchSet.para.losspower_enable == 1)
				{
					if(voltage_flag2 == 0x44 &&MeasureData.Protect_IA_val < 500 && MeasureData.Protect_IB_val < 500 && MeasureData.Protect_IC_val < 500)
					{
						if(ProtectDelay.para.loss_power == 0 && ProtectTimeout.para.loss_power == 0)
						{
							ProtectDelay.para.loss_power = AutoswitchSet.para.loss_power_time;  //失电延时时间
						}
					}
			  }
				
				//零序电压保护
				if(AutoswitchSet.para.zero_voltage_enable)
				{
					if(MeasureData.Protect_U0_val > 2000)
					{
						if(ProtectDelay.para.zero_voltage == 0 && ProtectTimeout.para.zero_voltage == 0)
						{
							ProtectDelay.para.zero_voltage = 60;  //零序电压保护延时时间固定为0.6s
						}
					}
				}
      }
			
			if(ProtectTimeout.para.err_current != 0) //故障检测时限
			{
				if((StartCntFlag & 0x04) && KZQMeasureData.para.ProtectFlag2.bit.have_err == 0)
				{
					Switchoff_Lock = 1;  //分闸闭锁
					SigresetLock = 1;    //闭锁复归计时开始标志
					KZQMeasureData.para.ProtectFlag1.bit.switchoff_lock=1;
					KZQMeasureData.para.RequestFlag1.bit.record_autofeed = 1;
					RecordAutofeed(TYPE_PROTECT_SWITCHOFF_LOCK);
				}
				StartCntFlag = 0;
				voltage_flag2 = 0;
				ProtectTimeout.para.err_current = 0;
			}
			else if(ProtectTimeout.para.err_fastoff != 0)
			{
				if(KZQMeasureData.para.DLQflag.bit.breaker == 1)
				{
					if(StartCntFlag & 0x08)
					{
						if(Switchoff_Lock == 1)
					    RecordSOE_Pad(SOE_ADDR_VOLTAGE_CURRENT2_pad,1);
						else
							RecordSOE_Pad(SOE_ADDR_VOLTAGE_CURRENT1_pad,1);
					}
					else
						RecordSOE_Pad(SOE_ADDR_VOLTAGE_TIME_pad,1);
					if(Switchoff_Lock == 0 && SwitchFlag == 0)
					{
						KZQMeasureData.para.ProtectFlag1.bit.err_fastoff=1;
						RecordAutofeed(TYPE_PROTECT_ERR_FASTOFF);
						SwitchFlag = 2;					// 置分闸标志
					}
				}
				voltage_flag2 = 0;
				ProtectTimeout.para.err_fastoff = 0;
			}
			else if(ProtectTimeout.para.loss_power != 0)
			{
				if(Switchoff_Lock == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 1) // 断路器合位
				{
					KZQMeasureData.para.ProtectFlag1.bit.loss_power=1;
					if(SwitchFlag == 0)
					{
						SwitchFlag = 2;					// 置自动分闸标志
						KZQMeasureData.para.ProtectFlag1.bit.loss_power=1;
						RecordAutofeed(TYPE_PROTECT_LOSS_POWER);
						RecordSOE_Pad(SOE_ADDR_LOSS_POWER_pad,1);
					}
				}
				voltage_flag2 = 0;
				ProtectTimeout.para.loss_power = 0;
			}
      else if(ProtectTimeout.para.zero_voltage != 0)
			{
       if(KZQMeasureData.para.DLQflag.bit.breaker == 1) // 断路器合位
				{
					if(AutoswitchSet.para.zero_voltage_enable == 1 && KZQMeasureData.para.ProtectFlag1.bit.zero_voltage == 0)
					{
						KZQMeasureData.para.ProtectFlag1.bit.zero_voltage=1;
						KZQMeasureData.para.RequestFlag1.bit.record_autofeed = 1;
						RecordSOE_Pad(SOE_ADDR_ZERO_VOLTAGE1_pad,1);
						RecordAutofeed(TYPE_PROTECT_ZERO_VOLTAGE2);
					}
					else if(AutoswitchSet.para.zero_voltage_enable == 2 && KZQMeasureData.para.ProtectFlag1.bit.zero_voltage == 0)
					{
						if(Switchoff_Lock == 0 && SwitchFlag == 0)
					  {
							KZQMeasureData.para.ProtectFlag1.bit.zero_voltage=1;
							RecordSOE_Pad(SOE_ADDR_ZERO_VOLTAGE2_pad,1);
							RecordAutofeed(TYPE_PROTECT_ZERO_VOLTAGE1);
						  SwitchFlag = 2;					// 置自动分闸标志
					  }
					}
				}
				ProtectTimeout.para.zero_voltage = 0;
			}				
			if(MeasureData.YaBan1 == 1 && Switchoff_Lock == 0 && Switchoff_Lock1 == 0)
			  state = 3; 
			else
				state = 4;
			break;
		case 3:
			if(MeasureData.Protect_U0_val > ProtectSet.para.zero_max_voltage)	// 零序过压
			{
				if(ProtectDelay.para.zero_max_voltage == 0 && ProtectTimeout.para.zero_max_voltage == 0
				//	  && (ProtectTimesLock&0x0001)==0 && ProtectSet.para.zero_max_voltage_enable == 1 && MeasureData.YaBan2 == 1)
				&& (ProtectTimesLock&0x0001)==0 && ProtectSet.para.zero_max_voltage_enable == 1)
				{
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					{
						if(ProtectSet.para.zero_max_voltage_time>0)
						  ProtectDelay.para.zero_max_voltage = ProtectSet.para.zero_max_voltage_time;	
						else 
						  ProtectDelay.para.zero_max_voltage = 1;
						ProtectTimeout.para.zero_max_voltage = 0;
						
						if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.zero_max_voltage == 0)
						{
							MeasureData.Pstartflag = 1;      
						}
					}
					ProtectTimesLockCnt = 0;
				}
			}
			else	// 延时过程中保护消除
			{  
				KZQMeasureData.para.ProtectFlag1.bit.zero_max_voltage=0;
				if(ProtectDelay.para.zero_max_voltage > 0 && ProtectTimeout.para.zero_max_voltage == 0)
				{
					MeasureData.Psaveflag = 0;
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					{
						ProtectDelay.para.zero_max_voltage = 0;
					}
				}
				ProtectTimesLock &=~0x0001;
			}

      //MeasureData.Protect_I0_val/1000为实际一次电流值     
      if(ProtectSet.para.zero_max_current < 2100)
		  {
				if(MeasureData.Protect_I0_val > ProtectSet.para.zero_max_current*10 
					 && (MeasureIAError == 0 && MeasureIBError == 0 && MeasureICError == 0))	// 零序过流
				{    
					fastoffcnt1++;
					if(ProtectDelay.para.zero_max_current == 0 && ProtectTimeout.para.zero_max_current == 0 && fastoffcnt1>2
						 //&& (ProtectTimesLock&0x0002)==0 && ProtectSet.para.zero_max_current_enable == 1 && MeasureData.YaBan2 == 1)
             && (ProtectTimesLock&0x0002)==0 && ProtectSet.para.zero_max_current_enable == 1) 					
					{   
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1 ) 
						{
							if(ProtectSet.para.zero_max_current_time > 0 )
								ProtectDelay.para.zero_max_current = ProtectSet.para.zero_max_current_time;
							else 
								ProtectDelay.para.zero_max_current = 1;
							ProtectTimeout.para.zero_max_current = 0;
							if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.zero_max_current == 0)
						  {
								MeasureData.Pstartflag = 1;      
						  }
						}
						ProtectTimesLockCnt = 0;
					}
				}
	  		else    // 延时过程中保护消除
	     	{   
					fastoffcnt1 = 0;		
					KZQMeasureData.para.ProtectFlag1.bit.zero_max_current=0;
					
					if(ProtectDelay.para.zero_max_current > 0 && ProtectTimeout.para.zero_max_current == 0)
					{
						MeasureData.Psaveflag = 0;
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						{
							ProtectDelay.para.zero_max_current = 0;
						}
					}
					ProtectTimesLock &=~0x0002;
	      }
      }
		  else
		  {
				if(MeasureData.Protect_10I0_val > ProtectSet.para.zero_max_current
					 && (MeasureIAError == 0 && MeasureIBError == 0 && MeasureICError == 0))	// 零序过流
				{  
          fastoffcnt1++;								
					if(ProtectDelay.para.zero_max_current == 0 && ProtectTimeout.para.zero_max_current == 0  && fastoffcnt1>2
					    //&& (ProtectTimesLock&0x0002)==0 && ProtectSet.para.zero_max_current_enable == 1 && MeasureData.YaBan2 == 1)
             && (ProtectTimesLock&0x0002)==0 && ProtectSet.para.zero_max_current_enable == 1) 	
					{   
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1 ) 
						{
							if(ProtectSet.para.zero_max_current_time > 0 )
								ProtectDelay.para.zero_max_current = ProtectSet.para.zero_max_current_time;
							else 
								ProtectDelay.para.zero_max_current = 1;
							ProtectTimeout.para.zero_max_current = 0;
							if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.zero_max_current == 0)
						  {
								MeasureData.Pstartflag = 1;      
						  }
						}
						ProtectTimesLockCnt = 0;
					}
				}
	  		else    // 延时过程中保护消除
	     	{
					fastoffcnt1 = 0;	
				  KZQMeasureData.para.ProtectFlag1.bit.zero_max_current=0;
					
					if(ProtectDelay.para.zero_max_current > 0 && ProtectTimeout.para.zero_max_current == 0)
					{
						MeasureData.Psaveflag = 0;
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						{
							ProtectDelay.para.zero_max_current = 0;
						}
					}
					ProtectTimesLock &=~0x0002;
	   	  }
		  }

			if((MeasureData.Protect_UA_val < 3000 || MeasureData.Protect_UB_val < 3000 || MeasureData.Protect_UC_val < 3000)
					&&MeasureData.Protect_U0_val>3000 &&jdyf==0)	//缺相
			{
				if(ProtectDelay.para.no_phase == 0 && ProtectTimeout.para.no_phase == 0 
					 && (ProtectTimesLock&0x0004)==0 && ProtectSet.para.phaseloss_protect_enable==1)
				{
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断合位
					{
						if( ReOnLockCnt >= 10 && ReOnLockCnt1 >= 15 )//保护重合闸后再次故障加速保护
						{
							if( ProtectSet.para.phaseloss_protect_time > 0 )
							{
								ProtectDelay.para.no_phase = ProtectSet.para.phaseloss_protect_time;	
							}
							else 
							{
									ProtectDelay.para.no_phase = 1;
							}
						}
						else
						{
							if( ProtectSet.para.phaseloss_protect_time > 0 )
							{
								ProtectDelay.para.no_phase = ProtectSet.para.phaseloss_protect_time/2;
							}	
								else 
							{
								ProtectDelay.para.no_phase = 1;
							}
						} 
						ProtectTimeout.para.no_phase = 0;
						if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.no_phase == 0)
						{
							MeasureData.Pstartflag = 1;      
						}
					}
					ProtectTimesLockCnt = 0;
				}
			}
			else	// 延时过程中保护消除
			{   
				KZQMeasureData.para.ProtectFlag1.bit.no_phase=0;
				
				if(ProtectDelay.para.no_phase > 0 && ProtectTimeout.para.no_phase == 0)
				{
					MeasureData.Psaveflag = 0;
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					{
						ProtectDelay.para.no_phase = 0;
					}
				}
				ProtectTimesLock &=~0x0004;
			}

//  		if(jdyf==1)//电压判断接地
//      {    
//				if((ProtectTimesLock&0x0008)==0 && ProtectDelay.para.ground == 0 && ProtectTimeout.para.ground == 0 && MeasureData.YaBan2 == 1) 
//				{   
//					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1 ) 
//					{
//						if(ProtectSet.para.zero_max_voltage_time > 0 )
//							ProtectDelay.para.ground = AlarmSet.para.alarmU0_time;
//						else 
//							ProtectDelay.para.ground = 1;
//						ProtectTimeout.para.ground = 0;
//					}
//					ProtectTimesLockCnt = 0;
//				}
//      }
//  		else 
//     	{
//      	KZQMeasureData.para.ProtectFlag1.bit.ground=0;
//      	if(ProtectDelay.para.ground > 0 && ProtectTimeout.para.ground == 0)
//				{
//					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
//					{
//						ProtectDelay.para.ground = 0;
//					}
//				}
//        ProtectTimesLock &=~0x0008;
//   		}

			if(ProtectSet.para.fast_off < 1000)	// 低于额定电流，取一倍电流
			{
				if((MeasureData.Protect_IASD_val > (ProtectSet.para.fast_off*10) && MeasureIAError == 0) ||
					 (MeasureData.Protect_IBSD_val > (ProtectSet.para.fast_off*10) && MeasureIBError == 0) ||
					 (MeasureData.Protect_ICSD_val > (ProtectSet.para.fast_off*10) && MeasureICError == 0))		// 速断
				{
					if((ProtectSet.para.fast_off_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA == 0 && KZQMeasureData.para.DLQflag.bit.directionC == 0)) ||	// 正向
						 (ProtectSet.para.fast_off_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA == 1 && KZQMeasureData.para.DLQflag.bit.directionC == 1)) ||	// 反向
						ProtectSet.para.fast_off_direction_lock == 0)
					{
						if((ProtectSet.para.fast_off_voltage_lock == 1 && (MeasureData.Protect_UAB_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UBC_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UCA_val < ProtectSet.para.min_voltage_lock)) ||	// 低压闭锁
							ProtectSet.para.fast_off_voltage_lock == 0)
						{
							fastoffcnt++;
							if(ProtectTimeout.para.fast_off == 0 && fastoffcnt > 3 && ProtectSet.para.fast_off_enable == 1 && 
								(ProtectTimesLock&0x0010)==0 && MeasureData.YaBan3 == 1)
							{
								//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
									ProtectTimeout.para.fast_off = 1;
								if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.fast_off == 0)
								{
									MeasureData.Pstartflag = 1;      
								}
								ProtectTimesLockCnt = 0;
							}
//							if(ProtectTimeout.para.fast_off == 0 && fastoffcnt > 3 && ProtectSet.para.fast_off_enable == 1 && MeasureData.YaBan3 == 1)
//							{
//								if(MeasureData.Protect_IASD_val > ProtectSet.para.fast_off*11.5 ||//11.5
//									 MeasureData.Protect_IBSD_val > ProtectSet.para.fast_off*11.5 ||
//									 MeasureData.Protect_ICSD_val > ProtectSet.para.fast_off*11.5)
//								{
//									//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
//										ProtectTimeout.para.fast_off = 1;
//								}
//								ProtectTimesLockCnt = 0;	
//							}						
						}
						if((ProtectSet.para.fast_off_voltage_lock == 1 && (MeasureData.Protect_UAB_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UBC_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UCA_val > ProtectSet.para.min_voltage_lock)))	// 低压闭锁
						{
							fastoffcnt = 0;
						}
					}
					if(ProtectSet.para.fast_off_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA != 0 || KZQMeasureData.para.DLQflag.bit.directionC != 0))
					{
						fastoffcnt = 0;
					}
					else if(ProtectSet.para.fast_off_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA != 1 || KZQMeasureData.para.DLQflag.bit.directionC != 1))
					{
						fastoffcnt = 0;
					}
				}
				else
				{
					KZQMeasureData.para.ProtectFlag1.bit.fast_off=0;
					fastoffcnt = 0;
					ProtectTimesLock &=~0x0010;
					//MeasureData.Psaveflag = 0;
				}				
			}
			else // 高于额定电流，取十倍电流
			{  
				if((MeasureData.Protect_IA10SD_val > ProtectSet.para.fast_off && MeasureIAError == 0) ||
					 (MeasureData.Protect_IB10SD_val > ProtectSet.para.fast_off && MeasureIBError == 0) ||
					 (MeasureData.Protect_IC10SD_val > ProtectSet.para.fast_off && MeasureICError == 0))		// 速断
				{
					if((ProtectSet.para.fast_off_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA == 0 && KZQMeasureData.para.DLQflag.bit.directionC == 0)) ||	// 正向
						(ProtectSet.para.fast_off_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA == 1 && KZQMeasureData.para.DLQflag.bit.directionC == 1)) ||	// 反向
						ProtectSet.para.fast_off_direction_lock == 0)
					{
						if((ProtectSet.para.fast_off_voltage_lock == 1 && (MeasureData.Protect_UAB_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UBC_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UCA_val < ProtectSet.para.min_voltage_lock)) ||	// 低压闭锁
							ProtectSet.para.fast_off_voltage_lock == 0)
						{
							fastoffcnt++;
							if(ProtectTimeout.para.fast_off == 0 && fastoffcnt > 3 && ProtectSet.para.fast_off_enable == 1 && 
								(ProtectTimesLock&0x0010)==0 && MeasureData.YaBan3 == 1)
							{
								//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
									ProtectTimeout.para.fast_off = 1;
								if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.fast_off == 0)
								{
									MeasureData.Pstartflag = 1;      
								}
								ProtectTimesLockCnt = 0;
							}
//							if(ProtectTimeout.para.fast_off == 0 && fastoffcnt > 3 && ProtectSet.para.fast_off_enable == 1 && MeasureData.YaBan3 == 1)
//							{
//								if(MeasureData.Protect_IA10SD_val > ProtectSet.para.fast_off*1.15 ||
//									 MeasureData.Protect_IB10SD_val > ProtectSet.para.fast_off*1.15 ||
//									 MeasureData.Protect_IC10SD_val > ProtectSet.para.fast_off*1.15)
//								{
//									//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
//										ProtectTimeout.para.fast_off = 1;
//								}	
//								ProtectTimesLockCnt = 0;
//							}						
						}
						if((ProtectSet.para.fast_off_voltage_lock == 1 && (MeasureData.Protect_UAB_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UBC_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UCA_val > ProtectSet.para.min_voltage_lock)))	// 低压闭锁
						{
							fastoffcnt = 0;
						}
					}
					if(ProtectSet.para.fast_off_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA != 0 || KZQMeasureData.para.DLQflag.bit.directionC != 0))
					{
						fastoffcnt = 0;
					}
					else if(ProtectSet.para.fast_off_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA != 1 || KZQMeasureData.para.DLQflag.bit.directionC != 1))
					{
						fastoffcnt = 0;
					}
				}
				else
				{
					KZQMeasureData.para.ProtectFlag1.bit.fast_off=0;
					fastoffcnt = 0;
					ProtectTimesLock &=~0x0010;
					//MeasureData.Psaveflag = 0;
				}
			}
			
    if((ProtectSet.para.max_current_after_enable == 1 && Max_current_after_cnt > 3000) || ProtectSet.para.max_current_after_enable == 0)
		{
			if(ProtectSet.para.delay_fast_off < 1000)	// 低于额定电流，取一倍电流
			{
				if((MeasureData.Protect_IA_val > (ProtectSet.para.delay_fast_off*10) && MeasureIAError == 0) ||
					 (MeasureData.Protect_IB_val > (ProtectSet.para.delay_fast_off*10) && MeasureIBError == 0) ||
					 (MeasureData.Protect_IC_val > (ProtectSet.para.delay_fast_off*10) && MeasureICError == 0))	// 延时速断
				{
					if(ProtectDelay.para.delay_fast_off == 0 && ProtectTimeout.para.delay_fast_off == 0 && ProtectSet.para.delay_fast_off_enable == 1 && MeasureData.YaBan3 == 1)
					{
						if((ProtectSet.para.delay_fast_off_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA == 0 && KZQMeasureData.para.DLQflag.bit.directionC == 0)) ||	// 正向
							(ProtectSet.para.delay_fast_off_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA == 1 && KZQMeasureData.para.DLQflag.bit.directionC == 1)) ||	// 反向
							ProtectSet.para.delay_fast_off_direction_lock == 0)
						{
							if((ProtectSet.para.delay_fast_off_voltage_lock == 1 && (MeasureData.Protect_UAB_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UBC_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UCA_val < ProtectSet.para.min_voltage_lock))// 低压闭锁
							  ||ProtectSet.para.delay_fast_off_voltage_lock == 0)
							{
								//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
								if((ProtectTimesLock&0x0020)==0)
								{
									if(ProtectSet.para.delay_fast_off_time>0)ProtectDelay.para.delay_fast_off = ProtectSet.para.delay_fast_off_time;
									else ProtectDelay.para.delay_fast_off = 1;
									ProtectTimeout.para.delay_fast_off = 0;
									if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off == 0)
									{
										MeasureData.Pstartflag = 1;      
									}
								}
								ProtectTimesLockCnt = 0;							
							}
							if((ProtectSet.para.delay_fast_off_voltage_lock == 1 && (MeasureData.Protect_UAB_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UBC_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UCA_val > ProtectSet.para.min_voltage_lock)))	// 低压闭锁
							{
								ProtectDelay.para.delay_fast_off = 0;
							}
						}
						if(ProtectSet.para.delay_fast_off_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA != 0 || KZQMeasureData.para.DLQflag.bit.directionC != 0))
						{
							ProtectDelay.para.delay_fast_off = 0;
						}
						else if(ProtectSet.para.delay_fast_off_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA != 1 || KZQMeasureData.para.DLQflag.bit.directionC != 1))
						{
							ProtectDelay.para.delay_fast_off = 0;
						}
					}
				}
				else	// 延时过程中保护消除
				{ 
					KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off=0;
					
					if(ProtectDelay.para.delay_fast_off > 0 && ProtectTimeout.para.delay_fast_off == 0)
					{
						MeasureData.Psaveflag = 0;
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						{
							ProtectDelay.para.delay_fast_off = 0;
						}
					}
					ProtectTimesLock &=~0x0020;
				}				
			}
			else // 高于额定电流，取十倍电流
			{
				if((MeasureData.Protect_IA10_val > ProtectSet.para.delay_fast_off && MeasureIAError == 0) ||
					 (MeasureData.Protect_IB10_val > ProtectSet.para.delay_fast_off && MeasureIBError == 0) ||
					 (MeasureData.Protect_IC10_val > ProtectSet.para.delay_fast_off && MeasureICError == 0))	// 延时速断
				{
					if(ProtectDelay.para.delay_fast_off == 0 && ProtectTimeout.para.delay_fast_off == 0 && ProtectSet.para.delay_fast_off_enable == 1 && MeasureData.YaBan3 == 1)
					{
						if((ProtectSet.para.delay_fast_off_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA == 0 && KZQMeasureData.para.DLQflag.bit.directionC == 0)) ||	// 正向
							(ProtectSet.para.delay_fast_off_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA == 1 && KZQMeasureData.para.DLQflag.bit.directionC == 1)) ||	// 反向
							ProtectSet.para.delay_fast_off_direction_lock == 0)
						{
							if((ProtectSet.para.delay_fast_off_voltage_lock == 1 && (MeasureData.Protect_UAB_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UBC_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UCA_val < ProtectSet.para.min_voltage_lock))// 低压闭锁
							  ||ProtectSet.para.delay_fast_off_voltage_lock == 0)
							{
								//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
								if((ProtectTimesLock&0x0020)==0)
								{
									if(ProtectSet.para.delay_fast_off_time>0)ProtectDelay.para.delay_fast_off = ProtectSet.para.delay_fast_off_time;
									else ProtectDelay.para.delay_fast_off = 1;
									ProtectTimeout.para.delay_fast_off = 0;
									if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off == 0)
									{
										MeasureData.Pstartflag = 1;      
									}
								}
								ProtectTimesLockCnt = 0;							
							}
							if((ProtectSet.para.delay_fast_off_voltage_lock == 1 && (MeasureData.Protect_UAB_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UBC_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UCA_val > ProtectSet.para.min_voltage_lock)))	// 低压闭锁
							{
								ProtectDelay.para.delay_fast_off = 0;
							}
						
						}
						if(ProtectSet.para.delay_fast_off_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA != 0 || KZQMeasureData.para.DLQflag.bit.directionC != 0))
						{
							ProtectDelay.para.delay_fast_off = 0;
						}
						else if(ProtectSet.para.delay_fast_off_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA != 1 || KZQMeasureData.para.DLQflag.bit.directionC != 1))
						{
							ProtectDelay.para.delay_fast_off = 0;
						}
					}
				}
				else	// 延时过程中保护消除
				{
					KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off=0;
					
					if(ProtectDelay.para.delay_fast_off > 0 && ProtectTimeout.para.delay_fast_off == 0)
					{
						MeasureData.Psaveflag = 0;
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						{
							ProtectDelay.para.delay_fast_off = 0;
						}
					}
					ProtectTimesLock &=~0x0020;
				}
			}
		}

		if((ProtectSet.para.max_current_after_enable == 1 && Max_current_after_cnt > 3000) || ProtectSet.para.max_current_after_enable == 0)
		{
			if(ProtectSet.para.max_current < 1000)	// 低于额定电流，取一倍电流
			{
				if((MeasureData.Protect_IA_val > (ProtectSet.para.max_current*10) && MeasureIAError == 0) ||
					 (MeasureData.Protect_IB_val > (ProtectSet.para.max_current*10) && MeasureIBError == 0) ||
					 (MeasureData.Protect_IC_val > (ProtectSet.para.max_current*10) && MeasureICError == 0))	// 过流
				{
					if(ProtectDelay.para.max_current == 0 && ProtectTimeout.para.max_current == 0 && ProtectSet.para.max_current_enable == 1 && MeasureData.YaBan3 == 1)
					{
						if((ProtectSet.para.max_current_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA == 0 && KZQMeasureData.para.DLQflag.bit.directionC == 0)) ||	// 正向
							(ProtectSet.para.max_current_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA == 1 && KZQMeasureData.para.DLQflag.bit.directionC == 1)) ||	// 反向
							 ProtectSet.para.max_current_direction_lock == 0)
						{
							if((ProtectSet.para.max_current_voltage_lock == 1 && (MeasureData.Protect_UAB_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UBC_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UCA_val < ProtectSet.para.min_voltage_lock))// 低压闭锁
							  ||ProtectSet.para.max_current_voltage_lock == 0)
							{
								//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
								if((ProtectTimesLock&0x0040)==0)
								{
									if(ProtectSet.para.max_current_time>0)ProtectDelay.para.max_current = ProtectSet.para.max_current_time;
									else ProtectDelay.para.max_current = 1;
									ProtectTimeout.para.max_current = 0;
									if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.max_current == 0)
									{
										MeasureData.Pstartflag = 1;      
									}
								}
								ProtectTimesLockCnt = 0;							
							}
							if((ProtectSet.para.max_current_voltage_lock == 1 && (MeasureData.Protect_UAB_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UBC_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UCA_val > ProtectSet.para.min_voltage_lock)))	// 低压闭锁
							{
								ProtectDelay.para.max_current = 0;
							}
						}
						if(ProtectSet.para.max_current_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA != 0 || KZQMeasureData.para.DLQflag.bit.directionC != 0))
						{
							ProtectDelay.para.max_current = 0;
						}
						else if(ProtectSet.para.max_current_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA != 1 || KZQMeasureData.para.DLQflag.bit.directionC != 1))
						{
							ProtectDelay.para.max_current = 0;
						}	
					}
				}
				else	// 延时过程中保护消除
				{
					KZQMeasureData.para.ProtectFlag1.bit.max_current=0;
					
					if(ProtectDelay.para.max_current > 0 && ProtectTimeout.para.max_current == 0)
					{
						MeasureData.Psaveflag = 0;
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						{
							ProtectDelay.para.max_current = 0;
						}
					}
					ProtectTimesLock &=~0x0040;
				}				
			}
			else // 高于额定电流，取十倍电流
			{
				if((MeasureData.Protect_IA10_val > ProtectSet.para.max_current && MeasureIAError == 0) ||
					 (MeasureData.Protect_IB10_val > ProtectSet.para.max_current && MeasureIBError == 0) ||
					 (MeasureData.Protect_IC10_val > ProtectSet.para.max_current && MeasureICError == 0))	// 过流
				{
					if(ProtectDelay.para.max_current == 0 && ProtectTimeout.para.max_current == 0 && ProtectSet.para.max_current_enable == 1 && MeasureData.YaBan3 == 1)
					{
						if((ProtectSet.para.max_current_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA == 0 && KZQMeasureData.para.DLQflag.bit.directionC == 0)) ||	// 正向
							(ProtectSet.para.max_current_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA == 1 && KZQMeasureData.para.DLQflag.bit.directionC == 1)) ||	// 反向
							ProtectSet.para.max_current_direction_lock == 0)
						{
							if((ProtectSet.para.max_current_voltage_lock == 1 && (MeasureData.Protect_UAB_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UBC_val < ProtectSet.para.min_voltage_lock || MeasureData.Protect_UCA_val < ProtectSet.para.min_voltage_lock))// 低压闭锁
							  ||ProtectSet.para.max_current_voltage_lock == 0)
							{
								//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
								if((ProtectTimesLock&0x0040)==0)
								{
									if(ProtectSet.para.max_current_time>0)ProtectDelay.para.max_current = ProtectSet.para.max_current_time;
									else ProtectDelay.para.max_current = 1;
									ProtectTimeout.para.max_current = 0;
									if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.max_current == 0)
									{
										MeasureData.Pstartflag = 1;      
									}
								}
								ProtectTimesLockCnt = 0;							
							}
							if((ProtectSet.para.max_current_voltage_lock == 1 && (MeasureData.Protect_UAB_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UBC_val > ProtectSet.para.min_voltage_lock && MeasureData.Protect_UCA_val > ProtectSet.para.min_voltage_lock)))	// 低压闭锁
							{
								ProtectDelay.para.max_current = 0;
							}
						}
						if(ProtectSet.para.max_current_direction_lock == 1 && (KZQMeasureData.para.DLQflag.bit.directionA != 0 || KZQMeasureData.para.DLQflag.bit.directionC != 0))
						{
							ProtectDelay.para.max_current = 0;
						}
						else if(ProtectSet.para.max_current_direction_lock == 2 && (KZQMeasureData.para.DLQflag.bit.directionA != 1 || KZQMeasureData.para.DLQflag.bit.directionC != 1))
						{
							ProtectDelay.para.max_current = 0;
						}
					}
				}
				else	// 延时过程中保护消除
				{
					KZQMeasureData.para.ProtectFlag1.bit.max_current=0;
					
					if(ProtectDelay.para.max_current > 0 && ProtectTimeout.para.max_current == 0)
					{
						MeasureData.Psaveflag = 0;
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						{
							ProtectDelay.para.max_current = 0;
						}
					}
					ProtectTimesLock &=~0x0040;
				}
			}
		}

			if(ProtectSet.para.max_current_after < 1000)	// 低于额定电流，取一倍电流
			{
				if((MeasureData.Protect_IA_val > (ProtectSet.para.max_current_after*10) && MeasureIAError == 0) ||
					 (MeasureData.Protect_IB_val > (ProtectSet.para.max_current_after*10) && MeasureIBError == 0) ||
					 (MeasureData.Protect_IC_val > (ProtectSet.para.max_current_after*10) && MeasureICError == 0))	// 过流后加速
				{
					if(ProtectDelay.para.max_current_after == 0 && ProtectTimeout.para.max_current_after == 0 
						 && ProtectSet.para.max_current_after_enable == 1 && MeasureData.YaBan3 == 1 && Max_current_after_cnt < 3000)
					{
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && (ProtectTimesLock&0x0080)==0)	// 断路器合位
						{
							if(ProtectSet.para.max_current_after_time>0)ProtectDelay.para.max_current_after = ProtectSet.para.max_current_after_time;
							else ProtectDelay.para.max_current_after = 1;
							ProtectTimeout.para.max_current_after = 0;
							if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.max_current_after == 0)
							{
								MeasureData.Pstartflag = 1;      
							}
						}
						ProtectTimesLockCnt = 0;	
					}
				}
				else	// 延时过程中保护消除
				{
					KZQMeasureData.para.ProtectFlag1.bit.max_current_after=0;
					
					if(ProtectDelay.para.max_current_after > 0 && ProtectTimeout.para.max_current_after == 0)
					{
						MeasureData.Psaveflag = 0;
						ProtectDelay.para.max_current_after = 0;
					}
					ProtectTimesLock &=~0x0080;
				}				
			}
			else // 高于额定电流，取十倍电流
			{
				if((MeasureData.Protect_IA10_val > ProtectSet.para.max_current_after && MeasureIAError == 0) ||
					 (MeasureData.Protect_IB10_val > ProtectSet.para.max_current_after && MeasureIBError == 0) ||
					 (MeasureData.Protect_IC10_val > ProtectSet.para.max_current_after && MeasureICError == 0))	// 过流后加速
				{
					if(ProtectDelay.para.max_current_after == 0 && ProtectTimeout.para.max_current_after == 0 
						 && ProtectSet.para.max_current_after_enable == 1 && Max_current_after_cnt < 3000 && MeasureData.YaBan3 == 1)
					{
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && (ProtectTimesLock&0x0080)==0)	// 断路器合位
						{
							if(ProtectSet.para.max_current_after_time>0)ProtectDelay.para.max_current_after = ProtectSet.para.max_current_after_time;
							else ProtectDelay.para.max_current_after = 1;
							ProtectTimeout.para.max_current_after = 0;
							if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag1.bit.max_current_after == 0)
							{
								MeasureData.Pstartflag = 1;      
							}
						}
						ProtectTimesLockCnt = 0;	
					}
				}
				else	// 延时过程中保护消除
				{
					KZQMeasureData.para.ProtectFlag1.bit.max_current_after=0;
					
					if(ProtectDelay.para.max_current_after > 0 && ProtectTimeout.para.max_current_after == 0)
					{
						MeasureData.Psaveflag = 0;
						ProtectDelay.para.max_current_after = 0;
					}	
					ProtectTimesLock &=~0x0080;
				}
			}

		  for(i=0;i<8;i++)
		  {
				if(ProtectTimeout.word[i] != 0)
				{
					switch(i)
					{
						case 0:		// 零序过压保护
							if(	MeasureData.Protect_U0_val > ProtectSet.para.zero_max_voltage)
							{	
								if((ProtectTimesLock&0x0001) == 0)
								{
									ProtectTimesLock |= 0x0001;
									if(ProtectFlag == 0)
									{
										ProtectFlag = 1;				// 保护标志置位
									}
									KZQMeasureData.para.ProtectFlag1.bit.zero_max_voltage=1; //不管有没有合闸都置1
									
									if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
									{
										MeasureData.Psaveflash = 0x01;      
										SendLock_Cnt = 0xFF; 
										if(SwitchFlag == 0)
										{
											SwitchFlag = 3;					// 置保护分闸标志
											RecordProtectStart(TYPE_PROTECT_ZERO_MAX_VOLTAGE);	// 记录保护启动信息
											if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
											{
												if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
												{
													ReOnFlag = 1;
													ReOnLockCnt = 0;
													ReOnTimes  = 0;
													ReOnDelay = ProtectSet.para.once_reon_time;
												}
											}	
										}
									}
							  }
							} 
							break;
						case 1:		// 零序过流保护
							if( (MeasureData.Protect_I0_val > (ProtectSet.para.zero_max_current*10) && ProtectSet.para.zero_max_current < 2100) ||
									(MeasureData.Protect_10I0_val > ProtectSet.para.zero_max_current && ProtectSet.para.zero_max_current >= 2100) )
							{
								if((ProtectTimesLock&0x0002) == 0)
								{
									ProtectTimesLock |= 0x0002;
									if(ProtectFlag == 0)
									{
										ProtectFlag = 1;				// 保护标志置位
									}		
                  KZQMeasureData.para.ProtectFlag1.bit.zero_max_current=1;									
									if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
									{
										MeasureData.Psaveflash = 0x01;      
										SendLock_Cnt = 0xFF; 
										if(SwitchFlag == 0)
										{
											SwitchFlag = 3;					// 置保护分闸标志
											RecordProtectStart(TYPE_PROTECT_ZERO_MAX_CURRENT);	// 记录保护启动信息
											if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
											{
												if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
												{
													ReOnFlag = 1;
													ReOnLockCnt = 0;
													ReOnTimes  = 0;
													ReOnDelay = ProtectSet.para.once_reon_time;
												}
											}									
										}
									}
								}
							}
							break;
						case 2:		// 缺相保护1
							if((MeasureData.Protect_UA_val < 3000 || MeasureData.Protect_UB_val < 3000 || MeasureData.Protect_UC_val < 3000)
									&& MeasureData.Protect_U0_val>3000 &&jdyf==0 && ProtectSet.para.phaseloss_protect_enable == 1)
							{
								if((ProtectTimesLock&0x0004) == 0)
								{
									ProtectTimesLock |= 0x0004;
									if(ProtectFlag == 0)
									{
										ProtectFlag = 1;				// 保护标志置位																					
									}
                  KZQMeasureData.para.ProtectFlag1.bit.no_phase = 1;									
									if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
									{
										MeasureData.Psaveflash = 0x01;      
										SendLock_Cnt = 0xFF; 
										if(SwitchFlag == 0)
										{
											SwitchFlag = 3;					// 置保护分闸标志
											RecordProtectStart(TYPE_PROTECT_PHASE_LOSS);	// 记录保护启动信息
											if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
											{
												if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
												{
													ReOnFlag = 1;
													ReOnLockCnt = 0;
													ReOnTimes  = 0;
													ReOnDelay = ProtectSet.para.once_reon_time;
												}
											}	
										}
									}
								}
							}
							break;
						case 3:  // 接地保护
               // if(MeasureData.Protect_U0_val > AlarmSet.para.alarmU0 && jdxb != 0 &&
						   if(MeasureData.Protect_U0_val > AlarmSet.para.alarmU0 &&
								 ((MeasureData.Protect_I0_val > (AlarmSet.para.alarmI0*10) && AlarmSet.para.alarmI0 < 2100) || 
							    (MeasureData.Protect_10I0_val > AlarmSet.para.alarmI0 && AlarmSet.para.alarmI0 >= 2100))&& 
							    ((SystemSet.para.groundtype == 1 && dycfzc2 == 2) || (SystemSet.para.groundtype == 2 && dycfzc == 2)))
							 {
								if((ProtectTimesLock&0x0008) == 0)
								{
									ProtectTimesLock |= 0x0008;
								  if(ProtectFlag == 0)
								  {
									  ProtectFlag = 1;				// 保护标志置位										
								  }
									KZQMeasureData.para.ProtectFlag1.bit.ground=1;									
									if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
									{
										if(SwitchFlag == 0)
										{
											SwitchFlag = 3;	
											RecordProtectStart(TYPE_PROTECT_GND);	// 记录保护启动信息																			 
											if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)// 重合闸软压板和硬压板投入
											{
												if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
												{
											
													ReOnFlag = 1;
													ReOnLockCnt = 0;
													ReOnTimes  = 0;
													ReOnDelay = ProtectSet.para.once_reon_time;
												}
											}	
										}
									}
							  }
							}
						  break;
						case 4:		// 速断保护
							if(	(MeasureData.Protect_IASD_val > (ProtectSet.para.fast_off*10) && ProtectSet.para.fast_off < 1000) || 
								(MeasureData.Protect_IA10SD_val > ProtectSet.para.fast_off && ProtectSet.para.fast_off >= 1000) ||
								(MeasureData.Protect_IBSD_val > (ProtectSet.para.fast_off*10) && ProtectSet.para.fast_off < 1000) || 
								(MeasureData.Protect_IB10SD_val > ProtectSet.para.fast_off && ProtectSet.para.fast_off >= 1000) ||
								(MeasureData.Protect_ICSD_val > (ProtectSet.para.fast_off*10) && ProtectSet.para.fast_off < 1000) || 
								(MeasureData.Protect_IC10SD_val > ProtectSet.para.fast_off && ProtectSet.para.fast_off >= 1000))
							{
								if((ProtectTimesLock&0x0010) == 0)
								{
									ProtectTimesLock |= 0x0010;									
									if(ProtectFlag == 0)
									{
										ProtectFlag = 1;				// 保护标志置位
									}
									KZQMeasureData.para.ProtectFlag1.bit.fast_off=1;									
									if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
									{
										MeasureData.Psaveflash = 0x01;      
										SendLock_Cnt = 0xFF; 
										if(SwitchFlag == 0)
										{
											SwitchFlag = 3;					// 置保护分闸标志
											RecordProtectStart(TYPE_PROTECT_FAST_OFF);	// 记录保护启动信息
											if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
											{
												if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
												{
													
													ReOnFlag = 1;
													ReOnLockCnt = 0;
													ReOnTimes  = 0;
													ReOnDelay = ProtectSet.para.once_reon_time;
												
												}
											}									
										}
								  }
								}
							}
							break;
						case 5:		// 延时速断保护
							if(	(MeasureData.Protect_IA_val > (ProtectSet.para.delay_fast_off*10) && ProtectSet.para.delay_fast_off < 1000) || 
								(MeasureData.Protect_IA10_val > ProtectSet.para.delay_fast_off && ProtectSet.para.delay_fast_off >= 1000) ||
								(MeasureData.Protect_IB_val > (ProtectSet.para.delay_fast_off*10) && ProtectSet.para.delay_fast_off < 1000) || 
								(MeasureData.Protect_IB10_val > ProtectSet.para.delay_fast_off && ProtectSet.para.delay_fast_off >= 1000) ||
								(MeasureData.Protect_IC_val > (ProtectSet.para.delay_fast_off*10) && ProtectSet.para.delay_fast_off < 1000) || 
								(MeasureData.Protect_IC10_val > ProtectSet.para.delay_fast_off && ProtectSet.para.delay_fast_off >= 1000))
							{
								if((ProtectTimesLock&0x0020) == 0)
								{
									ProtectTimesLock |= 0x0020;									
									if(ProtectFlag == 0)
									{
										ProtectFlag = 1;				// 保护标志置位
									}
									KZQMeasureData.para.ProtectFlag1.bit.delay_fast_off=1;									
									if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
									{
										MeasureData.Psaveflash = 0x01;      
										SendLock_Cnt = 0xFF; 									
										if(SwitchFlag == 0)
										{
											SwitchFlag = 3;					// 置保护分闸标志
											RecordProtectStart(TYPE_PROTECT_DELAY_FAST_OFF);	// 记录保护启动信息
											if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
											{
												if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
												{
													
													ReOnFlag = 1;
													ReOnLockCnt = 0;
													ReOnTimes = 0;
													ReOnDelay = ProtectSet.para.once_reon_time;
												
												}
											}									
										}
									}
								}
							}
							break;
						case 6:		// 过流保护
							if(	(MeasureData.Protect_IA_val > (ProtectSet.para.max_current*10) && ProtectSet.para.max_current < 1000) || 
								(MeasureData.Protect_IA10_val > ProtectSet.para.max_current && ProtectSet.para.max_current >= 1000) ||
								(MeasureData.Protect_IB_val > (ProtectSet.para.max_current*10) && ProtectSet.para.max_current < 1000) || 
								(MeasureData.Protect_IB10_val > ProtectSet.para.max_current && ProtectSet.para.max_current >= 1000) ||
								(MeasureData.Protect_IC_val > (ProtectSet.para.max_current*10) && ProtectSet.para.max_current < 1000) || 
								(MeasureData.Protect_IC10_val > ProtectSet.para.max_current && ProtectSet.para.max_current >= 1000) )
							{
								if((ProtectTimesLock&0x0040) == 0)
								{
									ProtectTimesLock |= 0x0040;
									if(ProtectFlag == 0)
									{
										ProtectFlag = 1;				// 保护标志置位
									}
									KZQMeasureData.para.ProtectFlag1.bit.max_current=1;									
									if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
									{
										MeasureData.Psaveflash = 0x01;      
										SendLock_Cnt = 0xFF; 									
										if(SwitchFlag == 0)
										{
											SwitchFlag = 3;					// 置保护分闸标志
											RecordProtectStart(TYPE_PROTECT_MAX_CURRENT);	// 记录保护启动信息
											if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
											{
												if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
												{
													ReOnFlag = 1;
													ReOnLockCnt = 0;
													ReOnTimes = 0;
													ReOnDelay = ProtectSet.para.once_reon_time;
												}
											}									
										}
									}
								}
							}
							break;
						case 7:		// 过流后加速保护
							if(	(MeasureData.Protect_IA_val > (ProtectSet.para.max_current_after*10) && ProtectSet.para.max_current_after < 1000) || 
								(MeasureData.Protect_IA10_val > ProtectSet.para.max_current_after && ProtectSet.para.max_current_after >= 1000) ||
								(MeasureData.Protect_IB_val > (ProtectSet.para.max_current_after*10) && ProtectSet.para.max_current_after < 1000) || 
								(MeasureData.Protect_IB10_val > ProtectSet.para.max_current_after && ProtectSet.para.max_current_after >= 1000) ||
								(MeasureData.Protect_IC_val > (ProtectSet.para.max_current_after*10) && ProtectSet.para.max_current_after < 1000) || 
								(MeasureData.Protect_IC10_val > ProtectSet.para.max_current_after && ProtectSet.para.max_current_after >= 1000) )
							{
								if((ProtectTimesLock&0x0080) == 0)
								{
									ProtectTimesLock |= 0x0080;									
									if(ProtectFlag == 0)
									{
										ProtectFlag = 1;				// 保护标志置位
									}
									KZQMeasureData.para.ProtectFlag1.bit.max_current_after=1;									
									if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
									{
										MeasureData.Psaveflash = 0x01;      
										SendLock_Cnt = 0xFF; 									
										if(SwitchFlag == 0)
										{
											SwitchFlag = 3;					// 置保护分闸标志
											RecordProtectStart(TYPE_PROTECT_MAX_CURRENT_AFTER);	// 记录保护启动信息
										}
									}
								}
							}
							break;
						default:	
							break;
				  }
				  ProtectTimeout.word[i] = 0;
			  }
		  }
			
			//过压保护
			if(MeasureData.Display_UAB_val > ProtectSet.para.max_voltage || MeasureData.Display_UBC_val > ProtectSet.para.max_voltage)
			{
				if(ProtectDelay.para.max_voltage == 0 && ProtectTimeout.para.max_voltage == 0 && ProtectSet.para.max_voltage_enable==1)
				{
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					if((ProtectTimesLock&0x0100)==0)
					{
						if(ProtectSet.para.max_voltage_time>0)
						  ProtectDelay.para.max_voltage = ProtectSet.para.max_voltage_time;	
						else 
						  ProtectDelay.para.max_voltage = 1;
						ProtectTimeout.para.max_voltage = 0;
						
						if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag2.bit.max_voltage == 0)
						{
							MeasureData.Pstartflag = 1;      
						}
					}
					ProtectTimesLockCnt = 0;
				}
			}
			else	// 延时过程中保护消除
			{   
				KZQMeasureData.para.ProtectFlag2.bit.max_voltage=0;
				
				if(ProtectDelay.para.max_voltage > 0 && ProtectTimeout.para.max_voltage == 0)
				{
					MeasureData.Psaveflag = 0;
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					{
						ProtectDelay.para.max_voltage = 0;
					}
				}
				ProtectTimesLock &=~0x0100;
			}
			//过压保护
			if(ProtectTimeout.para.max_voltage != 0)
			{
			  if(MeasureData.Display_UAB_val > ProtectSet.para.max_voltage || MeasureData.Display_UBC_val > ProtectSet.para.max_voltage)
				{	
					if((ProtectTimesLock&0x0100) == 0)
					{
						ProtectTimesLock |= 0x0100;	
						if(ProtectFlag == 0)
						{
							ProtectFlag = 1;				// 保护标志置位
						}
            KZQMeasureData.para.ProtectFlag2.bit.max_voltage=1;						
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
						{
							MeasureData.Psaveflash = 0x01;      
							SendLock_Cnt = 0xFF; 
							if(SwitchFlag == 0)
							{
								SwitchFlag = 3;					// 置保护分闸标志
								RecordProtectStart(TYPE_PROTECT_MAX_VOLTAGE);	// 记录保护启动信息
								if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
								{
									if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
									{
										ReOnFlag = 1;
										ReOnLockCnt = 0;
										ReOnTimes  = 0;
										ReOnDelay = ProtectSet.para.once_reon_time;
									}
								}	
							}
						}
					}
				} 
			}
			
			//高频保护
			if(MeasureData.freq > ProtectSet.para.max_freq)
			{
				if(ProtectDelay.para.max_freq == 0 && ProtectTimeout.para.max_freq == 0 && ProtectSet.para.max_freq_enable==1)
				{
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					if((ProtectTimesLock&0x0200)==0)
					{
						if(ProtectSet.para.max_freq_time>0)
						  ProtectDelay.para.max_freq = ProtectSet.para.max_freq_time;	
						else 
						  ProtectDelay.para.max_freq = 1;
						ProtectTimeout.para.max_freq = 0;
						
						if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag2.bit.max_freq == 0)
						{
							MeasureData.Pstartflag = 1;      
						}
					}
					ProtectTimesLockCnt = 0;
				}
			}
			else	// 延时过程中保护消除
			{   
				KZQMeasureData.para.ProtectFlag2.bit.max_freq=0;
				
				if(ProtectDelay.para.max_freq > 0 && ProtectTimeout.para.max_freq == 0)
				{
					MeasureData.Psaveflag = 0;
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					{
						ProtectDelay.para.max_freq = 0;
					}
				}
				ProtectTimesLock &=~0x0200;
			}
			//高频保护
			if(ProtectTimeout.para.max_freq != 0)
			{
			  if(MeasureData.freq > ProtectSet.para.max_freq)
				{	
					if((ProtectTimesLock&0x0200) == 0)
					{
						ProtectTimesLock |= 0x0200;
						if(ProtectFlag == 0)
						{
							ProtectFlag = 1;				// 保护标志置位
						}
						KZQMeasureData.para.ProtectFlag2.bit.max_freq=1;						
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
						{
							MeasureData.Psaveflash = 0x01;      
							SendLock_Cnt = 0xFF; 
							if(SwitchFlag == 0)
							{
								SwitchFlag = 3;					// 置保护分闸标志
								RecordProtectStart(TYPE_PROTECT_MAX_FREQ);	// 记录保护启动信息
								if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
								{
									if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
									{
										ReOnFlag = 1;
										ReOnLockCnt = 0;
										ReOnTimes  = 0;
										ReOnDelay = ProtectSet.para.once_reon_time;
									}
								}	
							}
						}
					}
				} 
			}
			
		  //低频保护
			if(MeasureData.freq < ProtectSet.para.low_freq && MeasureData.freq > 4000)
			{
				if(ProtectDelay.para.low_freq == 0 && ProtectTimeout.para.low_freq == 0 && ProtectSet.para.low_freq_enable==1)
				{
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					if((ProtectTimesLock&0x0400)==0)
					{
						if(ProtectSet.para.low_freq_time>0)
						  ProtectDelay.para.low_freq = ProtectSet.para.low_freq_time;	
						else 
						  ProtectDelay.para.low_freq = 1;
						ProtectTimeout.para.low_freq = 0;
						
						if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.ProtectFlag2.bit.low_freq == 0)
						{
							MeasureData.Pstartflag = 1;      
						}
					}
					ProtectTimesLockCnt = 0;
				}
			}
			else	// 延时过程中保护消除
			{   
				KZQMeasureData.para.ProtectFlag2.bit.low_freq=0;
				
				if(ProtectDelay.para.low_freq > 0 && ProtectTimeout.para.low_freq == 0)
				{
					MeasureData.Psaveflag = 0;
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					{
						ProtectDelay.para.low_freq = 0;
					}
				}
				ProtectTimesLock &=~0x0400;
			}
			//低频保护
			if(ProtectTimeout.para.low_freq != 0)
			{
			  if(MeasureData.freq < ProtectSet.para.low_freq  && MeasureData.freq > 4000)
				{	
					if((ProtectTimesLock&0x0400) == 0)
					{
						ProtectTimesLock |= 0x0400;
						if(ProtectFlag == 0)
						{
							ProtectFlag = 1;				// 保护标志置位
						}
						KZQMeasureData.para.ProtectFlag2.bit.low_freq=1;						
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 处于合闸状态就要保护分闸
						{
							MeasureData.Psaveflash = 0x01;      
							SendLock_Cnt = 0xFF; 
							if(SwitchFlag == 0)
							{
								SwitchFlag = 3;					// 置保护分闸标志
								RecordProtectStart(TYPE_PROTECT_LOW_FREQ);	// 记录保护启动信息
								if(ProtectSet.para.once_reon_enable == 1 && MeasureData.YaBan1 == 1)	// 重合闸软压板和硬压板投入
								{
									if((ReOnFlag == 0) && (ReOnLockCnt >= 10) && (ReOnLockCnt1 >= 15))	// 重合闸功能开启
									{
										ReOnFlag = 1;
										ReOnLockCnt = 0;
										ReOnTimes  = 0;
										ReOnDelay = ProtectSet.para.once_reon_time;
									}
								}	
							}
						}
					}
				} 
			}
		  state = 4; 
		  break;

		case 4:    //报警判断
			if(AlarmSet.para.max_load < 1000)	// 低于额定电流，取一倍电流
			{
				if((MeasureData.Protect_IA_val > (AlarmSet.para.max_load*10) && MeasureIAError == 0) 
			   ||(MeasureData.Protect_IB_val > (AlarmSet.para.max_load*10) && MeasureIBError == 0)
				 ||(MeasureData.Protect_IC_val > (AlarmSet.para.max_load*10) && MeasureICError == 0))	// 过负荷
				{
					if(AlarmDelay.para.max_load == 0 && AlarmTimeout.para.max_load == 0 && AlarmSet.para.max_load_enable == 1)
					{
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						if((AlarmTimesLock&0x0001)==0)
						{
							if(AlarmSet.para.max_load_time>0)AlarmDelay.para.max_load = AlarmSet.para.max_load_time;
							else AlarmDelay.para.max_load = 1;
							AlarmTimeout.para.max_load = 0;	
						}
						AlarmTimesLockCnt = 0;
					}
				}
				else	// 延时过程中保护消除
				{
					if(AlarmDelay.para.max_load > 0 && AlarmTimeout.para.max_load == 0)
					{						
						AlarmDelay.para.max_load = 0;
					}
					AlarmTimesLock &=~0x0001;
				}				
			}
			else // 高于额定电流，取十倍电流
			{
				if((MeasureData.Protect_IA10_val > AlarmSet.para.max_load && MeasureIAError == 0) ||
				   (MeasureData.Protect_IB10_val > AlarmSet.para.max_load && MeasureIBError == 0) ||
				   (MeasureData.Protect_IC10_val > AlarmSet.para.max_load && MeasureICError == 0))	// 过负荷
				{
					if(AlarmDelay.para.max_load == 0 && AlarmTimeout.para.max_load == 0 && AlarmSet.para.max_load_enable == 1)
					{
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						if((AlarmTimesLock&0x0001)==0)
						{
							if(AlarmSet.para.max_load_time>0)AlarmDelay.para.max_load = AlarmSet.para.max_load_time;
							else AlarmDelay.para.max_load = 1;
							AlarmTimeout.para.max_load = 0;	
						}
						AlarmTimesLockCnt = 0;
					}
				}
				else	// 延时过程中保护消除
				{
					if(AlarmDelay.para.max_load > 0 && AlarmTimeout.para.max_load == 0)
					{							
						AlarmDelay.para.max_load = 0;
					}	
					AlarmTimesLock &=~0x0001;
				}
			}

			if(MeasureData.Protect_UAB_val > AlarmSet.para.max_voltage || MeasureData.Protect_UBC_val > AlarmSet.para.max_voltage || MeasureData.Protect_UCA_val > AlarmSet.para.max_voltage)	// 过压
			{
				if(AlarmDelay.para.max_voltage == 0 && AlarmTimeout.para.max_voltage == 0 && AlarmSet.para.max_voltage_enable == 1)
				{
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
					if((AlarmTimesLock&0x0002)==0)
					{
						if(AlarmSet.para.max_voltage_time>0)AlarmDelay.para.max_voltage = AlarmSet.para.max_voltage_time;	
						else AlarmDelay.para.max_voltage = 1;
						AlarmTimeout.para.max_voltage = 0;
					}
					AlarmTimesLockCnt = 0;
				}
			}
			else	// 延时过程中保护消除
			{
				if(AlarmDelay.para.max_voltage > 0 && AlarmTimeout.para.max_voltage == 0)
				{
				  AlarmDelay.para.max_voltage = 0;
				}
				AlarmTimesLock &=~0x0002;
			}
			
			if(under_voltage_on)
			{
				if(MeasureData.Protect_UAB_val < AlarmSet.para.min_voltage || MeasureData.Protect_UBC_val < AlarmSet.para.min_voltage || MeasureData.Protect_UCA_val < AlarmSet.para.min_voltage)	// 欠压
				{
					if(AlarmDelay.para.min_voltage == 0 && AlarmTimeout.para.min_voltage == 0 && AlarmSet.para.min_voltage_enable == 1)
					{
						//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)	// 断路器合位
						if((AlarmTimesLock&0x0004)==0)
						{
							if(AlarmSet.para.min_voltage_time>0)AlarmDelay.para.min_voltage = AlarmSet.para.min_voltage_time;	
							else AlarmDelay.para.min_voltage = 1;
							AlarmTimeout.para.min_voltage = 0;
						}
						AlarmTimesLockCnt = 0;
					}

				}
				else	// 延时过程中保护消除
				{
					if(AlarmDelay.para.min_voltage > 0 && AlarmTimeout.para.min_voltage == 0)
					{
					  AlarmDelay.para.min_voltage = 0;
					}
					AlarmTimesLock &=~0x0004;
				}
		  }
			
			//过谐波
			if(FXBZUAB > AlarmSet.para.max_harmonic|| FXBZIA > AlarmSet.para.max_harmonic || FXBZIB > AlarmSet.para.max_harmonic || FXBZIC > AlarmSet.para.max_harmonic) // 谐波比
			{
				if (AlarmDelay.para.max_harmonic== 0 && AlarmTimeout.para.max_harmonic == 0 && AlarmSet.para.max_harmonic_enable==1 )
				{ 
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1)
					if((AlarmTimesLock&0x0008)==0)
					{
						if(AlarmSet.para.max_harmonic_time > 0)AlarmDelay.para.max_harmonic=AlarmSet.para.max_harmonic_time;
						else AlarmDelay.para.max_harmonic=1;
						AlarmTimeout.para.max_harmonic=0;
					}
					AlarmTimesLockCnt = 0;
				}
			}
			else
			{
				if(AlarmDelay.para.max_harmonic >0 && AlarmTimeout.para.max_harmonic==0)
					AlarmDelay.para.max_harmonic=0;  
        AlarmTimesLock &=~0x0008;				
			}
			
			//电压合格率
			if(qualif_volt<AlarmSet.para.voltage_qualifrate * 10)
			{
				if(AlarmTimeout.para.voltage_qualifrate == 0 && AlarmSet.para.voltage_qualifrate_enable==1 )
				{ 
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1 )
					if((AlarmTimesLock&0x0010)==0)
					{
							AlarmDelay.para.voltage_qualifrate = 10000;
							AlarmTimeout.para.voltage_qualifrate=0;
					}
					AlarmTimesLockCnt = 0;
				}
			}
			else
			{
				if(AlarmDelay.para.voltage_qualifrate > 0 && AlarmTimeout.para.voltage_qualifrate == 0)
				{
					AlarmDelay.para.voltage_qualifrate=0;
				} 
        AlarmTimesLock &=~0x0010;				
			}
                
			//电池低电压  
			if(MeasureData.V_BAT < AlarmSet.para.battery_low_voltage && MeasureData.V_BAT > 150 )//电池电压过低
			{
				if((AlarmTimesLock&0x0020)==0)
				{
					if(AlarmDelay.para.battery_low_voltage==0 && AlarmTimeout.para.battery_low_voltage==0 )
					{
						AlarmDelay.para.battery_low_voltage=500;
						AlarmTimeout.para.battery_low_voltage=0;
					}
			  }
			}
			else 
			{ 
				if(AlarmDelay.para.battery_low_voltage > 0 && AlarmTimeout.para.battery_low_voltage==0)
				{
					AlarmDelay.para.battery_low_voltage = 0;
				}
        AlarmTimesLock &=~0x0020;				
			}
			
		  //零序过压报警
			if(MeasureData.Protect_U0_val > AlarmSet.para.alarmU0)
			{    
				if(AlarmDelay.para.groundU0 == 0 && AlarmTimeout.para.groundU0 == 0 && AlarmSet.para.alarmU0_enable == 1 ) 
				{   
					//if(KZQMeasureData.para.DLQflag.bit.breaker == 1 )
					if((AlarmTimesLock&0x0040)==0)
					{
						if( AlarmSet.para.alarmU0_time > 0 )
						{
							AlarmDelay.para.groundU0 = AlarmSet.para.alarmU0_time;
						}
						else 
						{
							AlarmDelay.para.groundU0 = 1;
						}
						AlarmTimeout.para.groundU0= 0;
					}
					AlarmTimesLockCnt = 0;
				}
			}
			else 
			{
				if(AlarmDelay.para.groundU0 > 0 && AlarmTimeout.para.groundU0 == 0) 
				{
					AlarmDelay.para.groundU0 = 0;
				}
				AlarmTimesLock &=~0x0040;	
			}
	          
			//零序过流报警
			if(AlarmSet.para.alarmI0 < 2100)
			{
				if(MeasureData.Protect_I0_val > AlarmSet.para.alarmI0 * 10)  
				{    
					 if(AlarmDelay.para.groundI0 == 0 && AlarmTimeout.para.groundI0 == 0 && AlarmSet.para.alarmI0_enable == 1 ) 
					 {   
					   //if(KZQMeasureData.para.DLQflag.bit.breaker == 1)
						 if((AlarmTimesLock&0x0080)==0)
						 {
						   if( AlarmSet.para.alarmI0_time > 0 )
						   {
							   AlarmDelay.para.groundI0 = AlarmSet.para.alarmI0_time;
						   }
							 else 
							 {
							   AlarmDelay.para.groundI0 = 1;
						   }
						   AlarmTimeout.para.groundI0= 0;
						 }
						 AlarmTimesLockCnt = 0;
					 }
				}
				else 
				{
					if(AlarmDelay.para.groundI0 > 0 && AlarmTimeout.para.groundI0 == 0) 
					{
						AlarmDelay.para.groundI0 = 0;
					}
					AlarmTimesLock &=~0x0080;
				}
			}
			else
			{
			  if(MeasureData.Protect_10I0_val > AlarmSet.para.alarmI0)  
				{    
				  if(AlarmDelay.para.groundI0 == 0 && AlarmTimeout.para.groundI0 == 0 && AlarmSet.para.alarmI0_enable == 1 ) 
				  {   
					  //if(KZQMeasureData.para.DLQflag.bit.breaker == 1)
						if((AlarmTimesLock&0x0080)==0)
						{
					    if(AlarmSet.para.alarmI0_time > 0 )
						  {
						    AlarmDelay.para.groundI0 = AlarmSet.para.alarmI0_time;
						  }
							else 
							{
						    AlarmDelay.para.groundI0 = 1;
						  }
						 AlarmTimeout.para.groundI0= 0;
						}
						AlarmTimesLockCnt = 0;
				  }
				}
				else 
				{
	        if(AlarmDelay.para.groundI0 > 0 && AlarmTimeout.para.groundI0 == 0) 
					{
						AlarmDelay.para.groundI0 = 0;
					}
					AlarmTimesLock &=~0x0080;
		    }
			}
	          			     
			//零序接地报警
			if(jdyf==1 && MeasureData.YaBan2 == 1)  //电压判断接地
			{    
				if(AlarmDelay.para.ground == 0 && AlarmTimeout.para.ground == 0)
				{   
				  //if(KZQMeasureData.para.DLQflag.bit.breaker == 1)
					if((AlarmTimesLock&0x0100)==0)
				  {
						if(AlarmSet.para.alarmU0_time > 0 )
					  {
						  AlarmDelay.para.ground = AlarmSet.para.alarmU0_time;
					  }
						else 
						{
						  AlarmDelay.para.ground = 1;
					  }
					  AlarmTimeout.para.ground= 0;
						if(MeasureData.Psaveflag == 0 && SendLock_Cnt == 0 && KZQMeasureData.para.AlarmFlag.bit.ground == 0)
						{
							MeasureData.Pstartflag = 1;   						
						}
					}
				  AlarmTimesLockCnt = 0;
				}
			}
			else 
			{
				if(AlarmDelay.para.ground > 0 && AlarmTimeout.para.ground == 0) 
				{
					MeasureData.Psaveflag = 0;
					AlarmDelay.para.ground = 0;
				}
				AlarmTimesLock &=~0x0100;
			}

			for(i=0;i<(sizeof(struct ALARM_TIMEOUT)/2);i++)
			{
				if(AlarmTimeout.word[i] != 0)
				{
					switch(i)
					{
						case 0:
						  //过负荷
						  if((MeasureData.Protect_IA_val > (AlarmSet.para.max_load*10) && AlarmSet.para.max_load < 1000) || 
							   (MeasureData.Protect_IA10_val > AlarmSet.para.max_load && AlarmSet.para.max_load >= 1000)   ||
							   (MeasureData.Protect_IB_val > (AlarmSet.para.max_load*10) && AlarmSet.para.max_load < 1000)||
							   (MeasureData.Protect_IB10_val > AlarmSet.para.max_load && AlarmSet.para.max_load >= 1000)   ||
							   (MeasureData.Protect_IC_val > (AlarmSet.para.max_load*10) && AlarmSet.para.max_load < 1000) || 
							   (MeasureData.Protect_IC10_val > AlarmSet.para.max_load && AlarmSet.para.max_load >= 1000))
						  {	
						    if((AlarmTimesLock&0x0001) == 0)
							  {
									AlarmTimesLock |= 0x0001;
									if(KZQMeasureData.para.AlarmFlag.bit.max_load==0)
									{
										KZQMeasureData.para.AlarmFlag.bit.max_load = 1;				// 报警标志置位
										RecordAlarm(TYPE_ALARM_MAX_LOAD,AlarmSet.para.max_load,AlarmSet.para.max_load_time);
										KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
										AlarmDelay.word[i] = 0;
										AlarmTimeout.word[i] = 0;
									}	
							  }	
						  }
						  break;
						case 1:			
						  // 过压报警
						  if(MeasureData.Protect_UAB_val > AlarmSet.para.max_voltage || 
						 	 MeasureData.Protect_UBC_val > AlarmSet.para.max_voltage || 
							 MeasureData.Protect_UCA_val > AlarmSet.para.max_voltage)
						  {	
								if((AlarmTimesLock&0x0002) == 0)	
								{
									AlarmTimesLock |= 0x0002;
									if(KZQMeasureData.para.AlarmFlag.bit.max_voltage==0)
									{
										KZQMeasureData.para.AlarmFlag.bit.max_voltage = 1;				// 报警标志置位
										RecordAlarm(TYPE_ALARM_MAX_VOLTAGE,AlarmSet.para.max_voltage,AlarmSet.para.max_voltage_time);
										KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
										AlarmDelay.word[i] = 0;
										AlarmTimeout.word[i] = 0;
									}									
								}
						  } 
						  break;
						case 2:		
              if(under_voltage_on)
						  {								
								// 欠压报警
								if(MeasureData.Protect_UAB_val < AlarmSet.para.min_voltage || 
								 MeasureData.Protect_UBC_val < AlarmSet.para.min_voltage || 
								 MeasureData.Protect_UCA_val < AlarmSet.para.min_voltage)
								{		
									if((AlarmTimesLock&0x0004) == 0)
									{
										AlarmTimesLock |= 0x0004;
										if(KZQMeasureData.para.AlarmFlag.bit.min_voltage==0)
										{
											KZQMeasureData.para.AlarmFlag.bit.min_voltage = 1;				// 报警标志置位
											RecordAlarm(TYPE_ALARM_MIN_VOLTAGE,AlarmSet.para.min_voltage,AlarmSet.para.min_voltage_time);
											KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
											AlarmDelay.word[i] = 0;
											AlarmTimeout.word[i] = 0;
										}					
									}
								}
						  }
						  break;
						case 3:
						  //过谐波
						  if(FXBZUAB > AlarmSet.para.max_harmonic || FXBZIA > AlarmSet.para.max_harmonic  
						     || FXBZIB > AlarmSet.para.max_harmonic || FXBZIC > AlarmSet.para.max_harmonic) 
						  {
								if( (AlarmTimesLock&0x0008) == 0)
								{
									AlarmTimesLock |= 0x0008;
									if(KZQMeasureData.para.AlarmFlag.bit.max_harmonic==0)
									{
									  KZQMeasureData.para.AlarmFlag.bit.max_harmonic = 1;
									  RecordAlarm(TYPE_ALARM_MAX_HAR, AlarmSet.para.max_harmonic,AlarmSet.para.max_harmonic_time);	// 报警记录											
									  KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
									  AlarmDelay.word[i] = 0;
									  AlarmTimeout.word[i] = 0;
									}
								}
						  }
						  break;
						case 4:
						  //电压合格
						  if ( qualif_volt < AlarmSet.para.voltage_qualifrate * 10) 
						  {
								if((AlarmTimesLock&0x0010) == 0)
								{
									AlarmTimesLock |= 0x0010;
									if(KZQMeasureData.para.AlarmFlag.bit.voltage_qualifrate==0)
									{
										KZQMeasureData.para.AlarmFlag.bit.voltage_qualifrate=1;
										RecordAlarm(TYPE_ALARM_VOL_QUA_LOW,AlarmSet.para.voltage_qualifrate,0);// 报警记录											
										KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
										AlarmDelay.word[i] = 0;
										AlarmTimeout.word[i] = 0;
									}
								}
						  }
						  break;
						case 5:
						  //电池电压低
						  if(MeasureData.V_BAT < AlarmSet.para.battery_low_voltage && MeasureData.V_BAT > 100 ) 
						  {
								if((AlarmTimesLock&0x0020) == 0)
								{
									AlarmTimesLock |= 0x0020;
									if(KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage==0)
									{
										KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage=1;
										RecordAlarm(TYPE_ALARM_BAT_LOW,AlarmSet.para.battery_low_voltage,500);// 报警记录											
										KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
										AlarmDelay.word[i] = 0;
										AlarmTimeout.word[i] = 0;
									}
							  }
						  }
						  break;
						case 6:
						   //零序电压报警
						  if (MeasureData.Protect_U0_val > AlarmSet.para.alarmU0)
						  {
								if((AlarmTimesLock&0x0040) == 0)
								{
									AlarmTimesLock |= 0x0040;
									if(KZQMeasureData.para.AlarmFlag.bit.zero_max_voltage==0)
									{										
										KZQMeasureData.para.AlarmFlag.bit.zero_max_voltage=1;
										RecordAlarm(TYPE_ALARM_ZERO_MAX_VOLTAGE,AlarmSet.para.alarmU0,AlarmSet.para.alarmU0_time);// 报警记录											
										KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
										AlarmDelay.word[i] = 0;
										AlarmTimeout.word[i] = 0;
									}
						    }
						  }
						  break;
						case 7:
						   //零序电流报警
						  if( (MeasureData.Protect_I0_val > (AlarmSet.para.alarmI0*10) && AlarmSet.para.alarmI0 < 2100) ||
						      (MeasureData.Protect_10I0_val > AlarmSet.para.alarmI0 && AlarmSet.para.alarmI0 >= 2100) )
						  {
								if((AlarmTimesLock&0x0080) == 0)
								{
									AlarmTimesLock |= 0x0080;
									if(KZQMeasureData.para.AlarmFlag.bit.zero_max_current==0)
									{	
										KZQMeasureData.para.AlarmFlag.bit.zero_max_current=1;
										RecordAlarm(TYPE_ALARM_ZERO_MAX_CURRENT,AlarmSet.para.alarmI0,AlarmSet.para.alarmU0_time);// 报警记录											
										KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
										AlarmDelay.word[i] = 0;
										AlarmTimeout.word[i] = 0;
									}
								}
						  }
						  break;
						case 8:
						   //接地报警
						  if(MeasureData.Protect_U0_val > AlarmSet.para.alarmU0 && jdxb != 0 &&
								((MeasureData.Protect_I0_val > (AlarmSet.para.alarmI0*10) && AlarmSet.para.alarmI0 < 2100) || 
							   (MeasureData.Protect_10I0_val > AlarmSet.para.alarmI0 && AlarmSet.para.alarmI0 >= 2100))&& 
								 ((SystemSet.para.groundtype == 1 && dycfzc2 != 0) || (SystemSet.para.groundtype == 2 && dycfzc != 0)))
						  {
								if((AlarmTimesLock&0x0100) == 0)
								{
									AlarmTimesLock |= 0x0100;
									if(KZQMeasureData.para.AlarmFlag.bit.ground==0)
									{
										MeasureData.Psaveflash = 0x01;      
										SendLock_Cnt = 0xFF; 
										KZQMeasureData.para.AlarmFlag.bit.ground=1;
										RecordAlarm(TYPE_ALARM_GND,AlarmSet.para.alarmU0,AlarmSet.para.alarmU0_time);// 报警记录											
										KZQMeasureData.para.RequestFlag1.bit.record_alarm = 1;
										AlarmDelay.word[i] = 0;
										AlarmTimeout.word[i] = 0;
									}
						    }
						  }
						  break;
						default:
						  break;
					}
				}
			}
			state = 0;
			break;
		default:
			break;
	}
}

void JDPD(void)        //接地判断
{
  //zxdjdfs 中性点接地方式   1不接地，2通过消弧线圈接地，3通过小电阻接地，4直接接地
  //jdyf    接地与否         0无接地，1有接地。
  //jdxb    接地相别         0无接地;1A相；2、B相；3、C相。
  //dycfzc  电源侧/负载侧    0无接地 1电源侧；2负载侧。
  //jdxz    接地性质         0无接地 1金属性接地；2电阻性接地；3弧光接地
  //hgxb    弧光过电压相别   0无接地 1、A相；2、B相；3、C相。
  //hggdy   弧光过电压       HGGDYH,HUGDYL.
  //u16 U0_VALUE=0;
  static u8 dycfzc_cnt1=0;
  static u8 dycfzc_cnt2=0; 
  static u8 jdnum=0;
  zxdjdfs = SystemSet.para.groundtype;
//  if(AlarmSet.para.alarmU0_enable == 1)
//    U0_VALUE=AlarmSet.para.alarmU0;
//  else U0_VALUE=300;//(一次电压为3kv)
 
  iuxw=(s16)FDEGI0-(s16)FDEGU0;
  if (iuxw<0) 
    iuxw=iuxw+36000;

  //有无接地  (接地与否是判断的总条件，其他以下都是在接地情况下的判断)
  if((MeasureData.Protect_UA_val > 3000||MeasureData.Protect_UB_val > 3000||MeasureData.Protect_UC_val > 3000) 
		  //&&    MeasureData.Protect_U0_val > AlarmSet.para.alarmU0 && KZQMeasureData.para.DLQflag.bit.breaker == 1 
	    && KZQMeasureData.para.DLQflag.bit.breaker == 1 && (((MeasureData.Protect_I0_val > (AlarmSet.para.alarmI0*10) && AlarmSet.para.alarmI0 < 2100) || 
			//(MeasureData.Protect_10I0_val > AlarmSet.para.alarmI0 && AlarmSet.para.alarmI0 >= 2100))  || zxdjdfs==2)) 
	    (MeasureData.Protect_10I0_val > AlarmSet.para.alarmI0 && AlarmSet.para.alarmI0 >= 2100)))) 
  {
		jdnum++;
		if(jdnum>3)
		{
			jdyf=1;
      jdnum=0;		
			//接地相别的判断
			if(zxdjdfs==1 || zxdjdfs==0)    // '中性点不接地
			{
				u0pdxw = (s16)FDEGU0;
				if (u0pdxw < 0)     
					u0pdxw = u0pdxw + 36000;
			}
			if(zxdjdfs==2)       //'中性点消弧线圈接地   2016.12.21  发现消弧线圈过补偿模式下，修正不起作用，仍需要研究。
			{
				u0pdxw =(s16)FDEGU0;   
				if(u0pdxw <0)      u0pdxw = u0pdxw + 36000;
			}
			if(MeasureData.Protect_UA_val < 3000 || MeasureData.Protect_UB_val < 3000 ||MeasureData.Protect_UC_val < 3000)    //U0幅度超过0.2kV开始判断。
			{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
				if(MeasureData.Protect_UA_val<3000 && MeasureData.Protect_UB_val> 3000 && MeasureData.Protect_UC_val>3000) jdxb=1;   //相电压较低且U0较高
				if(MeasureData.Protect_UA_val>3000 && MeasureData.Protect_UB_val< 3000 && MeasureData.Protect_UC_val>3000) jdxb=2;
				if(MeasureData.Protect_UA_val>3000 && MeasureData.Protect_UB_val> 3000 && MeasureData.Protect_UC_val<3000) jdxb=3;
			}  
			else
			{
				if(zxdjdfs==1 || zxdjdfs==0)    // '中性点不接地
				{
					if(u0pdxw > 7500 && u0pdxw < 19501) jdxb=1;
					if(u0pdxw >=31500 ||(u0pdxw >=0 && u0pdxw < 7501)) jdxb=2;   //正序情况下
					if(u0pdxw > 19500 && u0pdxw < 31500) jdxb=3;                        
				}
				if(zxdjdfs==2)       //'中性点消线圈接地
				{
					if(u0pdxw > 16500 && u0pdxw < 28501) jdxb=1;
					if(u0pdxw >=4500  && u0pdxw < 16501) jdxb=2;    //正序情况下
					if(u0pdxw > 28500||(u0pdxw > 0 && u0pdxw < 4501))  jdxb=3;   
				}
			}
			
			 //电源侧与负载侧的判断
			if(zxdjdfs == 1)      //'中性点不接地
			{
				if(iuxw > 18000)    
				{
					dycfzc_cnt1++;
					dycfzc_cnt2=0;
					if(dycfzc_cnt1>15)
					{
						dycfzc2=2;
						dycfzc_cnt1=0;
					} 
				}
				else if(iuxw > 3000  && iuxw < 18000)
				{
					dycfzc_cnt2++;
					dycfzc_cnt1=0;
					if(dycfzc_cnt2>15)
					{
						dycfzc2=1;
						dycfzc_cnt2=0;
					}
				} 
			}
			if(zxdjdfs==2)         // '中性点消弧线圈接地
			{
				dycfzc2 = 0;
	//      if (iuxw>9000  && iuxw < 18000) dycfzc2=2;   //U0较低时认为是消弧线圈过补偿的。
	//      //if (iuxw>18000 && iuxw < 33000)  dycfzc2=2;
	//      if (iuxw>0000  && iuxw < 9000)  dycfzc2=1;
			}
	//    if(zxdjdfs==3)      // '中性点通过电阻接地
	//    {
	//      //  'If Val(VQ0(2).Text) > 0 And (Val(VDEG(2).Text)) > -180 And (VDEG(2).Text) < -30 Then Label7.Caption = "负载侧"
	//      //  'If Val(VQ0(2).Text) < 0 And Val(VDEG(2).Text) > 30 And (VDEG(2).Text) < 180 Then Label7.Caption = "电源侧"
	//    }
			
			//接地性质的判断
			if(MeasureData.Protect_UA_val <350 || MeasureData.Protect_UB_val <350 || MeasureData.Protect_UC_val < 350) jdxz=1;     //金属性接地
			else
			{
				if((MeasureData.Protect_UA_val < 6500)&&(MeasureData.Protect_UA_val > 1000))
				{
					if((abs(MeasureData.Protect_UA_val-MeasureData.Protect_UB_val)>300)&&(abs(MeasureData.Protect_UA_val-MeasureData.Protect_UC_val)>300))	jdxz=2;
				}
				else if((MeasureData.Protect_UB_val < 6500)&&(MeasureData.Protect_UB_val > 1000))
				{
					if((abs(MeasureData.Protect_UB_val-MeasureData.Protect_UA_val)>300)&&(abs(MeasureData.Protect_UB_val-MeasureData.Protect_UC_val)>300))	jdxz=2;
				}
				else if((MeasureData.Protect_UC_val < 6500)&&(MeasureData.Protect_UC_val > 1000))
				{
					if((abs(MeasureData.Protect_UC_val-MeasureData.Protect_UA_val)>300)&&(abs(MeasureData.Protect_UC_val-MeasureData.Protect_UB_val)>300))	jdxz=2;
				}
				if(FXBZU0>2000 || MeasureData.Protect_UA_val > 10000 || MeasureData.Protect_UB_val > 10000 || MeasureData.Protect_UC_val > 10000) 
					jdxz=3;  //弧光接地 //总谐波>20%,认为是弧光接地。
			}
			if(jdxz==3)      //弧光过电压的判断
			{
				if(MeasureData.Protect_UA_val >12000 && MeasureData.Protect_UA_val> MeasureData.Protect_UB_val && MeasureData.Protect_UA_val>MeasureData.Protect_UC_val) 
				{
					hgxb=1;  
					hggdy=MeasureData.Protect_UA_val;
				}
				if(MeasureData.Protect_UB_val >12000 && MeasureData.Protect_UB_val>MeasureData.Protect_UA_val && MeasureData.Protect_UB_val>MeasureData.Protect_UC_val) 
				{
					hgxb=2;
					hggdy=MeasureData.Protect_UB_val;
				}
				if(MeasureData.Protect_UC_val >12000 && MeasureData.Protect_UC_val>MeasureData.Protect_UA_val && MeasureData.Protect_UC_val>MeasureData.Protect_UB_val) 
				{   
					hgxb=3;
					hggdy=MeasureData.Protect_UC_val;
				}
			}
	  }
  }
  else
  {   
		jdnum=0;
		jdyf=0;    // 接地与否
		jdxb=0;    // 接地相别
		dycfzc=0;  // 接地位置
		dycfzc2=0; // 接地位置
		jdxz=0;    // 接地性质
		hgxb=0;
		hggdy=0;   //弧光过电压的电压值，在有弧光截的时候赋值，在没有接地的时候清零		
  }   
}



