#include "sysinit.h"
#include "queue.h"
#include "flash.h"
/* Private macro -------------------------------------------------------------*/  

struct QueuePara QueueTX1;			// 串口1发送队列控制变量
u8 BufferTX1[512] = {0};	      // 串口1发送缓冲

struct QueuePara QueueTX2;			// 串口2发送队列控制变量
u8 BufferTX2[820] = {0};		    // 串口2发送缓冲

struct QueuePara QueueTX3;			// 串口3发送队列控制变量
u8 BufferTX3[512] = {0};	      // 串口3发送缓冲

struct QueuePara QueueTX4;			// 串口4发送队列控制变量
u8 BufferTX4[512] = {0};		    // 串口4发送缓冲

struct QueuePara QueueTX5;			// 串口5发送队列控制变量 
u8 BufferTX5[512] = {0};		    // 串口5发送缓冲

struct QueuePara QueueTX6;			// 串口6发送队列控制变量
u8 BufferTX6[512] = {0};		    // 串口6发送缓冲

struct QueuePara QueueSOE_Pad;	// SOE_Pad发送队列控制变量  30组，每组10个字节   
u8 BufferSOE_Pad[300] = {0};;   // SOE_Pad发送缓冲

struct QueuePara QueueSOE;			// SOE发送队列控制变量 	64组，每组10个字节
u8 BufferSOE[640] = {0};		    // SOE发送缓冲

struct QueuePara *QueueTXChannel[6];
u8 *BufferTXChannel[6];
	
GPIO_InitTypeDef GPIO_InitStructure;

USART_InitTypeDef USART_InitStructure;
USART_ClockInitTypeDef USART_ClockInitStruct;

//void RCC_PLL_Configuration(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void UART_Configuration(void);
void Timer_Configuration(void);
void SysTick_Configuration(void);
void DMA_Configuration(void);
void FSMC_Configuration(void);
void Exti_Init(void);
void ADC_Configuration(void);

void System_Init(void)
{
	//RCC_PLL_Configuration();
  /* System Clocks Configuration */
  RCC_Configuration();
	
  /* NVIC configuration */
  NVIC_Configuration();
  
  /* Configure the GPIO ports */
  GPIO_Configuration();
	DMA_Configuration();
  UART_Configuration();
	Exti_Init();
  SPI_Flash_Init();
  Timer_Configuration();
	FSMC_Configuration();
  //SysTick_Configuration();
	ADC_Configuration();
  CreateQueue(&QueueTX1,512);
  CreateQueue(&QueueTX2,820);
  CreateQueue(&QueueTX3,512);
  CreateQueue(&QueueTX4,512);
  CreateQueue(&QueueTX5,512);
  CreateQueue(&QueueTX6,512);	
  CreateQueue(&QueueSOE,640);
	CreateQueue(&QueueSOE_Pad,300);
	QueueTXChannel[0] = &QueueTX1;
	QueueTXChannel[1] = &QueueTX2;
	QueueTXChannel[2] = &QueueTX3;
	QueueTXChannel[3] = &QueueTX4;
	QueueTXChannel[4] = &QueueTX5;
	QueueTXChannel[5] = &QueueTX6;
	BufferTXChannel[0] = BufferTX1;
	BufferTXChannel[1] = BufferTX2;
	BufferTXChannel[2] = BufferTX3;
	BufferTXChannel[3] = BufferTX4;
	BufferTXChannel[4] = BufferTX5;
	BufferTXChannel[5] = BufferTX6;
}

//void RCC_PLL_Configuration(void)
//{
//  RCC_DeInit(); /*将外设RCC寄存器重设为缺省值 */
//  RCC_HSEConfig(RCC_HSE_ON); /*设置外部高速晶振（HSE） HSE晶振打开(ON)*/
//  if(RCC_WaitForHSEStartUp() == SUCCESS) 
// {  /*等待HSE起振,  SUCCESS：HSE晶振稳定且就绪*/
//  while(RCC_GetSYSCLKSource() != 0x08);     /*0x08：PLL作为系统时钟 */	  
// }
//}

