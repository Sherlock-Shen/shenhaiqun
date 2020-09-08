//###########################################################################
//
// È«¾ÖÎÄ¼þ
//
// STM32F407ZET6   
//

//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  19.07| 20 JULY 2019 | S.H.Q. | 
//###########################################################################

#include "global.h"
#include "flash.h"
#include "2812_FFT.h"
#include "ATT7022.h"

u16 ExtRamTest = 0;	// ÍâÀ©ÄÚ´æÕýÈ·ÐÔ¼ì²â
u16 IntRamTest = 0;	// ÄÚ²¿ÄÚ´æÕýÈ·ÐÔ¼ì²â

u8 SystemParaSum = 0;			// ÏµÍ³²ÎÊýÐ£ÑéºÍ
u8 ProtectParaSum = 0;		// ±£»¤²ÎÊýÐ£ÑéºÍ
u8 AlarmParaSum = 0;			// ±¨¾¯²ÎÊýÐ£ÑéºÍ
u8 AutoswitchParaSum = 0;	// ×Ô¶¯½âÁÐ²ÎÊýÐ£ÑéºÍ
u8 MeasureParaSum = 0;    // ²âÁ¿²ÎÊýÐ£ÑéºÍ
u8 CorrectParaSum = 0;    // Ð£±í²ÎÊýÐ£ÑéºÍ
u8 Measure101_ParaSum = 0;// 101¹æÔ¼²ÎÊýÐ£ÑéºÍ

u8 ProtectParaChecked = 0;		// ±£»¤²ÎÊý¼ì²éÃ¿ÌìÖ»¶ÁÈ¡flashÒ»´Î£¬Áãµã¸´Î»
u8 SystemParaChecked = 0;		  // ÏµÍ³²ÎÊý¼ì²éÃ¿ÌìÖ»¶ÁÈ¡flashÒ»´Î£¬Áãµã¸´Î»
u8 AlarmParaChecked = 0;      // ±¨¾¯²ÎÊý¼ì²éÃ¿ÌìÖ»¶ÁÈ¡flashÒ»´Î£¬Áãµã¸´Î»
u8 AutoswitchParaChecked = 0; // ×Ô¶¯½âÁÐ²ÎÊý¼ì²éÃ¿ÌìÖ»¶ÁÈ¡flashÒ»´Î£¬Áãµã¸´Î»
u8 CorrectParaChecked = 0;    // Ð£±í²ÎÊý¼ì²éÃ¿ÌìÖ»¶ÁÈ¡flashÒ»´Î£¬Áãµã¸´Î»
u8 MeasureParaChecked = 0;    // ²âÁ¿ÏµÊý¼ì²éÃ¿ÌìÖ»¶ÁÈ¡flashÒ»´Î£¬Áãµã¸´Î»
u8 Measure101ParaChecked = 0;	// 101¹æÔ¼²ÎÊý¼ì²éÃ¿ÌìÖ»¶ÁÈ¡flashÒ»´Î£¬Áãµã¸´Î»
u8 PeriodMode[24] = {0};			// Ê±¶ÎÄ£Ê½£¬ÓÃÓÚµçÁ¿·ÖÊ±Í³¼Æ

// ÄÚ´æ×Ô¼ì£¬Íâ²¿ÄÚ´æ¹ÊÕÏ·µ»Ø0x01£¬ÄÚ²¿ÄÚ´æ¹ÊÕÏ·µ»Ø0x02
u8 RamTest(void)
{
	u16 ramtemp = 0;
	ExtRamTest = 0x5555;
	ramtemp = ExtRamTest;
	if(ramtemp != 0x5555)
	{
		KZQMeasureData.para.SelfCheck.bit.ext_ram = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0100;
		return 0x01;
	}
	ExtRamTest = 0xAAAA;
	ramtemp = ExtRamTest;
	if(ramtemp != 0xAAAA)
	{
		KZQMeasureData.para.SelfCheck.bit.ext_ram = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0100;
		return 0x01;
	}
	ExtRamTest = 0xFFFF;
	ramtemp = ExtRamTest;
	if(ramtemp != 0xFFFF)
	{
		KZQMeasureData.para.SelfCheck.bit.ext_ram = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0100;
		return 0x01;
	}
	ExtRamTest = 0;
	ramtemp = ExtRamTest;
	if(ramtemp != 0)
	{
		KZQMeasureData.para.SelfCheck.bit.ext_ram = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0100;
		return 0x01;
	}
	KZQMeasureData.para.SelfCheck.bit.ext_ram = 0;

	IntRamTest = 0x5555;
	ramtemp = IntRamTest;
	if(ramtemp != 0x5555)
	{
		KZQMeasureData.para.SelfCheck.bit.int_ram = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0200;
		return 0x02;
	}
	IntRamTest = 0xAAAA;
	ramtemp = IntRamTest;
	if(ramtemp != 0xAAAA)
	{
		KZQMeasureData.para.SelfCheck.bit.int_ram = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0200;
		return 0x02;
	}
	IntRamTest = 0xFFFF;
	ramtemp = IntRamTest;
	if(ramtemp != 0xFFFF)
	{
		KZQMeasureData.para.SelfCheck.bit.int_ram = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0200;
		return 0x02;
	}
	IntRamTest = 0;
	ramtemp = IntRamTest;
	if(ramtemp != 0)
	{
		KZQMeasureData.para.SelfCheck.bit.int_ram = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0200;
		return 0x02;
	}
	KZQMeasureData.para.SelfCheck.bit.int_ram = 0;
	return 0;		
}

// ¼ÇÂ¼ºÏÕ¢²Ù×÷ÐÅÏ¢

void RecordSwitchOn(u8 switch_type)
{
	Record_on.para.year = TimeNow.year;
	Record_on.para.month = TimeNow.month;
	Record_on.para.date = TimeNow.date;
	Record_on.para.hour = TimeNow.hour;
	Record_on.para.min = TimeNow.minute;
	Record_on.para.sec = TimeNow.second;
	Record_on.para.msec = TimeNow.msec;
	Record_on.para.type = switch_type;
	Record_on.para.setvalue = 0;
	Record_on.para.setvaluetime = 0;
	Record_on.para.Protect_UA_val = MeasureData.Protect_UA_val;
	Record_on.para.Protect_UB_val = MeasureData.Protect_UB_val;
	Record_on.para.Protect_UC_val = MeasureData.Protect_UC_val;
	Record_on.para.Protect_IA_val = MeasureData.Protect_IA_val;
	Record_on.para.Protect_IB_val = MeasureData.Protect_IB_val;
	Record_on.para.Protect_IC_val = MeasureData.Protect_IC_val;
	Record_on.para.P = (u16)MeasureData.P_val; 
	Record_on.para.Q = MeasureData.Q_val;
	Record_on.para.COS = MeasureData.cosALL;
	Record_on.para.Protect_U0_val = MeasureData.Protect_U0_val;
	Record_on.para.Protect_I0_val = MeasureData.Protect_I0_val;
	Record_on.para.degU0 = MeasureData.Display_degU0_val;
	Record_on.para.degI0 = MeasureData.Display_degI0_val;
}

// ¼ÇÂ¼·ÖÕ¢²Ù×÷ÐÅÏ¢

void RecordSwitchOff(u8 switch_type)
{
	Record_off.para.year = TimeNow.year;
	Record_off.para.month = TimeNow.month;
	Record_off.para.date = TimeNow.date;
	Record_off.para.hour = TimeNow.hour;
	Record_off.para.min = TimeNow.minute;
	Record_off.para.sec = TimeNow.second;
	Record_off.para.msec = TimeNow.msec;
	Record_off.para.type = switch_type;
	Record_off.para.setvalue = 0;
	Record_off.para.setvaluetime = 0;
	Record_off.para.Protect_UA_val = MeasureData.Protect_UA_val;
	Record_off.para.Protect_UB_val = MeasureData.Protect_UB_val;
	Record_off.para.Protect_UC_val = MeasureData.Protect_UC_val;
	Record_off.para.Protect_IA_val = MeasureData.Protect_IA_val;
	Record_off.para.Protect_IB_val = MeasureData.Protect_IB_val;
	Record_off.para.Protect_IC_val = MeasureData.Protect_IC_val;
	Record_off.para.P = (u16)MeasureData.P_val; 
	Record_off.para.Q = MeasureData.Q_val;
	Record_off.para.COS = MeasureData.cosALL;
	Record_off.para.Protect_U0_val = MeasureData.Protect_U0_val;
	Record_off.para.Protect_I0_val = MeasureData.Protect_I0_val;
	Record_off.para.degU0 = MeasureData.Display_degU0_val;
  Record_off.para.degI0 = MeasureData.Display_degI0_val;
}

// ¼ÇÂ¼±£»¤Æô¶¯ÐÅÏ¢

