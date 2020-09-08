//###########################################################################
//
// 串口4通讯协议文件
// 线损模块通信
//
//注意：上位机为非平衡模式，通信前需将地址域，传送原因设为2个字节；将系统参数的地址设为1；
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  18.03| 13 mary 2018 | S.H.Q. |
//###########################################################################
#include "protocol4.h"
#include "protocol1.h"
#include "ds1302.h"
#include "queue.h"

#define Addr101_4                 2
#define Reason101_4               2
#define TotalLen10_4              Addr101_4+3//10帧长偏移量
#define FrontReason_4             Addr101_4-1//传送原因前
#define TotalLen68_4              Addr101_4*2+Reason101_4-3//68总帧长偏移个数

struct BACKSTAGE_PROTOCOL_TYPE4 BackstageProtocol4;
struct BACKSTAGE_PROTOCOL_TYPE4 BackstageProtocol4bak;

static volatile u8 ProtocolResolveState = 0;	// 协议解析函数状态机
static u8 channel = 0;			// 数据通道
static u16 gyjsq,RxCounter1=0;
static u8 datdone1=0;

/****************************************************************************
*
*名    称：u8 ProtocolResolve4(u8 receivedata)
*
*功    能：串行通讯协议解析
*
*入口参数：串口4数据
*
*返回参数：
*
****************************************************************************/
u8 ProtocolResolve4(u8 receivedata)
{
	switch(ProtocolResolveState)
	{
		case 0:  //接收到新的数据帧
			if(receivedata == 0x10)			// 判断数据头，主站向子站发送的启动字符
			{
				if(Usart4RxReady == 0)		//未接收完成
				{
					channel = 0;
					BackstageProtocol4.DataBuffer[0] = receivedata;//这个和bak两个像是一个用来实时处理，另一个像是备份，channel=0对应实时处理，channel=1对应备份
					RxCounter1=1;
					gyjsq=TotalLen10_4;								//还剩TotalLen10_4-1 个字符没有接收
					ProtocolResolveState = 1;//接收到一个字符
				}
				else if(Usart4bakRxReady == 0)
				{
					channel = 1;
					BackstageProtocol4bak.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=TotalLen10_4;
					ProtocolResolveState = 1;
				}
			}
			else if (receivedata == 0x68)//总召，主站向子站发送总召启动字符
			{
				if(Usart4RxReady == 0)
				{
					channel = 0;
					BackstageProtocol4.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=3;
					ProtocolResolveState = 2;
				}
				else if(Usart4bakRxReady == 0)
				{
					channel = 1;
					BackstageProtocol4bak.DataBuffer[0] = receivedata;
					RxCounter1=1;
					gyjsq=3;
					ProtocolResolveState = 2;
				}
			}
			else 
				ProtocolResolveState = 0;
			break;
		case 1://之前已经接收到数据0x10
      if(channel == 0)//存在BackstageProtocol4中的数据
			{
				gyjsq--;//剩余数量减1
				BackstageProtocol4.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)								//如果已经接收到第五个字符了
			  {
					if(BackstageProtocol4.DataBuffer[Addr101_4+2]==UsartAdd(BackstageProtocol4.DataBuffer,1,Addr101_4+1))//根据101规约，这里既第4个数据等于第2和第3个数据之和，第四个即校验位
					{
							datdone1=2;
							Usart4RxReady=1;
							isr_evt_set(0x0008,t_Task4);   							 //这个任务4专门用来处理这些协议，除了这个之外还有其他的一些协议
					}
					ProtocolResolveState = 0;
			  }
			}
			else
			{
				gyjsq--;
				BackstageProtocol4bak.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)								//	//判断
			  {
					if(BackstageProtocol4bak.DataBuffer[Addr101_4+2]==UsartAdd(BackstageProtocol4bak.DataBuffer,1,Addr101_4+1))
					{
							datdone1=2;
							Usart4bakRxReady=1;
							isr_evt_set(0x0008,t_Task4);   							 //这个任务4专门用来处理这些协议，除了这个之外还有其他的一些协议
					}
					ProtocolResolveState = 0;
			  }
			}
			break;
		case 2://另一种命令，之前已经接收到0x68，这种命令只有4个字符，另外一个有5个字符
			if(channel == 0)
			{
				gyjsq--;
				BackstageProtocol4.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)
				{
					if(BackstageProtocol4.DataBuffer[1]==BackstageProtocol4.DataBuffer[2]&&BackstageProtocol4.DataBuffer[3]==0x68)
					{																											//即第1和第4个数据都是0x68，第2和第3个数据相同，101规约的总召确实如此
						gyjsq=BackstageProtocol4.DataBuffer[1]+2;           //第2个数据表示命令之后将要发送的数据长度
						ProtocolResolveState = 3;						                //进入到第三种模式
					}
					else if(BackstageProtocol4.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol4.DataBuffer[1]+(BackstageProtocol4.DataBuffer[2]<<8)+2;
						ProtocolResolveState = 4;                          	
					}
					else
						ProtocolResolveState = 0;                           //这个可以认为是接收出现了错误
				}
			}
			else
			{
				gyjsq--;
				BackstageProtocol4bak.DataBuffer[RxCounter1++] = receivedata;
				if(gyjsq==0)
				{
					if(BackstageProtocol4bak.DataBuffer[1]==BackstageProtocol4bak.DataBuffer[2]&&BackstageProtocol4bak.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol4bak.DataBuffer[1]+2;
						ProtocolResolveState = 3;						
					}
					else if(BackstageProtocol4bak.DataBuffer[3]==0x68)
					{
						gyjsq=BackstageProtocol4bak.DataBuffer[1]+(BackstageProtocol4bak.DataBuffer[2]<<8)+2;
						ProtocolResolveState = 4;                          	
					}
					else
					ProtocolResolveState = 0;
			  }
			}
			break;
		case 3://模式2下延伸出来的第三种模式，准备接收总召命令后的数据
		  if(channel == 0) //所以这个用来判断是放在BackstageProtocol4中还是放在BackstageProtocol4bak中
			{
				gyjsq--;
				BackstageProtocol4.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol4.DataBuffer[RxCounter1-1]==0x16)//判断最后一个是否为0x16，这个是帧尾
				  {
						  if(BackstageProtocol4.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol4.DataBuffer,4,BackstageProtocol4.DataBuffer[1]))
						  {																								 //倒数第2个数据等于第5个到第13个数据之和，其实就是除了命令和帧尾以及校验位的所有数据之和等于校验位
								Usart4RxReady=1;
							  datdone1=1;	
								isr_evt_set(0x0008,t_Task4);   							 //这个任务4专门用来处理这些协议，除了这个之外还有其他的一些协议
						  }																								 //由此也可以看出来到这里才算完成，可以去真正的去处理这些数据，0x10这个命令更像是一种通信间的握手，0x68才是真正通信的开始
				  }																										 //下面的CommandProcess1（）函数应该就是处理这些数据的
				  ProtocolResolveState=0;
			  }	  
			}	
			else
			{
				gyjsq--;
				BackstageProtocol4bak.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol4bak.DataBuffer[RxCounter1-1]==0x16)
				  {
						if(BackstageProtocol4bak.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol4bak.DataBuffer,4,BackstageProtocol4bak.DataBuffer[1]))
						{
							Usart4bakRxReady=1;
							datdone1=1;	
							isr_evt_set(0x0008,t_Task4);
						}
				  }
				  ProtocolResolveState=0;
			  }	  
			}	
			break;
		case 4:
		  if(channel == 0) 
			{
				gyjsq--;
				BackstageProtocol4.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol4.DataBuffer[RxCounter1-1]==0x16)//判断最后一个是否为0x16，这个是帧尾
				  {
						  if(BackstageProtocol4.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol4.DataBuffer,4,BackstageProtocol4.DataBuffer[1]|(BackstageProtocol4.DataBuffer[2]<<8)))
						  {																								 //倒数第2个数据等于第5个到第13个数据之和，其实就是除了命令和帧尾以及校验位的所有数据之和等于校验位
								Usart4RxReady=1;
							  datdone1=1;	
								isr_evt_set(0x0008,t_Task4);   							 //这个任务4专门用来处理这些协议，除了这个之外还有其他的一些协议
						  }																								 //由此也可以看出来到这里才算完成，可以去真正的去处理这些数据，0x10这个命令更像是一种通信间的握手，0x68才是真正通信的开始
				  }																										 //下面的CommandProcess1（）函数应该就是处理这些数据的
				  ProtocolResolveState=0;
			  }	  
			}	
			else
			{
				gyjsq--;
				BackstageProtocol4bak.DataBuffer[RxCounter1++] = receivedata;
			  if(gyjsq==0)
			  {
				  if(BackstageProtocol4bak.DataBuffer[RxCounter1-1]==0x16)
				  {
						if(BackstageProtocol4bak.DataBuffer[RxCounter1-2]==UsartAdd(BackstageProtocol4bak.DataBuffer,4,BackstageProtocol4bak.DataBuffer[1]|(BackstageProtocol4bak.DataBuffer[2]<<8)))
						{
							Usart4bakRxReady=1;
							datdone1=1;	
							isr_evt_set(0x0008,t_Task4);
						}
				  }
				  ProtocolResolveState=0;
			  }	  
			}	
			break;
		default:
			ProtocolResolveState=0;
			break;
	}
	return 0x0D;
}

