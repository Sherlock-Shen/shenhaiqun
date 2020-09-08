//###########################################################################
//
// ����2ͨѶЭ���ļ�
// 485ͨѶ��
// 
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  18.03| 13 mary 2018 | S.H.Q. | 
//###########################################################################
#include "protocol2.h"
#include "queue.h"
#include "ATT7022.h"
#include "ds1302.h"
#include "flash.h"

struct BACKSTAGE_PROTOCOL_TYPE2 BackstageProtocol2;
struct BACKSTAGE_PROTOCOL_TYPE2 BackstageProtocol2bak;

s16 PcurveUA_bak[1200];	// A�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
s16 PcurveUB_bak[1200];	// B�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
s16 PcurveUC_bak[1200];	// C�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
s16 PcurveIA_bak[1200];	// A�ౣ����������¼������������ǰ4�����ڣ�����������8������
s16 PcurveIB_bak[1200];	// B�ౣ����������¼������������ǰ4�����ڣ�����������8������
s16 PcurveIC_bak[1200];	// C�ౣ����������¼������������ǰ4�����ڣ�����������8������
s16 PcurveU0_bak[1200];	// ���򱣻���ѹ����¼������������ǰ4�����ڣ�����������8������
s16 PcurveI0_bak[1200];	// ���򱣻���������¼������������ǰ4�����ڣ�����������8������

static u8 sum = 0;
static volatile u8 ProtocolResolveState = 0;	// Э���������״̬��
static u8 channel = 0;			// ����ͨ��
#ifdef TEST
static u8 Curve_cnt = 0;
#endif
extern struct PhaseMode MyCurve;
extern struct PhaseMode1 MyCurve1;

void PCurve_Save(void)
{
	u16 i=0;
	if(AD_StartPoint < 1200)
	{
		for(i=0;i<1200-AD_StartPoint;i++)
		{
			PcurveUA_bak[i] = PcurveUA[AD_StartPoint+i];
			PcurveUB_bak[i] = PcurveUB[AD_StartPoint+i];
			PcurveUC_bak[i] = PcurveUC[AD_StartPoint+i];
			PcurveIA_bak[i] = PcurveIA[AD_StartPoint+i];
			PcurveIB_bak[i] = PcurveIB[AD_StartPoint+i];
			PcurveIC_bak[i] = PcurveIC[AD_StartPoint+i];
			PcurveU0_bak[i] = PcurveU0[AD_StartPoint+i];
			PcurveI0_bak[i] = PcurveI0[AD_StartPoint+i];
		}
		for(i=0;i<AD_StartPoint;i++)
		{
			PcurveUA_bak[1200-AD_StartPoint+i] = PcurveUA[i];
			PcurveUB_bak[1200-AD_StartPoint+i] = PcurveUB[i];
			PcurveUC_bak[1200-AD_StartPoint+i] = PcurveUC[i];
			PcurveIA_bak[1200-AD_StartPoint+i] = PcurveIA[i];
			PcurveIB_bak[1200-AD_StartPoint+i] = PcurveIB[i];
			PcurveIC_bak[1200-AD_StartPoint+i] = PcurveIC[i];
			PcurveU0_bak[1200-AD_StartPoint+i] = PcurveU0[i];
			PcurveI0_bak[1200-AD_StartPoint+i] = PcurveI0[i];
		}
	}
}

void P1SCurve_Save(u8 position)
{
	u8 bufread[8];	
	u8 buf[12];
	u16 recordorder = 0;
	s16 *Pstart=0;
	CS2BZ=0;
	if(position == 0)
	{
		SPI_Flash_Read(bufread,FLASH_P1SCURVE_ADDR+Curve_Index*FLASH_P1SCURVE_LENGTH,2);
		recordorder = bufread[0] | (bufread[1]<<8);
		if(recordorder == 0xFFFF) recordorder = 0;
		if(recordorder == Curve_Order[Curve_Index])
		{
			recordorder = Curve_Order[Curve_Index] +1;
			if(recordorder > 60000 || recordorder == 0)recordorder = 1;
			if(Curve_Order[Curve_Index] != 0)
			{
				if(Curve_Index < (FLASH_P1SCURVE_MAX-1))
					Curve_Index++;
				else
					Curve_Index = 0;				
			}
			Curve_Order[Curve_Index] = recordorder;
		}
		buf[0] = Curve_Order[Curve_Index] & 0x00FF;
		buf[1] = Curve_Order[Curve_Index] >> 8;
		buf[2] = AD_StartPoint & 0x00FF;
		buf[3] = AD_StartPoint >> 8;
		buf[4] = TimeNow.year&0xff;
		buf[5] = TimeNow.month&0xff;
		buf[6] = TimeNow.date&0xff;
		buf[7] = TimeNow.hour&0xff;
		buf[8] = TimeNow.minute&0xff;
		buf[9] = TimeNow.second&0xff;
		buf[10] = TimeNow.msec>>8;
		buf[11] = TimeNow.msec&0xff;
		SPI_Flash_Write(buf,FLASH_P1SCURVE_ADDR+(Curve_Index*FLASH_P1SCURVE_LENGTH),12);// д�����
  }
  else if(position==1) 
	{
	  Pstart = PcurveUA_bak;
	  SPI_Flash_Write((u8*)Pstart,FLASH_P1SCURVE_ADDR+100+Curve_Index*FLASH_P1SCURVE_LENGTH,2400);
	}
	else if(position==2) 
	{
	  Pstart = PcurveUB_bak;
	  SPI_Flash_Write((u8*)Pstart,FLASH_P1SCURVE_ADDR+100+Curve_Index*FLASH_P1SCURVE_LENGTH+2400,2400);
	}
	else if(position==3) 
	{
	  Pstart = PcurveUC_bak;
	  SPI_Flash_Write((u8*)Pstart,FLASH_P1SCURVE_ADDR+100+Curve_Index*FLASH_P1SCURVE_LENGTH+4800,2400);
	}
	else if(position==4) 
	{
	  Pstart = PcurveIA_bak;
	  SPI_Flash_Write((u8*)Pstart,FLASH_P1SCURVE_ADDR+100+Curve_Index*FLASH_P1SCURVE_LENGTH+7200,2400);
	}
	else if(position==5) 
	{
	  Pstart = PcurveIB_bak;
	  SPI_Flash_Write((u8*)Pstart,FLASH_P1SCURVE_ADDR+100+Curve_Index*FLASH_P1SCURVE_LENGTH+9600,2400);
	}
	else if(position==6) 
	{
	  Pstart = PcurveIC_bak;
	  SPI_Flash_Write((u8*)Pstart,FLASH_P1SCURVE_ADDR+100+Curve_Index*FLASH_P1SCURVE_LENGTH+12000,2400);
	}
	else if(position==7) 
	{
	  Pstart = PcurveU0_bak;
	  SPI_Flash_Write((u8*)Pstart,FLASH_P1SCURVE_ADDR+100+Curve_Index*FLASH_P1SCURVE_LENGTH+14400,2400);
	}
	else if(position==8) 
	{
	  Pstart = PcurveI0_bak;
	  SPI_Flash_Write((u8*)Pstart,FLASH_P1SCURVE_ADDR+100+Curve_Index*FLASH_P1SCURVE_LENGTH+16800,2400);
	}	
}

