
#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "stm32f4xx.h"
#include "sysinit.h"
#include <stdio.h>
#include <stdlib.h>
#include <RTL.h>
//#define DebugUART1
//#define DebugUART5
#define Four_Faith_MODEL
//#define SIM800C_MODEL
//#define WD_MODEL

#define ID_Address        0x1FFF7A10 // 103-0x1ffff7e8 407
#define Bank1_SRAM1_ADDR ((u32)(0x60000000))
#define RARED_UN          100
#define CONTROL_ON_DELAY  50  //��բ������ʱ
#define CONTROL_OFF_DELAY 50  //��բ������ʱ

#define DOWN_U_MODULUS    2060
#define DOWN_I_MODULUS    16932
#define DOWN_I0_MODULUS   6654
#define DOWN_U0_MODULUS   6300
#define UP_U_MODULUS      14170
#define UP_I_MODULUS      16620
#define DOWN_I10_MODULUS  1330

/*����汾*/
#define VERSION_YEAR	20 // ����ΪӲ���汾
#define VERSION_MONTH	9
#define VERSION_DATE	7
#define VERSION_TIME	8  // ����޸Ĵ���

#define PROTOCOL_VERSION_YEAR	19		// ͨѶЭ��汾�� 
#define PROTOCOL_VERSION_MONTH	12
#define PROTOCOL_VERSION_DATE	5

/**************************�洢�ռ䶨��***************************/
#define FLASH_KZQUPDATE_ADDR	      0x000000			// ��������������
#define FLASH_KZQUPDATE_LENGTH	    0x080000			// ���������򱾵����� ���ݳ��� 512k
#define FLASH_KZQBACKUP_ADDR	      0x080000			// ���������򱸷ݻָ� //������
#define FLASH_KZQBACKUP_LENGTH	    0x080000			// ���������򱸷ݻָ� ���ݳ��� 512k

#define FLASH_EventRecord_ADDR			0x100000		  // �¼���¼�����洢��ַ //������
#define FLASH_EventRecord_LENGTH		0x000080		  // �¼���¼���� ���ݳ����128�ֽڣ�ʵ�ʴ���100�ֽ�
#define FLASH_EventRecord_MAX			  500					  // �¼���¼���� ���512����ʵ��ʹ��500��
//һ��8������ 2400*8+100 = 19300�ֽڷ�������Ϣ��Ϊ�˲�д���㣬ʹ��5������20480���ֽ�
#define FLASH_P1SCURVE_ADDR         0x110000      //���������¼������    //������
#define FLASH_P1SCURVE_LENGTH       0x005000
#define FLASH_P1SCURVE_MAX          100           //�����100��
//
#define FLASH_SOERecord_ADDR      	0x304000		  // SOE��¼�����洢��ַ  //������
#define FLASH_SOERecord_LENGTH	    12			      // SOE��¼����  ���ݳ���
#define FLASH_SOERecord_MAX         300           // SOE��¼����  �������

#define FLASH_BATVOLTAGE_ADDR      	0x305000		  // ��ص�ѹ�洢��ַ  //������
#define FLASH_BATVOLTAGE_LENGTH	    3			        // ��ص�ѹ�洢����
#define FLASH_BATVOLTAGE_MAX        120           // ��ص�ѹ  �������
#define FLASH_BATTERY_RUNTIME_ADDR  0x305168      // �������ʱ��洢��ַ�������ֽ�

#define FLASH_101PARA_ADDR      	  0x306000		  // 101��Լ�����洢��ַ  //������
#define FLASH_101PARA_LENGTH	      256			      // 101��Լ�����洢����256�ֽ�
#define FLASH_101PARABAK_ADDR      	0x307000		  // 101��Լ�������ݴ洢��ַ  //������
#define FLASH_101PARABAK_LENGTH	    256			      // 101��Լ�������ݴ洢����256�ֽ�

#define FLASH_RESETYY_ADDR      		0x308000  	  // ��λԭ��

#define FLASH_SYSTEMSET_ADDR      	0x700000		  // ϵͳ��������  //������
#define FLASH_SYSTEMSET_LENGTH	    256			      // ϵͳ�������ô洢����256�ֽ�
#define FLASH_PROTECTSET_ADDR      	0x701000		  // ������������  //������
#define FLASH_PROTECTSET_LENGTH	    256			      // �����������ô洢����256�ֽ�
#define FLASH_ALARMSET_ADDR      	  0x702000		  // ������������  //������
#define FLASH_ALARMSET_LENGTH	      256			      // �����������ô洢����256�ֽ�
#define FLASH_AUTOFFSET_ADDR      	0x703000		  // �Զ����в�������  //������
#define FLASH_AUTOFFSET_LENGTH	    256			      // �Զ��������ô洢����256�ֽ�
#define FLASH_MEASURECF_ADDR      	0x704000		  // ����ϵ��     //������         
#define FLASH_MEASURECF_LENGTH	    256			      // ����ϵ���洢����256�ֽ�
#define FLASH_CORRECTCF_ADDR        0x705000		  // У��ϵ��     //������
#define FLASH_CORRECTCF_LENGTH	    256			      // У��ϵ���洢����256�ֽ�

#define FLASH_SYSTEMSETBAK_ADDR     0x600000		  // ϵͳ�������ñ���  //������
#define FLASH_SYSTEMSETBAK_LENGTH	  256			      // ϵͳ�������ñ��ݴ洢����256�ֽ�
#define FLASH_PROTECTSETBAK_ADDR    0x601000		  // �����������ñ���  //������
#define FLASH_PROTECTSETBAK_LENGTH	256			      // �����������ñ��ݴ洢����256�ֽ�
#define FLASH_ALARMSETBAK_ADDR      0x602000		  // �����������ñ���  //������
#define FLASH_ALARMSETBAK_LENGTH	  256			      // �����������ñ��ݴ洢����256�ֽ�
#define FLASH_AUTOFFSETBAK_ADDR     0x603000		  // �Զ��������ñ���  //������
#define FLASH_AUTOFFSETBAK_LENGTH	  256			      // �Զ��������ñ��ݴ洢����256�ֽ�
#define FLASH_MEASURECFBAK_ADDR     0x604000		  // ����ϵ������      //������         
#define FLASH_MEASURECFBAK_LENGTH	  256			      // ����ϵ�����ݴ洢����256�ֽ�
#define FLASH_CORRECTCFBAK_ADDR     0x605000		  // У��ϵ������     //������
#define FLASH_CORRECTCFBAK_LENGTH	  256			      // У��ϵ���洢���ݳ���256�ֽ�
#define FLASH_MANUNUNBER_ADDR       0x606000		  // ������ţ���������  //������
#define FLASH_MANUNUNBER_LENGTH	    256			      // ������Ŵ洢����256�ֽ�
#define FLASH_ENERGYVL_ADDR      	  0x608000		  // ���籣�������ѹ��Ͷ�� //������
#define FLASH_ENERGYVL_LENGTH	      256			      // ���籣������洢����256�ֽ�


#define FLASH_UPDATEFLAG_ADDR	      0x720000		  // ������������־    //������
#define FLASH_UPDATEFLAG_LENGTH	    10			      // ������������־     ���ݳ��� 10Byte

/**************************KZQ���������***************************/
//KZQ_CMD.datatype
#define		KZQTYPE_INFORMATION	          0x00		// ʵʱ��Ϣ
#define		KZQTYPE_RECORD_ON		          0x01		// ��բ��¼
#define		KZQTYPE_RECORD_OFF		        0x02		// ��բ��¼
#define		KZQTYPE_RECORD_PROTECT	      0x03	  // ������¼
#define		KZQTYPE_PARA_PROTECT	        0x04		// ��������
#define		KZQTYPE_PARA_SYSTEM	          0x05		// ϵͳ����
#define		KZQTYPE_PARA_ALARM		        0x06	  // ��������
#define		KZQTYPE_RECORD_ALARM	        0x07		// ������¼
#define   KZQTYPE_MEASURE_VALUE	        0x08		// ����ϵ��
#define		KZQTYPE_RECORD_GROUND	        0x09		// �ӵؼ�¼
#define		KZQTYPE_PARA_AUTOSWITCH		    0x0A	  // �Զ����в���
#define   KZQTYPE_BATTERY_ON            0x0B    // ��ص�ѹ
#define   KZQTYPE_GPS_TIME              0x0C    // GPSʱ��

#define		KZQTYPE_SWITCHON_A	          0x10		// A���բ���� 
#define		KZQTYPE_SWITCHON_B	          0x11		// B���բ���� 
#define		KZQKTYPE_SWITCHON_C	          0x12		// C���բ���� 
#define		KZQTYPE_SWITCHOFF_A	          0x13 		// A���բ���� 
#define		KZQTYPE_SWITCHOFF_B	          0x14 		// B���բ���� 
#define		KZQTYPE_SWITCHOFF_C	          0x15 		// C���բ���� 
#define		KZQTYPE_CURVE_PROTECT_A  	    0x16	  // A�ౣ������
#define		KZQTYPE_CURVE_PROTECT_B	      0x17	  // B�ౣ������
#define		KZQTYPE_CURVE_PROTECT_C	      0x18	  // C�ౣ������
#define		KZQTYPE_CURVE_PROTECT_O	      0x1B	  // ����ӵ�����

/**************************��·���¼���¼����***************************/
// �������Ͷ���
#define   TYPE_ACT_PAD_OFF                0x11    //ң�ط�բ
#define   TYPE_ACT_PAD_ON                 0x12    //ң�غ�բ
#define   TYPE_ACT_FAR_OFF                0x13    //Զ����բ
#define   TYPE_ACT_FAR_ON                 0x14    //Զ����բ
#define   TYPE_ACT_MAN_OFF                0x15    //�ֶ���բ
#define   TYPE_ACT_MAN_ON                 0x16    //�ֶ���բ
#define   TYPE_ACT_REON                   0x17    //�����غ�բ
#define   TYPE_ACT_REVERT                 0x18    //�źŸ���

// �������Ͷ���
#define   TYPE_PROTECT_REON               0x21    // �غ�բ
#define   TYPE_PROTECT_SECOND_REON        0x22    // �����غ�բ
#define   TYPE_PROTECT_FAST_OFF           0x23    // �ٶϱ���
#define   TYPE_PROTECT_DELAY_FAST_OFF     0x24    // ��ʱ�ٶϱ���
#define   TYPE_PROTECT_MAX_CURRENT        0x25    // ��������
#define   TYPE_PROTECT_MAX_CURRENT_AFTER  0x26    // ��������ٱ���
#define   TYPE_PROTECT_GND                0x27    // �ӵر���
#define   TYPE_PROTECT_PHASE_LOSS         0x28    // ȱ�ౣ��
#define   TYPE_PROTECT_ZERO_MAX_VOLTAGE		0x29	  // �����ѹ����
#define   TYPE_PROTECT_ZERO_MAX_CURRENT		0x2A	  // �����������
#define   TYPE_PROTECT_ZERO_FAST_OFF		  0x2B	  // �����ٶϱ���
#define   TYPE_PROTECT_MAX_VOLTAGE        0x2C    // ��ѹ����
#define   TYPE_PROTECT_MAX_FREQ           0x2D    // ��Ƶ����
#define   TYPE_PROTECT_LOW_FREQ           0x2E    // ��Ƶ����

//�����Զ�����������
#define   TYPE_PROTECT_LOSS_POWER         0x31    // ʧ���բ
#define   TYPE_PROTECT_GET_POWER          0x32    // �õ��բ
#define   TYPE_PROTECT_SINGLE_LOSS        0x33    // ʧ���բ
#define   TYPE_PROTECT_ERR_FASTOFF        0x34    // �ϵ����Ͽ��ٷ�բ
#define   TYPE_PROTECT_SWITCHOFF_LOCK     0x35    // ��բ����
#define   TYPE_PROTECT_SWITCHON_LOCK      0x36    // ��բ����
#define   TYPE_PROTECT_ZERO_VOLTAGE1      0x37    // �����ѹ����
#define   TYPE_PROTECT_ZERO_VOLTAGE2      0x38    // �����ѹ����
#define   TYPE_PROTECT_PTBREAK_ALARM      0x39    // PT���߸澯

// �������Ͷ���
#define   TYPE_ALARM_MAX_VOLTAGE          0x41    //��ѹ����
#define   TYPE_ALARM_MIN_VOLTAGE          0x42    //Ƿѹ����
#define   TYPE_ALARM_MAX_HAR              0x43    //��г������
#define   TYPE_ALARM_VOL_QUA_LOW          0x44    //��ѹ�ϸ��ʵͱ���
#define   TYPE_ALARM_MAX_LOAD             0x45    //�����ɱ���
#define   TYPE_ALARM_ZERO_MAX_VOLTAGE     0x46    //�����ѹ����
#define   TYPE_ALARM_ZERO_MAX_CURRENT     0x47    //�����������
#define   TYPE_ALARM_BAT_LOW              0x48    //��ص�ѹ�ͱ���
#define   TYPE_ALARM_GND                  0x49    //�ӵر���

//KZQ_CMD.flag
#define		KZQFLAG_CMD			        0x0001		// ��������
#define		KZQFLAG_SYSTEM_SET		  0x0002		// ϵͳ��������
#define		KZQFLAG_PROTECT_SET	    0x0004		// ������������
#define		KZQFLAG_ALARM_SET		    0x0008		// ������������
#define		KZQFLAG_MACHINE_SET	    0x0010		// ������Ϣ����
#define		KZQFLAG_INFORMATION	    0x0020		// ʵʱ��Ϣ
#define		KZQFLAG_GPS_time        0x0040		// GPSʱ��
#define		KZQFLAG_HOSTPC_time     0x0080		// ��̨Уʱ
#define		KZQFLAG_AUTOSWITCH_SET	0x0100		// �Զ����в�������

//KZQ_CMD.action
#define		KZQCMD_BREAKER_ON		    0x01		// ��բ           
#define		KZQCMD_BREAKER_OFF		  0x02		// ��բ
#define   KZQCMD_BATTERY_ON       0x03    //��ؼ̵�����
#define   KZQCMD_BATTERY_OFF      0x04    //��ؼ̵�����
#define		KZQCMD_REVERT			      0x05		// ����
#define		KZQCMD_CLEAR			      0x06		// ��յ�����Ϣ
#define		KZQCMD_RESET			      0x07		// ��λ
#define		KZQCMD_FORCE_ON		      0x08		// ǿ�ƺ�բ
#define		KZQCMD_FORCE_OFF	      0x09		// ǿ�Ʒ�բ

#define		KZQMODE_MODE1			      0x01		// ģʽ1 ���� mode
#define		KZQMODE_MODE2			      0x02		// ģʽ2 Զ��

/**************************SOEң�ŵ�ַ***************************/
#define   YX_SHAKE_TIME			                   8	// ң��ȥ��8ms

#define   SOE_ADDR_FAR                         Measure101Para.para.far_positoin            // Զ��ң�ŵ�ַ
#define   SOE_ADDR_GND                         Measure101Para.para.local_positoin          // �͵�ң�ŵ�ַ
#define		SOE_ADDR_POWER_LOST                  Measure101Para.para.AC_loss                 // ����ʧ��澯
#define		SOE_ADDR_ALARM_BAT_LOW_VOLTAGE       Measure101Para.para.Bat_low                 // ��ص͵�ѹ���������¼�
#define   SOE_ADDR_SWITCHON_LOCK               Measure101Para.para.lock_on                 // ���ر�����բ
#define   SOE_ADDR_SWITCHOFF_LOCK              Measure101Para.para.lock_off                // ���ر�����բ
#define   SOE_ADDR_IN2                         Measure101Para.para.break_on                // ��λң�ŵ�ַ
#define   SOE_ADDR_IN1                         Measure101Para.para.break_off               // ��λң�ŵ�ַ
#define   SOE_ADDR_IN3		                     Measure101Para.para.wcn_state	             // ����δ����
#define   SOE_ADDR_ALARM_MAX_LOAD		           Measure101Para.para.max_load_alarm          // �����ɱ��������¼�
#define   SOE_ADDR_ALARM_GROUNDI0   		       Measure101Para.para.zero_current1_alarm	   // ����������������¼�
#define   SOE_ADDR_ALARM_GROUND		             Measure101Para.para.ground_current_alarm	   // �ӵر��������¼�
#define   SOE_ADDR_FAST_OFF				             Measure101Para.para.max_current1_protec	   // �ٶϱ��������¼�
#define   SOE_ADDR_DELAY_FAST_OFF		  	       Measure101Para.para.max_current2_protec	   // ��ʱ�ٶϱ��������¼�
#define   SOE_ADDR_ZERO_MAX_CURRENT		         Measure101Para.para.zero_current1_protec	   // ����������������¼�
#define   SOE_ADDR_REON					               Measure101Para.para.reon               	   // �غ�բ�����¼�
#define   SOE_ADDR_MAX_CURRENT_AFTER		       Measure101Para.para.max_currentafter_protec // ��������ٱ��������¼�
#define   SOE_ADDR_MAX_VOLTAGE                 Measure101Para.para.max_voltage             // ��ѹ����
#define   SOE_ADDR_MAX_FREQ                    Measure101Para.para.high_freq               // ��Ƶ����
#define   SOE_ADDR_LOW_FREQ                    Measure101Para.para.low_freq                // ��Ƶ����
#define   SOE_ADDR_EVENT_ALL                   Measure101Para.para.event_all               // �¹���
#define   SOE_ADDR_MAX_CURRENT		             Measure101Para.para.max_current3_protec	   // �������������¼�
#define		SOE_ADDR_SELF_ERROR                  Measure101Para.para.break_err               // װ���쳣�澯
#define		SOE_ADDR_SWITCH_CONTACT_1            Measure101Para.para.segment_mode            // ���طֶε㹤��ģʽ
#define		SOE_ADDR_SWITCH_CONTACT_2            Measure101Para.para.contact_mode            // ��������㹤��ģʽ
#define		SOE_ADDR_SWITCH_POWERLOSS_1          Measure101Para.para.novoltage_A             // ���ص�Դ����ѹ
#define		SOE_ADDR_SWITCH_POWERLOSS_2          Measure101Para.para.novoltage_B             // ���ظ��ɲ���ѹ
#define   SOE_ADDR_POWERLOSS_OFF               Measure101Para.para.powerloss_off           // ����ʧѹ��բ
#define		SOE_ADDR_ALARM_BAT_ACTIVE            Measure101Para.para.bat_active              // ��ػ

#define   SOE_ADDR_IN1_pad		                     5	// ��λң�ŵ�ַ
#define   SOE_ADDR_IN2_pad		                     6	// ��λң�ŵ�ַ
#define   SOE_ADDR_IN3_pad		                     7	// δ����ң�ŵ�ַ
#define   SOE_ADDR_IN4_pad		                     8	// ����ң�ŵ�ַ
#define   SOE_ADDR_GND_pad		                     9	// �͵�ң�ŵ�ַ
#define		SOE_ADDR_BHYB1_pad                       10 // �ܱ���ѹ���ַ
#define		SOE_ADDR_BHYB2_pad                       11 // �ӵر���ѹ���ַ
#define		SOE_ADDR_BHYB3_pad                       12 // ��䱣��ѹ���ַ
#define		SOE_ADDR_FAR_pad                         13 // Զ��ң�ŵ�ַ
#define		SOE_ADDR_SIGRESET_pad                    14 // �źŸ����ַ

#define   SOE_ADDR_ZERO_MAX_VOLTAGE_pad		         15	// �����ѹ���������¼�
#define   SOE_ADDR_ZERO_MAX_CURRENT_pad		         16	// ����������������¼�
#define   SOE_ADDR_PHASE_LOSS_pad				           17	// ȱ�ౣ�������¼�
#define   SOE_ADDR_GROUND_pad				               18	// �ӵر��������¼�
#define   SOE_ADDR_FAST_OFF_pad				             19	// �ٶϱ��������¼�
#define   SOE_ADDR_DELAY_FAST_OFF_pad		  	       20	// ��ʱ�ٶϱ��������¼�
#define   SOE_ADDR_MAX_CURRENT_pad			           21	// �������������¼�
#define   SOE_ADDR_MAX_CURRENT_AFTER_pad		       22	// ��������ٱ��������¼�
#define   SOE_ADDR_REON_pad					               23	// �غ�բ�����¼�

#define   SOE_ADDR_ALARM_MAX_LOAD_pad		           24	// �����ɱ��������¼�
#define   SOE_ADDR_ALARM_MAX_VOLTAGE_pad		       25	// ����ѹ���������¼�
#define   SOE_ADDR_ALARM_MIN_VOLTAGE_pad		       26	// Ƿѹ���������¼�
#define   SOE_ADDR_ALARM_MAX_HARMONIC_pad		       27	// ��г�����������¼�
#define   SOE_ADDR_ALARM_VOLTAGE_QUALIFRATE_pad    28	// ��ѹ�ϸ��ʱ��������¼�
#define   SOE_ADDR_ALARM_BAT_LOW_VOLTAGE_pad	     29	// ��ص͵�ѹ���������¼�
#define   SOE_ADDR_ALARM_GROUND_pad   		         30	// �ӵر��������¼�
#define   SOE_ADDR_ALARM_GROUNDU0_pad   		       31	// �����ѹ���������¼�
#define   SOE_ADDR_ALARM_GROUNDI0_pad   		       32	// ����������������¼�
#define   SOE_ADDR_MAX_VOLTAGE_pad                 33 // ��ѹ����
#define   SOE_ADDR_MAX_FREQ_pad                    34 // ��Ƶ����
#define   SOE_ADDR_LOW_FREQ_pad                    35 // ��Ƶ����

#define   SOE_ADDR_ACT_PAD_OFF_pad                 51 //���ط�բ
#define   SOE_ADDR_ACT_PAD_ON_pad                  52 //���غ�բ
#define   SOE_ADDR_ACT_FAR_OFF_pad                 53 //Զ����բ
#define   SOE_ADDR_ACT_FAR_ON_pad                  54 //Զ����բ
#define   SOE_ADDR_ACT_MAN_OFF_pad                 55 //�ֶ���բ
#define   SOE_ADDR_ACT_MAN_ON_pad                  56 //�ֶ���բ

#define   SOE_ADDR_PT_BREAK_pad                    60 // PT����
#define   SOE_ADDR_LOSS_POWER_pad                  61 // ʧ����ʱ��բ
#define   SOE_ADDR_GETPOWER_A_pad                  62 // A��õ���ʱ��բ
#define   SOE_ADDR_GETPOWER_B_pad                  63 // B��õ���ʱ��բ
#define   SOE_ADDR_LOSSPOWER_A_pad                 64 // A��ʧ����ʱ��բ
#define   SOE_ADDR_LOSSPOWER_B_pad                 65 // B��ʧ����ʱ��բ
#define   SOE_ADDR_DOUBLEPOWER_LOCKON_pad          66 // ˫����ѹ������բ
#define   SOE_ADDR_VOLTAGE_TIME_pad                67 // ��ѹʱ����ʧ���բ
#define   SOE_ADDR_VOLTAGE_CURRENT1_pad            68 // ��ѹ������ʧ���բ�ޱ���
#define   SOE_ADDR_VOLTAGE_CURRENT2_pad            69 // ��ѹ������ʧ���բ�б���
#define   SOE_ADDR_SWITCHON_LOCKOFF_pad            70 // ��բ�ɹ�������բ
#define   SOE_ADDR_SIGRESET_LOCK_pad               71 // ��������
#define   SOE_ADDR_NOBREAK_CURRENT_pad             72 // ���ڶϵ�������
#define   SOE_ADDR_RESVOLTAGE_LOCK_pad             73 // ��ѹ����
#define   SOE_ADDR_INRUSH_pad                      74 // ӿ��
#define   SOE_ADDR_PHASE_ERR_pad                   75 // ������
#define   SOE_ADDR_GROUND_ERR_pad                  76 // �ӵع���
#define   SOE_ADDR_ZERO_VOLTAGE1_pad               77 // �����ѹ����
#define   SOE_ADDR_ZERO_VOLTAGE2_pad               78 // �����ѹ����

//����Flashλ�ö��� �¼�����
#define Flash_LineLoss_Set                0      //0
#define Flash_LineLoss_Lock               0x1000 //1  //����ģ�鶳�������λ����Ϣ
#define Flash_LineLoss_Event							0x2000 //2  //����ģ���¼�������λ����Ϣ
#define Flash_LineLoss_Fix                0x3000 //3- 54*60
#define Fix_Num                           54//�漸����
#define Flash_LineLoss_Rand								0x4000 //3- 54*3
#define Rand_Num                          54//�漸����
#define Flash_LineLoss_Frzd               0x5000 //62- 54*62
#define Frzd_Num                          54//�漸����
#define Flash_LineLoss_Sharp              0x6000//264 -14*264
#define Sharp_Num                         14//�漸����
#define Flash_LineLoss_Month              0x7000//12-38*12
#define Month_Num                         38//�漸����
#define Flash_LineLoss_Event_PAReverse    0x8000// 4+8*4+8+8*4  10�� �ָ�ʱ��¼����2�� 72
#define PAReverse_EndNum                  84//�漸����+8
#define PAReverse_ActNum                  44//�漸����
#define Flash_LineLoss_Event_PBReverse    0x9000// 8*4+8+8*4  10�� �ָ�ʱ��¼����2�� 72
#define PBReverse_EndNum                  84//�漸����
#define PBReverse_ActNum                  44//�漸����
#define Flash_LineLoss_Event_PCReverse    0xA000// 8*4+8+8*4  10�� �ָ�ʱ��¼����2�� 72
#define PCReverse_EndNum                  84//�漸����
#define PCReverse_ActNum                  44//�漸����
#define Flash_LineLoss_Event_PReverse     0xB000// 8*4  10�� �ָ�ʱ��¼����2�� 72
#define PReverse_Num                      44//�漸����
#define Flash_LineLoss_Event_ClearLock    0xC000// 8*4  10�� �ָ�ʱ��¼����2�� 72
#define ClearLock_Num                     44//�漸����
#define Flash_LineLoss_Event_ClearEvent   0xD000//����¼���� ֻ��¼�ܴ��� ��ռ�ÿռ� 4*10
#define ClearEvent_Num                    12
#define Flash_LineLoss_Event_CheckTime    0xE000//����¼���� ֻ��¼�ܴ��� ��ռ�ÿռ�
#define CheckTime_Num                     12
#define Flash_LineLoss_Event_DocTime      0xF000//����¼���� ֻ��¼�ܴ��� ��ռ�ÿռ�  //�ļ����һ���޸�ʱ��
#define Doc_Num                           43 //6���ļ�+У���

// /**************************READ_101Flag***************************/
#define  system_data        0x01  
#define  protect_data       0x02  
#define  alarm_data         0x04 
#define  autoswitch_data    0x08  
//����ģ�� ����
struct Lineloss
{
	float LockDay;
	float LockHour;
	float P_Reverse_Limit;
	float P_Reverse_Time;
	float Pt_Reverse_Limit;
	float Pt_Reverse_Time;
};

union Lineloss1
{
	struct Lineloss para;
	u8 two_array[6][sizeof(float)];
};

struct REVER_BITS {     			// bits  
  u8  PA_Reverse:1;  		// 0  	��λң��
	u8  PB_Reverse:1;  		// 1  	��λң��
	u8  PC_Reverse:1;  		// 2  	��λң��
	u8  P_Reverse:1;  		// 3  	��λң��
  u8  D4:1;  		// 4  	��λң��
	u8  D5:1;  		// 5  	��λң��
	u8  D6:1;  		// 6  	��λң��
	u8  D7:1;  		// 7  	��λң��
};

union REVERDefine					// ң�Ŷ���
{
	struct REVER_BITS bit;
	u8 byte;
};
struct LineLossEvent
{
	u32 PA_Reverse_Time;
	u32 PB_Reverse_Time;
	u32 PC_Reverse_Time;
	u32 P_Reverse_Time;
	u32 LineLoss_Zero_Time;
	u32 Event_Zero_Time;
	u32 Check_Time;//��ʱ
	u8  PA_Place;
	u8  PB_Place;
	u8  PC_Place;
	u8  P_Place;
	u8  LineLoss_Place;
	u8  Event_Place;
	u8  Check_Place;
	u32 PA_Reverse_Delay;
	u32 PB_Reverse_Delay;
	u32 PC_Reverse_Delay;
	u32 P_Reverse_Delay1;
	u32 P_Reverse_Delay2;
};	
struct LineLockEvent
{
	u16 Fix_Time;
	u16 Rand_Time;
	u16 Frzd_Time;
	u16 Sharp_Time;
	u16 Month_Time;//�洢����
//	u32 Event_Time;//   �¼���7�ֵ����洢
	u16  Fix_Place;//60   ʹ�ö���ʱ�ж���
	u16  Rand_Place;//3    �����
	u16  Frzd_Place;//62   0�㶳��
	u16  Sharp_Place;//264 ���㶳��
	u16  Month_Place;//12  �¶��� ������+����ʱ
	u16  Read_Place;//��ʱ����
	u16  Read_CS;   //��ȡ����
	u32  Read_Text; //��ȡλ��
	u32  Write_Text; //д��λ��
	u32  Next_Text;//�´�д���׵�ַ
	u32  Last_Text;//�ϴε�ַ
	u8   Write_EndBz;//������־
	u8   Read_EndBz;//������־
	u8   Read_Num;//������ȡ���� //�¼������Ϊ �ļ����ĳ���
	u8 	 Event_Time;//�ܴ���
	u8   Event_Bz;//1-7
	u8   Event_CS;//�ڼ��ζ�ȡ
	u8   Doc_Bz;//�ļ���־
};	
extern struct LineLockEvent LineLockNum;
extern struct LineLossEvent LineLossNum;
extern union Lineloss1 LineSet;
extern union REVERDefine PReverse;

//----------------------------------------���߽ṹ---------------------------------------//
struct Curve
{
	s16 voltage[400];		// ��ѹ����
	s16 current[400];		// ��������
	s16 locus[400];		// �켣����
	s16 position[50];
};

union CurveDat
{
	s16 byte[1250];
	struct Curve word; 
};

struct ActionMode
{
	union CurveDat On;				// ��բ
	union CurveDat Off;				// ��բ
	union CurveDat Protect;			// ������բ
};

struct PhaseMode
{
	struct ActionMode CurveA;
	struct ActionMode CurveB;
	struct ActionMode CurveC;
};
extern struct PhaseMode MyCurve;




//struct Curve1
//{
//	s16 voltage[800];		// ��ѹ����
//	s16 current[800];		// ��������
//	s16 locus[800];		// �켣����
//};

//union CurveDat1
//{
//	s16 byte[2400];
//	struct Curve1 word; 
//};

//struct ActionMode1
//{
//	union CurveDat1 On;				// ��բ
//	union CurveDat1 Off;				// ��բ
//	union CurveDat1 Protect;			// ������բ
//};

//struct PhaseMode1
//{
//	struct ActionMode1 CurveA;
//	struct ActionMode1 CurveB;
//	struct ActionMode1 CurveC;
//};
//extern struct PhaseMode1 MyCurve1;


struct Modulus
{
	u16 UAB;
	u16 UBC;
	u16 UCA;
	u16 UA;
	u16 UB;
	u16 UC;
	u16 U0;
	u16 IA;
	u16 IB;
	u16 IC;
	u16 IA10;
	u16 IB10;
	u16 IC10;
	u16 I0;
	u16 UA1;
	u16 I010;
	u16 UC1;
};

union Modulus1
{
	struct Modulus para;
	u16 word[sizeof(struct Modulus)/2];
};

struct Rule101Para
{
	u8 balance;              
	u8 addr_len;
  u8 reason_len;
	u8 data_type;
	u8 encrypt;
	u8 Remote_Ctrl;       //����ң��
	u8 Remote_Batact;     //��ػ
	u8 Remote_Reset;      //װ��Զ����λ
	u8 res5;
	u8 res6;                         //10
	u8 AC_power;
	u8 Bat_volt;
	u8 UAB;
	u8 UCB;
	u8 U0;
	u8 Sig_dBm;
	u8 freq;
	u8 res7;
	u8 res8;
	u8 IA;                           //20
	u8 IB;
	u8 IC;
	u8 I0;
	u8 P;
	u8 Q;
	u8 cos;
	u8 res9;
	u8 res10;
	u8 res11;
	u8 far_positoin;                  //30
	u8 local_positoin;
	u8 AC_loss;
	u8 Bat_low;
	u8 low_airlock;
	u8 lock_on;
	u8 lock_off;
	u8 break_on;
	u8 break_off;
	u8 wcn_state;
	u8 max_current1_alarm;            //40
	u8 max_current2_alarm;
	u8 max_load_alarm;
	u8 zero_current1_alarm;
	u8 ground_current_alarm;
	u8 max_current1_protec;
	u8 max_current2_protec;
	u8 zero_current1_protec;
	u8 zero_currentafter_protec;
	u8 reon;
	u8 max_currentafter_protec;        //50
	u8 max_voltage;
	u8 high_freq;
	u8 low_freq;
	u8 event_all;
	u8 max_current3_protec;
	u8 break_err;
	u8 segment_mode;
	u8 contact_mode;
	u8 novoltage_A;
	u8 novoltage_B;                    //60
	u8 powerloss_off;                          
	u8 bat_active;                //��ػ           
};

union Rule101Para1
{
	struct Rule101Para para;
	u8 word[sizeof(struct Rule101Para)];
};

struct Measure		// ����ֵ
{
	u8 Pstartflag;	 // �������߿�ʼ������־
	u8 Psaveflag;    // �������߿�ʼ�����־
	u8 Psaveflash;    // �������߱��浽falsh
	
	s16 UAB_val;		 // AB���ѹ��Чֵ
	s16 UBC_val;		 // BC���ѹ��Чֵ
	s16 UCA_val;		 // CA���ѹ��Чֵ

	s16 UA_val;			 // A���ѹ��Чֵ
	s16 UB_val;			 // B���ѹ��Чֵ
	s16 UC_val;			 // C���ѹ��Чֵ
	
	s16 UAB1_val;		 // AB���ѹ��Чֵ
	s16 UBC1_val;		 // BC���ѹ��Чֵ
	s16 UCA1_val;		 // CA���ѹ��Чֵ

	s16 UA1_val;		 // A���ѹ��Чֵ
	s16 UB1_val;		 // B���ѹ��Чֵ
	s16 UC1_val;		 // C���ѹ��Чֵ

	s16 IA_val;	     // A�������Чֵ
	s16 IB_val;		   // B�������Чֵ
	s16 IC_val;	     // C�������Чֵ

	u16 Display_UAB_val;		 // AB����ʾ��ѹ��Чֵ
	u16 Display_UBC_val;		 // BC����ʾ��ѹ��Чֵ
	u16 Display_UCA_val;		 // CA����ʾ��ѹ��Чֵ
	
	u16 Display_degUAB_val; // AB����ʾ��ѹ�Ƕ�	

	u16 Display_UA_val;		 // A����ʾ��ѹ��Чֵ
	u16 Display_UB_val;		 // B����ʾ��ѹ��Чֵ
	u16 Display_UC_val;		 // C����ʾ��ѹ��Чֵ

	u16 Display_degUA_val;	 // A����ʾ��ѹ�Ƕ�
	u16 Display_degUB_val;	 // B����ʾ��ѹ�Ƕ�
	u16 Display_degUC_val;	 // C����ʾ��ѹ�Ƕ�

//	u16 Display_UAB1_val;	// AB����ʾ��ѹ��Чֵ
//	u16 Display_UBC1_val;	// BC����ʾ��ѹ��Чֵ
//	u16 Display_UCA1_val;	// CA����ʾ��ѹ��Чֵ	

	u16 Display_UA1_val;		 // A����ʾ��ѹ��Чֵ
	u16 Display_UB1_val;		 // B����ʾ��ѹ��Чֵ
	u16 Display_UC1_val;		 // C����ʾ��ѹ��Чֵ

	u16 Display_U0_val;		 // �����ѹ��ʾ��Чֵ
  u16 Display_degU0_val;  // �����ѹ�Ƕ���ʾ

	u16 Display_IA_val;		 // A����ʾ������Чֵ
	u16 Display_IB_val;		 // B����ʾ������Чֵ
	u16 Display_IC_val;		 // C����ʾ������Чֵ

  u16 Display_degIA_val;  // A����ʾ�����Ƕ�
	u16 Display_degIB_val;	 // B����ʾ�����Ƕ�
 	u16 Display_degIC_val;	 // C����ʾ�����Ƕ�

	u16 Display_I0_val;		 // ���������ʾ��Чֵ
	u16 Display_degI0_val;	 // ���������ʾ�Ƕ�

	u16 Display_IA10_val;	 // A��10����ʾ������Чֵ
	u16 Display_IB10_val;	 // B��10����ʾ������Чֵ
	u16 Display_IC10_val;	 // C��10����ʾ������Чֵ

	u16 Protect_UAB_val;		 // AB�ౣ����ѹ��Чֵ
	u16 Protect_UBC_val;		 // BC�ౣ����ѹ��Чֵ
	u16 Protect_UCA_val;		 // CA�ౣ����ѹ��Чֵ	

	u16 Protect_UA_val;		 // A�ౣ����ѹ��Чֵ
	u16 Protect_UB_val;		 // B�ౣ����ѹ��Чֵ
	u16 Protect_UC_val;		 // C�ౣ����ѹ��Чֵ

//	u16 Protect_UAB1_val;	// AB�ౣ����ѹ��Чֵ
//	u16 Protect_UBC1_val;	// BC�ౣ����ѹ��Чֵ
//	u16 Protect_UCA1_val;	// CA�ౣ����ѹ��Чֵ	