void RecordProtectStart(u8 protect_type)
{
	if((ProtectRecord_flag & 0x0010) == 0)	// ±£»¤1Çø¼ÇÂ¼Îª¿Õ£¬¿ÉÒÔÐ´Èë
	{
		ProtectRecord_flag &= 0x0030;
		ProtectRecord_flag |= 0x0011;
		Record_protect1.para.year = TimeNow.year;
		Record_protect1.para.month = TimeNow.month;
		Record_protect1.para.date = TimeNow.date;
		Record_protect1.para.hour = TimeNow.hour;
		Record_protect1.para.min = TimeNow.minute;
		Record_protect1.para.sec = TimeNow.second;
		Record_protect1.para.msec = TimeNow.msec;
		Record_protect1.para.type = protect_type;

		Record_protect1.para.Protect_IA_val = MeasureData.Protect_IA_val;
		Record_protect1.para.Protect_IB_val = MeasureData.Protect_IB_val;
		Record_protect1.para.Protect_IC_val = MeasureData.Protect_IC_val;

		switch(protect_type)
		{
      case TYPE_PROTECT_ZERO_MAX_VOLTAGE:	// ÁãÐò¹ýÑ¹±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.zero_max_voltage;
				Record_protect1.para.setvaluetime = ProtectSet.para.zero_max_voltage_time;
				break;
      case TYPE_PROTECT_MAX_VOLTAGE:	// ¹ýÑ¹±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.max_voltage;
				Record_protect1.para.setvaluetime = ProtectSet.para.max_voltage_time;
				break;
      case TYPE_PROTECT_MAX_FREQ:	// ¸ßÆµ±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.max_freq;
				Record_protect1.para.setvaluetime = ProtectSet.para.max_freq_time;
				break;
			case TYPE_PROTECT_LOW_FREQ:	// µÍÆµ±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.low_freq;
				Record_protect1.para.setvaluetime = ProtectSet.para.low_freq_time;
				break;
			case TYPE_PROTECT_ZERO_MAX_CURRENT:	// ÁãÐò¹ýÁ÷±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.zero_max_current;
				Record_protect1.para.setvaluetime = ProtectSet.para.zero_max_current_time;
				if(ProtectSet.para.zero_max_current > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect1.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect1.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect1.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
      case TYPE_PROTECT_PHASE_LOSS:	// È±Ïà±£»¤
				Record_protect1.para.setvaluetime = ProtectSet.para.phaseloss_protect_time;
				break;
      case TYPE_PROTECT_GND:	// ½ÓµØ±£»¤
				Record_protect1.para.DYCFZC = dycfzc;
	      Record_protect1.para.JDXB = jdxb; 
	      Record_protect1.para.JDXZ = jdxz;
				break;
			case TYPE_PROTECT_FAST_OFF:	// ËÙ¶Ï±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.fast_off;
				Record_protect1.para.setvaluetime = ProtectSet.para.fast_off_time;
				if(ProtectSet.para.fast_off > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect1.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect1.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect1.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
			case TYPE_PROTECT_DELAY_FAST_OFF:	// ÑÓÊ±ËÙ¶Ï±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.delay_fast_off;
				Record_protect1.para.setvaluetime = ProtectSet.para.delay_fast_off_time;
				if(ProtectSet.para.delay_fast_off > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect1.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect1.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect1.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
			case TYPE_PROTECT_MAX_CURRENT:	// ¹ýÁ÷±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.max_current;
				Record_protect1.para.setvaluetime = ProtectSet.para.max_current_time;
				if(ProtectSet.para.max_current > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect1.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect1.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect1.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
			case TYPE_PROTECT_MAX_CURRENT_AFTER:	// ¹ýÁ÷ºó¼ÓËÙ±£»¤
				Record_protect1.para.setvalue = ProtectSet.para.max_current_after;
				Record_protect1.para.setvaluetime = ProtectSet.para.max_current_after_time;
				if(ProtectSet.para.max_current_after > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect1.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect1.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect1.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
			case TYPE_PROTECT_REON:	// ÖØºÏÕ¢
				Record_protect1.para.setvaluetime = ProtectSet.para.once_reon_time;
			    if(ProtectSet.para.fast_off > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect1.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect1.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect1.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
		}
		Record_protect1.para.Protect_UA_val = MeasureData.Protect_UA_val;
		Record_protect1.para.Protect_UB_val = MeasureData.Protect_UB_val;
		Record_protect1.para.Protect_UC_val = MeasureData.Protect_UC_val;
		Record_protect1.para.P = (u16)MeasureData.P_val; 
		Record_protect1.para.Q = MeasureData.Q_val;
		Record_protect1.para.COS = MeasureData.cosALL;
		Record_protect1.para.Protect_U0_val = MeasureData.Protect_U0_val;
		if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
	    Record_protect1.para.Protect_I0_val = MeasureData.Protect_I0_val/10;
	  else 
		  Record_protect1.para.Protect_I0_val = MeasureData.Protect_10I0_val;
		Record_protect1.para.degU0 = MeasureData.Display_degU0_val;
    Record_protect1.para.degI0 = MeasureData.Display_degI0_val;
    Record_protect1.para.Freq	= MeasureData.freq;
	}
	else	// ±£»¤1Çø¼ÇÂ¼Âú£¬Ö±½ÓÐ´Èë±£»¤2Çø
	{
		ProtectRecord_flag &= 0x0030;
		ProtectRecord_flag |= 0x0022;
		Record_protect2.para.year = TimeNow.year;
		Record_protect2.para.month = TimeNow.month;
		Record_protect2.para.date = TimeNow.date;
		Record_protect2.para.hour = TimeNow.hour;
		Record_protect2.para.min = TimeNow.minute;
		Record_protect2.para.sec = TimeNow.second;
		Record_protect2.para.msec = TimeNow.msec;
		Record_protect2.para.type = protect_type;

		Record_protect2.para.Protect_IA_val = MeasureData.Protect_IA_val;
		Record_protect2.para.Protect_IB_val = MeasureData.Protect_IB_val;
		Record_protect2.para.Protect_IC_val = MeasureData.Protect_IC_val;

		switch(protect_type)
		{
		  case TYPE_PROTECT_ZERO_MAX_VOLTAGE:	// ÁãÐò¹ýÑ¹±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.zero_max_voltage;
				Record_protect2.para.setvaluetime = ProtectSet.para.zero_max_voltage_time;
				break;
			case TYPE_PROTECT_MAX_VOLTAGE:	// ¹ýÑ¹±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.max_voltage;
				Record_protect2.para.setvaluetime = ProtectSet.para.max_voltage_time;
				break;
      case TYPE_PROTECT_MAX_FREQ:	// ¸ßÆµ±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.max_freq;
				Record_protect2.para.setvaluetime = ProtectSet.para.max_freq_time;
				break;
			case TYPE_PROTECT_LOW_FREQ:	// µÍÆµ±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.low_freq;
				Record_protect2.para.setvaluetime = ProtectSet.para.low_freq_time;
				break;
			case TYPE_PROTECT_ZERO_MAX_CURRENT:	// ÁãÐò¹ýÁ÷±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.zero_max_current;
				Record_protect2.para.setvaluetime = ProtectSet.para.zero_max_current_time;
				if(ProtectSet.para.zero_max_current > 2000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect2.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect2.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect2.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
            case TYPE_PROTECT_PHASE_LOSS:	// È±Ïà±£»¤
				Record_protect2.para.setvaluetime = ProtectSet.para.phaseloss_protect_time;
				break;
      case TYPE_PROTECT_GND:	// ½ÓµØ±£»¤
				Record_protect2.para.DYCFZC = dycfzc;
	      Record_protect2.para.JDXB = jdxb; 
	      Record_protect2.para.JDXZ = jdxz;
				break;
			case TYPE_PROTECT_FAST_OFF:	// ËÙ¶Ï±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.fast_off;
				Record_protect2.para.setvaluetime = ProtectSet.para.fast_off_time;
				if(ProtectSet.para.fast_off > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect2.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect2.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect2.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
			case TYPE_PROTECT_DELAY_FAST_OFF:	// ÑÓÊ±ËÙ¶Ï±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.delay_fast_off;
				Record_protect2.para.setvaluetime = ProtectSet.para.delay_fast_off_time;
				if(ProtectSet.para.delay_fast_off > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect2.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect2.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect2.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
			case TYPE_PROTECT_MAX_CURRENT:	// ¹ýÁ÷±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.max_current;
				Record_protect2.para.setvaluetime = ProtectSet.para.max_current_time;
				if(ProtectSet.para.max_current > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect2.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect2.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect2.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
			case TYPE_PROTECT_MAX_CURRENT_AFTER:	// ¹ýÁ÷ºó¼ÓËÙ±£»¤
				Record_protect2.para.setvalue = ProtectSet.para.max_current_after;
				Record_protect2.para.setvaluetime = ProtectSet.para.max_current_after_time;
				if(ProtectSet.para.max_current_after > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect2.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect2.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect2.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
			case TYPE_PROTECT_REON:	// ÖØºÏÕ¢
				Record_protect2.para.setvaluetime = ProtectSet.para.once_reon_time;
			    if(ProtectSet.para.fast_off > 1000)	// ¸ßÓÚ¶î¶¨µçÁ÷£¬È¡10±¶µçÁ÷
				{
					Record_protect2.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
					Record_protect2.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
					Record_protect2.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
				}
				break;
		}
		Record_protect2.para.Protect_UA_val = MeasureData.Protect_UA_val;
		Record_protect2.para.Protect_UB_val = MeasureData.Protect_UB_val;
		Record_protect2.para.Protect_UC_val = MeasureData.Protect_UC_val;
		Record_protect2.para.P = (u16)MeasureData.P_val; 
		Record_protect2.para.Q = MeasureData.Q_val;
		Record_protect2.para.COS = MeasureData.cosALL;
		Record_protect2.para.Protect_U0_val = MeasureData.Protect_U0_val;
		if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
	    Record_protect2.para.Protect_I0_val = MeasureData.Protect_I0_val/10;
	  else 
		  Record_protect2.para.Protect_I0_val = MeasureData.Protect_10I0_val;
		Record_protect2.para.degU0 = MeasureData.Display_degU0_val;
    Record_protect2.para.degI0 = MeasureData.Display_degI0_val;
    Record_protect1.para.Freq	= MeasureData.freq;		
	}
}
 
//±¨¾¯¼ÇÂ¼
void RecordAlarm(u8 type,s16 set_value,s16 set_delay)
{
  Record_alarm.para.year = TimeNow.year;    //0
	Record_alarm.para.month = TimeNow.month;
	Record_alarm.para.date = TimeNow.date;
	Record_alarm.para.hour = TimeNow.hour;    //3
	Record_alarm.para.min = TimeNow.minute;
	Record_alarm.para.sec = TimeNow.second;
	Record_alarm.para.msec = TimeNow.msec;     //6
  Record_alarm.para.type = type;
	Record_alarm.para.setvalue = set_value;    //8
	Record_alarm.para.setvaluetime = set_delay;              //9
//  Record_alarm.para.level = 1;
//  Record_alarm.para.delay_date = 0;
//  Record_alarm.para.delay_hour = 0;
//	Record_alarm.para.delay_min = 0;
//	Record_alarm.para.delay_sec = 0;
//	Record_alarm.para.electric_in = 0;
//	Record_alarm.para.electric_out = 0;
	Record_alarm.para.Protect_UA_val = MeasureData.Protect_UA_val;
	Record_alarm.para.Protect_UB_val = MeasureData.Protect_UB_val;
	Record_alarm.para.Protect_UC_val = MeasureData.Protect_UC_val;

	if(MeasureData.Protect_IA10_val > 1000)
	{
		Record_alarm.para.Protect_IA_val = MeasureData.Protect_IA10_val*10;
		Record_alarm.para.Protect_IB_val = MeasureData.Protect_IB10_val*10;
		Record_alarm.para.Protect_IC_val = MeasureData.Protect_IC10_val*10;
	}
	else
	{
		Record_alarm.para.Protect_IA_val = MeasureData.Protect_IA_val;
		Record_alarm.para.Protect_IB_val = MeasureData.Protect_IB_val;
		Record_alarm.para.Protect_IC_val = MeasureData.Protect_IC_val;	
	}
  Record_alarm.para.P = (u16)MeasureData.P_val;
	Record_alarm.para.Q = MeasureData.Q_val;
	Record_alarm.para.COS = MeasureData.cosALL;
	Record_alarm.para.Protect_U0_val = MeasureData.Protect_U0_val;
	if(MeasureData.Protect_I0_val < 21000 && MeasureData.Protect_10I0_val < 2100)
	  Record_alarm.para.Protect_I0_val = MeasureData.Protect_I0_val/10;
	else 
		Record_alarm.para.Protect_I0_val = MeasureData.Protect_10I0_val;
	Record_alarm.para.degU0 = MeasureData.Display_degU0_val;
	Record_alarm.para.degI0 = MeasureData.Display_degI0_val;
	Record_alarm.para.JDXZ = jdxz;
	Record_alarm.para.DYCFZC = dycfzc;
	Record_alarm.para.JDXB = jdxb;
	Record_alarm.para.Uqrate =1;
	Record_alarm.para.Uharmonic = FXBZUA;
	Record_alarm.para.Iharmonic = FXBZIA;
	AlarmRecord_flag = 1;
}

void SaveRecordAlarm(void)
{
	u8 buf[100],i;
	u8 bufread[10];	
	u16 recordorder = 0;
	CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),2);
	recordorder = bufread[0] | (bufread[1]<<8);
	if(recordorder == 0xFFFF)recordorder = 0;
	if(recordorder == DLQ_EventRecord_Order[DLQ_EventRecord_Index])
	{
		recordorder = DLQ_EventRecord_Order[DLQ_EventRecord_Index] +1;
		if(recordorder > 60000 || recordorder == 0)recordorder = 1;
		if(DLQ_EventRecord_Order[DLQ_EventRecord_Index] != 0)
		{
			if(DLQ_EventRecord_Index < (FLASH_EventRecord_MAX-1))
				DLQ_EventRecord_Index++;
			else
				DLQ_EventRecord_Index = 0;				
		}
		DLQ_EventRecord_Order[DLQ_EventRecord_Index] = recordorder;
	}
	buf[0] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] & 0x00FF;
	buf[1] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] >> 8;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 4;
	for(i=0;i<29;i++)
	{
		buf[(i*2)+5] = Record_alarm.word[i]&0x00FF;
		buf[(i*2)+6] = Record_alarm.word[i]>>8;
	}
	CS2BZ=0;
	SPI_Flash_Write(buf,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),63);// Ð´ÈëÐòºÅ
}

// À¡Ïß×Ô¶¯»¯¼ÇÂ¼ÐÅÏ¢
void RecordAutofeed(u8 switch_type)
{
	Record_autofeed.para.year = TimeNow.year;
	Record_autofeed.para.month = TimeNow.month;
	Record_autofeed.para.date = TimeNow.date;
	Record_autofeed.para.hour = TimeNow.hour;
	Record_autofeed.para.min = TimeNow.minute;
	Record_autofeed.para.sec = TimeNow.second;
	Record_autofeed.para.msec = TimeNow.msec;
	Record_autofeed.para.type = switch_type;
	Record_autofeed.para.setvalue = 0;
	Record_autofeed.para.setvaluetime = 0;
	Record_autofeed.para.Protect_UA_val = MeasureData.Protect_UA_val;
	Record_autofeed.para.Protect_UB_val = MeasureData.Protect_UB_val;
	Record_autofeed.para.Protect_UC_val = MeasureData.Protect_UC_val;
	Record_autofeed.para.Protect_IA_val = MeasureData.Protect_IA_val;
	Record_autofeed.para.Protect_IB_val = MeasureData.Protect_IB_val;
	Record_autofeed.para.Protect_IC_val = MeasureData.Protect_IC_val;
	Record_autofeed.para.P = (u16)MeasureData.P_val; 
	Record_autofeed.para.Q = MeasureData.Q_val;
	Record_autofeed.para.COS = MeasureData.cosALL;
	Record_autofeed.para.Protect_U0_val = MeasureData.Protect_U0_val;
	Record_autofeed.para.Protect_I0_val = MeasureData.Protect_I0_val;
	Record_autofeed.para.degU0 = MeasureData.Display_degU0_val;
  Record_autofeed.para.degI0 = MeasureData.Display_degI0_val;
}

// ÏµÍ³²ÎÊý±È½Ï
u8 SystemParaCompare(void)
{
	u8 buffer[sizeof(struct SYSTEM_PARA)] = {0};
	u8 num = sizeof(struct SYSTEM_PARA);
	u8 i = 0;
	
  SPI_Flash_Read(buffer,FLASH_SYSTEMSET_ADDR,num);
	for(i=0;i<(sizeof(struct SYSTEM_PARA)/2);i++)
	{
		if(SystemSet.word[i] != (buffer[i*2] | (buffer[i*2+1]<<8)))
		  return 0;
	}
	return 0x0D;
}

// ±£»¤²ÎÊý±È½Ï
u8 ProtectParaCompare(void)
{
	u8 buffer[sizeof(struct PROTECT_PARA)] = {0};
	u8 num = sizeof(struct PROTECT_PARA);
	u8 i = 0;
	
  SPI_Flash_Read(buffer,FLASH_PROTECTSET_ADDR,num);
	for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
	{
		if(ProtectSet.word[i] != (buffer[i*2] | (buffer[i*2+1]<<8)))
		  return 0;
	}
	return 0x0D;
}

// ±¨¾¯²ÎÊý±È½Ï
u8 AlarmParaCompare(void)
{
	u8 buffer[sizeof(struct ALARM_PARA)] = {0};
	u8 num = sizeof(struct ALARM_PARA);
	u8 i = 0;
	
  SPI_Flash_Read(buffer,FLASH_ALARMSET_ADDR,num);
	for(i=0;i<(sizeof(struct ALARM_PARA)/2);i++)
	{
		if(AlarmSet.word[i] != (buffer[i*2] | (buffer[i*2+1]<<8)))
		  return 0;
	}
	return 0x0D;
}

// À¡Ïß×Ô¶¯»¯²ÎÊý±È½Ï
u8 AutoswitchParaCompare(void)
{
	u8 buffer[sizeof(struct AUTO_SWITCHOFF)] = {0};
	u8 num = sizeof(struct AUTO_SWITCHOFF);
	u8 i = 0;
	
  SPI_Flash_Read(buffer,FLASH_AUTOFFSET_ADDR,num);
	for(i=0;i<(sizeof(struct AUTO_SWITCHOFF)/2);i++)
	{
		if(AutoswitchSet.word[i] != (buffer[i*2] | (buffer[i*2+1]<<8)))
		  return 0;
	}
	return 0x0D;
}

// ²âÁ¿ÏµÊý±È½Ï
u8 ModulusParaCompare(void)
{
	u8 buffer[sizeof(struct Modulus)] = {0};
	u8 num = sizeof(struct Modulus);
	u8 i = 0;
	
  SPI_Flash_Read(buffer,FLASH_MEASURECF_ADDR,num);
	for(i=0;i<(sizeof(struct Modulus)/2);i++)
	{
		if(ProtectModulus.word[i] != (buffer[i*2] | (buffer[i*2+1]<<8)))
		  return 0;
	}
	return 0x0D;
}

// ¶ÁÈ¡ÏµÍ³²ÎÊý
u8 ReadSystemPara(void)
{
	u8 buffer[sizeof(struct SYSTEM_PARA)+5] = {0};
	u8 i = 0,sum = 0,err = 0,returnvalue = 0x0D;
  u8 num = sizeof(struct SYSTEM_PARA);
  CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_SYSTEMSET_ADDR,num+5);
	SystemParaSum = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		err = 0;
	}
	else	// ²ÎÊý²»Õý³£
	{
		err = 1;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(SystemParaSum != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		err = 1;
		returnvalue = 0x01;
	}

	for(i=0;i<(sizeof(struct SYSTEM_PARA)/2);i++)
	{
		SystemSet.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
	}
	SystemSet.para.PT_Hvalue = 100;//2019.12.06
	//SystemSet.para.CT_Hvalue = 600;//2019.12.06
	Para_peak1_period_S = SystemSet.para.peak_valley_period[0];
	Para_peak1_period_E = SystemSet.para.peak_valley_period[1];
  Para_peak2_period_S = SystemSet.para.peak_valley_period[2];
	Para_peak2_period_E = SystemSet.para.peak_valley_period[3];
	Para_valley_period_S = SystemSet.para.peak_valley_period[4];
	Para_valley_period_E = SystemSet.para.peak_valley_period[5];

	for(i=0;i<24;i++)PeriodMode[i] = 0;
	for(i=0;i<24;i++)
	{
		if(Para_peak1_period_S < Para_peak1_period_E)
		{
			if(i>=Para_peak1_period_S && i<Para_peak1_period_E)
			{
				PeriodMode[i] = 1;
				continue;
			}
		}
		else if(Para_peak1_period_S > Para_peak1_period_E)
		{
			if(i>=Para_peak1_period_S || i<Para_peak1_period_E)
			{
				PeriodMode[i] = 1;
				continue;
			}
		}
		if(Para_peak2_period_S < Para_peak2_period_E)
		{
			if(i>=Para_peak2_period_S && i<Para_peak2_period_E)
			{
				PeriodMode[i] = 1;
				continue;
			}
		}
		else if(Para_peak2_period_S > Para_peak2_period_E)
		{
			if(i>=Para_peak1_period_S || i<Para_peak1_period_E)
			{
				PeriodMode[i] = 1;
				continue;
			}
		}
		if(Para_valley_period_S < Para_valley_period_E)
		{
			if(i>=Para_valley_period_S && i<Para_valley_period_E)
			{
				PeriodMode[i] = 2;
				continue;
			}
		}
		else if(Para_valley_period_S > Para_valley_period_E)
		{
			if(i>=Para_valley_period_S || i<Para_valley_period_E)
			{
				PeriodMode[i] = 2;
				continue;
			}
		}
		PeriodMode[i] = 3;
	}
	if(err != 0)	// ²ÎÊý´íÎó
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0700;
	}
	else
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 0;
	}
	return returnvalue;
}

// ±£´æÏµÍ³²ÎÊý
void SaveSystemPara(void)
{
	u8 buffer[sizeof(struct SYSTEM_PARA)+5] = {0};
	u8 i = 0,sum = 0;
	u8 num = sizeof(struct SYSTEM_PARA);

	for(i=0;i<(sizeof(struct SYSTEM_PARA)/2);i++)
	{
	  buffer[i*2] = SystemSet.word[i]&0x00FF;
	  buffer[i*2+1] = SystemSet.word[i]>>8;
	}

	Para_peak1_period_S = SystemSet.para.peak_valley_period[0];
	Para_peak1_period_E = SystemSet.para.peak_valley_period[1];
  Para_peak2_period_S = SystemSet.para.peak_valley_period[2];
	Para_peak2_period_E = SystemSet.para.peak_valley_period[3];
	Para_valley_period_S = SystemSet.para.peak_valley_period[4];
	Para_valley_period_E = SystemSet.para.peak_valley_period[5];

	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_SYSTEMSET_ADDR,num+5);
}

// ¶ÁÈ¡ÏµÍ³²ÎÊý±¸·Ý
u8 ReadSystemParaBAK(void)
{
	u8 buffer[sizeof(struct SYSTEM_PARA)+5] = {0};
	u8 i = 0,sum = 0,sum1 = 0;
  u8 num = sizeof(struct SYSTEM_PARA);
  CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_SYSTEMSETBAK_ADDR,num+5);
	
	sum1 = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{

	}
	else	// ²ÎÊý²»Õý³£
	{
		return 0;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(sum1 != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		return 1;
	}
	SystemParaSum = sum&0x00FF;

	for(i=0;i<(sizeof(struct SYSTEM_PARA)/2);i++)
	{
		SystemSet.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
	}
	
	Para_peak1_period_S = SystemSet.para.peak_valley_period[0];
	Para_peak1_period_E = SystemSet.para.peak_valley_period[1];
  Para_peak2_period_S = SystemSet.para.peak_valley_period[2];
	Para_peak2_period_E = SystemSet.para.peak_valley_period[3];
	Para_valley_period_S = SystemSet.para.peak_valley_period[4];
	Para_valley_period_E = SystemSet.para.peak_valley_period[5];

	for(i=0;i<24;i++)PeriodMode[i] = 0;
	for(i=0;i<24;i++)
	{
		if(Para_peak1_period_S < Para_peak1_period_E)
		{
			if(i>=Para_peak1_period_S && i<Para_peak1_period_E)
			{
				PeriodMode[i] = 1;
				continue;
			}
		}
		else if(Para_peak1_period_S > Para_peak1_period_E)
		{
			if(i>=Para_peak1_period_S || i<Para_peak1_period_E)
			{
				PeriodMode[i] = 1;
				continue;
			}
		}
		if(Para_peak2_period_S < Para_peak2_period_E)
		{
			if(i>=Para_peak2_period_S && i<Para_peak2_period_E)
			{
				PeriodMode[i] = 1;
				continue;
			}
		}
		else if(Para_peak2_period_S > Para_peak2_period_E)
		{
			if(i>=Para_peak1_period_S || i<Para_peak1_period_E)
			{
				PeriodMode[i] = 1;
				continue;
			}
		}
		if(Para_valley_period_S < Para_valley_period_E)
		{
			if(i>=Para_valley_period_S && i<Para_valley_period_E)
			{
				PeriodMode[i] = 2;
				continue;
			}
		}
		else if(Para_valley_period_S > Para_valley_period_E)
		{
			if(i>=Para_valley_period_S || i<Para_valley_period_E)
			{
				PeriodMode[i] = 2;
				continue;
			}
		}
		PeriodMode[i] = 3;
	}
	return 0x0D;
}

// ±£´æÏµÍ³²ÎÊý±¸·Ý
void SaveSystemParaBAK(void)
{
	u8 buffer[sizeof(struct SYSTEM_PARA)+5] = {0};
	u8 i = 0,sum = 0;
	u8 num = sizeof(struct SYSTEM_PARA);

	for(i=0;i<(sizeof(struct SYSTEM_PARA)/2);i++)
	{
		buffer[i*2] = SystemSet.word[i]&0x00FF;
	  buffer[i*2+1] = SystemSet.word[i]>>8;
	}
	Para_peak1_period_S = SystemSet.para.peak_valley_period[0];
	Para_peak1_period_E = SystemSet.para.peak_valley_period[1];
  Para_peak2_period_S = SystemSet.para.peak_valley_period[2];
	Para_peak2_period_E = SystemSet.para.peak_valley_period[3];
	Para_valley_period_S = SystemSet.para.peak_valley_period[4];
	Para_valley_period_E = SystemSet.para.peak_valley_period[5];
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_SYSTEMSETBAK_ADDR,num+5);
}

// ¼ì²éÏµÍ³²ÎÊý
void CheckSystemPara(void)
{

	u8 buffer[sizeof(struct SYSTEM_PARA)+5] = {0};
	u8 i = 0,sum = 0;
  u8 num = sizeof(struct SYSTEM_PARA);
	
  for(i=0;i<(sizeof(struct SYSTEM_PARA)/2);i++)
	{
	   buffer[i*2] = SystemSet.word[i]&0x00FF;
	   buffer[i*2+1] = SystemSet.word[i]>>8;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(SystemParaSum != (sum&0x00FF) && SystemParaChecked == 0)
	{
		SystemParaChecked = 1;
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(ReadSystemPara() != 0x0D)	// ¶ÁÏµÍ³ÉèÖÃ²ÎÊý
		{
			if(ReadSystemParaBAK() == 0x0D)	// Õý³£ÏµÍ³²ÎÊý³ö´í£¬±¸·ÝÏµÍ³²ÎÊýÕýÈ·£¬ÔòÖØÐÂ±£´æÕý³£ÏµÍ³²ÎÊý
			{
				SaveSystemPara();
			}
		}	
	}
}

// ¶ÁÈ¡±£»¤²ÎÊý
u8 ReadProtectPara(void)
{
	u8 buffer[sizeof(struct PROTECT_PARA)+5] = {0};
	u8 i = 0,sum = 0,err = 0,returnvalue = 0x0D;
	u8 num = sizeof(struct PROTECT_PARA);
  CS2BZ=0;
  SPI_Flash_Read(buffer,FLASH_PROTECTSET_ADDR,num+5);
	ProtectParaSum = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		err = 0;
	}
	else	// ²ÎÊý²»Õý³£
	{
		err = 1;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(ProtectParaSum != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		err = 1;
		returnvalue = 0x01;
	}
	for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
	{
		ProtectSet.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
	}

	if(err != 0)	// ²ÎÊý´íÎó
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0700;
	}
	else
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 0;
	}
	return returnvalue;
}