void Read_CurveData(void)
{
	static u8 datanum = 0;
	static u8 dataframe = 0;
	u8 sum=0;
	u8 point_value =0;
	u16 i=0;
	s16 *point=0;
	
	InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
	sum += 0xFE;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
	sum += 0xEF;
	InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
	sum += BackstageProtocol2.Device;
	InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
	sum += BackstageProtocol2.DeviceNum;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xCA);				// ����������
	sum += 0xCA;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x0C);				// ���ͳ��ȵ�λ  204������
	sum += 0x0C;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ  

	InsertDataToQueue(&QueueTX2,BufferTX2,0);				// ���Ͳ��κ�

	InsertDataToQueue(&QueueTX2,BufferTX2,0);				// ���͵ڼ�֡
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.year&0xff);
	sum += TimeNow.year&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.month&0xff);
	sum += TimeNow.month&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.date&0xff);
	sum += TimeNow.date&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.hour&0xff);
	sum += TimeNow.hour&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.minute&0xff);
	sum += TimeNow.minute&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.second&0xff);
	sum += TimeNow.second&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.msec>>8);
	sum += TimeNow.msec>>8;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.msec&0xff);
	sum += TimeNow.msec&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
	InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β	
	os_dly_wait (100);
	for(point_value=0;point_value<24;point_value++)
	{
		sum=0;
		if(((point_value +1) % 3) == 0)
			dataframe = 3;
		else
		  dataframe = (point_value +1) % 3;
		if(point_value == 0)
		{
			point = &PcurveUA[AD_StartPoint];
		  datanum = 1;
		}
		else if(point_value == 3)
		{
			point = &PcurveUB[AD_StartPoint];
		  datanum = 2;
		}
		else if(point_value == 6)
		{
			point = &PcurveUC[AD_StartPoint];
		  datanum = 3;
		}
		else if(point_value == 9)
		{
			point = &PcurveIA[AD_StartPoint];
		  datanum = 4;
		}
		else if(point_value == 12)
		{
			point = &PcurveIB[AD_StartPoint];
		  datanum = 5;
		}
		else if(point_value == 15)
		{
			point = &PcurveIC[AD_StartPoint];
		  datanum = 6;
		}
		else if(point_value == 18)
		{
			point = &PcurveU0[AD_StartPoint];
		  datanum = 7;
		}
		else if(point_value == 21)
		{
			point = &PcurveI0[AD_StartPoint];
		  datanum = 8;
		}
		
		InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
		sum += 0xFE;
		InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
		sum += 0xEF;
		InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
		sum += BackstageProtocol2.Device;
		InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
		sum += BackstageProtocol2.DeviceNum;
		InsertDataToQueue(&QueueTX2,BufferTX2,0xCA);				// ����������
		sum += 0xCA;
		InsertDataToQueue(&QueueTX2,BufferTX2,0x24);				// ���ͳ��ȵ�λ  204������
		sum += 0x24;
		InsertDataToQueue(&QueueTX2,BufferTX2,0x03);				// ���ͳ��ȸ�λ  204������
		sum += 0x03;
		InsertDataToQueue(&QueueTX2,BufferTX2,datanum);				// ���Ͳ��κ�
		sum += datanum;
		InsertDataToQueue(&QueueTX2,BufferTX2,dataframe);				// ���͵ڼ�֡
		sum += dataframe;
		for(i=0;i<400;i++)
		{
			InsertDataToQueue(&QueueTX2,BufferTX2,*point&0xff);
      sum+=*point&0xff;			
			InsertDataToQueue(&QueueTX2,BufferTX2,*point>>8);
      sum+=*point>>8;	
			point++;
			if(point > (&PcurveUA[1199]) && point_value < 3)
				point = &PcurveUA[0];
			else if(point > (&PcurveUB[1199]) && point_value < 6 && point_value >= 3)
				point = &PcurveUB[0];
			else if(point > (&PcurveUC[1199]) && point_value < 9 && point_value >= 6)
				point = &PcurveUC[0];
			else if(point > (&PcurveIA[1199]) && point_value < 12 && point_value >= 9)
				point = &PcurveIA[0];
			else if(point > (&PcurveIB[1199]) && point_value < 15 && point_value >= 12)
				point = &PcurveIB[0];
			else if(point > (&PcurveIC[1199]) && point_value < 18 && point_value >= 15)
				point = &PcurveIC[0];
			else if(point > (&PcurveU0[1199]) && point_value < 21 && point_value >= 18)
				point = &PcurveU0[0];
			else if(point > (&PcurveI0[1199]) && point_value < 24 && point_value >= 21)
				point = &PcurveI0[0];
		}
		InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
		InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
		os_dly_wait (100);
	}
}

