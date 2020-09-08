#include "global.h"
#include "2812_FFT.h"
#include "math.h"

#define FFT_NUM 64

struct Harmonic HarmonicPercent;

/*
u8 datacal=0;
u16   pfha;	   //1 : ÓÐ¹¦¹¦ÂÊÎª¸ºÖµ
u16   qfha;																																							
u16 UAC_AD=0;
u16 IB_AD=0;
u16 V0_AD=0;
s32 PFA_AD=0;
s32 PA_AD=0;
s32 QA_AD=0;
u32 SA_AD=0;
///´æ·ÅÂË²¨ADÀÛ¼ÓºÍ½á¹û///

u32 UAC_SUM=0;
u32 IB_SUM=0;
u32 V0_SUM=0;
u32 F_SUM=0;
s32 PFA_SUM=0;
s32 PA_SUM=0;
s32 QA_SUM=0;

u8	Time_C;	
u8  uajsq=0;
u8  ubjsq=0;
u8  ucjsq=0;
u8  iajsq=0;
u8  ibjsq=0;
u8  icjsq=0;
u16 ADWD[2];
u8  iamin,ibmin,icmin;

u8 xbjsq[6];
u8 pdcs=0;
*/

	
s32 Real[FFT_NUM];
s32 Imag[FFT_NUM];


//new add
u16   FJBUA,FJBUB,FJBUC,FJBIA,FJBIB,FJBIC,FJBU0,FJBI0,FJBUAB;           //FFT¼ÆËãµÄ·ùÖµ
u16   FFTCntUA,FFTCntUB,FFTCntUC,FFTCntIA,FFTCntIB,FFTCntIC,FFTCntU0,FFTCntI0;
u16   FDEGUA,FDEGUB,FDEGUC,FDEGIA,FDEGIB,FDEGIC,FDEGU0,FDEGI0,FDEGUAB,FDEGUPT1,FDEGUPT2;  //FFT¼ÆËã³öµÄ»ù²¨ÐÎ½Ç¶È
u16   FXBZUA,FXBZUB,FXBZUC,FXBZIA,FXBZIB,FXBZIC,FXBZU0,FXBZI0,FXBZUAB;
s32 PI1;
//FFTÏà¹ØÄÚ´æ
u32  jbfl,xbfl,xbb,zxbfl,xbflz,xbfl2,xbfl3,xbfl5,xbfl7,xbfl11,xbfl13,xbfl17,xbfl19;
u16   jbfd,xbbz,xbb2,xbb3,xbb5,xbb7,xbb11,xbb13,xbb17,xbb19;    //Ð³²¨±ÈÀý
u16   FXB2UA,FXB2UB,FXB2UC,FXB2IA,FXB2IB,FXB2IC,FXB2U0,FXB2I0,FXB2UAB;
u16   FXB3UA,FXB3UB,FXB3UC,FXB3IA,FXB3IB,FXB3IC,FXB3U0,FXB3I0,FXB3UAB;
u16   FXB5UA,FXB5UB,FXB5UC,FXB5IA,FXB5IB,FXB5IC,FXB5U0,FXB5I0,FXB5UAB;
u16   FXB7UA,FXB7UB,FXB7UC,FXB7IA,FXB7IB,FXB7IC,FXB7U0,FXB7I0,FXB7UAB;
u16                               FXB11IA,                    FXB11UAB;
u16                               FXB13IA,                    FXB13UAB;
u16                               FXB17IA,                    FXB17UAB;
u16                               FXB19IA,                    FXB19UAB;

static u32 Display_XB_I_sum0=0,Display_XB_I_sum1=0,Display_XB_I_sum2=0,Display_XB_I_sum3=0;
static u32 Display_XB_I_sum4=0,Display_XB_I_sum5=0,Display_XB_I_sum6=0,Display_XB_I_sum7=0;
static u32 Display_XB_I_sum8=0;
static u32 Display_XB_U_sum0=0,Display_XB_U_sum1=0,Display_XB_U_sum2=0,Display_XB_U_sum3=0;
static u32 Display_XB_U_sum4=0,Display_XB_U_sum5=0,Display_XB_U_sum6=0,Display_XB_U_sum7=0;
static u32 Display_XB_U_sum8=0;

const s16 COS_TABLE[FFT_NUM/2]= //FFT_Num=64
{
  1024,1019,1004,980,946,

  903,851,792,724,650,

  569,483,392,297,200,

  100,0,-100,-200,-297,

  -392,-483,-569,-650,-724,

  -792,-851,-903,-946,-980,

  -1004,-1019,
};
const s16 SIN_TABLE[FFT_NUM/2]=
 {
  0,100,200,297,392,

  483,569,650,724,792,

  851,903,946,980,1004,

  1019,1024,1019,1004,980,

  946,903,851,792,724,

  650,569,483,392,297,

  200,100
};
const u16 BRTable[32] =
{
  0,  16,  8,  24, 
  4,  20,  12,  28, 
  2,  18,  10,  26, 
  6,  22,  14,  30, 
  1,  17,  9,  25, 
  5,  21,  13, 29, 
  3,  19, 11,  27,
  7,  23,  15, 31
};

