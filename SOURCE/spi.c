#include "spi.h"
#include "global.h"
#include "protocol1.h"

//��������ӿ�SPI�ĳ�ʼ����SPI���ó���ģʽ							  

//SPI1��ʼ��
void SPI1_Init(void)
{
   SPI_InitTypeDef SPI_InitStructure;
   /* SPI1 configuration */ 
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;
   SPI_Init(SPI1, &SPI_InitStructure);

   /* Enable SPI1  */
   SPI_Cmd(SPI1, ENABLE);
}

//SPI1��дһ�ֽ�����
u8 SPI1_ReadWrite(u8 writedat)
{
 	u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//�ȴ���������	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=writedat;	 	  //����һ��byte 
	retry=0;
	while((SPI1->SR&1<<0)==0)//�ȴ�������һ��byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //�����յ�������	
}

void SPI2_Init(void)
{
   SPI_InitTypeDef SPI_InitStructure;
   /* SPI2 configuration */ 
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//168/4/2=21,�Ƽ�5~20MHz
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;
   SPI_Init(SPI2,&SPI_InitStructure);
	
   /* Enable SPI2  */
   SPI_Cmd(SPI2, ENABLE);
}

//SPI2��дһ�ֽ�����
u8 SPI2_ReadWrite(u8 writedat)
{
	u8 retry=0;				 
	while((SPI2->SR&1<<1)==0)//�ȴ���������	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI2->DR=writedat;	 	  //����һ��byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) //�ȴ�������һ��byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI2->DR;          //�����յ�������		
}

void SSN_L()
{
	u16 count=0;
	SPI2_FLASH_CS_L;								//���ͺ�������Ҫ��ʱ10us
	while(count++<1000);				//1us��Լ41.7���Լ�ָ��
}

void SSN_H()
{
	u16 count=0;
	while(count++<500);				//3us������
	SPI2_FLASH_CS_H;		
	count=0;
	while(count++<1500);				//���ߺ�����ά��10us
}


