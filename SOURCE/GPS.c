//###########################################################################
//
// GPSģ��ͨѶ��
//
//###########################################################################
// GPSģ��ͨѶ��
// $GPRMC( Recommended Minimum Specific GPS/TRANSIT Data)
// ��   ʽ��  $GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh<CR><LF>
// $GPRMC,024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706,,,A*50
// ˵  ����
// �ֶ� 0 ��$GPRMC �����ID �����������ΪRecommended Minimum Specific GPS/TRANSIT Data ��RMC ���Ƽ���С��λ��Ϣ
// <1> UTC ʱ�䣬hhmmss(ʱ����)��ʽ 
// <2> ��λ״̬��A=��Ч��λ��V=��Ч��λ 
// <3>γ��ddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������) 
// <4> γ�Ȱ���N(������)��S(�ϰ���) 
// <5>����dddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������) 
// <6> ���Ȱ���E(����)��W(����) 
// <7>��������(000.0~999.9�ڣ�ǰ���0Ҳ��������) 
// <8>���溽��(000.0~359.9�ȣ����汱Ϊ�ο���׼��ǰ���0Ҳ��������) 
// <9> UTC ���ڣ�ddmmyy(������)��ʽ 
// <10>��ƫ��(000.0~180.0�ȣ�ǰ���0Ҳ��������) 
// <11> ��ƫ�Ƿ���E(��)��W(��) 
// <12>ģʽָʾ(��NMEA01833.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч)
//
// $GPGLL( Geographic Position)
// ��   ʽ��  $GPGLL,<1>,<2>,<3>,<4>,<5>,<6>*hh<CR><LF>
// $GPGLL,2236.91418,N,11403.24669,E,060556.00,A,D*64 
// ˵  ����
// �ֶ� 0 ��$GPGLL �����ID �����������ΪGeographic Position 
// <1>γ��ddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������) 
// <2> γ�Ȱ���N(������)��S(�ϰ���) 
// <3>����dddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������) 
// <4> ���Ȱ���E(����)��W(����) 
// <5> UTC ʱ�䣬hhmmss(ʱ����)��ʽ 
// <6> ��λ״̬��A=��Ч��λ��V=��Ч��λ
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  2018.11.16| 13 November 2018 | S.H.Q. |
//###########################################################################
#include "GPS.h"
#include "queue.h"

/* �ṹ�嶨�� ----------------------------------------------------------------*/
struct GPS_INFO Gps;
struct GPS_INFO GpsBak;

static u8 sum = 0;
static u8 length = 0;
static volatile u8 ProtocolResolveState = 0;	// Э���������״̬��
static u8 channel = 0;			// ����ͨ��
static u8 Gps_Type = 0;
/****************************************************************************
*
*��    �ƣ�str_to_int(const char *str)
*
*��    �ܣ��ַ���ת��������
*
*��ڲ������ַ�
*
*���ز�����u32
*
****************************************************************************/
u32 str_to_int(u8 *str)
{
	int temp = 0;
	while(*str != 0)
	{
		if ((*str < '0') || (*str > '9'))  //�����ǰ�ַ���������
		{                       //���˳�ѭ��
			break;
		}
		temp = temp * 10 + (*str - '0'); //�����ǰ�ַ��������������ֵ
		str++;      //�Ƶ���һ���ַ�
	}   
	return temp;
}

/****************************************************************************
*
*��    �ƣ�str_to_double(u8 *str)
*
*��    �ܣ��ַ���ת���ɸ�����
*
*��ڲ������ַ�
*
*���ز�����double
*
****************************************************************************/
double str_to_double(u8 *str)
{
	double temp=0,temp1=0;
	u8 flag=0,times=0,i=0;
	while(*str != 0)
	{
		if(*str == '.')
			flag=1;
		if ((*str >= '0') && (*str <= '9'))  //�����ǰ�ַ��������������ֵ
		{
      if(flag == 0)
			  temp = temp * 10 + (*str - '0'); //��������
			else
			{
				times++;
				temp1 = (double)(*str - '0');
				for(i=0;i<times;i++)
				  temp1 /= 10;
				temp += temp1;//С������
			}
		}
		else if(*str != '.')
		{                       //���˳�ѭ��
			break;
		}
		str++;      //�Ƶ���һ���ַ�
	}   
	return temp;
}