void CLUACIBXB(void);
void TJSJ(void);
void Set_Sample(u16 F_vale);	
u16 sqrt_16(u32 x );
void XBBJS1(void);
void DEGJS1(s32  SBU,s32  XBU);
void XBBJS(void);
//-----------------------------------------------------------------------------
// Bit_Reverse
//-----------------------------------------------------------------------------
void InitBitRev(s16 BR_Array[])
{
  u16 swapA, swapB, sw_cnt;           // Swap Indices
  s16 TempStore;
   for (sw_cnt = 0; sw_cnt < FFT_NUM/2; sw_cnt++)
   {
      swapA = sw_cnt;                        // Store current location
      swapB = BRTable[sw_cnt] * 2;           // Retrieve bit-reversed index
      if (swapB > swapA)                     // If the bit-reversed index is
      {                                      // larger than the current index,
         TempStore = BR_Array[swapA];        // the two data locations are
         BR_Array[swapA] = BR_Array[swapB];  // swapped. Using this comparison
         BR_Array[swapB] = TempStore;        // ensures that locations are only
      }                                      // swapped once, and never with
                                             // themselves
      swapA += FFT_NUM/2;                    // Now perform the same operations
      swapB++;                               // on the second half of the data
      if (swapB > swapA)
      {
         TempStore = BR_Array[swapA];
         BR_Array[swapA] = BR_Array[swapB];
         BR_Array[swapB] = TempStore;
      }
   }
} 
/********************************************************************
º¯Êý¹¦ÄÜ£º½øÐÐFFTÔËËã¡£
Èë¿Ú²ÎÊý£ºpIn£ºÊäÈëÊý¾ÝµÄ»º³åÇø£»pRe£º±£´æÊµ²¿µÄ»º³åÇø£»pIm£º±£´æÐé²¿µÄ»º³åÇø¡£
·µ    »Ø£ºÎÞ¡£
±¸    ×¢£ºÔÚµ÷ÓÃ¸Ãº¯ÊýÖ®Ç°Ó¦¸Ãµ÷ÓÃFftInputº¯Êý¶ÔÊý¾Ý½øÐÐµ¹Ðò´¦Àí¡£
********************************************************************/
void FftExe(s16 *pIn, s32 *pRe, s32 *pIm)
{
 s32 i,j;
 s32 BlockSize;
 s32 tr,ti;
 s32 OffSet1,OffSet2;
 s32 OffSet0;
 s32 c,s;  
 for(j=0;j<FFT_NUM;j+=2)//ÏÈ¼ÆËã2µãµÄ
 {
  tr=pIn[j+1];
  pRe[j+1]=(pIn[j]-tr);
  pIm[j+1]=0;
  pRe[j]=(pIn[j]+tr);
  pIm[j]=0;
 }

 for(BlockSize=4;BlockSize<=FFT_NUM;BlockSize<<=1) //ÔÙÒ»²ã²ã¼ÆËã
 {
  for(j=0;j<FFT_NUM;j+=BlockSize)
  {
   for(i=0;i<BlockSize/2;i++)
   {
    OffSet0=FFT_NUM/BlockSize*i;
    c=COS_TABLE[OffSet0];
    s=SIN_TABLE[OffSet0];    
    OffSet1=i+j;
    OffSet2=OffSet1+BlockSize/2;
    tr=(s32)((c*pRe[OffSet2]+s*pIm[OffSet2])>>10);///1024
    ti=(s32)((c*pIm[OffSet2]-s*pRe[OffSet2])>>10);///1024
    pRe[OffSet2]=(pRe[OffSet1]-tr)>>1;
    pIm[OffSet2]=(pIm[OffSet1]-ti)>>1;
    pRe[OffSet1]=(pRe[OffSet1]+tr)>>1;
    pIm[OffSet1]=(pIm[OffSet1]+ti)>>1;
   }
  }
 }
 pRe[0]/=2;
 pIm[0]/=2;
}
/////////////////////////End of function/////////////////////////////


void s100to64(void)
{
  float mu,ms;
  s16  mi,mj;

  for (mi=0;mi<64;mi++)
  {	
	  mu=(float)mi*100 / 64;
	  mj=(s16)mu;
	  ms=mu-mj;
	  /*
	  mt=(1-ms);
	  MeasureCurve.XB_UAB[mi]=(s16)((float)MeasureCurve.XB_UAB[mj] *mt+(float)MeasureCurve.XB_UAB[mj+1]*ms); 
	  MeasureCurve.XB_UAB[mi] = MeasureCurve.XB_UAB[mi] * 30;
	  MeasureCurve.XB_IA[mi]=(s16)((float)MeasureCurve.XB_IA[mj]*mt+(float)MeasureCurve.XB_IA[mj+1]*ms); 
	  MeasureCurve.XB_IA[mi] = MeasureCurve.XB_IA[mi]* 5;
	  */
	  MeasureCurve.XB_UAB[mi]=(s16)((float)MeasureCurve.UABX[mj]+((float)MeasureCurve.UABX[mj+1]-(float)MeasureCurve.UABX[mj])*ms);
	  MeasureCurve.XB_UBC[mi]=(s16)((float)MeasureCurve.UBCX[mj]+((float)MeasureCurve.UBCX[mj+1]-(float)MeasureCurve.UBCX[mj])*ms);
	  MeasureCurve.XB_UCA[mi]=(s16)((float)MeasureCurve.UCAX[mj]+((float)MeasureCurve.UCAX[mj+1]-(float)MeasureCurve.UCAX[mj])*ms);
	  MeasureCurve.XB_UA[mi]=(s16)((float)MeasureCurve.UAX[mj]+((float)MeasureCurve.UAX[mj+1]-(float)MeasureCurve.UAX[mj])*ms);
	  MeasureCurve.XB_UB[mi]=(s16)((float)MeasureCurve.UBX[mj]+((float)MeasureCurve.UBX[mj+1]-(float)MeasureCurve.UBX[mj])*ms);
	  MeasureCurve.XB_UC[mi]=(s16)((float)MeasureCurve.UCX[mj]+((float)MeasureCurve.UCX[mj+1]-(float)MeasureCurve.UCX[mj])*ms);
	  MeasureCurve.XB_IA[mi]=(s16)((float)MeasureCurve.IAX[mj]+((float)MeasureCurve.IAX[mj+1]-(float)MeasureCurve.IAX[mj])*ms);
	  MeasureCurve.XB_IB[mi]=(s16)((float)MeasureCurve.IBX[mj]+((float)MeasureCurve.IBX[mj+1]-(float)MeasureCurve.IBX[mj])*ms);
	  MeasureCurve.XB_IC[mi]=(s16)((float)MeasureCurve.ICX[mj]+((float)MeasureCurve.ICX[mj+1]-(float)MeasureCurve.ICX[mj])*ms);
	  MeasureCurve.XB_U0[mi]=(s16)((float)MeasureCurve.U0X[mj]+((float)MeasureCurve.U0X[mj+1]-(float)MeasureCurve.U0X[mj])*ms);
	  MeasureCurve.XB_I0[mi]=(s16)((float)MeasureCurve.I0X[mj]+((float)MeasureCurve.I0X[mj+1]-(float)MeasureCurve.I0X[mj])*ms);   
    MeasureCurve.XB_UPT1[mi]=(s16)((float)MeasureCurve.UPT1X[mj]+((float)MeasureCurve.UPT1X[mj+1]-(float)MeasureCurve.UPT1X[mj])*ms);
	  MeasureCurve.XB_UPT2[mi]=(s16)((float)MeasureCurve.UPT2X[mj]+((float)MeasureCurve.UPT2X[mj+1]-(float)MeasureCurve.UPT2X[mj])*ms);   
  }
}

