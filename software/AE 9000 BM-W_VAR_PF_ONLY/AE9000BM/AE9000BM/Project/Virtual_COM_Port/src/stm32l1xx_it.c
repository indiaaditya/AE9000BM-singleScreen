/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Template/stm32l1xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    Apri-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_it.h"
#include "stm32l1xx_exti.h"
#include "USART1_Function.h"
#include "stm32l1xx_conf.h"
#include "System_Configuration.h"
#include "struct.h"
#include "MyFun.h"
#include "EXT_MEM.h"


#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbcrc.h"
#include "mbport.h"
#include "mbport.h"
#include "mbrtu.h"




BOOL            bTXEnabled;
BOOL            bRXEnabled;
extern unsigned char uiTxFrameDelay;




extern NVIC_InitTypeDef NVIC_InitStructure;
extern unsigned int BlinkMask[];
extern unsigned int FinalDisp[];
extern unsigned char ucSingleLED[],freq_comp;
 extern EXTI_InitTypeDef EXTI_InitStructure;
extern unsigned char curr_page,Run_flag;
extern unsigned int uiInitPage[8];
extern unsigned int uiParamPage[9][8];
extern unsigned char EndOfTransfer;
extern CONFIGURATION_PARAM reqConfiguration;
extern MEASURED_PARAM  reqMeasurepara;
extern UnCal reqUncal;
extern unsigned long int No_ip;
extern float Energy_watt_tot,Energy_var_tot;

volatile unsigned char ucKeyPressedd0=0;
volatile unsigned char ucKeyPressedd1=0;

volatile unsigned char ucKeyPressed0 = 0;
volatile unsigned char ucKeyPressed1 = 0;
volatile unsigned char ucKeyPressFlag0;
volatile unsigned char ucKeyPressFlag01;
volatile unsigned char ucKeyPressFlag1;
volatile unsigned char ucKeyPressFlag11;
volatile unsigned char ucKBIFlag0 =1;
volatile unsigned char ucKBIFlag1 =1;
unsigned char reverse_flag;


unsigned char MyCnt=0,Freq_count;
unsigned char PCRxFlag=0;
extern unsigned char freq_comp;
volatile unsigned int  MYFREQ=0,count=0;
float Frequ,AVG_FREQ,Fin_Freq;
unsigned char Tim_flag,eeprom_flag;
unsigned int Sec,w;
float KWH,Sum_Watt,Sum_Var,KVAR,KVARf, KWHf,Energy_var_tot,Energy_watt_tot;
unsigned long write_cycle;
unsigned char memory_location_flag;
struct key
{ 	
   unsigned char msb:1;
   unsigned char key_reco:2;		
   unsigned char ScanCodetemp;
   unsigned char ScanCode;
   unsigned char RetCodetemp;
   unsigned char RetCode;
}key_code;

unsigned char Keypad_dbnce=0;
unsigned char keys[2]={1,2};
#define KEY_NOT_PRESSED 0	


#define ROW0 (1 << 6)
#define ROW1 (1 << 7)

#define ROW_MASK (ROW0 | ROW1)
#define KEYPAD_ROW_SCAN GPIOC->IDR


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
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

}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */

}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */

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
void SVC_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
 /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */




void RTC_WKUP_IRQHandler (void)
{

}
/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_md.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