	u16 Protect_UA1_val;		  // A�ౣ����ѹ��Чֵ
	u16 Protect_UB1_val;		  // B�ౣ����ѹ��Чֵ
	u16 Protect_UC1_val;		  // C�ౣ����ѹ��Чֵ

	u16 Protect_U0_val;		  // �����ѹ������Чֵ

	u16 Protect_IA_val;		  // A�ౣ��������Чֵ
	u16 Protect_IB_val;		  // B�ౣ��������Чֵ
	u16 Protect_IC_val;		  // C�ౣ��������Чֵ

	u16 Protect_I0_val;		  // �������������Чֵ
	u16 Protect_10I0_val;		// �������10��������Чֵ

	u16 Protect_IA10_val;	  // A��10������������Чֵ
	u16 Protect_IB10_val;	  // B��10������������Чֵ
	u16 Protect_IC10_val;	  // C��10������������Чֵ

	u16 Protect_IASD_val;		// A�ౣ��������Чֵ
	u16 Protect_IBSD_val;		// B�ౣ��������Чֵ
	u16 Protect_ICSD_val;		// C�ౣ��������Чֵ

	u16 Protect_IA10SD_val;	// A��10������������Чֵ
	u16 Protect_IB10SD_val;	// B��10������������Чֵ
	u16 Protect_IC10SD_val;	// C��10������������Чֵ

	float PA_val;		// A���й����� 
	float PB_val;		// B���й�����
	float PC_val;		// C���й�����

	s16 QA_val;		// A���޹�����
	s16 QB_val;		// B���޹�����
	s16 QC_val;		// C���޹�����

	s16 cosA;		  // A�๦������
	s16 cosB;		  // B�๦������
	s16 cosC;		  // C�๦������

	float P_val;		// �����й�����
	s16 Q_val;		// �����޹�����
	s16 cosALL;		// �ܹ�������
	
	u16 freq;	// ��ȱ�Ƶ��
  u16 freq_cpu1;	// cpu����Ƶ��1
	u16 freq_cpu2;	// cpu����Ƶ��2
	
	s16 Wp;	          // �����й�����
	s16 Wq;	          // �����޹�����
	s16 cos_W;		    // ���յ�����������

	float Wp_all;	      // �������й�����
	float WpFX_all;	    // �ܷ����й�����
	float Wp_peak;	    // �������й�����
	float WpFX_peak;	  // �巴���й�����
	float Wp_valley;	  // �������й�����
	float WpFX_valley;	// �ȷ����й�����
	float Wp_level;	    // ƽ�����й�����
	float WpFX_level;	  // ƽ�����й�����

	float Wq_1all;	    // I  �������޹�����
	float Wq_2all;	    // II �������޹�����
	float Wq_3all;	    // III�������޹�����
	float Wq_4all;	    // IV �������޹�����

	u16 XB_U[9];	    // ��ѹг��
	u16 XB_I[9];	    // ����г��
	
	u16 DIS_XB_U[9];	// ��ѹг��
	u16 DIS_XB_I[9];	// ����г��

	u16 datatype;		// ��������	
	u8 YaBan1;		  // ѹ��1
	u8 YaBan2;		  // ѹ��2
	u8 YaBan3;		  // ѹ��3
	u32 UABsum;
	u32 UBCsum;
	u32 UCAsum;
	u32 UAsum;
	u32 UBsum;
	u32 UCsum;
	u32 UA1sum;
	u32 UB1sum;
	u32 UC1sum;
	u32 U0sum;
	u32 IAsum;
	u32 IBsum;
	u32 ICsum;
	u32 I0sum;
	u32 PIAsum;
	u32 PIBsum;
	u32 PICsum;
	u32 PI0sum;
	u32 IASDsum;
	u32 IBSDsum;
	u32 ICSDsum;
	u32 PIASDsum;
	u32 PIBSDsum;
	u32 PICSDsum;

	u16 Att7022Flag;

	u32 UAB_temp;
	u32 UBC_temp;
	u32 UCA_temp;
	u32 UA_temp;
	u32 UB_temp;
	u32 UC_temp;
	u32 U0_temp;
	u32 UA1_temp;
	u32 UB1_temp;
	u32 UC1_temp;
	u32 IA_temp;
	u32 IB_temp;
	u32 IC_temp;
	u32 PIA_temp;
	u32 PIB_temp;
	u32 PIC_temp;
	u32 I0_10_temp;
	u32 I0_temp;
	u16 V_BAT;
	
	s16 SA_val;		// A���й����� 
	s16 SB_val;		// B���й�����
	s16 SC_val;		// C���й�����
	s16 S_val;
};

struct Measure1		// ��������ֵ
{
	s16 IAX[100];
  s16 IBX[100];
  s16 ICX[100];
	s16 U0X[100];
	s16 I0X[100];
	s16 UAX[100];
	s16 UBX[100];
	s16 UCX[100];
	s16 UABX[100];
	s16 UBCX[100];	
	s16 UCAX[100];
  s16 UPT1X[100];	
	s16 UPT2X[100];

	s16 XB_IA[64];	// A�����г��
	s16 XB_IB[64];
	s16 XB_IC[64];
	s16 XB_UA[64];
	s16 XB_UB[64];
	s16 XB_UC[64];
	s16 XB_U0[64];
	s16 XB_I0[64];
	s16 XB_UAB[64];	// A���ѹг��
	s16 XB_UBC[64];
	s16 XB_UCA[64];
  s16 XB_UPT1[64];	
	s16 XB_UPT2[64];
	u32 V_BAT_SUM;

};

/**********************
*��·�������Ϣ 4-1
����  =  34
**********************/
struct DLQZD
{
	u16 open_distance[3];
	u16 overrun[3];
	u16 time_on[3];
	u16 speed_on[3];
	u16 meanwhile_on[3];
	u16 shake_on[3];
	u16 current_on;
	u16 time_off[3];
	u16 speed_off[3];
	u16 meanwhile_off[3];
	u16 burn_off[3];
	u16 current_off;
	u16 store_time;
	u16 store_current;
};
union DLQZDXX
{
	struct DLQZD para;
	u16 word[sizeof(struct DLQZD)/2];
};

struct time_now			// ϵͳʱ��
{
	u16 year;
	u16 month;
	u16 date;
	u16 hour;
	u16 minute;
	u16 second;
	u16 msec;
};

// ���������ݰ汾
struct UpdateVersion
{
	u8  KZQ_version[4];			  // KZQ����汾
	u8  IAP_version[4];			  // KZQ��������汾
	u8  GPRS_version[4];			// GPRS����汾
	u32 KZQ_length_update;		// KZQ�����������ݰ�����
	u8  KZQ_time_update[6];	  // KZQ�����������ݰ��޸�ʱ��
	u32 KZQ_length_backup;		// KZQ���ݳ������ݰ�����
	u8  KZQ_time_backup[6];	  // KZQ���ݳ������ݰ��޸�ʱ��
	u16 Product_number;	      // ��������
	u16 Product_flow;	        // ������ˮ
	u8  Product_date[3];	    // ��������
	u16 GPRS_length_update;	  // GPRS�������ݰ�����
};

/***********************����ֵ*********************************/
// ϵͳ��������
struct SYSTEM_PARA      				          // ϵͳ����
{
	u16  PT_Hvalue;					      // 0   PT���һ�β�
	u16  PT_Lvalue;					      // 1   PT��ȶ��β�
	u16  CT_Hvalue;			          // 2   CT���һ�β�
	u16  CT_Lvalue;			          // 3   CT��ȶ��β�
	u16  exchange_dycfzc;         // 4
	u16  wifi_gps_switch;         // 5
	u16  peak_valley_period[6];	  // 6   ʱ������
	u16  password;				        // 12  ����
	u16  groundtype;	            // 13  ���ĵ�ӵط�ʽ
	u16  com_protocol;				    // 14  ��Լ�ţ�88Ϊtest    
	u16  baud_rate;				        // 15  ������
	u16  battery_delay;		        // 16  ��ع���ʱ��
	u16  telnumber1[3];           // 17  �ֻ�����1
  u16  telnumber2[3];           // 20  �ֻ�����2
	u16  address;					        // 23  ������ַ
	u16  port1;						        // 24  Ŀ�Ķ˿ں�
	u16  port2;						        // 25  ���ö˿ں�
	u16  IP1[2];						      // 26	 Ŀ��IP��ַ
	u16  IP2[2];						      // 28  ����IP��ַ
	u16  control_time;            // 30  ң�ر���ʱ��
	u16  start_year;					    // 31  ����ʱ������
	u16  start_month;             // 32
	u16  start_date;              // 33
	u16  start_hour;					    // 34
	u16  start_min;               // 35
	u16  start_sec;               // 36
};
union SYSTEM_PARA1
{
	struct SYSTEM_PARA para;
	u16    word[sizeof(struct SYSTEM_PARA)/2];
};

struct PROTECT_PARA       				      // ������������
{
  u16  max_voltage;  					          // 0   	��ѹ
	u16  max_voltage_time;  				      // 1   	��ѹʱ��
	u16  max_voltage_enable;  			      // 2   	��ѹʹ��
  u16  zero_max_voltage;  				      // 3   	�����ѹ
	u16  zero_max_voltage_time;  		      // 4   	�����ѹʱ��
	u16  zero_max_voltage_enable;  		    // 5   	�����ѹʹ��
	u16  min_voltage;  					          // 6   	Ƿѹ
	u16  min_voltage_time;  				      // 7   	Ƿѹʱ��
	u16  min_voltage_enable;  			      // 8   	Ƿѹʹ��
	u16  fast_off;  						          // 9   	�ٶ�
	u16  fast_off_time;  				          // 10   �ٶ�ʱ��
	u16  fast_off_voltage_lock;  		      // 11   �ٶϵ�ѹ����
	u16  fast_off_direction_lock;  		    // 12   �ٶϷ������
	u16  fast_off_enable;  				        // 13   �ٶ�ʹ��
	u16  delay_fast_off;  				        // 14   ��ʱ�ٶ�
	u16  delay_fast_off_time;  			      // 15   ��ʱ�ٶ�ʱ��
	u16  delay_fast_off_voltage_lock;  	  // 16   ��ʱ�ٶϵ�ѹ����
	u16  delay_fast_off_direction_lock;   // 17   ��ʱ�ٶϷ������
	u16  delay_fast_off_enable;  		      // 18   ��ʱ�ٶ�ʹ��
	u16  max_current;  					          // 19   ����
	u16  max_current_time;  				      // 20   ����ʱ��
	u16  max_current_time_mode;  		      // 21   ����ʱ��
	u16  max_current_voltage_lock;  		  // 22   ������ѹ����
	u16  max_current_direction_lock;  	  // 23   �����������
	u16  max_current_enable;  			      // 24   ����ʹ��
	u16  max_current_reon_time;  		      // 25   �����غ�բʱ��
	u16  max_current_reon_enable;  		    // 26   �����غ�բʹ��
	u16  max_current_after;  			        // 27   ���������
	u16  max_current_after_time;  		    // 28   ���������ʱ��
	u16  max_current_after_enable;  		  // 29   ���������ʹ��
	u16  max_load;  						          // 30   ������
	u16  max_load_time;  				          // 31  	������ʱ��
	u16  max_load_enable;  				        // 32  	������ʹ��
	u16  zero_fast_off;  				          // 33   �����ٶ�
	u16  zero_fast_off_time;  			      // 34   �����ٶ�ʱ��
	u16  zero_fast_off_enable;  			    // 35   �����ٶ�ʹ��
	u16  zero_max_current;  				      // 36   �������
	u16  zero_max_current_time;  		      // 37   �������ʱ��
	u16  zero_max_current_enable;  		    // 38   �������ʹ��
	u16  zero_max_current_reon_time;  	  // 39   ��������غ�բʱ��
	u16  zero_max_current_reon_enable;  	// 40   ��������غ�բʹ��
	u16  zero_max_current_after;  		    // 41   ������������
	u16  zero_max_current_after_time;  	  // 42   ������������ʱ��
	u16  zero_max_current_after_enable;   // 43   ������������ʹ��
	u16  imbalance_current;  			        // 44   ��ƽ�����
	u16  imbalance_current_time;  		    // 45   ��ƽ�����ʱ��
	u16  imbalance_current_enable;  		  // 46   ��ƽ�����ʹ��
	u16  fast_off_bak_enable;			        // 47	  �ٶϺ�ʹ��
	u16  max_freq;  			                // 48   ��Ƶ����
	u16  max_freq_time;  		              // 49   ��Ƶ������ʱ
	u16  max_freq_enable;				          // 50   ��Ƶ����Ͷ��
	u16  low_freq;  			                // 48   ��Ƶ����
	u16  low_freq_time;  		              // 49   ��Ƶ������ʱ
	u16  low_freq_enable;				          // 50   ��Ƶ����Ͷ��
	u16  reon_Un_enable;				          // 54   �غ�բ����ѹʹ��
	u16  reon_synchron_enable;  		      // 55   �غ�բ��ͬ��ʹ��
	u16  min_voltage_lock;  				      // 56   ��ѹ����
	u16  reon_requency;                   // 57   �غ�բ����  
	u16  once_reon_time;                  // 58   һ���غ�բʱ��
	u16  once_reon_enable;  		          // 59   һ���غ�բʹ��
	u16  secondary_reon_time;             // 60   �����غ�բʱ��
	u16  phaseloss_protect_time;          // 61   ȱ�ౣ����ʱʱ��
	u16  phaseloss_protect_enable;        // 62   ȱ�ౣ����ʱʹ��
};
union PROTECT_PARA1
{
	struct PROTECT_PARA para;
	u16 word[sizeof(struct PROTECT_PARA)/2];
};

