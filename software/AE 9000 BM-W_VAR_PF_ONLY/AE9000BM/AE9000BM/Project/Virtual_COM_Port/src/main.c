/********************************************************************************
  * @file    main.c

******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include "misc.h"
#include "stm32l1xx.h"
#include "stm32l1xx_flash.h"
#include "stm32l1xx_adc.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_rtc.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_pwr.h"
#include "stm32l1xx_syscfg.h"
#include "stm32l1xx_dbgmcu.h"
#include "USART1_Function.h"
#include <stdlib.h>
#include "stm32l1xx_conf.h"
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


#define PUTCHAR_PROTOTYPE int putchar (int c)
#define GETCHAR_PROTOTYPE int getchar (void)

/*******************************************************************************/
  /**
  * @brief main entry point.
  * @par Parameters None
  * @retval void None
  * @par Required preconditions: None
  */
/* ModBus Parameter Variable */
void ucMBInit_condition(void);
extern unsigned char fTxFrame;
extern unsigned char uiTxFrameDelay;
extern unsigned int usRcvBufferPos;
static volatile eMBSndState eSndState;
unsigned long ModBuadRate;
static UCHAR     ucMBAddress;
unsigned char  mod_id;
extern unsigned int device_id;
/*------------global Variables----------------*/
extern unsigned int uiInitPage[8];
extern unsigned int uiParamPage[9][8];
extern unsigned char curr_page,Run_flag,PCRxFlag;
unsigned char freq_comp;
/*-------------structure---------------*/
CONFIGURATION_PARAM reqConfiguration;
MEASURED_PARAM  reqMeasurepara;
UnCal reqUncal;
unsigned int value=0;
extern  unsigned char ucPage;
extern void ConF_FUN(void);
extern float Sum_Watt_EM;
void Display_Modbus_init(void);
unsigned char key=0;
extern float KWHf;
extern unsigned char memory_location_flag;
/**************MODBUS*****************/

float VR_MB,VY_MB,VB_MB,VLNavg_MB;
float VRY_MB,VYB_MB,VBR_MB,VLLavg_MB;
float IR_MB,IY_MB,IB_MB,Iavg_MB;
float PFR_MB,PFY_MB,PFB_MB,PFavg_MB,Frq_MB;
float KWr_MB,KWy_MB,KWb_MB,WTot_MB;
float VARr_MB,VARy_MB,VARb_MB,VARTot_MB;
float KVAr_MB,KVAy_MB,KVAb_MB,KVATot_MB;
float KWHf_MB;
float **ptr_par;
unsigned char *ptr_Trans, *ptr_rec;
unsigned char modb_RxCntr=0,modb_TxCntr=0;

/************************************/
/***************************************************************************************************************************
Function     :  main
Parameter    :  null
Description  :
***************************************************************************************************************************/
void main(void)
{
  float *arr_MODBUS[31];
  unsigned char Usart_buf[10];
  unsigned char arr_MbSend[118];
  ptr_Trans = arr_MbSend;            // pointer for response 
  ptr_rec = Usart_buf;                // pointer for query
  
 
  
  
  arr_MODBUS[0]=&VR_MB;
  arr_MODBUS[1]=&VY_MB;
  arr_MODBUS[2]=&VB_MB;
  arr_MODBUS[3]=&VLNavg_MB;
  
  arr_MODBUS[4]=&VRY_MB;
  arr_MODBUS[5]=&VYB_MB;
  arr_MODBUS[6]=&VBR_MB;
  arr_MODBUS[7]=&VLLavg_MB;
  
  arr_MODBUS[8]=&IR_MB;
  arr_MODBUS[9]=&IY_MB;
  arr_MODBUS[10]=&IB_MB;
  arr_MODBUS[11]=&Iavg_MB;
  
  arr_MODBUS[12]=&PFR_MB;                                                        
  arr_MODBUS[13]=&PFY_MB;
  arr_MODBUS[14]=&PFB_MB;
  arr_MODBUS[15]=&PFavg_MB;
  arr_MODBUS[16]=&Frq_MB;
  
  arr_MODBUS[17]=&KWr_MB;
  arr_MODBUS[18]=&KWy_MB;
  arr_MODBUS[19]=&KWb_MB;
  arr_MODBUS[20]=&WTot_MB;
  
  arr_MODBUS[21]=&VARr_MB;
  arr_MODBUS[22]=&VARy_MB;
  arr_MODBUS[23]=&VARb_MB;
  arr_MODBUS[24]=&VARTot_MB;
  
  arr_MODBUS[25]=&KVAr_MB;
  arr_MODBUS[26]=&KVAy_MB;
  arr_MODBUS[27]=&KVAb_MB;
  arr_MODBUS[28]=&KVATot_MB;
  arr_MODBUS[29]=&KWHf_MB;
  
  arr_MODBUS[0]=&WTot_MB;
  arr_MODBUS[1]=&VARTot_MB;
  arr_MODBUS[2]=&PFavg_MB;
  
  ptr_par=arr_MODBUS;
    
  STM32L_Peripheral_Configuration();                                              /* System & Peripherals Clock Configuration */
 // GPIO_ResetBits(GPIOA,GPIO_Pin_1);
  Display_Modbus_init();  
  TIM11_Configuration();
  while (1)
  {
     RUN_FUN();
     MB_PARA();
    if(Run_flag==1)            /*--------------Display Current Page-----------------------*/
    {
      CreatePagePattParamDisp(curr_page);                               //For display all parameter
      MountDisplay(uiParamPage[(curr_page-1)], 0);
    }
    if(PCRxFlag==1)                                                       // Calibration of meter through uart
    {
      PCRxFlag =0;
      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

      Meter_Calib();
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    }
    else if(PCRxFlag==2)                                                  // Display parameters on uart
    {
      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
      PCRxFlag =0;
      Disp_FUN();
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    }
    else if(PCRxFlag==4)                                                  // Meter Configure from Uart
    {
      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
      SendChar('Y');                                                      // send character Y for acknowlege
      PCRxFlag =0;
      ConF_FUN();
      READ_CONFG();
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    }
//    eMBPoll();                                                            // ModBus Polling
//    if((fTxFrame == 1))                                                   // && uiTxFrameDelay == 6 )
//    {
//      fTxFrame = 0;
//      uiTxFrameDelay=0;
//      GPIO_SetBits(GPIOA, GPIO_Pin_1);
//      vMBPortSerialEnable(FALSE,TRUE);
//    }
  }
}
/***************************************************************************************************************************
Function     :  Display_Modbus_init
Parameter    :  null
Description  :
***************************************************************************************************************************/

