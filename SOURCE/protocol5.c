//###########################################################################
//
// ����5ͨѶЭ���ļ�
// wifiͨѶ
// 
//
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  18.03| 13 mary 2018 | S.H.Q. | 
//###########################################################################
#include "protocol5.h"
#include "protocol1.h"
#include "GPS.h"
#include "queue.h"
#include "flash.h"
struct BACKSTAGE_PROTOCOL_TYPE5 BackstageProtocol5;
struct BACKSTAGE_PROTOCOL_TYPE5 BackstageProtocol5bak;

static u8 sum = 0;
static volatile u8 ProtocolResolveState = 0;	// Э���������״̬��
static u8 channel = 0;			                  // ����ͨ��
static u8 WifiDelay = 0;					            // ģ����ʱ
static u8 WifiTimeout = 0;				            // ģ��ʱ�������־
static u8 IDnumber = 0;
static u8 IDnumberLast = 0;
static u8 UpdateFlag = 0;
static u16 WifiWdg = 0;
static u8 state_flag = 0;
u32 UpdateLength;
u8  WifiSelectAddr;
// ASCIIתbin
u8 ASCIItoBIN(u8 text)
{
	u8 bin = 0;
	if(text > 47 && text < 58)
	{
		bin = text - 48;
	}
	else if(text > 64 && text < 71)
	{
		bin = text - 55;
	}
	else
	{
		bin = 0xFF;
	}
	return bin;
}

/****************************************************************************
*
*��    �ƣ�void WifiSendStr(char *str)
*
*��    �ܣ�Wifiģ�鷢���ַ���
*
*��ڲ������ַ����׵�ַ
*
*���ز�����
*
****************************************************************************/
void WifiSendStr(char *str)
{
	char *strcopy;
	strcopy = str;
	while((*strcopy) != '\0')
	{
		InsertDataToQueue(&QueueTX5,BufferTX5,*strcopy);
		strcopy++;
	}	
}

/****************************************************************************
*
*��    �ƣ�void WifiCIPSENDEX(char *str)
*
*��    �ܣ�Wifiģ�鷢������ָ��
*
*��ڲ��������ݸ���
*
*���ز�����
*
****************************************************************************/
void WifiCIPSENDEX(u16 datanum)
{
	char num[4];
	WifiSendStr("AT+CIPSENDEX=");
	InsertDataToQueue(&QueueTX5,BufferTX5,IDnumber);
	InsertDataToQueue(&QueueTX5,BufferTX5,',');
	sprintf((char*)num,"%04d",datanum);
  WifiSendStr(num);
	WifiSendStr("\r\n");
}