// ±£´æ±£»¤²ÎÊý
void SaveProtectPara(void)
{
	u8 buffer[sizeof(struct PROTECT_PARA)+5] = {0};
	u8 i = 0, sum = 0;
	u8 num = sizeof(struct PROTECT_PARA);
	for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
	{
	   buffer[i*2] = ProtectSet.word[i]&0x00FF;
	   buffer[i*2+1] = ProtectSet.word[i]>>8;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
  CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_PROTECTSET_ADDR,num+5);
}
 
// ¶ÁÈ¡±£»¤²ÎÊý±¸·Ý
u8 ReadProtectParaBAK(void)
{
	u8 buffer[sizeof(struct PROTECT_PARA)+5] = {0};
	u8 i = 0,sum = 0,sum1 = 0;
	u8 num = sizeof(struct PROTECT_PARA);
  CS2BZ=0;
  SPI_Flash_Read(buffer,FLASH_PROTECTSETBAK_ADDR,num+5);

	sum1 = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{

	}
	else	// ²ÎÊý²»Õý³£
	{
		return 0;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(sum1 != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		return 1;
	}
	ProtectParaSum = sum&0x00FF;
	for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
	{
		ProtectSet.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
	}
	return 0x0D;
}

// ±£´æ±£»¤²ÎÊý±¸·Ý
void SaveProtectParaBAK(void)
{
	u8 buffer[sizeof(struct PROTECT_PARA)+5] = {0};
	u8 i = 0, sum = 0;
	u8 num = sizeof(struct PROTECT_PARA);
	for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
	{
	   buffer[i*2] = ProtectSet.word[i]&0x00FF;
	   buffer[i*2+1] = ProtectSet.word[i]>>8;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
  CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_PROTECTSETBAK_ADDR,num+5);
}

// ¼ì²é±£»¤²ÎÊý
void CheckProtectPara(void)
{
	u8 buffer[sizeof(struct PROTECT_PARA)+5] = {0};
	u8 i = 0,sum = 0;
	u8 num = sizeof(struct PROTECT_PARA);
	for(i=0;i<(sizeof(struct PROTECT_PARA)/2);i++)
	{
	   buffer[i*2] = ProtectSet.word[i]&0x00FF;
	   buffer[i*2+1] = ProtectSet.word[i]>>8;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(ProtectParaSum != (sum&0x00FF) && ProtectParaChecked == 0)
	{
		ProtectParaChecked = 1;
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(ReadProtectPara() != 0x0D)	// ¶ÁÈ¡±£»¤²ÎÊý
		{
			if(ReadProtectParaBAK() == 0x0D)	// Õý³£±£»¤²ÎÊý³ö´í£¬±¸·Ý±£»¤²ÎÊýÕýÈ·£¬ÔòÖØÐÂ±£´æÕý³£ý
			{
				SaveProtectPara();
			}
		}
	}
}

// ¶ÁÈ¡±¨¾¯²ÎÊý
u8 ReadAlarmPara(void)
{
	u8 buffer[sizeof(struct ALARM_PARA)+5] = {0};
	u8 i = 0, sum = 0,err = 0, returnvalue = 0x0D;
	u8 num = sizeof(struct ALARM_PARA);
	CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_ALARMSET_ADDR,num+5);
	AlarmParaSum = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		err = 0;
	}
	else	// ²ÎÊý²»Õý³£
	{
		err = 1;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(AlarmParaSum != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		err = 1;
		returnvalue = 0x01;
	}
	for(i=0;i<(sizeof(struct ALARM_PARA)/2);i++)
	{
		AlarmSet.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
	}
	if(err != 0)	// ²ÎÊý´íÎó
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0700;
	}
	else
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 0;
	}

	return returnvalue;
}

