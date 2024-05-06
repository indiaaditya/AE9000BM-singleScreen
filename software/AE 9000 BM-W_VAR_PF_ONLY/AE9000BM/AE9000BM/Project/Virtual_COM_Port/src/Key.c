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

extern  EXTI_InitTypeDef EXTI_InitStructure;
extern MEASURED_PARAM  reqMeasurepara;
extern CONFIGURATION_PARAM reqConfiguration;
extern volatile unsigned char ucKeyPressedd0;
extern volatile unsigned char ucKeyPressedd1;
extern volatile unsigned char ucKeyPressed0 ;
extern volatile unsigned char ucKeyPressed1;
extern volatile unsigned char ucKeyPressFlag0;
extern volatile unsigned char ucKeyPressFlag01;
extern volatile unsigned char ucKeyPressFlag1;
extern volatile unsigned char ucKeyPressFlag11;
extern volatile unsigned char ucKBIFlag0 ;
extern volatile unsigned char ucKBIFlag1 ;
extern volatile unsigned char ucKBIFlag0 ;
extern volatile unsigned char ucKBIFlag1 ;

extern unsigned int uiConfigPage[10][8];
extern unsigned int uiParamPage[9][8];
extern unsigned int uiTestPage[1][8];
extern unsigned char uiBlinkChar,mod_id;
extern unsigned int value;
extern unsigned long ModBuadRate;
extern long const pow_10[8];
unsigned char Flag_Entr,Run_flag=1,Set_Cunt=0,curr_page;
unsigned char ConFig_Flag,BaudRate_flag,On_time_Flag;
static unsigned char ucDigitArray[8]= {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char Pos_BlinkArry[8]={5,6,7,8,9,10,11,12};
long x;
unsigned long ulValue;
unsigned int uiInd;
extern float KWHf,Sum_Watt;
 extern unsigned int i;
 unsigned long val;
 static unsigned char ucValue = 0;
 static unsigned char ucPos = 0;
 extern unsigned char Reset_Energy_flag;
 extern unsigned int device_id;
/***********************Key main function*******************************/
//void Key_Read(void)
//{
//      if(SET == ucKeyPressFlag0)
//      {
//      	ucKeyPressFlag0 = CLEAR;
//
//        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) != RESET)
//        {
//          ucKBIFlag0 = 2;
//          EXTI_InitStructure.EXTI_Line = EXTI_Line7;
//          EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//          EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//          EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//          EXTI_Init(&EXTI_InitStructure);
//        }
//        else
//        {
//           Select_Key();              /*---------------------Select key---------------------------------*/
//          ucKeyPressedd0 = 0;
//          ucKBIFlag0 = 1;
//          EXTI_InitStructure.EXTI_Line = EXTI_Line7 ;
//          EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//          EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//          EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//          EXTI_Init(&EXTI_InitStructure);
//        }
//      }
//      if(SET == ucKeyPressFlag1)
//      {
//      	ucKeyPressFlag1 = CLEAR;
//        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) != RESET)
//        {
//          ucKBIFlag1 = 2;
//          EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//          EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//          EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//          EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//          EXTI_Init(&EXTI_InitStructure);
//        }
//        else
//        {
//         Enter_Key();                 /*-------------------------ENETR KEY-------------------------------*/
//          ucKeyPressedd1 = 0;
//          ucKBIFlag1 = 1;
//          EXTI_InitStructure.EXTI_Line =  EXTI_Line6 ;
//          EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//          EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//          EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//          EXTI_Init(&EXTI_InitStructure);
//        }
//      }
//       if(SET == ucKeyPressFlag01)
//      {
//        ucKeyPressFlag01 = CLEAR;
//
//          ucKBIFlag0 = 1;
//          EXTI_InitStructure.EXTI_Line = EXTI_Line7;
//          EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//          EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//          EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//          EXTI_Init(&EXTI_InitStructure);
//
//      }
//
//      if(SET == ucKeyPressFlag11)
//      {
//         ucKeyPressFlag11 = CLEAR;
//          ucKBIFlag1 = 1;
//          EXTI_InitStructure.EXTI_Line =  EXTI_Line6 ;
//          EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//          EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//          EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//          EXTI_Init(&EXTI_InitStructure);
//      }
//}

/*************************ENTER KEY Function**********************************************************/

void Enter_Key(void)
{
   Flag_Entr++;
   Run_flag=0;

   if(Flag_Entr==1)             /*----------------------Display configuration window----------------------*/
   {
     ConFig_Flag=1;
     Set_Cunt=1;
     Config_Fun();
   }
   if(Set_Cunt==1)           /*-----------------------For system configuration-----------------------*/
   {
     if(Flag_Entr==2)
     {
       Blink_Config();
       ConFig_Flag=0; 
     }
     else if (Flag_Entr==3)
     {
       Config_Fun();
       Flag_Entr=1;       /*save configuration*/
       ConFig_Flag=1;
     }
   }
   else if ((Set_Cunt==2)|| (Set_Cunt==3)||(Set_Cunt==4)||(Set_Cunt==5))          /*---------------------CTR/PTR Ratio--------------------------*/
   {
     if(Flag_Entr <= 9)
     {
       ConFig_Flag=0;
       if(Flag_Entr==2)
       { ucPos=0; }
       else {ucPos++;}
       if(Set_Cunt==2){val = (unsigned long)(reqConfiguration.ulCTRPri);}
       else if (Set_Cunt==3){val = (unsigned long)reqConfiguration.ulCTRSec;}
       else if (Set_Cunt==4){val = (unsigned long)reqConfiguration.ulPTRPri;}
       else if (Set_Cunt==5){val = (unsigned long)reqConfiguration.ulPTRSec;}
       for (i=0; i< 8; i++)
       {
         x = pow_10[7-i];
         uiInd = (unsigned int)(val/x);
         ucDigitArray[i] = uiInd;
         val = val%x;
       }
       uiBlinkChar=Pos_BlinkArry[ucPos];
       CreatePagePattConfigDisp(Set_Cunt);
       MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
     }
     else
     {
       Config_Fun();
       ConFig_Flag=1;               //save ratio
       Flag_Entr=1;
       ucPos=0;
       Factor_Update();
     }
   }
   else if (Set_Cunt==6)             /*---------------------Scroll--------------------------*/
   {
      if(Flag_Entr==2)
      {
        Blink_Config();
        ConFig_Flag=0;
      }
      if(Flag_Entr==3)
      {
        if(reqConfiguration.uiScrollON==1)
        {
         reqConfiguration.uiScrollON=0;             //scroll OFF
         Config_Fun();
         ConFig_Flag=1;
         Flag_Entr=1;
         TIM_Cmd(TIM3, DISABLE);
         reqConfiguration.uiScrollON=0;
        }
        else
        {
         On_time_Flag=1;
         reqConfiguration.uiScrollON=1;              //scroll ON
         uiBlinkChar=18;
         CreatePagePattConfigDisp(Set_Cunt);
         MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
        }
      }
      if(Flag_Entr==4)
      {
        reqConfiguration.uiScrollON=1;             //scroll ON and save scroll time.
        value=5000*(reqConfiguration.uiScrollTime);
        TIM3_Configuration(value);
        Config_Fun();
        ConFig_Flag=1;
        Flag_Entr=1;
        On_time_Flag=0;
      }
   }
   else if (Set_Cunt==7)             /*---------------------MOD ID-------------------------*/
   {
     if(Flag_Entr==2)
     {
       uiBlinkChar=Pos_BlinkArry[ucPos];
       CreatePagePattConfigDisp(Set_Cunt);
       MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
       ConFig_Flag=0;
       val = reqConfiguration.uiNetworkID;
       for (i=0; i<2; i++)
       {
          uiInd = val & 0x0f;
          ucDigitArray[1-i] = uiInd;
          val = val >> 4;
       }
     }
     else if (Flag_Entr==3)
     {
       ucPos++;
       uiBlinkChar=Pos_BlinkArry[ucPos];
       CreatePagePattConfigDisp(Set_Cunt);
       MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
     }
     else if (Flag_Entr==4)
     {
      Config_Fun();
      ConFig_Flag=1;
      Flag_Entr=1;
      ucPos=0;
     }
   }
   else if (Set_Cunt==8)            /*---------------------Baud Rate--------------------------*/
   {
     if(Flag_Entr==2)
     {// Shree made changes
       if((reqConfiguration.uiBaudRate==4)) //(reqConfiguration.uiBaudRate==3)||
       {
         BaudRate_flag=1;//For baud rate 19 38 Row three blinking flag
       }
       else
       {
         BaudRate_flag=0; // for baud rate 48 96  // 96 19
       }
       Blink_Config();
       ConFig_Flag=0;
      }
             else if (Flag_Entr==3)
             {
               Config_Fun();
               Flag_Entr=1;       //save configuration
               ConFig_Flag=1;

                xMBPortSerialInit(1,ModBuadRate,8, MB_PAR_EVEN );
             }
          /*
            if(Flag_Entr==2)
             {
               if((reqConfiguration.uiBaudRate==3)||(reqConfiguration.uiBaudRate==4))
            {
             BaudRate_flag=1;
            }
            else
            {
            BaudRate_flag=0;

            }

               Blink_Config();
                ConFig_Flag=0;
             }
             else if (Flag_Entr==3)
             {


             Config_Fun();
             Flag_Entr=1;       //save configuration
             ConFig_Flag=1;
              xMBPortSerialInit(1,ModBuadRate,8, MB_PAR_EVEN );
             }
          */
   }
//                     /*---------------------For RESET ENERGY --------------------------*/

     else if (Set_Cunt==9)
     {

                       if(Flag_Entr==2)
                        {
                          Blink_Config();
                          ConFig_Flag=0;
                        }
                    else if (Flag_Entr==3)
                   {
                          if(Reset_Energy_flag== 0)
                          {
                                 KWHf=0;
                                 Sum_Watt=0;
                                 fnWriteDataToMemory(0x00010,0);
                          }
                          Config_Fun();
                          ConFig_Flag=1;
                          Flag_Entr=1;
                   }

     }

   else if (Set_Cunt==10)                   /*---------------------For Test --------------------------*/
   {

      CreatePage(TEST_DISP, 1, All_GLOW,All_GLOW , All_GLOW, All_GLOW);
      MountDisplay(uiTestPage[0],0);
      Flag_Entr=1;
     delay(65535); delay(65535); delay(65535);delay(65535); delay(65535); delay(65535);
     delay(65535); delay(65535); delay(65535);delay(65535); delay(65535); delay(65535);
     delay(65535); delay(65535); delay(65535);delay(65535); delay(65535); delay(65535);
      Config_Fun();
      Flag_Entr=1;
      ConFig_Flag=1;
   }
   else if (Set_Cunt==11)                 //For exit
   {

          WRITE_CONFG();
            curr_page=1;
            CreatePagePattParamDisp(curr_page);                               //For display all parameter
            MountDisplay(uiParamPage[(curr_page-1)], 0);
            Run_flag=1;
            Flag_Entr=0;
   }
}

/*****************SELECT KEY*********************************************/
void Select_Key(void)
{

  if(Run_flag==1)            /*--------------display pages as per key selection-----------------------*/
     {

          if(reqConfiguration.uiScrollON==0)
          {
            //curr_page++; :IANET: 24042024: To disable the scroll function, since only one screen is being displayed.
             if(curr_page==12)//10) Changes to add per phase PF
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
             MountDisplay(uiParamPage[(curr_page-1)],0);
          }
    }
   else
   {
                if(ConFig_Flag==1)         /*--------------select window of configuration-----------------------*/
                {
                Set_Cunt++;
                //IANET: 060524
                if((Set_Cunt==6) || (Set_Cunt==9)) //6: scroll 9: Reset Energy
                  Set_Cunt++;


                if(Set_Cunt==12)
                {
                Set_Cunt=1;
                }
                Config_Fun();
                }
                else
                {
                     if(Set_Cunt==1)            /*--------------select system config-----------------------*/
                     {
                      reqConfiguration.uiSysConfiguration++;
                      if(reqConfiguration.uiSysConfiguration==4)
                      {
                      reqConfiguration.uiSysConfiguration=0;
                      }
                     Blink_Config();
                     }
                    else if (Set_Cunt==2)      /*--------------select Ctp-----------------------*/
                     {      ucDigitArray[ucPos]++;
                            if(ucDigitArray[ucPos] >= 10)
                            ucDigitArray[ucPos]=0;
                            ulValue = 10000000*ucDigitArray[0] + 1000000*ucDigitArray[1] + 100000*ucDigitArray[2] + \
                                      10000 * ucDigitArray[3]+ 1000*ucDigitArray[4] + 100*ucDigitArray[5] + \
                                      10*ucDigitArray[6] + ucDigitArray[7];
                            reqConfiguration.ulCTRPri = ulValue;
                            uiBlinkChar=Pos_BlinkArry[ucPos];
                            CreatePagePattConfigDisp(Set_Cunt);
                            MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);

                    }
                    else if (Set_Cunt==3)   /*--------------select Cts-----------------------*/
                    {
                           ucDigitArray[ucPos]++;
                           if(ucDigitArray[ucPos] >= 10)
                            ucDigitArray[ucPos]=0;
                            ulValue = 10000000*ucDigitArray[0] + 1000000*ucDigitArray[1] + 100000*ucDigitArray[2] + \
                                      10000 * ucDigitArray[3]+ 1000*ucDigitArray[4] + 100*ucDigitArray[5] + \
                                      10*ucDigitArray[6] + ucDigitArray[7];
                            reqConfiguration.ulCTRSec = ulValue;
                            uiBlinkChar=Pos_BlinkArry[ucPos];
                            CreatePagePattConfigDisp(Set_Cunt);
                            MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);


                    }
                    else if (Set_Cunt==4)    /*--------------select Ptp-----------------------*/
                    {

                          ucDigitArray[ucPos]++;
                          if(ucDigitArray[ucPos] >= 10)
                          ucDigitArray[ucPos]=0;
                           ulValue = 10000000*ucDigitArray[0] + 1000000*ucDigitArray[1] + 100000*ucDigitArray[2] + \
                                     10000 * ucDigitArray[3]+ 1000*ucDigitArray[4] + 100*ucDigitArray[5] + \
                                      10*ucDigitArray[6] + ucDigitArray[7];
                            reqConfiguration.ulPTRPri = ulValue;
                             uiBlinkChar=Pos_BlinkArry[ucPos];
                            CreatePagePattConfigDisp(Set_Cunt);
                            MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
                    }
                    else if (Set_Cunt==5)    /*--------------select Pts-----------------------*/
                    {
                            ucDigitArray[ucPos]++;
                            if(ucDigitArray[ucPos] >= 10)
                            ucDigitArray[ucPos]=0;
                            ulValue = 10000000*ucDigitArray[0] + 1000000*ucDigitArray[1] + 100000*ucDigitArray[2] + \
                                      10000 * ucDigitArray[3]+ 1000*ucDigitArray[4] + 100*ucDigitArray[5] + \
                                      10*ucDigitArray[6] + ucDigitArray[7];
                             reqConfiguration.ulPTRSec = ulValue;
                             uiBlinkChar=Pos_BlinkArry[ucPos];
                             CreatePagePattConfigDisp(Set_Cunt);
                             MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
                    }
                    //IANET: commented on 060524
                    /*else if (Set_Cunt==6)   /*--------------select scroll-----------------------
                    {
                      if(On_time_Flag==1)
                      {                      // ON time edit

                            reqConfiguration.uiScrollTime++;
                           if(reqConfiguration.uiScrollTime==11)
                           {
                           reqConfiguration.uiScrollTime=0;
                           }
                            reqConfiguration.uiScrollON=1;              //scroll ON
                            uiBlinkChar=18;
                            CreatePagePattConfigDisp(Set_Cunt);
                            MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
                      }
                      else
                      {
                           if(reqConfiguration.uiScrollON==1)  //for scroll on
                           {
                               Blink_Config();
                               reqConfiguration.uiScrollON=0;
                           }
                           else         //for scroll off
                           {
                               Blink_Config();
                              reqConfiguration.uiScrollON=1;
                           }
                      }
                    }*/
                    else if (Set_Cunt==7)     /*---------------------------MOD ID SELECTION-------------------------*/
                    {  ucDigitArray[ucPos]++;
                        if(ucDigitArray[ucPos] >= 15)
                           ucDigitArray[ucPos]=0;

                           ucValue = (ucDigitArray[0] << 4) | ucDigitArray[1];
                            reqConfiguration.uiNetworkID= (unsigned int)ucValue;
                            mod_id=(unsigned char)(reqConfiguration.uiNetworkID);
                            device_id=mod_id;
                            uiBlinkChar=Pos_BlinkArry[ucPos];
                            CreatePagePattConfigDisp(Set_Cunt);
                            MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);

                    }
                    else if (Set_Cunt==8)      /*---------------------------Baud Rate Selection-------------------------*/
                    {
                            reqConfiguration.uiBaudRate++;
                            if(reqConfiguration.uiBaudRate==4)// changed from if(reqConfiguration.uiBaudRate==5)  abhijit on 28/08/2015
                            { // Shree made changes
                             reqConfiguration.uiBaudRate=1; //// changed from reqConfiguration.uiBaudRate=2; abhijit on 28/08/2015 // changes Baud skipping 4800
                            }
                            if(reqConfiguration.uiBaudRate==4) //((reqConfiguration.uiBaudRate==3)||(reqConfiguration.uiBaudRate==4)) //
                            {
                             BaudRate_flag=1;
                            }
                            else
                            {
                               BaudRate_flag=0;
                            }
                            Blink_Config();
                            /*
                            reqConfiguration.uiBaudRate++;
                             if(reqConfiguration.uiBaudRate==5)
                            {
                            reqConfiguration.uiBaudRate=1;
                            }
                            if((reqConfiguration.uiBaudRate==3)||(reqConfiguration.uiBaudRate==4))
                            {
                             BaudRate_flag=1;
                            }
                            else
                            {
                            BaudRate_flag=0;
                            }
                            Blink_Config();
                            */
                   }
                  //IANET: commented on 060524
                   /* else if (Set_Cunt==9)                   /*---------------------For RESET ENERGY --------------------------
                     {

                       if(Reset_Energy_flag ==0)
                         Reset_Energy_flag=1;
                      else
                        Reset_Energy_flag=0;


                      CreatePagePattConfigDisp(Set_Cunt);
                      MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
                     }
                     */ 


                }

   }
}

/***********************************Display configuration without blink*********************/
void Config_Fun(void)
{
  uiBlinkChar=0;
  CreatePagePattConfigDisp(Set_Cunt);
  MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
}
/***********************************Display configuration with blink*********************/


void Blink_Config(void)
{
  if(BaudRate_flag==1)
  {
   uiBlinkChar=14;
   BaudRate_flag=0;
  }
  else
  {
   uiBlinkChar=13;
  }
  CreatePagePattConfigDisp(Set_Cunt);
  MountDisplay(uiConfigPage[Set_Cunt-1], uiBlinkChar);
}

void ED_Config(void)
{

}
void ED_PTP_Fun(void)
{

}
void ED_PTS_Fun(void)
{

}
void ED_CTP_Fun(void)
{


}

void ED_CTS_Fun(void)
{

}
void ED_Scroll_Fun(void)
{

}
void ED_NI_Fun(void)
{

}
void ED_Baud_Fun(void)
{

}
void Test_Fun(void)
{

}