void CommandProcess4Prepare(void)
{
	u16 i = 0;	
	for(i=0;i<RxCounter1;i++)
		BackstageProtocol4.DataBuffer[i] = BackstageProtocol4bak.DataBuffer[i];	
}

void Read_Loss_SetDeal_4(u8 USARTxChnnel)
{
	u8 i,j=0;
	LockSetTotal=(BackstageProtocol4.DataBuffer[7+FrontReason_4]&0x7F);
	if(LockSetTotal == 0)
	{
		for(i=0;i<9;i++)
		{
		  LockSetType[i]= 4;
		  LockSetAddr[i]= 0x8001+i;
		}
	}
	else
	{
		for(i=0;i<LockSetTotal;i++)
		{
			LockSetType[i]= 38;
			LockSetAddr[i]=BackstageProtocol4.DataBuffer[12+j+TotalLen68_4]+(BackstageProtocol4.DataBuffer[13+j+TotalLen68_4]<<8);//2
			j=j+2;
		}
  }
}

void Write_Loss_SetDeal_4(u8 USARTxChannel)
{
	u8 i,j=0,k;
	LockSetTotal=(BackstageProtocol4.DataBuffer[7+FrontReason_4]&0x7F);
	for(i=0;i<LockSetTotal;i++)
	{
		LockSetAddr[i]=BackstageProtocol4.DataBuffer[13+j+TotalLen68_4]+(BackstageProtocol4.DataBuffer[14+j+TotalLen68_4]<<8);//2
		LockSetType[i]=BackstageProtocol4.DataBuffer[15+j+TotalLen68_4];//1
		LockSetNum[i]=BackstageProtocol4.DataBuffer[16+j+TotalLen68_4];//1
		for(k=0;k<LockSetNum[i];k++)
		{
			LockSetValue[i][k]=BackstageProtocol4.DataBuffer[17+TotalLen68_4+k+j];
		}
		j=j+LockSetNum[i]+4;
	}
}