void Fast_s100to64(void)
{
  float mu,ms;
  s16  mi,mj;
  for (mi=0;mi<64;mi++)
  {	
	  mu=(float)mi*100 / 64;
	  mj=(s16)mu;
	  ms=mu-mj;	  
	  MeasureCurve.XB_IA[mi]=(s16)((float)MeasureCurve.IAX[mj]+((float)MeasureCurve.IAX[mj+1]-(float)MeasureCurve.IAX[mj])*ms);
	  MeasureCurve.XB_IB[mi]=(s16)((float)MeasureCurve.IBX[mj]+((float)MeasureCurve.IBX[mj+1]-(float)MeasureCurve.IBX[mj])*ms);
	  MeasureCurve.XB_IC[mi]=(s16)((float)MeasureCurve.ICX[mj]+((float)MeasureCurve.ICX[mj+1]-(float)MeasureCurve.ICX[mj])*ms); 
    MeasureCurve.XB_U0[mi]=(s16)((float)MeasureCurve.U0X[mj]+((float)MeasureCurve.U0X[mj+1]-(float)MeasureCurve.U0X[mj])*ms);
	  MeasureCurve.XB_I0[mi]=(s16)((float)MeasureCurve.I0X[mj]+((float)MeasureCurve.I0X[mj+1]-(float)MeasureCurve.I0X[mj])*ms);  		
  }
}