// ±£´æ±¨¾¯²ÎÊý
void SaveAlarmPara(void)
{
	u8 buffer[sizeof(struct ALARM_PARA)+5] = {0};
	u8 i = 0;
	u8 sum = 0;
	u8 num = sizeof(struct ALARM_PARA);
	for(i=0;i<(sizeof(struct ALARM_PARA)/2);i++)
	{
	   buffer[i*2] = AlarmSet.word[i]&0x00FF;
	   sum += (AlarmSet.word[i]&0x00FF);
	   buffer[i*2+1] = AlarmSet.word[i]>>8;
	   sum += ((AlarmSet.word[i]>>8)&0x00FF);
	}

	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
  CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_ALARMSET_ADDR,num+5);
}

// ¶ÁÈ¡±¨¾¯²ÎÊý±¸·Ý
u8 ReadAlarmParaBAK(void)
{
	u8 buffer[sizeof(struct ALARM_PARA)+5] = {0};
	u8 i = 0,sum1 = 0, sum = 0;
	u8 num = sizeof(struct ALARM_PARA);
  CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_ALARMSETBAK_ADDR,num+5);
	sum1 = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		
	}
	else	// ²ÎÊý²»Õý³£
	{
		return 0;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(sum1 != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		return 1;
	}
	AlarmParaSum = sum&0x00FF;
	for(i=0;i<(sizeof(struct ALARM_PARA)/2);i++)
	{
		AlarmSet.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
	}
	return 0x0D;
}

// ±£´æ±¨¾¯²ÎÊý±¸·Ý
void SaveAlarmParaBAK(void)
{
	u8 buffer[sizeof(struct ALARM_PARA)+5] = {0};
	u8 i = 0, sum = 0;
	u8 num = sizeof(struct ALARM_PARA);
	for(i=0;i<(sizeof(struct ALARM_PARA)/2);i++)
	{
	   buffer[i*2] = AlarmSet.word[i]&0x00FF;
	   sum += (AlarmSet.word[i]&0x00FF);
	   buffer[i*2+1] = AlarmSet.word[i]>>8;
	   sum += ((AlarmSet.word[i]>>8)&0x00FF);
	}
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
  CS2BZ=0;
  SPI_Flash_Write(buffer,FLASH_ALARMSETBAK_ADDR,num+5);
}

