/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "queue.h"
#include "global.h"
#include "AD7606.h" 
#include "control.h"
#include "protocol1.h"
#include "protocol2.h"
#include "protocol3.h"
#include "protocol4.h"
#include "protocol5.h"
#include "protocol6.h"
#include "Encryption.h"
#include "GPS.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
   
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  if(CoreDebug->DHCSR & 1)
		__breakpoint(0);
	return;
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{

  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
// void SVC_Handler(void)
// {
// }

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
// void PendSV_Handler(void)
// {
// }

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//   void SysTick_Handler(void)
//   {
//     //
//   }

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

void TIM1_CC_IRQHandler(void)
{
	static u8 flag_capture1 = 0;
	static u8 flag_capture2 = 0;
	static u32 IC1Value1 = 0;
	static u32 IC1Value2 = 0;
	static u32 IC2Value1 = 0;
	static u32 IC2Value2 = 0;
	if(TIM_GetITStatus(TIM1,TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
		if(flag_capture1 == 0)
		{
		  IC1Value1 = TIM_GetCapture1(TIM1);
			flag_capture1 = 1;
		}
		else 
		{
			IC1Value2 = TIM_GetCapture1(TIM1);
			flag_capture1 = 0;
			if(IC1Value2 > IC1Value1)
				MeasureData.freq_cpu1 = 100000000 / (IC1Value2 - IC1Value1);
			else
				MeasureData.freq_cpu1 = 100000000 / (IC1Value2 - IC1Value1 + 0xFFFF);
		}
  }

	if(TIM_GetITStatus(TIM1,TIM_IT_CC4) != RESET)
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_CC4);
		if(flag_capture2 == 0)
		{
		  IC2Value1 = TIM_GetCapture4(TIM1);
			flag_capture2 = 1;
		}
		else 
		{
			IC2Value2 = TIM_GetCapture4(TIM1);
			flag_capture2 = 0;
			if(IC2Value2 > IC2Value1)
				MeasureData.freq_cpu2 = 100000000 / (IC2Value2 - IC2Value1);
			else
				MeasureData.freq_cpu2 = 100000000 / (IC2Value2 - IC2Value1 + 0xFFFF);
		}
	}
}

void TIM2_IRQHandler(void)		 // 1ms
{
	static u8 delay10ms = 0;
	static u16 pulse_value = 0;
	static u16 lock2_cnt = 0;
	static u16 power_up_delay = 0;		// 上电延时
	u8 i = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
		TaskRemarks(); 
		delay10ms++;
	  if(delay10ms>9)
	  {
		  delay10ms=0;		
			for(i=0;i<(sizeof(struct PROTECT_DELAY)/2);i++)	// 保护延时
			{
				if(ProtectDelay.word[i]>0 && ProtectTimeout.word[i] == 0)
				{
					ProtectDelay.word[i]--;
					if(ProtectDelay.word[i] == 0)
					{
						ProtectTimeout.word[i] = 1;
						if(i==8)
							ProtectTimeout.para.novoltage_value_A = 0;
						else if(i==9)
							ProtectTimeout.para.novoltage_value_B = 0;
						else if(i==10)
							ProtectTimeout.para.voltage_value_A = 0;
						else if(i==11)
							ProtectTimeout.para.voltage_value_B = 0;
					}
				}
			}

			for(i=0;i<(sizeof(struct ALARM_TIMEOUT)/2);i++)	// 报警延时
			{
				if(AlarmDelay.word[i]>0 && AlarmTimeout.word[i] == 0)
				{
					AlarmDelay.word[i]--;
					if(AlarmDelay.word[i] == 0)
						AlarmTimeout.word[i] = 1;
				}
			}
			
			if(ReOnTimeDelay > 0)         //重合闸延时
			{
				ReOnTimeDelay--;
				if(ReOnTimeDelay == 0)
				{
					ReOnTimeOut = 1;
				}
			}
			
			if(Switchon_Lock2_Flag == 1)
			{
				lock2_cnt++;
				if(lock2_cnt > 4)
				{
					if(MeasureData.Protect_UA_val > 1400 || MeasureData.Protect_UB_val > 1400)//残压固定为25%
					{
						Switchon_Lock2 = 1;
						if(KZQMeasureData.para.ProtectFlag1.bit.switchon_lock == 0)
						{
					    RecordSOE_Pad(SOE_ADDR_RESVOLTAGE_LOCK_pad,1);
					    KZQMeasureData.para.ProtectFlag1.bit.switchon_lock = 1;
						}
					}
				}
				if(lock2_cnt >= (AutoswitchSet.para.err_current_time+5))
				{
          Switchon_Lock2 = 0;
					lock2_cnt = 0;
					Switchon_Lock2_Flag = 0;
				}
			}
		}
		
		if(StoreTimeDelay > 0)
	  {
			StoreTimeDelay--;
			if(StoreTimeDelay == 0)
			{
			  StoreTimeout = 1;
				pulse_value = 0;
			}
			pulse_value++;
			if(pulse_value == 5000)
			{
				KZCN_OFF;
			}
			else if(pulse_value == 5500 && WCN_IN3 == 0)
			{
				KZCN_ON;
				OUT_COM_ON;		        // 打开输出公共端		
			}
	  }
		else if(StoreTimeDelay == 0)
		{
			pulse_value = 0;
		}
		
		// 断路器位置状态
		if(HW_IN2 == 0 && FW_IN1 == 1)
		{
			KZQMeasureData.para.DLQflag.bit.breaker = 1;		// 合闸状态	
		}
		else if(HW_IN2 == 1 && FW_IN1 == 0)
		{
			KZQMeasureData.para.DLQflag.bit.breaker = 2;	// 分闸状态
			ProtectTimesLock = 0;
		}
		else KZQMeasureData.para.DLQflag.bit.breaker = 0;

		if(Max_current_after_cnt<10000)Max_current_after_cnt++;
		
		if(CtrlTimeDelay > 0)
		{
			CtrlTimeDelay--;
			if(CtrlTimeDelay == 0)
			{
				CtrlTimeout = 1;
			}
		}

		if(power_up_delay < 4000)
			power_up_delay++;
		if(power_up_delay > 1000) //上电超过1s可以控制
		{
			CtrlOutput(&SwitchFlag);
		}
		else
		{
			SwitchFlag = 0;
		}
		if(TESTFlag == 0)	// 出厂检测状态
	  {
			if((ProtectFlag==1 || (KZQMeasureData.para.AlarmFlag.word & 0x01ff)) && power_up_delay>2000)	// 事故输出
			{
				OUT6_ON;    //out6为公用端，高电平保持常开
				OUT4_ON;
			}
			else
			{
				OUT4_OFF;
			}

			if((KZQMeasureData.para.AlarmFlag.word & 0x01ff) && power_up_delay>2000)    //报警输出
			{
				OUT6_ON;    //out6为公用端，高电平保持常开
				OUT5_ON;
			}
			else
			{
				OUT5_OFF;
			}
    }	
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}

//void ADC_IRQHandler(void)
//{
//	u16 ADC7606DATA = 0;
//  if (ADC_GetITStatus(ADC1,ADC_IT_EOC)==SET) 
//	{
//    //读取ADC的转换值
//    ADC7606DATA = ADC_GetConversionValue(ADC1);
//  }
//  ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
//}

void TIM4_IRQHandler(void)		 // 200us
{
	u8 err_value = 0;
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		ADC_SoftwareStartConv(ADC1);		

		if(HumiTest_Flag == 1)
		{
			err_value = Humi_Handle_Step();
			if(err_value == 0x0D) 	// 读取温湿度成功
			{
				HumiTest_Flag = 0;
				KZQMeasureData.para.SelfCheck.bit.Temp_Humi = 1;
			}
			else if(err_value == 0)
			{
				HumiTest_Flag = 0;
				KZQMeasureData.para.SelfCheck.bit.Temp_Humi = 0;
			}
	  }
	  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}	
}