/**************************************************************************************
* FunctionName   : Realdata_send(void)
* Description    : �����ƻ�����ʵʱ������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Realdata_send(void)
{
	u8 sum=0xED,i;
	u8 j = 13;
	KZQMeasureData.word[j++]=MeasureData.Display_UAB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UBC_val;
  KZQMeasureData.word[j++]=MeasureData.Display_UCA_val;	
	KZQMeasureData.word[j++]=MeasureData.Display_UA_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UC_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UA1_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UB1_val;
	KZQMeasureData.word[j++]=MeasureData.Display_UC1_val;	
	KZQMeasureData.word[j++]=MeasureData.Display_degUA_val; 
	KZQMeasureData.word[j++]=MeasureData.Display_degUB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degUC_val;
	KZQMeasureData.word[j++]=MeasureData.Display_U0_val;
	if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
	  KZQMeasureData.word[j++]=MeasureData.Display_I0_val/10;
	else 
		KZQMeasureData.word[j++]=MeasureData.Protect_10I0_val;
  KZQMeasureData.word[j++]=MeasureData.freq;
	KZQMeasureData.word[j++]=MeasureData.Display_degU0_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degI0_val;
	KZQMeasureData.word[j++]=MeasureData.Display_IA_val;
	KZQMeasureData.word[j++]=MeasureData.Display_IB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_IC_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degIA_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degIB_val;
	KZQMeasureData.word[j++]=MeasureData.Display_degIC_val;	
	KZQMeasureData.word[j++]=(u16)MeasureData.P_val;
	KZQMeasureData.word[j++]=MeasureData.Q_val;
	KZQMeasureData.word[j++]=MeasureData.cosALL;	
	KZQMeasureData.word[j++]=MeasureData.Wp;
	KZQMeasureData.word[j++]=MeasureData.Wq;
	KZQMeasureData.word[j++]=MeasureData.cos_W;	
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[0];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[1];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[2];
  KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[3];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[4];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[5];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[6];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[7];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_U[8];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[0];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[1];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[2];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[3];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[4];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[5];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[6];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[7];
	KZQMeasureData.word[j++]=MeasureData.DIS_XB_I[8];	
	KZQMeasureData.word[j++]=MeasureData.Protect_UAB_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_UBC_val;
	KZQMeasureData.word[j++]=dycfzc2;
	KZQMeasureData.word[j++]=MeasureData.Display_degUAB_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IA_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IB_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IC_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IA10_val;
	KZQMeasureData.word[j++]=MeasureData.Protect_IB10_val;	
	KZQMeasureData.word[j++]=MeasureData.Protect_IC10_val;	
	KZQMeasureData.word[j++]=zxdjdfs;
	KZQMeasureData.word[j++]=jdxb;
	KZQMeasureData.word[j++]=dycfzc;
	KZQMeasureData.word[j++]=jdxz;
	KZQMeasureData.word[j++]=jdyf;	
	KZQMeasureData.word[j++]=hgxb;
	KZQMeasureData.word[j++]=hggdy;
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_1all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_1all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_peak&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_peak>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_2all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_2all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_valley&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_valley>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_3all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_3all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_level&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wp_level>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_4all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.Wq_4all>>16);	
	KZQMeasureData.word[j++]=MeasureData.UA_val;
	KZQMeasureData.word[j++]=MeasureData.UB_val;
	KZQMeasureData.word[j++]=MeasureData.UC_val;
	KZQMeasureData.word[j++]=MeasureData.IA_val;
	KZQMeasureData.word[j++]=MeasureData.IB_val;
	KZQMeasureData.word[j++]=MeasureData.IC_val;
	KZQMeasureData.word[j++]=MeasureData.cosA;
	KZQMeasureData.word[j++]=MeasureData.cosB;
	KZQMeasureData.word[j++]=MeasureData.cosC;	
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_all&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_all>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_peak&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_peak>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_valley&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_valley>>16);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_level&0xFFFF);
	KZQMeasureData.word[j++]=((s32)MeasureData.WpFX_level>>16);
	KZQMeasureData.word[j++]= TimeNow.year;
	KZQMeasureData.word[j++]= TimeNow.month;
	KZQMeasureData.word[j++]= TimeNow.date;
	KZQMeasureData.word[j++]= TimeNow.hour;
	KZQMeasureData.word[j++]= TimeNow.minute;
	KZQMeasureData.word[j++]= TimeNow.second;
  KZQMeasureData.word[j]= MeasureData.V_BAT;

	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFF);
	sum+=0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,sizeof(struct KZQMEASURE)+2);
	sum+=sizeof(struct KZQMEASURE)+2;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	for(i=0;i<(sizeof(struct KZQMEASURE)/2);i++)
	{	
	  InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.word[i]&0xFF);
	  sum+=KZQMeasureData.word[i]&0xFF;
		InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.word[i]>>8);
	  sum+=KZQMeasureData.word[i]>>8;
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

/**************************************************************************************
* FunctionName   : Systemdata_send(void)
* Description    : �����ƻ�����ϵͳ����������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Systemdata_send(void)
{
	u8 sum=0xED,i;
	ReadSystemPara();	// ��ϵͳ���ò���
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xF1);
	sum+=0xF1;
	InsertDataToQueue(&QueueTX5,BufferTX5,sizeof(struct SYSTEM_PARA)+2);
	sum+=sizeof(struct SYSTEM_PARA)+2;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	for(i=0;i<((sizeof(struct SYSTEM_PARA)/2)-6);i++)
	{	
	  InsertDataToQueue(&QueueTX5,BufferTX5,SystemSet.word[i]&0xFF);
	  sum+=SystemSet.word[i]&0xFF;
		InsertDataToQueue(&QueueTX5,BufferTX5,SystemSet.word[i]>>8);
	  sum+=SystemSet.word[i]>>8;
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,TimeNow.year&0xFF);
	sum+=TimeNow.year&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	InsertDataToQueue(&QueueTX5,BufferTX5,TimeNow.month&0xFF);
	sum+=TimeNow.month&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	InsertDataToQueue(&QueueTX5,BufferTX5,TimeNow.date&0xFF);
	sum+=TimeNow.date&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	InsertDataToQueue(&QueueTX5,BufferTX5,TimeNow.hour&0xFF);
	sum+=TimeNow.hour&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	InsertDataToQueue(&QueueTX5,BufferTX5,TimeNow.minute&0xFF);
	sum+=TimeNow.minute&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	InsertDataToQueue(&QueueTX5,BufferTX5,TimeNow.second&0xFF);
	sum+=TimeNow.second&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

/**************************************************************************************
* FunctionName   : Protectdata_send(void)
* Description    : �����ƻ����ͱ�������������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Protectdata_send(void)
{
	u8 sum=0xED,i;
	ReadProtectPara();	// ��ȡ��������
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x02);   //�°汾��������
	sum+=0x02;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xF2);
	sum+=0xF2;
	InsertDataToQueue(&QueueTX5,BufferTX5,sizeof(struct PROTECT_PARA)+2);
	sum+=sizeof(struct PROTECT_PARA)+2;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	for(i=0;i<((sizeof(struct PROTECT_PARA))/2);i++)
	{	
	  InsertDataToQueue(&QueueTX5,BufferTX5,ProtectSet.word[i]&0xFF);
	  sum+=ProtectSet.word[i]&0xFF;
		InsertDataToQueue(&QueueTX5,BufferTX5,ProtectSet.word[i]>>8);
	  sum+=ProtectSet.word[i]>>8;
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

/**************************************************************************************
* FunctionName   : Alarmdata_send(void)
* Description    : �����ƻ����ͱ�������������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Alarmdata_send(void)
{
	u8 sum=0xED,i;
	ReadAlarmPara();
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xF3);
	sum+=0xF3;
	InsertDataToQueue(&QueueTX5,BufferTX5,sizeof(struct ALARM_PARA)+2);
	sum+=sizeof(struct ALARM_PARA)+2;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	for(i=0;i<((sizeof(struct ALARM_PARA))/2);i++)
	{	
	  InsertDataToQueue(&QueueTX5,BufferTX5,AlarmSet.word[i]&0xFF);
	  sum+=AlarmSet.word[i]&0xFF;
		InsertDataToQueue(&QueueTX5,BufferTX5,AlarmSet.word[i]>>8);
	  sum+=AlarmSet.word[i]>>8;
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

/**************************************************************************************
* FunctionName   : AutoSwitchdata_send(void)
* Description    : �����ƻ����Ϳ��ƽ��в���������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void AutoSwitchdata_send(void)
{
	u8 sum=0xED,i;
	ReadAutoSwitchPara();
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xF4);
	sum+=0xF4;
	InsertDataToQueue(&QueueTX5,BufferTX5,sizeof(struct AUTO_SWITCHOFF)+2); 
	sum+=sizeof(struct AUTO_SWITCHOFF)+2;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	for(i=0;i<((sizeof(struct AUTO_SWITCHOFF))/2);i++)
	{	
	  InsertDataToQueue(&QueueTX5,BufferTX5,AutoswitchSet.word[i]&0xFF);
	  sum+=AutoswitchSet.word[i]&0xFF;
		InsertDataToQueue(&QueueTX5,BufferTX5,AutoswitchSet.word[i]>>8);
	  sum+=AutoswitchSet.word[i]>>8;
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

/**************************************************************************************
* FunctionName   : Battery_Selfcheck_datasend(void)
* Description    : �����ƻ����͵�������������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Battery_Selfcheck_datasend(void)
{
	u8 bufread[360];
	u8 sum=0xED,i,energy,state;
  BATTERY_CHARGE_OFF;
	Battery_Readflag = 1;
	os_dly_wait (100);
	CS2BZ=0;
	//SPI_Flash_Read(bufread,FLASH_BATVOLTAGE_ADDR,360);
	for(i=0;i<120;i++)
	{
		if(bufread[2+3*i] >100 || bufread[2+3*i] <30)
			bufread[2+3*i] = 0xFF;
	}
  energy = MeasureData.V_BAT/24;
	if(energy>100) energy=100;
	if(MeasureData.V_BAT<500) 
		state =2;
	else if(MeasureData.V_BAT>500 && MeasureData.V_BAT<1800)
		state =1;
	else
		state =0;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xF5);
	sum+=0xF5;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x82); 
	sum+=0x82;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,energy);
	sum+=energy;
	InsertDataToQueue(&QueueTX5,BufferTX5,state);
	sum+=state;
	InsertDataToQueue(&QueueTX5,BufferTX5,MeasureData.V_BAT&0xFF);
	sum+=MeasureData.V_BAT&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,MeasureData.V_BAT>>8);
	sum+=MeasureData.V_BAT>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	InsertDataToQueue(&QueueTX5,BufferTX5,Battery_RunTime&0xFF);
	sum+=Battery_RunTime&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,Battery_RunTime>>8);
	sum+=Battery_RunTime>>8;
  if((bufread[0] | (bufread[1]<<8))!=0xFFFF)
	{
	  if((bufread[0] | (bufread[1]<<8))>FLASH_BATVOLTAGE_MAX)
		{
			if(Battery_Voltage_Index<FLASH_BATVOLTAGE_MAX)
			{
			  for(i=(Battery_Voltage_Index+1);i<FLASH_BATVOLTAGE_MAX;i++)
	      {
			    InsertDataToQueue(&QueueTX5,BufferTX5,bufread[i*3+2]&0xFF);
	        sum+=bufread[i*3+2]&0xFF;
			  }
				for(i=0;i<(Battery_Voltage_Index+1);i++)
	      {
			    InsertDataToQueue(&QueueTX5,BufferTX5,bufread[i*3+2]&0xFF);
	        sum+=bufread[i*3+2]&0xFF;
			  }
			}
			else
			{
				for(i=0;i<FLASH_BATVOLTAGE_MAX;i++)
	      {
			    InsertDataToQueue(&QueueTX5,BufferTX5,bufread[i*3+2]&0xFF);
	        sum+=bufread[i*3+2]&0xFF;
			  }
			}
		}
		else
		{
		  if(Battery_Voltage_Index<FLASH_BATVOLTAGE_MAX)
			{
				for(i=0;i<(Battery_Voltage_Index+1);i++)
	      {
			    InsertDataToQueue(&QueueTX5,BufferTX5,bufread[i*3+2]&0xFF);
	        sum+=bufread[i*3+2]&0xFF;
			  }
				for(i=(Battery_Voltage_Index+1);i<FLASH_BATVOLTAGE_MAX;i++)
	      {
			    InsertDataToQueue(&QueueTX5,BufferTX5,0);
			  }
			}
			else
			{
				for(i=0;i<FLASH_BATVOLTAGE_MAX;i++)
	      {
			    InsertDataToQueue(&QueueTX5,BufferTX5,bufread[i*3+2]&0xFF);
	        sum+=bufread[i*3+2]&0xFF;
			  }
			}
		}
	}
	else
	{
	  for(i=0;i<FLASH_BATVOLTAGE_MAX;i++)
	  {
		  InsertDataToQueue(&QueueTX5,BufferTX5,0);
		}
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
	Battery_Readflag = 0;
}

/**************************************************************************************
* FunctionName   : Measure101Para_datasend(void)
* Description    : �����ƻ�����101��Լ����������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Measure101Para_datasend(void)
{
	u8 sum=0xED,i;
	Read101Para();
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xF6);
	sum+=0xF6;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x40); 
	sum+=0x40;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	for(i=0;i<(sizeof(struct Rule101Para));i++)
	{	
	  InsertDataToQueue(&QueueTX5,BufferTX5,Measure101Para.word[i]);
	  sum+=Measure101Para.word[i];
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

/**************************************************************************************
* FunctionName   : Eventdata_send(u16 number)
* Description    : �����ƻ������¼�������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Eventdata_send(u16 number)
{
	u8 bufread[100]={0},i,sum=0xED;
	s16 recordindex=0;
	CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR,2);
  if((bufread[0] | (bufread[1]<<8))!=0xFFFF && number < 501)
	{
	  recordindex = (s16)DLQ_EventRecord_Index - (s16)number +1;
		if((bufread[0] | (bufread[1]<<8))>500)
		{
		  if(recordindex<0) 
			  recordindex=recordindex+500;
			if(recordindex>=0)
			{				
			  SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR+(u32)recordindex*FLASH_EventRecord_LENGTH,100);
				if(bufread[5] < 100 && bufread[7] < 13 && bufread[9] < 32)
				{
			    bufread[0] = number&0xFF;
			    bufread[1] = number>>8;
				}
				else
				{
					bufread[0] = 0;
			    bufread[1] = 0;
				}
			}
		}
		else
		{
		  if((recordindex>=0) && (recordindex<=DLQ_EventRecord_Index))
			{
				SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR+(u32)recordindex*FLASH_EventRecord_LENGTH,100);
				
			  if(bufread[5] < 100 && bufread[7] < 13 && bufread[9] < 32)
				{
			    bufread[0] = number&0xFF;
			    bufread[1] = number>>8;
				}
				else
				{
					bufread[0] = 0;
			    bufread[1] = 0;
				}
			}
			else
			{
				bufread[0] = 0;
			  bufread[1] = 0;
			}
		}
		
		InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
		InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0xE0);
		sum+=0xE0;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x66);
		sum+=0x66;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		sum+=0x00;
		for(i=0;i<100;i++)
		{	
			InsertDataToQueue(&QueueTX5,BufferTX5,bufread[i]);
			sum+=bufread[i];
		}
		InsertDataToQueue(&QueueTX5,BufferTX5,sum);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
	}
	else
	{
		InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
		InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0xE0);
		sum+=0xE0;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x66);
		sum+=0x66;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		sum+=0x00;
		for(i=0;i<100;i++)
		{	
			InsertDataToQueue(&QueueTX5,BufferTX5,0);
			sum+=0;
		}
		InsertDataToQueue(&QueueTX5,BufferTX5,sum);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
	}
}

/**************************************************************************************
* FunctionName   : Curvedata_send(u16 number,u8 frame)
* Description    : �����ƻ����͹���¼������������Ϣ
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Curvedata_send(u16 number,u8 frame)
{
	u8 bufread[400]={0},sum=0xED;
	u16 i;
	s16 recordindex=0;
	CS2BZ=0;
	if(frame < 17)
	{
		SPI_Flash_Read(bufread,FLASH_P1SCURVE_ADDR,2);
		if((bufread[0] | (bufread[1]<<8))!=0xFFFF)
		{
			recordindex = (s16)Curve_Index - (s16)number +1;
			if((bufread[0] | (bufread[1]<<8))>100)
			{
				if(recordindex<0) 
					recordindex=recordindex+100;
				if(recordindex>=0)
					SPI_Flash_Read(bufread,FLASH_P1SCURVE_ADDR+((u32)recordindex*FLASH_P1SCURVE_LENGTH+(u32)frame*400-300),400);				
			}
			else
			{
				if((recordindex>=0) && (recordindex<=DLQ_EventRecord_Index))
					SPI_Flash_Read(bufread,FLASH_P1SCURVE_ADDR+(u32)recordindex*FLASH_P1SCURVE_LENGTH+(u32)frame*400-300,400);				
			}
		}
		else
	  {
      bufread[0] = 0;
		  bufread[1] = 0;
	  }
		if((frame==13) || (frame==14) || (frame==15) || (frame==16))
		{
		  if(bufread[0]==0xFF && (bufread[1]==0xFF) && (bufread[2]==0xFF) && (bufread[3]==0xFF))
			{
				for(i=0;i<400;i++)
				{
					bufread[i] = 0;
				}
			}
		}
		InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
		InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0xE1);
		sum+=0xE1;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x95);
		sum+=0x95;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,number&0xFF);
		sum+=number&0xFF;
		InsertDataToQueue(&QueueTX5,BufferTX5,number>>8);
		sum+=number>>8;
		InsertDataToQueue(&QueueTX5,BufferTX5,frame);
		sum+=frame;
		for(i=0;i<400;i++)
		{	
			InsertDataToQueue(&QueueTX5,BufferTX5,bufread[i]);
			sum+=bufread[i];
		}
		InsertDataToQueue(&QueueTX5,BufferTX5,sum);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
  }
}

/**************************************************************************************
* FunctionName   : Program_Update_Send(u16 number,u8 result)
* Description    : KZQ��������
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Program_Update_Send(u16 number,u8 result)
{
	u8 buf[600],sum=0xED;
	u16 i;
	if(number==1)
	  UpdateLength=0;
	for(i=0;i<(BackstageProtocol5.length-4);i++)
	{
		if(i<600)
		  buf[i] = BackstageProtocol5.DataBuffer[2+i];
// 		InsertDataToQueue(&QueueTX2,BufferTX2,buf[i]);
	}
	CS2BZ=0;
	SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+UpdateLength,BackstageProtocol5.length-4);
	os_dly_wait (10);
	UpdateLength += (BackstageProtocol5.length-4);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEE);
	sum+=0xEE;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x05);
	sum+=0x05;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,number&0xFF);
	sum+=number&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,number>>8);
	sum+=number>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,result);
	sum+=result;
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
	UpdateFlag = 1;
}
/**************************************************************************************
* FunctionName   : Program_Updata_Lastframe(void)
* Description    : KZQ��������  (����ȷ��֡)
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Program_Updata_Lastframe(void)
{
  u8 result,sum=0xED;
	u8 buf[10];
	if(UpdateLength == (BackstageProtocol5.DataBuffer[0]|(BackstageProtocol5.DataBuffer[1]<<8)|(BackstageProtocol5.DataBuffer[2]<<16)))
    result = 0x5A;
	else
		result = 0xA5;
	if(UpdateFlag == 1)
	{
		UpdateFlag = 0;
		InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
		InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0xEE);
		sum+=0xEE;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x05);
		sum+=0x05;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		sum+=0x00;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		sum+=0x00;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		sum+=0x00;
		InsertDataToQueue(&QueueTX5,BufferTX5,result);
		sum+=result;
		InsertDataToQueue(&QueueTX5,BufferTX5,sum);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
		WifiSendStr("\\0");
		if(result == 0x5A)
		{
			buf[0] = TimeNow.year;
			buf[1] = TimeNow.month;
		  buf[2] = TimeNow.date;
		  buf[3] = TimeNow.hour;
		  buf[4] = TimeNow.minute;
		  buf[5] = TimeNow.second;
		  buf[6] = UpdateLength & 0x000000FF;
		  buf[7] = (UpdateLength & 0x0000FF00) >> 8;
		  buf[8] = (UpdateLength & 0x00FF0000) >> 16;
		  buf[9] = (UpdateLength & 0xFF000000) >> 24;
			CS2BZ=0;
		  SPI_Flash_Write(buf,FLASH_KZQUPDATE_ADDR+FLASH_KZQUPDATE_LENGTH-10,10); 	// �������ֽ���
			RecoverKZQBackupFlag = 1;
		}
	}
}

/**************************************************************************************
* FunctionName   : Programbak_Update_Send(u16 number,u8 result)
* Description    : KZQ���ݳ�������
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Programbak_Update_Send(u16 number,u8 result)
{
	u8 buf[600],sum=0xED;
	u16 i;
	if(number==1)
		UpdateLength=0;
	for(i=0;i<(BackstageProtocol5.length-4);i++)
	{
		if(i<600)
	    buf[i] = BackstageProtocol5.DataBuffer[2+i];
	}
	CS2BZ=0;
	SPI_Flash_Write(buf,FLASH_KZQBACKUP_ADDR+UpdateLength,BackstageProtocol5.length-4);
	os_dly_wait (10);
	UpdateLength += (BackstageProtocol5.length-4);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEC);
	sum+=0xEC;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x05);
	sum+=0x05;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,number&0xFF);
	sum+=number&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,number>>8);
	sum+=number>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,result);
	sum+=result;
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
	UpdateFlag = 3;
}
/**************************************************************************************
* FunctionName   : Programbak_Updata_Lastframe(void)
* Description    : KZQ���ݳ�������  (����ȷ��֡)
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void Programbak_Updata_Lastframe(void)
{
  u8 result,sum=0xED;
	u8 buf[10];
	if(UpdateLength == (BackstageProtocol5.DataBuffer[0]|(BackstageProtocol5.DataBuffer[1]<<8)|(BackstageProtocol5.DataBuffer[2]<<16)))
    result = 0x5A;
	else
		result = 0xA5;
	if(UpdateFlag == 3)
	{
		UpdateFlag = 0;
		InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
		InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0xEC);
		sum+=0xEC;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x05);
		sum+=0x05;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		sum+=0x00;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		sum+=0x00;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		sum+=0x00;
		InsertDataToQueue(&QueueTX5,BufferTX5,result);
		sum+=result;
		InsertDataToQueue(&QueueTX5,BufferTX5,sum);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
		WifiSendStr("\\0");
		if(result == 0x5A)
		{
			buf[0] = TimeNow.year;
			buf[1] = TimeNow.month;
		  buf[2] = TimeNow.date;
		  buf[3] = TimeNow.hour;
		  buf[4] = TimeNow.minute;
		  buf[5] = TimeNow.second;
		  buf[6] = UpdateLength & 0x000000FF;
		  buf[7] = (UpdateLength & 0x0000FF00) >> 8;
		  buf[8] = (UpdateLength & 0x00FF0000) >> 16;
		  buf[9] = (UpdateLength & 0xFF000000) >> 24;
			CS2BZ=0;
		  SPI_Flash_Write(buf,FLASH_KZQBACKUP_ADDR+FLASH_KZQBACKUP_LENGTH-10,10); 	// �������ֽ���
			RecoverKZQBackupFlag = 2;
		}
	}
}

/**************************************************************************************
* FunctionName   : HeartJump(u8 instruct,u8 data)
* Description    : ��������
* EntryParameter : ��
* ReturnValue    : None
**************************************************************************************/
void HeartJump(u8 instruct,u8 data)
{
	u8 sum=0xED;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,instruct);
	sum+=instruct;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x03);
	sum+=0x03;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,data);
	sum+=data;
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