void Display_Modbus_init(void)
{
    unsigned char count=18;
    CreatePage(INIT_DISP, 0, AE_patts, Dev_patts, BM_patts, All_GLOW);          // disply frist page at intial u-Met 9900
    MountDisplay(uiInitPage, 0);                                                // Display initial page on seven segment
    while(count)
    {
        delay(65535);
        count--;
    }

    READ_CONFG();                                                               // Read configured parameters  from EEprom
    READ_EEPROM();                                                              // Read calibration parameters  from EEprom
    Factor_Update();                                                            // calculation of Factor using CTR PTR ratio

    TIM_Cmd(TIM3, DISABLE);
    if( reqConfiguration.uiScrollON==1)
    {
      value=5000*(reqConfiguration.uiScrollTime);
      TIM3_Configuration(value);
    }
    curr_page=1;                                                                  // Display Page 1 initialy
    CreatePagePattParamDisp(curr_page);                                           // For display all parameter
    MountDisplay(uiParamPage[(curr_page-1)], 0);
    mod_id=reqConfiguration.uiNetworkID;
    ucMBAddress=mod_id;
    device_id=mod_id; //edited by abhijit mali on 25/12/2014
    xMBPortSerialInit(1,ModBuadRate,8, MB_PAR_EVEN );                             // serial port initialisation i.e. usart
   // ucMBInit_condition();                                                         // modbus init in RTU mode
   // vMBPortSerialEnable(TRUE, FALSE );                                            // Sencond time Initialisation of USART2 as receiver //enable port timer
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
   
}

/***************************************************************************************************************************
Function     :  delay
Parameter    :  stre
Description  :
***************************************************************************************************************************/

void delay(unsigned int stre)
{
    unsigned int n;
    for(n=stre;n!=0;n--);
    for(n=stre;n!=0;n--);
    for(n=stre;n!=0;n--);
    for(n=stre;n!=0;n--);
    for(n=stre;n!=0;n--);
    for(n=stre;n!=0;n--);
    for(n=stre;n!=0;n--);
    for(n=stre;n!=0;n--);
}
/***************************************************************************************************************************
Function     :  Delay
Parameter    :  stre
Description  :
***************************************************************************************************************************/
void Delay(__IO uint32_t nCount)
{
  for(; nCount!= 0;nCount--);
}


/******************************************************************************
  * @brief  Initialize all the keys (implemented, enabled and DxS group)
  * @param  None
  * @retval None
  * @brief Example of LED switching using touch sensing keys
  * @param None
  * @retval None
******************************************************************************/

PUTCHAR_PROTOTYPE
{

  USART1->DR=c;

  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE)== RESET);
     USART_ClearFlag(USART1,USART_FLAG_TXE);
  return (c);
}

GETCHAR_PROTOTYPE
{
    int c = 0;
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
    USART_ClearFlag(USART1,USART_FLAG_RXNE);
    c =USART1->DR;
    return (c);
 }
#ifdef  USE_FULL_ASSERT
/*******************************************************************************
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
********************************************************************************/

void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/******************* (C) COPYRIGHT 2011 Automatic Electric Ltd  *****END OF FILE****/