void RCC_Configuration(void)
{   
  /*使能串口1时钟*/
  RCC_APB2PeriphClockCmd(USART1_CLK, ENABLE); 
  /*使能串口2时钟*/
  RCC_APB1PeriphClockCmd(USART2_CLK, ENABLE); 
  /*使能串口3时钟*/
  RCC_APB1PeriphClockCmd(USART3_CLK, ENABLE); 
  /*使能串口4时钟*/
  RCC_APB1PeriphClockCmd(UART4_CLK, ENABLE); 
  /*使能串口5时钟*/
  RCC_APB1PeriphClockCmd(UART5_CLK, ENABLE); 
  /*使能串口6时钟*/
  RCC_APB2PeriphClockCmd(USART6_CLK, ENABLE);    
  /*使能LED灯使用的GPIO时钟*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);  //开启FSMC时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  //开启DMA1时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  //开启DMA2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  //开启ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);	//ADC1复位结束

//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

/*
********************************************************************************
** 函数名称 ： FSMC_Configuration(void)
** 函数功能 ： FSMC初始化
** 输    入	： 无
** 输    出	： 无
** 返    回	： 无
********************************************************************************
*/
void FSMC_Configuration(void)
{
	FSMC_NORSRAMInitTypeDef        FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTimingInitStructure;
	
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 0x0; //地址建立时间为13个HCLK
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0x00;  //地址保持时间模式A未用到
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 0x8;    //数据保持时间为13个HCLK
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;//块1区1 
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //地址数据不复用
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;//设置为SRAM存储类型
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b; //存储器数据宽度为8bit，因为硬件上采用的是并行字节模式
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;//我们采用异步模式，不是能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = 1;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait= FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; 
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Disable;//存储器写禁止
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; //读写使用相同的时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;//读写使用相同的时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;    //读写使用相同的时序
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //初始化FSMC配置
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 
}

/*
********************************************************************************
** 函数名称 ： ADC_Configuration(void)
** 函数功能 ： ADC初始化
** 输    入	： 无
** 输    出	： 无
** 返    回	： 无
********************************************************************************
*/
void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//ADC的最大工作频率为36MHz，所以APB2进行4分频，使ADCCLK频率为21MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//禁止DMA直接访问模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;//仅适用于双重和三重模式
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC1 Init */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式，多通道扫描
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//ENABLE;//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//上升沿触发检测
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;       //Time2定时器触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
  ADC_InitStructure.ADC_NbrOfConversion = 9;//9个转换在规则序列中
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
  
  /* ADC1 regular channels configuration */  //设置ADC1的通道参数：通道10，规则组第1通道，采样时间56个周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10,1, ADC_SampleTime_28Cycles);  /*UA1*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11,2, ADC_SampleTime_28Cycles);  /*UB1*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12,3, ADC_SampleTime_28Cycles);  /*UC1*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13,4, ADC_SampleTime_28Cycles);  /*UA2*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14,5, ADC_SampleTime_28Cycles);  /*UB2*/
 	ADC_RegularChannelConfig(ADC1, ADC_Channel_15,6, ADC_SampleTime_28Cycles);  /*UC2*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 7, ADC_SampleTime_28Cycles);  /*I0-10*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 8, ADC_SampleTime_28Cycles);  /*VG*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 9, ADC_SampleTime_28Cycles);  /*V-BAT*/
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);
  	
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE); //使能DMAADC
	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//ADC转换结束产生中断，在中断服务程序中读取转换值
  ADC_Cmd(ADC1, ENABLE);
}

void Exti_Init(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,EXTI_PinSource13);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void USART5_Configuration(u32 baudrate)
{
	/*失能串口5 */
	USART_Cmd(UART5, DISABLE);
	/* DISABLE UART5 DMA TX request */  
  USART_DMACmd(UART5, USART_DMAReq_Tx, DISABLE);
  /* DISABLE UART5 DMA RX request */  
  USART_DMACmd(UART5, USART_DMAReq_Rx, DISABLE);
	/*失能串口5的发送和接收中断*/
  USART_ITConfig(UART5, USART_IT_TC, DISABLE);
  USART_ITConfig(UART5, USART_IT_IDLE, DISABLE);
	USART_InitStructure.USART_BaudRate = baudrate;             /*设置波特率为baudrate*/
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*设置数据位为8*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
  USART_InitStructure.USART_Parity = USART_Parity_No;        /*无奇偶校验*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/
  /*配置串口5 */
  USART_Init(UART5, &USART_InitStructure);
	/*使能串口5的发送和接收中断*/
  USART_ITConfig(UART5, USART_IT_TC, ENABLE);
  USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
  /* Enable UART5 DMA TX request */  
  USART_DMACmd(UART5, USART_DMAReq_Tx, ENABLE);
  /* Enable UART5 DMA RX request */  
  USART_DMACmd(UART5, USART_DMAReq_Rx, ENABLE);
	/* 使能串口5 */
  USART_Cmd(UART5, ENABLE);
	USART_ClearFlag(UART5,USART_FLAG_TC);
}