/***************************************************************************************************************************
Function     :  USART1_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/
unsigned char kd=0;

void USART1_IRQHandler(void)
{
 if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
     //    kd =USART1->DR;
     if(USART1->DR=='C')    //Configuration
     {
      PCRxFlag=4;
     }

    else if(USART1->DR=='c')   //calibration
     {
      PCRxFlag=1;
     }

     else if(USART1->DR=='R')   //Read all data
     {
        PCRxFlag=2;

     }
     else if(USART1->DR=='S')   //Stop Data sending
     {
      PCRxFlag=3;
     }
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
}

/***************************************************************************************************************************
Function     :  USART2_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/
unsigned char query_complete_flag;
extern unsigned char *ptr_Trans, *ptr_rec;
extern void Uart_Frame_Received(void);
unsigned char buf_in;
unsigned char buf_out;
extern unsigned char modb_RxCntr,modb_TxCntr;
unsigned char usart_count=0;
unsigned char time=0;
//unsigned char usart_count=0;

void USART2_IRQHandler(void)
{
  volatile unsigned int IIR,data;
  IIR = USART2->SR;
  if(IIR & USART_FLAG_RXNE)//USART_FLAG_RXNE
  {
    USART2->SR &= ~USART_FLAG_RXNE;	          // clear interrupt
    IIR = USART2->SR;
    if(query_complete_flag==0)
    {
       *(ptr_rec+modb_RxCntr++) = USART2->DR;
       if((modb_RxCntr>=8))
       {
         TIM_Cmd(TIM6,ENABLE);
         usart_count=0;
         buf_in =0;
         buf_out =0;
         Uart_Frame_Received();
         modb_RxCntr=0;
         time=10;
       }
    }
  }
  if (IIR & USART_FLAG_TXE)
  {
    if(USART_GetITStatus(USART2, USART_IT_TC) == SET)
    {
      USART_ClearITPendingBit(USART2, USART_IT_TXE);
      USART_ClearITPendingBit(USART2, USART_IT_TC);
      USART2->SR &=~USART_FLAG_TXE;
      if(query_complete_flag==1)
      {
        USART_SendData(USART2, *(ptr_Trans+(++buf_out)));
        USART_ClearITPendingBit(USART2, USART_IT_TXE);
        if(buf_in == buf_out)
        {
          for(unsigned int j=0;j<9000;j++);
          USART_ITConfig(USART2, USART_IT_TC, DISABLE);          
          query_complete_flag=0;
          modb_RxCntr=0xFF;
          usart_count=0xff;
          GPIO_ResetBits(GPIOA,GPIO_Pin_1);
        }
      }
    }
  }
   if (IIR & USART_FLAG_ORE)
  {
    data=USART2->SR;
    data=USART2->DR;
  }
//////  if ((bRXEnabled &&USART_GetITStatus(USART2, USART_IT_RXNE) != RESET))
//////  {
//////     USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//////     xMBRTUReceiveFSM();
//////  }
//////  else if(bRXEnabled &&(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET))
//////  {
//////  }
//////
//////  else if(bTXEnabled && USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
//////  {
//////     xMBRTUTransmitFSM();
//////  }
}

/***************************************************************************************************************************
Function     :  TIM6_IRQHandler
Parameter    :  null
Description  :  This TIM6_IRQHandler is used to handle timer overflow interrupt  is used for MODBUS
***************************************************************************************************************************/
extern unsigned char modb_RxCntr,modb_TxCntr;
void TIM6_IRQHandler(void)
{

    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
       TIM_ClearFlag(TIM6,TIM_FLAG_Update);
       usart_count=0;
       modb_RxCntr=0x00;
       TIM_Cmd(TIM6,DISABLE);
    }
}
/***************************************************************************************************************************
Function     :  TIM2_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/
void TIM2_IRQHandler(void)
{

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {

       TIM_ClearFlag(TIM2,TIM_FLAG_Update);
    }

}
/***************************************************************************************************************************
Function     :  TIM3_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/

void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    if( Run_flag==1)
    {
      curr_page++;
      if(curr_page==11)        //10) Changes to add per phase PF
      {
         curr_page=1;
      }
      if((curr_page==3)&&(reqConfiguration.uiSysConfiguration==1))   //skip page 3 i.e.vrn,vyn and vbn for 3P2E
      {
         curr_page=4;
      }
      if((curr_page==4)&&(reqConfiguration.uiSysConfiguration==3))   //skip page 4 i.e.vry,vyb,vbr for 1P1E
      {
         curr_page=5;
      }
      CreatePagePattParamDisp(curr_page);                               //For display all parameter
      MountDisplay(uiParamPage[(curr_page-1)], 0);
    }
    TIM_ClearFlag(TIM3,TIM_FLAG_Update);
   }
}

/***************************************************************************************************************************
Function     :  Init_keypad
Parameter    :  null
Description  :
***************************************************************************************************************************/