struct PROTECT_DELAY
{
	u16  zero_max_voltage;  			// 1   	�����ѹʱ��
	u16  zero_max_current;  			// 2    �������ʱ��
	u16  no_phase;                // 3    ȱ�ౣ��ʱ��
  u16  ground;  				        // 4   	�ӵ�ʱ��
	u16  fast_off;  					    // 5   	�ٶ�ʱ��
	u16  delay_fast_off;  				// 6   	��ʱ�ٶ�ʱ��
	u16  max_current;  				    // 7    ����ʱ��
	u16  max_current_after;  			// 8    ���������ʱ��	
	
	u16  voltage_value_A;         // 9    A����ѹ��ʱ   
	u16  voltage_value_B;         // 10   B����ѹ��ʱ
	u16  novoltage_value_A;       // 11   A����ѹ��ʱ   
	u16  novoltage_value_B;       // 12   B����ѹ��ʱ   
	u16  loss_power;  				    // 13   ʧ����ʱ
	u16  get_power;  		          // 14   �õ���ʱ
	u16  single_loss;  				    // 15   ����ʧ����ʱ
	u16  err_current;  		        // 16   ���ϼ����ʱ
	u16  err_fastoff;  			      // 17   ������բ
	u16  zero_voltage;  			    // 18   �����ѹ��ʱ
	u16  nonbreak_current;  		  // 19   ���ڶϵ�������
	
	u16  max_voltage;             // 20   ��ѹʱ��
  u16  max_freq;                // 21   ��Ƶʱ��
	u16  low_freq;                // 22   ��Ƶʱ��
};

union PROTECT_DELAY1
{
	struct PROTECT_DELAY para;
	u16 word[sizeof(struct PROTECT_DELAY)/2];
};

struct PROTECT_TIMEOUT
{	
	u16  zero_max_voltage;  			// 1   	�����ѹʱ��
	u16  zero_max_current;  			// 2    �������ʱ��
	u16  no_phase;                // 3    ȱ�ౣ��ʱ��
  u16  ground;  				        // 4   	�ӵ�ʱ��
	u16  fast_off;  					    // 5   	�ٶ�ʱ��
	u16  delay_fast_off;  				// 6   	��ʱ�ٶ�ʱ��
	u16  max_current;  				    // 7    ����ʱ��
	u16  max_current_after;  			// 8    ���������ʱ��	
	
	u16  voltage_value_A;         // 9    A����ѹ��ʱ   
	u16  voltage_value_B;         // 10   B����ѹ��ʱ
	u16  novoltage_value_A;       // 11   A����ѹ��ʱ   
	u16  novoltage_value_B;       // 12   B����ѹ��ʱ   
	u16  loss_power;  				    // 13   ʧ����ʱ
	u16  get_power;  		          // 14   �õ���ʱ
	u16  single_loss;  				    // 15   ����ʧ����ʱ
	u16  err_current;  		        // 16   ���ϼ����ʱ
	u16  err_fastoff;  			      // 17   ������բ
	u16  zero_voltage;  			    // 18   �����ѹ��ʱ
	u16  nonbreak_current;  		  // 19   ���ڶϵ�������
	
	u16  max_voltage;             // 20   ��ѹʱ��
  u16  max_freq;                // 21   ��Ƶʱ��
	u16  low_freq;                // 22   ��Ƶʱ��
};

union PROTECT_TIMEOUT1
{
	struct PROTECT_TIMEOUT para;
	u16 word[sizeof(struct PROTECT_TIMEOUT)/2];
};

// ������������
struct ALARM_PARA      				//��������
{
	u16  alarmU0;                     // 0  �����ѹ����
  u16  alarmU0_time;                // 1  �����ѹ������ʱʱ��
  u16	 alarmU0_enable;              // 2  �����ѹ����ʹ��
  u16  alarmI0;                     // 3  �����������
  u16  alarmI0_time;                // 4  �������������ʱʱ��
  u16	 alarmI0_enable;              // 5  �����������ʹ��
  u16  max_load;				            // 6  �����ɱ���
	u16  max_load_time;			          // 7  �����ɱ�����ʱʱ��
	u16  max_load_enable;             // 8  �����ɱ���ʹ��
	u16  max_voltage;				          // 9  ��ѹ����
	u16  max_voltage_time;            // 10 ��ѹ������ʱʱ��
	u16  max_voltage_enable;          // 11 ��ѹ����ʹ��
	u16  min_voltage;                 // 12 �͵�ѹ����
  u16  min_voltage_time;            // 13 �͵�ѹ������ʱʱ��
  u16	 min_voltage_enable;          // 14 �͵�ѹ����ʹ��
	u16  max_harmonic;                // 15 ��г������
  u16  max_harmonic_time;           // 16 ��г��������ʱʱ��
  u16	 max_harmonic_enable;         // 17 ��г������ʹ��
	u16  voltage_qualifrate;          // 18 ��ѹ�ϸ��ʱ���
	u16  qualificate_voltage_min;     // 19 ��ѹ�ϸ�������
	u16  qualificate_voltage_max;     // 20 ��ѹ�ϸ�������
	u16  voltage_qualifrate_enable;   // 21 ��ѹ�ϸ���ʹ��
	u16  battery_low_voltage;         // 22 ��ص͵�ѹ����

};
union ALARM_PARA1
{
	struct   ALARM_PARA para;
	u16 word[sizeof(struct ALARM_PARA)/2];
};

struct ALARM_DELAY
{
  u16  max_load;	              //0 ������						 
	u16  max_voltage;			        //1 ��ѹ	
	u16  min_voltage;             //2 Ƿѹ
	u16  max_harmonic;            //3 ��г��
	u16  voltage_qualifrate;      //4 ��ѹ�ϸ���
	u16  battery_low_voltage;     //5 ��ص͵�ѹ
	u16  groundU0;  			        //6 �ӵص�ѹ   
  u16  groundI0;  			        //7 �ӵص���
	u16  ground;  			          //8 �ӵ�
};

union ALARM_DELAY1
{
	struct ALARM_DELAY para;
	u16 word[sizeof(struct ALARM_DELAY)/2];
};

struct ALARM_TIMEOUT
{
  u16  max_load;	              //0 ������						 
	u16  max_voltage;			        //1 ��ѹ	
	u16  min_voltage;             //2 Ƿѹ
	u16  max_harmonic;            //3 ��г��
	u16  voltage_qualifrate;      //4 ��ѹ�ϸ���
	u16  battery_low_voltage;     //5 ��ص͵�ѹ
	u16  groundU0;  			        //6 �ӵص�ѹ   
  u16  groundI0;  			        //7 �ӵص���
	u16  ground;  			          //8 �ӵ�
};
union ALARM_TIMEOUT1
{
	struct ALARM_TIMEOUT para;
	u16 word[sizeof(struct ALARM_TIMEOUT)/2];
};

// �Զ����в�������
struct AUTO_SWITCHOFF 
{     			    	
  u16  segment_contact_mode;   // 1  ������ͨ�������Զ������ֶΣ�����ģʽ
	u16  voltage_value;          // 2  ��ѹ��ֵ
	u16  voltage_value_time;     // 3  ��ѹ��ֵ��ʱ0-99s
	u16  novoltage_value;        // 4  ʧѹ��ֵ
	u16  novoltage_value_time;   // 5  ʧѹ��ֵ��ʱ0-99s
  u16  losspower_enable;       // 6  ʧѹ��ʱ��բʹ��		
	u16  loss_power_time;  		   // 7  ʧ���բʱ�� 0-99s
	u16  getpower_enable_A;  		 // 8  A��õ��բʹ��
	u16  getpower_enable_B;  		 // 9  B��õ��բʹ��
	u16  get_power_time;  		   // 10 �õ���ʱʱ�� 0-99s
	u16  single_loss_enable_A;   // 11 A��ʧ����ʱʹ��
	u16  single_loss_enable_B;   // 12 B��ʧ����ʱʹ��
	u16  single_loss_time;       // 13 ����ʧ����ʱʱ��0-99s
	u16  lock_switchoff_enable;  // 14 ��բ�ɹ�����ʧѹ��
	u16  lock_switchoff_time;    // 15 ��բ�ɹ�����ʧѹ��ʱ��0-99s
	u16  err_lockon_enable;      // 16 �ϵ����ϱ�����բʹ��
	u16  err_current_time;  		 // 17 ���ϼ��ʱ��
	u16  err_fastoff_enable;     // 18 �ϵ����Ͽ�����բʹ��
	u16  err_fastoff_time;       // 19 �ϵ����Ͽ�����բʱ��0-99s

	u16  nonbreak_current_enable;// 20 ���ڶϵ���ʹ��
	u16  nonbreak_current_value; // 21 ���ڶϵ�����ֵ
	u16  inrush_current_enable;  // 22 ӿ��ʶ��ʹ��
	u16  inrush_current_value;   // 23 ӿ��ʶ��ֵ
	u16  lock_resvoltage_enable; // 24 ��ѹ����
	u16  PTbreak_alarm_enable;   // 25 PT���߸澯
  u16  phase_err_value;  			 // 26 �����ϵ�����ֵ
	u16  ground_err_value;       // 27 �ӵع��ϵ�����ֵ
  u16  err_current_enable;     // 28 �ϵ����ϵ����о�
	u16  zero_voltage_enable;    // 29 �����ѹ����
};

union AUTO_SWITCHOFF1
{
	struct AUTO_SWITCHOFF para;
	u16 word[sizeof(struct AUTO_SWITCHOFF)/2];
};

struct STATE1_BITS {     				// bits  
  u16  breaker:2;  			// 0~1  ��·��
	u16  handcart:2;  			// 2~3  �ֳ�
	u16  store:2;  				// 4~5  ����
	u16  knife:2;  				// 6~7  �ص�  ������û�еص�ң�ţ����������ͱ�����¼��
	u16  directionA:1;  			// 8   	A��������� 1=����
	u16  directionB:1;  			// 9   	B��������� 1=����
	u16  directionC:1;  			// 10  	C��������� 1=����
	u16  rsv3:1;  				// 11  	����
	u16  rsv4:1;  				// 12  	����
	u16  rsv5:1;  				// 13  	����
	u16  disconnection_on:1;  	// 14  	���غ���Ȧ����
	u16  disconnection_off:1;  	// 15  	���ط���Ȧ����
};
union STATE								// ��·��״̬ �ӱ���ʵʱ���ݶ�ȡ
{
	struct STATE1_BITS bit;
	u16 word;
};

struct PROTECT1_BITS {     			 // bits  
  u16  zero_max_voltage:1;  		   // 0   �����ѹ
	u16  zero_max_current:1;  		   // 1   �������
	u16  no_phase:1;                 // 2   ȱ��
	u16  ground:1;  		    	       // 3   �ӵ�
	u16  fast_off:1;  				       // 4   �ٶ�
	u16  delay_fast_off:1;  		     // 5   ��ʱ�ٶ�
	u16  max_current:1;  			       // 6   ����
	u16  max_current_after:1;  	     // 7   ���������
	
	u16  loss_power:1;  			       // 8   ʧ���բ
	u16  get_power:1;  		           // 9   �õ��բ
	u16  single_loss:1;  			       // 10  ʧ���բ
	u16  err_fastoff:1;  		         // 11  �ϵ����Ͽ��ٷ�բ
	u16  switchoff_lock:1;  		     // 12  ��բ����
  u16  switchon_lock:1;  		       // 13  ��բ����
	u16  zero_voltage:1;  		       // 14  �����ѹ����
	u16  ptbreak_alarm:1;  		       // 15  PT���߸澯
};

union PROTECT1							// ��������1
{
	struct PROTECT1_BITS bit;
	u16 word;
};