void Write_Loss_SetGHDeal_4(u8 USARTxChannel)
{
	u8 i,j;
	settype.byte=BackstageProtocol4.DataBuffer[12+TotalLen68_4];
	
	if(settype.bit.S_E==1)//预置
	{
		Write_Loss_SetDeal_4(USARTxChannel);
		if(balance == 1)
		  Write_Loss_Set_YZ(USARTxChannel);
		else 
	  	SetSign=20;
	}
	else if(settype.bit.CR==1)//取消
	{	
		for(i=0;i<sizeof(LockSetAddr);i++)
		{
			LockSetAddr[i]=0;//2
			LockSetType[i]=0;//1
			LockSetNum[i]=0;//1
			for(j=0;j<sizeof(LockSetValue[i]);j++)
				LockSetValue[i][j]=0;
		}
		if(balance == 1)
		  Write_Loss_Set_GH(USARTxChannel,9);
		else 
		  SetSign=25;
	}
	else if(settype.bit.S_E==0)//固化
	{
		for(i=0;i<LockSetTotal;i++)
		{
			InsertValue(i);
		}
		LockSet_Flag = 1;
		if(balance == 1)
		  Write_Loss_Set_GH(USARTxChannel,7);
		else 
		  SetSign=30;
	}
}

void Read_List_Deal_4(void)
{
	u8 i;
//	for(i=0;i<4;i++)
//		List_ID[i]=BackstageProtocol4.DataBuffer[14+TotalLen68_4+i];
	for(i=0;i<BackstageProtocol4.DataBuffer[18+TotalLen68_4];i++)
	 List_Name[i]=BackstageProtocol4.DataBuffer[19+TotalLen68_4+i];
	List_NameNum=BackstageProtocol4.DataBuffer[18+TotalLen68_4];
}
void Read_Doc_Deal_4(void)
{
	u8 i;
	for(i=0;i<BackstageProtocol4.DataBuffer[14+TotalLen68_4];i++)
	 Doc_Name[i]=BackstageProtocol4.DataBuffer[15+TotalLen68_4+i];
	Doc_NameNum=BackstageProtocol4.DataBuffer[14+TotalLen68_4];
}