void UART_Configuration(void)
{
  /*配置串口 1 */
	USART_InitStructure.USART_BaudRate = 115200;               /*设置波特率为115200*/ 
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*设置数据位为8*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
  USART_InitStructure.USART_Parity = USART_Parity_No;        /*无奇偶校验*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/
  USART_Init(USART1, &USART_InitStructure);		               // GPRS口
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USART1,&USART_ClockInitStruct);
	
  USART_InitStructure.USART_BaudRate = 115200;               /*设置波特率为115200*/
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*设置数据位为8*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
  USART_InitStructure.USART_Parity = USART_Parity_No;        /*无奇偶校验*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/
  USART_Init(USART2, &USART_InitStructure);		              // 485 口
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USART2,&USART_ClockInitStruct);
	
  USART_InitStructure.USART_BaudRate = 115200;               /*设置波特率为115200*/
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*设置数据位为8*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
  USART_InitStructure.USART_Parity = USART_Parity_No;        /*无奇偶校验*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/
  USART_Init(USART3, &USART_InitStructure);		              // 对外232接口
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USART3,&USART_ClockInitStruct);
	
  USART_InitStructure.USART_BaudRate = 9600;               /*设置波特率为9600*/
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;/*设置数据位为9*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
  USART_InitStructure.USART_Parity = USART_Parity_Even;      /*偶校验*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/
  USART_Init(UART4, &USART_InitStructure);		               // 对外232接口
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(UART4,&USART_ClockInitStruct);
	
  USART_InitStructure.USART_BaudRate = 115200;               /*设置波特率为115200*/
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*设置数据位为8*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
  USART_InitStructure.USART_Parity = USART_Parity_No;        /*无奇偶校验*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/
  USART_Init(UART5, &USART_InitStructure);		               // WIFI模块通讯
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(UART5,&USART_ClockInitStruct);
	
  USART_InitStructure.USART_BaudRate = 600000;               /*设置波特率为600000*/
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*设置数据位为8*/
  USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
  USART_InitStructure.USART_Parity = USART_Parity_No;        /*无奇偶校验*/
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/
  USART_Init(USART6, &USART_InitStructure);		               // 备用
  USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USART6,&USART_ClockInitStruct);
	
  /*使能串口1的发送和接收中断*/
  USART_ITConfig(USART1, USART_IT_TC, ENABLE);
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
  /*使能串口2的发送和接收中断*/
  USART_ITConfig(USART2, USART_IT_TC, ENABLE);
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
  /*使能串口3的发送和接收中断*/
  USART_ITConfig(USART3, USART_IT_TC, ENABLE);
  USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
  /*使能串口4的发送和接收中断*/
  USART_ITConfig(UART4, USART_IT_TC, ENABLE);
  USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
  /*使能串口5的发送和接收中断*/
  USART_ITConfig(UART5, USART_IT_TC, ENABLE);
  USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
//	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
  /*使能串口6的发送和接收中断*/
  USART_ITConfig(USART6, USART_IT_TC, ENABLE);
  USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);
	
  /* Enable USART1 DMA TX request */  
  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  
  /* Enable USART2 DMA TX request */  
  USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); 
  /* Enable USART3 DMA TX request */  
  USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
  /* Enable UART4 DMA TX request */  
  USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
  /* Enable UART5 DMA TX request */  
  USART_DMACmd(UART5, USART_DMAReq_Tx, ENABLE);
  /* Enable USART6 DMA TX request */  
  USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE); 


  /* Enable USART1 DMA RX request */  
  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);  
  /* Enable USART2 DMA RX request */  
  USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); 
  /* Enable USART3 DMA RX request */  
  USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
  /* Enable UART4 DMA RX request */  
  USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
  /* Enable UART5 DMA RX request */  
  USART_DMACmd(UART5, USART_DMAReq_Rx, ENABLE);
  /* Enable USART6 DMA RX request */  
  USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);	
	
	