void Humi_Temp_value(void)
{
	u8 sum=0xED;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xA7);
	sum+=0xA7;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x06);
	sum+=0x06;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,Humi_value & 0xFF);
	sum+=Humi_value & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(Humi_value>>8) & 0xFF);
	sum+=(Humi_value>>8) & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,Temp_value & 0xFF);
	sum+=Temp_value & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(Temp_value>>8) & 0xFF);
	sum+=(Temp_value>>8) & 0xFF;
  InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

void VersionInformation(void)
{
	u8 sum=0xED;
	u8 i;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xAE);
	sum+=0xAE;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x37);
	sum+=0x37;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	for(i=0;i<4;i++)
	{
		InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.KZQ_version[i]);
	  sum+=MyVersion.KZQ_version[i];
	}
	for(i=0;i<4;i++)
	{
		InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.IAP_version[i]);
	  sum+=MyVersion.IAP_version[i];
	}
	
	InsertDataToQueue(&QueueTX5,BufferTX5,'C');
	sum+='C';
	InsertDataToQueue(&QueueTX5,BufferTX5,'a');
	sum+='a';
	InsertDataToQueue(&QueueTX5,BufferTX5,'p');
	sum+='p';
	InsertDataToQueue(&QueueTX5,BufferTX5,0);
	
	InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.KZQ_length_update & 0xff);
	sum+=MyVersion.KZQ_length_update & 0xff;
	InsertDataToQueue(&QueueTX5,BufferTX5,(MyVersion.KZQ_length_update>>8) & 0xff);
	sum+=(MyVersion.KZQ_length_update>>8) & 0xff;
	InsertDataToQueue(&QueueTX5,BufferTX5,(MyVersion.KZQ_length_update>>16) & 0xff);
	sum+=(MyVersion.KZQ_length_update>>16) & 0xff;
	InsertDataToQueue(&QueueTX5,BufferTX5,(MyVersion.KZQ_length_update>>24) & 0xff);
	sum+=(MyVersion.KZQ_length_update>>24) & 0xff;
	for(i=0;i<6;i++)
	{
		InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.KZQ_time_update[i]);
	  sum+=MyVersion.KZQ_time_update[i];
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.KZQ_length_backup & 0xff);
	sum+=MyVersion.KZQ_length_backup & 0xff;
	InsertDataToQueue(&QueueTX5,BufferTX5,(MyVersion.KZQ_length_backup>>8) & 0xff);
	sum+=(MyVersion.KZQ_length_backup>>8) & 0xff;
	InsertDataToQueue(&QueueTX5,BufferTX5,(MyVersion.KZQ_length_backup>>16) & 0xff);
	sum+=(MyVersion.KZQ_length_backup>>16) & 0xff;
	InsertDataToQueue(&QueueTX5,BufferTX5,(MyVersion.KZQ_length_backup>>24) & 0xff);
	sum+=(MyVersion.KZQ_length_backup>>24) & 0xff;
	for(i=0;i<6;i++)
	{
		InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.KZQ_time_backup[i]);
	  sum+=MyVersion.KZQ_time_backup[i];
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.Product_number & 0xff);  // ��������
	sum+=MyVersion.Product_number & 0xff;
	InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.Product_number>>8);
	sum+=MyVersion.Product_number >> 8;
	InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.Product_flow & 0xff);    // ������ˮ
	sum+=MyVersion.Product_flow & 0xff;
	InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.Product_flow>>8);
	sum+=MyVersion.Product_flow >> 8;
  InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.Product_date[0]);
	sum+=MyVersion.Product_date[0];
	InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.Product_date[1]);
	sum+=MyVersion.Product_date[1];
	InsertDataToQueue(&QueueTX5,BufferTX5,MyVersion.Product_date[2]);
	sum+=MyVersion.Product_date[2];

	for(i=0;i<14;i++)
	{
		InsertDataToQueue(&QueueTX5,BufferTX5,0);
	}

	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

