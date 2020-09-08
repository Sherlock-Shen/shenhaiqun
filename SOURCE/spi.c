#include "spi.h"
#include "global.h"
#include "protocol1.h"

//串行外设接口SPI的初始化，SPI配置成主模式							  

//SPI1初始化
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

//SPI1读写一字节数据
u8 SPI1_ReadWrite(u8 writedat)
{
 	u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=writedat;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0)//等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //返回收到的数据	
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
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//168/4/2=21,推荐5~20MHz
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;
   SPI_Init(SPI2,&SPI_InitStructure);
	
   /* Enable SPI2  */
   SPI_Cmd(SPI2, ENABLE);
}

//SPI2读写一字节数据
u8 SPI2_ReadWrite(u8 writedat)
{
	u8 retry=0;				 
	while((SPI2->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI2->DR=writedat;	 	  //发送一个byte 
	retry=0;
	while((SPI2->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI2->DR;          //返回收到的数据		
}

void SSN_L()
{
	u16 count=0;
	SPI2_FLASH_CS_L;								//拉低后最少需要延时10us
	while(count++<1000);				//1us大约41.7次自加指令
}

void SSN_H()
{
	u16 count=0;
	while(count++<500);				//3us后拉高
	SPI2_FLASH_CS_H;		
	count=0;
	while(count++<1500);				//拉高后最少维持10us
}