//  /* 使能串口1 */
	USART_Cmd(USART1, ENABLE);
	USART_ClearFlag(USART1,USART_FLAG_TC);
//  /* 使能串口2 */
  USART_Cmd(USART2, ENABLE);
	USART_ClearFlag(USART2,USART_FLAG_TC);
//  /* 使能串口3 */
  USART_Cmd(USART3, ENABLE);
	USART_ClearFlag(USART3,USART_FLAG_TC);
//  /* 使能串口4 */
  USART_Cmd(UART4, ENABLE);
	USART_ClearFlag(UART4,USART_FLAG_TC);
//  /* 使能串口5 */
  USART_Cmd(UART5, ENABLE);
	USART_ClearFlag(UART5,USART_FLAG_TC);
//  /* 使能串口6 */
  USART_Cmd(USART6, ENABLE);
	USART_ClearFlag(USART6,USART_FLAG_TC);
}

/* 管脚定义对照
				bit15		bit14		bit13		bit12		bit11		bit10		bit9		bit8		bit7		bit6		bit5		bit4		bit3		bit2		bit1		bit0
PortA		out			仿真		仿真		out			out			AF			AF			nc			AF			AF			AF			out			AF			AF			nc			nc
PortB		AF			AF			AF			out			AF			AF			nc			nc			out			nc			AF			nc			AF			in			out			out			
PortC		晶振		晶振		nc			AF			AF			AF			out			out			AF			AF			nc			nc			nc			nc			nc			nc			
PortD		nc			nc			nc			in			in			in			in			out			nc			out			out			out			nc			AF			nc			in			
PortE		out			out			out			out			out			out			out			out			out			out			in			in			in			in			in			in			
PortF		nc			nc			nc			nc			nc			nc			nc			nc			in			in			in			in			in			in			in			in			
*/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /*串口1 RX管脚配置*/
  /* Configure USART1 Rx as input floating */                                                                                                                 
  GPIO_InitStructure.GPIO_Pin = USART1_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART1_GPIO, &GPIO_InitStructure);
	GPIO_PinAFConfig(USART1_GPIO,GPIO_PinSource10,GPIO_AF_USART1);
  /*串口2 RX管脚配置*/
  /* Configure USART2 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USART2_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART2_GPIO, &GPIO_InitStructure);  
	GPIO_PinAFConfig(USART2_GPIO,GPIO_PinSource3,GPIO_AF_USART2);
  /*串口3 RX管脚配置*/
  /* Configure USART3 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USART3_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART3_GPIO, &GPIO_InitStructure);  
	GPIO_PinAFConfig(USART3_GPIO,GPIO_PinSource11,GPIO_AF_USART3);
  /*串口4 RX管脚配置*/
  /* Configure UART4 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = UART4_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(UART4_GPIO, &GPIO_InitStructure);  
	GPIO_PinAFConfig(UART4_GPIO,GPIO_PinSource11,GPIO_AF_UART4);
  /*串口5 RX管脚配置*/
  /* Configure UART5 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = UART5_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);  
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);
  /*串口6 RX管脚配置*/
  /* Configure USART6 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USART6_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART6_GPIO, &GPIO_InitStructure);
	GPIO_PinAFConfig(USART6_GPIO,GPIO_PinSource7,GPIO_AF_USART6);
  /*串口1 TX管脚配置*/ 
  /* Configure USART1 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USART1_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART1_GPIO, &GPIO_InitStructure);
	GPIO_PinAFConfig(USART1_GPIO,GPIO_PinSource9,GPIO_AF_USART1);
  /*串口2 TX管脚配置*/ 
  /* Configure USART2 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USART2_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART2_GPIO, &GPIO_InitStructure);  
	GPIO_PinAFConfig(USART2_GPIO,GPIO_PinSource2,GPIO_AF_USART2);
  /*串口3 TX管脚配置*/ 
  /* Configure USART3 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USART3_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART3_GPIO, &GPIO_InitStructure);  
	GPIO_PinAFConfig(USART3_GPIO,GPIO_PinSource10,GPIO_AF_USART3);
  /*串口4 TX管脚配置*/ 
  /* Configure UART4 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = UART4_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(UART4_GPIO, &GPIO_InitStructure);  
	GPIO_PinAFConfig(UART4_GPIO,GPIO_PinSource10,GPIO_AF_UART4);
  /*串口5 TX管脚配置*/ 
  /* Configure UART5 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = UART5_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(UART5_GPIO, &GPIO_InitStructure);  
	GPIO_PinAFConfig(UART5_GPIO,GPIO_PinSource12,GPIO_AF_UART5);
  /*串口6 TX管脚配置*/ 
  /* Configure USART6 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USART6_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART6_GPIO, &GPIO_InitStructure);
	GPIO_PinAFConfig(USART6_GPIO,GPIO_PinSource6,GPIO_AF_USART6);
	/*SPI1 管脚配置*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	// SCK
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	// MISO
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	// MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	/*SPI2 管脚配置*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	// SCK
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	// MISO
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	// MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
	/*SPI3 管脚配置*/ 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	// SCK
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);	
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	// MISO
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);	
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	// MOSI
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);	

	/*FSMC 数据管脚配置*/ 
	GPIO_InitStructure.GPIO_Pin =FMC_CS_GPIO_PIN|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
	GPIO_Init(FMC_CS_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_CS_GPIO_PORT,FMC_CS_PINSOURCE,GPIO_AF_FSMC);	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);	
	
	GPIO_InitStructure.GPIO_Pin =FSMC_D0_PIN|FSMC_D1_PIN|FSMC_D2_PIN|FSMC_D3_PIN|FSMC_D13_PIN|FSMC_D14_PIN|FSMC_D15_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,FSMC_D0_PINSOURCE,GPIO_AF_FSMC);	
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D1_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,FSMC_D1_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D2_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,FSMC_D2_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D3_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,FSMC_D3_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D13_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,FSMC_D13_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D14_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,FSMC_D14_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D15_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,FSMC_D15_PINSOURCE,GPIO_AF_FSMC);