void RandID_Send(void)
{
	u8 sum=0xED;
// 	srand(os_time_get());
// 	pbuffer[0] = rand()&0xff;
// 	pbuffer[1] = (rand()>>8)&0xff;
// 	pbuffer[2] = (rand()>>16)&0xff;
// 	pbuffer[3] = (rand()>>24)&0xff;
// 	SPI_Flash_Write(pbuffer,FLASH_RandID_ADDR,FLASH_RandID_LENGTH); 	// �������ֽ���
// 	SPI_Flash_Read(pbuffer,FLASH_RandID_ADDR,FLASH_RandID_LENGTH);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xA6);
	sum+=0xA6;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x06);
	sum+=0x06;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,Lock_IdCode&0xFF);
	sum+=Lock_IdCode&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(Lock_IdCode>>8)&0xFF);
	sum+=(Lock_IdCode>>8)&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(Lock_IdCode>>16)&0xFF);
	sum+=(Lock_IdCode>>16)&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(Lock_IdCode>>24)&0xFF);
	sum+=(Lock_IdCode>>24)&0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

void GPS_DataSend(void)
{
	u8 sum=0xED;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xE2);
	sum+=0xE2;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x19);
	sum+=0x19;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
  InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.time*100) & 0xFF);
	sum+=(u32)(Gps.time*100) & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.time*100)>>8);
	sum+=(u32)(Gps.time*100)>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.time*100)>>16);
	sum+=(u32)(Gps.time*100)>>16;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.time*100)>>24);
	sum+=(u32)(Gps.time*100)>>24;
	InsertDataToQueue(&QueueTX5,BufferTX5,Gps.state & 0xFF);
	sum+=Gps.state & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.longitude*100000) & 0xFF);
	sum+=(u32)(Gps.longitude*100000) & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.longitude*100000)>>8);
	sum+=(u32)(Gps.longitude*100000)>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.longitude*100000)>>16);
	sum+=(u32)(Gps.longitude*100000)>>16;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.longitude*100000)>>24);
	sum+=(u32)(Gps.longitude*100000)>>24;
	InsertDataToQueue(&QueueTX5,BufferTX5,Gps.NS & 0xFF);
	sum+=Gps.NS & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.latitude*100000) & 0xFF);
	sum+=(u32)(Gps.latitude*100000) & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.latitude*100000)>>8);
	sum+=(u32)(Gps.latitude*100000)>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.latitude*100000)>>16);
	sum+=(u32)(Gps.latitude*100000)>>16;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u32)(Gps.latitude*100000)>>24);
	sum+=(u32)(Gps.latitude*100000)>>24;
	InsertDataToQueue(&QueueTX5,BufferTX5,Gps.EW & 0xFF);
	sum+=Gps.EW & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u16)(Gps.speed*10) & 0xFF);
	sum+=(u16)(Gps.speed*10) & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u16)(Gps.speed*10)>>8);
	sum+=(u16)(Gps.speed*10)>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u16)(Gps.direction*10) & 0xFF);
	sum+=(u16)(Gps.direction*10) & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,(u16)(Gps.direction*10)>>8);
	sum+=(u16)(Gps.direction*10)>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,Gps.date & 0xFF);
	sum+=Gps.date & 0xFF;
	InsertDataToQueue(&QueueTX5,BufferTX5,Gps.date>>8);
	sum+=Gps.date>>8;
	InsertDataToQueue(&QueueTX5,BufferTX5,Gps.date>>16);
	sum+=Gps.date>>16;
	InsertDataToQueue(&QueueTX5,BufferTX5,Gps.date>>24);
	sum+=Gps.date>>24;
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

void SelfCheck_Send(void)
{
  u8 sum=0xED;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xE3);
	sum+=0xE3;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x1C);
	sum+=0x1C;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.para.DLQflag.bit.breaker);
	sum+=KZQMeasureData.para.DLQflag.bit.breaker;
	InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.para.DLQflag.bit.breaker);
	sum+=KZQMeasureData.para.DLQflag.bit.breaker;
	InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.para.DLQflag.bit.breaker);
	sum+=KZQMeasureData.para.DLQflag.bit.breaker;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage);
	sum+=KZQMeasureData.para.AlarmFlag.bit.battery_low_voltage;
	InsertDataToQueue(&QueueTX5,BufferTX5,(KZQMeasureData.para.SelfCheck.word>>11)&0x07);
	sum+=(KZQMeasureData.para.SelfCheck.word>>11)&0x07;
	InsertDataToQueue(&QueueTX5,BufferTX5,(KZQMeasureData.para.SelfCheck.word>>8)&0x07);
	sum+=(KZQMeasureData.para.SelfCheck.word>>8)&0x07;
	InsertDataToQueue(&QueueTX5,BufferTX5,(KZQMeasureData.para.SelfCheck.word>>8)&0x07);
	sum+=(KZQMeasureData.para.SelfCheck.word>>8)&0x07;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.para.SelfCheck.bit.GPRS_state);//GPRS״̬
	sum+=KZQMeasureData.para.SelfCheck.bit.GPRS_state;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	if(KZQMeasureData.para.SelfCheck.bit.Temp_Humi)
	{
	  InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	  sum+=0x00;
	}
	else
	{
	  InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	  sum+=0x01;
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,(KZQMeasureData.para.SelfCheck.bit.ext_ram | KZQMeasureData.para.SelfCheck.bit.int_ram));
	sum+=(KZQMeasureData.para.SelfCheck.bit.ext_ram | KZQMeasureData.para.SelfCheck.bit.int_ram);
	InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.para.SelfCheck.bit.para_set);
	sum+=KZQMeasureData.para.SelfCheck.bit.para_set;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

void YABAN_Send(void)
{
	u8 sum=0xED;
	u8 error_value = 0;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xE4);
	sum+=0xE4;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x09);
	sum+=0x09;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,MeasureData.YaBan1);
	sum+=MeasureData.YaBan1;
	InsertDataToQueue(&QueueTX5,BufferTX5,MeasureData.YaBan2);
	sum+=MeasureData.YaBan2;
	InsertDataToQueue(&QueueTX5,BufferTX5,MeasureData.YaBan3);
	sum+=MeasureData.YaBan3;
	if(FAR_IN6 == 0 && LOCAL_IN7 == 1)               //Զ��״̬
	{
	  InsertDataToQueue(&QueueTX5,BufferTX5,1);
	  sum+=1;
	}
	else if(FAR_IN6 == 1 && LOCAL_IN7 == 0)          //����״̬
	{
	  InsertDataToQueue(&QueueTX5,BufferTX5,0);
	  sum+=0;
	}
	else                                               //����״̬
	{
		InsertDataToQueue(&QueueTX5,BufferTX5,2);
		sum+=2;
	}
	InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.para.DLQflag.bit.breaker);
	sum+=KZQMeasureData.para.DLQflag.bit.breaker;
	InsertDataToQueue(&QueueTX5,BufferTX5,KZQMeasureData.para.DLQflag.bit.store);
	sum+=KZQMeasureData.para.DLQflag.bit.store;
	
	if(ProtectFlag)
	  error_value |= 0x01;
	else
		error_value &= ~0x01;
	
	if(KZQMeasureData.para.AlarmFlag.word)
	  error_value |= 0x02;
	else
		error_value &= ~0x02;
	if(Switchoff_Lock || Switchoff_Lock1)
	  error_value |= 0x04;
	else
		error_value &= ~0x04;
	if(Switchon_Lock || Switchon_Lock1 || Switchon_Lock2)
	  error_value |= 0x08;
	else
		error_value &= ~0x08;
	InsertDataToQueue(&QueueTX5,BufferTX5,error_value);
	sum+=error_value;
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