// ¼ì²é±¨¾¯²ÎÊý
void CheckAlarmPara(void)
{
	u8 buffer[sizeof(struct ALARM_PARA)+5] = {0};
	u8 i = 0, sum = 0;
	u8 num = sizeof(struct ALARM_PARA);

	for(i=0;i<(sizeof(struct ALARM_PARA)/2);i++)
	{
	   buffer[i*2] = AlarmSet.word[i]&0x00FF;
	   buffer[i*2+1] = AlarmSet.word[i]>>8;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(AlarmParaSum != (sum&0x00FF) && AlarmParaChecked == 0)
	{
		AlarmParaChecked = 1;
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(ReadAlarmPara() != 0x0D)	// ¶ÁÈ¡±£»¤²ÎÊý
		{
			if(ReadAlarmParaBAK() == 0x0D)	// Õý³£±£»¤²ÎÊý³ö´í£¬±¸·Ý±£»¤²ÎÊýÕýÈ·£¬ÔòÖØÐÂ±£´æÕý³£±£»¤²ÎÊý
			{
				SaveAlarmPara();
			}
		}
	}
}

// ¶ÁÈ¡×Ô¶¯½âÁÐ²ÎÊý
u8 ReadAutoSwitchPara(void)
{
	u8 buffer[sizeof(struct AUTO_SWITCHOFF)+5] = {0};
	u8 i = 0, sum = 0,err = 0, returnvalue = 0x0D;
	u8 num = sizeof(struct AUTO_SWITCHOFF);
	CS2BZ=0; 
	SPI_Flash_Read(buffer,FLASH_AUTOFFSET_ADDR,num+5);
	AutoswitchParaSum = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		err = 0;
	}
	else	// ²ÎÊý²»Õý³£
	{
		err = 1;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(AutoswitchParaSum != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		err = 1;
		returnvalue = 0x01;
	}
	for(i=0;i<(sizeof(struct AUTO_SWITCHOFF)/2);i++)
	{
		AutoswitchSet.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
	}
	if(err != 0)	// ²ÎÊý´íÎó
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0700;
	}
	else
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 0;
	}

	return returnvalue;
}

// ±£´æ×Ô¶¯½âÁÐ²ÎÊý
void SaveAutoSwitchPara(void)
{
	u8 buffer[sizeof(struct AUTO_SWITCHOFF)+5] = {0};
	u8 i = 0,sum = 0;
	u8 num = sizeof(struct AUTO_SWITCHOFF);
	for(i=0;i<(sizeof(struct AUTO_SWITCHOFF)/2);i++)
	{
	   buffer[i*2] = AutoswitchSet.word[i]&0x00FF;
	   sum += (AutoswitchSet.word[i]&0x00FF);
	   buffer[i*2+1] = AutoswitchSet.word[i]>>8;
	   sum += (AutoswitchSet.word[i]>>8);
	}

	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
  CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_AUTOFFSET_ADDR,num+5);
}

// ¶ÁÈ¡×Ô¶¯½âÁÐ²ÎÊý±¸·Ý
u8 ReadAutoSwitchParaBAK(void)
{
	u8 buffer[sizeof(struct AUTO_SWITCHOFF)+5] = {0};
	u8 i = 0, sum = 0,sum1 = 0;
	u8 num = sizeof(struct AUTO_SWITCHOFF);
	CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_AUTOFFSETBAK_ADDR,num+5);
	sum1 = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		
	}
	else	// ²ÎÊý²»Õý³£
	{
		return 0;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(sum1 != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		return 1;
	}
	AutoswitchParaSum = sum&0x00FF;
	for(i=0;i<(sizeof(struct AUTO_SWITCHOFF)/2);i++)
	{
		AutoswitchSet.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
	}
	return 0x0D;
}

// ±£´æ×Ô¶¯½âÁÐ²ÎÊý±¸·Ý
void SaveAutoSwitchParaBAK(void)
{
	u8 buffer[sizeof(struct AUTO_SWITCHOFF)+5] = {0};
	u8 i = 0, sum = 0;
	u8 num = sizeof(struct AUTO_SWITCHOFF);
	for(i=0;i<(sizeof(struct AUTO_SWITCHOFF)/2);i++)
	{
	   buffer[i*2] = AutoswitchSet.word[i]&0x00FF;
	   sum += (AutoswitchSet.word[i]&0x00FF);
	   buffer[i*2+1] = AutoswitchSet.word[i]>>8;
	   sum += (AutoswitchSet.word[i]>>8);
	}

	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
  CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_AUTOFFSETBAK_ADDR,num+5);
}

// ¼ì²é×Ô¶¯½âÁÐ²ÎÊý
void CheckAutoSwitchPara(void)
{
	u8 buffer[sizeof(struct AUTO_SWITCHOFF)+5] = {0};
	u8 i = 0;
	u8 sum = 0;
	u8 num = sizeof(struct AUTO_SWITCHOFF);

	for(i=0;i<(sizeof(struct AUTO_SWITCHOFF)/2);i++)
	{
	   buffer[i*2] = AutoswitchSet.word[i]&0x00FF;
	   buffer[i*2+1] = AutoswitchSet.word[i]>>8;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(AutoswitchParaSum != (sum&0x00FF) && AutoswitchParaChecked == 0)
	{
		AutoswitchParaChecked = 1;
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(ReadAutoSwitchPara() != 0x0D)	// ¶ÁÈ¡±£»¤²ÎÊý
		{
			if(ReadAutoSwitchParaBAK() == 0x0D)	// Õý³£±£»¤²ÎÊý³ö´í£¬±¸·Ý±£»¤²ÎÊýÕýÈ·£¬ÔòÖØÐÂ±£´æÕý³£±£»¤²ÎÊý
			{
				SaveAutoSwitchPara();
			}
		}
	}
}

// ¶ÁÈ¡²âÁ¿ÏµÊý
u8 ReadModulus(void)
{
	u8 buffer[sizeof(struct Modulus)+5] = {0};
	u8 i = 0, sum = 0,err = 0, returnvalue = 0x0D;
  u8 num = sizeof(struct Modulus);
	CS2BZ=0;
  SPI_Flash_Read(buffer,FLASH_MEASURECF_ADDR,num+5);
	MeasureParaSum = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		err = 0;
	}
	else	// ²ÎÊý²»Õý³£
	{
		err = 1;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(MeasureParaSum != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		err = 1;
		returnvalue = 0x01;
	}
	
	for(i=0;i<(sizeof(struct Modulus)/2);i++)
	{
		ProtectModulus.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
		if(ProtectModulus.word[i]<15000 || ProtectModulus.word[i]>55000)
			ProtectModulus.word[i]=32768;
	}

	if(err != 0)	// ²ÎÊý´íÎó
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0700;
	}
	else
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 0;
	}
	return returnvalue;
}

// ±£´æ²âÁ¿ÏµÊý
void SaveModulus(void)
{
	u8 buffer[sizeof(struct Modulus)+5] = {0};
	u8 i = 0, sum = 0;
  u8 num = sizeof(struct Modulus);
	for(i=0;i<(sizeof(struct Modulus)/2);i++)
	{
	   if(ProtectModulus.word[i]<15000 || ProtectModulus.word[i]>55000)
			ProtectModulus.word[i]=32768;
	   buffer[i*2] = ProtectModulus.word[i]&0x00FF;
	   buffer[i*2+1] = ProtectModulus.word[i]>>8;
	}
	for(i=0;i<num;i++)sum +=buffer[i];
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_MEASURECF_ADDR,num+5);
}

// ¶ÁÈ¡²âÁ¿ÏµÊý±¸·Ý
u8 ReadModulusBAK(void)
{
	u8 buffer[sizeof(struct Modulus)+5] = {0};
	u8 i = 0, sum = 0,sum1 = 0;
  u8 num = sizeof(struct Modulus);
	CS2BZ=0;
  SPI_Flash_Read(buffer,FLASH_MEASURECFBAK_ADDR,num+5);
	sum1 = buffer[num];
  if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		
	}
	else	// ²ÎÊý²»Õý³£
	{
		return 0;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(sum1 != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		return 1;
	}
	MeasureParaSum = sum&0x00FF;
	for(i=0;i<(sizeof(struct Modulus)/2);i++)
	{
		ProtectModulus.word[i] = buffer[i*2] | (buffer[i*2+1]<<8);
		if(ProtectModulus.word[i]<15000 || ProtectModulus.word[i]>55000)
			ProtectModulus.word[i]=32768;
	}
	return 0x0D;
}

// ±£´æ²âÁ¿ÏµÊý±¸·Ý
void SaveModulusBAK(void)
{
	u8 buffer[sizeof(struct Modulus)+5] = {0};
	u8 i = 0, sum = 0;
  u8 num = sizeof(struct Modulus);
	for(i=0;i<(sizeof(struct Modulus)/2);i++)
	{
	   if(ProtectModulus.word[i]<15000 || ProtectModulus.word[i]>55000)
			ProtectModulus.word[i]=32768;
	   buffer[i*2] = ProtectModulus.word[i]&0x00FF;
	   buffer[i*2+1] = ProtectModulus.word[i]>>8;
	}
	for(i=0;i<num;i++)sum +=buffer[i];
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_MEASURECFBAK_ADDR,num+5);
}

