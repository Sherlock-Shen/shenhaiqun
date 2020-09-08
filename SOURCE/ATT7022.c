#include "ATT7022.h"
#include <math.h>

void delay(u16 s)     
{
 u16 x,y;
	 for(x=s;x>0;x--)
	 {
	 	for(y=5;y>0;y--);
	 }
}

void EnableCs(void)
{
     ATT_CS_1;
     delay(5);     
     ATT_SCLK_0;
     delay(5); 
     ATT_CS_0;    
}

void DisableCs(void)
{
     ATT_CS_1;
}

void SpiSend1Byte(u8 Data)
{
	u8 i;
	for(i=0;i<8;i++)
	{	
	   ATT_SCLK_1;
       delay(5); 
       if (Data&0x80) ATT_SDI_1;                   
       else ATT_SDI_0;
	   delay(5);  
	   ATT_SCLK_0;	
	   Data=Data<<1;		
	}		
}

void SpiSend3Byte(u32 Data)
{
	u8 i;
	for(i=0;i<24;i++)
	{	
	  ATT_SCLK_1;
    delay(5);  
    if (Data&0x800000) ATT_SDI_1;                   
    else ATT_SDI_0;
	  delay(5); 
	  ATT_SCLK_0;	
	  Data=Data<<1;		
	}		
}

u32 SpiReceive3Byte(void)
{
   u8  i;
   u32 Data=0;

   for(i=0;i<24;i++)
   { 
     ATT_SCLK_1;
	   delay(5);
     Data=Data<<1;
     if(ATT_SDO) Data+=1;
	   delay(5);
     ATT_SCLK_0;	
   }
   return Data;
}

void WriteAT7052(u8 Command, u32 dat)
{	 
	 EnableCs();
   delay(5);
 	 Command|= 0x80;
	 SpiSend1Byte(Command);	  
	 SpiSend3Byte(dat);
   delay(5);
	 DisableCs();  	 
}

u32 ReadAT7052(u8 Command)
{ 
	 u32 dat;
	 EnableCs();
   delay(5);
	 SpiSend1Byte(Command);		 
	 delay(5);          
	 dat=SpiReceive3Byte();
   delay(5);
	 DisableCs();  
	 return dat;
}



void ATT7022_Clear(void)
{
	WriteAT7052(0xC3,0x000000);	// ���У������
}

void ATT7022_WREnable(void)
{
	WriteAT7052(0xC9,0x00005A);	// У������дʹ��
}

void ATT7022_WRDisable(void)
{
	WriteAT7052(0xC9,0x000000);	// У�����ݹر�дʹ��
}

void ATT7022_RDEnable(void)		// ��У�����ʹ��
{
    WriteAT7052(0xC6,0x00005A);	// 	
}

void ATT7022_RDDisable(void)	// ��У������ر�ʹ��
{
    WriteAT7052(0xC6,0x000000);	// 	
}

void ATT7022_Reset(void)		// оƬ��λ
{
	WriteAT7052(0xD3,0x000000);	// 	
}
void ATT7022_Config(void)
{
  WriteAT7052(0x81,0x00F9FE);  //��дģʽ���üĴ���0X01����λΪ1 ����������81H�� 

  WriteAT7052(0x83,0x00F8A4); //��дEMU���üĴ���0X03����λΪ1 ����������83H��

  WriteAT7052(0xB1,0x003427); //��дģ��ģ��ʹ�ܼĴ���0X31����λΪ1 ����������B1H��

  WriteAT7052(0x9E,0x0000E7);//HFCONST= INT[2.592*10^10*G*G*Vu*vi/(EC*UN*IB)]
                             //=INT[2.592*10^10*1.163*1.163*0.25*0.2643/(20000*100*5)]
							 //=INT[231.618]=231=E7H

  WriteAT7052(0x9D,0x001DB2); //�������� IO=5*58*0.4%=1.16   ISTARTUP=0.8*i0*2^13=0.8*1.16*2^13=7602=1DB2H
}

void ATT7022_Init(void)
{
  ATT7022_WREnable();
	
  WriteAT7052(0x81,0x00B9FE);  //��дģʽ���üĴ���0X01����λΪ1 ����������81H�� 
  WriteAT7052(0x83,0x00F8A4); //��дEMU���üĴ���0X03����λΪ1 ����������83H��

  WriteAT7052(0xB1,0x003427); //��дģ��ģ��ʹ�ܼĴ���0X31����λΪ1 ����������B1H��

  WriteAT7052(0x9E,0x0000E7);//HFCONST= INT[2.592*10^10*G*G*Vu*vi/(EC*UN*IB)]
                             //=INT[2.592*10^10*1.163*1.163*0.25*0.2643/(20000*100*5)]
							 //=INT[231.618]=231=E7H

  WriteAT7052(0x9D,0x001DB2); //�������� IO=5*58*0.4%=1.16   ISTARTUP=0.8*i0*2^13=0.8*1.16*2^13=7602=1DB2H
  //ATT7022_WRDisable();
}