void Control_Select(u8 addr,u8 state)
{
	u8 sum=0xED;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
	sum+=0x01;
	InsertDataToQueue(&QueueTX5,BufferTX5,0xDD);
	sum+=0xDD;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x05);
	sum+=0x05;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,addr);
	sum+=addr;
	InsertDataToQueue(&QueueTX5,BufferTX5,state);
	sum+=state;
	InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
	sum+=0x00;
	InsertDataToQueue(&QueueTX5,BufferTX5,sum);
	InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
}

void Record_SOEPad_Send (void)
{
	u8 sum=0xED,soedat[300];
	u16 i,soenum;
	soenum = GetQueueDataNum(&QueueSOE_Pad);
	if((soenum%10)==0 && soenum>9 && soenum<301)
	{
	  for(i=0;i<soenum;i++)
	    soedat[i] = GetDataFromQueue(&QueueSOE_Pad,BufferSOE_Pad);
		InsertDataToQueue(&QueueTX5,BufferTX5,0xFE);
		InsertDataToQueue(&QueueTX5,BufferTX5,0xEF);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		sum+=0x01;
		InsertDataToQueue(&QueueTX5,BufferTX5,0xAA);
		sum+=0xAA;
		if(soenum<254)
		{
		  InsertDataToQueue(&QueueTX5,BufferTX5,soenum+2);
		  sum+=(soenum+2);
		  InsertDataToQueue(&QueueTX5,BufferTX5,0x00);
		  sum+=0x00;
		}
		else
		{
			InsertDataToQueue(&QueueTX5,BufferTX5,soenum-253);
		  sum+=(soenum-253);
		  InsertDataToQueue(&QueueTX5,BufferTX5,0x01);
		  sum+=0x01;
		}
		for(i=0;i<soenum;i++)
		{
			InsertDataToQueue(&QueueTX5,BufferTX5,soedat[i]);
			sum+=soedat[i];
		}
		InsertDataToQueue(&QueueTX5,BufferTX5,sum&0xff);
		InsertDataToQueue(&QueueTX5,BufferTX5,0x16);
	}
	else if(soenum>0)
	{
		for(i=0;i<(soenum%10);i++)
		  GetDataFromQueue(&QueueSOE_Pad,BufferSOE_Pad);					
	}
	soenum = GetQueueDataNum(&QueueSOE_Pad);
	if(soenum == 0)
	  SOEPad_Flag = 0;
}

/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolve5(u8 receivedata)
*
*��    �ܣ�����ͨѶЭ�����
*
*��ڲ���������5����
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolve5(u8 receivedata)
{
	static u8 lengthL = 0;
	static u16 lengthcnt = 0;
	u16 number;
	switch(ProtocolResolveState)
	{
		case 0:
			if(receivedata == 0xFE)			// �ж�����ͷ
			{
				if(Usart5RxReady == 0)
					channel = 0;
				else if(Usart5bakRxReady == 0)
					channel = 1;
				ProtocolResolveState = 1;
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
			}
			break;
		case 2:
			if(receivedata == 0x01)			// ȷ���豸����
			{
				ProtocolResolveState = 3;	
				sum += receivedata;
			}
			else
			{
				ProtocolResolveState = 0;
			}
			break;
		case 3:
		  if(channel == 0)
				BackstageProtocol5.DeviceNum = receivedata;			// ���յ����豸��ַ
			else
				BackstageProtocol5bak.DeviceNum = receivedata;			// ���յ����豸��ַ
			sum += receivedata;
			ProtocolResolveState = 4;
			break;
		case 4:								// ����
			if(channel == 0)
				BackstageProtocol5.command = receivedata;
			else
				BackstageProtocol5bak.command = receivedata;
			if(receivedata == 0xEE || receivedata == 0xBB || receivedata == 0xEC || receivedata == 0xBC)
			{
			 	ProtocolResolveState = 5;	
			}
			else if(receivedata == 0xCA || receivedata == 0xCB || receivedata == 0xCC || receivedata == 0xCD || receivedata == 0xA5 ||
      				receivedata == 0xA6 || receivedata == 0xA7 || receivedata == 0xA8 || receivedata == 0xA9 || receivedata == 0xAD ||
              receivedata == 0xAE || receivedata == 0xAF || receivedata == 0xE2 || receivedata == 0xD5 || receivedata == 0xD6 || 
			        receivedata == 0xD7)
			{
			 	ProtocolResolveState = 6;	
			}
			else if(receivedata == 0xFF)
				ProtocolResolveState = 7;	
			else if(receivedata == 0xDD || receivedata == 0xEB || receivedata == 0xBE || receivedata == 0xED || receivedata == 0xBD)
				ProtocolResolveState = 8;
      else			
				ProtocolResolveState = 0;
			sum += receivedata;
			break;
		case 5:
			lengthL = receivedata;			// ���ݳ��ȵ��ֽ�
			sum += receivedata;
			ProtocolResolveState = 9;
			break;
		case 6:
			lengthL = receivedata;			// ���ݳ��ȵ��ֽ�
			sum += receivedata;
			ProtocolResolveState = 10;
			break;
		case 7:
			lengthL = receivedata;			// ���ݳ��ȵ��ֽ�
			sum += receivedata;
			ProtocolResolveState = 11;
			break;
		case 8:
			lengthL = receivedata;			// ���ݳ��ȵ��ֽ�
			sum += receivedata;
			ProtocolResolveState = 12;
			break;
		case 9:								// �������ݳ���
			if(channel == 0)
			{
				BackstageProtocol5.length = ((u16)receivedata<<8)+(u16)lengthL;
				if(BackstageProtocol5.length >4 && BackstageProtocol5.length <605)
				  ProtocolResolveState = 13;
				else
					ProtocolResolveState = 0;
			}
			else
			{
				BackstageProtocol5bak.length = ((u16)receivedata<<8)+(u16)lengthL;
				if(BackstageProtocol5bak.length >4 && BackstageProtocol5bak.length <605)	
				  ProtocolResolveState = 13;
				else 
					ProtocolResolveState = 0;
			}	
			sum += receivedata;
			lengthcnt = 0;
			break;
		case 10:								// �������ݳ���
			if(channel == 0)
			{
				BackstageProtocol5.length = ((u16)receivedata<<8)+(u16)lengthL;
				if(BackstageProtocol5.length ==(sizeof(struct SYSTEM_PARA)+2) || BackstageProtocol5.length == (sizeof(struct PROTECT_PARA)+2)  || 
					 BackstageProtocol5.length ==(sizeof(struct ALARM_PARA)+2)  || BackstageProtocol5.length == (sizeof(struct AUTO_SWITCHOFF)+2)||
					 BackstageProtocol5.length ==(sizeof(struct Rule101Para)+2) || BackstageProtocol5.length ==3 || BackstageProtocol5.length ==9|| 
					 BackstageProtocol5.length ==18)
				  ProtocolResolveState = 14;
				else
					ProtocolResolveState = 0;
			}
			else
			{
				BackstageProtocol5bak.length = ((u16)receivedata<<8)+(u16)lengthL;
				if(BackstageProtocol5bak.length ==(sizeof(struct SYSTEM_PARA)+2) || BackstageProtocol5bak.length == (sizeof(struct PROTECT_PARA)+2)     || 
					 BackstageProtocol5bak.length ==(sizeof(struct ALARM_PARA)+2)  ||  BackstageProtocol5bak.length ==(sizeof(struct AUTO_SWITCHOFF)+2)   || 
					 BackstageProtocol5bak.length ==(sizeof(struct Rule101Para)+2) || BackstageProtocol5bak.length ==3 || BackstageProtocol5bak.length ==9|| 
					 BackstageProtocol5bak.length ==18)
				  ProtocolResolveState = 14;
				else 
					ProtocolResolveState = 0;
			}	
			sum += receivedata;
			lengthcnt = 0;
			break;
		case 11:								// �������ݳ���
			if(channel == 0)
			{
				BackstageProtocol5.length = ((u16)receivedata<<8)+(u16)lengthL;
				if(BackstageProtocol5.length == 11)
				  ProtocolResolveState = 15;
				else 
					ProtocolResolveState = 0;
			}
			else
			{
				BackstageProtocol5bak.length = ((u16)receivedata<<8)+(u16)lengthL;
				if(BackstageProtocol5bak.length == 11)
				  ProtocolResolveState = 15;
				else 
					ProtocolResolveState = 0;
			}	
			sum += receivedata;
			lengthcnt = 0;
			break;
		case 12:								// �������ݳ���
			if(channel == 0)
			{
				BackstageProtocol5.length = ((u16)receivedata<<8)+(u16)lengthL;
				if(BackstageProtocol5.length == 5 || BackstageProtocol5.length ==6)
				  ProtocolResolveState = 16;
				else 
					ProtocolResolveState = 0;
			}
			else
			{
				BackstageProtocol5bak.length = ((u16)receivedata<<8)+(u16)lengthL;
				if(BackstageProtocol5bak.length == 5 || BackstageProtocol5bak.length ==6)
				  ProtocolResolveState = 16;
				else 
					ProtocolResolveState = 0;
			}	
			sum += receivedata;
			lengthcnt = 0;
			break;	
		case 13:								// ��������
			if(channel == 0)
			{
				BackstageProtocol5.DataBuffer[lengthcnt] = receivedata;
			}
			else
			{
				BackstageProtocol5bak.DataBuffer[lengthcnt] = receivedata;
			}
			sum += receivedata;
			ProtocolResolveState=13;
			lengthcnt++;
			if(channel == 0)
			{
				if((lengthcnt+2) == BackstageProtocol5.length)		// ���ݽ��ս���
					ProtocolResolveState = 18;
			}
			else
			{
				if((lengthcnt+2) == BackstageProtocol5bak.length)		// ���ݽ��ս���
					ProtocolResolveState = 18;
			}
			break;
		case 14:								// ��������
			if(channel == 0)
			{
				BackstageProtocol5.DataBuffer[lengthcnt] = receivedata;
			}
			else
			{
				BackstageProtocol5bak.DataBuffer[lengthcnt] = receivedata;
			}
			sum += receivedata;
			ProtocolResolveState=14;
			lengthcnt++;
			if(channel == 0)
			{
				if((lengthcnt+2) == BackstageProtocol5.length)		// ���ݽ��ս���
					ProtocolResolveState = 17;
			}
			else
			{
				if((lengthcnt+2) == BackstageProtocol5bak.length)		// ���ݽ��ս���
					ProtocolResolveState = 17;
			}
			break;
		case 15:								// ��������
			if(channel == 0)
			{
				BackstageProtocol5.DataBuffer[lengthcnt] = receivedata;
			}
			else
			{
				BackstageProtocol5bak.DataBuffer[lengthcnt] = receivedata;
			}
			sum += receivedata;
			ProtocolResolveState=15;
			lengthcnt++;
			if(lengthcnt == 9)		// ���ݽ��ս���
				ProtocolResolveState = 17;
			break;
		case 16:								// ��������
			if(channel == 0)
			{
				BackstageProtocol5.DataBuffer[lengthcnt] = receivedata;
			}
			else
			{
				BackstageProtocol5bak.DataBuffer[lengthcnt] = receivedata;
			}
			sum += receivedata;
			ProtocolResolveState=16;
			lengthcnt++;
			if(channel == 0)
			{
				if((lengthcnt+2) == BackstageProtocol5.length)		// ���ݽ��ս���
					ProtocolResolveState = 17;
			}
			else
			{
				if((lengthcnt+2) == BackstageProtocol5bak.length)		// ���ݽ��ս���
					ProtocolResolveState = 17;
			}
			break;
		case 17:
			if((sum&0x00ff) == receivedata)	// ��У��
				ProtocolResolveState = 19;
			else							// ��У��ʧ��
 				ProtocolResolveState = 0;
			break;
		case 18:
			if((sum&0x00ff) == receivedata)	// ��У��
				ProtocolResolveState = 19;
			else							// ��У��ʧ��
			{
 				ProtocolResolveState = 0;
				number = BackstageProtocol5.DataBuffer[0]|(BackstageProtocol5.DataBuffer[1]<<8);
				WifiCIPSENDEX(12);
		    os_dly_wait (30);
				if(channel == 0)
			  {
					if(BackstageProtocol5.command == 0xEE)
			      Program_Update_Send(number,0xA5);
					else if(BackstageProtocol5.command == 0xEC) 
					  Programbak_Update_Send(number,0xA5);
				}
				else 
				{
					if(BackstageProtocol5bak.command == 0xEE)
			      Program_Update_Send(number,0xA5);
					else if(BackstageProtocol5bak.command == 0xEC)
			      Programbak_Update_Send(number,0xA5);
				}
				WifiSendStr("\\0");
			}
			break;
		case 19:
			if(0x16 == receivedata)	//��У��
			{
				if(channel == 0)
				  Usart5RxReady=1;
				else
				  Usart5bakRxReady=1;
				isr_evt_set (0x0010, t_Task4);
			}
 			ProtocolResolveState = 0;
			break;
		default:
			break;
	}
	return 0;
}