void EXTI15_10_IRQHandler(void)	
{
	static u16 DMAADCReadIndex = 0;
	s16 ADC7606DATA1 = 0;
	s16 ADC7606DATA2 = 0;
	s16 ADC7606DATA3 = 0;
	s16 ADC7606DATA4 = 0;
	s16 ADC7606DATA5 = 0;
	s16 ADC7606DATA6 = 0;
	s16 ADC7606DATA7 = 0;
	s16 ADC7606DATA8 = 0;
	static u16 Avr_cnt = 0;
	static u32 IA_sum1 = 0;
	static u32 IB_sum1 = 0;
	static u32 IC_sum1 = 0;
	static u32 PIA_sum1 = 0;
	static u32 PIB_sum1 = 0;
	static u32 PIC_sum1 = 0;
	static u32 U0_sum1 = 0;
	static u32 I0_sum1 = 0;	
	static u32 PI0_sum1 = 0;	
	
	static u32 IA_sum2 = 0;
	static u32 IB_sum2 = 0;
	static u32 IC_sum2 = 0;
	static u32 PIA_sum2 = 0;
	static u32 PIB_sum2 = 0;
	static u32 PIC_sum2 = 0;
	static u32 U0_sum2 = 0;
	static u32 I0_sum2 = 0;		
	static u32 PI0_sum2 = 0;	
	
	static u32 IA_sum3 = 0;
	static u32 IB_sum3 = 0;
	static u32 IC_sum3 = 0;
	static u32 PIA_sum3 = 0;
	static u32 PIB_sum3 = 0;
	static u32 PIC_sum3 = 0;
	static u32 U0_sum3 = 0;
	static u32 I0_sum3 = 0;	
  static u32 PI0_sum3 = 0;		
	
	static u32 IA_sum4 = 0;
	static u32 IB_sum4 = 0;
	static u32 IC_sum4 = 0;
	static u32 PIA_sum4 = 0;
	static u32 PIB_sum4 = 0;
	static u32 PIC_sum4 = 0;
	static u32 U0_sum4 = 0;
	static u32 I0_sum4 = 0;		
	static u32 PI0_sum4 = 0;	
	
	static s32 IA_sum_last = 0;
	static s32 IB_sum_last = 0;
	static s32 IC_sum_last = 0;
	static s32 PIA_sum_last = 0;
	static s32 PIB_sum_last = 0;
	static s32 PIC_sum_last = 0;
	static s32 U0_sum_last = 0;
	static s32 I0_sum_last = 0;	
	static s32 UA_sum_last = 0;
	static s32 UB_sum_last = 0;
	static s32 UC_sum_last = 0;
	static s32 UPT1_sum_last = 0;
	static s32 UPT2_sum_last = 0;

	static s16 IA_avr = 0;
	static s16 IB_avr = 0;
	static s16 IC_avr = 0;
	static s16 PIA_avr = 0;
	static s16 PIB_avr = 0;
	static s16 PIC_avr = 0;
	static s16 U0_avr = 0;
	static s16 I0_avr = 0;
	static s16 UA_avr = 0;
	static s16 UB_avr = 0;
	static s16 UC_avr = 0;
	static s16 UPT1_avr = 0;
	static s16 UPT2_avr = 0;
	
	static s32 IA_sum = 0;
	static s32 IB_sum = 0;
	static s32 IC_sum = 0;
	static s32 PIA_sum = 0;
	static s32 PIB_sum = 0;
	static s32 PIC_sum = 0;
	static s32 U0_sum = 0;
	static s32 I0_sum = 0;
	static s32 UA_sum = 0;
	static s32 UB_sum = 0;
	static s32 UC_sum = 0;
	static s32 UPT1_sum = 0;
	static s32 UPT2_sum = 0;
  static u8 AD_Period = 0;		// AD采样周期
	static u8 CurvesaveFlag = 0;
	static u16 CurvesaveCnt = 0;
	
  if (EXTI_GetITStatus(EXTI_Line13)!=RESET)	
	{
		if(DMA_GetCurrDataCounter(DMA2_Stream4)<900)
		  DMAADCReadIndex = (900 - DMA_GetCurrDataCounter(DMA2_Stream4)-1)/9;
    ADC7606DATA1 = *(s16*) Bank1_SRAM1_ADDR;
		AD_IA_val = ADC7606DATA1>>2;
		Delay(2);
		ADC7606DATA2 = *(s16*) Bank1_SRAM1_ADDR;
		AD_IB_val = ADC7606DATA2>>2;
		Delay(2);
		ADC7606DATA3 = *(s16*) Bank1_SRAM1_ADDR;
		AD_IC_val = ADC7606DATA3>>2;
		Delay(2);
		ADC7606DATA4 = *(s16*) Bank1_SRAM1_ADDR;
		AD_PIA_val = ADC7606DATA4>>2;
		Delay(2);
		ADC7606DATA5 = *(s16*) Bank1_SRAM1_ADDR;
		AD_PIB_val = ADC7606DATA5>>2;
		Delay(2);
		ADC7606DATA6 = *(s16*) Bank1_SRAM1_ADDR;
		AD_PIC_val = ADC7606DATA6>>2;
		Delay(2);
		ADC7606DATA7 = *(s16*) Bank1_SRAM1_ADDR;
		AD_U0_val = ADC7606DATA7>>2;
		Delay(2);
		ADC7606DATA8 = *(s16*) Bank1_SRAM1_ADDR;
		AD_I0_val = ADC7606DATA8>>2;
		
	  if(AD_Index == 0)
	  {
			MeasureData.IAsum = IA_sum1 + IA_sum2 + IA_sum3 + IA_sum4;
		  MeasureData.IBsum = IB_sum1 + IB_sum2 + IB_sum3 + IB_sum4;
		  MeasureData.ICsum = IC_sum1 + IC_sum2 + IC_sum3 + IC_sum4;
			MeasureData.PIAsum = PIA_sum1 + PIA_sum2 + PIA_sum3 + PIA_sum4;
		  MeasureData.PIBsum = PIB_sum1 + PIB_sum2 + PIB_sum3 + PIB_sum4;
		  MeasureData.PICsum = PIC_sum1 + PIC_sum2 + PIC_sum3 + PIC_sum4;
			MeasureData.U0sum = U0_sum1 + U0_sum2 + U0_sum3 + U0_sum4;
			MeasureData.I0sum = I0_sum1 + I0_sum2 + I0_sum3 + I0_sum4;
			MeasureData.PI0sum = PI0_sum1 + PI0_sum2 + PI0_sum3 + PI0_sum4;
			MeasureData.IASDsum = IA_sum3 + IA_sum4;
			MeasureData.IBSDsum = IB_sum3 + IB_sum4;
			MeasureData.ICSDsum = IC_sum3 + IC_sum4;
			MeasureData.PIASDsum = PIA_sum3 + PIA_sum4;
			MeasureData.PIBSDsum = PIB_sum3 + PIB_sum4;
			MeasureData.PICSDsum = PIC_sum3 + PIC_sum4;
			MeasureReady = 1;
			
			IA_sum1 = ((s32)AD_IA_val - (s32)IA_avr) * ((s32)AD_IA_val - (s32)IA_avr);
			IB_sum1 = ((s32)AD_IB_val - (s32)IB_avr)*((s32)AD_IB_val - (s32)IB_avr);
			IC_sum1 = ((s32)AD_IC_val - (s32)IC_avr)*((s32)AD_IC_val - (s32)IC_avr);
			PIA_sum1 = ((s32)AD_PIA_val - (s32)PIA_avr)*((s32)AD_PIA_val - (s32)PIA_avr);
			PIB_sum1 = ((s32)AD_PIB_val - (s32)PIB_avr)*((s32)AD_PIB_val - (s32)PIB_avr);
			PIC_sum1 = ((s32)AD_PIC_val - (s32)PIC_avr)*((s32)AD_PIC_val - (s32)PIC_avr);
			U0_sum1 = ((s32)AD_U0_val - (s32)U0_avr)*((s32)AD_U0_val - (s32)U0_avr);
			I0_sum1 = ((s32)AD_I0_val - (s32)I0_avr)*((s32)AD_I0_val - (s32)I0_avr);	
			PI0_sum1 = ((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr)
			          *((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr);	
		}
		else if(AD_Index > 0 && AD_Index < 25)
	  {
			IA_sum1 += ((s32)AD_IA_val - (s32)IA_avr) * ((s32)AD_IA_val - (s32)IA_avr);
			IB_sum1 += ((s32)AD_IB_val - (s32)IB_avr)*((s32)AD_IB_val - (s32)IB_avr);
			IC_sum1 += ((s32)AD_IC_val - (s32)IC_avr)*((s32)AD_IC_val - (s32)IC_avr);
			PIA_sum1 += ((s32)AD_PIA_val - (s32)PIA_avr)*((s32)AD_PIA_val - (s32)PIA_avr);
			PIB_sum1 += ((s32)AD_PIB_val - (s32)PIB_avr)*((s32)AD_PIB_val - (s32)PIB_avr);
			PIC_sum1 += ((s32)AD_PIC_val - (s32)PIC_avr)*((s32)AD_PIC_val - (s32)PIC_avr);
			U0_sum1 += ((s32)AD_U0_val - (s32)U0_avr)*((s32)AD_U0_val - (s32)U0_avr);
			I0_sum1 += ((s32)AD_I0_val - (s32)I0_avr)*((s32)AD_I0_val - (s32)I0_avr);	
			PI0_sum1 += ((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr)
			           *((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr);	
		}
		else if(AD_Index == 25)
	  {
			MeasureData.IAsum = IA_sum1 + IA_sum2 + IA_sum3 + IA_sum4;
		  MeasureData.IBsum = IB_sum1 + IB_sum2 + IB_sum3 + IB_sum4;
		  MeasureData.ICsum = IC_sum1 + IC_sum2 + IC_sum3 + IC_sum4;
			MeasureData.PIAsum = PIA_sum1 + PIA_sum2 + PIA_sum3 + PIA_sum4;
		  MeasureData.PIBsum = PIB_sum1 + PIB_sum2 + PIB_sum3 + PIB_sum4;
		  MeasureData.PICsum = PIC_sum1 + PIC_sum2 + PIC_sum3 + PIC_sum4;
			MeasureData.U0sum = U0_sum1 + U0_sum2 + U0_sum3 + U0_sum4;
			MeasureData.I0sum = I0_sum1 + I0_sum2 + I0_sum3 + I0_sum4;
			MeasureData.PI0sum = PI0_sum1 + PI0_sum2 + PI0_sum3 + PI0_sum4;
			MeasureData.IASDsum = IA_sum1 + IA_sum4;
			MeasureData.IBSDsum = IB_sum1 + IB_sum4;
			MeasureData.ICSDsum = IC_sum1 + IC_sum4;
			MeasureData.PIASDsum = PIA_sum1 + PIA_sum4;
			MeasureData.PIBSDsum = PIB_sum1 + PIB_sum4;
			MeasureData.PICSDsum = PIC_sum1 + PIC_sum4;
			MeasureReady = 1;
			
			IA_sum2 = ((s32)AD_IA_val - (s32)IA_avr) * ((s32)AD_IA_val - (s32)IA_avr);
			IB_sum2 = ((s32)AD_IB_val - (s32)IB_avr)*((s32)AD_IB_val - (s32)IB_avr);
			IC_sum2 = ((s32)AD_IC_val - (s32)IC_avr)*((s32)AD_IC_val - (s32)IC_avr);
			PIA_sum2 = ((s32)AD_PIA_val - (s32)PIA_avr)*((s32)AD_PIA_val - (s32)PIA_avr);
			PIB_sum2 = ((s32)AD_PIB_val - (s32)PIB_avr)*((s32)AD_PIB_val - (s32)PIB_avr);
			PIC_sum2 = ((s32)AD_PIC_val - (s32)PIC_avr)*((s32)AD_PIC_val - (s32)PIC_avr);
			U0_sum2 = ((s32)AD_U0_val - (s32)U0_avr)*((s32)AD_U0_val - (s32)U0_avr);
			I0_sum2 = ((s32)AD_I0_val - (s32)I0_avr)*((s32)AD_I0_val - (s32)I0_avr);	
			PI0_sum2 = ((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr)
			          *((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr);
		}
		else if(AD_Index > 25 && AD_Index < 50)
	  {
			IA_sum2 += ((s32)AD_IA_val - (s32)IA_avr) * ((s32)AD_IA_val - (s32)IA_avr) ;
			IB_sum2 += ((s32)AD_IB_val - (s32)IB_avr)*((s32)AD_IB_val - (s32)IB_avr) ;
			IC_sum2 += ((s32)AD_IC_val - (s32)IC_avr)*((s32)AD_IC_val - (s32)IC_avr) ;
			PIA_sum2 += ((s32)AD_PIA_val - (s32)PIA_avr)*((s32)AD_PIA_val - (s32)PIA_avr) ;
			PIB_sum2 += ((s32)AD_PIB_val - (s32)PIB_avr)*((s32)AD_PIB_val - (s32)PIB_avr) ;
			PIC_sum2 += ((s32)AD_PIC_val - (s32)PIC_avr)*((s32)AD_PIC_val - (s32)PIC_avr) ;
			U0_sum2 += ((s32)AD_U0_val - (s32)U0_avr)*((s32)AD_U0_val - (s32)U0_avr) ;
			I0_sum2 += ((s32)AD_I0_val - (s32)I0_avr)*((s32)AD_I0_val - (s32)I0_avr) ;	
			PI0_sum2 += ((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr)
			           *((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr);	
		}
		else if(AD_Index == 50)
	  {
			MeasureData.IAsum = IA_sum1 + IA_sum2 + IA_sum3 + IA_sum4;
		  MeasureData.IBsum = IB_sum1 + IB_sum2 + IB_sum3 + IB_sum4;
		  MeasureData.ICsum = IC_sum1 + IC_sum2 + IC_sum3 + IC_sum4;
			MeasureData.PIAsum = PIA_sum1 + PIA_sum2 + PIA_sum3 + PIA_sum4;
		  MeasureData.PIBsum = PIB_sum1 + PIB_sum2 + PIB_sum3 + PIB_sum4;
		  MeasureData.PICsum = PIC_sum1 + PIC_sum2 + PIC_sum3 + PIC_sum4;
			MeasureData.U0sum = U0_sum1 + U0_sum2 + U0_sum3 + U0_sum4;
			MeasureData.I0sum = I0_sum1 + I0_sum2 + I0_sum3 + I0_sum4;
			MeasureData.PI0sum = PI0_sum1 + PI0_sum2 + PI0_sum3 + PI0_sum4;
			MeasureData.IASDsum = IA_sum1 + IA_sum2;
			MeasureData.IBSDsum = IB_sum1 + IB_sum2;
			MeasureData.ICSDsum = IC_sum1 + IC_sum2;
			MeasureData.PIASDsum = PIA_sum1 + PIA_sum2;
			MeasureData.PIBSDsum = PIB_sum1 + PIB_sum2;
			MeasureData.PICSDsum = PIC_sum1 + PIC_sum2;
			MeasureReady = 1;
			
			IA_sum3 = ((s32)AD_IA_val - (s32)IA_avr) * ((s32)AD_IA_val - (s32)IA_avr) ;
			IB_sum3 = ((s32)AD_IB_val - (s32)IB_avr)*((s32)AD_IB_val - (s32)IB_avr) ;
			IC_sum3 = ((s32)AD_IC_val - (s32)IC_avr)*((s32)AD_IC_val - (s32)IC_avr) ;
			PIA_sum3 = ((s32)AD_PIA_val - (s32)PIA_avr)*((s32)AD_PIA_val - (s32)PIA_avr) ;
			PIB_sum3 = ((s32)AD_PIB_val - (s32)PIB_avr)*((s32)AD_PIB_val - (s32)PIB_avr) ;
			PIC_sum3 = ((s32)AD_PIC_val - (s32)PIC_avr)*((s32)AD_PIC_val - (s32)PIC_avr) ;
			U0_sum3 = ((s32)AD_U0_val - (s32)U0_avr)*((s32)AD_U0_val - (s32)U0_avr) ;
			I0_sum3 = ((s32)AD_I0_val - (s32)I0_avr)*((s32)AD_I0_val - (s32)I0_avr) ;	
			PI0_sum3 = ((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr)
			          *((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr);
		}
		else if(AD_Index > 50 && AD_Index < 75)
	  {
			IA_sum3 += ((s32)AD_IA_val - (s32)IA_avr) * ((s32)AD_IA_val - (s32)IA_avr) ;
			IB_sum3 += ((s32)AD_IB_val - (s32)IB_avr)*((s32)AD_IB_val - (s32)IB_avr) ;
			IC_sum3 += ((s32)AD_IC_val - (s32)IC_avr)*((s32)AD_IC_val - (s32)IC_avr) ;
			PIA_sum3 += ((s32)AD_PIA_val - (s32)PIA_avr)*((s32)AD_PIA_val - (s32)PIA_avr) ;
			PIB_sum3 += ((s32)AD_PIB_val - (s32)PIB_avr)*((s32)AD_PIB_val - (s32)PIB_avr) ;
			PIC_sum3 += ((s32)AD_PIC_val - (s32)PIC_avr)*((s32)AD_PIC_val - (s32)PIC_avr) ;
			U0_sum3 += ((s32)AD_U0_val - (s32)U0_avr)*((s32)AD_U0_val - (s32)U0_avr) ;
			I0_sum3 += ((s32)AD_I0_val - (s32)I0_avr)*((s32)AD_I0_val - (s32)I0_avr) ;	
			PI0_sum3 += ((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr)
			           *((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr);	
		}
		else if(AD_Index == 75)
	  {
			MeasureData.IAsum = IA_sum1 + IA_sum2 + IA_sum3 + IA_sum4;
		  MeasureData.IBsum = IB_sum1 + IB_sum2 + IB_sum3 + IB_sum4;
		  MeasureData.ICsum = IC_sum1 + IC_sum2 + IC_sum3 + IC_sum4;
			MeasureData.PIAsum = PIA_sum1 + PIA_sum2 + PIA_sum3 + PIA_sum4;
		  MeasureData.PIBsum = PIB_sum1 + PIB_sum2 + PIB_sum3 + PIB_sum4;
		  MeasureData.PICsum = PIC_sum1 + PIC_sum2 + PIC_sum3 + PIC_sum4;
			MeasureData.U0sum = U0_sum1 + U0_sum2 + U0_sum3 + U0_sum4;
			MeasureData.I0sum = I0_sum1 + I0_sum2 + I0_sum3 + I0_sum4;
			MeasureData.PI0sum = PI0_sum1 + PI0_sum2 + PI0_sum3 + PI0_sum4;
			MeasureData.IASDsum = IA_sum2 + IA_sum3;
			MeasureData.IBSDsum = IB_sum2 + IB_sum3;
			MeasureData.ICSDsum = IC_sum2 + IC_sum3;
			MeasureData.PIASDsum = PIA_sum2 + PIA_sum3;
			MeasureData.PIBSDsum = PIB_sum2 + PIB_sum3;
			MeasureData.PICSDsum = PIC_sum2 + PIC_sum3;
			MeasureReady = 1;
			
			IA_sum4 = ((s32)AD_IA_val - (s32)IA_avr) * ((s32)AD_IA_val - (s32)IA_avr) ;
			IB_sum4 = ((s32)AD_IB_val - (s32)IB_avr)*((s32)AD_IB_val - (s32)IB_avr) ;
			IC_sum4 = ((s32)AD_IC_val - (s32)IC_avr)*((s32)AD_IC_val - (s32)IC_avr) ;
			PIA_sum4 = ((s32)AD_PIA_val - (s32)PIA_avr)*((s32)AD_PIA_val - (s32)PIA_avr) ;
			PIB_sum4 = ((s32)AD_PIB_val - (s32)PIB_avr)*((s32)AD_PIB_val - (s32)PIB_avr) ;
			PIC_sum4 = ((s32)AD_PIC_val - (s32)PIC_avr)*((s32)AD_PIC_val - (s32)PIC_avr) ;
			U0_sum4 = ((s32)AD_U0_val - (s32)U0_avr)*((s32)AD_U0_val - (s32)U0_avr) ;
			I0_sum4 = ((s32)AD_I0_val - (s32)I0_avr)*((s32)AD_I0_val - (s32)I0_avr) ;	
			PI0_sum4 = ((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr)
			          *((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr);
		}
		else if(AD_Index > 75 && AD_Index < 100)
	  {
			IA_sum4 += ((s32)AD_IA_val - (s32)IA_avr) * ((s32)AD_IA_val - (s32)IA_avr) ;
			IB_sum4 += ((s32)AD_IB_val - (s32)IB_avr)*((s32)AD_IB_val - (s32)IB_avr) ;
			IC_sum4 += ((s32)AD_IC_val - (s32)IC_avr)*((s32)AD_IC_val - (s32)IC_avr) ;
			PIA_sum4 += ((s32)AD_PIA_val - (s32)PIA_avr)*((s32)AD_PIA_val - (s32)PIA_avr) ;
			PIB_sum4 += ((s32)AD_PIB_val - (s32)PIB_avr)*((s32)AD_PIB_val - (s32)PIB_avr) ;
			PIC_sum4 += ((s32)AD_PIC_val - (s32)PIC_avr)*((s32)AD_PIC_val - (s32)PIC_avr) ;
			U0_sum4 += ((s32)AD_U0_val - (s32)U0_avr)*((s32)AD_U0_val - (s32)U0_avr) ;
			I0_sum4 += ((s32)AD_I0_val - (s32)I0_avr)*((s32)AD_I0_val - (s32)I0_avr) ;	
			PI0_sum4 += ((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr)
			           *((s32)AD_IA_val - (s32)IA_avr + (s32)AD_IB_val - (s32)IB_avr + (s32)AD_IC_val - (s32)IC_avr);	
		}
		
		IA_sum += AD_IA_val;
		IB_sum += AD_IB_val;
		IC_sum += AD_IC_val;
		PIA_sum += AD_PIA_val;
		PIB_sum += AD_PIB_val;
		PIC_sum += AD_PIC_val;
		U0_sum += AD_U0_val;
		I0_sum += AD_I0_val;
		UA_sum += DMAADCbuffer1[DMAADCReadIndex][3];
	  UB_sum += DMAADCbuffer1[DMAADCReadIndex][4];
	  UC_sum += DMAADCbuffer1[DMAADCReadIndex][5];
	  UPT1_sum += DMAADCbuffer1[DMAADCReadIndex][0];
	  UPT2_sum += DMAADCbuffer1[DMAADCReadIndex][2];
		
		Avr_cnt++;
		if(Avr_cnt > 1999)
		{
			Avr_cnt = 0;
			if(abs(IA_sum_last - IA_sum) < 10000)
				IA_avr = IA_sum/2000;
			if(abs(IB_sum_last - IB_sum) < 10000)
				IB_avr = IB_sum/2000;
			if(abs(IC_sum_last - IC_sum) < 10000)
				IC_avr = IC_sum/2000;
			if(abs(PIA_sum_last - PIA_sum) < 10000)
				PIA_avr = PIA_sum/2000;
			if(abs(PIB_sum_last - PIB_sum) < 10000)
				PIB_avr = PIB_sum/2000;
			if(abs(PIC_sum_last - PIC_sum) < 10000)
				PIC_avr = PIC_sum/2000;
			if(abs(U0_sum_last - U0_sum) < 1000)
				U0_avr = U0_sum/2000;
			if(abs(I0_sum_last - I0_sum) < 10000)
				I0_avr = I0_sum/2000;
			if(abs(UA_sum_last - UA_sum) < 1000)
				UA_avr = UA_sum/2000;
			if(abs(UB_sum_last - UB_sum) < 1000)
				UB_avr = UB_sum/2000;
			if(abs(UC_sum_last - UC_sum) < 1000)
				UC_avr = UC_sum/2000;
			if(abs(UPT1_sum_last - UPT1_sum) < 1000)
				UPT1_avr = UPT1_sum/2000;
			if(abs(UPT2_sum_last - UPT2_sum) < 1000)
				UPT2_avr = UPT2_sum/2000;
			IA_sum_last = IA_sum;
			IB_sum_last = IB_sum;
			IC_sum_last = IC_sum;
			PIA_sum_last = PIA_sum;
			PIB_sum_last = PIB_sum;
			PIC_sum_last = PIC_sum;
			U0_sum_last = U0_sum;
			I0_sum_last = I0_sum;
			UA_sum_last = UA_sum;
			UB_sum_last = UB_sum;
			UC_sum_last = UC_sum;
			UPT1_sum_last = UPT1_sum;
			UPT2_sum_last = UPT2_sum;
			
			IA_sum = 0;
			IB_sum = 0;
			IC_sum = 0;
			PIA_sum = 0;
			PIB_sum = 0;
			PIC_sum = 0;
			U0_sum = 0;
			I0_sum = 0;
			UA_sum = 0;
			UB_sum = 0;
			UC_sum = 0;
			UPT1_sum = 0;
			UPT2_sum = 0;
			
			ZeroPointIA = abs(IA_avr);
			ZeroPointIA10 = abs(PIA_avr);
			ZeroPointIB = abs(IB_avr);
			ZeroPointIB10 = abs(PIB_avr);
			ZeroPointIC = abs(IC_avr);
			ZeroPointIC10 = abs(PIC_avr);
		}
    if(MeasureData.Pstartflag)
		{
			MeasureData.Pstartflag = 0;
			CurvesaveFlag = 1;
			CurvesaveCnt = 0;
			MeasureData.Psaveflag = 0;
		}
		if(CurvesaveFlag)
		  CurvesaveCnt++;
		if(CurvesaveCnt > 600) 
		{
			AD_StartPoint = 100*AD_Period + AD_Index;
			MeasureData.Psaveflag = 0x01;
			CurvesaveFlag = 0;
			CurvesaveCnt = 0;
		}

		if(RealCurveFlag == 0x01 && MeasureData.Psaveflag == 0)
		{
			AD_StartPoint = 100*AD_Period + AD_Index;
			RealCurveFlag = 0x11;
		}
		if(MeasureData.Psaveflag == 0 && RealCurveFlag == 0)
		{
			//标准源为100V时，测得输入电压为3.25V,表示电压为100V;
			//电压的采样基准电压 1.506V; +/-1.414+1.506 = 0.09——2.92V / 3.3V *4096 = 123——3987;
			PcurveUA[100*AD_Period + AD_Index] = DMAADCbuffer1[DMAADCReadIndex][3];  
			PcurveUB[100*AD_Period + AD_Index] = DMAADCbuffer1[DMAADCReadIndex][4];  
			PcurveUC[100*AD_Period + AD_Index] = DMAADCbuffer1[DMAADCReadIndex][5]; 
			if(SystemSet.para.exchange_dycfzc == 0)	
			{
			  //标准源为5A时，测得输入电压为0.2V,表示保护电流为5A
			  //保护电流的采样电压 +/-0.22 V / 5V * 32768 /4 = +/-360 (AD_PIA_val值右移缩小了4倍)
			  PcurveIA[100*AD_Period + AD_Index] =  AD_IA_val - IA_avr;  //+/-0.22V的正弦波
			  PcurveIB[100*AD_Period + AD_Index] =  AD_IB_val - IB_avr;  //+/-0.22V的正弦波
			  PcurveIC[100*AD_Period + AD_Index] =  AD_IC_val - IC_avr;  //+/-0.22V的正弦波
				//标准源为5A时，测得输入电压为0.2V,表示零序电流为20A
			  //零序电流的采样电压 +/-3.535 V / 5V * 32768 /4 = +/-5792 (AD_I0_val值右移缩小了4倍)
			  PcurveI0[100*AD_Period + AD_Index] =  AD_I0_val - I0_avr;  //+/-3.535的正弦波	
			}
			else
			{
				//标准源为5A时，测得输入电压为0.2V,表示保护电流为5A
			  //保护电流的采样电压 +/-0.22 V / 5V * 32768 /4 = +/-360 (AD_PIA_val值右移缩小了4倍)
			  PcurveIA[100*AD_Period + AD_Index] =  IA_avr - AD_IA_val;  //+/-0.22V的正弦波
			  PcurveIB[100*AD_Period + AD_Index] =  IB_avr - AD_IB_val;  //+/-0.22V的正弦波
			  PcurveIC[100*AD_Period + AD_Index] =  IC_avr - AD_IC_val;  //+/-0.22V的正弦波
				//标准源为5A时，测得输入电压为0.2V,表示零序电流为20A
			  //零序电流的采样电压 +/-3.535 V / 5V * 32768 /4 = +/-5792 (AD_I0_val值右移缩小了4倍)
			  PcurveI0[100*AD_Period + AD_Index] =  I0_avr - AD_I0_val;  //+/-3.535的正弦波	
			}
			//标准源为100V时，测得输入电压为3.75V,表示电压为100V;
			//零序电压 +/-0.84 V/ 5V * 32768 /4 = +/-1376 (AD_U0_val值右移缩小了4倍)
			PcurveU0[100*AD_Period + AD_Index] =  AD_U0_val - U0_avr;  //+/-0.84V的正弦波
		}
		
		AD_Index++;
		if(AD_Index>99)		
		{
			AD_Index=0;
			AD_Period++;
			if(AD_Period>11)
				AD_Period = 0;
		}
	
		//标准源为100V时，测得输入电压为3.25V,表示保护电压为100V
		//保护电压的采样电压=+/-2344(AD_UA_val-MeasureData.UA_avr值右移缩小了4倍)*4/32768*5V
		MeasureCurve.UAX[AD_Index] = DMAADCbuffer1[DMAADCReadIndex][3]-UA_avr; //正负1.43V的正弦波
		MeasureCurve.UBX[AD_Index] = DMAADCbuffer1[DMAADCReadIndex][4]-UB_avr; //正负1.43V的正弦波
		MeasureCurve.UCX[AD_Index] = DMAADCbuffer1[DMAADCReadIndex][5]-UC_avr; //正负1.43V的正弦波
		MeasureCurve.UABX[AD_Index] = MeasureCurve.UAX[AD_Index] - MeasureCurve.UBX[AD_Index]; //采样值为4010,2344*1.732=4059
		MeasureCurve.UBCX[AD_Index] = MeasureCurve.UBX[AD_Index] - MeasureCurve.UCX[AD_Index]; 
		MeasureCurve.UCAX[AD_Index] = MeasureCurve.UCX[AD_Index] - MeasureCurve.UAX[AD_Index];
		//标准源为100V时，测得输入电压为3.752V,表示零序电压为100V
		//零序电压的采样电压=+/-1390(AD_U0_val-MeasureData.U0_avr值右移缩小了4倍)*4/32768*5V
		MeasureCurve.U0X[AD_Index] = AD_U0_val - U0_avr; //正负0.848V的正弦波
		
		if(SystemSet.para.exchange_dycfzc == 0)	
		{
			//标准源为5A时，测得输入电压为0.2V,表示零序电流为20A
			//零序电流的采样电压=+/-5780(AD_I0_val-MeasureData.I0_avr值右移缩小了4倍)*4/32768*5V
			MeasureCurve.I0X[AD_Index] = AD_I0_val - I0_avr; //正负3.527V的正弦波
		  //标准源为100V时，测得输入电压为1V,表示测量电流为5A
		  //测量电流的采样电压=+/-1894(AD_IA_val-MeasureData.IA_avr值)/4096*3V
		  MeasureCurve.IAX[AD_Index] = AD_IA_val- IA_avr;   //正负1.387V的正弦波
		  MeasureCurve.IBX[AD_Index] = AD_IB_val- IB_avr;   //正负1.387V的正弦波
		  MeasureCurve.ICX[AD_Index] = AD_IC_val- IC_avr;   //正负1.387V的正弦波
		}
		else
		{
			//标准源为5A时，测得输入电压为0.2V,表示零序电流为20A
			//零序电流的采样电压=+/-5780(AD_I0_val-MeasureData.I0_avr值右移缩小了4倍)*4/32768*5V
			MeasureCurve.I0X[AD_Index] = I0_avr - AD_I0_val; //正负3.527V的正弦波
			//标准源为100V时，测得输入电压为1V,表示测量电流为5A
		  //测量电流的采样电压=+/-1894(AD_IA_val-MeasureData.IA_avr值)/4096*3V
		  MeasureCurve.IAX[AD_Index] = IA_avr - AD_IA_val;   //正负1.387V的正弦波
		  MeasureCurve.IBX[AD_Index] = IB_avr - AD_IB_val;   //正负1.387V的正弦波
		  MeasureCurve.ICX[AD_Index] = IC_avr - AD_IC_val;   //正负1.387V的正弦波
		}
		MeasureCurve.UPT1X[AD_Index] = DMAADCbuffer1[DMAADCReadIndex][0] - UPT1_avr;
		MeasureCurve.UPT2X[AD_Index] = DMAADCbuffer1[DMAADCReadIndex][2] - UPT2_avr;
		
		if(CtrlMeasureFlag)
		{					
			switch(CtrlMeasureFlag) //400个点                    
			{
				case 1:		// 操作合闸
					MyCurve.CurveA.On.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][3]; //单位V
					MyCurve.CurveB.On.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][4]; //单位V
					MyCurve.CurveC.On.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][5]; //单位V
					
					MyCurve.CurveA.On.word.current[CtrlMeasureCnt] = AD_IA_val;//单位A
          MyCurve.CurveB.On.word.current[CtrlMeasureCnt] = AD_IB_val;//单位A
					MyCurve.CurveC.On.word.current[CtrlMeasureCnt] = AD_IC_val;//单位A
						
					if( HW_IN2 != 0) 
					{
						MyCurve.CurveA.On.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
						MyCurve.CurveB.On.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
						MyCurve.CurveC.On.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
					}
					else
					{
						MyCurve.CurveA.On.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
						MyCurve.CurveB.On.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
						MyCurve.CurveC.On.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
					}
					break;
				case 2:		// 操作分闸
					MyCurve.CurveA.Off.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][3]; //单位V
					MyCurve.CurveB.Off.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][4]; //单位V
					MyCurve.CurveC.Off.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][5]; //单位V
					
					MyCurve.CurveA.Off.word.current[CtrlMeasureCnt] = AD_IA_val;//单位A
          MyCurve.CurveB.Off.word.current[CtrlMeasureCnt] = AD_IB_val;//单位A
					MyCurve.CurveC.Off.word.current[CtrlMeasureCnt] = AD_IC_val;//单位A
					   
					if( HW_IN2 != 0) 
					{
						MyCurve.CurveA.Off.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
						MyCurve.CurveB.Off.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
						MyCurve.CurveC.Off.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
					}
					else
					{
						MyCurve.CurveA.Off.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
						MyCurve.CurveB.Off.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
						MyCurve.CurveC.Off.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
					}
					break;
				case 3:		// 保护分闸
					MyCurve.CurveA.Protect.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][3]; //单位V
					MyCurve.CurveB.Protect.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][4]; //单位V
					MyCurve.CurveC.Protect.word.voltage[CtrlMeasureCnt] = DMAADCbuffer1[DMAADCReadIndex][5]; //单位V
					
					MyCurve.CurveA.Protect.word.current[CtrlMeasureCnt] = AD_PIA_val;//单位A
          MyCurve.CurveB.Protect.word.current[CtrlMeasureCnt] = AD_PIB_val;//单位A
					MyCurve.CurveC.Protect.word.current[CtrlMeasureCnt] = AD_PIC_val;//单位A
					   
					if( HW_IN2 != 0) 
					{
						MyCurve.CurveA.Protect.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
						MyCurve.CurveB.Protect.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
						MyCurve.CurveC.Protect.word.position[CtrlMeasureCnt/8] |= (1<<(CtrlMeasureCnt%8));
					}
					else
					{
						MyCurve.CurveA.Protect.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
						MyCurve.CurveB.Protect.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
						MyCurve.CurveC.Protect.word.position[CtrlMeasureCnt/8] &= ~(1<<(CtrlMeasureCnt%8));
					}	
					break;					
				default:
					break;
			}
			if(CtrlMeasureCnt < 399)
				CtrlMeasureCnt++;
			else
			{
				DLQZD_flag = CtrlMeasureFlag;	// 置诊断信息计算标志
				CtrlMeasureFlag = 0;					// 结束采样
			}
		}	
	  EXTI_ClearITPendingBit(EXTI_Line13);
  }
}
		 
void USART1_IRQHandler(void)
{
 		u16 i = 0, cnt = 0;
 		if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
 		{
 				USART_ClearFlag(USART1,USART_FLAG_ORE);
 				USART_ReceiveData(USART1);
 		}
   	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
   	{
        USART_ClearFlag(USART1, USART_FLAG_TC);
    }
  	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)	 
  	{
        i = USART1->SR;  
        i = USART1->DR; 
				cnt =50 - DMA_GetCurrDataCounter(DMA2_Stream5);
				USART_ClearFlag(USART1, USART_FLAG_IDLE);
			if(DMAReadIndex1 != cnt) 
				{
					if(cnt < 25)
					{
						for(i=DMAReadIndex1;i<cnt;i++)
						{
							#ifdef Four_Faith_MODEL
							  ProtocolResolveEncryptionData(DMARXbuffer1[i]);		   // GPRS数据处理
							#endif
							#ifdef SIM800C_MODEL
							  ProtocolResolveGprsData(DMARXbuffer1[i]);		   // GPRS数据处理
		            ProtocolResolveGprsState(DMARXbuffer1[i]);
							#endif
							#ifdef WD_MODEL
							  ProtocolResolve1(DMARXbuffer1[i]);		          // GPRS数据处理
							#endif
				      if(Start_Debug)
				        InsertDataToQueue(&QueueTX5,BufferTX5,DMARXbuffer1[i]);//调试状态
							#ifdef DebugUART1
							InsertDataToQueue(&QueueTX2,BufferTX2,DMARXbuffer1[i]);
							#endif
						}
						DMAReadIndex1 = cnt;	
					}
					else
					{
						for(i=DMAReadIndex1;i<cnt;i++)
						{
							#ifdef Four_Faith_MODEL
							  ProtocolResolveEncryptionData(DMARXbuffer1[i]);		   // GPRS数据处理
							#endif
							#ifdef SIM800C_MODEL
							  ProtocolResolveGprsData(DMARXbuffer1[i]);		   // GPRS数据处理
		            ProtocolResolveGprsState(DMARXbuffer1[i]);
							#endif
							#ifdef WD_MODEL
							  ProtocolResolve1(DMARXbuffer1[i]);		          // GPRS数据处理
							#endif
							#ifdef DebugUART1
							InsertDataToQueue(&QueueTX2,BufferTX2,DMARXbuffer1[i]);//2019.12.11
							#endif
							if(Start_Debug)
				        InsertDataToQueue(&QueueTX5,BufferTX5,DMARXbuffer1[i]);//调试状态
						}
						DMAReadIndex1 = cnt;
					}			
				}
    }
}


void USART2_IRQHandler(void)
{
		u16 i = 0, cnt = 0;
		if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
		{
			USART_ClearFlag(USART2,USART_FLAG_ORE);
			USART_ReceiveData(USART2);
		}
  	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
  	{
      USART_ClearFlag(USART2, USART_FLAG_TC);
		  T485_2_RX;
    }
		if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)	 //
  	{
        i = USART2->SR;  
        i = USART2->DR; 
				cnt =50 - DMA_GetCurrDataCounter(DMA1_Stream5);
				USART_ClearFlag(USART2, USART_FLAG_IDLE);
				Usart2TxReady = 1;	// 485
				if(DMAReadIndex2 != cnt) 
				{
					if(cnt < 25)
					{
						for(i=DMAReadIndex2;i<cnt;i++)
						{
							ProtocolResolve2(DMARXbuffer2[i]);			// ??????
							
						}
						DMAReadIndex2 = cnt;	
					}
					else
					{
						for(i=DMAReadIndex2;i<cnt;i++)
						{
							ProtocolResolve2(DMARXbuffer2[i]);			// ??????
							
						}
						DMAReadIndex2 = cnt;
					}			
				}  
    }
}

void USART3_IRQHandler(void)
{
		u16 i = 0, cnt = 0;
		if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
		{
			USART_ClearFlag(USART3,USART_FLAG_ORE);
			USART_ReceiveData(USART3);
		}
  	if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  	{
        USART_ClearFlag(USART3, USART_FLAG_TC);
    }
		if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)	 // ??????
  	{
        i = USART3->SR;  
        i = USART3->DR; 
				cnt =50 - DMA_GetCurrDataCounter(DMA1_Stream1);
				USART_ClearFlag(USART3, USART_FLAG_IDLE);
				if(DMAReadIndex3 != cnt) 
				{
					if(cnt < 25)
					{
						for(i=DMAReadIndex3;i<cnt;i++)
						{
							IEC_104_Resolve(DMARXbuffer3[i]);					
						}
						DMAReadIndex3 = cnt;	
					}
					else
					{
						for(i=DMAReadIndex3;i<cnt;i++)
						{
							IEC_104_Resolve(DMARXbuffer3[i]);					
						}
						DMAReadIndex3 = cnt;
					}			
				}  
    }
}

void UART4_IRQHandler(void)
{
	u16 i = 0, cnt = 0;
		if(USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET)
		{
			USART_ClearFlag(UART4,USART_FLAG_ORE);
			USART_ReceiveData(UART4);
		}
  	if(USART_GetITStatus(UART4, USART_IT_TC) != RESET)
  	{
        USART_ClearFlag(UART4, USART_FLAG_TC);
    }
		if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)	 
  	{
        i = UART4->SR;  
        i = UART4->DR; 
				cnt =50 - DMA_GetCurrDataCounter(DMA1_Stream2);
				USART_ClearFlag(UART4, USART_FLAG_IDLE);
				if(DMAReadIndex4 != cnt) 
				{
					if(cnt < 25)
					{
						for(i=DMAReadIndex4;i<cnt;i++)
						{
							if(SystemSet.para.com_protocol == 88)
							  Encryption_Handle(DMARXbuffer4[i],1);
							else
								ProtocolResolve4(DMARXbuffer4[i]);		//线损数据接受				
						}
						DMAReadIndex4 = cnt;	
					}
					else
					{
						for(i=DMAReadIndex4;i<cnt;i++)
						{
							if(SystemSet.para.com_protocol == 88)
								Encryption_Handle(DMARXbuffer4[i],1);
							else
								ProtocolResolve4(DMARXbuffer4[i]);		//线损数据接受				
						 }
						DMAReadIndex4 = cnt;
					}			
				}  
    }
}

void UART5_IRQHandler(void)
{
		u16 i = 0, cnt = 0;
		if(USART_GetFlagStatus(UART5, USART_FLAG_ORE) != RESET)
		{
			USART_ClearFlag(UART5,USART_FLAG_ORE);
			USART_ReceiveData(UART5);
		}
  	if(USART_GetITStatus(UART5, USART_IT_TC) != RESET)
  	{
				USART_ClearFlag(UART5, USART_FLAG_TC);
		}
		if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)	
  	{
			  i = UART5->SR;                                     //读SR，DR就是清IDLE标志
        i = UART5->DR;		
				cnt =50 - DMA_GetCurrDataCounter(DMA1_Stream0);
				USART_ClearFlag(UART5, USART_FLAG_IDLE);
				if(DMAReadIndex5 != cnt) 
				{
					if(cnt < 25)
					{
						for(i=DMAReadIndex5;i<cnt;i++)
						{
							ProtocolResolve5(DMARXbuffer5[i]);		   // WIFI数据处理
							ProtocolResolve_Gps(DMARXbuffer5[i]);			 // GPS数据处理
							ProtocolResolveWIFIError(DMARXbuffer5[i]);
							#ifdef DebugUART5
								InsertDataToQueue(&QueueTX2,BufferTX2,DMARXbuffer5[i]);
							#endif
						}
						DMAReadIndex5 = cnt;	
					}
					else
					{
						for(i=DMAReadIndex5;i<cnt;i++)
						{
							ProtocolResolve5(DMARXbuffer5[i]);		   // WIFI数据处理
							ProtocolResolve_Gps(DMARXbuffer5[i]);			 // GPS数据处理
							ProtocolResolveWIFIError(DMARXbuffer5[i]);
							#ifdef DebugUART5
								InsertDataToQueue(&QueueTX2,BufferTX2,DMARXbuffer5[i]);
							#endif
						}
						DMAReadIndex5 = cnt;
					}			
				}				
    }
}

void USART6_IRQHandler(void)
{
		u16 i = 0, cnt = 0;
		if(USART_GetFlagStatus(USART6, USART_FLAG_ORE) != RESET)
		{
			USART_ClearFlag(USART6,USART_FLAG_ORE);
			USART_ReceiveData(USART6);
		}
  	if(USART_GetITStatus(USART6, USART_IT_TC) != RESET)
  	{
        USART_ClearFlag(USART6, USART_FLAG_TC);
    }
		if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)	 // ??????
  	{
        i = USART6->SR;  
        i = USART6->DR; 
				cnt =50 - DMA_GetCurrDataCounter(DMA2_Stream1);
				USART_ClearFlag(USART6, USART_FLAG_IDLE);
				if(DMAReadIndex6 != cnt) 
				{
					if(cnt < 25)
					{
						for(i=DMAReadIndex6;i<cnt;i++)
						{
							ProtocolResolve6(DMARXbuffer6[i]);
						}
						DMAReadIndex6 = cnt;	
					}
					else
					{
						for(i=DMAReadIndex6;i<cnt;i++)
						{
							ProtocolResolve6(DMARXbuffer6[i]);			// 			
						}
						DMAReadIndex6 = cnt;
					}			
				}  
    }
}
/******************************************************************************/
void DMA2_Stream7_IRQHandler(void)	// TX1
{
   u16 NumTemp = 0;
   u16 i = 0;
	 if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7) != RESET)	  	
   {
   	   DMA_Cmd(DMA2_Stream7,DISABLE);
			 DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
			 if(GetQueueDataNum(&QueueTX1) > 0 )									
			 {
					 NumTemp = GetQueueDataNum(&QueueTX1);
				 if(NumTemp > 299)
				 {
					for(i=0;i<300;i++)
					{
						DMATXbuffer1[i] = GetDataFromQueue(&QueueTX1,BufferTX1);
					}
					DMA2_Stream7->NDTR = 300;							
			    DMA_Cmd(DMA2_Stream7, ENABLE);	 					
				 }
				 else
				 {
					for(i=0;i<NumTemp;i++)
					{
							DMATXbuffer1[i] = GetDataFromQueue(&QueueTX1,BufferTX1);	
					}
					DMA2_Stream7->NDTR = NumTemp;							
					DMA_Cmd(DMA2_Stream7, ENABLE);	 					
				 }
			 }
   }
   if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TEIF7) != RESET)	  	
   {
				DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TEIF7);
   }
}