void Init_keypad (void)
{
	key_code.msb          = 0;
	key_code.key_reco     = 0;
	key_code.ScanCodetemp = 0;
	key_code.RetCodetemp  = 0;
}
/***************************************************************************************************************************
Function     :  Key_Read1
Parameter    :  null
Description  :
***************************************************************************************************************************/
unsigned char Key_Read1(void)
{
   unsigned char KEY;
   if(key_code.msb == 1)						        // Flag to indicate key detection
   {
     key_code.msb = 0;
     key_code.key_reco = 2;

       if(key_code.RetCode == 1)
       {			
	  KEY = (keys[0]);
          return KEY;
       }
       else if(key_code.RetCode == 2)
       {
	  KEY = (keys[1]);
	  return KEY;
       }
   }
   return KEY_NOT_PRESSED;
}
/***************************************************************************************************************************
Function     :  TIM4_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/

void TIM4_IRQHandler(void)
{
  static volatile unsigned char ucDeBounce0 = 0;
  static volatile unsigned char ucDeBounce1 = 0;

  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
     No_ip++;
     if(No_ip==1000)No_ip=1;
     Display(FinalDisp);

     key_code.RetCodetemp  = (( KEYPAD_ROW_SCAN & ROW_MASK)>>6);

     if (key_code.RetCodetemp == 3)										       //no key pressed
     {
       if(key_code.key_reco!=0)
       {	
         key_code.key_reco = 0;
	  Keypad_dbnce = 0;
       }
     }
     else
     {
	if(( key_code.RetCode == key_code.RetCodetemp))	  //same as last key				
        {
	  if(key_code.key_reco==1)					   // For repeatative send comment this sentence
	  {
	    Keypad_dbnce++;
	    if(Keypad_dbnce==8)
	    {
	      key_code.msb = 1;
	      Keypad_dbnce=0;
	    }
          }
        }
	if (!key_code.key_reco)
	{
	  key_code.key_reco = 1;
	  Keypad_dbnce=0;
	  key_code.RetCode = key_code.RetCodetemp;
        }
       }
//      uiTxFrameDelay++;
//      if ( uiTxFrameDelay > 15 )
//      {
//        uiTxFrameDelay= 0;
//      }
     TIM_ClearFlag(TIM4,TIM_FLAG_Update);
    }
}
/***************************************************************************************************************************
Function     :  TIM5_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/

void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
       TIM_ClearFlag(TIM5,TIM_FLAG_Update);
    }
}

/***************************************************************************************************************************
Function     :  TIM9_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/
void TIM9_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
    {
       TIM_ClearFlag(TIM9,TIM_FLAG_Update);
    }
}
/***************************************************************************************************************************
Function     :  Key_Read1
Parameter    :  null
Description  :  modbus timer
***************************************************************************************************************************/
void TIM10_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
    {
      TIM_ClearITPendingBit(TIM10, TIM_IT_Update );
      xMBRTUTimerT35Expired();
    }
}
/***************************************************************************************************************************
Function     :  TIM11_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/

void TIM11_IRQHandler(void)
{
   if (TIM_GetITStatus(TIM11, TIM_IT_Update) != RESET)
    {
        
        TIM_ClearITPendingBit(TIM11, TIM_IT_Update );

        Sec++;      
        
       // Sum_Watt = ((Sum_Watt + Energy_watt_tot));
        Sum_Watt = ((Sum_Watt + reqMeasurepara.flKWTot )) ;
        KWH = Sum_Watt;
        KWHf = KWH/3600;
     
       
         count++;
         if (count >=2)       //30
         {
                count=0;
              //  write_cycle= 3700023;
                write_cycle++;
                
              //  KWHf =write_cycle;
               // --------------------  write Watt_data to EEprom -----------------------
                if(eeprom_flag ==0)                       
                {
                  eeprom_flag=1;
                     if(memory_location_flag==0)
                      {
                         fnWriteDataToMemory(0x00010,0);       
                      }
                     else
                     {
                       fnWriteDataToMemory(0x00030,0);         // write Watt_data
                     }
               }
                // ------------------write write_cycle to EEprom----------------------------
               else                                          
               {
                   eeprom_flag=0;
                             if(memory_location_flag==0)
                             {
                                           fnWriteDataToMemory(0x00020,1);         //s  ave write cycle        
                                         if(write_cycle==2)
                                          {
                                              for (w=0;w<1000;w++)
                                              for (w=0;w<1000;w++);
                                              for (w=0;w<1000;w++);
                                              for (w=0;w<1000;w++);
                                              memory_location_flag=0;
                                              fnWriteDataToMemory(0x00060,2);      // set flag==0      save memory_location_flag
                                          }
                                          else if(write_cycle>=3700000)                 // 3700000 after 7 years memory location get change
                                          {    
                                              for (w=0;w<1000;w++)
                                              for (w=0;w<1000;w++);
                                              for (w=0;w<1000;w++);
                                              for (w=0;w<1000;w++);
                                              write_cycle=0;
                                              memory_location_flag=1;
                                              fnReadFromNVMemory(0x00060,2);                // save memory_location_flag
                                         }
                           }
                           else
                           {
                               fnWriteDataToMemory(0x00040,1);                       //save write cycle
                           }
               }
                
                
             
            
         }
        xMBRTUTimerT35Expired();
        
    }
}
/***************************************************************************************************************************
Function     :  Key_Read1
Parameter    :  null
Description  :
***************************************************************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC1))                                                 /* Test on DMA1 Channel6 Transfer Complete interrupt */
  {
      DMA_ClearITPendingBit(DMA1_IT_GL1);
      EndOfTransfer=1;
  }
}
/***************************************************************************************************************************
Function     :  ADC1_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/

void ADC1_IRQHandler(void)
{
  if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);                                  /* Clear ADC1 AWD pending interrupt bit */
  }
}
/***************************************************************************************************************************
Function     :  EXTI9_5_IRQHandler
Parameter    :  null
Description  :
***************************************************************************************************************************/
float Freq_calib_factor;
float factor1=0,factor2=0,factor=1;