//	GPIO_InitStructure.GPIO_Pin = FSMC_D15_PIN;	  //T485B2,SPI1_CS                      
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOD, &GPIO_InitStructure); 	
	
	
	GPIO_InitStructure.GPIO_Pin =FSMC_D4_PIN|FSMC_D5_PIN|FSMC_D6_PIN|FSMC_D7_PIN|FSMC_D8_PIN|FSMC_D9_PIN|FSMC_D10_PIN|FSMC_D11_PIN|FSMC_D12_PIN;// 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D4_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D5_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D5_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D6_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D6_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D7_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D7_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D8_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D8_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D9_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D9_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D10_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D10_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D11_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D11_PINSOURCE,GPIO_AF_FSMC);
	
//	GPIO_InitStructure.GPIO_Pin =FSMC_D12_PIN;// 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE,FSMC_D12_PINSOURCE,GPIO_AF_FSMC);
	

	
  /******输入口配置************************************/
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;// VBAT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;       //模拟输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //不上拉也不下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1;// I0-10,VG
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;       //模拟输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //不上拉也不下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;// UA1,UB1,UC1,UA2,UB2,UC2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;       //模拟输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //不上拉也不下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	


	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9;//ATT_SDO,IN5,IN4,IN3_WCN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_9;//IN7,IN8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;         //AD7606_BUSY引脚，外部中断下降沿触发
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     //IN2_HW,IN1FW,DYXH2,DYXH1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_8|GPIO_Pin_12|GPIO_Pin_13;//LED_Judge,IN6,ATT7022_SEL,ATT7022_SIG
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

  /******捕获输入************************************/
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8; // TIM1_CH1捕获PWM波,UAC1_0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;   //无上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11; // TIM1_CH4捕获PWM波,UAC1_1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;   //无上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_TIM1);
	
	/******输出口配置************************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4;	  //T485B2,SPI1_CS1                      
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_8|GPIO_Pin_12;	  //KZSC1161Y,AT_SDI,DJZZKZ1,SPI2_CS                        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	 //CLOCK_RST                          
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_11;//|GPIO_Pin_12;//HTCLK_OUT ,AD7606_RESET //AD7606_CONVST                       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12; // TIM4_CH2输出PWM波
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//配置为复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推免输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;//DCCDKZ,CLOCK_SCLK             
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	//PWRKEY,GPRS模块电源,SPI1_CS2,BATT,OUT4,OUT5,OUT6,F6                          
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15;	                         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //HZIN,FZIN,CNIN,KZBH,KZWIFI,KZGPS,DJFZKZ1,HZLED1,FZLED1,AT_SCLK,AT_CS
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	

	
  /******数据双向口***********************************/
	/*温湿度传感器数据接口*/ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;       //CLOCK_IO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

