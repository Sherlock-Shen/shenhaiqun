//###########################################################################
//
// 电磁铁控制文件
//
// 
//
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  12.08| 29 Aug 2012 | S.J.M. | 
//###########################################################################
#include "control.h"
#include "flash.h"
#include "math.h"

u16 CtrlTimeDelay = 0;
u16 StoreTimeDelay = 0;
u8 CtrlTimeout = 0;
u8 CtrlState = 0;
u8 StoreTimeout = 0;
static u16 yx_cnt = 0;
static u16 wcn_cnt = 0;

u8 CtrlOutput(u8 *switch_flag)
{
	static u8 state = 0;
	
	if(*switch_flag == 0)						// 无任何动作
	{
		state = 0;
		return 0;	
	}
	else if(*switch_flag == 1)		// 合闸动作，确保无保护时执行
	{
    switch(state)
		{
      case 0:                     // 打开继电器
				if(Switchon_Lock == 0 && Switchon_Lock1 == 0 && Switchon_Lock2 == 0)    //闭锁合闸
				{
					HE_ON;					          // 打开合闸继电器
					OUT_COM_ON;		            // 打开输出公共端
					yx_cnt = 0;
					state = 1;
					CtrlTimeDelay = 20;//20ms			
					CtrlTimeout = 0;
					CtrlMeasureCnt = 0;
					CtrlMeasureFlag = 0x01;
					KZQMeasureData.para.ProtectFlag2.bit.close_flag = 1;  //合闸标志
				}
				else
				{
					*switch_flag = 0;
		      state = 0;
				}
				break;
			case 1:								
				if(CtrlTimeout == 1)// 延时时间到
				{
					state = 2;
					CtrlTimeDelay = CONTROL_ON_DELAY;  //50ms				
					CtrlTimeout = 0;
				}
				break;
			case 2:							
				if(CtrlTimeout == 1)// 延时时间到
				{
					HE_OFF;				// 关闭合闸继电器
					state = 3;
					CtrlTimeDelay = 40;//40ms				
					CtrlTimeout = 0;
					yx_cnt = 0;
				}
				else
				{
					if(FW_IN1 != 0 && HW_IN2 != 0 )	// 断路器位置脱开
					{
					  yx_cnt++;
						if(yx_cnt > 20 )
						{
							HE_OFF;				// 关闭合闸继电器
							state = 3;
							CtrlTimeDelay = 40;//40ms			
							CtrlTimeout = 0;
							yx_cnt = 0;
						}
					}
					else
					{
						yx_cnt = 0;
					}
				}
				break;
			case 3:			// 判断操作是否成功
				if(CtrlTimeout == 1)// 延时时间到
				{
					if(HW_IN2 == 0 && FW_IN1 == 1 )	// 断路器合位
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 0;
						KZQMeasureData.para.DLQflag.bit.breaker = 1;		// 合闸状态
					}
					else if(HW_IN2 != 0 && FW_IN1 == 0 && WCN_IN3 == 1)// 断路器分位,合闸线圈通，已储能
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 1;
					}
					else
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 1;
					}
					*switch_flag = 0;
					state = 0;
					if(KZQMeasureData.para.ProtectFlag1.word & 0xFF00)
					{
						KZQMeasureData.para.ProtectFlag1.word &= 0x00FF;
						if(KZQMeasureData.para.ProtectFlag2.bit.close_err == 0) Record_autofeed.para.type |= 0x80;
						
						KZQMeasureData.para.RequestFlag1.bit.record_autofeed = 1;
					}
					else
					{
						if(Record_on.para.type == TYPE_ACT_PAD_ON) //本地合闸
							RecordSOE_Pad(SOE_ADDR_ACT_PAD_ON_pad,KZQMeasureData.para.ProtectFlag2.bit.close_err^0x01);  //close_err取反
						else if(Record_on.para.type == TYPE_ACT_FAR_ON) //远方合闸
							RecordSOE_Pad(SOE_ADDR_ACT_FAR_ON_pad,KZQMeasureData.para.ProtectFlag2.bit.close_err^0x01);  //close_err取反

						if(KZQMeasureData.para.ProtectFlag2.bit.close_err == 0) Record_on.para.type |= 0x80;
						
						KZQMeasureData.para.RequestFlag1.bit.record_on = 1;	  // 置位操作合闸记录
				  }
				}
				break;
			default:
				break;
		}
	}
	else if(*switch_flag == 2)					// 分闸动作		
	{
		switch(state)
		{
			case 0:								        // 打开继电器
				if(Switchoff_Lock == 0 && Switchoff_Lock1 == 0)
				{
					FEN_ON;					            // 打开分闸继电器
					OUT_COM_ON;		              // 打开输出公共端
					yx_cnt = 0;
					state = 1;
					CtrlTimeDelay = 20;        //20ms
					CtrlTimeout = 0;
					CtrlMeasureCnt = 0;
					CtrlMeasureFlag = 0x02;
					KZQMeasureData.para.ProtectFlag2.bit.open_flag = 1; //分闸标志
				}
				else
				{
					*switch_flag = 0;
		      state = 0;
				}
				break;
			case 1:								
				if(CtrlTimeout == 1)// 延时时间到
				{
					state = 2;
					CtrlTimeDelay = CONTROL_OFF_DELAY;  //50ms				
					CtrlTimeout = 0;
				}
				break;
			case 2:								
				if(CtrlTimeout == 1)// 延时时间到
				{
					FEN_OFF;				// 关闭分闸继电器
					state = 3;
					CtrlTimeDelay = 40;	//40ms			
					CtrlTimeout = 0;
					yx_cnt = 0;
				}
				else
				{
					if(HW_IN2 != 0 && FW_IN1 != 0 )	// 断路器位置脱开
					{
						yx_cnt++;
						if(yx_cnt > 20)
						{	
							FEN_OFF;				// 关闭分闸继电器
							state = 3;
					        CtrlTimeDelay = 40;	//40ms				
					        CtrlTimeout = 0;
					        yx_cnt = 0;
						}						
					}
					else
					{
						yx_cnt = 0;
					}
				}
				break;
			case 3:			// 判断操作是否成功
				if(CtrlTimeout == 1)// 延时时间到
				{
					if( FW_IN1 == 0 && HW_IN2 == 1 )// 断路器分位
					{
						KZQMeasureData.para.ProtectFlag2.bit.open_err = 0;
						KZQMeasureData.para.DLQflag.bit.breaker = 2;	// 分闸状态
					}
					else if( HW_IN2 == 0 && FW_IN1 == 1 )	// 断路器合位,分闸线圈通
					{
						KZQMeasureData.para.ProtectFlag2.bit.open_err = 1;
					}
					else
					{
						KZQMeasureData.para.ProtectFlag2.bit.open_err = 1;
					}
					*switch_flag = 0;
					state = 0;
					if(KZQMeasureData.para.ProtectFlag1.word & 0xFF00)
					{
						KZQMeasureData.para.ProtectFlag1.word &= 0x00FF;
						if(KZQMeasureData.para.ProtectFlag2.bit.open_err == 0) Record_autofeed.para.type |= 0x80;
						
						KZQMeasureData.para.RequestFlag1.bit.record_autofeed = 1;
					}
					else
					{
						if(Record_off.para.type == TYPE_ACT_PAD_OFF) //本地分闸
							RecordSOE_Pad(SOE_ADDR_ACT_PAD_OFF_pad,KZQMeasureData.para.ProtectFlag2.bit.open_err^0x01);  //open_err取反
						else if(Record_off.para.type == TYPE_ACT_FAR_OFF) //远方分闸
							RecordSOE_Pad(SOE_ADDR_ACT_FAR_OFF_pad,KZQMeasureData.para.ProtectFlag2.bit.open_err^0x01);  //open_err取反

						if(KZQMeasureData.para.ProtectFlag2.bit.open_err == 0) Record_off.para.type |= 0x80;
						
						KZQMeasureData.para.RequestFlag1.bit.record_off = 1;	// 置位操作分闸记录
				  }
				}
				break;
			default:
				break;
		}
		return 2;
	}
	else if(*switch_flag == 3)					// 保护分闸动作	
	{
		switch(state)
		{
			case 0:								// 打开继电器
				if(Switchoff_Lock == 0 && Switchoff_Lock1 == 0)
				{
					FEN_ON;					// 打开分闸继电器
					OUT_COM_ON;		            // 打开输出公共端
					CtrlTimeDelay = 20;		//20ms		
					CtrlTimeout = 0;
					state = 1;
					CtrlMeasureCnt = 0;
					CtrlMeasureFlag = 0x03;
					KZQMeasureData.para.ProtectFlag2.bit.protect_flag = 1;
				}
				else
				{
					*switch_flag = 0;
		      state = 0;
				}
				break;
			case 1:								
				if(CtrlTimeout == 1)// 延时时间到
				{
					state = 2;
					CtrlTimeDelay = CONTROL_OFF_DELAY;  //50ms				
					CtrlTimeout = 0;
				}
				break;
			case 2:							
				if(CtrlTimeout == 1)// 延时时间到
				{		
					FEN_OFF;				// 关闭分闸继电器
					state = 3;
					CtrlTimeDelay = 40; //40ms				
					CtrlTimeout = 0;
					yx_cnt = 0;
				}
				else
				{	
					if(HW_IN2 != 0 && FW_IN1 != 0 )	// 断路器位置脱开
					{
						yx_cnt++;
						if(yx_cnt > 20)
						{	
							FEN_OFF;				// 关闭分闸继电器
							state = 3;
							CtrlTimeDelay = 40;	//40ms					
							CtrlTimeout = 0;
							yx_cnt = 0;
						}						
					}
					else
					{
						yx_cnt = 0;
					}
				}
				break;
			case 3:			// 判断操作是否成功
				if(CtrlTimeout == 1)// 延时时间到
				{
					if(HW_IN2 != 0 && FW_IN1 == 0)// 断路器分位
					{
						KZQMeasureData.para.ProtectFlag2.bit.open_err = 0;
						KZQMeasureData.para.DLQflag.bit.breaker = 2;	// 分闸状态
						if(ReOnFlag) ProtectFlag = 0;	
					}
					else if(HW_IN2 == 0 && FW_IN1 == 1)	// 断路器合位,分闸线圈通
					{
						KZQMeasureData.para.ProtectFlag2.bit.open_err = 1;
					}
					else
					{
						KZQMeasureData.para.ProtectFlag2.bit.open_err = 1;
					}
					*switch_flag = 0;
					state = 0;
					
					if((ProtectRecord_flag&0x0010) != 0)
					{
						ProtectRecord_flag &= ~0x0010;
						if(KZQMeasureData.para.ProtectFlag2.bit.open_err == 0) Record_protect1.para.type |= 0x80;
						
					}
					else if((ProtectRecord_flag&0x0020) != 0)
					{
						ProtectRecord_flag &= ~0x0020;
					  if(KZQMeasureData.para.ProtectFlag2.bit.open_err == 0) Record_protect2.para.type |= 0x80;
					}
					
					KZQMeasureData.para.RequestFlag1.bit.record_protect = 1;	// 置位保护记录
				}
				break;
			default:
				break;
		}
		return 3;
	}
	else if(*switch_flag == 4)					// 重合闸动作，确保无保护时执行
	{
		switch(state)
		{
			case 0:				   //同期合闸控制字
				if(ProtectSet.para.reon_synchron_enable == 1 && MeasureData.Protect_UA_val>3000 && MeasureData.Protect_UB_val>3000)
				{
					CtrlTimeDelay = 6000;	//6s
					CtrlTimeout = 0;
					state = 1;
			  }
			  else
			  {
          state = 2;
			  }
			  break;
      case 1:                  //同期合闸
			  if(abs(MeasureData.Protect_UA_val- MeasureData.Protect_UB_val)< 1440 && 
					 MeasureData.Display_UB1_val< 3000 && abs(MeasureData.freq_cpu1 - MeasureData.freq_cpu2) < 200)
		    {
          state = 2;
        }
				else
				{
          state = 1;
				}
        if(CtrlTimeout == 1)// 延时时间到
				{
				  state = 5;
				}
				break;
			case 2:								  // 打开继电器
				if(Switchon_Lock == 0 && Switchon_Lock1 == 0 && Switchon_Lock2 == 0)
				{
					ReOnLockCnt = 0;
					HE_ON;					// 打开合闸继电器
					OUT_COM_ON;		        // 打开输出公共端
					state = 3;
					CtrlTimeDelay = 20;	//20ms			
					CtrlTimeout = 0;
					CtrlMeasureCnt = 0;
					CtrlMeasureFlag = 0x01;
					KZQMeasureData.para.ProtectFlag2.bit.reon_act = 1;// 重合闸动作标志
				}
			  else
				{
					*switch_flag = 0;
		      state = 0;
				}
				break;
			case 3:								
				if(CtrlTimeout == 1)// 延时时间到
				{
					state = 4;
					CtrlTimeDelay = CONTROL_ON_DELAY;  //50ms				
					CtrlTimeout = 0;
				}
				break;
			case 4:								
				if(CtrlTimeout == 1)// 延时时间到
				{
					HE_OFF;				// 关闭合闸继电器
					state = 5;
					CtrlTimeDelay = 40;	//40ms				
					CtrlTimeout = 0;
					yx_cnt = 0;
				}
				else
				{
					if(FW_IN1 != 0 && HW_IN2 != 0 )	// 断路器位置脱开
					{
					    yx_cnt++;
						if(yx_cnt > 20 )
						{
							HE_OFF;				// 关闭合闸继电器
							state = 5;
					        CtrlTimeDelay = 40;	//40ms			
					        CtrlTimeout = 0;
					        yx_cnt = 0;
						}
					}
					else
					{
						yx_cnt = 0;
					}
				}
				break;
			case 5:			// 判断操作是否成功
				if(CtrlTimeout == 1)// 延时时间到
				{
					if(HW_IN2 == 0 && FW_IN1 == 1)	// 断路器合位
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 0;
						KZQMeasureData.para.DLQflag.bit.breaker = 1;		// 合闸状态
					}
					else if(HW_IN2 == 1 && FW_IN1 == 0 )// 断路器分位,合闸线圈通，已储能
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 1;
					}
					else
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 1;
					}
					*switch_flag = 0;
					state = 0;
					if(Record_on.para.type == TYPE_ACT_REON) //保护重合闸
						RecordSOE_Pad(SOE_ADDR_REON_pad,KZQMeasureData.para.ProtectFlag2.bit.close_err^0x01);  //close_err取反
					
					if(KZQMeasureData.para.ProtectFlag2.bit.close_err == 0) Record_on.para.type |= 0x80;
					
					KZQMeasureData.para.RequestFlag1.bit.record_on = 1;	  // 置位操作合闸记录
				}
				break;
			default:
				break;
		}
		return 4;
	}
	return 0;
}

void StoreOutput(void)
{
	static u8 state = 0;
	switch(state)
	{
		case 0:
			if(WCN_IN3 == 0 && SwitchFlag == 0)	
			{
				StoreTimeDelay = 2000;
				StoreTimeout = 0;
				state = 1;
			}
			break;
		case 1:
			if(StoreTimeout == 1)
			{	
				KZCN_ON;
				OUT_COM_ON;		        // 打开输出公共端		
				StoreTimeDelay = 18000;     //储能时间18S
				StoreTimeout = 0;
				state = 2;
			}
			break;
		case 2:							
			if(StoreTimeout == 1)
			{
				KZCN_OFF;			
				StoreTimeDelay = 1000; //关闭时间1S
				StoreTimeout = 0;
				state = 3;				
			}
			if( WCN_IN3 == 1 )
			{
	    	  wcn_cnt++;
			  if(wcn_cnt > 100)
			  {
			    KZCN_OFF;
					StoreTimeDelay = 0;
				  state = 0;
				  wcn_cnt = 0;			
			  }					
			}
			else
			  wcn_cnt = 0;
			break;
		case 3:							
			if(StoreTimeout == 1)
			{
				KZCN_OFF;			
				state = 0;				
			}
			break;
		default:
			break;	
	}
}