float str_to_float(u8 *str)
{
	float temp=0,temp1=0;
	u8 flag=0,times=0,i=0;
	while(*str != 0)
	{
		if(*str == '.')
			flag=1;
		if ((*str >= '0') && (*str <= '9'))  //�����ǰ�ַ��������������ֵ
		{
      if(flag == 0)
			  temp = temp * 10 + (*str - '0'); //��������
			else
			{
				times++;
				temp1 = (float)(*str - '0');
				for(i=0;i<times;i++)
				  temp1 /= 10;
				temp += temp1;//С������
			}
		}
		else if(*str != '.')
		{                       //���˳�ѭ��
			break;
		}
		str++;      //�Ƶ���һ���ַ�
	}   
	return temp;
}
/****************************************************************************
*
*��    �ƣ�u8 ProtocolResolve_Gps(u8 receivedata)
*
*��    �ܣ�����ͨѶЭ�����
*
*��ڲ�����GPS������
*
*���ز�����
*
****************************************************************************/
u8 ProtocolResolve_Gps(u8 receivedata)
{ 
	switch(ProtocolResolveState)
	{
		case 0:
			if(receivedata == '$' || receivedata == 'P')			// �ж�����ͷ
			  ProtocolResolveState = 1;
			break;
		case 1:
			if(receivedata == 'G')			// �ж�����ͷ
			{
			  ProtocolResolveState = 2;
			  sum = receivedata;        // sum���к�У��
			}
			else
				ProtocolResolveState = 0;
			break;
		case 2:
			if(receivedata == 'P' || receivedata == 'L')			// �ж�����ͷ
			{
			  ProtocolResolveState = 3;
			  sum ^= receivedata;
			}
			else
				ProtocolResolveState = 0;
			break;
		case 3:
			if(receivedata == 'R')			// �ж�����ͷ
			  ProtocolResolveState = 4;
			else if(receivedata == 'G')
				ProtocolResolveState = 6;
			else
				ProtocolResolveState = 0;
			sum ^= receivedata;
			break;
		case 4:
			if(receivedata == 'M')			// �ж�����ͷ
			{
			  ProtocolResolveState = 5;
				sum ^= receivedata;
			}
			else
				ProtocolResolveState = 0;
			break;
	  case 5:
			if(receivedata == 'C')			// �ж�����ͷ
			{
			  ProtocolResolveState = 8;
			  Gps_Type = 1;
				sum ^= receivedata;
			}
			else
				ProtocolResolveState = 0;
			break;
		case 6:
			if(receivedata == 'L')
			{
				ProtocolResolveState = 7;
				sum ^= receivedata;
			}
			else
				ProtocolResolveState = 0;
		  break;
		case 7:
			if(receivedata == 'L')
			{
				ProtocolResolveState = 8;
			  Gps_Type = 2;
				KZQMeasureData.para.SelfCheck.bit.GPS_state = 1;
				sum ^= receivedata;
			}
			else
				ProtocolResolveState = 0;
		  break;
		case 8:
			if(receivedata == ',')			
			{
				if(Usart5RxReady == 0)
					channel = 0;
				else if(Usart5bakRxReady == 0)
					channel = 1;
				ProtocolResolveState = 9;
				sum ^= receivedata;
				length=0;
			}
			else
				ProtocolResolveState = 0;
			break;
		case 9:
			if(receivedata == '*')
				ProtocolResolveState = 10;
			else
			{
				if(channel == 0)
				{
					Gps.DataBuffer[length] = receivedata;
				}
				else
				{
					GpsBak.DataBuffer[length] = receivedata;
				}
		    sum ^= receivedata;
			  length++;
				ProtocolResolveState = 9;
			}
			break;
		case 10:
			 if(((sum>>4)&0x0F) == (receivedata - '0'))	// ��У��
			 {
			   ProtocolResolveState = 11;
			 }
			 else 
				 ProtocolResolveState = 0;
		  break;
		case 11:
			 if((sum&0x0F) == (receivedata - '0'))	// ��У��
			 {
				 if(channel == 0)
				   Usart5RxReady=1;
				 else
				   Usart5bakRxReady=1;
				 isr_evt_set (0x0040,t_Task4);
			 }
 			 ProtocolResolveState = 0;
		  break;
		default:
			break;
	}
	return 0;
}