void CommandProcess4(u8 USARTxChannel)
{
	static u8 CTRL;          //控制域C
	static u16 ADDR;         //地址域A
	static u8 TI = 0;        //类型标识
	static u8 COT = 0;       //传送原因
	static u16 object_addr;  //信息对象地址

	if(datdone1==2)        //固定帧长
	{
	  if(Addr101_4==2)
		  ADDR= BackstageProtocol4.DataBuffer[2]+(BackstageProtocol4.DataBuffer[3]<<8);
		else ADDR= BackstageProtocol4.DataBuffer[2];
		CTRL = BackstageProtocol4.DataBuffer[1];
  }
	else if(datdone1==1)  //可变帧长
	{
		CTRL = BackstageProtocol4.DataBuffer[4];
	  if(Addr101_4==2)
		  ADDR= BackstageProtocol4.DataBuffer[5]+(BackstageProtocol4.DataBuffer[6]<<8);
		else ADDR= BackstageProtocol4.DataBuffer[5];
		TI = BackstageProtocol4.DataBuffer[6+FrontReason_4];
		COT =BackstageProtocol4.DataBuffer[8+FrontReason_4];
		
		object_addr = (u16)(BackstageProtocol4.DataBuffer[11+TotalLen68_4]<<8)+BackstageProtocol4.DataBuffer[10+TotalLen68_4];//低前高后，信息体地址
		
		if(TI==45 || TI==46)
		{
			control.byte = BackstageProtocol4.DataBuffer[12+TotalLen68_4];
		}
					
		if(balance == 1)    //平衡模式
		{
			control_field.bit.DIR = 0x01;   //终端发出
			control_field.bit.PRM = 0x01;   //启动站
			control_field.bit.FCV = 0x01;   //帧计数位有效
			control_field.bit.FC  = 0x03;   //发送/确认用户数据
			if((CTRL&0x0f) == 0x03)  //发送/确认用户数据
			{
        if(CTRL&0x10)          //FCV位为1
				{
					control_field.bit.FCB ^= 0x01;
				}
			}
			if((control.bit.S_E==1) && (COT==6) && (Select101Addr == 1)) //<6>:=激活 //预置
			{
				IEC101_Staid(0x81,USARTxChannel);                                               //<1>:=否定认可
				Select101Addr=0;
			}		
      else
		    IEC101_Staid(0x80,USARTxChannel);                                               //<0>:=认可
		}			
		else                //非平衡模式
		{
		  IEC101_Staid(0x20,USARTxChannel);                                               //<0>:=认可
		}	
	}
	
	if(ADDR==SystemSet.para.address||ADDR==0xFFFF||(ADDR==0xFF&&Addr101_4==1))
	{
		if(balance == 1) //平衡模式
		{		
			if(datdone1==2)
			{
				if(BackstageProtocol4.DataBuffer[1]==0x40)	      //主站复位远方链路
				{
					IEC101_Staid(0x80,USARTxChannel);//复位链路确认
					os_dly_wait (30);	
					IEC101_Staid(0xC9,USARTxChannel);//请求链路
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x42)   //主站发送链路测试功能
				{
					IEC101_Staid(0x80,USARTxChannel);
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x49)	  //主站请求链路状态
				{
					IEC101_Staid(0x8B,USARTxChannel);
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x0B)	  //主站响应链路状态
				{
					IEC101_Staid(0xC0,USARTxChannel);//复位主站链路
					InitSign=1;
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x00)	  //主站响应确认数据
				{
					if(InitSign!=0)            //初始化最优先
					{	
						if(InitSign==1)
						{
							InitEnd(USARTxChannel);//初始化结束
							InitSign=2;
						}
						else
						  InitSign=0;		
					}
					else if(TotalSign!=0)//总召唤标志
					{
						if(TotalSign==1)
						{
							Read_YX(USARTxChannel);
							TotalSign=2;
						}
						else if(TotalSign==2)
						{	
							Read_RealData(USARTxChannel);	
							TotalSign=3;
						}
						else if(TotalSign==3)
						{
							IEC101_Staid_All(USARTxChannel,10);
							TotalSign=4;
						}
						else 
						{
							TotalSign=0;
						  InitComplete=1;
						}
					}
					else if(ControlSign!=0)//控制 45单点控制，46双点控制
					{
						if(ControlSign==1) 
						{
							Remote_Control(USARTxChannel,10,TI,object_addr);//控制结束
							ControlSign=2;
						}
						else ControlSign=0;
					}
					else if(EnergySign!=0)
					{
						if(EnergySign==1)
						{
						  IEC101_Staid_Enegy(5,USARTxChannel,10);
						}
						else if(EnergySign==2)
						{
						  IEC101_Staid_Enegy(0x45,USARTxChannel,10);
							Rand_Flag = 1;
						}
						EnergySign = 0;
					}
					else if(DocSign!=0)//文件
					{	
						if(DocSign==1)//   传输文件第一帧
						{
							if(LineLockNum.Doc_Bz==1)
							{
								Read_Doc_Fix_Tou(USARTxChannel);														
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								Read_Doc_Rand_Tou(USARTxChannel);	
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								Read_Doc_Frzd_Tou(USARTxChannel);
							}							
							else if(LineLockNum.Doc_Bz==4)
							{
								Read_Doc_Sharp_Tou(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								 Read_Doc_Month_Tou(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								Read_Doc_Event_Tou(USARTxChannel);							
							}
						}					
						else if(DocSign==20)
						{
							if(LineLockNum.Doc_Bz==1)
							{
							  if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=59;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Fix_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=2;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Rand_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=61;
								else LineLockNum.Read_Place--;
								  LineLockNum.Read_CS++;
								Read_Doc_Frzd_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==4)
							{
								if(LineLockNum.Sharp_Time >= 264)
								{
									if(LineLockNum.Read_Place<3 && (LineLockNum.Read_Place+261)>=LineLockNum.Sharp_Place)
									{
										LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place+=261;
										if(LineLockNum.Read_Place == LineLockNum.Sharp_Place)
											LineLockNum.Read_EndBz=1;
										else
											LineLockNum.Read_EndBz=0;
									}
									else if(LineLockNum.Read_Place<3 && (LineLockNum.Read_Place+261)<LineLockNum.Sharp_Place)
									{									
										if(LineLockNum.Read_Place == 1)
										{
										  LineLockNum.Read_Num = 2;
										}
										else if(LineLockNum.Read_Place == 0)
										{
											if(LineLockNum.Sharp_Place == 262)
												LineLockNum.Read_Num = 2;
											else if(LineLockNum.Sharp_Place == 263)
												LineLockNum.Read_Num = 1;
										}
										LineLockNum.Read_Place=LineLockNum.Sharp_Place;
										LineLockNum.Read_EndBz=1;
									}
									else if(LineLockNum.Read_Place>3)
									{
										LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place-=3;
										LineLockNum.Read_EndBz=0;
									}
								}
								else
								{
									if(LineLockNum.Read_Place<3)
									{
										LineLockNum.Read_Num = LineLockNum.Read_Place;
										LineLockNum.Read_Place = 0;
										LineLockNum.Read_EndBz=1;
									}
                  else	
									{
                    LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place-=3;
										if(LineLockNum.Read_Place == 0)
											LineLockNum.Read_EndBz = 1;
										else
										  LineLockNum.Read_EndBz=0;
									}										
								}
								LineLockNum.Read_CS++;
								Read_Doc_Sharp_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=11;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Month_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								GetAddr();
								Read_Doc_Event_Data(USARTxChannel);
							}
						}			
					}
				}
		  }
		  else if(datdone1==1)
		  {
				if(TI==100)	//总召唤
				{
					IEC101_Staid_All(USARTxChannel,7);
					TotalSign=1;
					Soesend_cnt=Max_Time;
				}
				else if(TI==101 && COT==6)   //总的召唤/冻结电能计数量
				{
					if(BackstageProtocol4.DataBuffer[12+TotalLen68_4]==5)       //召唤电能脉冲计数量
					{
						IEC101_Staid_Enegy(5,USARTxChannel,7);
            os_dly_wait (30);	
						IEC101_Enegy_Send(USARTxChannel);
						EnergySign = 1;
					}
					else if(BackstageProtocol4.DataBuffer[12+TotalLen68_4]==0x45) //冻结不带复位的电能脉冲计数量
					{
						IEC101_Staid_Enegy(0x45,USARTxChannel,7);
						EnergySign = 2;
					}
				}
				else if(TI==103)	//时钟同步
				{
					if(COT==6)
					{	
						 TimeNow.second  = ((u16)(BackstageProtocol4.DataBuffer[12+TotalLen68_4] | (BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8))/1000)&0x3F;  
						 TimeNow.minute  = BackstageProtocol4.DataBuffer[14+TotalLen68_4]&0x3F;
						 TimeNow.hour    = BackstageProtocol4.DataBuffer[15+TotalLen68_4]&0x1F;
						 TimeNow.date    = BackstageProtocol4.DataBuffer[16+TotalLen68_4]&0x1F;
						 TimeNow.month   = BackstageProtocol4.DataBuffer[17+TotalLen68_4]&0x0F;
						 TimeNow.year    = BackstageProtocol4.DataBuffer[18+TotalLen68_4]&0x7F;
						 correct_time();
						 Time_Synchronization(USARTxChannel,7);//激活
						 CheckTime_Flag = 1;
					}
					else if(COT==5)
					{	
						Time_Synchronization(USARTxChannel,5);//当前时间
					}
				}	
				else if(TI==104)	//测试链路
				{	  
					TestAddr=BackstageProtocol4.DataBuffer[12+TotalLen68_4]+(BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8);
					Test_101(USARTxChannel);
				}
				else if(TI==105)	//复位进程
				{ 
					ResetLianlu(USARTxChannel);
					os_dly_wait (100);
					__set_FAULTMASK(1);
					NVIC_SystemReset();
				}
				else if(TI==45 || TI==46)	//45单点遥控，46双点遥控
				{						
					if((control.bit.S_E==1) && (COT==6))     //<6>:=激活 //预置
					{
						if(Select101Addr == 0)
						{				
						  Remote_Control(USARTxChannel,7,TI,object_addr);                                    //<7>:=激活确认
						  Select101Addr=1;
						}
					}
					else if((control.bit.S_E==0) && (COT==6))//<6>:=激活
					{							
						Remote_Control(USARTxChannel,7,TI,object_addr);                                    //<7>:=激活确认

						if(object_addr==(0x6001 + Ctrl_offset)) 
						{
							if((control.bit.QCS_RCS == 2) && (Select101Addr==1))  //QCS_RCS：1，合
							{
								if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
								{
									RecordSwitchOn(TYPE_ACT_PAD_ON);// 操作合闸记录
									SwitchFlag = 1;
								}
							}

							if((control.bit.QCS_RCS == 1) && (Select101Addr==1))  //QCS_RCS：0，分
							{
								if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
								{
									RecordSwitchOff(TYPE_ACT_PAD_OFF);// 操作分闸记录
									SwitchFlag = 2;
								}
							}
						}
						
				    if(object_addr==0x4001)                            //45单点遥控
						{
							if((control.bit.QCS_RCS == 1) && (Select101Addr==1))  //QCS_RCS：1，合
							{
								if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
								{
									RecordSwitchOn(TYPE_ACT_PAD_ON);// 操作合闸记录
									SwitchFlag = 1;
								}
							}
						}
						else if(object_addr==0x4002) 
						{
							if((control.bit.QCS_RCS == 0) && (Select101Addr==1))  //QCS_RCS：0，分
							{
								if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
								{
									RecordSwitchOff(TYPE_ACT_PAD_OFF);// 操作分闸记录
									SwitchFlag = 2;
								}
							}
						}
						else if(object_addr==0x4003 || object_addr==(0x6001 + Reset_offset)) 
						{
							if(Select101Addr==1)
							{
								SigresetFlag=1;
							}
						}
						
						if(object_addr==0x6001)         //线损模块遥控清零
						{
							if(Select101Addr==1) 
							{
								Clearloss_Flag = 1;
							}
						}
						else if(object_addr==0x6002)
						{
							if(Select101Addr==1) 
							{
								Clearevent_Flag = 1;
							}
						}
						ControlSign=1;
					}
					else if((control.bit.S_E==0) && (COT==8))//<8>:=停止激活
					{							
						Remote_Control(USARTxChannel,9,TI,object_addr);                                    //<9>:=停止激活确认
						Select101Addr=0;
					}
					control.byte = 0;
				}
				else if(TI==200)//切换区号
				{
					SN_ID=(u16)(BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8)+BackstageProtocol4.DataBuffer[12+TotalLen68_4];  
					Write_Loss_SN(USARTxChannel);
				}
				else if(TI==201)//读区号
				{	  
					Read_Loss_SN(USARTxChannel);
				}
				else if(TI==202)//读参数
				{
					Read_Loss_SetDeal_4(USARTxChannel);
					Read_Loss_Set(USARTxChannel);
				}
				else if(TI==203)//写参数
				{	
					Write_Loss_SetGHDeal_4(USARTxChannel);
				}
				else if(TI==210)//文件传输
				{
					if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==1)//读目录
					{
							Read_List_Deal_4();
							Read_List_OK(USARTxChannel,SearchList());
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==3)//读文件激活
					{
							Read_Doc_Deal_4();
							LineLockNum.Doc_Bz=SearchDoc();
							DocSign=1;//激活
							if(LineLockNum.Doc_Bz==1)
							{
								Read_Doc_Fix_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								Read_Doc_Rand_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								Read_Doc_Frzd_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==4)
							{
								Read_Doc_Sharp_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								Read_Doc_Month_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								Read_Doc_Event_OK(USARTxChannel);							
							}
							else 
							{
								Read_Doc_Error(USARTxChannel);
								DocSign=0;
							}
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==6)//读文件确认
					{
						//
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==7)//写文件激活
					{						
//						Write_DocOK_Deal();
//						Write_Doc_OK(USARTxChannel);
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==9)//写文件
					{
						Write_Doc_Data_Deal();
						Write_Doc_Data_OK(USARTxChannel,DocErr);
					}
				}
				else if(TI==212)//文件同步
				{	
					Read_Doc_Same(USARTxChannel);
				}
				else if(TI==211) //程序升级启动 结束 停止
				{
					Updatetype.byte = BackstageProtocol4.DataBuffer[12+TotalLen68_4];
					if(COT==6 && Updatetype.bit.S_E==1) //启动
					{
						SoftUpdate(USARTxChannel,7);
						LineLockNum.Write_Text=0;
						LineLockNum.Last_Text=0;
						LineLockNum.Next_Text=0;
					}
					else if(COT==8 && Updatetype.bit.S_E==0)//取消
					{
						SoftUpdate(USARTxChannel,9);
					}
					else if(COT==6&&Updatetype.bit.S_E==0)//升级结束
					{
						SoftUpdate(USARTxChannel,10);
						os_dly_wait (2000);	//2S以后复位更新程序
						SofeReset();
					}
				}	
		  }
    }
	  else //非平衡模式
	  {
		  if(datdone1==2)
		  {
        if(BackstageProtocol4.DataBuffer[1]==0x40)	    //复位远方链路
				{
					IEC101_Staid(0x20,USARTxChannel);             //链路复位确认，有一级数据
					InitSign=1;
				}
				else if(BackstageProtocol4.DataBuffer[1]==0x49)	//请求链路状态
				{
					IEC101_Staid(0x0B,USARTxChannel);             //链路状态
				}
				else if((BackstageProtocol4.DataBuffer[1] & 0x40) && (BackstageProtocol4.DataBuffer[1] & 0x0F)==0x0A)	//请求1级用户数据
				{
					if(InitSign!=0)             //初始化最优先
					{	
						if(InitSign==1)
						{
							InitEnd(USARTxChannel); //初始化结束
						}
						InitSign=0;
					}
					else if(TotalSign!=0)//总召唤标志
					{
						if(TotalSign==1)
						{
							IEC101_Staid_All(USARTxChannel,7);
							TotalSign=2;
						}
						else if(TotalSign==2)
						{
							Read_YX(USARTxChannel);
							TotalSign=3;
						}
						else if(TotalSign==3)
						{
							Read_RealData(USARTxChannel);
							TotalSign=4;
						}
						else if(TotalSign==4)
						{
							IEC101_Staid_All(USARTxChannel,10);
							TotalSign=0;
						}
						else TotalSign=0;
					}
					else if(ControlSign!=0)//控制 45单点控制，46双点控制
					{	
						if(ControlSign==1) 
						{
							if((control.bit.S_E==1) && (COT==6))     //<6>:=激活 //预置
							{
								ControlSign=0;
								if(Select101Addr == 0)
								{
									Remote_Control(USARTxChannel,7,TI,object_addr);                                 //<7>:=激活确认
									Select101Addr=1;
								}
								else
								{
									IEC101_Staid(0x21,USARTxChannel);                                               //<1>:=否定认可
									Select101Addr = 0;
								}
							}
							else if((control.bit.S_E==0) && (COT==6))//<6>:=激活 //执行
							{	
                ControlSign=2;								
								if(object_addr==(0x6001 + Ctrl_offset))                                      //46双点遥控
								{
									if((control.bit.QCS_RCS == 2) && (Select101Addr==1))  //QCS_RCS：1，合
									{
										if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
										{
											RecordSwitchOn(TYPE_ACT_PAD_ON);// 操作合闸记录
											SwitchFlag = 1;
										}
									}

									if((control.bit.QCS_RCS == 1) && (Select101Addr==1))  //QCS_RCS：0，分
									{
										if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
										{
											RecordSwitchOff(TYPE_ACT_PAD_OFF);// 操作分闸记录
											SwitchFlag = 2;
										}
									}
								}
								else if(object_addr==0x4001)                            //45单点遥控
								{
									if((control.bit.QCS_RCS == 1) && (Select101Addr==1))  //QCS_RCS：1，合
									{
										if(ProtectFlag == 0 && KZQMeasureData.para.DLQflag.bit.breaker == 2 && SwitchFlag == 0)
										{
											RecordSwitchOn(TYPE_ACT_PAD_ON);// 操作合闸记录
											SwitchFlag = 1;
										}
									}
								}
								else if(object_addr==0x4002) 
								{
									if((control.bit.QCS_RCS == 0) && (Select101Addr==1))  //QCS_RCS：0，分
									{
										if(KZQMeasureData.para.DLQflag.bit.breaker == 1 && SwitchFlag == 0)
										{
											RecordSwitchOff(TYPE_ACT_PAD_OFF);// 操作分闸记录
											SwitchFlag = 2;
										}
									}
								}
								else if(object_addr==0x4003 || object_addr==(0x6001 + Reset_offset)) 
								{
									if(Select101Addr==1)
									{
										SigresetFlag=1;
									}
								}
								
								if(object_addr==0x6001)              //线损模块遥控清零
								{
									if(Select101Addr==1) 
									{
										Clearloss_Flag = 1;
									}
								}
								else if(object_addr==0x6002)
								{
									if(Select101Addr==1) 
									{
										Clearevent_Flag = 1;
									}
								}
								Remote_Control(USARTxChannel,7,TI,object_addr);                                //<7>:=遥控执行确认
							}
							else if((control.bit.S_E==0) && (COT==0))//<0>:=停止激活
							{
								Remote_Control(USARTxChannel,9,TI,object_addr);                                //<9>:=停止激活确认
								Select101Addr=0;
								ControlSign=0;
							}
							control.byte = 0;
						}
						else if(ControlSign==2)
						{
							Remote_Control(USARTxChannel,10,TI,object_addr);//遥控执行结束
							ControlSign=0;
						}
						else ControlSign=0;
					}
					else if(ResetSign!=0)//复位链路
					{
						if(ResetSign==1)
						{
							ResetLianlu(USARTxChannel);
							os_dly_wait (100);
							__set_FAULTMASK(1);
							NVIC_SystemReset();
							ResetSign=0;
						}
						else ResetSign=0;
					}
					else if(SetSign!=0)//复位链路
					{
						if(SetSign==1) //切换SN
						{
							Write_Loss_SN(USARTxChannel);
							SetSign=0;
						}
						else if(SetSign==5)//读SN
						{
							Read_Loss_SN(USARTxChannel);
							SetSign=0;
						}
						else if(SetSign==10)//读参数
						{
							Read_Loss_Set(USARTxChannel);
							SetSign=0;
						}
						else if(SetSign==20)//预置
						{
							Write_Loss_Set_YZ(USARTxChannel);
							SetSign=0;
						}
						else if(SetSign==25)//取消
						{
							Write_Loss_Set_GH(USARTxChannel,9);
							SetSign=0;
						}
						else if(SetSign==30)//固化
						{
							Write_Loss_Set_GH(USARTxChannel,7);
							SetSign=0;
						}
						else SetSign=0;
					}
					else if(EnergySign!=0)
					{
							if(EnergySign==1)
							{
								EnergySign=2;
								IEC101_Staid_Enegy(5,USARTxChannel,7);
							}
							else if(EnergySign==2)
							{
								EnergySign=3;
								IEC101_Enegy_Send(USARTxChannel);
							}
							else if(EnergySign==3)
							{
								EnergySign=0;
								IEC101_Staid_Enegy(5,USARTxChannel,10);
							}
							else if(EnergySign==4)
							{
								EnergySign=5;
                IEC101_Staid_Enegy(0x45,USARTxChannel,7);
							}
							else if(EnergySign==5)
							{
								EnergySign=0;
                IEC101_Staid_Enegy(0x45,USARTxChannel,10);
								Rand_Flag = 1;
							}		
							else EnergySign=0;
					}
//					else if(SOE_Flag!=0)//变位遥信
//					{
//						Record_SOE_Send(USARTxChannel);  //SOE事件顺序记录  
//					}        
					else 
						IEC101_Staid(0x09,USARTxChannel);           //0 1 0ACD(要求访问位) 0DFC(数据流控制位) (都为0，不使用) 1001(功能码9为无所召唤的数据)  
				}
				else if((BackstageProtocol4.DataBuffer[1] & 0x40) && (BackstageProtocol4.DataBuffer[1] & 0x0F)==0x0B)	//请求2级用户数据
				{
					KZQMeasureData.para.SelfCheck.bit.GPRS_state = 2;

					if(ClockSign!=0)
					{
						if(ClockSign==1)
						{
							ClockSign=0;
							Time_Synchronization(USARTxChannel,7);//激活
							CheckTime_Flag = 1;
						}
						else if(ClockSign==5)
						{	
							ClockSign=0;
							Time_Synchronization(USARTxChannel,5);//当前时间
						}
						else ClockSign=0;
					}
					else if(TestSign!=0)//链路测试
					{
						if(TestSign==1)
						{
							Test_101(USARTxChannel);
							TestSign=0;
						}
						else TestSign=0;
					}
					else if(DocSign!=0)//文件
					{	
						if(DocSign==1) //读目录激活确认
						{
							LineLockNum.Read_CS=0;
							Read_List_OK(USARTxChannel,SearchList());
							DocSign=0;	
						}
						else if(DocSign==10)//读文件激活确认
						{	
							DocSign=11;
							if(LineLockNum.Doc_Bz==1)
							{
								Read_Doc_Fix_OK(USARTxChannel);														
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								Read_Doc_Rand_OK(USARTxChannel);	
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								Read_Doc_Frzd_OK(USARTxChannel);
							}							
							else if(LineLockNum.Doc_Bz==4)
							{
								Read_Doc_Sharp_OK(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								 Read_Doc_Month_OK(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								Read_Doc_Event_OK(USARTxChannel);							
							}
							else 
							{
								Read_Doc_Error(USARTxChannel);
								DocSign=0;
							}
						}
						else if(DocSign==11)//   传输文件第一帧
						{
							if(LineLockNum.Doc_Bz==1)
							{
								Read_Doc_Fix_Tou(USARTxChannel);														
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								Read_Doc_Rand_Tou(USARTxChannel);	
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								Read_Doc_Frzd_Tou(USARTxChannel);
							}							
							else if(LineLockNum.Doc_Bz==4)
							{
								Read_Doc_Sharp_Tou(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								 Read_Doc_Month_Tou(USARTxChannel);							
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								Read_Doc_Event_Tou(USARTxChannel);							
							}
						}
						else if(DocSign==20)
						{			
							if(LineLockNum.Doc_Bz==1)
							{
							  if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=59;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Fix_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==2)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=2;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Rand_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==3)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=61;
								else LineLockNum.Read_Place--;
								  LineLockNum.Read_CS++;
								Read_Doc_Frzd_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==4)
							{
								if(LineLockNum.Sharp_Time >= 264)
								{
									if(LineLockNum.Read_Place<3 && (LineLockNum.Read_Place+261)>=LineLockNum.Sharp_Place)
									{
										LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place+=261;
										if(LineLockNum.Read_Place == LineLockNum.Sharp_Place)
											LineLockNum.Read_EndBz=1;
										else
											LineLockNum.Read_EndBz=0;
									}
									else if(LineLockNum.Read_Place<3 && (LineLockNum.Read_Place+261)<LineLockNum.Sharp_Place)
									{									
										if(LineLockNum.Read_Place == 1)
										{
										  LineLockNum.Read_Num = 2;
										}
										else if(LineLockNum.Read_Place == 0)
										{
											if(LineLockNum.Sharp_Place == 262)
												LineLockNum.Read_Num = 2;
											else if(LineLockNum.Sharp_Place == 263)
												LineLockNum.Read_Num = 1;
										}
										LineLockNum.Read_Place=LineLockNum.Sharp_Place;
										LineLockNum.Read_EndBz=1;
									}
									else if(LineLockNum.Read_Place>3)
									{
										LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place-=3;
										LineLockNum.Read_EndBz=0;
									}
								}
								else
								{
									if(LineLockNum.Read_Place<3)
									{
										LineLockNum.Read_Num = LineLockNum.Read_Place;
										LineLockNum.Read_Place = 0;
										LineLockNum.Read_EndBz=1;
									}
                  else	
									{
                    LineLockNum.Read_Num = 3;
										LineLockNum.Read_Place-=3;
										if(LineLockNum.Read_Place == 0)
											LineLockNum.Read_EndBz = 1;
										else
										  LineLockNum.Read_EndBz=0;
									}										
								}
								LineLockNum.Read_CS++;
								Read_Doc_Sharp_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==5)
							{
								if(LineLockNum.Read_Place==0) 
									LineLockNum.Read_Place=11;
								else 
									LineLockNum.Read_Place--;
								LineLockNum.Read_CS++;
								Read_Doc_Month_Data(USARTxChannel);
							}
							else if(LineLockNum.Doc_Bz==6)
							{
								GetAddr();
								Read_Doc_Event_Data(USARTxChannel);
							}
						}
					}
					else if(WriteDocSign!=0)
					{
						if(WriteDocSign==1)
						{
							Write_Doc_OK(USARTxChannel);
							WriteDocSign=0;
						}
						else if(WriteDocSign==2)
						{
							Write_Doc_Data_OK(USARTxChannel,DocErr);
							WriteDocSign=0;
						}
						else WriteDocSign=0;
					}
					else if(SoftUpSign!=0)
					{
						if(SoftUpSign==1)//启动
						{
							 SoftUpdate(USARTxChannel, 7);
							 SoftUpSign=0;
						}
						else if(SoftUpSign==5)// 结束
						{
							 SoftUpdate(USARTxChannel, 10);
							 SoftUpSign=0;
							 os_dly_wait (2000);	//2S以后复位更新程序
						   SofeReset();
						}
						else if(SoftUpSign==10)//中止
						{
							 SoftUpdate(USARTxChannel, 9);
							 SoftUpSign=0;
						}
						else SoftUpSign=0;
					}
					else if(DocSameSign!=0)
					{
							if(DocSameSign==1)
							{
								DocSameSign=0;
								Read_Doc_Same(USARTxChannel);
							}
							else DocSameSign=0;
					}
					else
					{
						//先判断是否有1级数据 遥信变位 常规控制域一直回的08，有一级数据回28			
						H2SJ(USARTxChannel); //回变化遥测
					}
				}
		  }
		  else if(datdone1==1)//数据0x68开头
		  {
				if(TI==100)	//总召唤
				{		
					TotalSign=1;
				}
				else if(TI==101 && COT==6)   //总的召唤/冻结电能计数量
				{
					if(BackstageProtocol4.DataBuffer[12+TotalLen68_4]==5)       //召唤电能脉冲计数量
					{
						EnergySign=1;
					}
					else if(BackstageProtocol4.DataBuffer[12+TotalLen68_4]==0x45) //冻结不带复位的电能脉冲计数量
					{
						EnergySign=4;
					}
				}
				else if(TI==103)	//时钟同步
				{
					if(COT==6)
					{
						TimeNow.second  = ((u16)(BackstageProtocol4.DataBuffer[12+TotalLen68_4] | (BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8))/1000)&0x3F;  
						TimeNow.minute  = BackstageProtocol4.DataBuffer[14+TotalLen68_4]&0x3F;
						TimeNow.hour    = BackstageProtocol4.DataBuffer[15+TotalLen68_4]&0x1F;
						TimeNow.date    = BackstageProtocol4.DataBuffer[16+TotalLen68_4]&0x1F;
						TimeNow.month   = BackstageProtocol4.DataBuffer[17+TotalLen68_4]&0x0F;
						TimeNow.year    = BackstageProtocol4.DataBuffer[18+TotalLen68_4]&0x7F;
						correct_time();
						ClockSign=1;
					}
					else if(COT==5)
					{	
						ClockSign=5;
					}
				}	
				else if(TI==104)	//测试链路
				{
					TestAddr=BackstageProtocol4.DataBuffer[12+TotalLen68_4]+(BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8);
					TestSign=1;
				}
				else if(TI==105)	//复位进程
				{
					ResetSign=1;
				}
				else if(TI==45 || TI==46)	//45单点遥控，46双点遥控
				{
					ControlSign=1;
				}
				else if(TI==200)//切换区号
				{
					SN_ID=(u16)(BackstageProtocol4.DataBuffer[13+TotalLen68_4]<<8)+BackstageProtocol4.DataBuffer[12+TotalLen68_4];
					SetSign=1;  
				}
				else if(TI==201)//读区号
				{
					SetSign=5; 
				}
				else if(TI==202)//读参数
				{
					SetSign=10;	
					Read_Loss_SetDeal_4(USARTxChannel);
				}
				else if(TI==203)//写参数
				{
					Write_Loss_SetGHDeal_4(USARTxChannel);
				}
				else if(TI==210)//文件传输
				{
					if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==1)//读目录
					{
						Read_List_Deal_4();
						DocSign=1;
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==3)//读文件激活
					{
						Read_Doc_Deal_4();
						LineLockNum.Doc_Bz=SearchDoc();
						DocSign=10;//激活
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==6)//读文件确认
					{
				
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==7)//写文件激活
					{				
						//Write_DocOK_Deal();
						//WriteDocSign=1;
					}
					else if(BackstageProtocol4.DataBuffer[13+TotalLen68_4]==9)//写文件
					{
						//Write_Doc_Data_Deal();
						//WriteDocSign=3;
					}
				}
				else if(TI==212)//文件同步
				{
					DocSameSign=1;
				}
				else if(TI==211) //程序升级启动 结束 停止
				{
					Updatetype.bit.S_E=BackstageProtocol4.DataBuffer[12+TotalLen68_4];
					if(COT==6 && Updatetype.bit.S_E==1) //启动
					{
						SoftUpSign=1;
						LineLockNum.Write_Text=0;
						LineLockNum.Last_Text=0;
						LineLockNum.Next_Text=0;
					}
					else if(COT==8&&Updatetype.bit.S_E==0)//取消
					{
						SoftUpSign=10;
					}
					else if(COT==6&&Updatetype.bit.S_E==0)//升级结束
					{
						SoftUpSign=5;
					}
				}	
		  }
    }
	}
}