extern unsigned char freq_calib_flag;

void EXTI9_5_IRQHandler(void)
{
  if((EXTI_GetITStatus(EXTI_Line8) != RESET)||(EXTI_GetITStatus(EXTI_Line9) != RESET))
  {
    MYFREQ=(TIM_GetCounter(TIM9));
    MyCnt++;
    if(MyCnt==1)
    {
       TIM_Cmd(TIM9, ENABLE);                                                // timer start
    }
    else if(MyCnt==2)
    {
      TIM_Cmd(TIM9, DISABLE);
      TIM_SetCounter(TIM9,0) ;
      MyCnt=0;
      Frequ=(2000000.0/(float)MYFREQ);
      if((Frequ>=35.00)&&(Frequ<=80.00))
      {
         Freq_count++;
         if( Freq_count>=3)
         {
          AVG_FREQ=Frequ+AVG_FREQ;
         }
         if(Freq_count==18)//18
         {
           Fin_Freq=(AVG_FREQ/16.0);//16
           if(freq_calib_flag==1)
              Freq_calib_factor = (50.00/Fin_Freq);
           else if(freq_calib_flag==2)
              factor1 = (50.00/(Fin_Freq* Freq_calib_factor));
           else if(freq_calib_flag==3)
           {  factor2 = (50.00/(Fin_Freq* Freq_calib_factor));
              factor=((factor1+factor2)/2);
           }
           reqMeasurepara.flFreq = (Fin_Freq * Freq_calib_factor);//*1.002406;
           reqMeasurepara.flFreq = (reqMeasurepara.flFreq * factor);//*1.002406;

           AVG_FREQ=0.0;
           Freq_count=0;
           freq_comp=1;
         }
      }
    }
    EXTI_ClearITPendingBit(EXTI_Line8);
    EXTI_ClearITPendingBit(EXTI_Line9);
  }
}

/***************************************************************************************************************************
Function     :  Key_Read1
Parameter    :  null
Description  :
***************************************************************************************************************************/
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line0);                                        /* Clear the EXTI line 0 pending bit */
  }
}
/***************************************************************************************************************************
Function     :  Key_Read1
Parameter    :  null
Description  :
***************************************************************************************************************************/
void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line1);                                       /* Clear the EXTI line 1 pending bit */
  }
}

/***************************************************************************************************************************/