/******************************************************************************* 
* Function Name  : DMA_Configuration 
* Description    : Configures the DMA. 
* Input          : None 
* Output         : None 
* Return         : None 
*******************************************************************************/  
void DMA_Configuration(void)  
{  
	DMA_InitTypeDef DMA_InitStructure;  
  /*****************发送部分设置*****************/
  /* DMA2_Stream7 (triggered by USART1 Tx event) Config  
  每个通道都有固定的外设请求，因此在需要传输外设数据时需要选择对应的通道*/  
		
  DMA_DeInit(DMA2_Stream7);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR)); //定义了外设地址   
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMATXbuffer1; //定义了目的地址   
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;    //外设作为数据的目的地   
  DMA_InitStructure.DMA_BufferSize = 300;     //传送数据缓存大小   
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //定义地址在传送后是否会递增   
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存到内存后地址递增   
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//数据宽度是8位   
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;   //工作在正常缓存模式，还有一种是循环缓存模式   
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //通道拥有高优先级   
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;// 单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;// 单次传输
  DMA_Init(DMA2_Stream7, &DMA_InitStructure);  
    
  /* DMA1_Stream6 (triggered by USART2 Tx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA1_Stream6); 
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;	
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART2->DR)); 
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMATXbuffer2;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
	DMA_InitStructure.DMA_BufferSize = 820;     //传送数据缓存大小  
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; //通道拥有高优先级
  DMA_Init(DMA1_Stream6, &DMA_InitStructure);  

  /* DMA1_Stream3 (triggered by USART3 Tx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA1_Stream3);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART3->DR)); 
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMATXbuffer3;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
	DMA_InitStructure.DMA_BufferSize = 100;     //传送数据缓存大小  
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //通道拥有高优先级
  DMA_Init(DMA1_Stream3, &DMA_InitStructure);  

  /* DMA1_Stream4 (triggered by USART4 Tx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA1_Stream4);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(UART4->DR)); 
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMATXbuffer4;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; 
  DMA_InitStructure.DMA_BufferSize = 300;     //传送数据缓存大小  	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //通道拥有高优先级
  DMA_Init(DMA1_Stream4, &DMA_InitStructure);  

  /* DMA1_Stream7 (triggered by USART5 Tx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA1_Stream7);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(UART5->DR)); 
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMATXbuffer5;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
	DMA_InitStructure.DMA_BufferSize = 420;     //传送数据缓存大小
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //通道拥有高优先级
  DMA_Init(DMA1_Stream7, &DMA_InitStructure);
	
  /* DMA2_Stream6 (triggered by USART6 Tx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA2_Stream6);
  DMA_InitStructure.DMA_Channel = DMA_Channel_5;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART6->DR));
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMATXbuffer6;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
	DMA_InitStructure.DMA_BufferSize = 100;     //传送数据缓存大小  	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low; //通道拥有高优先级
  DMA_Init(DMA2_Stream6, &DMA_InitStructure);	
  /*****************接收部分设置*****************/
  /* DMA2_Stream5 (triggered by USART1 Rx event) Config  
  每个通道都有固定的外设请求，因此在需要传输外设数据时需要选择对应的通道*/  
  DMA_DeInit(DMA2_Stream5);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR)); //定义了外设地址   
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMARXbuffer1; //定义了目的地址   
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;    //内存作为数据的目的地   
  DMA_InitStructure.DMA_BufferSize = 50;     //传送数据缓存大小   
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //定义地址在传送后是否会递增   
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存到内存后地址递增   
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//数据宽度是8位   
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   //工作在循环缓存模式，还有一种是正常缓存模式   
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //通道拥有高优先级   
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;// 单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;// 单次传输	
  DMA_Init(DMA2_Stream5, &DMA_InitStructure); 

  /* DMA1_Stream5 (triggered by USART2 Rx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA1_Stream5);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART2->DR));  
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMARXbuffer2;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  
	DMA_InitStructure.DMA_BufferSize = 50;     //传送数据缓存大小   
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; //通道拥有高优先级
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);  

  /* DMA1_Stream1 (triggered by USART3 Rx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA1_Stream1);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4; 	
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART3->DR));
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMARXbuffer3;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  
	DMA_InitStructure.DMA_BufferSize = 50;     //传送数据缓存大小   
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //通道拥有高优先级
  DMA_Init(DMA1_Stream1, &DMA_InitStructure);  

  /* DMA1_Stream2 (triggered by USART4 Rx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA1_Stream2);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(UART4->DR));
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMARXbuffer4;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  
	DMA_InitStructure.DMA_BufferSize = 50;     //传送数据缓存大小   
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //通道拥有高优先级
  DMA_Init(DMA1_Stream2, &DMA_InitStructure);  

  /* DMA1_Stream0 (triggered by USART5 Rx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA1_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(UART5->DR)); 	
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMARXbuffer5;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  
	DMA_InitStructure.DMA_BufferSize = 50;     //传送数据缓存大小   
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; //通道拥有高优先级
  DMA_Init(DMA1_Stream0, &DMA_InitStructure);  

  /* DMA2_Stream1 (triggered by USART6 Rx event) Config  
  工作模式只需要把不一样的部分设置一下就可以，其他的还和上面一样*/  
  DMA_DeInit(DMA2_Stream1);
  DMA_InitStructure.DMA_Channel = DMA_Channel_5;   
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART6->DR)); 
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMARXbuffer6;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  
	DMA_InitStructure.DMA_BufferSize = 50;     //传送数据缓存大小   
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low; //通道拥有高优先级
  DMA_Init(DMA2_Stream1, &DMA_InitStructure); 
	
	/*ADC采样通道*/
	DMA_DeInit(DMA2_Stream4); 
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC1->DR));//((u32)(ADC1+0x4c))
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMAADCbuffer1;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 900;                              
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;// 单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;// 单次传输	
  DMA_Init(DMA2_Stream4, &DMA_InitStructure);
	/* Enable ADC1 DMA */ 