// ¼ì²é²âÁ¿ÏµÊý
void CheckModulus(void)
{
	u8 buffer[sizeof(struct Modulus)+5] = {0};
	u8 i = 0;
	u8 sum = 0;
	u8 num = sizeof(struct Modulus);

	for(i=0;i<(sizeof(struct Modulus)/2);i++)
	{
	   buffer[i*2] = ProtectModulus.word[i]&0x00FF;
	   buffer[i*2+1] = ProtectModulus.word[i]>>8;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(MeasureParaSum != (sum&0x00FF) && MeasureParaChecked == 0)
	{
		MeasureParaChecked = 1;
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(ReadModulus() != 0x0D)	// ¶ÁÈ¡±£»¤²ÎÊý
		{
			if(ReadModulusBAK() == 0x0D)	// Õý³£²âÁ¿ÏµÊý³ö´í£¬±¸·Ý²âÁ¿ÏµÊýÕýÈ·£¬ÔòÖØÐÂ±£´æÕý³£²âÁ¿ÏµÊý
			{
				SaveModulus();
			}
		}
	}
}

// ¼ì²éÐ£±íÏµÊý
void Check_ATT7022(void)
{
	u8 sum = 0;
	u8 buffer[62] = {0};
  u32 readATT7022_sum = 0x0D;
	CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_CORRECTCF_ADDR,62);
	ATT7022_sum = buffer[54] | (buffer[55]<<8) | (buffer[56]<<16);
	
	readATT7022_sum = ReadAT7052(0x3E);		
	if(readATT7022_sum != ATT7022_sum)
	{
		sum += ZYXSA&0x00FF;  // 1
		sum += ZYXSA>>8;      // 2
		sum += ZYXSB&0x00FF;  // 3
		sum += ZYXSB>>8;      // 4
		sum += ZYXSC&0x00FF;  // 5
		sum += ZYXSC>>8;      // 6
		sum += XWXSA&0x00FF;  // 7
		sum += XWXSA>>8;      // 8
		sum += XWXSB&0x00FF;  // 9
		sum += XWXSB>>8;      // 10
		sum += XWXSC&0x00FF;  // 11
		sum += XWXSC>>8;      // 12
		sum += WGXSA&0x00FF;  // 13
		sum += WGXSA>>8;      // 14
		sum += WGXSB&0x00FF;  // 15
		sum += WGXSB>>8;      // 16
		sum += WGXSC&0x00FF;  // 17
		sum += WGXSC>>8;      // 18
		sum += DYXSA&0x00FF;  // 19
		sum += DYXSA>>8;      // 20
		sum += DYXSB&0x00FF;  // 21
		sum += DYXSB>>8;      // 22
		sum += DYXSC&0x00FF;  // 23
		sum += DYXSC>>8;      // 24
		sum += DLXSA&0x00FF;  // 25
		sum += DLXSA>>8;      // 26
		sum += DLXSB&0x00FF;  // 27
		sum += DLXSB>>8;      // 28
		sum += DLXSC&0x00FF;  // 29
		sum += DLXSC>>8;      // 30
		sum += DYPYA&0x00FF;  // 31
		sum += DYPYA>>8;      // 32
		sum += DYPYB&0x00FF;  // 33
		sum += DYPYB>>8;      // 34
		sum += DYPYC&0x00FF;  // 35
		sum += DYPYC>>8;      // 36
		sum += DLPYA&0x00FF;  // 37
		sum += DLPYA>>8;      // 38
		sum += DLPYB&0x00FF;  // 39
		sum += DLPYB>>8;      // 40
		sum += DLPYC&0x00FF;  // 41
		sum += DLPYC>>8;      // 42
		sum += DYXSA1&0x00FF; // 43
		sum += DYXSA1>>8;     // 44
		sum += DYXSB1&0x00FF; // 45
		sum += DYXSB1>>8;     // 46
		sum += DYXSC1&0x00FF; // 47
		sum += DYXSC1>>8;     // 48
		sum += DLXSA1&0x00FF; // 49
		sum += DLXSA1>>8;     // 50
		sum += DLXSB1&0x00FF; // 51
		sum += DLXSB1>>8;     // 52
		sum += DLXSC1&0x00FF; // 53
		sum += DLXSC1>>8;     // 54
		sum += ATT7022_sum&0x00FF;// 55
		sum += ATT7022_sum>>8;    // 56
		sum += ATT7022_sum>>16;   // 57
		sum += 1;

		if(CorrectParaSum != (sum&0x00FF) && CorrectParaChecked == 0)
	  {
			CorrectParaChecked = 1;
			KZQMeasureData.para.SelfCheck.bit.para_set = 1;
			if(Read_ATT7022() != 0x0D)		  // ¶ÁÈ¡ATT7022Ð£±í²ÎÊý
	    {
		    Read_ATT7022BAK();
	    }
	  }		
		Write_ATT7022();
	}
}

// ¶ÁÈ¡Ð£±íÏµÊý
u8 Read_ATT7022(void)
{
	u8 buffer[62] = {0};
	u8 i = 0, sum = 0,err = 0, returnvalue = 0x0D;
	CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_CORRECTCF_ADDR,62);
	CorrectParaSum = buffer[57];
	if(buffer[58] == 0x5A && buffer[59] == 0xA5 && buffer[60] == 0x0F && buffer[61] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{

	}
	else	// ²ÎÊý²»Õý³£
	{
		err = 1;
	}
	for(i=0;i<57;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(CorrectParaSum != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		err = 1;
		returnvalue = 0x01;
	}
	
	ZYXSA = buffer[0] | (buffer[1]<<8);
	ZYXSB = buffer[2] | (buffer[3]<<8);
	ZYXSC = buffer[4] | (buffer[5]<<8);
	
	XWXSA = buffer[6] | (buffer[7]<<8);
	XWXSB = buffer[8] | (buffer[9]<<8);
	XWXSC = buffer[10] | (buffer[11]<<8);
	
	WGXSA = buffer[12] | (buffer[13]<<8);
	WGXSB = buffer[14] | (buffer[15]<<8);
	WGXSC = buffer[16] | (buffer[17]<<8);
	
	DYXSA = buffer[18] | (buffer[19]<<8);
	DYXSB = buffer[20] | (buffer[21]<<8);
	DYXSC = buffer[22] | (buffer[23]<<8);
	
	DLXSA = buffer[24] | (buffer[25]<<8);
	DLXSB = buffer[26] | (buffer[27]<<8);
	DLXSC = buffer[28] | (buffer[29]<<8);
	
	DYPYA = buffer[30] | (buffer[31]<<8);
	DYPYB = buffer[32] | (buffer[33]<<8);
	DYPYC = buffer[34] | (buffer[35]<<8);
	
	DLPYA = buffer[36] | (buffer[37]<<8);
	DLPYB = buffer[38] | (buffer[39]<<8);
	DLPYC = buffer[40] | (buffer[41]<<8);
	
	DYXSA1 = buffer[42] | (buffer[43]<<8);
	DYXSB1 = buffer[44] | (buffer[45]<<8);
	DYXSC1 = buffer[46] | (buffer[47]<<8);
	
	DLXSA1 = buffer[48] | (buffer[49]<<8);
	DLXSB1 = buffer[50] | (buffer[51]<<8);
	DLXSC1 = buffer[52] | (buffer[53]<<8);
	
	ATT7022_sum = buffer[54] | (buffer[55]<<8) | (buffer[56]<<16);
	
	if(err != 0)	// ²ÎÊý´íÎó
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0700;
	}
	else
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 0;
	}
	return returnvalue;
}

// ¶ÁÈ¡Ð£±íÏµÊý±¸·Ý
void Read_ATT7022BAK(void)
{
	u8 buffer[62] = {0};
	u8 i = 0, sum = 0,sum1 = 0;
	CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_CORRECTCFBAK_ADDR,62);
	sum1 = buffer[57];
	for(i=0;i<57;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(sum1 == sum)	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
    ZYXSA = buffer[0] | (buffer[1]<<8);
		ZYXSB = buffer[2] | (buffer[3]<<8);
		ZYXSC = buffer[4] | (buffer[5]<<8);
		
		XWXSA = buffer[6] | (buffer[7]<<8);
		XWXSB = buffer[8] | (buffer[9]<<8);
		XWXSC = buffer[10] | (buffer[11]<<8);
		
		WGXSA = buffer[12] | (buffer[13]<<8);
		WGXSB = buffer[14] | (buffer[15]<<8);
		WGXSC = buffer[16] | (buffer[17]<<8);
		
		DYXSA = buffer[18] | (buffer[19]<<8);
		DYXSB = buffer[20] | (buffer[21]<<8);
		DYXSC = buffer[22] | (buffer[23]<<8);
		
		DLXSA = buffer[24] | (buffer[25]<<8);
		DLXSB = buffer[26] | (buffer[27]<<8);
		DLXSC = buffer[28] | (buffer[29]<<8);
		
		DYPYA = buffer[30] | (buffer[31]<<8);
		DYPYB = buffer[32] | (buffer[33]<<8);
		DYPYC = buffer[34] | (buffer[35]<<8);
		
		DLPYA = buffer[36] | (buffer[37]<<8);
		DLPYB = buffer[38] | (buffer[39]<<8);
		DLPYC = buffer[40] | (buffer[41]<<8);
		
		DYXSA1 = buffer[42] | (buffer[43]<<8);
		DYXSB1 = buffer[44] | (buffer[45]<<8);
		DYXSC1 = buffer[46] | (buffer[47]<<8);
		
		DLXSA1 = buffer[48] | (buffer[49]<<8);
		DLXSB1 = buffer[50] | (buffer[51]<<8);
		DLXSC1 = buffer[52] | (buffer[53]<<8);
		
		ATT7022_sum = buffer[54] | (buffer[55]<<8) | (buffer[56]<<16);
	}
}

void Write_ATT7022(void)
{
	ATT7022_WREnable();
	// ÓÐ¹¦ÔöÒæÏµÊý
	WriteAT7052(0x84,ZYXSA);
	WriteAT7052(0x85,ZYXSB);
	WriteAT7052(0x86,ZYXSC);
	
	// ÏàÎ»Ð£ÕýÏµÊý
	WriteAT7052(0x8D,XWXSA);
	WriteAT7052(0x90,XWXSA);
	WriteAT7052(0x8E,XWXSB);

	WriteAT7052(0x91,XWXSB);
	WriteAT7052(0x8F,XWXSC);
	WriteAT7052(0x92,XWXSC);

	// ÎÞ¹¦ÔöÒæÏµÊý
	WriteAT7052(0x87,WGXSA);
	WriteAT7052(0x88,WGXSB);
	WriteAT7052(0x89,WGXSC);

	// µçÑ¹Ð£ÕýÏµÊý
	WriteAT7052(0x97,DYXSA);
	WriteAT7052(0x98,DYXSB);
	WriteAT7052(0x99,DYXSC);

	// µçÁ÷Ð£ÕýÏµÊý
	WriteAT7052(0x9A,DLXSA);
	WriteAT7052(0x9B,DLXSB);
	WriteAT7052(0x9C,DLXSC);	
	
	// µçÑ¹ÏµÊý
	WriteAT7052(0xA4,DYXSA1);
	WriteAT7052(0xA5,DYXSB1);
	WriteAT7052(0xA6,DYXSC1);
	
	// µçÁ÷Ð£ÕýÏµÊý
	WriteAT7052(0xA7,DLXSA1);
	WriteAT7052(0xA8,DLXSB1);
	WriteAT7052(0xA9,DLXSC1);
	//ATT7022_WRDisable();	
}