void CommandProcess_GpsPrepare(void)
{
  u16 i = 0;
	for(i=0;i<length;i++)
		Gps.DataBuffer[i] = GpsBak.DataBuffer[i];	
}

void CommandProcess_Gps(void)
{
  u8 buf[20]={0};
	u8 *str;
	u8 temp = 0;
	u8 i = 0;
	str = Gps.DataBuffer;
	if(Gps_Type == 1)
	{
		while(*str != 0)
	  { 
			switch(temp)
			{
				case 0:
					if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						str++;
//						Gps.time = str_to_double(buf);		
					}
					if(i>9)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 1:
					if(*str != ',')
					{
//						Gps.state = *str;
						i++;
						str++;
					}
					else
					{
						i=0;
						temp++;	
						str++;
						if(Gps.state == 'A')
							Gps_Type = 3;
					}
					if(i>1)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 2:
					if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						str++;
//						Gps.longitude = str_to_double(buf);		
					}
					if(i>10)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 3:
					if(*str != ',')
					{
//						Gps.NS = *str;
						str++;
						i++;
					}
					else
					{
						i=0;
						temp++;	
						str++;
					}
					if(i>1)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 4:
					if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						str++;
//						Gps.latitude = str_to_double(buf);		
					}
					if(i>11)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 5:
					if(*str != ',')
					{
//						Gps.EW = *str;
						str++;
						i++;
					}
					else
					{
						i=0;
						temp++;	
						str++;
					}
					if(i>1)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 6:
					if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						str++;
						Gps.speed = str_to_float(buf);		
					}
					if(i>5)
					{
						i=0;
						temp=0;
						*str=0;
					}	
					break;
				case 7:
					if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						str++;
						Gps.direction = str_to_float(buf);		
					}
					if(i>5)
					{
						i=0;
						temp=0;
						*str=0;
					}	
					break;
				case 8:
					if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						Gps.date = str_to_int(buf);		
					}
					if(i>6)
					{
						i=0;
						temp=0;
						*str=0;
					}	
					break;
				default:
					i=0;
					*str = 0;
					temp = 0;
					break;
			}
		}
	}
	else if(Gps_Type == 2)
	{
		while(*str != 0)
	  { 
			switch(temp)
			{
				case 0:
					if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						str++;
						Gps.longitude = str_to_double(buf);		
					}
					if(i>10)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 1:
					if(*str != ',')
					{
						Gps.NS = *str;
						str++;
						i++;
					}
					else
					{
						i=0;
						temp++;	
						str++;
					}
					if(i>1)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 2:
          if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						str++;
						Gps.latitude = str_to_double(buf);		
					}
					if(i>11)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 3:
					if(*str != ',')
					{
						Gps.EW = *str;
						str++;
						i++;
					}
					else
					{
						i=0;
						temp++;	
						str++;
					}
					if(i>1)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 4:
					if(*str != ',')
						buf[i++] = *(str++);
					else
					{
						buf[i]=0;
						i=0;
						temp++;
						str++;
						Gps.time = str_to_double(buf);		
					}
					if(i>9)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				case 5:
					if(*str != ',')
					{
						Gps.state = *str;
						i++;
						str++;
					}
					else
					{
						i=0;
						temp++;	
						if(Gps.state == 'A')
							Gps_Type = 3;
					}
					if(i>1)
					{
						i=0;
						temp=0;
						*str=0;
					}
					break;
				default:
					i=0;
					*str = 0;
					temp = 0;
					break;
			}
		}
	}
}