/********************************************************************
º¯Êý¹¦ÄÜ£º²âÁ¿Ð³²¨°Ù·Ö±È
Èë¿Ú²ÎÊý£ºÎÞ
·µ    »Ø£ºÎÞ
±¸    ×¢£ºÎÞ
********************************************************************/
void MeasureHarmonicPercent(void)
{
	u16 k;
	s16 temp_fft_data_max;
	s32 FDEGUA0,FDEGUB0,FDEGUC0,FDEGIA0,FDEGIB0,FDEGIC0,FDEGU00,FDEGI00,FDEGUAB0,FDEGUPT10,FDEGUPT20;  //FFT¼ÆËãµÄ·ùÖµ
	s32 DEGJSBF; //½Ç¶È¼ÆËãÔÝ´æ
  s100to64();
  /////UA///////
	FFTCntUA = 0;	 
	temp_fft_data_max =abs( MeasureCurve.XB_UA[0]);

	for (k=0; k<FFT_NUM/4; k++)
	{   
	  if(abs(MeasureCurve.XB_UA[4*k]) > temp_fft_data_max)
		  temp_fft_data_max =abs( MeasureCurve.XB_UA[4*k]);          
	} 
	if(temp_fft_data_max < 8192)
	{
		if(temp_fft_data_max >20)     //if data less than 512, skip pre-processing of the data
		{
			while(temp_fft_data_max < 8192)
			{
				temp_fft_data_max = temp_fft_data_max << 1;     //·Å´ó´¦Àí
				FFTCntUA++;
			}
		}
  }

  for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_UA[k] = MeasureCurve.XB_UA[k] << FFTCntUA; 
	} 

  /////UB///////
	FFTCntUB = 0;	 
	temp_fft_data_max = abs(MeasureCurve.XB_UB[0]);

	for (k=0; k<FFT_NUM/4; k++)
	{   
	  if(abs(MeasureCurve.XB_UB[4*k]) > temp_fft_data_max)
		  temp_fft_data_max =abs( MeasureCurve.XB_UB[4*k]);           
	} 
	if(temp_fft_data_max < 8192)
	{
	  if(temp_fft_data_max >20)	//if data less than 512, skip pre-processing of the data
		{
			while(temp_fft_data_max < 8192)
			{
				temp_fft_data_max = temp_fft_data_max << 1;     //·Å´ó´¦Àí
				FFTCntUB++;
			}
		}
	}

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_UB[k] = MeasureCurve.XB_UB[k] << FFTCntUB; 
	} 

  /////UC///////
	FFTCntUC = 0;	 
	temp_fft_data_max =abs( MeasureCurve.XB_UC[0]);

	for (k=0; k<FFT_NUM/4; k++)
	{   
	  if(abs(MeasureCurve.XB_UC[4*k]) > temp_fft_data_max)
		  temp_fft_data_max =abs( MeasureCurve.XB_UC[4*k]);           
	} 
	if(temp_fft_data_max < 8192)
	{
	  if(temp_fft_data_max >20)	//if data less than 512, skip pre-processing of the data
		{
			while(temp_fft_data_max < 8192)
			{
				temp_fft_data_max = temp_fft_data_max << 1;     //·Å´ó´¦Àí
				FFTCntUC++;
			}
		}
	}

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_UC[k] = MeasureCurve.XB_UC[k] << FFTCntUC; 
	} 

  /////IA///////
	FFTCntIA = 0;	 
	temp_fft_data_max =abs( MeasureCurve.XB_IA[0]);

	for (k=0; k<FFT_NUM/4; k++)
	{   
	  if(abs(MeasureCurve.XB_IA[4*k]) > temp_fft_data_max)
		  temp_fft_data_max =abs( MeasureCurve.XB_IA[4*k]);           
	} 
	if(temp_fft_data_max < 8192)
	{
		if(temp_fft_data_max >2)	//if data less than 512, skip pre-processing of the data
		{
			while(temp_fft_data_max < 8192)
			{
				temp_fft_data_max = temp_fft_data_max << 1;     //·Å´ó´¦Àí
				FFTCntIA++;
			}
		}
	}

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_IA[k] = MeasureCurve.XB_IA[k] << FFTCntIA; 
	} 

  /////IB///////
	FFTCntIB = 0;	 
	temp_fft_data_max =abs( MeasureCurve.XB_IB[0]);

	for (k=0; k<FFT_NUM/4; k++)
	{   
	  if(abs(MeasureCurve.XB_IB[4*k]) > temp_fft_data_max)
		  temp_fft_data_max =abs( MeasureCurve.XB_IB[4*k]);           
	} 
	if(temp_fft_data_max < 8192)
	{
		if(temp_fft_data_max >2)	//if data less than 512, skip pre-processing of the data
		{
			while(temp_fft_data_max < 8192)
			{
				temp_fft_data_max = temp_fft_data_max << 1;     //·Å´ó´¦Àí
				FFTCntIB++;
			}
		}
	}

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_IB[k] = MeasureCurve.XB_IB[k] << FFTCntIB; 
	} 
	
  /////IC///////
	FFTCntIC = 0;	 
	temp_fft_data_max =abs( MeasureCurve.XB_IC[0]);

	for (k=0; k<FFT_NUM/4; k++)
	{   
	  if(abs(MeasureCurve.XB_IC[4*k]) > temp_fft_data_max)
		  temp_fft_data_max = abs(MeasureCurve.XB_IC[4*k]);           
	} 
	if(temp_fft_data_max < 8192)
	{
		if(temp_fft_data_max >2)	//if data less than 512, skip pre-processing of the data
		{
			while(temp_fft_data_max < 8192)
			{
				temp_fft_data_max = temp_fft_data_max << 1;     //·Å´ó´¦Àí
				FFTCntIC++;
			}
		}
	}

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_IC[k] = MeasureCurve.XB_IC[k] << FFTCntIC; 
	} 

  /////U0///////
	FFTCntU0 = 0;	 
	temp_fft_data_max =abs( MeasureCurve.XB_U0[0]);

	for (k=0; k<FFT_NUM/4; k++)
	{   
	  if(abs(MeasureCurve.XB_U0[4*k]) > temp_fft_data_max)
		  temp_fft_data_max =abs( MeasureCurve.XB_U0[4*k]);           
	} 
	if(temp_fft_data_max < 8192)
	{
		if(temp_fft_data_max >2)	//if data less than 512, skip pre-processing of the data
		{
			while(temp_fft_data_max < 8192)
			{
				temp_fft_data_max = temp_fft_data_max << 1;     //·Å´ó´¦í
				FFTCntU0++;
			}
		}
	}

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_U0[k] = MeasureCurve.XB_U0[k] << FFTCntU0; 
	} 

  /////I0///////
	FFTCntI0 = 0;	 
	temp_fft_data_max =abs( MeasureCurve.XB_I0[0]);

 	for (k=0; k<FFT_NUM/4; k++)
 	{   
	  if(abs(MeasureCurve.XB_I0[4*k]) > temp_fft_data_max)
  	  temp_fft_data_max =abs( MeasureCurve.XB_I0[4*k]);           
	} 
	if(temp_fft_data_max < 8192)
	{
		if(temp_fft_data_max >2)	//if data less than 512, skip pre-processing of the data
		{
			while(temp_fft_data_max < 8192)
			{
				temp_fft_data_max = temp_fft_data_max << 1;     //·Å´ó´¦Àí
				FFTCntI0++;
			}
		}
	}

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_I0[k] = MeasureCurve.XB_I0[k] << FFTCntI0; 
	} 
  /////UPT1µÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_UPT1);
	FftExe(MeasureCurve.XB_UPT1,Real,Imag);
  DEGJS1(Real[1],Imag[1]);
  FDEGUPT10=PI1;
  FDEGUPT1=FDEGUPT10;
	
  /////UPT2µÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_UPT2);
	FftExe(MeasureCurve.XB_UPT2,Real,Imag);
  DEGJS1(Real[1],Imag[1]);
  FDEGUPT20=PI1;
  FDEGUPT2=FDEGUPT20;	
     
  /////UAµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_UA);
	FftExe(MeasureCurve.XB_UA,Real,Imag);
  DEGJS1(Real[1],Imag[1]);
  FDEGUA0=PI1;
  FDEGUA=FDEGUA0;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
  FJBUA=(u16)((jbfl*25029)>>FFTCntUA>>11);//15->11
	jbfd=FJBUA;
	XBBJS();
  FXBZUA=xbbz;
	FXB2UA=xbb2;
	FXB3UA=xbb3;
	FXB5UA=xbb5;
	FXB7UA=xbb7;

  /////UBµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_UB);
	FftExe(MeasureCurve.XB_UB,Real,Imag);
  DEGJS1(Real[1],Imag[1]);
  FDEGUB0=PI1;
  FDEGUB=FDEGUB0;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
  FJBUB=(u16)((jbfl*25029)>>FFTCntUB>>11);//
	jbfd=FJBUB;
	XBBJS();
	FXBZUB=xbbz;
	FXB2UB=xbb2;
	FXB3UB=xbb3;
	FXB5UB=xbb5;
	FXB7UB=xbb7;

  /////UCµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_UC);
	FftExe(MeasureCurve.XB_UC,Real,Imag);
	DEGJS1(Real[1],Imag[1]);
	FDEGUC0=PI1;
	FDEGUC=FDEGUC0;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBUC=(u16)((jbfl*25029)>>FFTCntUC>>11);
 	jbfd=FJBUC;
	XBBJS();
	FXBZUC=xbbz;
	FXB2UC=xbb2;
	FXB3UC=xbb3;
	FXB5UC=xbb5;
	FXB7UC=xbb7;

  /////IAµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_IA);
	FftExe(MeasureCurve.XB_IA,Real,Imag);
	DEGJS1(Real[1],Imag[1]);
	FDEGIA0=PI1;
	FDEGIA=FDEGIA0;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBIA=(u16)((jbfl*42708)>>FFTCntIA>>14);//*2.42;
	jbfd=FJBIA;
	XBBJS();
	XBBJS1();
	FXBZIA=xbbz;
	FXB2IA=xbb2;
	FXB3IA=xbb3;
	FXB5IA=xbb5;
	FXB7IA=xbb7;
	FXB11IA=xbb11;
	FXB13IA=xbb13;
	FXB17IA=xbb17;
	FXB19IA=xbb19;
	MeasureData.XB_I[0]= xbbz;
	MeasureData.XB_I[1]= xbb2;
	MeasureData.XB_I[2]= xbb3;
	MeasureData.XB_I[3]= xbb5;
	MeasureData.XB_I[4]= xbb7;
	MeasureData.XB_I[5]= xbb11;
	MeasureData.XB_I[6]= xbb13;
	MeasureData.XB_I[7]= xbb17;
	MeasureData.XB_I[8]= xbb19;

  Display_XB_I_sum0 += MeasureData.XB_I[0];
	MeasureData.DIS_XB_I[0] = Display_XB_I_sum0>>3;
	Display_XB_I_sum0 -= MeasureData.DIS_XB_I[0];

	Display_XB_I_sum1 += MeasureData.XB_I[1];
	MeasureData.DIS_XB_I[1] = Display_XB_I_sum1>>4;
	Display_XB_I_sum1 -= MeasureData.DIS_XB_I[1];

	Display_XB_I_sum2 += MeasureData.XB_I[2];
	MeasureData.DIS_XB_I[2] = Display_XB_I_sum2>>4;
	Display_XB_I_sum2 -= MeasureData.DIS_XB_I[2];

	Display_XB_I_sum3 += MeasureData.XB_I[3];
	MeasureData.DIS_XB_I[3] = Display_XB_I_sum3>>4;
	Display_XB_I_sum3 -= MeasureData.DIS_XB_I[3];

	Display_XB_I_sum4 += MeasureData.XB_I[4];
	MeasureData.DIS_XB_I[4] = Display_XB_I_sum4>>4;
	Display_XB_I_sum4 -= MeasureData.DIS_XB_I[4];

	Display_XB_I_sum5 += MeasureData.XB_I[5];
	MeasureData.DIS_XB_I[5] = Display_XB_I_sum5>>4;
	Display_XB_I_sum5 -= MeasureData.DIS_XB_I[5];

	Display_XB_I_sum6 += MeasureData.XB_I[6];
	MeasureData.DIS_XB_I[6] = Display_XB_I_sum6>>4;
	Display_XB_I_sum6 -= MeasureData.DIS_XB_I[6];

	Display_XB_I_sum7 += MeasureData.XB_I[7];
	MeasureData.DIS_XB_I[7] = Display_XB_I_sum7>>3;
	Display_XB_I_sum7 -= MeasureData.DIS_XB_I[7];

	Display_XB_I_sum8 += MeasureData.XB_I[8];
	MeasureData.DIS_XB_I[8] = Display_XB_I_sum8>>3;
	Display_XB_I_sum8 -= MeasureData.DIS_XB_I[8];

  /////IBµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_IB);
	FftExe(MeasureCurve.XB_IB,Real,Imag);
	DEGJS1(Real[1],Imag[1]);
	FDEGIB0=PI1;
	FDEGIB=FDEGIB0;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBIB=(u16)((jbfl*42708)>>FFTCntIB>>14);//*2.42;
	jbfd=FJBIB;
	XBBJS();
	FXBZIB=xbbz;
	FXB2IB=xbb2;
	FXB3IB=xbb3;
	FXB5IB=xbb5;
	FXB7IB=xbb7;

  /////ICµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_IC);
	FftExe(MeasureCurve.XB_IC,Real,Imag);
	DEGJS1(Real[1],Imag[1]);
	FDEGIC0=PI1;
	FDEGIC=FDEGIC0;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBIC=(u16)((jbfl*42708)>>FFTCntIC>>14);//*2.42;
	jbfd=FJBIC;
	XBBJS();
	FXBZIC=xbbz;
	FXB2IC=xbb2;
	FXB3IC=xbb3;
	FXB5IC=xbb5;
	FXB7IC=xbb7;
	 
  /////U0µÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_U0);
 	FftExe(MeasureCurve.XB_U0,Real,Imag);         //ËãFFT
	DEGJS1(Real[1],Imag[1]);
	FDEGU00=PI1;
	FDEGU0=FDEGU00;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBU0=(u16)((jbfl*25029)>>FFTCntU0>>11); //»ù²¨
	jbfd=FJBU0;
	XBBJS();         //Ð³²¨·ÖÁ¿¼ÆËã
	FXBZU0=xbbz; 
	FXB2U0=xbb2; 
	FXB3U0=xbb3; 
	FXB5U0=xbb5;
	FXB7U0=xbb7;

  ////////I0µÄ¼ÆËã//////
	InitBitRev(MeasureCurve.XB_I0);
	FftExe(MeasureCurve.XB_I0,Real,Imag);
	DEGJS1(Real[1],Imag[1]);
	FDEGI00=PI1;
	FDEGI0=FDEGI00;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBI0=(u16)((jbfl*35376)>>FFTCntI0>>15);//*1.87;
	jbfd=FJBI0;
	XBBJS();
	FXBZI0=xbbz;
	FXB2I0=xbb2;
	FXB3I0=xbb3;
	FXB5I0=xbb5;
	FXB7I0=xbb7;;

  ////////UABµÄ¼ÆËã//////
	InitBitRev(MeasureCurve.XB_UAB);
	FftExe(MeasureCurve.XB_UAB,Real,Imag);
	DEGJS1(Real[1],Imag[1]);
	FDEGUAB0=PI1;
	FDEGUAB=FDEGUAB0;
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBUAB=(u16)((jbfl*25029)>>11);
	jbfd=FJBUAB;
	XBBJS();
	XBBJS1();
	FXBZUAB=xbbz;
	FXB2UAB=xbb2;
	FXB3UAB=xbb3;
	FXB5UAB=xbb5;
	FXB7UAB=xbb7;
	FXB11UAB=xbb11;
	FXB13UAB=xbb13;
	FXB17UAB=xbb17;
	FXB19UAB=xbb19;

	MeasureData.XB_U[0]= xbbz;
	MeasureData.XB_U[1]= xbb2;
	MeasureData.XB_U[2]= xbb3;
	MeasureData.XB_U[3]= xbb5;
	MeasureData.XB_U[4]= xbb7;
	MeasureData.XB_U[5]= xbb11;
	MeasureData.XB_U[6]= xbb13;
	MeasureData.XB_U[7]= xbb17;
	MeasureData.XB_U[8]= xbb19; 

	Display_XB_U_sum0 += MeasureData.XB_U[0];
	MeasureData.DIS_XB_U[0] = Display_XB_U_sum0>>4;
	Display_XB_U_sum0 -= MeasureData.DIS_XB_U[0];

	Display_XB_U_sum1 += MeasureData.XB_U[1];
	MeasureData.DIS_XB_U[1] = Display_XB_U_sum1>>4;
	Display_XB_U_sum1 -= MeasureData.DIS_XB_U[1];

	Display_XB_U_sum2 += MeasureData.XB_U[2];
	MeasureData.DIS_XB_U[2] = Display_XB_U_sum2>>4;
	Display_XB_U_sum2 -= MeasureData.DIS_XB_U[2];

	Display_XB_U_sum3 += MeasureData.XB_U[3];
	MeasureData.DIS_XB_U[3] = Display_XB_U_sum3>>4;
	Display_XB_U_sum3 -= MeasureData.DIS_XB_U[3];

	Display_XB_U_sum4 += MeasureData.XB_U[4];
	MeasureData.DIS_XB_U[4] = Display_XB_U_sum4>>4;
	Display_XB_U_sum4 -= MeasureData.DIS_XB_U[4];

	Display_XB_U_sum5 += MeasureData.XB_U[5];
	MeasureData.DIS_XB_U[5] = Display_XB_U_sum5>>4;
	Display_XB_U_sum5 -= MeasureData.DIS_XB_U[5];

	Display_XB_U_sum6 += MeasureData.XB_U[6];
	MeasureData.DIS_XB_U[6] = Display_XB_U_sum6>>4;
	Display_XB_U_sum6 -= MeasureData.DIS_XB_U[6];

	Display_XB_U_sum7 += MeasureData.XB_U[7];
	MeasureData.DIS_XB_U[7] = Display_XB_U_sum7>>4;
	Display_XB_U_sum7 -= MeasureData.DIS_XB_U[7];

	Display_XB_U_sum8 += MeasureData.XB_U[8];
	MeasureData.DIS_XB_U[8] = Display_XB_U_sum8>>4;
	Display_XB_U_sum8 -= MeasureData.DIS_XB_U[8];

  ////////////////
	DEGJSBF=FDEGUA0-FDEGUAB0;

	if(DEGJSBF > 2500 && DEGJSBF < 3500)
	{
		DEGJSBF=FDEGUA0-FDEGUAB0-3000;          //ÒÔUABµÄÏàÎ»×÷Îª»ù×¼¡£
		if(DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if(DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGUA=(u16)DEGJSBF;
		DEGJSBF=FDEGUB0-FDEGUAB0-3000;
		if(DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if(DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGUB=(u16)DEGJSBF;
		DEGJSBF=FDEGUC0-FDEGUAB0-3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGUC=(u16)DEGJSBF;

		DEGJSBF=FDEGIA0-FDEGUAB0-3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGIA=(u16)DEGJSBF;
		DEGJSBF=FDEGIB0-FDEGUAB0-3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGIB=(u16)DEGJSBF;
		DEGJSBF=FDEGIC0-FDEGUAB0-3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGIC=(u16)DEGJSBF;

		DEGJSBF=FDEGU00-FDEGUAB0-3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGU0=(u16)DEGJSBF;
		DEGJSBF=FDEGI00-FDEGUAB0-3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGI0=(u16)DEGJSBF;
		FDEGUAB=33000;
  }
	else if(DEGJSBF < (-2500)  && DEGJSBF > (-3500))
	{
		DEGJSBF=FDEGUA0-FDEGUAB0+3000;          //ÒÔUABµÄÏàÎ»×÷Îª»ù×¼¡£
		if(DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if(DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGUA=(u16)DEGJSBF;
		DEGJSBF=FDEGUB0-FDEGUAB0+3000;
		if(DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if(DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGUB=(u16)DEGJSBF;
		DEGJSBF=FDEGUC0-FDEGUAB0+3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGUC=(u16)DEGJSBF;

		DEGJSBF=FDEGIA0-FDEGUAB0+3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGIA=(u16)DEGJSBF;
		DEGJSBF=FDEGIB0-FDEGUAB0+3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGIB=(u16)DEGJSBF;
		DEGJSBF=FDEGIC0-FDEGUAB0+3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGIC=(u16)DEGJSBF;

		DEGJSBF=FDEGU00-FDEGUAB0+3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGU0=(u16)DEGJSBF;
		DEGJSBF=FDEGI00-FDEGUAB0+3000;
		if (DEGJSBF<0) DEGJSBF=DEGJSBF+36000;
		else if (DEGJSBF>36000) DEGJSBF=DEGJSBF-36000;
		FDEGI0=(u16)DEGJSBF;
		FDEGUAB=3000;
	}
}

/********************************************************************
º¯Êý¹¦ÄÜ£º¿ìËÙ²âÁ¿Ð³²¨°Ù·Ö±È
Èë¿Ú²ÎÊý£ºÎÞ
·µ    »Ø£ºÎÞ
±¸    ×¢£ºÎÞ
********************************************************************/
void Measure_FastHarmonic_Percent(void)
{
	u16 k;
  Fast_s100to64();

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_IA[k] = MeasureCurve.XB_IA[k] << FFTCntIA; 
	} 

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_IB[k] = MeasureCurve.XB_IB[k] << FFTCntIB; 
	} 

	for(k=0; k<FFT_NUM; k++)
	{    
	  MeasureCurve.XB_IC[k] = MeasureCurve.XB_IC[k] << FFTCntIC; 
	} 
	
	/////IAµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_IA);
	FftExe(MeasureCurve.XB_IA,Real,Imag);
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBIA=(u16)((jbfl*42708)>>FFTCntIA>>14);//*2.42;
	jbfd=FJBIA;
	if(jbfd>100)
  {
	  xbfl2=sqrt_16(Real[2]*Real[2]+Imag[2]*Imag[2]);//2´ÎÐ³²¨
	  xbb2=xbfl2*10000/jbfl;
	}
	else
		xbb2 = 0;
	FXB2IA=xbb2;

  /////IBµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_IB);
	FftExe(MeasureCurve.XB_IB,Real,Imag);
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBIB=(u16)((jbfl*42708)>>FFTCntIB>>14);//*2.42;
	jbfd=FJBIB;
	if(jbfd>100)
  {
	  xbfl2=sqrt_16(Real[2]*Real[2]+Imag[2]*Imag[2]);//2´ÎÐ³²¨
	  xbb2=xbfl2*10000/jbfl;
	}
	else
		xbb2 = 0;
	FXB2IB=xbb2;

  /////ICµÄ¼ÆËã
	InitBitRev(MeasureCurve.XB_IC);
	FftExe(MeasureCurve.XB_IC,Real,Imag);
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	FJBIC=(u16)((jbfl*42708)>>FFTCntIC>>14);//*2.42;
	jbfd=FJBIC;
	if(jbfd>100)
  {
	  xbfl2=sqrt_16(Real[2]*Real[2]+Imag[2]*Imag[2]);//2´ÎÐ³²¨
	  xbb2=xbfl2*10000/jbfl;
	}
	else
		xbb2 = 0;
	FXB2IC=xbb2;
}

//===============================================
/*
º¯ÊýÃû£ºsqrt_16
¹¦ÄÜ£º  ÊµÏÖ¶Ô32Î»¶¨µãÊýµÄ¿ª·½
ÐÔÄÜ£º  60MÖ÷Æµ28015Ó²¼þÏÂÔËÐÐÊ±¼äÐ¡ÓÚ10us
*/
//===============================================
u16 sqrt_16(u32 x )
{
 	u32 x1;
	u16 s=1;
	u32 g0,g1;

	if(x<=1)  return x;

	x1=x-1;
	if(x1>65535)
	{
		s+=8;
		x1>>=16;
	}
	if(x1>255)
	{
		s+=4;
		x1>>=8;
	}
	if(x1>15)
	{
		s+=2;
		x1>>=4;
	}
	if(x1>3)
	{
	  s+=1;
	}
	g0=1;
	g0=g0<<s;
	g1 =(g0 +(x>>s))>>1;

	while(g1<g0)
	{
		g0=g1;
		g1=(g0+x/g0)>>1;
	}
  return (u16)(g0);
}

void DEGJS1(s32 SBU,s32 XBU)    //ÊäÈëÊµ²¿ºÍÐé²¿£¬ÓÃ¹«¹²±äÁ¿Êä³ö½Ç¶È
{
	float  bf1;
	bf1=(float)XBU/(float)SBU;       //16Î»¸¡µãÊý

	if  (abs(SBU)<0.1) 
	{
		if  ((XBU*SBU)>0)
		{
		  PI1=9000;
		}
		else
		{
		  PI1=-9000;
		}
  }
	else
	{
	  PI1=(s16)(atan(bf1)*(36000)/6.2832);  
	}
	PI1=PI1+3000;             //¼Ó30¡ã
	if (SBU > 0)
	{
	  PI1=PI1+18000;      // ·´Ïàµ÷Õû
	}
	if (PI1 < 0)  
	{
	  PI1=PI1+36000;      //ÔÝÊ±²ÉÓÃ¹«¹²±äÁ¿PI·µ»Ø¡£
	}
}
void XBBJS1(void)
{
	if(jbfd>100)
	{
		xbfl11=sqrt_16(Real[11]*Real[11]+Imag[11]*Imag[11]);//11´ÎÐ³²¨
		xbfl13=sqrt_16(Real[13]*Real[13]+Imag[13]*Imag[13]);//13´ÎÐ³²¨
		xbfl17=sqrt_16(Real[17]*Real[17]+Imag[17]*Imag[17]);//17´ÎÐ³²¨
		xbfl19=sqrt_16(Real[19]*Real[19]+Imag[19]*Imag[19]);//19´ÎÐ³²¨
		xbb11=xbfl11*10000/jbfl;
		xbb13=xbfl13*10000/jbfl;
		xbb17=xbfl17*10000/jbfl;
		xbb19=xbfl19*10000/jbfl;
  }
  else
  {
		xbb11=0;     //»ù²¨·ù¶È¹ýÐ¡£¬Ð³²¨È¡0.
		xbb13=0;
		xbb17=0;
		xbb19=0;
  }
}
void XBBJS(void)
{
  u16 i;
  if (jbfd>100)
  {
		zxbfl=0;  //×ÜÐ³²¨·ÖÁ¿
		for(i=2;i<20;i++)
		{
			zxbfl+=(Real[i]*Real[i]+Imag[i]*Imag[i]);
		}
		jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
		xbflz=sqrt_16(zxbfl);//×ÜÐ³²¨
		xbfl2=sqrt_16(Real[2]*Real[2]+Imag[2]*Imag[2]);//2´ÎÐ³²¨
		xbfl3=sqrt_16(Real[3]*Real[3]+Imag[3]*Imag[3]);//3´ÎÐ³²¨
		xbfl5=sqrt_16(Real[5]*Real[5]+Imag[5]*Imag[5]);//5´ÎÐ³²¨
		xbfl7=sqrt_16(Real[7]*Real[7]+Imag[7]*Imag[7]);//7´ÎÐ³²¨ 
		xbbz=xbflz*10000/jbfl;
		xbb2=xbfl2*10000/jbfl;
		xbb3=xbfl3*10000/jbfl;
		xbb5=xbfl5*10000/jbfl;
		xbb7=xbfl7*10000/jbfl;
  }
  else 
  {
		xbbz=0;     //»ù²¨·ù¶È¹ýÐ¡£¬Ð³²¨È¡0.
		xbb2=0;
		xbb3=0;
		xbb5=0;
		xbb7=0;
  }
}

/*
void MeasureHarmonicPercent(void)
{	u16 i;u32 jbfl,xbfl,xbb,zxbfl;
	static u16 XBU[6] = {0};
	static u16 XBI[6] = {0};
	static u16 state = 0;
	s100to64();
	state++;
	if(state>2)
	{
		state = 0;
		for(i=0;i<6;i++)
		{
			MeasureData.XB_U[i] = XBU[i];
			MeasureData.XB_I[i] = XBI[i];
		}
		for(i=0;i<6;i++)
		{
			XBU[i] = 0;
			XBI[i] = 0;
		}
	}	 
	InitBitRev(MeasureCurve.XB_UAB);
	FftExe(MeasureCurve.XB_UAB,Real,Imag);
	zxbfl=0;
	for(i=3;i<12;i+=2)
	{
		zxbfl+=(Real[i]*Real[i]+Imag[i]*Imag[i]);
	}
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	if(jbfl==0)
		jbfl = 65535;
	
	xbfl=sqrt_16(zxbfl);//×ÜÐ³²¨
	if(MeasureData.UAB_val<100)//µçÑ¹Îª0
	{
		xbfl=0;
	}	
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBU[0] < xbb)
		XBU[0] = xbb;	
	xbfl=sqrt_16(Real[3]*Real[3]+Imag[3]*Imag[3]);//3´ÎÐ³²¨
	if(MeasureData.UAB_val<100)//µçÑ¹Îª0
	{
		xbfl=0;
	}	
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBU[1] < xbb)
		XBU[1] = xbb;

	xbfl=sqrt_16(Real[5]*Real[5]+Imag[5]*Imag[5]);//5´ÎÐ³²¨
	if(MeasureData.UAB_val<100)//µçÑ¹Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBU[2] < xbb)
		XBU[2] = xbb;
	
	xbfl=sqrt_16(Real[7]*Real[7]+Imag[7]*Imag[7]);//7´ÎÐ³²¨
	if(MeasureData.UAB_val<100)//µçÑ¹Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBU[3] < xbb)
		XBU[3] = xbb;
	
	xbfl=sqrt_16(Real[9]*Real[9]+Imag[9]*Imag[9]);//9´ÎÐ³²¨
	if(MeasureData.UAB_val<100)//µçÑ¹Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBU[4] < xbb)
		XBU[4] = xbb;
	
	xbfl=sqrt_16(Real[11]*Real[11]+Imag[11]*Imag[11]);//11´ÎÐ³²¨
	if(MeasureData.UAB_val < 100)//µçÑ¹Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBU[5] < xbb)
		XBU[5] = xbb;
	
	InitBitRev(MeasureCurve.XB_IA);
	FftExe(MeasureCurve.XB_IA,Real,Imag);
	zxbfl=0;
	for(i=3;i<12;i+=2)
	{
		zxbfl+=(Real[i]*Real[i]+Imag[i]*Imag[i]);
	}
	jbfl=sqrt_16(Real[1]*Real[1]+Imag[1]*Imag[1]);//»ù²¨
	if(jbfl==0)
		jbfl = 65535;
	xbfl=sqrt_16(zxbfl);//×ÜÐ³²¨
	if(MeasureData.IA_val<100)//µçÁ÷Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBI[0] < xbb)
		XBI[0] = xbb;
	
	xbfl=sqrt_16(Real[3]*Real[3]+Imag[3]*Imag[3]);//3´ÎÐ³²¨
	if(MeasureData.IA_val<100)//µçÁ÷Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBI[1] < xbb)
		XBI[1] = xbb;
	
	xbfl=sqrt_16(Real[5]*Real[5]+Imag[5]*Imag[5]);//5´ÎÐ³²¨
	if(MeasureData.IA_val<100)//µçÁ÷Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBI[2] < xbb)
		XBI[2] = xbb;
	
	xbfl=sqrt_16(Real[7]*Real[7]+Imag[7]*Imag[7]);//7´ÎÐ³²¨
	if(MeasureData.IA_val<100)//µçÁ÷Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBI[3] < xbb)
		XBI[3] = xbb;
		
	xbfl=sqrt_16(Real[9]*Real[9]+Imag[9]*Imag[9]);//9´ÎÐ³²¨
	if(MeasureData.IA_val<100)//µçÁ÷Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBI[4] < xbb)
		XBI[4] = xbb;
	
	xbfl=sqrt_16(Real[11]*Real[11]+Imag[11]*Imag[11]);//11´ÎÐ³²¨
	if(MeasureData.IA_val<100)//µçÁ÷Îª0
	{
		xbfl=0;
	}
	xbfl*=10000;
	xbb=xbfl/jbfl;
	if(XBI[5] < xbb)
		XBI[5] = xbb;
	
	
}
*/
