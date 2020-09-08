/*************************************************************
*�ļ����ƣ�sysinit.h
*�������ڣ�2019��07��17��
*������ڣ�
*��    �ߣ�shq
**************************************************************/

#ifndef _SYSINIT_H_
#define _SYSINIT_H_
#include "global.h"
  /* Private define ------------------------------------------------------------*/
  #define USART1_GPIO              GPIOA
  #define USART1_CLK               RCC_APB2Periph_USART1
  #define USART1_GPIO_CLK          RCC_AHB1Periph_GPIOA
  #define USART1_RxPin             GPIO_Pin_10
  #define USART1_TxPin             GPIO_Pin_9
  #define USART1_IRQn              USART1_IRQn
  #define USART1_IRQHandler        USART1_IRQHandler
  
  #define USART2_GPIO              GPIOA
  #define USART2_CLK               RCC_APB1Periph_USART2
  #define USART2_GPIO_CLK          RCC_AHB1Periph_GPIOA
  #define USART2_RxPin             GPIO_Pin_3
  #define USART2_TxPin             GPIO_Pin_2
  #define USART2_IRQn              USART2_IRQn
  #define USART2_IRQHandler        USART2_IRQHandler

  #define USART3_GPIO              GPIOB
  #define USART3_CLK               RCC_APB1Periph_USART3
  #define USART3_GPIO_CLK          RCC_AHB1Periph_GPIOB
  #define USART3_RxPin             GPIO_Pin_11
  #define USART3_TxPin             GPIO_Pin_10
  #define USART3_IRQn              USART3_IRQn
  #define USART3_IRQHandler        USART3_IRQHandler
  
  #define UART4_GPIO               GPIOC
  #define UART4_CLK                RCC_APB1Periph_UART4
  #define UART4_GPIO_CLK           RCC_AHB1Periph_GPIOC
  #define UART4_RxPin              GPIO_Pin_11
  #define UART4_TxPin              GPIO_Pin_10
  #define UART4_IRQn               UART4_IRQn
  #define UART4_IRQHandler         UART4_IRQHandler

  #define UART5_GPIO               GPIOC
  #define UART5_CLK                RCC_APB1Periph_UART5
  #define UART5_GPIO_CLK           RCC_AHB1Periph_GPIOC
  #define UART5_RxPin              GPIO_Pin_2
  #define UART5_TxPin              GPIO_Pin_12
  #define UART5_IRQn               UART5_IRQn
  #define UART5_IRQHandler         UART5_IRQHandler

  #define USART6_GPIO              GPIOC
  #define USART6_CLK               RCC_APB2Periph_USART6
  #define USART6_GPIO_CLK          RCC_AHB1Periph_GPIOC
  #define USART6_RxPin             GPIO_Pin_7
  #define USART6_TxPin             GPIO_Pin_6
  #define USART6_IRQn              USART6_IRQn
  #define USART6_IRQHandler        USART6_IRQHandler
	
	/*FSMC CS Ƭѡ*/
  #define FMC_CS_GPIO_PORT         GPIOD
  #define FMC_CS_GPIO_CLK          RCC_AHB1Periph_GPIOD
  #define FMC_CS_GPIO_PIN          GPIO_Pin_7
  #define FMC_CS_PINSOURCE         GPIO_PinSource7

  #define FSMC_D0_PIN              GPIO_Pin_14
	#define FSMC_D0_PINSOURCE        GPIO_PinSource14
	#define FSMC_D1_PIN              GPIO_Pin_15
	#define FSMC_D1_PINSOURCE        GPIO_PinSource15
	#define FSMC_D2_PIN              GPIO_Pin_0
	#define FSMC_D2_PINSOURCE        GPIO_PinSource0
	#define FSMC_D3_PIN              GPIO_Pin_1
	#define FSMC_D3_PINSOURCE        GPIO_PinSource1
	#define FSMC_D4_PIN              GPIO_Pin_7
	#define FSMC_D4_PINSOURCE        GPIO_PinSource7
	#define FSMC_D5_PIN              GPIO_Pin_8
	#define FSMC_D5_PINSOURCE        GPIO_PinSource8
	#define FSMC_D6_PIN              GPIO_Pin_9
	#define FSMC_D6_PINSOURCE        GPIO_PinSource9
	#define FSMC_D7_PIN              GPIO_Pin_10
	#define FSMC_D7_PINSOURCE        GPIO_PinSource10
	#define FSMC_D8_PIN              GPIO_Pin_11
	#define FSMC_D8_PINSOURCE        GPIO_PinSource11
	#define FSMC_D9_PIN              GPIO_Pin_12
	#define FSMC_D9_PINSOURCE        GPIO_PinSource12
	#define FSMC_D10_PIN             GPIO_Pin_13
	#define FSMC_D10_PINSOURCE       GPIO_PinSource13
	#define FSMC_D11_PIN             GPIO_Pin_14
	#define FSMC_D11_PINSOURCE       GPIO_PinSource14
	#define FSMC_D12_PIN             GPIO_Pin_15
	#define FSMC_D12_PINSOURCE       GPIO_PinSource15
	#define FSMC_D13_PIN             GPIO_Pin_8
	#define FSMC_D13_PINSOURCE       GPIO_PinSource8
	#define FSMC_D14_PIN             GPIO_Pin_9
	#define FSMC_D14_PINSOURCE       GPIO_PinSource9
	#define FSMC_D15_PIN             GPIO_Pin_10
	#define FSMC_D15_PINSOURCE       GPIO_PinSource10
	
  /*W25X64  flash*/
	#define     W25X64_Init         SPI1_Init
	#define     W25X64_ReadWrite    SPI1_ReadWrite
	#define     SPI_FLASH_CS_H			GPIO_SetBits(GPIOA,GPIO_Pin_4)
	#define     SPI_FLASH_CS_L			GPIO_ResetBits(GPIOA,GPIO_Pin_4)
  #define     SPI_FLASH_CS2_H			GPIO_SetBits(GPIOF,GPIO_Pin_9)
  #define     SPI_FLASH_CS2_L			GPIO_ResetBits(GPIOF,GPIO_Pin_9)
  #define     SPI2_FLASH_CS_H			GPIO_SetBits(GPIOB,GPIO_Pin_12)
  #define     SPI2_FLASH_CS_L			GPIO_ResetBits(GPIOB,GPIO_Pin_12)

  /*����2 485������ض���*/
  #define     RCC_GPIO_T485_2     RCC_AHB1Periph_GPIOA    /*T485ʹ�õ�GPIOʱ��*/
  #define     GPIO_T485_2         GPIOA                   /*485ʹ�õ�GPIO��*/
  #define     T485_2_PIN          GPIO_Pin_1              /*485ʹ�õ�GPIO�ܽ�*/
  #define     T485_2_RX						GPIO_ResetBits(GPIO_T485_2,T485_2_PIN);
  #define     T485_2_TX						GPIO_SetBits(GPIO_T485_2,T485_2_PIN);

  /***********************WIFI,GPS,SC1161Y����*************************************/  
	#define     WIFI_OFF	          GPIO_SetBits(GPIOG,GPIO_Pin_6)	// WIFIģ��ر�
  #define     WIFI_ON	            GPIO_ResetBits(GPIOG,GPIO_Pin_6)// 
	#define     GPS_OFF	            GPIO_SetBits(GPIOG,GPIO_Pin_7)	// GPSģ��ر�
  #define     GPS_ON	            GPIO_ResetBits(GPIOG,GPIO_Pin_7)// 
	#define     SC1161Y_OFF	        GPIO_SetBits(GPIOB,GPIO_Pin_3)	// SC1161YоƬ�ر�
  #define     SC1161Y_ON	        GPIO_ResetBits(GPIOB,GPIO_Pin_3)//

  /***********************��ʪ��IO��*************************************/
	#define     HTDATA_IN           GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)
	#define     HTDATA_OUT_0        GPIO_ResetBits(GPIOD,GPIO_Pin_6)
	#define     HTDATA_OUT_1        GPIO_SetBits(GPIOD,GPIO_Pin_6)
	#define     HTCLK_OUT_0         GPIO_ResetBits(GPIOD,GPIO_Pin_5)
	#define     HTCLK_OUT_1         GPIO_SetBits(GPIOD,GPIO_Pin_5)
	
	/***********************�������*************************************/
	#define     DJZZKZ_ON           GPIO_ResetBits(GPIOB,GPIO_Pin_8)
	#define     DJZZKZ_OFF          GPIO_SetBits(GPIOB,GPIO_Pin_8)
	#define     DJFZKZ_ON           GPIO_ResetBits(GPIOG,GPIO_Pin_9)
	#define     DJFZKZ_OFF          GPIO_SetBits(GPIOG,GPIO_Pin_9)
	
	/***********************�ֺ�բ���ܿ���*************************************/
	#define     HZLED_OFF	          GPIO_SetBits(GPIOG,GPIO_Pin_10)	// ��բָʾ�ƹ�
  #define     HZLED_ON	          GPIO_ResetBits(GPIOG,GPIO_Pin_10)// ��բָʾ�ƿ�	
	#define     FZLED_OFF	          GPIO_SetBits(GPIOG,GPIO_Pin_11)	// ��բָʾ�ƹ�
	#define     FZLED_ON            GPIO_ResetBits(GPIOG,GPIO_Pin_11)// ��բָʾ�ƿ�
	#define     HE_ON		            GPIO_ResetBits(GPIOG,GPIO_Pin_0)
	#define     HE_OFF		          GPIO_SetBits(GPIOG,GPIO_Pin_0)
  #define     FEN_ON		          GPIO_ResetBits(GPIOG,GPIO_Pin_1)
	#define     FEN_OFF		          GPIO_SetBits(GPIOG,GPIO_Pin_1)
	#define     KZCN_ON             GPIO_ResetBits(GPIOG,GPIO_Pin_2)
  #define     KZCN_OFF            GPIO_SetBits(GPIOG,GPIO_Pin_2)
  #define     OUT_COM_ON	        GPIO_SetBits(GPIOG,GPIO_Pin_3)
	#define     OUT_COM_OFF	        GPIO_ResetBits(GPIOG,GPIO_Pin_3)
	
	/***********************��ر����¹��ⲿ���Ź�******************************/
	#define     BATTERY_CHARGE_ON   GPIO_ResetBits(GPIOE,GPIO_Pin_3)
	#define     BATTERY_CHARGE_OFF  GPIO_SetBits(GPIOE,GPIO_Pin_3)
  #define     BATT_ON             GPIO_ResetBits(GPIOF,GPIO_Pin_11)
	#define     BATT_OFF            GPIO_SetBits(GPIOF,GPIO_Pin_11)
  #define     OUT4_ON		          GPIO_ResetBits(GPIOF,GPIO_Pin_12)
	#define     OUT4_OFF		        GPIO_SetBits(GPIOF,GPIO_Pin_12)
  #define     OUT5_ON		          GPIO_ResetBits(GPIOF,GPIO_Pin_13)
	#define     OUT5_OFF		        GPIO_SetBits(GPIOF,GPIO_Pin_13)
  #define     OUT6_ON		          GPIO_SetBits(GPIOF,GPIO_Pin_14)                //out6Ϊ���öˣ��ߵ�ƽ���ֳ���
	#define     OUT6_OFF		        GPIO_ResetBits(GPIOF,GPIO_Pin_14)
	#define     EXIT_WATCHDOG_0     GPIO_ResetBits(GPIOF,GPIO_Pin_15)
	#define     EXIT_WATCHDOG_1     GPIO_SetBits(GPIOF,GPIO_Pin_15)
	
	/***********************�ⲿң�Ŷ�ȡ******************************/
	#define     LED_Judge           GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4)
	#define     DYXH1	              GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)     // �������ѹ�ź�1
  #define     DYXH2	              GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)	    // �������ѹ�ź�2
	#define     FW_IN1		          GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)	    // ��λ�ź�
  #define     HW_IN2              GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)     // ��λ�ź�
	#define     WCN_IN3             GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)     // δ�����ź�
	#define     DJ_IN4              GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)     // 18�Ŷ���
  #define     DJ_IN5              GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)     // 17�ų���
	#define     FAR_IN6             GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8)     // Զ��
	#define     LOCAL_IN7           GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)     // ����
	#define     IN8                 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)     // ����
	extern void USART5_Configuration(u32 baudrate);
  extern void System_Init(void);
	extern void DMA_Configuration(void);
	extern void RCC_Configuration(void);
#endif