void XBAT7022(void)
{ 

 /*******ec=3200 220V--0.05v  1.5A--0.22v************/

  WriteAT7052(0x81,0x00B9FF);  //��дģʽ���üĴ���0X01����λΪ1 ����������81H�� 

  WriteAT7052(0x83,0x00F804); //��дEMU���üĴ���0X03����λΪ1 ����������83H��

  WriteAT7052(0xB1,0x003437); //��дģ��ģ��ʹ�ܼĴ���0X31����λΪ1 ����������B1H��

  WriteAT7052(0x9E,0x0000E7);//HFCONST= INT[2.592*10^10*G*G*Vu*vi/(EC*UN*IB)]
                             //=INT[2.592*10^10*1.163*1.163*0.25*0.2643/(20000*100*5)]
							 //=INT[231.618]=231=E7H

  WriteAT7052(0x9D,0x001DB2); //�������� IO=5*58*0.4%=1.16   ISTARTUP=0.8*i0*2^13=0.8*1.16*2^13=7602=1DB2H


 /************A�����**********************/
  WriteAT7052(0x84,0x0000F4); //A�๦������ �й�   �����ѹ����220V,������A1.5A ��������Ϊ1 ������Ϊ-0.74%�� 
                              //PGAIN=-ERR/1+ERR=0.0074/(1-0.0074)=0.00745516
                              //PGAIN=PGAIN*2^15=244=0F4H

  WriteAT7052(0x87,0x0000F4); // �޹�

  WriteAT7052(0x8A,0x0000F4); // ʵ������Ĵ���дͬ����У��ֵ


  WriteAT7052(0x8D,0x00FFBF); // A����λ �����ѹ����220V ��IA=1.5A,��������0.5 ��׼�� ����������0.34%
                              // &=-0.0034/1.732=-0.001963<0
                              // phsreg=2^16+0.0034/1.732*2^15=65471=FFBFH

  WriteAT7052(0x90,0x00FFBF);

 /************B�����**********************/

   WriteAT7052(0x85,0x00FF68);//   B�๦������ �����ѹ����220V ��IB=1.5A,��������1 ��׼�� ����������1.26%
                              //  PGAIN=2^16+-ERR/1+ERR*2^15=FF68H
   WriteAT7052(0x88,0x00FF68);

   WriteAT7052(0x8B,0x00FF38);


   WriteAT7052(0x8E,0x000040); //��λУ��  ERR=-0.34%  PHSREG=40h
   WriteAT7052(0x91,0x000040); //��λУ��  ERR=-0.34%  PHSREG=40h

 /************C�����**********************/

   WriteAT7052(0x86,0x00FF5C);//   B�๦������ �����ѹ����220V ��IC=1.5A,��������1 ��׼�� ����������0.5%
                              //  PGAIN=FF5CH
   WriteAT7052(0x89,0x00FF5C);

   WriteAT7052(0x8C,0x00FF5C);


   WriteAT7052(0x8F,0x00FFD2); //��λУ��  ERR=0.24%  PHSREG=FFD2H
   WriteAT7052(0x92,0x00FFD2); 

   /*******************ABC���ѹУ��******************/
   /****����220V�������1.5A ��������1.0�� ��ѹ�Ĵ�����ֵ 2134645  2131300 2094285   */
   /* ���� URMS=VU*2^10/2^13                  ���ܱ��       260.576 260.168  255.650*/
   /*ugain=ur/urms-1                                                                 */
   /*  ugain=int(2^16+ugain*2^15))                                                   */
   /**********************************************************************************/

   WriteAT7052(0x97,0x00EC11);
   WriteAT7052(0x98,0x00EC3C);
   WriteAT7052(0x99,0x00EE26);

     /*******************ABC�����У��******************/
   /****����220V�������1.5A ��������1.0�� �����Ĵ�����ֵ 507904   482508   491520  */
   /* ����IRMS=VI*2^10/2^13                  ���ܱ��     1.55     1.4725   1.498   */
   /*ugain=Ir/Irms-1                                                                */
   /*  ugain=int(2^16+ugain*2^15))                                                  */
   /**********************************************************************************/

   WriteAT7052(0x9A,0x00FBDE);
   WriteAT7052(0x9B,0x000263);
   WriteAT7052(0x9C,0x00002B);
}