struct PROTECT2_BITS {     				// bits  
	u16  poweroff:1;  			// 0  ��Դʧ��
	u16  close_err:1;  		  // 1  �ϲ���
	u16  open_err:1;  			// 2  �ֲ���
	u16  lock:1;  					// 3  ����
	u16  reon_err:1;  			// 4  �غ�բ����
	u16  reon_fail:1;  		  // 5  �غ�բʧ��
	u16  reon_act:1;  			// 6  �غ�բ������־
  u16  protect_flag:1;    // 7  ������־
	
	u16  open_flag:1;  			// 8  ��բ��־
	u16  close_flag:1;  		// 9  ��բ��־
  u16  have_err:1;  			// 10 ���ϵ���
	u16  max_voltage:1;  		// 11 ��ѹ
	u16  max_freq:1;  		  // 12 ��Ƶ
	u16  low_freq:1;  			// 13 ��Ƶ
  u16  rsv1:1;            // 14 ����
	u16  rsv2:1;            // 15 ����
};

union PROTECT2							// ��������2
{
	struct PROTECT2_BITS bit;
	u16 word;
};

// ������������1
struct REQUEST1_BITS {     		 // bits  
  u16  record_on:1;  			     // 0  ��բ������¼
	u16  record_off:1;  			   // 1  ��բ������¼
	u16  record_protect:1;   	   // 2  ������¼
	u16  record_alarm:1;  	     // 3  ������¼
	u16  record_autofeed:1;  	   // 4  �����Զ�����¼
	u16  reon_act:1;             // 5  �غ�բ��¼
	u16  curve_current_forward:1;// 6  
	u16  curve_current_back:1;   // 7  
	u16  curve_on_A:1;  			   // 8 A���բ����
	u16  curve_on_B:1;  			   // 9 B���բ����
	u16  curve_on_C:1;  			   // 10C���բ����
	u16  curve_off_A:1;  		     // 11A���բ����
	u16  curve_off_B:1;  		     // 12B���բ����
	u16  curve_off_C:1;  		     // 13C���բ����
	u16  curve_protect_A:1;      // 14A�ౣ������
	u16  curve_protect_B:1;      // 15B�ౣ������
};

union REQUEST1							// ������������1
{
	struct REQUEST1_BITS bit;
	u16 word;
};
// ������������2
struct REQUEST2_BITS {     		// bits  
  u16  curve_protect_C:1;  	  // 0  	C�ౣ������
	u16  curve_position_on:1;   // 1  	��բλ������
	u16  curve_position_off:1;  // 2  	��բλ������
	u16  curve_ground_O:1;  	  // 3  	����ӵ�����
	u16  curve_PUA:1;  		      // 4  	A�ౣ����ѹ����
  u16  curve_PUB:1;  		      // 5    B�ౣ����ѹ����
	u16  curve_PUC:1;  		      // 6  	C�ౣ����ѹ����
  u16  curve_PU0:1;  		      // 7    ���������ѹ����
	u16  curve_PIA:1;  		      // 8  	A�ౣ����������
	u16  curve_PIB:1;  		      // 9  	B�ౣ����������
	u16  curve_PIC:1;  		      // 10  	C�ౣ����������
	u16  curve_PI0:1;  		      // 11   ���������������
	u16  rsv12:1;  		          // 12   ����
	u16  rsv13:1;  		          // 13   ����
	u16  rsv14:1;  		          // 14  	����
	u16  rsv15:1;  		          // 15  	����
};

union REQUEST2							// ������������2
{
	struct REQUEST2_BITS bit;
	u16 word;
};
// ������������3
struct REQUEST3_BITS {     		// bits  
	u16  curve_P10s_IB:1;  		// 0  	B��10�뱣��������Чֵ����
  u16  curve_P10s_IC:1;  		// 1  	C��10�뱣��������Чֵ����
	u16  rsv2:1;  		// 2    ����
	u16  rsv3:1;      // 3    ����
  u16  rsv4:1;  		// 4  	����
	u16  rsv5:1;  		// 5  	����
	u16  rsv6:1;  		// 6  	����
	u16  rsv7:1;  		// 7  	����
	u16  rsv8:1;  		// 8   	����
	u16  rsv9:1;  		// 9   	����
	u16  rsv10:1;  		// 10  	����
	u16  rsv11:1;  		// 11  	����
	u16  rsv12:1;  		// 12  	����
	u16  rsv13:1;  		// 13  	����
	u16  rsv14:1;  		// 14  	����
	u16  rsv15:1;  		// 15  	����
};

union REQUEST3							// ������������3
{
	struct REQUEST3_BITS bit;
	u16 word;
};

// ����ң��2
struct BTYX2_BITS {     			// bits  
  u16  rsv0:1;  		// 0  	��N12
	u16  BHYX1:1;  	  // 1  	����1N6
	u16  rsv1:1;  		// 2  	��N13
	u16  HQYX1:1;  	  // 3  	��բ����ң��1
  u16  BHYX2:1;  	  // 4  	����2N7
	u16  rsv2:1;  		// 5  	��N14
	u16  FQYX2:1;  	  // 6  	��բ����ң��2
	u16  BHYX3:1;  	  // 7  	����3N8
	u16  POW5V:1;  	  // 8   	5V2��ԴN15
	u16  BSYX3:1;  	  // 9   	����ң��3
	u16  BHYX4:1;  	  // 10  	����4N9
	u16  POW12V:1;  	// 11  	12V��ԴN16
	u16  HBYX4:1;  	  // 12  	�󱸶���ң��4
	u16  PTDX:1;  		// 13  	PT����
	u16  CTDX:1;  		// 14  	CT����
	u16  rsv5:1;  		// 15  	����
};

union BTYX2							// ����ң��2
{
	struct BTYX2_BITS bit;
	u16 word;
};
// ����ң��3
struct BTYX3_BITS {     			// bits  
  u16  voltage_lossA:1;  		// 0  	A��ʧѹ	  1=ʧѹ
	u16  voltage_lossB:1;  		// 1  	B��ʧѹ
	u16  voltage_lossC:1;  		// 2  	C��ʧѹ
	u16  voltage_sequence:1;  	// 3  	��ѹ�����	1=�����
  u16  current_sequence:1;  	// 4  	���������
	u16  current_directionA:1;  	// 5  	A���������	1=����
	u16  current_directionB:1;  	// 6  	B���������
	u16  current_directionC:1;  	// 7  	C���������
	u16  rsv8:1;  		// 8   	����
	u16  rsv9:1;  		// 9   	����
	u16  rsv10:1;  		// 10  	����
	u16  rsv11:1;  		// 11  	����
	u16  rsv12:1;  		// 12  	����
	u16  rsv13:1;  		// 13  	����
	u16  rsv14:1;  		// 14  	����
	u16  rsv15:1;  		// 15  	����
};

union BTYX3							// ����ң��3
{
	struct BTYX3_BITS bit;
	u16 word;
};

struct SELF_CHECK_BITS {    // bits  
	u16  ext_ram:1;  				  // 0	  �ⲿ�ڴ����
	u16  int_ram:1;  				  // 1	  �ڲ��ڴ����
	u16  att7022:1;  				  // 2	  ��ȱ�оƬ����
	u16  para_set:1;  				// 3	  ��ֵ����
	u16  GPRS_state:2;  		  // 4,5  GPRS״̬
	u16  rsv1:1;  						// 6  	����
	u16  WIFI_state:1;  			// 7  	wifi״̬
	u16  currentA:1;  				// 8  	A���������ͨ������
	u16  currentB:1;  				// 9  	B���������ͨ������
	u16  currentC:1;  				// 10  	C���������ͨ������
	u16  voltageA:1;  				// 11  	A���ѹ��������
	u16  voltageB:1;  				// 12  	B���ѹ��������
	u16  voltageC:1;  				// 13  	C���ѹ��������
	u16  Temp_Humi:1;  				// 14  	��ʪ��״̬
	u16  GPS_state:1;  				// 15  	GPS״̬
};
union SELF_CHECK_STATE				// �����Լ�״̬
{
	struct SELF_CHECK_BITS bit;
	u16 word;
};

struct ALARM_BITS {     			// bits  
	u16  max_load:1;  					// 0   	����
	u16  max_voltage:1;  				// 1   	��ѹ
	u16  min_voltage:1;  				// 2  	Ƿѹ
	u16  max_harmonic:1;  			// 3  	��г��
	u16  voltage_qualifrate:1;  // 4  	��ѹ�ϸ���
	u16  battery_low_voltage:1; // 5  	���Ƿѹ
	u16  ground:1;  				    // 6  	�ӵ�
	u16  zero_max_voltage:1;  	// 7  	�����ѹ
	u16  zero_max_current:1;  	// 8  	�������
	u16  bat_active:1;  				// 9  	��ػ
	u16  rsv10:1;  				      // 10  	����
	u16  rsv11:1;  				      // 11  	����
	u16  rsv12:1;  				      // 12  	����
	u16  rsv13:1;  				      // 13  	����
	u16  rsv14:1;  				      // 14  	����
	u16  rsv15:1;  				      // 15  	����
};
union ALARM_STATE					// ����״̬
{
	struct ALARM_BITS bit;
	u16 word;
};

// ң�� �ֽ�
struct YX_BITS {     			// bits  
  u8  D0:1;  		// 0  	��λң��
	u8  D1:1;  		// 1  	��λң��
	u8  D2:1;  		// 2  	��λң��
	u8  D3:1;  		// 3  	��λң��
  u8  D4:1;  		// 4  	��λң��
	u8  D5:1;  		// 5  	��λң��
	u8  D6:1;  		// 6  	��λң��
	u8  D7:1;  		// 7  	��λң��
};

union YXDefine					// ң�Ŷ���
{
	struct YX_BITS bit;
	u8 byte;
};

// ң��	��
struct YXWORD_BITS {     			// bits  
  u8  D0:1;  		// 0  	��λң��
	u8  D1:1;  		// 1  	��λң��
	u8  D2:1;  		// 2  	��λң��
	u8  D3:1;  		// 3  	��λң��
  u8  D4:1;  		// 4  	��λң��
	u8  D5:1;  		// 5  	��λң��
	u8  D6:1;  		// 6  	��λң��
	u8  D7:1;  		// 7  	��λң��
  u8  D8:1;  		// 8  	��λң��
	u8  D9:1;  		// 9  	��λң��
	u8  D10:1;  		// 10  	��λң��
	u8  D11:1;  		// 11  	��λң��
  u8  D12:1;  		// 12  	��λң��
	u8  D13:1;  		// 13  	��λң��
	u8  D14:1;  		// 14  	��λң��
	u8  D15:1;  		// 15  	��λң��
};

union YXWORDDefine					// ң�Ŷ���
{
	struct YXWORD_BITS bit;
	u16 word;
};

