//###########################################################################
//
// GPS模块通讯口
//
//###########################################################################
// GPS模块通讯口
// $GPRMC( Recommended Minimum Specific GPS/TRANSIT Data)
// 格   式：  $GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh<CR><LF>
// $GPRMC,024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706,,,A*50
// 说  明：
// 字段 0 ：$GPRMC ，语句ID ，表明该语句为Recommended Minimum Specific GPS/TRANSIT Data （RMC ）推荐最小定位信息
// <1> UTC 时间，hhmmss(时分秒)格式 
// <2> 定位状态，A=有效定位，V=无效定位 
// <3>纬度ddmm.mmmm(度分)格式(前面的0也将被传输) 
// <4> 纬度半球N(北半球)或S(南半球) 
// <5>经度dddmm.mmmm(度分)格式(前面的0也将被传输) 
// <6> 经度半球E(东经)或W(西经) 
// <7>地面速率(000.0~999.9节，前面的0也将被传输) 
// <8>地面航向(000.0~359.9度，以真北为参考基准，前面的0也将被传输) 
// <9> UTC 日期，ddmmyy(日月年)格式 
// <10>磁偏角(000.0~180.0度，前面的0也将被传输) 
// <11> 磁偏角方向，E(东)或W(西) 
// <12>模式指示(仅NMEA01833.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效)
//
// $GPGLL( Geographic Position)
// 格   式：  $GPGLL,<1>,<2>,<3>,<4>,<5>,<6>*hh<CR><LF>
// $GPGLL,2236.91418,N,11403.24669,E,060556.00,A,D*64 
// 说  明：
// 字段 0 ：$GPGLL ，语句ID ，表明该语句为Geographic Position 
// <1>纬度ddmm.mmmm(度分)格式(前面的0也将被传输) 
// <2> 纬度半球N(北半球)或S(南半球) 
// <3>经度dddmm.mmmm(度分)格式(前面的0也将被传输) 
// <4> 经度半球E(东经)或W(西经) 
// <5> UTC 时间，hhmmss(时分秒)格式 
// <6> 定位状态，A=有效定位，V=无效定位
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  2018.11.16| 13 November 2018 | S.H.Q. |
//###########################################################################
#include "GPS.h"
#include "queue.h"

/* 结构体定义 ----------------------------------------------------------------*/
struct GPS_INFO Gps;
struct GPS_INFO GpsBak;

static u8 sum = 0;
static u8 length = 0;
static volatile u8 ProtocolResolveState = 0;	// 协议解析函数状态机
static u8 channel = 0;			// 数据通道
static u8 Gps_Type = 0;
/****************************************************************************
*
*名    称：str_to_int(const char *str)
*
*功    能：字符串转换成整型
*
*入口参数：字符
*
*返回参数：u32
*
****************************************************************************/
u32 str_to_int(u8 *str)
{
	int temp = 0;
	while(*str != 0)
	{
		if ((*str < '0') || (*str > '9'))  //如果当前字符不是数字
		{                       //则退出循环
			break;
		}
		temp = temp * 10 + (*str - '0'); //如果当前字符是数字则计算数值
		str++;      //移到下一个字符
	}   
	return temp;
}

/****************************************************************************
*
*名    称：str_to_double(u8 *str)
*
*功    能：字符串转换成浮点型
*
*入口参数：字符
*
*返回参数：double
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
		if ((*str >= '0') && (*str <= '9'))  //如果当前字符是数字则计算数值
		{
      if(flag == 0)
			  temp = temp * 10 + (*str - '0'); //整数部分
			else
			{
				times++;
				temp1 = (double)(*str - '0');
				for(i=0;i<times;i++)
				  temp1 /= 10;
				temp += temp1;//小数部分
			}
		}
		else if(*str != '.')
		{                       //则退出循环
			break;
		}
		str++;      //移到下一个字符
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
		if ((*str >= '0') && (*str <= '9'))  //如果当前字符是数字则计算数值
		{
      if(flag == 0)
			  temp = temp * 10 + (*str - '0'); //整数部分
			else
			{
				times++;
				temp1 = (float)(*str - '0');
				for(i=0;i<times;i++)
				  temp1 /= 10;
				temp += temp1;//小数部分
			}
		}
		else if(*str != '.')
		{                       //则退出循环
			break;
		}
		str++;      //移到下一个字符
	}   
	return temp;
}
/****************************************************************************
*
*名    称：u8 ProtocolResolve_Gps(u8 receivedata)
*
*功    能：串行通讯协议解析
*
*入口参数：GPS口数据
*
*返回参数：
*
****************************************************************************/
u8 ProtocolResolve_Gps(u8 receivedata)
{ 
	switch(ProtocolResolveState)
	{
		case 0:
			if(receivedata == '$' || receivedata == 'P')			// 判断数据头
			  ProtocolResolveState = 1;
			break;
		case 1:
			if(receivedata == 'G')			// 判断数据头
			{
			  ProtocolResolveState = 2;
			  sum = receivedata;        // sum进行和校验
			}
			else
				ProtocolResolveState = 0;
			break;
		case 2:
			if(receivedata == 'P' || receivedata == 'L')			// 判断数据头
			{
			  ProtocolResolveState = 3;
			  sum ^= receivedata;
			}
			else
				ProtocolResolveState = 0;
			break;
		case 3:
			if(receivedata == 'R')			// 判断数据头
			  ProtocolResolveState = 4;
			else if(receivedata == 'G')
				ProtocolResolveState = 6;
			else
				ProtocolResolveState = 0;
			sum ^= receivedata;
			break;
		case 4:
			if(receivedata == 'M')			// 判断数据头
			{
			  ProtocolResolveState = 5;
				sum ^= receivedata;
			}
			else
				ProtocolResolveState = 0;
			break;
	  case 5:
			if(receivedata == 'C')			// 判断数据头
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
			 if(((sum>>4)&0x0F) == (receivedata - '0'))	// 和校验
			 {
			   ProtocolResolveState = 11;
			 }
			 else 
				 ProtocolResolveState = 0;
		  break;
		case 11:
			 if((sum&0x0F) == (receivedata - '0'))	// 和校验
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