void CommandProcess5Prepare(void)
{
	u16 i = 0;
	BackstageProtocol5.command = BackstageProtocol5bak.command;			
	BackstageProtocol5.length = BackstageProtocol5bak.length;				
	BackstageProtocol5.DeviceNum = BackstageProtocol5bak.DeviceNum;	
	for(i=0;i<BackstageProtocol5bak.length-2;i++)
		BackstageProtocol5.DataBuffer[i] = BackstageProtocol5bak.DataBuffer[i];	
}

void CommandProcess5(void)
{
  u16 i = 0,number = 0;
	//u8 frame=0;
	static u8 YaBan1 = 0;
	static u8 YaBan2 = 0;
	static u8 YaBan3 = 0;
	Send_Flag = 0;
	switch(BackstageProtocol5.command)
	{
		case 0xFF:	
			switch(BackstageProtocol5.DataBuffer[0])
			{
				case 0x00:
					WifiCIPSENDEX(271);
					os_dly_wait(30);
					Realdata_send();
					WifiSendStr("\\0");
					break;
				case 0x01:
					WifiCIPSENDEX(83);
					os_dly_wait(30);
					Systemdata_send();
					WifiSendStr("\\0");
					break;
				case 0x02:
					WifiCIPSENDEX(135);
					os_dly_wait(30);
					Protectdata_send();
					WifiSendStr("\\0");
					break;
				case 0x03:
					WifiCIPSENDEX(55);
					os_dly_wait(30);
					Alarmdata_send();
					WifiSendStr("\\0");
					break;
				case 0x04:
					WifiCIPSENDEX(109);
					os_dly_wait(30);
					number = BackstageProtocol5.DataBuffer[1]|(BackstageProtocol5.DataBuffer[2]<<8);
					Eventdata_send(number);
					WifiSendStr("\\0");
					break;
//				case 0x05:
//					WifiCIPSENDEX(412);
//					os_dly_wait(30);
//					number = BackstageProtocol5.DataBuffer[1]|(BackstageProtocol5.DataBuffer[2]<<8);
//					frame = BackstageProtocol5.DataBuffer[3];
//					Curvedata_send(number,frame);
//					WifiSendStr("\\0");
//					break;
				case 0x06:
					WifiCIPSENDEX(32);  //GPS���ݷ���
					os_dly_wait(30);
					GPS_DataSend();
					WifiSendStr("\\0");
			    break;
				case 0x07:
					WifiCIPSENDEX(35);
					os_dly_wait(30);
					SelfCheck_Send();   //�Լ����ݷ���
					WifiSendStr("\\0");
			    break;
				case 0x08:
					WifiCIPSENDEX(16);
					os_dly_wait(30);
					YABAN_Send();       //ѹ�����ݷ���
					WifiSendStr("\\0");
			    break;
				case 0x09:
					while(SOEPad_Flag)
					{
						WifiCIPSENDEX(309);
						os_dly_wait(30);
						Record_SOEPad_Send();
						WifiSendStr("\\0");
						os_dly_wait(30);
					}
					break;
				case 0x0A:
					WifiCIPSENDEX(sizeof(struct AUTO_SWITCHOFF)+9);
					os_dly_wait(30);
					AutoSwitchdata_send();
					WifiSendStr("\\0");
					break;
				case 0x0B:
					WifiCIPSENDEX(137);
					os_dly_wait(30);
					Battery_Selfcheck_datasend();
					WifiSendStr("\\0");
					break;
				case 0x0C:
					WifiCIPSENDEX(sizeof(struct Rule101Para)+9);
					os_dly_wait(30);
					Measure101Para_datasend();
					WifiSendStr("\\0");
					break;
				default:
					break;				
			}
			break;
		case 0xCA:		//ϵͳ��������
			#ifdef SIM800C_MODEL
			if((SystemSet.para.IP1[0] != (BackstageProtocol5.DataBuffer[52]|(BackstageProtocol5.DataBuffer[53]<<8))) |
				 (SystemSet.para.IP1[1] != (BackstageProtocol5.DataBuffer[54]|(BackstageProtocol5.DataBuffer[55]<<8))) |
			   (SystemSet.para.port1  != (BackstageProtocol5.DataBuffer[48]|(BackstageProtocol5.DataBuffer[49]<<8))))
			{
        GprsWdg = 400;
				IPswitchFlag = 0;
			}
			else if((SystemSet.para.IP2[0] != (BackstageProtocol5.DataBuffer[56]|(BackstageProtocol5.DataBuffer[57]<<8))) |
				      (SystemSet.para.IP2[1] != (BackstageProtocol5.DataBuffer[58]|(BackstageProtocol5.DataBuffer[59]<<8))) |
			        (SystemSet.para.port2  != (BackstageProtocol5.DataBuffer[50]|(BackstageProtocol5.DataBuffer[51]<<8))))
			{
        GprsWdg = 400;
				IPswitchFlag = 1;
			}
			else if(SystemSet.para.address != (BackstageProtocol5.DataBuffer[46]|(BackstageProtocol5.DataBuffer[47]<<8)))
			{
				GprsWdg = 400;
				if(IPswitchFlag == 0)
				{
					IPswitchFlag = 1;
				}
				else
				{
					IPswitchFlag = 0;
				}
			}
			#endif
			for(i=0;i<((sizeof(struct SYSTEM_PARA))/2);i++)
			{	
				SystemSet.word[i]=BackstageProtocol5.DataBuffer[2*i]|(BackstageProtocol5.DataBuffer[2*i+1]<<8);
			}
			SetSystemFlag=1;
			os_dly_wait(1000);
			WifiCIPSENDEX(10);
			os_dly_wait(30);
			HeartJump(0xA5,0x01);
			WifiSendStr("\\0");
			break;
		case 0xCB:		//������������
			for(i=0;i<((sizeof(struct PROTECT_PARA))/2);i++)
			{	
				ProtectSet.word[i]=BackstageProtocol5.DataBuffer[2*i]|(BackstageProtocol5.DataBuffer[2*i+1]<<8);
			}
			SetProtectFlag=1;
			os_dly_wait(1000);
			WifiCIPSENDEX(10);
			os_dly_wait(30);
			HeartJump(0xA5,0x02);
			WifiSendStr("\\0");
			break;
		case 0xCC:		//������������
			for(i=0;i<((sizeof(struct ALARM_PARA))/2);i++)
			{	
				AlarmSet.word[i]=BackstageProtocol5.DataBuffer[2*i]|(BackstageProtocol5.DataBuffer[2*i+1]<<8);
			}
			SetAlarmFlag=1;
			os_dly_wait(1000);
			WifiCIPSENDEX(10);
			os_dly_wait(30);
			HeartJump(0xA5,0x03);
			WifiSendStr("\\0");
			break;
		case 0xCD:		//���ƽ��в�������
			for(i=0;i<((sizeof(struct AUTO_SWITCHOFF))/2);i++)
			{	
				AutoswitchSet.word[i]=BackstageProtocol5.DataBuffer[2*i]|(BackstageProtocol5.DataBuffer[2*i+1]<<8);
			}
			SetAutoswitchFlag=1;
			os_dly_wait(1000);
			WifiCIPSENDEX(10);
			os_dly_wait(30);
			HeartJump(0xA5,0x04);
			WifiSendStr("\\0");
			break;
		case 0xDD:		//��������
			if(BackstageProtocol5.DataBuffer[0]==0x00 && BackstageProtocol5.DataBuffer[1]==0x00)
			{
				WifiSelectAddr=0;
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x01 && BackstageProtocol5.DataBuffer[1]==0x01)
			{
				if(LOCAL_IN7 == 0)
				{
				  WifiSelectAddr=1;
				  WifiCIPSENDEX(12);
				  os_dly_wait(30);
					if(ControlLock == 0)
						Control_Select(0x01,0x05);
					else
				    Control_Select(0x01,0x01);
				  WifiSendStr("\\0");
				}
				else if(FAR_IN6 == 0)
				{
				  WifiSelectAddr=0;
				  WifiCIPSENDEX(12);
				  os_dly_wait(30);
				  Control_Select(0x01,0x04);
				  WifiSendStr("\\0");
				}
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x02 && BackstageProtocol5.DataBuffer[1]==0x02)
			{
				if(LOCAL_IN7 == 0)
				{
				  WifiSelectAddr=2;
				  WifiCIPSENDEX(12);
				  os_dly_wait(30);
					if(ControlLock == 0)
						Control_Select(0x02,0x05);
					else
				    Control_Select(0x02,0x02);
				  WifiSendStr("\\0");
				}
				else if(FAR_IN6 == 0)
				{
					WifiSelectAddr=0;
				  WifiCIPSENDEX(12);
				  os_dly_wait(30);
				  Control_Select(0x02,0x04);
				  WifiSendStr("\\0");
				}
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x05 && BackstageProtocol5.DataBuffer[1]==0x03)
			{
				  WifiSelectAddr=3;
				  WifiCIPSENDEX(12);
				  os_dly_wait(30);
	        if(ControlLock == 0)
						Control_Select(0x05,0x05);
					else
				    Control_Select(0x05,0x03);
				  WifiSendStr("\\0");
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x01 && BackstageProtocol5.DataBuffer[1]==0x11)
			{
				if(WifiSelectAddr==1 && LOCAL_IN7 == 0)
				{
					os_dly_wait(8);
					if(LOCAL_IN7 == 0 && ControlLock == 0x11)
					{
						if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
						{
							RecordSwitchOn(TYPE_ACT_PAD_ON);// ������բ��¼
							SwitchFlag = 1;
						}
					}
					WifiSelectAddr=0;
				}
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x02 && BackstageProtocol5.DataBuffer[1]==0x12)
			{
				if(WifiSelectAddr==2 && LOCAL_IN7 == 0)
				{
					os_dly_wait(8);
					if(LOCAL_IN7 == 0 && ControlLock == 0x11)
					{
						if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
						{
							RecordSwitchOff(TYPE_ACT_PAD_OFF);// ������բ��¼
							SwitchFlag = 2;
						}
					}
					WifiSelectAddr=0;
				}
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x05 && BackstageProtocol5.DataBuffer[1]==0x13)
			{
				if(WifiSelectAddr==3)
				{
					SigresetFlag=1;
					WifiSelectAddr=0;
				}
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x03 && BackstageProtocol5.DataBuffer[1]==0x61) //��ػ��
			{
				BATTERY_CHARGE_OFF;
        KZQMeasureData.para.AlarmFlag.bit.bat_active = 1;	
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x03 && BackstageProtocol5.DataBuffer[1]==0x62)  //��ػ��
			{
			  BATTERY_CHARGE_ON;
			  KZQMeasureData.para.AlarmFlag.bit.bat_active = 0;	
			}
			else if(BackstageProtocol5.DataBuffer[0]==0x06)
			{
				ClearTJXX_Flag = 1;
			}
			
			if(BackstageProtocol5.DataBuffer[0]==10)
			{
        if(BackstageProtocol5.DataBuffer[1] == 0x21)
				{
					YaBan1 = 1;
					if(MeasureData.YaBan1 != YaBan1)
					{
						MeasureData.YaBan1 = 1;
						YabanSave_Flag = 1;
					}
				}
				else if(BackstageProtocol5.DataBuffer[1] == 0x22)
				{
					YaBan1 = 0;
					if(MeasureData.YaBan1 != YaBan1)
					{
						MeasureData.YaBan1 = 0;
						YabanSave_Flag = 1;
					}
				}
			}
			else if(BackstageProtocol5.DataBuffer[0]==11)
			{
        if(BackstageProtocol5.DataBuffer[1] == 0x31)
				{
					YaBan2 = 1;
					if(MeasureData.YaBan2 != YaBan2)
					{
						MeasureData.YaBan2 = 1;
						YabanSave_Flag = 1;
					}
				}
				else if(BackstageProtocol5.DataBuffer[1] == 0x32)
				{
					YaBan2 = 0;
					if(MeasureData.YaBan2 != YaBan2)
					{
						MeasureData.YaBan2 = 0;
						YabanSave_Flag = 1;
					}
				}
			}
			else if(BackstageProtocol5.DataBuffer[0]==12)
			{
        if(BackstageProtocol5.DataBuffer[1] == 0x41)
				{
					YaBan3 = 1;
					if(MeasureData.YaBan3 != YaBan3)
					{
						MeasureData.YaBan3 = 1;
						YabanSave_Flag = 1;
					}
				}
				else if(BackstageProtocol5.DataBuffer[1] == 0x42)
				{
					YaBan3 = 0;
					if(MeasureData.YaBan3 != YaBan3)
					{
						MeasureData.YaBan3 = 0;
						YabanSave_Flag = 1;
					}
				}
			}
			break;
		case 0xEE:		//KZQ�����������֡
			WifiCIPSENDEX(12);
			os_dly_wait(30);
			number = BackstageProtocol5.DataBuffer[0]|(BackstageProtocol5.DataBuffer[1]<<8);
			Program_Update_Send(number,0x5A);
			WifiSendStr("\\0");
			break;
		case 0xEB:		//KZQ������½���֡
			WifiCIPSENDEX(12);
			os_dly_wait(100);
			Program_Updata_Lastframe();
			break;
		case 0xEC:		//KZQ���ݳ����������֡
			WifiCIPSENDEX(12);
			os_dly_wait(30);
			number = BackstageProtocol5.DataBuffer[0]|(BackstageProtocol5.DataBuffer[1]<<8);
			Programbak_Update_Send(number,0x5A);
			WifiSendStr("\\0");
			break;
		case 0xED:		//KZQ���ݳ�����½���֡
			WifiCIPSENDEX(12);
			os_dly_wait(30);
			Programbak_Updata_Lastframe();
			break;
		case 0xA6:    //������ΨһID����
			WifiCIPSENDEX(13);
			os_dly_wait(30);
			RandID_Send();
			WifiSendStr("\\0");
		  break;
		case 0xA7:    //��ʪ�ȷ���
			WifiCIPSENDEX(13);
			os_dly_wait(30);
			Humi_Temp_value();
			WifiSendStr("\\0");
		  break;
		#ifdef SIM800C_MODEL
		case 0xA8:    //����ģʽ
			WifiCIPSENDEX(2048);
			os_dly_wait(30);
		  GprsWdg = 400;
      Start_Debug = 1;
		  break;
		case 0xA9:    //��������ģʽ
			WifiSendStr("\\0");
			os_dly_wait(30);
      Start_Debug = 0;
		  break;
		#endif
		case 0xAD:    //������Ϣ����
			MyVersion.Product_number =  BackstageProtocol5.DataBuffer[0]|(BackstageProtocol5.DataBuffer[1]<<8);
		  MyVersion.Product_flow =    BackstageProtocol5.DataBuffer[2]|(BackstageProtocol5.DataBuffer[3]<<8);
		  MyVersion.Product_date[0] = BackstageProtocol5.DataBuffer[4];
		  MyVersion.Product_date[1] = BackstageProtocol5.DataBuffer[5];
		  MyVersion.Product_date[2] = BackstageProtocol5.DataBuffer[6];
		  FLASH_Unlock();                 /*����flash**/
		  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                      FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
		  FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);	    //0x080E0000-0x080FFFFF   FLASH_Sector_11 128KB
		  /*������ϣ���ʼд��*/
		  FLASH_ProgramHalfWord(0x080E0000,MyVersion.Product_number);
		  FLASH_ProgramHalfWord(0x080E0002,MyVersion.Product_flow);
		  FLASH_ProgramByte(0x080E0004,MyVersion.Product_date[0]);
		  FLASH_ProgramByte(0x080E0005,MyVersion.Product_date[1]);
		  FLASH_ProgramByte(0x080E0006,MyVersion.Product_date[2]);
		  FLASH_Lock();  
		
			WifiCIPSENDEX(10);
			os_dly_wait(30);
			HeartJump(0xA5,0x06);
			WifiSendStr("\\0");
			break;
		case 0xAE:    //�汾��Ϣ����
			WifiCIPSENDEX(62);
			os_dly_wait(30);
			VersionInformation();
			WifiSendStr("\\0");
			break;
    case 0xAF:
			WifiCIPSENDEX(10);
			os_dly_wait(30);
			HeartJump(0xAF,KZQUpdataSucced);
			WifiSendStr("\\0");
			break;		
    case 0xD5:		//��Ȩ�����
//			for(i=0;i<16;i++)
//			{	
//				if(AUTHORIZATIONcode[i]!=BackstageProtocol5.DataBuffer[i])
//				  break;	
//			}
//			if(i==16)
		
		  if((BackstageProtocol5.DataBuffer[0]|BackstageProtocol5.DataBuffer[1]<<8) == SystemSet.para.password)
      {
				ControlLock = 0x11;
				WifiCIPSENDEX(10);
			  os_dly_wait(30);
			  HeartJump(0xD5,0x5D);
			  WifiSendStr("\\0");
			}
			else
      {
				ControlLock = 0;
				WifiCIPSENDEX(10);
			  os_dly_wait(30);
			  HeartJump(0xD5,0x55);
			  WifiSendStr("\\0");
			}
			break;	
    case 0xD6:		//��ά�����
				ControlLock = 0x11;
		    CodeTimeout = BackstageProtocol5.DataBuffer[14] | (BackstageProtocol5.DataBuffer[15]<<8); //��Ȩ����
				WifiCIPSENDEX(10);
			  os_dly_wait(30);
			  HeartJump(0xD6,0x6D);
			  WifiSendStr("\\0");
			break;
    case 0xD7:		//101��Լ��������
			for(i=0;i<(sizeof(struct Rule101Para));i++)
			{	
				Measure101Para.word[i]=BackstageProtocol5.DataBuffer[i];
			}
			Set101ParaFlag=1;
			os_dly_wait(1000);
			WifiCIPSENDEX(10);
			os_dly_wait(30);
			HeartJump(0xA5,0x05);
			WifiSendStr("\\0");
			break;		
		default:
			break;		
	}
}