//	DMA_ClearFlag(DMA2_Stream4,DMA_FLAG_TCIF4);
	DMA_Cmd(DMA2_Stream4, ENABLE);

	
  /*********************使能相应的DMA通道**************************/  

  /*****************发送部分设置*****************/
  /* Enable TX1 DMA */  
  DMA_Cmd(DMA2_Stream7, ENABLE);  
  /* Enable TX2 DMA */  
  DMA_Cmd(DMA1_Stream6, ENABLE); 
  /* Enable TX3 DMA */  
  DMA_Cmd(DMA1_Stream3, ENABLE);
  /* Enable TX4 DMA */  
  DMA_Cmd(DMA1_Stream4, ENABLE);
  /* Enable TX5 DMA */  
  DMA_Cmd(DMA1_Stream7, ENABLE);
  /* Enable TX6 DMA */  
  DMA_Cmd(DMA2_Stream6, ENABLE);
	
  DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA2_Stream6, DMA_IT_TC, ENABLE);	
    
  /*****************接收部分设置*****************/
  /* Enable DMA RX1 */  
  DMA_Cmd(DMA2_Stream5, ENABLE);  
  /* Enable DMA RX2 */  
  DMA_Cmd(DMA1_Stream5, ENABLE); 
  /* Enable DMA RX3 */  
  DMA_Cmd(DMA1_Stream1, ENABLE);
  /* Enable DMA RX4 */  
  DMA_Cmd(DMA1_Stream2, ENABLE);
  /* Enable DMA RX5 */  
  DMA_Cmd(DMA1_Stream0, ENABLE);
  /* Enable DMA RX6 */  
  DMA_Cmd(DMA2_Stream1, ENABLE);		
	
 // DMA_ITConfig(DMA2_Stream4, DMA_IT_TC, ENABLE);
	
  DMA_ITConfig(DMA2_Stream5, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA2_Stream5, DMA_IT_HT, ENABLE);
  DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA1_Stream5, DMA_IT_HT, ENABLE); 
  DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA1_Stream1, DMA_IT_HT, ENABLE); 
  DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA1_Stream2, DMA_IT_HT, ENABLE);
  DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA1_Stream0, DMA_IT_HT, ENABLE); 
  DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE); 
  DMA_ITConfig(DMA2_Stream1, DMA_IT_HT, ENABLE);
}  