//#define TEST
#ifdef TEST
void Read_InstantData(void)
{
	static u8 datanum = 0;
	static u8 dataframe = 0;
	u8 point_value =0;
	u8 buffer[800];
	u8 sum=0;
	u16 i=0;
	CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_P1SCURVE_ADDR+(Curve_cnt*FLASH_P1SCURVE_LENGTH),100);
	
	InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
	sum += 0xFE;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
	sum += 0xEF;
	InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
	sum += BackstageProtocol2.Device;
	InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
	sum += BackstageProtocol2.DeviceNum;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xCA);				// ����������
	sum += 0xCA;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x0C);				// ���ͳ��ȵ�λ  204������
	sum += 0x0C;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ  

	InsertDataToQueue(&QueueTX2,BufferTX2,0);				// ���Ͳ��κ�

	InsertDataToQueue(&QueueTX2,BufferTX2,0);				// ���͵ڼ�֡
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.year&0xff);
	sum += TimeNow.year&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.month&0xff);
	sum += TimeNow.month&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.date&0xff);
	sum += TimeNow.date&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.hour&0xff);
	sum += TimeNow.hour&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.minute&0xff);
	sum += TimeNow.minute&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.second&0xff);
	sum += TimeNow.second&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.msec>>8);
	sum += TimeNow.msec>>8;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.msec&0xff);
	sum += TimeNow.msec&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
	InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β	
	os_dly_wait (100);
  for(point_value=0;point_value<24;point_value++)
	{
		sum=0; 
		if(((point_value +1) % 3) == 0)
			dataframe = 3;
		else
		  dataframe = (point_value +1) % 3;
		if(point_value == 0)
		{
		  datanum = 1;
		}
		else if(point_value == 3)
		{
		  datanum = 2;
		}
		else if(point_value == 6)
		{
		  datanum = 3;
		}
		else if(point_value == 9)
		{
		  datanum = 4;
		}
		else if(point_value == 12)
		{
		  datanum = 5;
		}
		else if(point_value == 15)
		{
		  datanum = 6;
		}
		else if(point_value == 18)
		{
		  datanum = 7;
		}
		else if(point_value == 21)
		{
		  datanum = 8;
		}
		
		InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
		sum += 0xFE;
		InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
		sum += 0xEF;
		InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
		sum += BackstageProtocol2.Device;
		InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
		sum += BackstageProtocol2.DeviceNum;
		InsertDataToQueue(&QueueTX2,BufferTX2,0xCA);				// ����������
		sum += 0xCA;
		InsertDataToQueue(&QueueTX2,BufferTX2,0x24);				// ���ͳ��ȵ�λ  204������
		sum += 0x24;
		InsertDataToQueue(&QueueTX2,BufferTX2,0x03);				// ���ͳ��ȸ�λ  204������
		sum += 0x03;
		InsertDataToQueue(&QueueTX2,BufferTX2,datanum);				// ���Ͳ��κ�
		sum += datanum;
		InsertDataToQueue(&QueueTX2,BufferTX2,dataframe);				// ���͵ڼ�֡
		sum += dataframe;
		
		SPI_Flash_Read(buffer,FLASH_P1SCURVE_ADDR+(4*FLASH_P1SCURVE_LENGTH)+100+point_value*800,800);
		for(i=0;i<800;i++)
		{
			InsertDataToQueue(&QueueTX2,BufferTX2,buffer[i]);
			sum+=buffer[i];			
		}

		InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
		InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
		os_dly_wait (100);
	}
	if(Curve_cnt < 99)
	  Curve_cnt++;
	else
		Curve_cnt = 0;
}
#else
void Read_InstantData(void)
{
	static u8 datanum = 0;
	static u8 dataframe = 0;
	u8 point_value =0;
  s16 *point=0;
	u8 sum=0;
	u16 i=0;
	
	InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
	sum += 0xFE;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
	sum += 0xEF;
	InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
	sum += BackstageProtocol2.Device;
	InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
	sum += BackstageProtocol2.DeviceNum;
	InsertDataToQueue(&QueueTX2,BufferTX2,0xCA);				// ����������
	sum += 0xCA;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x0C);				// ���ͳ��ȵ�λ  204������
	sum += 0x0C;
	InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ  

	InsertDataToQueue(&QueueTX2,BufferTX2,0);				// ���Ͳ��κ�

	InsertDataToQueue(&QueueTX2,BufferTX2,0);				// ���͵ڼ�֡
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.year&0xff);
	sum += TimeNow.year&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.month&0xff);
	sum += TimeNow.month&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.date&0xff);
	sum += TimeNow.date&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.hour&0xff);
	sum += TimeNow.hour&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.minute&0xff);
	sum += TimeNow.minute&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.second&0xff);
	sum += TimeNow.second&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.msec>>8);
	sum += TimeNow.msec>>8;
	InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.msec&0xff);
	sum += TimeNow.msec&0xff;
	InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
	InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β	
	os_dly_wait (100);
  for(point_value=0;point_value<24;point_value++)
	{
		sum=0; 
		if(((point_value +1) % 3) == 0)
			dataframe = 3;
		else
		  dataframe = (point_value +1) % 3;
		if(point_value == 0)
		{
			point = PcurveUA_bak;
		  datanum = 1;
		}
		else if(point_value == 3)
		{
			point = PcurveUB_bak;
		  datanum = 2;
		}
		else if(point_value == 6)
		{
			point = PcurveUC_bak;
		  datanum = 3;
		}
		else if(point_value == 9)
		{
			point = PcurveIA_bak;
		  datanum = 4;
		}
		else if(point_value == 12)
		{
			point = PcurveIB_bak;
		  datanum = 5;
		}
		else if(point_value == 15)
		{
			point = PcurveIC_bak;
		  datanum = 6;
		}
		else if(point_value == 18)
		{
			point = PcurveU0_bak;
		  datanum = 7;
		}
		else if(point_value == 21)
		{
			point = PcurveI0_bak;
		  datanum = 8;
		}
		
		InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
		sum += 0xFE;
		InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
		sum += 0xEF;
		InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
		sum += BackstageProtocol2.Device;
		InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
		sum += BackstageProtocol2.DeviceNum;
		InsertDataToQueue(&QueueTX2,BufferTX2,0xCA);				// ����������
		sum += 0xCA;
		InsertDataToQueue(&QueueTX2,BufferTX2,0x24);				// ���ͳ��ȵ�λ  204������
		sum += 0x24;
		InsertDataToQueue(&QueueTX2,BufferTX2,0x03);				// ���ͳ��ȸ�λ  204������
		sum += 0x03;
		InsertDataToQueue(&QueueTX2,BufferTX2,datanum);				// ���Ͳ��κ�
		sum += datanum;
		InsertDataToQueue(&QueueTX2,BufferTX2,dataframe);				// ���͵ڼ�֡
		sum += dataframe;
			
		for(i=0;i<400;i++)
		{
			InsertDataToQueue(&QueueTX2,BufferTX2,*point&0xff);
			sum+=*point&0xff;			
			InsertDataToQueue(&QueueTX2,BufferTX2,*point>>8);
			sum+=*point>>8;	
			point++;		
		}
		
		InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
		InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
		os_dly_wait (100);
	}
}
#endif
/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolve2(u8 receivedata)
*
*��    �ܣ�����ͨѶЭ�����
*
*��ڲ���������2����
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolve2(u8 receivedata)
{
	static u8 lengthL = 0;
	static u16 lengthcnt = 0;
	switch(ProtocolResolveState)
	{
		case 0:
			if(receivedata == 0xFE)			// �ж�����ͷ
			{
					if(Usart2RxReady == 0)
				{
					channel = 0;
					ProtocolResolveState = 1;
				}
				else if(Usart2bakRxReady == 0)
				{
					channel = 1;
					ProtocolResolveState = 1;
				}
			}
			break;
		case 1:
			if(receivedata == 0xEF)			// ȷ������ͷ,������ʼ��
			{
				sum = 0xED;					// sumҲ���к�У��
				ProtocolResolveState = 2;
			}
			else
			{
				ProtocolResolveState = 0;
				return 0;
			}
			break;
		case 2:
			if(channel == 0)
				BackstageProtocol2.Device = receivedata;			// ���յ����豸����
			else
				BackstageProtocol2bak.Device = receivedata;			// ���յ����豸����
				sum += receivedata;
				ProtocolResolveState = 3;
			break;
		case 3:
			if(channel == 0)
				BackstageProtocol2.DeviceNum = receivedata;			
			else
				BackstageProtocol2bak.DeviceNum = receivedata;		
			sum += receivedata;                // ���յ������ݵ�ַ
			ProtocolResolveState = 4;
			break;
		case 4:								// ����
			if(channel == 0)
				BackstageProtocol2.command = receivedata;
			else
				BackstageProtocol2bak.command = receivedata;
        sum += receivedata;
			ProtocolResolveState = 5;	
			break;
		case 5:
			lengthL = receivedata;			// ���ݳ��ȵ��ֽ�
			sum += receivedata;
			ProtocolResolveState = 6;
			break;
		case 6:								// �������ݳ���
			if(channel == 0)
				BackstageProtocol2.length = ((u16)receivedata<<8)+(u16)lengthL;
			else
				BackstageProtocol2bak.length = ((u16)receivedata<<8)+(u16)lengthL;
			ProtocolResolveState = 7;
			sum += receivedata;
			lengthcnt = 0;
			break;
		case 7:								// ��������
			if(channel == 0)
				BackstageProtocol2.DataBuffer[lengthcnt] = receivedata;
			else
				BackstageProtocol2bak.DataBuffer[lengthcnt] = receivedata;
			sum += receivedata;
			ProtocolResolveState=7;
			lengthcnt++;
			if(lengthcnt > 255)			// ���ݼ�飬��ֹ���
			{
				ProtocolResolveState = 0;
			}
			if(channel == 0)
			{
				if((lengthcnt+2) == BackstageProtocol2.length)		// ���ݽ��ս���
					ProtocolResolveState = 8;
			}
			else
			{
				if((lengthcnt+2) == BackstageProtocol2bak.length)		// ���ݽ��ս���
					ProtocolResolveState = 8;
			}
			break;
		case 8:
			if((sum&0x00ff) == receivedata)	// ��У��
			{
		    ProtocolResolveState = 9;
		  }
			else							// ��У��ʧ��
 				ProtocolResolveState = 0;
			break;
		case 9:
			if(0x16 == receivedata)	//У��
			{
				ProtocolResolveState = 0;
				if(channel == 0)
				  Usart2RxReady=1;
				else
				  Usart2bakRxReady=1;
				isr_evt_set (0x0002, t_Task4);
			}
			else							//У��ʧ��
 				ProtocolResolveState = 0;
			break;
		default:
			break;
	}
	return 0;
}
void CommandProcess2Prepare(void)
{
	u16 i = 0;
	BackstageProtocol2.command = BackstageProtocol2bak.command;			
	BackstageProtocol2.length = BackstageProtocol2bak.length;				
	BackstageProtocol2.Device = BackstageProtocol2bak.Device;
  BackstageProtocol2.DeviceNum = BackstageProtocol2bak.DeviceNum;
	BackstageProtocol2.AgeAddress = BackstageProtocol2bak.AgeAddress;
	for(i=0;i<BackstageProtocol2bak.length-2;i++)
		BackstageProtocol2.DataBuffer[i] = BackstageProtocol2bak.DataBuffer[i];	
}