// ±£´æÐ£±í²ÎÊý
void SaveATT7022(void)
{
	u8 buffer[62] = {0};
	u8 sum = 0;
	buffer[0] = ZYXSA&0x00FF;
	sum += ZYXSA&0x00FF;
	buffer[1] = ZYXSA>>8;
	sum += ZYXSA>>8;
	buffer[2] = ZYXSB&0x00FF;
	sum += ZYXSB&0x00FF;
	buffer[3] = ZYXSB>>8;
	sum += ZYXSB>>8;
	buffer[4] = ZYXSC&0x00FF;
	sum += ZYXSC&0x00FF;
	buffer[5] = ZYXSC>>8;
	sum += ZYXSC>>8;
	buffer[6] = XWXSA&0x00FF;
	sum += XWXSA&0x00FF;
	buffer[7] = XWXSA>>8;
	sum += XWXSA>>8;
	buffer[8] = XWXSB&0x00FF;
	sum += XWXSB&0x00FF;
	buffer[9] = XWXSB>>8;
	sum += XWXSB>>8;
	buffer[10] = XWXSC&0x00FF;
	sum += XWXSC&0x00FF;
	buffer[11] = XWXSC>>8;
  sum += XWXSC>>8;
	buffer[12] = WGXSA&0x00FF;
	sum += WGXSA&0x00FF;
	buffer[13] = WGXSA>>8;
	sum += WGXSA>>8;
	buffer[14] = WGXSB&0x00FF;
	sum += WGXSB&0x00FF;
	buffer[15] = WGXSB>>8;
	sum += WGXSB>>8;
	buffer[16] = WGXSC&0x00FF;
	sum += WGXSC&0x00FF;
	buffer[17] = WGXSC>>8;
  sum += WGXSC>>8;
	buffer[18] = DYXSA&0x00FF;
	sum += DYXSA&0x00FF;
	buffer[19] = DYXSA>>8;
	sum += DYXSA>>8;
	buffer[20] = DYXSB&0x00FF;
	sum += DYXSB&0x00FF;
	buffer[21] = DYXSB>>8;
	sum += DYXSB>>8;
	buffer[22] = DYXSC&0x00FF;
	sum += DYXSC&0x00FF;
	buffer[23] = DYXSC>>8;
  sum += DYXSC>>8;
	buffer[24] = DLXSA&0x00FF;
	sum += DLXSA&0x00FF;
	buffer[25] = DLXSA>>8;
	sum += DLXSA>>8;
	buffer[26] = DLXSB&0x00FF;
	sum += DLXSB&0x00FF;
	buffer[27] = DLXSB>>8;
	sum += DLXSB>>8;
	buffer[28] = DLXSC&0x00FF;
	sum += DLXSC&0x00FF;
	buffer[29] = DLXSC>>8;
  sum += DLXSC>>8;
	buffer[30] = DYPYA&0x00FF;
	sum += DYPYA&0x00FF;
	buffer[31] = DYPYA>>8;
	sum += DYPYA>>8;
	buffer[32] = DYPYB&0x00FF;
	sum += DYPYB&0x00FF;
	buffer[33] = DYPYB>>8;
	sum += DYPYB>>8;
	buffer[34] = DYPYC&0x00FF;
	sum += DYPYC&0x00FF;
	buffer[35] = DYPYC>>8;
  sum += DYPYC>>8;
	buffer[36] = DLPYA&0x00FF;
	sum += DLPYA&0x00FF;
	buffer[37] = DLPYA>>8;
	sum += DLPYA>>8;
	buffer[38] = DLPYB&0x00FF;
	sum += DLPYB&0x00FF;
	buffer[39] = DLPYB>>8;
	sum += DLPYB>>8;
	buffer[40] = DLPYC&0x00FF;
	sum += DLPYC&0x00FF;
	buffer[41] = DLPYC>>8;
	sum += DLPYC>>8;
	buffer[42] = DYXSA1&0x00FF;
	sum += DYXSA1&0x00FF;
	buffer[43] = DYXSA1>>8;
	sum += DYXSA1>>8;
	buffer[44] = DYXSB1&0x00FF;
	sum += DYXSB1&0x00FF;
	buffer[45] = DYXSB1>>8;
	sum += DYXSB1>>8;
	buffer[46] = DYXSC1&0x00FF;
	sum += DYXSC1&0x00FF;
	buffer[47] = DYXSC1>>8;
  sum += DYXSC1>>8;
	buffer[48] = DLXSA1&0x00FF;
	sum += DLXSA1&0x00FF;
	buffer[49] = DLXSA1>>8;
	sum += DLXSA1>>8;
	buffer[50] = DLXSB1&0x00FF;
	sum += DLXSB1&0x00FF;
	buffer[51] = DLXSB1>>8;
	sum += DLXSB1>>8;
	buffer[52] = DLXSC1&0x00FF;
	sum += DLXSC1&0x00FF;
	buffer[53] = DLXSC1>>8;
	sum += DLXSC1>>8;
	buffer[54] = ATT7022_sum&0x00FF;
	sum += ATT7022_sum&0x00FF;
	buffer[55] = ATT7022_sum>>8;
	sum += ATT7022_sum>>8;
	buffer[56] = ATT7022_sum>>16;
  sum += ATT7022_sum>>16;
	sum += 1;
	CorrectParaSum = sum&0x00FF;
	buffer[57] = sum&0x00FF;
	buffer[58] = 0x5A;
	buffer[59] = 0xA5;
	buffer[60] = 0x0F;
	buffer[61] = 0xF0;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_CORRECTCF_ADDR,62);
	SPI_Flash_Write(buffer,FLASH_CORRECTCFBAK_ADDR,62);
}

// ¶ÁÈ¡µôµç²ÎÊý
void ReadPowerOffPara(void)
{
	u8 buffer[116] = {0};
	u64 temp = 0;
	u8 i = 0;
	CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_ENERGYVL_ADDR,115);
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[7-i];
	}
	WpLast = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[15-i];
	}
	WqLast = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[23-i];
	}
	Wp_all_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[31-i];
	}
	Wq_1all_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[39-i];
	}
	Wp_peak_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[47-i];
	}
	Wq_2all_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[55-i];
	}
	Wp_valley_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[63-i];
	}
	Wq_3all_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[71-i];
	}
	Wp_level_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[79-i];
	}
	Wq_4all_Last = temp;

	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[87-i];
	}
	WpFX_all_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[95-i];
	}
	WpFX_peak_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[103-i];
	}
	WpFX_valley_Last = temp;
	
	temp = 0;
	for(i=0;i<8;i++)
	{
		temp <<= 8;
		temp += buffer[111-i];
	}
	WpFX_level_Last = temp;
	
	MeasureData.YaBan1 = buffer[112]&0x01;
	MeasureData.YaBan2 = buffer[113]&0x01;
	MeasureData.YaBan3 = buffer[114]&0x01;
}

// ±£´æµôµçÐè±£´æµÄ²ÎÊý
void SavePowerOffPara(void)
{
	u8 buffer[116] = {0};
	u64 temp = 0;
	u8 i = 0;
	temp = WpLast;
	for(i=0;i<8;i++)
	{
		buffer[i] = temp & 0xff;
		temp >>= 8;
	}

	temp = WqLast;
	for(i=0;i<8;i++)
	{
		buffer[i+8] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = Wp_all_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+16] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = Wq_1all_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+24] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = Wp_peak_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+32] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = Wq_2all_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+40] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = Wp_valley_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+48] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = Wq_3all_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+56] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = Wp_level_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+64] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = Wq_4all_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+72] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = WpFX_all_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+80] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = WpFX_peak_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+88] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = WpFX_valley_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+96] = temp & 0xff;
		temp >>= 8;
	}
	
	temp = WpFX_level_Last;
	for(i=0;i<8;i++)
	{
		buffer[i+104] = temp & 0xff;
		temp >>= 8;
	}

	buffer[112] = MeasureData.YaBan1&0x01;
	buffer[113] = MeasureData.YaBan2&0x01;
	buffer[114] = MeasureData.YaBan3&0x01;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_ENERGYVL_ADDR,115);
}

// ¶ÁÈ¡³ö³§±àºÅºÍÖÆÔìÄêÔÂ
void ReadMachineInformation(void)
{
	u8 buffer[15] = {0};
	u8 i = 0;
	CS2BZ=0;
  SPI_Flash_Read(buffer,FLASH_MANUNUNBER_ADDR,15);
	for(i=0;i<13;i++)
	{
		MachineInformation[i] = buffer[i];
	}
	SwitchCnt = buffer[13] + (buffer[14]<<8);
}

// ±£´æ³ö³§±àºÅºÍÖÆÔìÄêÔÂ
void SaveMachineInformation(void)
{
	u8 buffer[15] = {0};
	u8 i = 0;
	for(i=0;i<13;i++)
	{
		buffer[i] = MachineInformation[i];
	}
  buffer[13] = SwitchCnt&0xff;
  buffer[14] = SwitchCnt>>8;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_MANUNUNBER_ADDR,15);
}

// ±£´æºÏÕ¢¼ÇÂ¼
void SaveRecordOn(void)
{
	u8 buf[100],i;
	u8 bufread[10];	
	u16 recordorder = 0;
	CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),5);
	recordorder = bufread[0] | (bufread[1]<<8);
	if(recordorder == 0xFFFF) recordorder = 0;
	if(recordorder == DLQ_EventRecord_Order[DLQ_EventRecord_Index])
	{
		recordorder = DLQ_EventRecord_Order[DLQ_EventRecord_Index] +1;
		if(recordorder > 60000 || recordorder == 0)recordorder = 1;
		if(DLQ_EventRecord_Order[DLQ_EventRecord_Index] != 0)
		{
			if(DLQ_EventRecord_Index < (FLASH_EventRecord_MAX-1))
				DLQ_EventRecord_Index++;
			else
				DLQ_EventRecord_Index = 0;				
		}
		DLQ_EventRecord_Order[DLQ_EventRecord_Index] = recordorder;
	}
	buf[0] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] & 0x00FF;
	buf[1] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] >> 8;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 1;
	for(i=0;i<8;i++)
	{
		buf[(i*2)+5] = Record_on.word[i]&0x00FF;
		buf[(i*2)+6] = Record_on.word[i]>>8;
	}
	SPI_Flash_Write(buf,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),21);// Ð´ÈëÐòºÅ
}

// ±£´æ·ÖÕ¢¼ÇÂ¼
void SaveRecordOff(void)
{
	u8 buf[100],i;
	u8 bufread[10];	
	u16 recordorder = 0;
	CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),5);
	recordorder = bufread[0] | (bufread[1]<<8);
	if(recordorder == 0xFFFF)recordorder = 0;
	if(recordorder == DLQ_EventRecord_Order[DLQ_EventRecord_Index])
	{
		recordorder = DLQ_EventRecord_Order[DLQ_EventRecord_Index] +1;
		if(recordorder > 60000 || recordorder == 0)recordorder = 1;
		if(DLQ_EventRecord_Order[DLQ_EventRecord_Index] != 0)
		{
			if(DLQ_EventRecord_Index < (FLASH_EventRecord_MAX-1))
				DLQ_EventRecord_Index++;
			else
				DLQ_EventRecord_Index = 0;				
		}
		DLQ_EventRecord_Order[DLQ_EventRecord_Index] = recordorder;
	}
	buf[0] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] & 0x00FF;
	buf[1] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] >> 8;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 2;
	for(i=0;i<8;i++)
	{
		buf[(i*2)+5] = Record_off.word[i]&0x00FF;
		buf[(i*2)+6] = Record_off.word[i]>>8;
	}
	SPI_Flash_Write(buf,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),21);// Ð´ÈëÐòºÅ	
}

// ±£´æ±£»¤¼ÇÂ¼1
void SaveRecordProtect1(void)
{
	u8 buf[100],i;
	u8 bufread[10];	
	u16 recordorder = 0;
	CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),2);
	recordorder = bufread[0] | (bufread[1]<<8);
	if(recordorder == 0xFFFF)recordorder = 0;
	if(recordorder == DLQ_EventRecord_Order[DLQ_EventRecord_Index])
	{
		recordorder = DLQ_EventRecord_Order[DLQ_EventRecord_Index] +1;
		if(recordorder > 60000 || recordorder == 0)recordorder = 1;
		if(DLQ_EventRecord_Order[DLQ_EventRecord_Index] != 0)
		{
			if(DLQ_EventRecord_Index < (FLASH_EventRecord_MAX-1))
				DLQ_EventRecord_Index++;
			else
				DLQ_EventRecord_Index = 0;				
		}
		DLQ_EventRecord_Order[DLQ_EventRecord_Index] = recordorder;
	}
	buf[0] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] & 0x00FF;
	buf[1] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] >> 8;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 3;
	for(i=0;i<sizeof(struct RECORDpara)/2;i++)
	{
		buf[(i*2)+5] = Record_protect1.word[i]&0x00FF;
		buf[(i*2)+6] = Record_protect1.word[i]>>8;
	}
	SPI_Flash_Write(buf,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),sizeof(struct RECORDpara)+5);// Ð´ÈëÐòºÅ
}