void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the UART4 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the UART5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART6 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART1 DMA TX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART1 DMA RX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 DMA TX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream6_IRQn;     
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=4;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 DMA RX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=4;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART3 DMA TX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=5;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART3 DMA RX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=5;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the UART4 DMA TX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=6;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the UART4 DMA RX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=6;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the UART5 DMA TX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the UART5 DMA RX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART6 DMA TX Interrupt */	
  NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=7;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART6 DMA RX Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=7;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the TIM1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=TIM1_CC_IRQn;//TIM1捕获中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=8;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the TIM1 Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_TIM10_IRQn;//TIM1更新中断
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority=8;
//  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the TIM4 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
//	/* Enable the ADC1 Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the EXTI Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream4_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority=8;
//  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

}
void Timer_Configuration(void)
{
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
	TIM_OCInitTypeDef         TIM_OCInitStructure;
	TIM_ICInitTypeDef         TIM_ICInitStructure;
	
	TIM_DeInit(TIM1);//APB2的时钟为84MHz,所以TIM1的是时钟为168MHz
	TIM_TimeBaseStructure.TIM_Period=0xFFFF;		 //ARR的值
	TIM_TimeBaseStructure.TIM_Prescaler=167;     
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //采样分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM1, DISABLE);//禁止ARR预装载缓冲器
	/*PWM 捕获模式配置*/
	/* PWM1 Mode configuration: Channel1 */
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //捕获通道选择
	TIM_ICInitStructure.TIM_ICPolarity =  TIM_ICPolarity_Rising; //上升沿触发
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 设置为TRC输入
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //1分频，即捕获信号的每个有效边沿都捕获
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//滤波
	TIM_ICInit(TIM1,&TIM_ICInitStructure); 
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4; //捕获通道选择
	TIM_ICInitStructure.TIM_ICPolarity =  TIM_ICPolarity_Rising; //上升沿触发
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 设置为TRC输入
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //1分频，即捕获信号的每个有效边沿都捕获
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//滤波
	TIM_ICInit(TIM1,&TIM_ICInitStructure); 
	TIM_ClearFlag(TIM1,TIM_FLAG_CC1|TIM_FLAG_CC4);
	TIM_ITConfig(TIM1,TIM_IT_CC1|TIM_IT_CC4,ENABLE);
  TIM_Cmd(TIM1, ENABLE);	//开启时钟	
	
  TIM_DeInit(TIM2);//APB1的时钟为42MHz,所以TIM2的是时钟为84MHz
	TIM_TimeBaseStructure.TIM_Period=8399;		   //ARR的值
	TIM_TimeBaseStructure.TIM_Prescaler=9;       //定时器时钟为 (8399+1)*(9+1)/84000000=1/1000s=1ms
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //采样分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//TIM_PrescalerConfig(TIM2,0,TIM_PSCReloadMode_Immediate);
	TIM_ARRPreloadConfig(TIM2, DISABLE);//禁止ARR预装载缓冲器
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2, ENABLE);	//开启时钟
	
	TIM_DeInit(TIM4);//APB1的时钟为42MHz,所以TIM4的是时钟为84MHz
	TIM_TimeBaseStructure.TIM_Period=16799;		 //ARR的值
	TIM_TimeBaseStructure.TIM_Prescaler=0;     //定时器时钟为 (16799+1)*(0+1)/84000000=1/5000s=0.2ms=200us
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //采样分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	//TIM_PrescalerConfig(TIM4,1,TIM_PSCReloadMode_Immediate);  //84MHz /(8400*(1+1)) = 5k
	
	/*PWM 模式配置*/
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //使能输出
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性低
	TIM_OCInitStructure.TIM_Pulse = 100; //设置PWM初始脉冲宽度为42，42/8400=0.005*200=1us
	TIM_OC1Init(TIM4,&TIM_OCInitStructure); 
//	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);//允许ARR预装载缓冲器
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4, ENABLE);	//开启时钟	
	//TIM_SelectOutputTrigger(TIM4,TIM_TRGOSource_Update);
}

void SysTick_Configuration(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	/* Setup SysTick Timer for 1 msec interrupts  */
  SysTick_Config(SystemCoreClock/1000); //SysTick配置函数
 /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);//SysTick中断优先级设置
}