void CommandProcess2(void)
{
	u16 i = 0;
	u8 sum = 0;
	u8 BTYX1 = 0;	// ����ң��״̬�źţ����ڼ��װ��
	u8 BTYX2 = 0;
	u8 BTYX3 = 0;
	u8 BTYX4 = 0;
	u16 standardvalue = 0;
	u16 temp = 0;
	switch(BackstageProtocol2.command)
	{
		case 0xFF:							// ʵʱ����
			switch(BackstageProtocol2.DataBuffer[0])			// ��������
			{
				case 0x04:	// ����������ȡ
					ReadProtectPara();	// ��ȡ��������
					sum = 0;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
					sum += 0xFE;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
					sum += 0xEF;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
					sum += BackstageProtocol2.Device;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
					sum += BackstageProtocol2.DeviceNum;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xF3);				// ����������
					sum += 0xF3;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x80);				// ���ͳ��ȵ�λ
					sum += 0x80;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ
					sum += 0x00;
					for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
					{
						InsertDataToQueue(&QueueTX2,BufferTX2,ProtectSet.word[i]&0x00FF);	// 
						sum += ProtectSet.word[i]&0x00FF;
						InsertDataToQueue(&QueueTX2,BufferTX2,ProtectSet.word[i]>>8);	// 
						sum += ProtectSet.word[i]>>8;	
					}
					InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
					InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
					break;
				case 0x05:	// ϵͳ������ȡ
					ReadSystemPara();	// ��ϵͳ���ò���
					sum = 0;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// �����ͷ
					sum += 0xFE;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
					sum += 0xEF;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
					sum += BackstageProtocol2.Device;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
					sum += BackstageProtocol2.DeviceNum;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xF4);				// ����������
					sum += 0xF4;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x4C);				// ���ͳ��ȵ�λ
					sum += 0x4C;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ
					sum += 0x00;
					for(i=0;i<31;i++)
					{	
						InsertDataToQueue(&QueueTX2,BufferTX2,SystemSet.word[i]&0xFF);
						sum+=SystemSet.word[i]&0xFF;
						InsertDataToQueue(&QueueTX2,BufferTX2,SystemSet.word[i]>>8);
						sum+=SystemSet.word[i]>>8;
					}
					InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.year&0xFF);
					sum+=TimeNow.year&0xFF;
					InsertDataToQueue(&QueueTX2,BufferTX2,0);
					InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.month&0xFF);
					sum+=TimeNow.month&0xFF;
					InsertDataToQueue(&QueueTX2,BufferTX2,0);
					InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.date&0xFF);
					sum+=TimeNow.date&0xFF;
					InsertDataToQueue(&QueueTX2,BufferTX2,0);
					InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.hour&0xFF);
					sum+=TimeNow.hour&0xFF;
					InsertDataToQueue(&QueueTX2,BufferTX2,0);
					InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.minute&0xFF);
					sum+=TimeNow.minute&0xFF;
					InsertDataToQueue(&QueueTX2,BufferTX2,0);
					InsertDataToQueue(&QueueTX2,BufferTX2,TimeNow.second&0xFF);
					sum+=TimeNow.second&0xFF;
					InsertDataToQueue(&QueueTX2,BufferTX2,0);
	
					InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
					InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β					
					break;
				case 0x06:	                                                // ������������
					ReadAlarmPara();
					sum = 0;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
					sum += 0xFE;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
					sum += 0xEF;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
					sum += BackstageProtocol2.Device;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
					sum += BackstageProtocol2.DeviceNum;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xF5);				// ����������
					sum += 0xF5;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x30);				// ���ͳ��ȵ�λ
					sum += 0x30;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ
					sum += 0x00;
					for(i=0;i<(sizeof(struct ALARM_PARA)/2);i++)
					{
						InsertDataToQueue(&QueueTX2,BufferTX2,AlarmSet.word[i]&0x00FF);	
						sum += AlarmSet.word[i]&0x00FF;
						InsertDataToQueue(&QueueTX2,BufferTX2,AlarmSet.word[i]>>8);	
						sum += AlarmSet.word[i]>>8;	
					}		
					InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
					InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
					break;
				case 0x08:	                                                // ��ȡ����ϵ��
					sum = 0;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
					sum += 0xFE;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
					sum += 0xEF;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
					sum += BackstageProtocol2.Device;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
					sum += BackstageProtocol2.DeviceNum;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xF7);				// ����������
					sum += 0xF7;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x24);				// ���ͳ��ȵ�λ
					sum += 0x24;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ
					sum += 0x00;
					for(i=0;i<(sizeof(struct Modulus)/2);i++)
					{
					 	InsertDataToQueue(&QueueTX2,BufferTX2,ProtectModulus.word[i]&0x00FF);// ����ϵ�����ֽ�
						sum += ProtectModulus.word[i]&0x00FF;
						InsertDataToQueue(&QueueTX2,BufferTX2,ProtectModulus.word[i]>>8);	// ����ϵ�����ֽ�
						sum += ProtectModulus.word[i]>>8;
					}
					InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
					InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
					break;
				case 0x0A:	// �Զ����в�������
					ReadAutoSwitchPara();
					sum = 0;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
					sum += 0xFE;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
					sum += 0xEF;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
					sum += BackstageProtocol2.Device;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);		// �����豸��ַ
					sum += BackstageProtocol2.DeviceNum;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xF9);				// ����������
					sum += 0xF9;
					InsertDataToQueue(&QueueTX2,BufferTX2,sizeof(struct AUTO_SWITCHOFF)+2);				// ���ͳ��ȵ�λ
					sum += sizeof(struct AUTO_SWITCHOFF)+2;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ
					sum += 0x00;
					for(i=0;i<(sizeof(struct AUTO_SWITCHOFF)/2);i++)
					{
						InsertDataToQueue(&QueueTX2,BufferTX2,AutoswitchSet.word[i]&0x00FF);	// 
						sum += AutoswitchSet.word[i]&0x00FF;
						InsertDataToQueue(&QueueTX2,BufferTX2,AutoswitchSet.word[i]>>8);	// 
						sum += AutoswitchSet.word[i]>>8;	
					}		
					InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
					InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
          break;
				case 0x0C:	// 101��Լ��������
					Read101Para();
				  sum = 0;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
					sum += 0xFE;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
					sum += 0xEF;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
					sum += BackstageProtocol2.Device;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);	// �����豸��ַ
					sum += BackstageProtocol2.DeviceNum;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xFB);				// ����������
					sum += 0xFB;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x40);				// ���ͳ��ȵ�λ
					sum += 0x40;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ
					sum += 0x00;
					for(i=0;i<(sizeof(struct Rule101Para));i++)
					{	
						InsertDataToQueue(&QueueTX2,BufferTX2,Measure101Para.word[i]);
						sum+=Measure101Para.word[i];
					}
					InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
					InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β			
					break;
				default:
				  break;
			}
			break;
		case 0xDD:							// ��������
			if(BackstageProtocol2.Device == 0x01)
			{
				switch(BackstageProtocol2.DataBuffer[0])	// �������� 
				{
					case 0x01:	// ��բ
						switch(BackstageProtocol2.DataBuffer[1])	// ����ģʽ
						{
							case 0x01:	// ģʽ1 ����
								if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
								{
									RecordSwitchOn(TYPE_ACT_PAD_ON);//DLQTYPE_LOCAL | DLQTYPE_ON	// ������բ��¼
									SwitchFlag = 1;
								}
								break;
							case 0x02:	//ģʽ2 Զ��
								if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
								{
									RecordSwitchOn(TYPE_ACT_FAR_ON);//DLQTYPE_FAR | DLQTYPE_ON	// ������բ��¼
									SwitchFlag = 1;
								}
								break;
							default:
								RecordSwitchOn(TYPE_ACT_MAN_ON);//DLQTYPE_LOCAL | DLQTYPE_ON	// ������բ��¼
								SwitchFlag = 1;
								break; 
						}					
						break;
					case 0x02:	// ��բ
						switch(BackstageProtocol2.DataBuffer[1])	// ����ģʽ
						{
							case 0x01:	// ģʽ1
								if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
								{
									RecordSwitchOff(TYPE_ACT_PAD_OFF);//DLQTYPE_LOCAL | DLQTYPE_OFF	// �����֢��¼
									SwitchFlag = 2;
								}
								break;
							case 0x02:	// ģʽ2
								if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
								{
									RecordSwitchOff(TYPE_ACT_FAR_OFF);	// DLQTYPE_FAR | DLQTYPE_OFF// ������բ��¼
									SwitchFlag = 2;
								}
								break;
							default:
								RecordSwitchOff(TYPE_ACT_MAN_OFF);	//DLQTYPE_LOCAL | DLQTYPE_OFF // �����֢��¼
								SwitchFlag = 2;
								break; 
						}
						break;
					case 0x05:	// ����
						SigresetFlag=1;
						break;
					case 0x06:	// ����
						ClearTJXX_Flag = 1;
						break;
//					case 0x07:	// ��λ
//					  __set_FAULTMASK(1);
//					  NVIC_SystemReset();
							
					case 0x08:	// ǿ�ƺ�բ 
						switch(BackstageProtocol2.DataBuffer[1])	// ����ģʽ
						{
							case 0x01:	// ģʽ1 ����
								if(SwitchFlag == 0)
								{
									RecordSwitchOn(TYPE_ACT_PAD_ON);//DLQTYPE_LOCAL | DLQTYPE_ON	// ������բ��¼
									SwitchFlag = 1;
								}
								
								break;
							case 0x02:	// ģʽ2 Զ��
								if(SwitchFlag == 0)
								{
									RecordSwitchOn(TYPE_ACT_FAR_ON);//DLQTYPE_FAR | DLQTYPE_ON	// ������բ��¼
									SwitchFlag = 1;
								}
								break;
							default:
								break; 
						}
						break;
					case 0x09:	// ǿ�Ʒ�բ ֻ�ܱ��ؽ���
						switch(BackstageProtocol2.DataBuffer[1])	// ����ģʽ
						{
							case 0x01:	// ģʽ1
								if(SwitchFlag == 0)
								{
									RecordSwitchOff(TYPE_ACT_PAD_OFF);//DLQTYPE_LOCAL | DLQTYPE_OFF	// ������բ��¼
									SwitchFlag = 2;
								}
								break;
							case 0x02:	// ģʽ2
								if(SwitchFlag == 0)
								{
									RecordSwitchOff(TYPE_ACT_FAR_OFF);//DLQTYPE_FAR | DLQTYPE_OFF	// ������բ��¼
									SwitchFlag = 2;
								}
								break;
							default:
								break; 
						}
						break;
					case 0x0E:	// ��������ϵ��
						standardvalue = BackstageProtocol2.DataBuffer[2] | (BackstageProtocol2.DataBuffer[3]<<8);
						if(standardvalue == 0 || BackstageProtocol2.DataBuffer[1] > 16)break;
						switch(BackstageProtocol2.DataBuffer[1])	// ϵ������
						{
							case 0:	// Uab
								ProtectModulus.para.UAB = ((long)standardvalue*DOWN_U_MODULUS)/MeasureData.UAB_temp;
								break;
							case 1:	// Ubc
								ProtectModulus.para.UBC = ((long)standardvalue*DOWN_U_MODULUS)/MeasureData.UBC_temp;
								break;
							case 2:	// Uca
								ProtectModulus.para.UCA = ((long)standardvalue*DOWN_U_MODULUS)/MeasureData.UCA_temp;
								break;
							case 3:	// Ua
								ProtectModulus.para.UA = ((long)standardvalue*DOWN_U_MODULUS)/MeasureData.UA_temp;
								break;
							case 4:	// Ub
								ProtectModulus.para.UB = ((long)standardvalue*DOWN_U_MODULUS)/MeasureData.UB_temp;
								break;
							case 5:	// Uc
								ProtectModulus.para.UC = ((long)standardvalue*DOWN_U_MODULUS)/MeasureData.UC_temp;
								break;
							case 6:	// U0
								ProtectModulus.para.U0 = ((long)standardvalue*DOWN_U0_MODULUS)/MeasureData.U0_temp;
								break;
							case 7:	// Ia
								ProtectModulus.para.IA = ((long)standardvalue*UP_I_MODULUS)/MeasureData.IA_temp;
								break;
							case 8:	// Ib
								ProtectModulus.para.IB = ((long)standardvalue*UP_I_MODULUS)/MeasureData.IB_temp;
								break;
							case 9:	// Ic
								ProtectModulus.para.IC = ((long)standardvalue*UP_I_MODULUS)/MeasureData.IC_temp;
								break;
							case 10:// PIa
								ProtectModulus.para.IA10 = ((long)standardvalue*DOWN_I_MODULUS)/MeasureData.PIA_temp;
								break;
							case 11:// PIb
								ProtectModulus.para.IB10 = ((long)standardvalue*DOWN_I_MODULUS)/MeasureData.PIB_temp;
								break;
							case 12:// PIc
								ProtectModulus.para.IC10 = ((long)standardvalue*DOWN_I_MODULUS)/MeasureData.PIC_temp;
								break;
							case 13:// I0
								ProtectModulus.para.I0 = ((long)standardvalue*DOWN_I0_MODULUS)/MeasureData.I0_temp;
								break;
							case 14:
								ProtectModulus.para.UA1 = ((long)standardvalue*UP_U_MODULUS)/MeasureData.UA1_temp;
								break;
							case 15:
								ProtectModulus.para.I010 = ((long)standardvalue*DOWN_I10_MODULUS)/MeasureData.I0_10_temp;
								break;
							case 16:
								ProtectModulus.para.UC1 = ((long)standardvalue*UP_U_MODULUS)/MeasureData.UC1_temp;
								break;

							default:
								break;
						}
						ModulusFlag=1;	// ����ϵ��
						break;
					default:
						break;
				}				
			}
			break;
		case 0xCB:
			for(i=0;i<(sizeof(struct ALARM_PARA)/2);i++)
			{
				AlarmSet.word[i] = BackstageProtocol2.DataBuffer[i*2] | (BackstageProtocol2.DataBuffer[i*2+1]<<8);
			}	
			SetAlarmFlag=1;
			break;
		case 0xCC:							// ������������	
			for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
			{
				ProtectSetReceive.word[i] = BackstageProtocol2.DataBuffer[i*2] | (BackstageProtocol2.DataBuffer[i*2+1]<<8);
			}
			ProtectSetReceive.para.fast_off_voltage_lock = 0;
			ProtectSetReceive.para.fast_off_direction_lock = 0; 
			ProtectSetReceive.para.delay_fast_off_voltage_lock = 0; 
			ProtectSetReceive.para.delay_fast_off_direction_lock = 0; 
			ProtectSetReceive.para.max_current_time_mode = 0; 
			ProtectSetReceive.para.max_current_voltage_lock = 0;
			ProtectSetReceive.para.max_current_direction_lock = 0;
            
			for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
			{
			   ProtectSet.word[i] = ProtectSetReceive.word[i];
			}
			SetProtectFlag=1;
			break;
		case 0xCD:							// ϵͳ��������	
			for(i=0;i<((sizeof(struct SYSTEM_PARA))/2);i++)
			{
				SystemSet.word[i]= BackstageProtocol2.DataBuffer[i*2] | (BackstageProtocol2.DataBuffer[i*2+1]<<8);				
			} 
      SetSystemFlag=1;
			break;
		case 0xCE:							// ������Ϣ����
			for(i=0;i<13;i++)
				MachineInformation[i] = BackstageProtocol2.DataBuffer[i];
			SaveMachineInformation();  // ���������ź���������
			break;
		case 0xCF:							// ����ϵ������
			for(i=0;i<17;i++)
			{
				temp = BackstageProtocol2.DataBuffer[i*2] | (BackstageProtocol2.DataBuffer[i*2+1]<<8);
				if(temp > 15000 && temp < 55000)
				{
					ProtectModulus.word[i] = temp;
				}
			}
			ModulusFlag=1;
			break;
		case 0xC5:			// ���װ�õ�ȡң������
			if(TESTFlag == 0)
			  BATTERY_CHARGE_OFF;
			JudgePoint_Flag = 1;
		  Battery_Readflag = 1;
		  os_dly_wait (100);
			sum = 0;
			InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
			sum += 0xFE;
			InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
			sum += 0xEF;
			InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
			sum += BackstageProtocol2.Device;
			InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.DeviceNum);			// �����豸��ַ
			sum += BackstageProtocol2.DeviceNum;				
			InsertDataToQueue(&QueueTX2,BufferTX2,0xC5);				// ����������
			sum += 0xC5;
			InsertDataToQueue(&QueueTX2,BufferTX2,0x52+32);				// ���ͳ��ȵ�λ
			sum += (0x52+32);
			InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ
			sum += 0x00;

			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UAB_val&0x00FF);			// ���͵�ѹ����
			sum += MeasureData.UAB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UAB_val>>8);				// ���͵�ѹ����
			sum += MeasureData.UAB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UBC_val&0x00FF);			// ���͵�ѹ����
			sum += MeasureData.UBC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UBC_val>>8);				// ����ѹ����
			sum += MeasureData.UBC_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UCA_val&0x00FF);			// ���͵�ѹ����
			sum += MeasureData.UCA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UCA_val>>8);				// ���͵�ѹ����
			sum += MeasureData.UCA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.IA_val&0x00FF);			// ���͵�������
			sum += MeasureData.IA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.IA_val>>8);				// ���͵�������
			sum += MeasureData.IA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.IB_val&0x00FF);			// ���͵�������
			sum += MeasureData.IB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.IB_val>>8);				// ���͵�������
			sum += MeasureData.IB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.IC_val&0x00FF);			// ���͵�������
			sum += MeasureData.IC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.IC_val>>8);				// ���͵������
			sum += MeasureData.IC_val>>8; 

			InsertDataToQueue(&QueueTX2,BufferTX2,(s16)MeasureData.PA_val&0x00FF);
			sum += (s16)MeasureData.PA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,(s16)MeasureData.PA_val>>8);
			sum += (s16)MeasureData.PA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,(s16)MeasureData.PB_val&0x00FF);
			sum += (s16)MeasureData.PB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,(s16)MeasureData.PB_val>>8);
			sum += (s16)MeasureData.PB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,(s16)MeasureData.PC_val&0x00FF);
			sum += (s16)MeasureData.PC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,(s16)MeasureData.PC_val>>8);
			sum += (s16)MeasureData.PC_val>>8;
			
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.QA_val&0x00FF);
			sum += MeasureData.QA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.QA_val>>8);
			sum += MeasureData.QA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.QB_val&0x00FF);
			sum += MeasureData.QB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.QB_val>>8);
			sum += MeasureData.QB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.QC_val&0x00FF);
			sum += MeasureData.QC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.QC_val>>8);
			sum += MeasureData.QC_val>>8;
			
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.cosA&0x00FF);
			sum += MeasureData.cosA&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.cosA>>8);
			sum += MeasureData.cosA>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.cosB&0x00FF);
			sum += MeasureData.cosB&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.cosB>>8);
			sum += MeasureData.cosB>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.cosC&0x00FF);
			sum += MeasureData.cosC&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.cosC>>8);
			sum += MeasureData.cosC>>8;
			
			
			InsertDataToQueue(&QueueTX2,BufferTX2,(s16)MeasureData.P_val&0x00FF);			// �����й���������
			sum += (s16)MeasureData.P_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,(s16)MeasureData.P_val>>8);				// �����й��������
			sum += (s16)MeasureData.P_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Q_val&0x00FF);			// �����޹���������
			sum += MeasureData.Q_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Q_val>>8);				// �����޹���������
			sum += MeasureData.Q_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.cosALL&0x00FF);		// ���͹�����������
			sum += MeasureData.cosALL&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.cosALL>>8);			// ���͹�����������
			sum += MeasureData.cosALL>>8;
			
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UAB_val&0x00FF);			// ���ͱ�����ѹ����
			sum += MeasureData.Protect_UAB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UAB_val>>8);				// ���ͱ�����ѹ����
			sum += MeasureData.Protect_UAB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UBC_val&0x00FF);			// ���ͱ�����ѹ����
			sum += MeasureData.Protect_UBC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UBC_val>>8);				// ���ͱ�����ѹ����
			sum += MeasureData.Protect_UBC_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UCA_val&0x00FF);			// ���ͱ�����ѹ����
			sum += MeasureData.Protect_UCA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UCA_val>>8);				// ���ͱ�����ѹ����
			sum += MeasureData.Protect_UCA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IA_val&0x00FF);			// ���ͱ�����������
			sum += MeasureData.Protect_IA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IA_val>>8);				// ���ͱ�����������
			sum += MeasureData.Protect_IA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IB_val&0x00FF);			// ���ͱ�����������
			sum += MeasureData.Protect_IB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IB_val>>8);				// ��ͱ�����������
			sum += MeasureData.Protect_IB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IC_val&0x00FF);			// ���ͱ�����������
			sum += MeasureData.Protect_IC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IC_val>>8);				// ���ͱ�����������
			sum += MeasureData.Protect_IC_val>>8;	
			
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IA10_val&0x00FF);			// ����10��������������
			sum += MeasureData.Protect_IA10_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IA10_val>>8);				// ����10��������������
			sum += MeasureData.Protect_IA10_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IB10_val&0x00FF);			// ����10��������������
			sum += MeasureData.Protect_IB10_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IB10_val>>8);				// ����10��������������
			sum += MeasureData.Protect_IB10_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IC10_val&0x00FF);			// ����10��������������
			sum += MeasureData.Protect_IC10_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IC10_val>>8);				// ����10��������������
			sum += MeasureData.Protect_IC10_val>>8;
			BTYX1 = ~(FW_IN1|(HW_IN2 << 1)|(WCN_IN3<<2)|(DJ_IN4<<3)|(DJ_IN5<<4)|(FAR_IN6<<5)|(LOCAL_IN7<<6)|1<<7);
			BTYX2 = ~((DYXH1&DYXH2)|(LED_Judge << 1));
			BTYX3 = (MeasureData.YaBan1&0x01)|((MeasureData.YaBan2&0x01)<<1)|((MeasureData.YaBan3&0x01)<<2);
      BTYX4 = 0;
			if(TimeNow.year > 18 && TimeNow.year < 99)
			{
				if(TimeNow.month > 0 && TimeNow.month < 13)
					if(TimeNow.date > 0 && TimeNow.date < 32)
						if(TimeNow.hour<24)
							if(TimeNow.minute<60)
								if(TimeNow.second<60)
									BTYX4 |=	0x01;
			}
      if(KZQMeasureData.para.SelfCheck.bit.Temp_Humi)
			  BTYX4 |=	0x02;
			if(KZQMeasureData.para.SelfCheck.bit.GPRS_state != 1)
				BTYX4 |=	0x04;
			if(KZQMeasureData.para.SelfCheck.bit.GPS_state)
			{
				BTYX4 |=	0x08;
			}
			else 
			{
				GPS_Flag = 1;
			}
			if(KZQMeasureData.para.SelfCheck.bit.WIFI_state)
			{
				BTYX4 |=	0x10;
			}

			InsertDataToQueue(&QueueTX2,BufferTX2,BTYX1);				// ����ң������1
			sum += BTYX1;
			InsertDataToQueue(&QueueTX2,BufferTX2,BTYX2);				// ����ң������2
			sum += BTYX2;
			InsertDataToQueue(&QueueTX2,BufferTX2,BTYX3);				// ����ң������3
			sum += BTYX3;
			InsertDataToQueue(&QueueTX2,BufferTX2,BTYX4);				// ����ң������4
			sum += BTYX4;
			
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.V_BAT&0x00FF);	        	//��ص�ѹ
			sum += MeasureData.V_BAT&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.V_BAT>>8);			
			sum += MeasureData.V_BAT>>8;        	
			
      InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_10I0_val&0x00FF);// ����10�������������
			sum += MeasureData.Protect_10I0_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_10I0_val>>8);	   // ����10�������������
 			sum += MeasureData.Protect_10I0_val>>8;
			
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UA_val&0x00FF);			     // ���͵�ѹ���  ATT7022
			sum += MeasureData.UA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UA_val>>8);				       // ���͵�ѹ����  ATT7022
			sum += MeasureData.UA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UB_val&0x00FF);			     // ���͵�ѹ����  ATT7022
			sum += MeasureData.UB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UB_val>>8);				       // ���͵�ѹ����  ATT7022
			sum += MeasureData.UB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UC_val&0x00FF);			     // ���͵�ѹ����  ATT7022
			sum += MeasureData.UC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.UC_val>>8);				       // ���͵�ѹ����  ATT7022
			sum += MeasureData.UC_val>>8;

			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UA_val&0x00FF);				// �������ѹ����
			sum += MeasureData.Protect_UA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UA_val>>8);				    // �������ѹ����
			sum += MeasureData.Protect_UA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UB_val&0x00FF);				// �������ѹ����
			sum += MeasureData.Protect_UB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UB_val>>8);				    // �������ѹ����
			sum += MeasureData.Protect_UB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UC_val&0x00FF);				// �������ѹ����
			sum += MeasureData.Protect_UC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UC_val>>8);				    // �������ѹ����
			sum += MeasureData.Protect_UC_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_U0_val&0x00FF);				// ���������ѹ����
			sum += MeasureData.Protect_U0_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_U0_val>>8);				    // ���������ѹ����
			sum += MeasureData.Protect_U0_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_I0_val&0x00FF);				// ���������������
			sum += MeasureData.Protect_I0_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_I0_val>>8);				    // ���������������
			sum += MeasureData.Protect_I0_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.freq&0x00FF);				          // ����Ƶ������
			sum += MeasureData.freq&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.freq>>8);				              // ����Ƶ������
			sum += MeasureData.freq>>8;
			
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UA1_val&0x00FF);		  // ����PT1��ѹ����
			sum += MeasureData.Protect_UA1_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UA1_val>>8);				  // ����PT1��ѹ����
			sum += MeasureData.Protect_UA1_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,0);				

			InsertDataToQueue(&QueueTX2,BufferTX2,0);				

			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UC1_val&0x00FF);		  // ����PT2��ѹ����
			sum += MeasureData.Protect_UC1_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_UC1_val>>8);				  // ����PT2��ѹ����
			sum += MeasureData.Protect_UC1_val>>8;

			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IA10SD_val&0x00FF);		// ���ͱ�����������
			sum += MeasureData.Protect_IA10SD_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IA10SD_val>>8);				// ���ͱ�����������
			sum += MeasureData.Protect_IA10SD_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IB10SD_val&0x00FF);		// ���ͱ�����������
			sum += MeasureData.Protect_IB10SD_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IB10SD_val>>8);				// ���ͱ�����������
			sum += MeasureData.Protect_IB10SD_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IC10SD_val&0x00FF);		// ���ͱ�����������
			sum += MeasureData.Protect_IC10SD_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Protect_IC10SD_val>>8);				// ���ͱ�����������
			sum += MeasureData.Protect_IC10SD_val>>8;
			
      InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degUAB_val&0x00FF);		// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degUAB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degUAB_val>>8);				// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degUAB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degUA_val&0x00FF);		// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degUA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degUA_val>>8);				// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degUA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degUB_val&0x00FF);		// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degUB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degUB_val>>8);				// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degUB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degUC_val&0x00FF);		// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degUC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degUC_val>>8);				// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degUC_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degU0_val&0x00FF);		// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degU0_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degU0_val>>8);				// ���͵�ѹ�Ƕ�����
			sum += MeasureData.Display_degU0_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degIA_val&0x00FF);		// ���͵����Ƕ�����
			sum += MeasureData.Display_degIA_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degIA_val>>8);				// ���͵����Ƕ�����
			sum += MeasureData.Display_degIA_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degIB_val&0x00FF);		// ���͵����Ƕ�����
			sum += MeasureData.Display_degIB_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degIB_val>>8);				// ���͵����Ƕ�����
			sum += MeasureData.Display_degIB_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degIC_val&0x00FF);		// ���͵����Ƕ�����
			sum += MeasureData.Display_degIC_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degIC_val>>8);				// ���͵����Ƕ�����
			sum += MeasureData.Display_degIC_val>>8;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degI0_val&0x00FF);		// ���͵����Ƕ�����
			sum += MeasureData.Display_degI0_val&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,MeasureData.Display_degI0_val>>8);				// ���͵����Ƕ�����
			sum += MeasureData.Display_degI0_val>>8;
			
			InsertDataToQueue(&QueueTX2,BufferTX2,JudgePoint&0x00FF);			                  // ���͵����Ƕ�����
			sum += JudgePoint&0x00FF;
			InsertDataToQueue(&QueueTX2,BufferTX2,JudgePoint>>8);				                    // ���͵����Ƕ�����
			sum += JudgePoint>>8;
			
			InsertDataToQueue(&QueueTX2,BufferTX2,sum);				// ����У���
			InsertDataToQueue(&QueueTX2,BufferTX2,0x16);				// ����֡β
			Battery_Readflag = 0;
			break;
		case 0xC6:			// ���װ�ÿ�������
			TESTFlag = 1;
			TestFlagCnt = 0;
		  if(BackstageProtocol2.DataBuffer[0] & 0x01)
			  OUT6_ON;
      else	
        OUT6_OFF;	
		  if(BackstageProtocol2.DataBuffer[0] & 0x02)
			  OUT4_ON;
      else	
        OUT4_OFF;	
		  if(BackstageProtocol2.DataBuffer[0] & 0x04)
			  OUT5_ON;
      else	
        OUT5_OFF;	
		  if(BackstageProtocol2.DataBuffer[0] & 0x08)
			  BATT_ON;
      else	
        BATT_OFF;	
		  if(BackstageProtocol2.DataBuffer[0] & 0x10)
			  OUT_COM_ON;
      else	
        OUT_COM_OFF;	
		  if(BackstageProtocol2.DataBuffer[0] & 0x20)
			  HE_ON;
      else	
        HE_OFF;
		  if(BackstageProtocol2.DataBuffer[0] & 0x40)
			  FEN_ON;
      else	
        FEN_OFF;			
		  if(BackstageProtocol2.DataBuffer[0] & 0x80)
			  KZCN_ON;
      else	
        KZCN_OFF;			
		  if(BackstageProtocol2.DataBuffer[1] & 0x01)
			  DJZZKZ_ON;
      else	
        DJZZKZ_OFF;		
		  if(BackstageProtocol2.DataBuffer[1] & 0x02)
			  DJFZKZ_ON;
      else	
        DJFZKZ_OFF;		
		  if(BackstageProtocol2.DataBuffer[1] & 0x04)
			  HZLED_ON;
      else	
        HZLED_OFF;		
		  if(BackstageProtocol2.DataBuffer[1] & 0x08)
			  FZLED_ON;
      else	
        FZLED_OFF;
		  if(BackstageProtocol2.DataBuffer[1] & 0x10)
			{
			  BATTERY_CHARGE_ON;
			  KZQMeasureData.para.AlarmFlag.bit.bat_active = 0;
			}
      else	
			{
        BATTERY_CHARGE_OFF;
        KZQMeasureData.para.AlarmFlag.bit.bat_active = 1;			
			}	
		  break;
		case 0xC7:			// У��
			TESTFlag = 1;
			TestFlagCnt = 0;
			switch(BackstageProtocol2.DataBuffer[0])	// У����
			{
				case 0x01:				// ��ʼ
					ATT7022_WREnable();
					ATT7022_Config();
					
					WriteAT7052(0x84,0);
					WriteAT7052(0x85,0);
					WriteAT7052(0x86,0);
					WriteAT7052(0x87,0);
					WriteAT7052(0x88,0);
					WriteAT7052(0x89,0);

					WriteAT7052(0x8D,0);
					WriteAT7052(0x8E,0);
					WriteAT7052(0x8F,0);
					WriteAT7052(0x90,0);
					WriteAT7052(0x91,0);
					WriteAT7052(0x92,0);

					WriteAT7052(0xA4,0);
					WriteAT7052(0xA5,0);
					WriteAT7052(0xA6,0);

					WriteAT7052(0xA7,0);
					WriteAT7052(0xA8,0);
					WriteAT7052(0xA9,0);

					WriteAT7052(0x97,0);
					WriteAT7052(0x98,0);
					WriteAT7052(0x99,0);

					WriteAT7052(0x9A,0);
					WriteAT7052(0x9B,0);
					WriteAT7052(0x9C,0);
					saveATT7022_flag = 1;
					break;
				case 0x02:				// ����У��
					if(saveATT7022_flag == 1)
					{
					  WriteAT7052(0x84,(BackstageProtocol2.DataBuffer[2]<<8)|BackstageProtocol2.DataBuffer[1]);
					  WriteAT7052(0x85,(BackstageProtocol2.DataBuffer[4]<<8)|BackstageProtocol2.DataBuffer[3]);
					  WriteAT7052(0x86,(BackstageProtocol2.DataBuffer[6]<<8)|BackstageProtocol2.DataBuffer[5]);

					  WriteAT7052(0x87,(BackstageProtocol2.DataBuffer[2]<<8)|BackstageProtocol2.DataBuffer[1]);
					  WriteAT7052(0x88,(BackstageProtocol2.DataBuffer[4]<<8)|BackstageProtocol2.DataBuffer[3]);
					  WriteAT7052(0x89,(BackstageProtocol2.DataBuffer[6]<<8)|BackstageProtocol2.DataBuffer[5]);
					}
					break;
				case 0x03:				// ��λУ��
					if(saveATT7022_flag == 1)
					{
					  WriteAT7052(0x8D,(BackstageProtocol2.DataBuffer[2]<<8)|BackstageProtocol2.DataBuffer[1]);
					  WriteAT7052(0x8E,(BackstageProtocol2.DataBuffer[4]<<8)|BackstageProtocol2.DataBuffer[3]);
					  WriteAT7052(0x8F,(BackstageProtocol2.DataBuffer[6]<<8)|BackstageProtocol2.DataBuffer[5]);
				
					  WriteAT7052(0x90,(BackstageProtocol2.DataBuffer[2]<<8)|BackstageProtocol2.DataBuffer[1]);
					  WriteAT7052(0x91,(BackstageProtocol2.DataBuffer[4]<<8)|BackstageProtocol2.DataBuffer[3]);
					  WriteAT7052(0x92,(BackstageProtocol2.DataBuffer[6]<<8)|BackstageProtocol2.DataBuffer[5]);
					}
					break;
		     	case 0x04:				// ��ѹУ��
					if(saveATT7022_flag == 1)
					{
					  WriteAT7052(0x97,(BackstageProtocol2.DataBuffer[2]<<8)|BackstageProtocol2.DataBuffer[1]);
					  WriteAT7052(0x98,(BackstageProtocol2.DataBuffer[4]<<8)|BackstageProtocol2.DataBuffer[3]);
					  WriteAT7052(0x99,(BackstageProtocol2.DataBuffer[6]<<8)|BackstageProtocol2.DataBuffer[5]);
					}
					break;
				case 0x05:				// ����У��
					if(saveATT7022_flag == 1)
					{
					  WriteAT7052(0x9A,(BackstageProtocol2.DataBuffer[2]<<8)|BackstageProtocol2.DataBuffer[1]);
					  WriteAT7052(0x9B,(BackstageProtocol2.DataBuffer[4]<<8)|BackstageProtocol2.DataBuffer[3]);
					  WriteAT7052(0x9C,(BackstageProtocol2.DataBuffer[6]<<8)|BackstageProtocol2.DataBuffer[5]);	
					}
					break;
				case 0x08:				// ��ѹ���У��
				  if(saveATT7022_flag == 1)
					{
					  WriteAT7052(0xA4,(BackstageProtocol2.DataBuffer[2]<<8)|BackstageProtocol2.DataBuffer[1]);
					  WriteAT7052(0xA5,(BackstageProtocol2.DataBuffer[4]<<8)|BackstageProtocol2.DataBuffer[3]);
					  WriteAT7052(0xA6,(BackstageProtocol2.DataBuffer[6]<<8)|BackstageProtocol2.DataBuffer[5]);
				  }
					break;
				case 0x09:				// �������У��
					if(saveATT7022_flag == 1)
					{
					  WriteAT7052(0xA7,(BackstageProtocol2.DataBuffer[2]<<8)|BackstageProtocol2.DataBuffer[1]);
					  WriteAT7052(0xA8,(BackstageProtocol2.DataBuffer[4]<<8)|BackstageProtocol2.DataBuffer[3]);
					  WriteAT7052(0xA9,(BackstageProtocol2.DataBuffer[6]<<8)|BackstageProtocol2.DataBuffer[5]);	
					}
					break;
				case 0x06:				// ����
					ATT7022_WRDisable();
				  os_dly_wait (100);
					ATT7022_sum = ReadAT7052(CHKSUM);
					ATT7022_RDEnable();
					ZYXSA = ReadAT7052(0x04);
					ZYXSB = ReadAT7052(0x05);
					ZYXSC = ReadAT7052(0x06);

					XWXSA = ReadAT7052(0x0D);
					XWXSB = ReadAT7052(0x0E);
					XWXSC = ReadAT7052(0x0F);

					WGXSA = ReadAT7052(0x07);
					WGXSB = ReadAT7052(0x08);
					WGXSC = ReadAT7052(0x09);

					DYXSA = ReadAT7052(0x17);
					DYXSB = ReadAT7052(0x18);
					DYXSC = ReadAT7052(0x19);

					DLXSA = ReadAT7052(0x1A);
					DLXSB = ReadAT7052(0x1B);
					DLXSC = ReadAT7052(0x1C);
					//
					DYXSA1 = ReadAT7052(0x24);
					DYXSB1 = ReadAT7052(0x25);
					DYXSC1 = ReadAT7052(0x26);

					DLXSA1 = ReadAT7052(0x27);
					DLXSB1 = ReadAT7052(0x28);
					DLXSC1 = ReadAT7052(0x29);
					ATT7022_RDDisable();
					
					if(saveATT7022_flag)
					  SaveATT7022();		// ����У�����
					saveATT7022_flag = 0;
					
					sum = 0;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xFE);				// ��������ͷ
					sum += 0xFE;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xEF);				// ��������ͷ
					sum += 0xEF;
					InsertDataToQueue(&QueueTX2,BufferTX2,BackstageProtocol2.Device);			// �����豸����
					sum += BackstageProtocol2.Device;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x01);				// �����豸��ַ
					sum += 0x01;
					InsertDataToQueue(&QueueTX2,BufferTX2,0xC7);				// ����������
					sum += 0xC7;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x21+12);				// ���ͳ��ȵ�λ
					sum += (0x21+12);
					InsertDataToQueue(&QueueTX2,BufferTX2,0x00);				// ���ͳ��ȸ�λ
					sum += 0x00;
					InsertDataToQueue(&QueueTX2,BufferTX2,0x07);				// ���Ͷ���������
					sum += 0x07;

					InsertDataToQueue(&QueueTX2,BufferTX2,ZYXSA&0x00FF);
					sum += ZYXSA&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,ZYXSA>>8);
					sum += ZYXSA>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,ZYXSB&0x00FF);
					sum += ZYXSB&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,ZYXSB>>8);
					sum += ZYXSB>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,ZYXSC&0x00FF);
					sum += ZYXSC&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,ZYXSC>>8);
					sum += ZYXSC>>8;
					
					InsertDataToQueue(&QueueTX2,BufferTX2,XWXSA&0x00FF);
					sum += XWXSA&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,XWXSA>>8);
					sum += XWXSA>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,XWXSB&0x00FF);
					sum += XWXSB&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,XWXSB>>8);
					sum += XWXSB>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,XWXSC&0x00FF);
					sum += XWXSC&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,XWXSC>>8);
					sum += XWXSC>>8;
					
					InsertDataToQueue(&QueueTX2,BufferTX2,WGXSA&0x00FF);
					sum += WGXSA&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,WGXSA>>8);
					sum += WGXSA>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,WGXSB&0x00FF);
					sum += WGXSB&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,WGXSB>>8);
					sum += WGXSB>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,WGXSC&0x00FF);
					sum += WGXSC&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,WGXSC>>8);
					sum += WGXSC>>8;
					
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSA&0x00FF);
					sum += DYXSA&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSA>>8);
					sum += DYXSA>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSB&0x00FF);
					sum += DYXSB&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSB>>8);
					sum += DYXSB>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSC&0x00FF);
					sum += DYXSC&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSC>>8);
					sum += DYXSC>>8;
					
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSA&0x00FF);
					sum += DLXSA&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSA>>8);
					sum += DLXSA>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSB&0x00FF);
					sum += DLXSB&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSB>>8);
					sum += DLXSB>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSC&0x00FF);
					sum += DLXSC&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSC>>8);
					sum += DLXSC>>8;
					
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSA1&0x00FF);
					sum += DYXSA1&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSA1>>8);
					sum += DYXSA1>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSB1&0x00FF);
					sum += DYXSB1&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSB1>>8);
					sum += DYXSB1>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSC1&0x00FF);
					sum += DYXSC1&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DYXSC1>>8);
					sum += DYXSC1>>8;
					
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSA1&0x00FF);
					sum += DLXSA1&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSA1>>8);
					sum += DLXSA1>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSB1&0x00FF);
					sum += DLXSB1&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSB1>>8);
					sum += DLXSB1>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSC1&0x00FF);
					sum += DLXSC1&0x00FF;
					InsertDataToQueue(&QueueTX2,BufferTX2,DLXSC1>>8);
					sum += DLXSC1>>8;
					InsertDataToQueue(&QueueTX2,BufferTX2,sum);
					InsertDataToQueue(&QueueTX2,BufferTX2,0x16);										
					break;
				default:
					break;
			}
			break;
		case 0xC9:
			for(i=0;i<(sizeof(struct AUTO_SWITCHOFF)/2);i++)
			{
				AutoswitchSet.word[i] = BackstageProtocol2.DataBuffer[i*2] | (BackstageProtocol2.DataBuffer[i*2+1]<<8);
			}	
			SetAutoswitchFlag=1;	
			break;
		case 0xCA:	// ��ʵʱ��������
			if(MeasureData.Psaveflash == 0)
			{
				#ifdef TEST
				Curve_cnt = 0;
				#endif
				RealCurveFlag = 0x01;
				os_dly_wait (30);
				Read_CurveData();
				RealCurveFlag = 0;
				AD_StartPoint = 0;
				MeasureData.Psaveflag = 0;
				os_dly_wait (500);
			}
			break;
	  case 0xBB:	// �ϴ�������������
			if(MeasureData.Psaveflash == 0)
			{
		    Read_InstantData();
		    os_dly_wait (500);
			}
			break;
		case 0xC4:			// ��λ����ȡ101��Լ����
			for(i=0;i<(sizeof(struct Rule101Para));i++)
			{	
				Measure101Para.word[i]=BackstageProtocol2.DataBuffer[i];
			}
			Set101ParaFlag=1;	
			break;
		default:
			break;
	}	
}