// ±£´æ±£»¤¼ÇÂ¼2
void SaveRecordProtect2(void)
{
	u8 buf[100],i;
	u8 bufread[10];	
	u16 recordorder = 0;
	CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),2);
	recordorder = bufread[0] | (bufread[1]<<8);
	if(recordorder == 0xFFFF)recordorder = 0;
	if(recordorder == DLQ_EventRecord_Order[DLQ_EventRecord_Index])
	{
		recordorder = DLQ_EventRecord_Order[DLQ_EventRecord_Index] +1;
		if(recordorder > 60000 || recordorder == 0)recordorder = 1;
		if(DLQ_EventRecord_Order[DLQ_EventRecord_Index] != 0)
		{
			if(DLQ_EventRecord_Index < (FLASH_EventRecord_MAX-1))
				DLQ_EventRecord_Index++;
			else
				DLQ_EventRecord_Index = 0;				
		}
		DLQ_EventRecord_Order[DLQ_EventRecord_Index] = recordorder;
	}
	buf[0] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] & 0x00FF;
	buf[1] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] >> 8;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 3;
	for(i=0;i<sizeof(struct RECORDpara)/2;i++)
	{
		buf[(i*2)+5] = Record_protect2.word[i]&0x00FF;
		buf[(i*2)+6] = Record_protect2.word[i]>>8;
	}
	SPI_Flash_Write(buf,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),sizeof(struct RECORDpara)+5);// Ð´ÈëÐòºÅ
}

// ±£´æÀ¡Ïß×Ô¶¯»¯¼ÇÂ¼
void SaveRecordAutofeed(void)
{
	u8 buf[100],i;
	u8 bufread[10];	
	u16 recordorder = 0;
	CS2BZ=0;
	SPI_Flash_Read(bufread,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),5);
	recordorder = bufread[0] | (bufread[1]<<8);
	if(recordorder == 0xFFFF) recordorder = 0;
	if(recordorder == DLQ_EventRecord_Order[DLQ_EventRecord_Index])
	{
		recordorder = DLQ_EventRecord_Order[DLQ_EventRecord_Index] +1;
		if(recordorder > 60000 || recordorder == 0)recordorder = 1;
		if(DLQ_EventRecord_Order[DLQ_EventRecord_Index] != 0)
		{
			if(DLQ_EventRecord_Index < (FLASH_EventRecord_MAX-1))
				DLQ_EventRecord_Index++;
			else
				DLQ_EventRecord_Index = 0;				
		}
		DLQ_EventRecord_Order[DLQ_EventRecord_Index] = recordorder;
	}
	buf[0] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] & 0x00FF;
	buf[1] = DLQ_EventRecord_Order[DLQ_EventRecord_Index] >> 8;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 5;
	for(i=0;i<8;i++)
	{
		buf[(i*2)+5] = Record_autofeed.word[i]&0x00FF;
		buf[(i*2)+6] = Record_autofeed.word[i]>>8;
	}
	SPI_Flash_Write(buf,FLASH_EventRecord_ADDR+(DLQ_EventRecord_Index*FLASH_EventRecord_LENGTH),21);// Ð´ÈëÐòºÅ
}

// ¶ÁÈ¡101¹æÔ¼²ÎÊý
u8 Read101Para(void)
{
	u8 buffer[sizeof(struct Rule101Para)+5] = {0};
	u8 i = 0, sum = 0,err = 0, returnvalue = 0x0D;
  u8 num = sizeof(struct Rule101Para);
	CS2BZ=0;
  SPI_Flash_Read(buffer,FLASH_101PARA_ADDR,num+5);
	Measure101_ParaSum = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		err = 0;
	}
	else	// ²ÎÊý²»Õý³£
	{
		err = 1;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(Measure101_ParaSum != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		err = 1;
		returnvalue = 0x01;
	}
	
	for(i=0;i<(sizeof(struct Rule101Para));i++)
	{
		Measure101Para.word[i] = buffer[i];
		if(i>9 && i<29)
		{
		  if(Measure101Para.word[i] > 29)
			  Measure101Para.word[i] = 29;
	  }
		else if(i>28)
		{
		  if(Measure101Para.word[i] > 47)
			  Measure101Para.word[i] = 47;
	  }
	}
	
	if(Measure101Para.para.balance != 1)//·ÇÆ½ºâÄ£Ê½
		Measure101Para.para.balance = 0;
  if(Measure101Para.para.addr_len != 2)
		Measure101Para.para.addr_len = 1; //1»ò2¸ö
	if(Measure101Para.para.reason_len != 2)
     Measure101Para.para.reason_len = 1; //1¸ö»ò2¸ö
	if(Measure101Para.para.data_type != 1)
     Measure101Para.para.data_type = 0;
	if(Measure101Para.para.encrypt != 1)//²»¼ÓÃÜ
     Measure101Para.para.encrypt = 0;

	if(err != 0)	// ²ÎÊý´íÎó
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(SelfCheckErrCode == 0)
			SelfCheckErrCode = 0x0700;
	}
	else
	{
		KZQMeasureData.para.SelfCheck.bit.para_set = 0;
	}
	return returnvalue;
}

// ¶ÁÈ¡101¹æÔ¼²ÎÊý±¸·Ý
u8 Read101ParaBAK(void)
{
	u8 buffer[sizeof(struct Rule101Para)+5] = {0};
	u8 i = 0, sum = 0,sum1 = 0;
	u8 num = sizeof(struct Rule101Para);
	CS2BZ=0;
	SPI_Flash_Read(buffer,FLASH_101PARABAK_ADDR,num+5);
	sum1 = buffer[num];
	if(buffer[num+1] == 0x5A && buffer[num+2] == 0xA5 && buffer[num+3] == 0x0F && buffer[num+4] == 0xF0)	// ²ÎÊýÊÇ·ñ±»ÐÞ¸Ä
	{
		
	}
	else	// ²ÎÊý²»Õý³£
	{
		return 0;
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(sum1 != (sum&0x00FF))	// ²ÎÊýÐ£ÑéºÍ²»Ò»ÖÂ£¬²ÎÊý³ö´í
	{
		return 1;
	}
	Measure101_ParaSum = sum&0x00FF;
	for(i=0;i<(sizeof(struct Rule101Para));i++)
	{
		Measure101Para.word[i] = buffer[i];
		if(i>9 && i<29)
		{
		  if(Measure101Para.word[i] > 29)
			  Measure101Para.word[i] = 29;
	  }
		else if(i>28)
		{
		  if(Measure101Para.word[i] > 47)
			  Measure101Para.word[i] = 47;
	  }
	}
	
	if(Measure101Para.para.balance != 1)//·ÇÆ½ºâÄ£Ê½
		Measure101Para.para.balance = 0;
	if(Measure101Para.para.addr_len != 2)
		Measure101Para.para.addr_len = 1; //1»ò2¸ö
	if(Measure101Para.para.reason_len != 2)
		 Measure101Para.para.reason_len = 1; //1¸ö»ò2¸ö
	if(Measure101Para.para.data_type != 1)
		 Measure101Para.para.data_type = 0;
	if(Measure101Para.para.encrypt != 1)//²»¼ÓÃÜ
     Measure101Para.para.encrypt = 0;
	return 0x0D;
}

// ±£´æ101¹æÔ¼²ÎÊý
void Save101Para(void)
{
	u8 buffer[sizeof(struct Rule101Para)+5] = {0};
	u8 i = 0, sum = 0;
  u8 num = sizeof(struct Rule101Para);
	for(i=0;i<(sizeof(struct Rule101Para));i++)
	{
	  buffer[i] = Measure101Para.word[i];
		if(i>9 && i<29)
		{
		  if(buffer[i] > 29)
			buffer[i] = 29;
	  }
		else if(i>28)
		{
		  if(buffer[i] > 47)
			buffer[i] = 47;
	  }
	}
	for(i=0;i<num;i++)sum +=buffer[i];
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_101PARA_ADDR,num+5);
}

// ±£´æ101¹æÔ¼²ÎÊý±¸·Ý
void Save101ParaBAK(void)
{
	u8 buffer[sizeof(struct Rule101Para)+5] = {0};
	u8 i = 0, sum = 0;
  u8 num = sizeof(struct Rule101Para);
  for(i=0;i<(sizeof(struct Rule101Para));i++)
	{
	  buffer[i] = Measure101Para.word[i];
		if(i>9 && i<29)
		{
		  if(buffer[i] > 29)
			buffer[i] = 29;
	  }
		else if(i>28)
		{
		  if(buffer[i] > 47)
			buffer[i] = 47;
	  }
	}
	for(i=0;i<num;i++)sum +=buffer[i];
	sum += 1;
	buffer[num] = sum&0x00FF;
	buffer[num+1] = 0x5A;
	buffer[num+2] = 0xA5;
	buffer[num+3] = 0x0F;
	buffer[num+4] = 0xF0;
	CS2BZ=0;
	SPI_Flash_Write(buffer,FLASH_101PARABAK_ADDR,num+5);
}

// 101¹æÔ¼²ÎÊý±È½Ï
u8 Measure101ParaCompare(void)
{
	u8 buffer[sizeof(struct Rule101Para)] = {0};
	u8 num = sizeof(struct Rule101Para);
	u8 i = 0;
	
  SPI_Flash_Read(buffer,FLASH_101PARA_ADDR,num);
	for(i=0;i<(sizeof(struct Rule101Para));i++)
	{
		if(Measure101Para.word[i] != buffer[i])
		  return 0;
	}
	return 0x0D;
}

// ¼ì²é101¹æÔ¼²ÎÊý
void CheckMeasure101Para(void)
{

	u8 buffer[sizeof(struct Rule101Para)+5] = {0};
	u8 i = 0,sum = 0;
  u8 num = sizeof(struct Rule101Para);
	
  for(i=0;i<(sizeof(struct Rule101Para));i++)
	{
	  buffer[i] = Measure101Para.word[i];
		if(i>9 && i<29)
		{
		  if(buffer[i] > 29)
			buffer[i] = 29;
	  }
		else if(i>28)
		{
		  if(buffer[i] > 47)
			buffer[i] = 47;
	  }
	}
	for(i=0;i<num;i++)
	{
		sum += buffer[i];
	}
	sum += 1;
	if(Measure101_ParaSum != (sum&0x00FF) && Measure101ParaChecked == 0)
	{
		Measure101ParaChecked = 1;
		KZQMeasureData.para.SelfCheck.bit.para_set = 1;
		if(Read101Para() != 0x0D)	// ¶ÁÈ¡101¹æÔ¼²ÎÊý
	  {
			if(Read101ParaBAK() == 0x0D)	// Õý³£101¹æÔ¼²ÎÊý³ö´í£¬±¸·Ý101¹æÔ¼²ÎÊýÕýÈ·£¬ÔòÖØÐÂ±£´æÕý³£101¹æÔ¼²ÎÊý
			{
				Save101Para();
			}
	  }	
	}
}

