// W25X64 
// 8M�ֽڣ�32768ҳ ÿҳ256�ֽ�
// 2048������ÿ����4K�ֽ�,ÿ������16ҳ    ��ַ0x000000~0x800000
#include "flash.h" 
#include "spi.h"
u8 CS2BZ = 0;
u8 Flash_Writeflag = 0;
//��ʼ��SPI FLASH��IO��
void SPI_Flash_Init(void)
{
	W25X64_Init();		   //��ʼ��SPI
	SPI2_Init();
}  

//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
	W25X64_ReadWrite(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=W25X64_ReadWrite(0xFF);             //��ȡһ���ֽ�  
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H; 
	return byte;   
} 
//дSPI_FLASH״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
	W25X64_ReadWrite(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	W25X64_ReadWrite(sr);               //д��һ���ֽ�  
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H;    
}   
//SPI_FLASHдʹ��	
//��WEL��λ   
void SPI_FLASH_Write_Enable(void)   
{
	if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
  W25X64_ReadWrite(W25X_WriteEnable);      //����дʹ��  
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H;     
} 
//SPI_FLASHд��ֹ	
//��WEL����  
void SPI_FLASH_Write_Disable(void)   
{  
	if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
  W25X64_ReadWrite(W25X_WriteDisable);     //����д��ָֹ��    
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H; 
} 			    
//��ȡоƬID W25X16��ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
	W25X64_ReadWrite(0x90);//���Ͷ�ȡID����	    
	W25X64_ReadWrite(0x00); 	    
	W25X64_ReadWrite(0x00); 	    
	W25X64_ReadWrite(0x00); 	 			   
	Temp|=W25X64_ReadWrite(0xFF)<<8;  
	Temp|=W25X64_ReadWrite(0xFF);	  
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H; 
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i; 
	if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
		SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
	W25X64_ReadWrite(W25X_ReadData);         //���Ͷ�ȡ����   
	W25X64_ReadWrite((u8)((ReadAddr)>>16));  //����24bit��ַ    
	W25X64_ReadWrite((u8)((ReadAddr)>>8));   
	W25X64_ReadWrite((u8)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=W25X64_ReadWrite(0xFF);   //ѭ������   
	}
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H;      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
  SPI_FLASH_Write_Enable();                  //SET WEL 
  if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
	W25X64_ReadWrite(W25X_PageProgram);      //����дҳ����   
	W25X64_ReadWrite((u8)((WriteAddr)>>16)); //����24bit��ַ    
	W25X64_ReadWrite((u8)((WriteAddr)>>8));   
	W25X64_ReadWrite((u8)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)W25X64_ReadWrite(pBuffer[i]);//ѭ��д��  
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H;
  SPI_FLASH_Write_Disable();	
	SPI_Flash_Wait_Busy();					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0xFF,�����ڷ�0xFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;  
	Flash_Writeflag = 0x02;
	secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0xFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)
		{
			Flash_Writeflag &= ~0x02;
			break;//д�������
		}
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 	 
}

//����ָ������FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
void SPI_Flash_Erase(u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;  
	Flash_Writeflag = 0x01;
	secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0xFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);//�����������
			if(secoff > 0 || secremain < 4096)
			{
				for(i=0;i<secremain;i++)	   //����
				{
					SPI_FLASH_BUF[i+secoff]=0xFF;	  
				}
				SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  
		  }
		}			   
		if(NumByteToWrite==secremain)
		{
			Flash_Writeflag &= ~0x01;
			break;//д�������
		}
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 
			WriteAddr+=secremain;//д��ַƫ��	   
		  NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	} 	 
}

//��������оƬ
//��Ƭ����ʱ��:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//�ȴ�ʱ�䳬��...
void SPI_Flash_Erase_Chip(void)   
{                                             
  SPI_FLASH_Write_Enable();                  //SET WEL 
  SPI_Flash_Wait_Busy();   
  if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
  W25X64_ReadWrite(W25X_ChipErase);        //����Ƭ��������  
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H; 
	SPI_Flash_Wait_Busy();   				   //�ȴ�оƬ��������
}    
//����һ������
//Dst_Addr:������ַ 0~2047 for W25X64
//����һ��ɽ��������ʱ��:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
	SPI_FLASH_Write_Enable();                  //SET WEL 	 
	SPI_Flash_Wait_Busy();   
	if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
	W25X64_ReadWrite(W25X_SectorErase);      //������������ָ�� 
	W25X64_ReadWrite((u8)((Dst_Addr)>>16));  //����24bit��ַ    
	W25X64_ReadWrite((u8)((Dst_Addr)>>8));   
	W25X64_ReadWrite((u8)Dst_Addr);  
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H; 
	SPI_Flash_Wait_Busy();   				   //�ȴ��������
}  

//�ȴ�����
void SPI_Flash_Wait_Busy(void)   
{
  u16 waitcnt = 0;	
	while ((SPI_Flash_ReadSR()&0x01)==0x01)
	{
		os_dly_wait (1);
		waitcnt++;
		if(waitcnt > 3000)
      return;
	}   // �ȴ�BUSYλ���
}
//�������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
 if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
  W25X64_ReadWrite(W25X_PowerDown);        //���͵�������  
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H; 
  Delay(3);                               //�ȴ�TPD  
}   
//����
void SPI_Flash_WAKEUP(void)   
{  
 if(CS2BZ==0) 
	{
		SPI_FLASH_CS_L; 
	  SPI_FLASH_CS2_H;
	}
	else 
	{
		SPI_FLASH_CS2_L; 
		SPI_FLASH_CS_H;
	}//ʹ������   
  W25X64_ReadWrite(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	SPI_FLASH_CS_H;                            //ȡ��Ƭѡ     
	SPI_FLASH_CS2_H; 
  Delay(3);                                  //�ȴ�TRES1
}   





