/****************************************************************************
*
*��    �ƣ�void WifiSendByte(u8 byte)
*
*��    �ܣ�Wifiģ�鷢��һ���ֽ�
*
*��ڲ�����һ���ֽ�����
*
*���ز�����
*
****************************************************************************/
void WifiSendByte(u8 byte)
{
	InsertDataToQueue(&QueueTX5,BufferTX5,byte);
}	 

u8 WifiConnectCtrl(void)
{ char number[10];
	WifiWdg++;		// ���Ź�����
	if(state_flag == 1)
	{
		if(WifiConnectState == 9)
      Send_Flag = 1;
		else
			Send_Flag = 0;
	}
	else
		Send_Flag = 0;
	if(WifiWdg>3600)
	{
		//WifiConnectState = 0;	 // ����X����û�з��ش������ݣ�ģ����������
		WifiWdg = 0;
	}
	// ������ʱ	
	if(WifiTimeout==0)
	{
		if(WifiDelay>0)
		{
			 WifiDelay--;
		}
		else
		{
			 WifiTimeout = 1;
		}	
	}
  //����״̬��ת
	switch(WifiConnectState)
	{
		case 0:		
			if(WifiTimeout == 1)
			{
        WIFI_OFF;
	      GPS_OFF;
				GpsOn_Flag = 0;
				USART5_Configuration(115200);
				WifiConnectState = 1;
				WifiDelay = 10; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
		case 1:		
			if(WifiTimeout == 1)
			{
        WIFI_ON;          //wifi��
				WifiConnectState = 2;
				WifiDelay = 3; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
    case 2:		
			if(WifiTimeout == 1)
			{
				WifiSendStr("AT+RST");
				WifiSendStr("\r\n");
				WifiConnectState = 3;
				WifiDelay = 3; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
		case 3:		
			if(WifiTimeout == 1)
			{
        WifiSendStr("AT+CWMODE_CUR=2");
				WifiSendStr("\r\n");
				WifiConnectState = 4;
				WifiDelay = 2; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
		case 4:		
			if(WifiTimeout == 1)
			{
				WifiSendStr("AT+RST");
				WifiSendStr("\r\n");
				WifiConnectState = 5;
				WifiDelay = 3; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
		case 5:		
			if(WifiTimeout == 1)
			{
				sprintf((char*)number,"%010u",Lock_IdCode);
			  WifiSendStr("AT+CWSAP_DEF=\"10kV_CAPOutdoor_");
				WifiSendStr(number);
				WifiSendStr("\",\"12345678\",8,3");
				WifiSendStr("\r\n");
				WifiConnectState = 6;
				WifiDelay = 2; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
		case 6:		
			if(WifiTimeout == 1)
			{
				WifiSendStr("AT+CIPMUX=1");
				WifiSendStr("\r\n");
				WifiConnectState = 7;
				WifiDelay = 2; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
		case 7:		
			if(WifiTimeout == 1)
			{
				WifiSendStr("AT+CIPSERVER=1,8080");
				WifiSendStr("\r\n");
				WifiConnectState = 8;
				WifiDelay = 2; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
		case 8:		
			if(WifiTimeout == 1)
			{
			  WifiSendStr("AT+CIFSR");
				WifiSendStr("\r\n");
				WifiConnectState = 9;
				WifiDelay = 2; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;
		case 9:	
      if(WifiTimeout == 1)
			{ 			
				WifiConnectState = 9;
				WifiDelay = 5; 	// ��ʱ
				WifiTimeout = 0;
			}
			break;				
		default:
			WifiConnectState = 0;
			break;
	}
	return 0;
}

/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolveWIFIError(u8 receivedata)
*
*��    �ܣ�WIFIͨѶ����Э�����
*
*��ڲ�������������
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolveWIFIError(u8 receivedata)
{
	static u8 ProtocolErrorState = 0;
	static u8 fail_num = 0;
	switch(ProtocolErrorState)
	{
		case 0:
			if(receivedata == 'S')
			{
				ProtocolErrorState = 1;
			}
			else if(receivedata == 'v')
			{
				ProtocolErrorState = 7;
			}
			else if(receivedata == '+')
			{
				ProtocolErrorState = 11;
			}
			else if(receivedata == 'O')
			{
				ProtocolErrorState = 16;
			}
			else if(receivedata == 'C')
			{
				ProtocolErrorState = 17;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 1:
			if(receivedata == 'E')
			{
				ProtocolErrorState = 2;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 2:
			if(receivedata == 'N')
			{
				ProtocolErrorState = 3;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 3:
			if(receivedata == 'D')
			{
				ProtocolErrorState = 4;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 4:
			if(receivedata == ' ')
			{
				ProtocolErrorState = 5;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 5:
			if(receivedata == 'O')
			{
				ProtocolErrorState = 6;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 6:
			if(receivedata == 'K')
			{
				ProtocolErrorState = 0;	
				WifiWdg = 0;
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 7:
			if(receivedata == 'a')
			{
				ProtocolErrorState = 8;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;	
		case 8:
			if(receivedata == 'l')
			{
				ProtocolErrorState = 9;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;	
		case 9:
			if(receivedata == 'i')
			{
				ProtocolErrorState = 10;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 10:
			if(receivedata == 'd')
			{
				fail_num++;
				if(fail_num > 2)
				{
					fail_num = 0;
					state_flag = 0;
				}
				WifiConnectState = 0;
				WifiSendStr("AT+CIPCLOSE=5");
				WifiSendStr("\r\n");
				WifiDelay = 2; 	// ��ʱ
				WifiTimeout = 0;
				ProtocolErrorState = 0;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
		case 11:
			if(receivedata == 'I')
			{
				ProtocolErrorState = 12;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
    case 12:
			if(receivedata == 'P')
			{
				ProtocolErrorState = 13;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
    case 13:
			if(receivedata == 'D')
			{
				ProtocolErrorState = 14;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;	
    case 14:
			if(receivedata == ',')
			{
				ProtocolErrorState = 15;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;
    case 15:
			state_flag = 1;
		  fail_num = 0;
			IDnumber =  receivedata; 
		  if(IDnumber!=IDnumberLast)
				ControlLock = 0;
		  IDnumberLast = IDnumber;
			ProtocolErrorState = 0;
			break;	
		case 16:
      if(receivedata == 'K')
			{
        KZQMeasureData.para.SelfCheck.bit.WIFI_state = 1;
			}
			ProtocolErrorState = 0;
			break;	
    case 17:
      if(receivedata == 'L')
			{
        ProtocolErrorState = 18;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;	
    case 18:
      if(receivedata == 'O')
			{
        ProtocolErrorState = 19;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;	
	  case 19:
      if(receivedata == 'S')
			{
        ProtocolErrorState = 20;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;	
		case 20:
      if(receivedata == 'E')
			{
        ProtocolErrorState = 21;	
			}
			else
			{
				ProtocolErrorState = 0;
			}
			break;	
		case 21:
      if(receivedata == 'D')
			{
        state_flag = 0;
				ProtocolErrorState = 0;
			}
			break;	
		default:
			ProtocolErrorState = 0;
			break;
	}
	return 0;
}

void Heart_Jump(void)
{
	WifiCIPSENDEX(2);
	os_dly_wait(30);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xAB);
	InsertDataToQueue(&QueueTX5,BufferTX5,0xBA);
	WifiSendStr("\\0");
}
