/* Includes ------------------------------------------------------------------*/
#include "misc.h"
#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_conf.h"
#include "stm32l1xx_usart.h"
#include "System_Configuration.h"
#include "struct.h"
#include "MyFun.h"

#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbcrc.h"
#include "mbport.h"
#include "mbport.h"
#include "mbrtu.h"


//*****************************Peripheral's Structure Initialisation **************************************//

#define ADC1_DR_ADDRESS    ((uint32_t)0x40012458)
#define BUFFER_ADDRESS     ((uint32_t)(Buffer))

#define BUFF_SIZE 5200

GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
SPI_InitTypeDef SPI_InitStructure;
EXTI_InitTypeDef   EXTI_InitStructure;
TIM_ICInitTypeDef TIM_ICInitStructure;
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
extern void Delay(__IO uint32_t);
void TIM11_Configuration(void);
extern void Init_keypad (void);
uint16_t Buffer[BUFF_SIZE];

/***************************************************************************************************************************
Function     :  STM32L_Peripheral_Configuration
Parameter    :  null
Description  :  This function is used to initialise peripherals with some specific configurations.
***************************************************************************************************************************/

void STM32L_Peripheral_Configuration(void)
{
    RCC_HSICmd(ENABLE);                                                   // Enable the HSI
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);                   // Wait until HSI oscillator is ready
    Init_GPIOs();                                                        // Init I/O ports */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                       // NVIC priority assign
    TIM4_Configuration();                                                 // Timer 4 for Display and key ...........2.5ms */
    TIM2_Configuration(312);                                              // Timer For ADC triggering*/
    DMA_Configuration();                                                  // DMA for sample collection*/
    ADC_Configuration();                                                  // ADC Configuration */
    SPI1_Configuration();                                                 // SPI for TLC5920*/
    USART1_Configuration();                                               // USART1 Peripheral Configuration */
    EXTI_Fre_Config();                                                    // External interrupt for freq measurement*/
    TIM9_Configuration();                                                 // Timer for frequency measurement
    TIM6_Configuration();
     SPI2_Configuration();
    // xMBPortTimersInit(1);                                                 // Modbus configuration timer 10 initialisation for port t3.5char timer
  //  vMBPortTimersEnable();
    Init_keypad();
}

/***************************************************************************************************************************
Function     :  TIM6_Configuration
Parameter    :  null
Description  :  Timer 6 is used for modbus communication
***************************************************************************************************************************/
void TIM6_Configuration()
{
  TIM_DeInit(TIM6);                                                           // Enable TIM2 clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period =40990;                                    // This value  will be loaded in Auto reload register
  TIM_TimeBaseStructure.TIM_Prescaler =1;                                     // Prescalar is 1 . 16000000/2= 8000000 , F=1/T , T=1/8000000, T=0.000000125 125nsec timer generated
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;                              //
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 // Timer in up counting mode is used
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  //TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);                       // TIM2 TRGO selection
  
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);                                  // Enable TIM3 Update Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  TIM_Cmd(TIM6,DISABLE);                                                      // Enable TIM2
}
/***************************************************************************************************************************
Function     :  Init_GPIOs
Parameter    :  null
Description  :  This function is used to initialise GPIO ports with specific configurations.
***************************************************************************************************************************/

