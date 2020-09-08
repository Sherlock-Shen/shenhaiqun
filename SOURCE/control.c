//###########################################################################
//
// ����������ļ�
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
	
	if(*switch_flag == 0)						// ���κζ���
	{
		state = 0;
		return 0;	
	}
	else if(*switch_flag == 1)		// ��բ������ȷ���ޱ���ʱִ��
	{
    switch(state)
		{
      case 0:                     // �򿪼̵���
				if(Switchon_Lock == 0 && Switchon_Lock1 == 0 && Switchon_Lock2 == 0)    //������բ
				{
					HE_ON;					          // �򿪺�բ�̵���
					OUT_COM_ON;		            // �����������
					yx_cnt = 0;
					state = 1;
					CtrlTimeDelay = 20;//20ms			
					CtrlTimeout = 0;
					CtrlMeasureCnt = 0;
					CtrlMeasureFlag = 0x01;
					KZQMeasureData.para.ProtectFlag2.bit.close_flag = 1;  //��բ��־
				}
				else
				{
					*switch_flag = 0;
		      state = 0;
				}
				break;
			case 1:								
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					state = 2;
					CtrlTimeDelay = CONTROL_ON_DELAY;  //50ms				
					CtrlTimeout = 0;
				}
				break;
			case 2:							
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					HE_OFF;				// �رպ�բ�̵���
					state = 3;
					CtrlTimeDelay = 40;//40ms				
					CtrlTimeout = 0;
					yx_cnt = 0;
				}
				else
				{
					if(FW_IN1 != 0 && HW_IN2 != 0 )	// ��·��λ���ѿ�
					{
					  yx_cnt++;
						if(yx_cnt > 20 )
						{
							HE_OFF;				// �رպ�բ�̵���
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
			case 3:			// �жϲ����Ƿ�ɹ�
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					if(HW_IN2 == 0 && FW_IN1 == 1 )	// ��·����λ
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 0;
						KZQMeasureData.para.DLQflag.bit.breaker = 1;		// ��բ״̬
					}
					else if(HW_IN2 != 0 && FW_IN1 == 0 && WCN_IN3 == 1)// ��·����λ,��բ��Ȧͨ���Ѵ���
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
						if(Record_on.para.type == TYPE_ACT_PAD_ON) //���غ�բ
							RecordSOE_Pad(SOE_ADDR_ACT_PAD_ON_pad,KZQMeasureData.para.ProtectFlag2.bit.close_err^0x01);  //close_errȡ��
						else if(Record_on.para.type == TYPE_ACT_FAR_ON) //Զ����բ
							RecordSOE_Pad(SOE_ADDR_ACT_FAR_ON_pad,KZQMeasureData.para.ProtectFlag2.bit.close_err^0x01);  //close_errȡ��

						if(KZQMeasureData.para.ProtectFlag2.bit.close_err == 0) Record_on.para.type |= 0x80;
						
						KZQMeasureData.para.RequestFlag1.bit.record_on = 1;	  // ��λ������բ��¼
				  }
				}
				break;
			default:
				break;
		}
	}
	else if(*switch_flag == 2)					// ��բ����		
	{
		switch(state)
		{
			case 0:								        // �򿪼̵���
				if(Switchoff_Lock == 0 && Switchoff_Lock1 == 0)
				{
					FEN_ON;					            // �򿪷�բ�̵���
					OUT_COM_ON;		              // �����������
					yx_cnt = 0;
					state = 1;
					CtrlTimeDelay = 20;        //20ms
					CtrlTimeout = 0;
					CtrlMeasureCnt = 0;
					CtrlMeasureFlag = 0x02;
					KZQMeasureData.para.ProtectFlag2.bit.open_flag = 1; //��բ��־
				}
				else
				{
					*switch_flag = 0;
		      state = 0;
				}
				break;
			case 1:								
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					state = 2;
					CtrlTimeDelay = CONTROL_OFF_DELAY;  //50ms				
					CtrlTimeout = 0;
				}
				break;
			case 2:								
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					FEN_OFF;				// �رշ�բ�̵���
					state = 3;
					CtrlTimeDelay = 40;	//40ms			
					CtrlTimeout = 0;
					yx_cnt = 0;
				}
				else
				{
					if(HW_IN2 != 0 && FW_IN1 != 0 )	// ��·��λ���ѿ�
					{
						yx_cnt++;
						if(yx_cnt > 20)
						{	
							FEN_OFF;				// �رշ�բ�̵���
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
			case 3:			// �жϲ����Ƿ�ɹ�
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					if( FW_IN1 == 0 && HW_IN2 == 1 )// ��·����λ
					{
						KZQMeasureData.para.ProtectFlag2.bit.open_err = 0;
						KZQMeasureData.para.DLQflag.bit.breaker = 2;	// ��բ״̬
					}
					else if( HW_IN2 == 0 && FW_IN1 == 1 )	// ��·����λ,��բ��Ȧͨ
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
						if(Record_off.para.type == TYPE_ACT_PAD_OFF) //���ط�բ
							RecordSOE_Pad(SOE_ADDR_ACT_PAD_OFF_pad,KZQMeasureData.para.ProtectFlag2.bit.open_err^0x01);  //open_errȡ��
						else if(Record_off.para.type == TYPE_ACT_FAR_OFF) //Զ����բ
							RecordSOE_Pad(SOE_ADDR_ACT_FAR_OFF_pad,KZQMeasureData.para.ProtectFlag2.bit.open_err^0x01);  //open_errȡ��

						if(KZQMeasureData.para.ProtectFlag2.bit.open_err == 0) Record_off.para.type |= 0x80;
						
						KZQMeasureData.para.RequestFlag1.bit.record_off = 1;	// ��λ������բ��¼
				  }
				}
				break;
			default:
				break;
		}
		return 2;
	}
	else if(*switch_flag == 3)					// ������բ����	
	{
		switch(state)
		{
			case 0:								// �򿪼̵���
				if(Switchoff_Lock == 0 && Switchoff_Lock1 == 0)
				{
					FEN_ON;					// �򿪷�բ�̵���
					OUT_COM_ON;		            // �����������
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
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					state = 2;
					CtrlTimeDelay = CONTROL_OFF_DELAY;  //50ms				
					CtrlTimeout = 0;
				}
				break;
			case 2:							
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{		
					FEN_OFF;				// �رշ�բ�̵���
					state = 3;
					CtrlTimeDelay = 40; //40ms				
					CtrlTimeout = 0;
					yx_cnt = 0;
				}
				else
				{	
					if(HW_IN2 != 0 && FW_IN1 != 0 )	// ��·��λ���ѿ�
					{
						yx_cnt++;
						if(yx_cnt > 20)
						{	
							FEN_OFF;				// �رշ�բ�̵���
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
			case 3:			// �жϲ����Ƿ�ɹ�
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					if(HW_IN2 != 0 && FW_IN1 == 0)// ��·����λ
					{
						KZQMeasureData.para.ProtectFlag2.bit.open_err = 0;
						KZQMeasureData.para.DLQflag.bit.breaker = 2;	// ��բ״̬
						if(ReOnFlag) ProtectFlag = 0;	
					}
					else if(HW_IN2 == 0 && FW_IN1 == 1)	// ��·����λ,��բ��Ȧͨ
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
					
					KZQMeasureData.para.RequestFlag1.bit.record_protect = 1;	// ��λ������¼
				}
				break;
			default:
				break;
		}
		return 3;
	}
	else if(*switch_flag == 4)					// �غ�բ������ȷ���ޱ���ʱִ��
	{
		switch(state)
		{
			case 0:				   //ͬ�ں�բ������
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
      case 1:                  //ͬ�ں�բ
			  if(abs(MeasureData.Protect_UA_val- MeasureData.Protect_UB_val)< 1440 && 
					 MeasureData.Display_UB1_val< 3000 && abs(MeasureData.freq_cpu1 - MeasureData.freq_cpu2) < 200)
		    {
          state = 2;
        }
				else
				{
          state = 1;
				}
        if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
				  state = 5;
				}
				break;
			case 2:								  // �򿪼̵���
				if(Switchon_Lock == 0 && Switchon_Lock1 == 0 && Switchon_Lock2 == 0)
				{
					ReOnLockCnt = 0;
					HE_ON;					// �򿪺�բ�̵���
					OUT_COM_ON;		        // �����������
					state = 3;
					CtrlTimeDelay = 20;	//20ms			
					CtrlTimeout = 0;
					CtrlMeasureCnt = 0;
					CtrlMeasureFlag = 0x01;
					KZQMeasureData.para.ProtectFlag2.bit.reon_act = 1;// �غ�բ������־
				}
			  else
				{
					*switch_flag = 0;
		      state = 0;
				}
				break;
			case 3:								
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					state = 4;
					CtrlTimeDelay = CONTROL_ON_DELAY;  //50ms				
					CtrlTimeout = 0;
				}
				break;
			case 4:								
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					HE_OFF;				// �رպ�բ�̵���
					state = 5;
					CtrlTimeDelay = 40;	//40ms				
					CtrlTimeout = 0;
					yx_cnt = 0;
				}
				else
				{
					if(FW_IN1 != 0 && HW_IN2 != 0 )	// ��·��λ���ѿ�
					{
					    yx_cnt++;
						if(yx_cnt > 20 )
						{
							HE_OFF;				// �رպ�բ�̵���
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
			case 5:			// �жϲ����Ƿ�ɹ�
				if(CtrlTimeout == 1)// ��ʱʱ�䵽
				{
					if(HW_IN2 == 0 && FW_IN1 == 1)	// ��·����λ
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 0;
						KZQMeasureData.para.DLQflag.bit.breaker = 1;		// ��բ״̬
					}
					else if(HW_IN2 == 1 && FW_IN1 == 0 )// ��·����λ,��բ��Ȧͨ���Ѵ���
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 1;
					}
					else
					{
						KZQMeasureData.para.ProtectFlag2.bit.close_err = 1;
					}
					*switch_flag = 0;
					state = 0;
					if(Record_on.para.type == TYPE_ACT_REON) //�����غ�բ
						RecordSOE_Pad(SOE_ADDR_REON_pad,KZQMeasureData.para.ProtectFlag2.bit.close_err^0x01);  //close_errȡ��
					
					if(KZQMeasureData.para.ProtectFlag2.bit.close_err == 0) Record_on.para.type |= 0x80;
					
					KZQMeasureData.para.RequestFlag1.bit.record_on = 1;	  // ��λ������բ��¼
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
				OUT_COM_ON;		        // �����������		
				StoreTimeDelay = 18000;     //����ʱ��18S
				StoreTimeout = 0;
				state = 2;
			}
			break;
		case 2:							
			if(StoreTimeout == 1)
			{
				KZCN_OFF;			
				StoreTimeDelay = 1000; //�ر�ʱ��1S
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
