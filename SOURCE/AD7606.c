#include "AD7606.h"    
#include "global.h"


typedef struct 
{
	u16 AD7606_RAM;
}AD7606_TypeDef;

#define Bank1_NORSRAM1_ADDR ((u32)0x60000000) 
#define AD7606              ((AD7606_TypeDef *) Bank1_NORSRAM1_ADDR)



void AD7606_delay_US(u32 nCount)     
{
 for(; nCount != 0; nCount--);
}

void AD7606Reset(void)
{   
  AD7606_RESET_0;
	AD7606_delay_US(12);
	AD7606_RESET_1;          //上升沿复位AD7606
	AD7606_delay_US(12);
	AD7606_RESET_0;
}

void AD7606START(void)
{
	AD7606_CONVST_0;
	AD7606_delay_US(12);  //延迟至少45ns
	AD7606_CONVST_1;
}

//void AD7606Read(void)
//{
//  u8 i;
//  s16 AD7606DATA[8];
//  for(i=0;i<8;i++)
//	{
//	  AD7606DATA[i]= *(s16*) Bank1_SRAM1_ADDR;
//  }
//}

//void AD7606Read(void)
//{
//	unsigned char j;
////	unsigned char Busy;
//     int  ADCDATA[16]={0}; 


//   AD7606_delay_US(3);
//   for(j=0; j<16; j++)
//	{
//		ADCDATA[j]= *((unsigned int *)0x100000);
//        ADCDATA[j]=(ADCDATA[j]&0x00FF);
//    }
//   for(j=0; j<16; j+=2)
//   {
//   AD7606DATA[j/2]=(ADCDATA[j]<<8)+ADCDATA[j+1];     
//   }
//   
//}



void AD7606ReadBuffer(u16* pBuffer)
{
	unsigned char j;
	AD7606_delay_US(3);
	for(j=0; j<8; j++)
	{
//		*pBuffer++=*(vu16*)(Bank1_NORSRAM1_ADDR+j*2);
		*pBuffer++=AD7606->AD7606_RAM;
	}
}