void  Init_GPIOs (void)
{

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);                         // Enable A, B, C, D GPIO clocks
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /********************* ADC GPIO pins Enable ********************************/
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /********************* For TLC5920 GPIO pins config ********************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15 ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;             // GPIO_Pin_5 6 and 7 configured as alternet function
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);                                      // SPI SCK pin configuration
                                                                              // Configure the GPIO_A, SPI1 pins for ADE7878
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);                     //  Connect PXx to SPIx_CLK
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);                     // Connect PXx to SPIx_MISO
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);                     // Connect PXx to SPIx_MOSI

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);                  // Connect PXx to USARTx_Tx
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);                  // Connect PXx to USARTx_Rx

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;                                   // Configure USART Tx as alternate function push-pull
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                                  // Configure USART Rx as alternate function push-pull
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                                   // ModBus IC AD2587 (RS485) DE Pin to Enable Transmitter
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
////////   /* Configure USART Tx as alternate function push-pull */
////////    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART3);
////////    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
////////    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
////////    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
////////    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
////////    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
////////    GPIO_Init(GPIOA, &GPIO_InitStructure);
////////    
////////
////////    /* Configure USART Rx as alternate function push-pull */
////////   // GPIO_Mode_OUT  = 0x01,
////////    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART3);
////////    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
////////    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
////////    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
////////    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
////////    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
////////    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/***************************************************************************************************************************
Function     :  USART1_Configuration
Parameter    :  null
Description  :  Uart1 is configured in this function
***************************************************************************************************************************/
/*******************************
 USARTx configured as follow:
   - BaudRate = 115200 baud
   - Word Length = 8 Bits
   - One Stop Bit
   - None parity
   - Hardware flow control disabled (RTS and CTS signals)
   - Receive and transmit enabled
**********************************/
void USART1_Configuration(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);                      // Enable UART clock
  USART_InitStructure.USART_BaudRate =115200;                                 // 115.200kbps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                 // 8 bit data is is tranmitted and received
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                      // 1 stop bit is used
  USART_InitStructure.USART_Parity = USART_Parity_No;                         // no parity bit is used
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;             // uart RX and TX enabled

  USART_Init(USART1, &USART_InitStructure);                                   // USART configuration
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                              // Enable Receive Interrupt
  USART_Cmd(USART1, ENABLE);                                                  // Enable USART
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                           // Enable the USART1 gloabal Interrupt
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}
/***************************************************************************************************************************
Function     :  ADC_Configuration
Parameter    :  null
Description  :  This function is used to configure ADC channels for input.
***************************************************************************************************************************/
void ADC_Configuration(void)
{
  ADC_DeInit(ADC1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);                        //  ADC peripheral clock enable
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                      //  12 bit ADC resolution
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;                                //  Scan coversion mode enabled
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                         //  continuous conversion is disabled
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;// ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;      //  ADC_ExternalTrigConv_T2_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion =8;                                   //  Including 2.5V reference voltage    //1;//
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14 ,1, ADC_SampleTime_24Cycles);  //  ADC_SampleTime_9Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_15 ,2, ADC_SampleTime_24Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8  ,3, ADC_SampleTime_24Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9  ,4, ADC_SampleTime_24Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10 ,5, ADC_SampleTime_24Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11 ,6, ADC_SampleTime_24Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12 ,7, ADC_SampleTime_24Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13 ,8, ADC_SampleTime_24Cycles);

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);                     // ADC_DelayLength_7Cycles);//ADC_DelayLength_None);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);                           // Enable the request after last transfer for DMA Circular mode
  ADC_DMACmd(ADC1, ENABLE);                                                   // Enable ADC1 DMA
  ADC_Cmd(ADC1, ENABLE);                                                      // Enable ADC1
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);                   // Wait until ADC1 ON status
}
/***************************************************************************************************************************
Function     :  DMA_Configuration
Parameter    :  null
Description  :  DMA channel 11 is is configured to collect data from ADC data registor and transfer it to  memory
***************************************************************************************************************************/
void DMA_Configuration(void)
{
  DMA_DeInit(DMA1_Channel1);                                                  // Enable DMA1 clock
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                          // DMA1 channel1 configuration
  DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)(&ADC1->DR));         // ADC1_DR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = BUFFER_ADDRESS;                      // Address of Buffer
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          // Peripheral register is used as sourse for data copying
  DMA_InitStructure.DMA_BufferSize = 5200;                                    // Buffer 5200 bytes
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            // DMA_PeripheralInc_Enable;//
  DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable;                      // DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // Peipheral register data size is halfword
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         // Memory data size is halfword(8 bit)
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             // DMA_Mode_Normal;//
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         // DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                // DMA_M2M_Enable;//
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                             // Enable DMA1 Channel1 Transfer Complete interrupt
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;                    // Enable DMA1 channel1 IRQ Channel
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_Cmd(DMA1_Channel1, DISABLE);                                            // Enable DMA1 channel1
}
/***************************************************************************************************************************
Function     :  TIM2_Configuration
Parameter    :  null
Description  :  Timer 2 is configured as trigger input to ADC for ADC conversion
***************************************************************************************************************************/
void TIM2_Configuration(unsigned int upVal)
{
  TIM_DeInit(TIM2);                                                           // Enable TIM2 clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period =upVal;                                    // This value will will be loaded in Auto reload register
  TIM_TimeBaseStructure.TIM_Prescaler =1;                                     // Prescalar is 1 . 16000000/1= 1000000 , F=1/T , T=1/1000000, T=0.000001 1usec timer generated
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;                              //
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 // Timer in up counting mode is used
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);                       // TIM2 TRGO selection
  TIM_Cmd(TIM2,DISABLE);                                                      // Enable TIM2
}
/***************************************************************************************************************************
Function     :  SPI1_Configuration
Parameter    :  null
Description  : SPI for TLC 5920
***************************************************************************************************************************/
 /* SPI1 configured as follow:
        - SCLK Freq. (BaudRate) = 2MHz                                          250 KHz
        - Word Length = 8 Bits
        - SPI_CPOL = SPI_CPOL_High
        - SPI_CPHA = SPI_CPHA_2Edge
        - MSB FirstBit
        - CRCPolynomial = 7
        - SPI_Mode_Master
        - Direction = FullDuplex
*/
void SPI1_Configuration(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);                        // Enable the SPI peripheral */
  SPI_I2S_DeInit(SPI1);                                                       // Deinitialise SPI
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;          // Full duplex communication used
  SPI_InitStructure.SPI_DataSize  = SPI_DataSize_16b;                         // Data transfer size is 16 bit
  SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;                            // SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;                           // SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;                             // SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;          // SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                         // SPI first bit for tranmit is MSB
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;                          // SPI in master mode is used
  SPI_Init(SPI1, &SPI_InitStructure);                                         // Initializes the SPI communication
  SPI_Cmd(SPI1, ENABLE);
}
/***************************************************************************************************************************
Function     :  TIM4_Configuration
Parameter    :  null
Description  : For Dislay timer 4 is configured
***************************************************************************************************************************/
void TIM4_Configuration(void)
{
  TIM_DeInit(TIM4);                                                           // Deinitialise TIM4
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                        // Enable Tim4 Peripheral clock
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period =249;                                      // 0.000 010 * 250 = 2.5msec.  interrupt will be generated after   2.5msec
  TIM_TimeBaseStructure.TIM_Prescaler =159;                                   // 16000000/160= 100000 KHz , T=1/100000 , T= 0.000 010 = 10 us
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 // Timer in upcounting mode is used
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);                                  // Enable TIM4 Update Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_Cmd(TIM4,ENABLE);                                                       // Enable TIM4
}