void DMA1_Stream6_IRQHandler(void)	// TX2
{
   u16 NumTemp = 0;
   u16 i = 0;
   if(DMA_GetITStatus(DMA1_Stream6,DMA_IT_TCIF6) != RESET)	  	
   {
			 DMA_Cmd(DMA1_Stream6,DISABLE);
			 DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
			 if(GetQueueDataNum(&QueueTX2) > 0 )									
			 {
					 NumTemp = GetQueueDataNum(&QueueTX2);
					 if(NumTemp > 819)
					 {
						  T485_2_TX;
							for(i=0;i<820;i++)
							{
								DMATXbuffer2[i] = GetDataFromQueue(&QueueTX2,BufferTX2);	
							}
							DMA1_Stream6->NDTR = 820;							
							DMA_Cmd(DMA1_Stream6, ENABLE);	 					
					 }
					 else
					 {
						  T485_2_TX;
							for(i=0;i<NumTemp;i++)
							{
									DMATXbuffer2[i] = GetDataFromQueue(&QueueTX2,BufferTX2);							
							}
							DMA1_Stream6->NDTR = NumTemp;							
							DMA_Cmd(DMA1_Stream6, ENABLE);	 					
					 }
			 
			 }
   }
   if(DMA_GetITStatus(DMA1_Stream6,DMA_IT_TEIF6) != RESET)	  	
   {
				DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TEIF6);  
   }
}
void DMA1_Stream3_IRQHandler(void)	// TX3
{
   u16 NumTemp = 0;
   u16 i = 0;
   if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3) != RESET)	  	
   {
   	   DMA_Cmd(DMA1_Stream3,DISABLE);
			 DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);
			 if(GetQueueDataNum(&QueueTX3) > 0 )									
			 {
					 NumTemp = GetQueueDataNum(&QueueTX3);
					 if(NumTemp > 99)
					 {
							for(i=0;i<100;i++)
							{
								DMATXbuffer3[i] = GetDataFromQueue(&QueueTX3,BufferTX3);	
							}
							DMA1_Stream3->NDTR = 100;							
							DMA_Cmd(DMA1_Stream3, ENABLE);	 					
					 }
					 else
					 {
							for(i=0;i<NumTemp;i++)
							{
									DMATXbuffer3[i] = GetDataFromQueue(&QueueTX3,BufferTX3);	
							}
							DMA1_Stream3->NDTR = NumTemp;							
							DMA_Cmd(DMA1_Stream3, ENABLE);	 					
					 }
			 }
   }
   if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TEIF3) != RESET)	  	
   {
				DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TEIF3);
   }
}
void DMA1_Stream4_IRQHandler(void)	// TX4
{
  u16 NumTemp = 0;
	u16 i = 0;	
	if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4) != RESET)	  	
   {
   	   DMA_Cmd(DMA1_Stream4,DISABLE);
			 DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
			 if(GetQueueDataNum(&QueueTX4) > 0 )									 
			 {
					 NumTemp = GetQueueDataNum(&QueueTX4);
					 if(NumTemp > 299)
					 {
							for(i=0;i<300;i++)
							{
								DMATXbuffer4[i] = GetDataFromQueue(&QueueTX4,BufferTX4);	
							}
							DMA1_Stream4->NDTR = 300;							
							DMA_Cmd(DMA1_Stream4, ENABLE);	 					
					 }
					 else
					 {
							for(i=0;i<NumTemp;i++)
							{
									DMATXbuffer4[i] = GetDataFromQueue(&QueueTX4,BufferTX4);	
							}
							DMA1_Stream4->NDTR = NumTemp;							
							DMA_Cmd(DMA1_Stream4, ENABLE);	 					
					 }
			 }
   }
   if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TEIF4) != RESET)	  	
   {
			DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TEIF4);
   }
}
void DMA1_Stream7_IRQHandler(void)	// TX5
{
   u16 NumTemp = 0;
   u16 i = 0;
   if(DMA_GetITStatus(DMA1_Stream7,DMA_IT_TCIF7) != RESET)	  	
   {
   	   DMA_Cmd(DMA1_Stream7,DISABLE);
			 DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7);
			 if(GetQueueDataNum(&QueueTX5) > 0 )									
			 {
					 NumTemp = GetQueueDataNum(&QueueTX5);
					 if(NumTemp > 419)
					 {
							for(i=0;i<420;i++)
							{
								DMATXbuffer5[i] = GetDataFromQueue(&QueueTX5,BufferTX5);	
							}
							DMA1_Stream7->NDTR = 420;							
							DMA_Cmd(DMA1_Stream7, ENABLE);	 					
					 }
					 else
					 {
							for(i=0;i<NumTemp;i++)
							{
							  DMATXbuffer5[i] = GetDataFromQueue(&QueueTX5,BufferTX5);	
							}
							DMA1_Stream7->NDTR = NumTemp;							
							DMA_Cmd(DMA1_Stream7, ENABLE);	 					
					 }
			 }
   }
   if(DMA_GetITStatus(DMA1_Stream7,DMA_IT_TEIF7) != RESET)	  	
   {
			DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TEIF7);
   }
}
void DMA2_Stream6_IRQHandler(void)	// TX6
{
  u16 NumTemp = 0;
	u16 i = 0;
	if(DMA_GetITStatus(DMA2_Stream6,DMA_IT_TCIF6) != RESET)	  	
   {
   	   DMA_Cmd(DMA2_Stream6,DISABLE);
			 DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TCIF6);
			 if(GetQueueDataNum(&QueueTX6) > 0 )									
			 {
					 NumTemp = GetQueueDataNum(&QueueTX6);
					 if(NumTemp > 99)
					 {
							for(i=0;i<100;i++)
							{
								DMATXbuffer6[i] = GetDataFromQueue(&QueueTX6,BufferTX6);	
							}
							DMA2_Stream6->NDTR = 100;							
							DMA_Cmd(DMA2_Stream6, ENABLE);	 					
					 }
					 else
					 {
							for(i=0;i<NumTemp;i++)
							{
									DMATXbuffer6[i] = GetDataFromQueue(&QueueTX6,BufferTX6);	
							}
							DMA2_Stream6->NDTR = NumTemp;							
							DMA_Cmd(DMA2_Stream6, ENABLE);	 					
					 }
			 }
   }
   if(DMA_GetITStatus(DMA2_Stream6,DMA_IT_TEIF6) != RESET)	  	
   {
			DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TEIF6);
   }
}
void DMA2_Stream5_IRQHandler(void)	// RX1
{
   u16 i = 0;
   if(DMA_GetITStatus(DMA2_Stream5,DMA_IT_TCIF5) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5);
	   	for(i=DMAReadIndex1;i<50;i++)
			{
				#ifdef Four_Faith_MODEL
					ProtocolResolveEncryptionData(DMARXbuffer1[i]);		   // GPRS数据处理
				#endif
				#ifdef SIM800C_MODEL
					ProtocolResolveGprsData(DMARXbuffer1[i]);		   // GPRS数据处理
					ProtocolResolveGprsState(DMARXbuffer1[i]);
				#endif
				#ifdef WD_MODEL
					ProtocolResolve1(DMARXbuffer1[i]);		          // GPRS数据处理
				#endif
				#ifdef DebugUART1
				InsertDataToQueue(&QueueTX2,BufferTX2,DMARXbuffer1[i]);//2019.12.11
				#endif
				if(Start_Debug)
				  InsertDataToQueue(&QueueTX5,BufferTX5,DMARXbuffer1[i]);//调试状态
			}
			DMAReadIndex1 = 0;
   }
   if(DMA_GetITStatus(DMA2_Stream5,DMA_IT_HTIF5) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_HTIF5);
	   	for(i=DMAReadIndex1;i<25;i++)
			{
				#ifdef Four_Faith_MODEL
					ProtocolResolveEncryptionData(DMARXbuffer1[i]);		   // GPRS数据处理
				#endif
				#ifdef SIM800C_MODEL
					ProtocolResolveGprsData(DMARXbuffer1[i]);		   // GPRS数据处理
					ProtocolResolveGprsState(DMARXbuffer1[i]);
				#endif
				#ifdef WD_MODEL
					ProtocolResolve1(DMARXbuffer1[i]);		          // GPRS数据处理
				#endif
				#ifdef DebugUART1
				InsertDataToQueue(&QueueTX2,BufferTX2,DMARXbuffer1[i]);//2019.12.11
				#endif
				if(Start_Debug)
				  InsertDataToQueue(&QueueTX5,BufferTX5,DMARXbuffer1[i]);//调试状态
			}
			DMAReadIndex1 = 25;
   }
   if(DMA_GetITStatus(DMA2_Stream5,DMA_IT_TEIF5) != RESET)	  	
   {
	   DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TEIF5);
   }
}
void DMA1_Stream5_IRQHandler(void)	// RX2
{
   u16 i = 0;
   if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5) != RESET)	  	
   {
   	   	Usart2TxReady = 0;	
				DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);
				for(i=DMAReadIndex2;i<50;i++)
				{
					ProtocolResolve2(DMARXbuffer2[i]);			
					
				}
				DMAReadIndex2 = 0;
   }
   if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_HTIF5) != RESET)	  	
   {
   	  Usart2TxReady = 0;	
	   	DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_HTIF5);
	   	for(i=DMAReadIndex2;i<25;i++)
			{
				ProtocolResolve2(DMARXbuffer2[i]);			
			}
			DMAReadIndex2 = 25;
   }
   if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TEIF5) != RESET)	  	
   {
	   DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TEIF5);
   }
}
void DMA1_Stream1_IRQHandler(void)	// RX3
{
   u16 i = 0;
   if(DMA_GetITStatus(DMA1_Stream1,DMA_IT_TCIF1) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_TCIF1);
	   	for(i=DMAReadIndex3;i<50;i++)
			{
        	IEC_104_Resolve(DMARXbuffer3[i]);					
			}
			DMAReadIndex3 = 0;
   }
   if(DMA_GetITStatus(DMA1_Stream1,DMA_IT_HTIF1) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_HTIF1);
	   	for(i=DMAReadIndex3;i<25;i++)
			{	
        	IEC_104_Resolve(DMARXbuffer3[i]);					
			}
			DMAReadIndex3 = 25;
   }
   if(DMA_GetITStatus(DMA1_Stream1,DMA_IT_TEIF1) != RESET)	  	
   {
	   DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_TEIF1);
   }
}
void DMA1_Stream2_IRQHandler(void)	// RX4
{
   u16 i = 0;
   if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA1_Stream2,DMA_FLAG_TCIF2);
	   	for(i=DMAReadIndex4;i<50;i++)
			{
        if(SystemSet.para.com_protocol == 88)
          Encryption_Handle(DMARXbuffer4[i],1);
        else
        	ProtocolResolve4(DMARXbuffer4[i]);		//线损数据接受						
			}
			DMAReadIndex4 = 0;
   }
   if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_HTIF2) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA1_Stream2,DMA_FLAG_HTIF2);
			for(i=DMAReadIndex4;i<25;i++)
			{
        if(SystemSet.para.com_protocol == 88)
          Encryption_Handle(DMARXbuffer4[i],1);
        else
        	ProtocolResolve4(DMARXbuffer4[i]);		//线损数据接受					
			}
			DMAReadIndex4 = 25;
   }
   if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TEIF2) != RESET)	  	
   {
	   DMA_ClearFlag(DMA1_Stream2,DMA_FLAG_TEIF2);
   }
}
void DMA1_Stream0_IRQHandler(void)	// RX5
{
   u16 i = 0;
   if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TCIF0) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0);
	   	for(i=DMAReadIndex5;i<50;i++)
			{
			  ProtocolResolve5(DMARXbuffer5[i]);		   // WIFI数据处理
				ProtocolResolve_Gps(DMARXbuffer5[i]);			 // GPS数据处理
			  ProtocolResolveWIFIError(DMARXbuffer5[i]);
				#ifdef DebugUART5
					InsertDataToQueue(&QueueTX2,BufferTX2,DMARXbuffer5[i]);
				#endif
			}
			DMAReadIndex5 = 0;
   }
   if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_HTIF0) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_HTIF0);
			for(i=DMAReadIndex5;i<25;i++)
			{
			  ProtocolResolve5(DMARXbuffer5[i]);		   // WIFI数据处理
				ProtocolResolve_Gps(DMARXbuffer5[i]);			 // GPS数据处理
			  ProtocolResolveWIFIError(DMARXbuffer5[i]);
				#ifdef DebugUART5
					InsertDataToQueue(&QueueTX2,BufferTX2,DMARXbuffer5[i]);
				#endif
			}
			DMAReadIndex5 = 25;
   }
   if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TEIF0) != RESET)	  	
   {
	   DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TEIF0);
   }
}
void DMA2_Stream1_IRQHandler(void)	// RX6
{
	 u16 i = 0;
   if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1) != RESET)	  	
   {
	   	DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1);
	   	for(i=DMAReadIndex6;i<50;i++)
			{
				ProtocolResolve6(DMARXbuffer6[i]);
			}
			DMAReadIndex6 = 0;
   }
   if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_HTIF1) != RESET)	  	
   {	
	   	DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_HTIF1);
	   	for(i=DMAReadIndex6;i<25;i++)
			{
				ProtocolResolve6(DMARXbuffer6[i]);
			}
			DMAReadIndex6 = 25;
   }
   if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TEIF1) != RESET)	  	
   {
	   DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TEIF1);
   }
}


void DMA2_Stream4_IRQHandler(void)	// RX4
{
	if(DMA_GetFlagStatus(DMA2_Stream4,DMA_FLAG_TCIF4)!=RESET)
	DMA_ClearFlag(DMA2_Stream4,DMA_FLAG_TCIF4);
//if(DMA_GetITStatus(DMA2_Stream4,DMA_IT_TCIF4)!=RESET)
//DMA_ClearITPendingBit(DMA2_Stream4, DMA_IT_TCIF4);

}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