// ��·��������ʵʱ����
struct KZQMEASURE
{
	/*ң��*/
	union STATE DLQflag;				// ��·����־
	union PROTECT1 ProtectFlag1;		// ������־1
	union PROTECT2 ProtectFlag2;		// ������־2
	union REQUEST1 RequestFlag1;		// ���������־1
	union REQUEST2 RequestFlag2;		// ���������־2
	union REQUEST3 RequestFlag3;		// ���������־3
	u16 BTYX1;
	union BTYX2 YX2;
	union BTYX3 YX3;
	union SELF_CHECK_STATE SelfCheck;	// �Լ��־
	u16 ErrCode;
	union ALARM_STATE AlarmFlag;		// ������־
	union YXWORDDefine FXState;			// �����·��״̬
	/*ң��*/
	u16 Uab;				 	// �ߵ�ѹ//13
	u16 Ubc;
	u16 Uca;
	u16 PTUa;				 	// PT���ѹ
	u16 PTUb;
	u16 PTUc;
	u16 Ua;				 	// ���ѹ
	u16 Ub;
	u16 Uc;
	u16 degUA;
	u16 degUB;
	u16 degUC;
	u16 U0;				 	// �����ѹ
	u16 I0;					// �������
	u16 Freq;				// ����Ƶ�� 27
	u16 degU0;
	u16 degI0;
	u16 Ia;					// �ߵ���
	u16 Ib;
	u16 Ic;
	u16 degIA;
	u16 degIB;
	u16 degIC;
	s16	P_all;				// �����й�
  s16 Q_all;				// �����޹�
	u16 COS_all;			// ��������
	u16 Wp;					// �����й�����
	u16 Wq;					// �����޹�����
	u16 COS_W;				// ���յ�����������
	u16 XBU_all;			// ��ѹг��   42
	u16 XBU2;
	u16 XBU3;
	u16 XBU5;
	u16 XBU7;
	u16 XBU11;
	u16 XBU13;
	u16 XBU17;
	u16 XBU19;
	u16 XBI_all;			// ����г��
	u16 XBI2;
	u16 XBI3;
	u16 XBI5;
	u16 XBI7;
	u16 XBI11;
	u16 XBI13;
	u16 XBI17;
	u16 XBI19;
	u16 PUab;				// �����ߵ�ѹ
	u16 PUbc;
	u16 PUca;
	u16 PdegUab;
	u16 PIa;					// �����ߵ���    64
	u16 PIb;
	u16 PIc;
	u16 P10Ia;				// 10�������ߵ���
	u16 P10Ib;
	u16 P10Ic;                    //69
  u16 NJDFS;  
	u16 JDXB;
  u16 DYCFZC;   
  u16 JDXZ;
  u16 JDYF;
	u16 HGXB;
	u16 HGGDY;                      //76
	u16 Wp_all_L;		// �������й�����
	u16 Wp_all_H;		// �������й�����
	u16 Wq_1all_L;		// ���������޹�����
	u16 Wq_1all_H;		// ���������޹�����
	u16 Wp_peak_L;		// ������й�����
	u16 Wp_peak_H;		// ������й�����
	u16 Wq_2all_L;		// ���������޹�����
	u16 Wq_2all_H;		// ���������޹�����
	u16 Wp_valley_L;	// ������й�����
	u16 Wp_valley_H;	// ������й�����
	u16 Wq_3all_L;		// ���������޹�����
	u16 Wq_3all_H;		// ���������޹�����
	u16 Wp_level_L;	// ����ƽ�й�����
	u16 Wp_level_H;	// ����ƽ�й�����
	u16 Wq_4all_L;		// ���������޹�����
	u16 Wq_4all_H;		// ���������޹�����
	u16 EMUAB;         //93
	u16 EMUBC;
	u16 EMUCA;
	u16 EMIA;
	u16 EMIB;
	u16 EMIC;
	u16 COSA;
	u16 COSB;
	u16 COSC;
	u16 WpFX_all_L;		// �������й�����
	u16 WpFX_all_H;		// �������й�����
	u16 WpFX_peak_L;		// ������й�����
	u16 WpFX_peak_H;		// ������й�����
	u16 WpFX_valley_L;	// ������й�����
	u16 WpFX_valley_H;	// ������й�����
	u16 WpFX_level_L;	// ����ƽ�й�����
	u16 WpFX_level_H;	// ����ƽ�й�����
	u16 year;          //110
	u16 month;
	u16 date;
	u16 hour;
	u16 minute;
	u16 second;
	u16 battery_vol;
};
union KZQMEASUREDATA						// ��·��ʵʱ����
{
	struct KZQMEASURE para;
  u16 word[sizeof(struct KZQMEASURE)/2];
};

/**********************
*��¼
*���� = 29
**********************/
struct RECORDpara		// ��ϸ��¼
{
	u16 year;	// ��		0	byte		
	u16 month;	// ��			byte
	u16 date;	// ��			byte
	u16 hour;	// ʱ			byte
	u16 min;		// ��			byte
	u16 sec;		// ��		5	byte
	u16 msec;	// ����	6
	u16 type;			// 7��������
	u16 setvalue;	// 8����ֵ
	u16 setvaluetime;//9�趨��ʱ
	u16 Protect_UA_val;				// 10	A���ѹ
	u16 Protect_UB_val;				// 11	B���ѹ
	u16 Protect_UC_val;				// 12	C���ѹ
	u16 Protect_IA_val;				// 		A�����
	u16 Protect_IB_val;				// 		B�����
	u16 Protect_IC_val;				// 		C�����
	u16 P;					          // 		�й�����
	u16 Q;					          // 		�޹�����
	u16 COS;				          // 		��������
	u16 Protect_U0_val;				// 19 �����ѹ
	u16 Protect_I0_val;				// 20 �������
	u16 degU0;
	u16 degI0;
	u16 JDXZ;
	u16 DYCFZC;
	u16 JDXB;
	u16 Uqrate;			
  u16 Uharmonic;
	u16 Iharmonic;
	u16 Freq;                 // Ƶ��
};

union RECORD_ACTION			
{
	struct RECORDpara para;
	u16 word[sizeof(struct RECORDpara)/2];
};

extern s16 PcurveUA[1200];	// A�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveUB[1200];	// B�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveUC[1200];	// C�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveIA[1200];	// A�ౣ����������¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveIB[1200];	// B�ౣ����������¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveIC[1200];	// C�ౣ����������¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveU0[1200];	// ���򱣻���ѹ����¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveI0[1200];	// ���򱣻���������¼������������ǰ4�����ڣ�����������8������

extern s16 PcurveUA_bak[1200];	// A�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveUB_bak[1200];	// B�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveUC_bak[1200];	// C�ౣ����ѹ����¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveIA_bak[1200];	// A�ౣ����������¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveIB_bak[1200];	// B�ౣ����������¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveIC_bak[1200];	// C�ౣ����������¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveU0_bak[1200];	// ���򱣻���ѹ����¼������������ǰ4�����ڣ�����������8������
extern s16 PcurveI0_bak[1200];	// ���򱣻���������¼������������ǰ4�����ڣ�����������8������
/*******************************ATT7022E��ȡУ�����*********************************/
extern u32 ZYXSA,ZYXSB,ZYXSC;	// �й�����ϵ��
extern u32 XWXSA,XWXSB,XWXSC;	// ��λ����ϵ��
extern u32 WGXSA,WGXSB,WGXSC;	// �޹�����ϵ��
extern u32 DYXSA,DYXSB,DYXSC;	// ��ѹ����ϵ��
extern u32 DLXSA,DLXSB,DLXSC;	// ��������ϵ��
extern u32 DYPYA,DYPYB,DYPYC;	// ��ѹ����ϵ��
extern u32 DLPYA,DLPYB,DLPYC;	// ��������ϵ��
extern u32 DYXSA1,DYXSB1,DYXSC1;	// ��ѹ����ϵ��
extern u32 DLXSA1,DLXSB1,DLXSC1;	// ��������ϵ��
extern s32 ATT7022_Ept,ATT7022_Eqt,ATT7022_Est;			  // �й����ܣ��޹����ܣ����ڵ���
extern u32 ATT7022_sum;
extern u8 saveATT7022_flag;
extern u8 Start_Debug;
extern u8 TESTFlag;
extern u8 GPS_Flag;
extern u8 TestFlagCnt;
extern u8 Battery_Readflag;
extern u8 MachineInformation[13];	// ���������������(2)��(2)�������(7)

extern u16  Para_peak1_period_S;		// ����ͳ��ʱ������
extern u16  Para_peak1_period_E;
extern u16  Para_peak2_period_S;
extern u16  Para_peak2_period_E;
extern u16  Para_valley_period_S;
extern u16  Para_valley_period_E;

extern s16 AD_IA_val;               
extern s16 AD_IB_val;
extern s16 AD_IC_val;
extern s16 AD_PIA_val;
extern s16 AD_PIB_val;
extern s16 AD_PIC_val;
extern s16 AD_U0_val;
extern s16 AD_I0_val;

extern u32 WpLast;			      // ��һ�ε����й�����
extern u32 WqLast;			      // ��һ�ε����޹�
extern u32 Wp_all_Last;		    // ��һ�����й�����
extern u32 WpFX_all_Last;	    // ��һ���ܷ����й�����
extern u32 Wp_peak_Last;		  // ��һ�η��й�����
extern u32 WpFX_peak_Last;	  // ��һ�η巴���й�����
extern u32 Wp_valley_Last;	  // ��һ�ι��й�����
extern u32 WpFX_valley_Last;	// ��һ�ιȷ����й�����
extern u32 Wp_level_Last;	    // ��һ��ƽ�й�����
extern u32 WpFX_level_Last;	  // ��һ��ƽ�����й�����

extern u64 Wq_1all_Last;		  // ��һ��I�������޹�
extern u64 Wq_2all_Last;		  // ��һ��II�������޹�
extern u64 Wq_3all_Last;		  // ��һ��III�������޹�
extern u64 Wq_4all_Last;		  // ��һ��IV�������޹�
extern u16 PowerOffCnt;			  // ���������
extern u8 BattryTimeOut;
extern u8 YabanSave_Flag;			  // ѹ�屣���־
extern u8 ParaInitReadyFlag;		// ������ʼ��׼����

extern u8 MeasureIAError;			  // A��������ϱ�־
extern u8 MeasureIBError;			  // B��������ϱ�־
extern u8 MeasureICError;			  // C��������ϱ�־
extern u16 ZeroPointIA;				  // һ�������������	
extern u16 ZeroPointIA10;				// ʮ�������������	
extern u16 ZeroPointIB;				  // һ�������������	
extern u16 ZeroPointIB10;				// ʮ�������������	
extern u16 ZeroPointIC;				  // һ�������������	
extern u16 ZeroPointIC10;				// ʮ�������������	
extern u16 CtrlMeasureCnt;		  // ���Ʋ�������
extern u8  CtrlMeasureFlag;		  // ���Ʋ�����־
extern u8 StartCntFlag;         // ��բ������־
extern u16 SwitchCnt;           // ��������
extern u16 SelfCheckErrCode;		// �Լ���ϴ���
extern u8 AlarmRecord_flag;
extern u8 SwitchFlag;				    // ����״̬
extern u8 ReOnFlag;					    // �غ�բ��־
extern u16 ReOnTimeDelay;				// �غ�բ��ʱ10ms
extern u8 ReOnTimeOut;					// �غ�բʱ�䵽���־
extern u8 ReOnLockCnt;				  // �غ�բ����ʱ��10��
extern u8 ReOnLockCnt1;				  // �Ϲ����غ�բ����ʱ��10��
extern u16 ReOnDelay;					  // �غ�բ��ʱʱ��
extern u8 ReOnTimes;	          // �غ�բ����
extern u8 DLQZD_flag;				    // ��·�������Ϣ��־
extern u8 ProtectFlag;          // ������־
extern u16 ProtectTimesLock;	    // ����������������բλ��ֻ����һ�Σ���բ�������һ�α���
extern u8 ProtectTimesLockCnt;  // ����ʱ���������
extern u16 AlarmTimesLock;	      // ����������������բλ��ֻ����һ�Σ���բ�������һ�α���
extern u8 AlarmTimesLockCnt;		// ����ʱ���������
extern u8 Switchon_Lock;	      // ��բ����
extern u8 Switchon_Lock1;	      // ��������ֹ��բ
extern u8 Switchon_Lock2;	      // ��ѹ������բ
extern u8 Switchon_Lock2_Flag;
extern u8 Switchoff_Lock;       // ��բ����
extern u8 Switchoff_Lock1;       // ��բ����
extern u8 SigresetLock;         // ���������ʱ��ʼ��־
extern u8 AD_Index;		          // AD�������
extern u16 AD_StartPoint;		    // AD�����ٽ��
extern u8 ProtectRecord_flag;		// ������¼��־�����ڼ�¼�����������Ƿ�������
extern u16 Max_current_after_cnt; // ��������ټ�ʱ
extern u8  PTDX_Flag;				    // PT���߱�־��1=����
extern u8  CTDX_Flag;				    // CT���߱�־��1=����
extern u8  PTDX_Enable;				  // PT���߼������1=����
extern u8  CTDX_Enable;				  // CT���߼������1=����
extern u8  MeasureReady;				  // ������ɱ�־
extern u8  GpsOn_Flag;
extern u16 qualif_volt;           // ��ѹ�ϸ���
extern u8 AUTHORIZATIONcode[16];	//��Ȩ��
extern u8 ClearTJXX_Flag;
// DMA���ͻ���
extern u8 DMATXbuffer1[300]; 
extern u8 DMATXbuffer2[820]; 
extern u8 DMATXbuffer3[100]; 
extern u8 DMATXbuffer4[300];
extern u8 DMATXbuffer5[420]; 
extern u8 DMATXbuffer6[100]; 
// DMA���ջ���
extern u8 DMARXbuffer1[50];
extern u8 DMARXbuffer2[50];
extern u8 DMARXbuffer3[50];
extern u8 DMARXbuffer4[50];
extern u8 DMARXbuffer5[50];
extern u8 DMARXbuffer6[50];
extern u16 DMAADCbuffer1[100][9];
extern u16 DMAReadIndex1;
extern u16 DMAReadIndex2;
extern u16 DMAReadIndex3;
extern u16 DMAReadIndex4;
extern u16 DMAReadIndex5;
extern u16 DMAReadIndex6;