///***************************************************************************************************************************
//Function     :  EXTI_Configuration
//Parameter    :  null
//Description  : External interrupt  for Keys Configuration
//***************************************************************************************************************************/
//void EXTI_Configuration(void)
//{
//
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//  
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;                      // EXTI_Trigger_Rising
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//
//  /* Enable and set EXTI15_10 Interrupt to the lowest priority */
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//}
///***************************************************************************************************************************
//Function     :  EXTI_Configuration
//Parameter    :  null
//Description  : External interrupt  for Keys Configuration
//***************************************************************************************************************************/
//
//void EXTI1_Configuration(void)
//{
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//  
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource7);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line7;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//
//  /* Enable and set EXTI15_10 Interrupt to the lowest priority */
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//}

/***************************************************************************************************************************
Function     :  Timer3
Parameter    :  null
Description  : Timer3  for scroll time
***************************************************************************************************************************/
void TIM3_Configuration(unsigned int TimPeriod)
{

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = TimPeriod;                               // update event = 200 * Time period
  TIM_TimeBaseStructure.TIM_Prescaler = 3199;                                 // 16000000/3200 = 5000khz, T =1/5000= 200usec
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                                  // TIM IT enable */

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                             // Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             // ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_Cmd(TIM3, ENABLE);                                                     // TIM enable counter
}
/***************************************************************************************************************************
Function     :  EXTI_Fre_Config
Parameter    :  null
Description  : external interrupt for frequency measurement
***************************************************************************************************************************/
void  EXTI_Fre_Config()
{

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  

//////  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);
//////  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
//////  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//////  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;
//////  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//////  EXTI_Init(&EXTI_InitStructure);
//////
//////  /* Enable and set EXTI15_10 Interrupt to the lowest priority */
//////  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//////  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//2
//////  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//1
//////  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
//////  NVIC_Init(&NVIC_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/***************************************************************************************************************************
Function     :  TIM9_Configuration
Parameter    :  null
Description  : For frequency measurement
***************************************************************************************************************************/
void TIM9_Configuration(void)
{
  TIM_DeInit(TIM9);                                                           // Enable TIM9 clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 65535;                                   // Timer overflow =65536 * 0.000001
  TIM_TimeBaseStructure.TIM_Prescaler =7;                                    // 16000000/16= 1000000 , T= 1/1000000= 0.000001
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 // Timer in upcounting mode is used
  TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
}
/***************************************************************************************************************************
Function     :  TIM11_Configuration
Parameter    :  null
Description  :
***************************************************************************************************************************/


void TIM11_Configuration(void)
{
  /********************************************************************************/
    /* Enable TIM9 clock */
  TIM_DeInit(TIM11);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period =1000;// 65530;//0x189C;//0x276;
  TIM_TimeBaseStructure.TIM_Prescaler =16000;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);
  
    TIM_ITConfig(TIM11, TIM_IT_Update, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM11_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* Enable TIM2 */
  TIM_Cmd(TIM11,ENABLE);
}


void  SPI2_Configuration()
{
     /* Connect PXx to SPIx_CLK */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);

  /* Connect PXx to SPIx_MISO*/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);

  /* Connect PXx to SPIx_MOSI */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

   SPI_InitTypeDef SPI_Config;
  GPIO_InitTypeDef   GPIO_InitStructure;
      
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
  SPI_Config.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_Config.SPI_Mode = SPI_Mode_Master;
  SPI_Config.SPI_DataSize = SPI_DataSize_8b;
  SPI_Config.SPI_CPOL = SPI_CPOL_Low;
  SPI_Config.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_Config.SPI_NSS = SPI_NSS_Hard;
  SPI_Config.SPI_BaudRatePrescaler =SPI_BaudRatePrescaler_2;
  SPI_Config.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Config.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2,&SPI_Config);
  
  SPI_SSOutputCmd(SPI2, ENABLE);
  
  SPI_Cmd(SPI2 , ENABLE);
  
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
}

/****************************************** End of File ***************************************/