extern u8 Usart1RxReady; // ����1������ɱ�־
extern u8 Usart1bakRxReady; // ����1������ɱ�־
extern u8 Usart2RxReady; // ����2������ɱ�־
extern u8 Usart2bakRxReady; // ����2������ɱ�־
extern u8 Usart3RxReady; // ����3������ɱ�־
extern u8 Usart3bakRxReady; // ����3������ɱ�־
extern u8 Usart4RxReady; // ����4������ɱ�־
extern u8 Usart4bakRxReady; // ����4������ɱ�־
extern u8 Usart5RxReady; // ����5������ɱ�־
extern u8 Usart5bakRxReady; // ����5������ɱ�־
extern u8 Usart6RxReady; // ����6������ɱ�־
extern u8 Usart6bakRxReady; // ����6������ɱ�־
extern u8 Usart2TxReady;// ����2׼�����ͱ�־
extern u8 delay_time_101;
extern u8 timer_enable;	  //ʹ�ܼ�ʱ��
extern const  u8 SOE_Enable[20];
extern u8 RealCurveFlag;
extern u8 SOEPad_Flag;
extern u8 SOE_Flag;
extern u8 ModulusFlag;
extern u8 SetSystemFlag;
extern u8 SetProtectFlag;
extern u8 SetAlarmFlag;
extern u8 SetAutoswitchFlag;
extern u8 Set101ParaFlag;
extern u8 SigresetFlag;
extern u8 OperateNumber;
extern u8 GroundNumber;
extern u8 AlarmNumber;
extern u8 ProtectNumber;
extern u8 ZeroTimeArrived;
extern u8 KZQUpdataSucced;
extern u8 RecoverKZQBackupFlag;
extern u8 Select101Addr;
extern u8 EVENT_101Flag;  // 101Э���¼���־
extern u8 WifiConnectState;
extern u8 ControlLock;
extern u8 Battery_Voltage_Index;
extern u16 DLQ_EventRecord_Index;
extern u16 DLQ_EventRecord_Order[500];
extern u16 DLQ_SOERecord_Index;
extern u16 DLQ_SOERecord_Order[300];
extern u16 Battery_Voltage_Order[120];
extern u8 Curve_Index;
extern u16 Curve_Order[100];
extern u8 SendLock_Cnt;
extern u16 Battery_RunTime; // �������ʱ��
extern u16 CodeTimeout;
extern s16 Temp_value,Humi_value;
extern u32 Lock_IdCode;
extern u32 Reset_Value;
extern u16 JudgePoint;
extern u8 JudgePoint_Flag;
extern u8 HumiTest_Flag;
extern u8 Send_Flag;
extern u16 Freez_15min_cnt;

extern struct Measure          MeasureData;			     // ����ֵ
extern struct Measure1         MeasureCurve;		     // ��������
extern struct time_now         TimeNow;	     // ��ǰʱ��
extern struct UpdateVersion    MyVersion;	           // ���г���ͱ��ݳ���汾���޸�ʱ��
extern union  PROTECT_DELAY1   ProtectDelay;	       // ������ʱ��ʱ
extern union  PROTECT_TIMEOUT1 ProtectTimeout;	     // ����ʱ�䵽���־
extern union  ALARM_DELAY1     AlarmDelay;           // ������ʱ��ʱ
extern union  ALARM_TIMEOUT1   AlarmTimeout;         // ����ʱ�䵽���־
extern union  KZQMEASUREDATA   KZQMeasureData;	     // ��·��ʵʱ����
extern union  RECORD_ACTION    Record_on;	           // ��բ��¼
extern union  RECORD_ACTION    Record_off;	         // ��բ��¼
extern union  RECORD_ACTION    Record_protect1;	     // ������¼
extern union  RECORD_ACTION    Record_protect2;	     // ������¼
extern union  RECORD_ACTION    Record_alarm;	  	   // ������¼
extern union  RECORD_ACTION    Record_autofeed;      // �����Զ�����¼
extern union  SYSTEM_PARA1     SystemSet;	           // ϵͳ��������
extern union  PROTECT_PARA1    ProtectSet;	         // ������������
extern union  PROTECT_PARA1    ProtectSetReceive;
extern union  ALARM_PARA1      AlarmSet;	           // ������������
extern union  AUTO_SWITCHOFF1  AutoswitchSet;	       // �Զ����в�������
extern union  Modulus1         ProtectModulus;		   // ��������ϵ��
extern union  DLQZDXX          DLQZDinformation;     // ��·�������Ϣ
extern union  Rule101Para1     Measure101Para;      // 101��Լ����
extern union YXDefine          lockbz;

extern OS_TID t_Task1;                        /* assigned task id of task: Task1   */
extern OS_TID t_Task2;                        /* assigned task id of task: Task2  */
extern OS_TID t_Task3;                        /* assigned task id of task: Task3  */
extern OS_TID t_Task4;                        /* assigned task id of task: Task4  */
extern OS_TID t_Task5;                        /* assigned task id of task: Task5  */
extern OS_TID t_Task6;                        /* assigned task id of task: Task6  */
extern OS_TID t_Task7;                        /* assigned task id of task: Task7  */
extern OS_TID t_Task8;                        /* assigned task id of task: Task8  */
extern OS_TID t_Task9;                        /* assigned task id of task: Task9  */
extern OS_TID t_Task10;                        /* assigned task id of task: Task10  */
extern OS_TID t_Task11;                        /* assigned task id of task: Task11  */
extern OS_TID t_Task12;                        /* assigned task id of task: Task12  */
extern OS_TID t_Task13;                        /* assigned task id of task: Task13  */
extern OS_TID t_Task14;                        /* assigned task id of task: Task14  */
extern OS_TID t_Task15;                        /* assigned task id of task: Task15  */
extern OS_TID t_Task16;                        /* assigned task id of task: Task16  */
extern OS_TID t_Task17;                        /* assigned task id of task: Task17  */

extern struct QueuePara QueueTX1;			// ����1���Ͷ��п��Ʊ���
extern u8 BufferTX1[512];		// ����1���ͻ���

extern struct QueuePara QueueTX2;			// ����2���Ͷ��п��Ʊ���
extern u8 BufferTX2[820];		// ����2���ͻ���

extern struct QueuePara QueueTX3;			// ����3���Ͷ��п��Ʊ���
extern u8 BufferTX3[512];		// ����3���ͻ���

extern struct QueuePara QueueTX4;			// ����4���Ͷ��п��Ʊ���
extern u8 BufferTX4[512];		// ����4���ͻ���

extern struct QueuePara QueueTX5;			// ����5���Ͷ��п��Ʊ���
extern u8 BufferTX5[512];		// ����5���ͻ���

extern struct QueuePara QueueTX6;			// ����6���Ͷ��п��Ʊ���
extern u8 BufferTX6[512];		// ����6���ͻ���

extern struct QueuePara QueueSOE;			// SOE���Ͷ��п��Ʊ���
extern u8 BufferSOE[640];		// SOE���ͻ���

extern u16 SOE[16][11];

extern struct QueuePara QueueSOE_Pad;	// SOE_Pad���Ͷ��п��Ʊ���
extern u8 BufferSOE_Pad[300];// SOE_Pad���ͻ���

extern struct QueuePara *QueueTXChannel[6];
extern u8 *BufferTXChannel[6];

extern u8 ProtectParaChecked;		// �����������ÿ��ֻ��ȡflashһ�Σ���㸴λ
extern u8 SystemParaChecked;		// ϵͳ�������ÿ��ֻ��ȡflashһ�Σ���㸴λ
extern u8 AlarmParaChecked;     // �����������ÿ��ֻ��ȡflashһ�Σ���㸴λ
extern u8 AutoswitchParaChecked;// �Զ����в������ÿ��ֻ��ȡflashһ�Σ���㸴λ
extern u8 CorrectParaChecked;   // У��������ÿ��ֻ��ȡflashһ�Σ���㸴λ
extern u8 MeasureParaChecked;   // ����ϵ�����ÿ��ֻ��ȡflashһ�Σ���㸴λ
extern u8 Measure101ParaChecked;// 101��Լ�������ÿ��ֻ��ȡflashһ�Σ���㸴λ
extern u8 PeriodMode[24];			  // ʱ��ģʽ�����ڵ�����ʱͳ��

/************************** �ӵ� **************************/
extern u16  u0fd;         // u0fd=FJBU0/3
extern u16  xxpdxs;       // �����ж�ϵ��
extern s16  iuxw;         // U0�����жϵ���λ��I0��U0����λ�
extern u8   jdyf;         // �ӵ����
extern u8   zxdjdfs;      // ���Ե�ӵط�ʽ
extern s16  u0pdxw;       // ��ѹ�����֮�����λ
extern u8   jdxb;         // �ӵ����
extern u8   dycfzc;       // ��Դ��/���ز�
extern u8   dycfzc2;      // ��Դ��/���ز�2
extern u8   jdxz;         // �ӵ�����
extern u8   hgxb;         // �������
extern u16  hggdy;        // �������ѹ
/***************************************/
extern u8 under_voltage_on;
extern u8 RamTest(void);
extern void SaveCruve(u8 page,s16 *curve,s16 point);// ���汣������
extern void ReadCruve(u8 page,s16 *curve);// ��ȡ��������
extern void SaveProtectPara(void); // ���汣������
extern u8 ReadProtectPara(void);// ��ȡ��������
extern void SaveProtectParaBAK(void); // ���汣����������
extern u8 ReadProtectParaBAK(void);// ��ȡ������������
extern void SaveModulus(void);
extern u8 ReadModulus(void);
extern void SaveModulusBAK(void);
extern u8 ReadModulusBAK(void);
extern u8 SystemParaCompare(void);
extern u8 ProtectParaCompare(void);
extern u8 AlarmParaCompare(void);
extern u8 AutoswitchParaCompare(void);
extern u8 ModulusParaCompare(void);
extern void SaveSystemPara(void);// ����ϵͳ���ò���
extern u8 ReadSystemPara(void);// ��ȡϵͳ���ò���
extern void SaveSystemParaBAK(void);// ����ϵͳ���ò�������
extern u8 ReadSystemParaBAK(void);// ��ȡϵͳ���ò�������
extern void SaveAlarmPara(void);// ���汨�����ò���
extern u8 ReadAlarmPara(void);// ��ȡ�������ò���
extern void SaveAlarmParaBAK(void);// ���汨�����ò�������
extern u8 ReadAlarmParaBAK(void);// ��ȡ�������ò�������
extern u8 Read101Para(void);
extern u8 Read101ParaBAK(void);
extern void Save101Para(void);
extern void Save101ParaBAK(void);
extern u8 Measure101ParaCompare(void);

extern void SaveAutoSwitchPara(void);// �����Զ����в���
extern u8 ReadAutoSwitchPara(void);// ��ȡ�Զ����в���
extern void SaveAutoSwitchParaBAK(void);// �����Զ����в�������
extern u8 ReadAutoSwitchParaBAK(void);// ��ȡ�Զ����в�������

extern void SaveMachineInformation(void);
extern void SaveATT7022(void);
extern u8 Read_ATT7022(void);
extern void Read_ATT7022BAK(void);
extern void Write_ATT7022(void);
extern void Check_ATT7022(void);
extern void RecordSwitchOn(u8 switch_type);	   	// ��¼��բ������¼��Ϣ
extern void RecordSwitchOff(u8 switch_type);	   	// ��¼��բ������¼��Ϣ
extern void RecordProtectStart(u8 protect_type);	// ��¼����������¼��Ϣ
extern void RecordAlarm(u8 type,s16 set_value,s16 set_delay);
extern void SaveRecordAlarm(void);
extern void RecordAutofeed(u8 switch_type);	// �����Զ�����¼��Ϣ
extern void JDPD(void);

extern void SaveRecordOn(void);  // �����բ��¼
extern void SaveRecordOff(void);  // �����բ��¼
extern void SaveRecordProtect1(void); // ���汣����¼1
extern void SaveRecordProtect2(void);	 // ���汣����¼2
extern void SaveRecordAutofeed(void);	 // ���������Զ�����¼
extern u32 SqrtINT32U(u32 radicand); 	/* 32 λ���Ŀ��� */

extern void CheckSystemPara(void);          // ���ϵͳ��������
extern void CheckProtectPara(void);			    // ��鱣����������
extern void CheckAlarmPara(void);           // ��鱨����������
extern void CheckAutoSwitchPara(void);      // ����Զ����в�������
extern void CheckModulus(void);             // ����������
extern void CheckMeasure101Para(void);      // ���101��Լ����
extern void SavePowerOffPara(void);
extern void TaskRemarks(void);
extern void ClearTJXX(void);
extern void RecordSOE(u8 addr,u8 state);// SOE�¼���¼
extern void RecordSOE_Pad(u8 addr,u8 state);
extern void UART1_Configuration(s32 baudrate);
extern void ReadVersionfromFlash(void);
extern void ReadInformation(u8 mode);
extern void Delay(u32 nCount);
extern void ReadPowerOffPara(void);
extern void ReadMachineInformation(void);
extern void MeasureDataCalcAndProcess(void);
extern u8 Humi_Handle_Step(void);
#endif

