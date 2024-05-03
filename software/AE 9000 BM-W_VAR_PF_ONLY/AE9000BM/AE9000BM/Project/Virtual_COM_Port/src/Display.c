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
#include <stdio.h>
#include <string.h>

extern CONFIGURATION_PARAM reqConfiguration;
extern MEASURED_PARAM  reqMeasurepara;


 unsigned char ucRow1[4];
 unsigned char ucRow2[4];
 unsigned char ucRow3[4];
 unsigned char ucSingleLED[4] = {0x00, 0x00, 0x00, 0x00};
 unsigned char ucPage;
 unsigned int i;
 signed int uiPrintval;

unsigned int uiParamPage[10][8]; //Changes to add per phase PF [9][8];
unsigned int uiConfigPage[10][8];
unsigned int uiTestPage[1][8];
unsigned char temp[4]={0,0,0,0};
unsigned int BlinkMask[8];
unsigned int FinalDisp[8];
unsigned char uiBlinkChar;
unsigned int uiInitPage[8];
unsigned char ucIndex = 0;
long const pow_10[8] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
extern unsigned char ConFig_Flag,pf_error;
extern float KWH,Sum_Watt,Sum_Var,KVAR,KWHf,KVARf;
unsigned int KWHf_low,KWHf_high;
/* Changes PF Error added 19 April 2013 */
extern unsigned char pfr_error,pfy_error,pfb_error;
 unsigned char ucRowLong[8],Reset_Energy_flag=1;
 extern unsigned char reverse_flag;
 extern unsigned char reverse_flag_r_phase,reverse_flag_y_phase,reverse_flag_b_phase;

unsigned char hex_digit_patts[16]=

   {CHAR_ZERO, CHAR_ONE, CHAR_TWO, CHAR_THREE, CHAR_FOUR, CHAR_FIVE, CHAR_SIX, CHAR_SEVEN,

    CHAR_EIGHT, CHAR_NINE, CHAR_A, CHAR_b, CHAR_C, CHAR_d, CHAR_E, CHAR_F};


inline float fnabs(float  val)
{
      if(val < 0)
      {
        val = val * -1;
      }
      return val;
}
/*************Final Display on 7-Seg********************************************************/
void Display(unsigned int *uiDispPage)
{
  static unsigned char ucDispIndx = 0;
  unsigned int BlinkDisp[8]={0, 0, 0, 0, 0, 0, 0, 0};
  int i;
  static unsigned int itrn = 0;

  BlinkDisp[ucDispIndx] =  (uiDispPage[ucDispIndx] & BlinkMask[ucDispIndx]);

 if(itrn < 300)
 {
   SPI_I2S_SendData(SPI1, uiDispPage[ucDispIndx]);
 }
 else if(itrn >= 300)
  {
    SPI_I2S_SendData(SPI1,  BlinkDisp[ucDispIndx]);
 }
 if(itrn >= 500)
    itrn = 0;
 itrn++;
  while((SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)))
   ;
  GPIO_WriteBit(GPIOB,GPIO_Pin_6 ,Bit_SET);
  for(i=0;i<8;i++)
   ;
 GPIO_WriteBit(GPIOB,GPIO_Pin_6 ,Bit_RESET);

  switch(ucDispIndx)
  {
  case 0:
    {
      GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,Bit_RESET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,Bit_RESET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_5 ,Bit_RESET);
      break;
    }
  case 1:
    {
      GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,Bit_SET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,Bit_RESET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
      break;
    }
  case 2:
    {
      GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,Bit_RESET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,Bit_SET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_5 ,Bit_RESET);
      break;
    }
  case 3:
    {
      GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,Bit_SET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,Bit_SET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_5 ,Bit_RESET);
      break;
    }
  case 4:
    {
      GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,Bit_RESET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,Bit_RESET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);
      break;
    }
  case 5:
    {
      GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,Bit_SET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,Bit_RESET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_5 ,Bit_SET);
      break;
    }
  case 6:
    {
      GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,Bit_RESET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,Bit_SET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_5 ,Bit_SET);
      break;
    }
  case 7:
    {
      GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,Bit_SET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,Bit_SET);
      GPIO_WriteBit(GPIOB,GPIO_Pin_5 ,Bit_SET);
      break;
    }
  default:
    {
      break;
    }
  }
  ucDispIndx++;
  if(ucDispIndx >= 8)
  {
     ucDispIndx = 0;
  }
}

/*****************************Mount Display*********************************************/
void MountDisplay(unsigned int * DisplayScreen, unsigned int uiBlinkChar)
{
    char i;
  for(i=0; i<8; i++)
  {
    BlinkMask[i] = 0xFFFF;
  }
  for(i=0; i<8; i++)
  {
     FinalDisp[i] = DisplayScreen[i];
  }
  switch(uiBlinkChar)
  {
  case 0:
    {
       break;
    }
  case 1:
    {
      BlinkMask[0] &= 0x00FF;
      break;
    }
  case 2:
    {
      BlinkMask[1] &= 0x00FF;
      break;
    }
  case 3:
    {
      BlinkMask[2] &= 0x00FF;
      break;
    }
  case 4:
    {
      BlinkMask[3] &= 0x00FF;
      break;
    }
  case 5:
    {
      BlinkMask[4] &= 0xFF00;
      break;
    }
  case 6:
    {
      BlinkMask[5] &= 0xFF00;
      break;
    }
  case 7:
    {
      BlinkMask[6] &= 0xFF00;
      break;
    }
  case 8:
    {
      BlinkMask[7] &= 0xFF00;
      break;
    }
  case 9:
    {
      BlinkMask[0] &= 0xFF00;
      break;
    }
  case 10:
    {
      BlinkMask[1] &= 0xFF00;
      break;
    }
  case 11:
    {
      BlinkMask[2] &= 0xFF00;
      break;
    }
  case 12:
    {
     BlinkMask[3] &= 0xFF00;
      break;
    }
  case 13:
    {
     BlinkMask[4] &= 0xFF00;
     BlinkMask[5] &= 0xFF00;
     BlinkMask[6] &= 0xFF00;
     BlinkMask[7] &= 0xFF00;
      break;
    }
  case 14:
    {
     BlinkMask[0] &= 0xFF00;
     BlinkMask[1] &= 0xFF00;
     BlinkMask[2] &= 0xFF00;
     BlinkMask[3] &= 0xFF00;
      break;
    }
    case 15:
    {
     BlinkMask[0] &= 0x00FF;
     BlinkMask[1] &= 0x00FF;
     BlinkMask[2] &= 0x00FF;
     BlinkMask[3] &= 0x00FF;
      break;
    }
    case 16:
    {
     BlinkMask[4] &= 0xFF00;
     BlinkMask[5] &= 0xFF00;
      break;
    }
    case 17:
    {
     BlinkMask[6] &= 0xFF00;
     BlinkMask[7] &= 0xFF00;
      break;
    }
    case 18:
    {
     BlinkMask[0] &= 0xFF00;
     BlinkMask[1] &= 0xFF00;
      break;
    }
    case 19:
    {
     BlinkMask[2] &= 0xFF00;
     BlinkMask[3] &= 0xFF00;
      break;
    }

  default:
    {
      break;

    }
  }
}
/*******************Page Create Function********************************************/
void CreatePage(unsigned char ucPageType, unsigned char ucPageId, unsigned char const * ucRow1, unsigned char const * ucRow2, unsigned char const * ucRow3, unsigned char const * ucRow4)
{
  unsigned int i;
  if(ucPageType == PARAM_DISP)
  {
    for(i=0; i<4; i++)
    {
      uiParamPage[ucPageId - 1][i] = ucRow1[i];
      uiParamPage[ucPageId - 1][i] <<= 8;
      uiParamPage[ucPageId - 1][i] |= ucRow3[i];
      uiParamPage[ucPageId - 1][i+4] = ucRow4[i];
      uiParamPage[ucPageId - 1][i+4] <<= 8;
      uiParamPage[ucPageId - 1][i+4] |= ucRow2[i];
    }
  }
  else if(ucPageType == CONFIG_DISP)
  {
    for(i=0; i<4; i++)
    {
      uiConfigPage[ucPageId - 1][i] = ucRow1[i];
      uiConfigPage[ucPageId - 1][i] <<= 8;
      uiConfigPage[ucPageId - 1][i] |= ucRow3[i];
      uiConfigPage[ucPageId - 1][i+4] = ucRow4[i];
      uiConfigPage[ucPageId - 1][i+4] <<= 8;
      uiConfigPage[ucPageId - 1][i+4] |= ucRow2[i];
    }
  }
  else if(ucPageType == TEST_DISP)
  {
    for(i=0; i<4; i++)
    {
      uiTestPage[ucPageId - 1][i] = ucRow1[i];
      uiTestPage[ucPageId - 1][i] <<= 8;
      uiTestPage[ucPageId - 1][i] |= ucRow3[i];
      uiTestPage[ucPageId - 1][i+4] = ucRow4[i];
      uiTestPage[ucPageId - 1][i+4] <<= 8;
      uiTestPage[ucPageId - 1][i+4] |= ucRow2[i];
    }
  }
    else if (ucPageType == INIT_DISP)
  {
    for(i=0; i<4; i++)
    {
      uiInitPage[i] = ucRow1[i];
      uiInitPage[i] <<= 8;
      uiInitPage[i] |= ucRow3[i];
      uiInitPage[i+4] = ucRow4[i];
      uiInitPage[i+4] <<= 8;
      uiInitPage[i+4] |= ucRow2[i];
    }
  }
}
/********************page creation on configuration mode**********************************/
void CreatePagePattConfigDisp(unsigned char ucPageID)
{
  unsigned char ucRow2[4];
  unsigned char ucRow3[4];
 unsigned char ucSingleLED[4] = {CHAR_BLANK, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};

  switch(ucPageID)
  {
    case 1:                                //page 1 for configuration
    {
        CreatePage(CONFIG_DISP, 1, ConF_patts, sel_ConF_patts[reqConfiguration.uiSysConfiguration] , Blank_patts, ucSingleLED);  //C3P3E_patts
      break;
    }
    case 2:                                      //page2 for CTR primary
    {
      unsigned char ucRowLong[8];
      unsigned int i;
      convert_to_dec_patt_signed8((long)reqConfiguration.ulCTRPri, ucRowLong);
      for(i=0; i<4; i++)
      {
              ucRow2[i] = ucRowLong[i];
              ucRow3[i] = ucRowLong[i+4];
      }
      CreatePage(CONFIG_DISP, 2, CTR1_patts, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    case 3:                                //page3 for CTR secondary
    {
      unsigned char ucRowLong[8];
      unsigned int i;
      convert_to_dec_patt_signed8((long)reqConfiguration.ulCTRSec, ucRowLong);
      for(i=0; i<4; i++)
      {
              ucRow2[i] = ucRowLong[i];
              ucRow3[i] = ucRowLong[i+4];
      }
      CreatePage(CONFIG_DISP, 3, CTR2_patts, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    case 4:                                //page4 for PTR primary
    {
      unsigned char ucRowLong[8];
      unsigned int i;
      convert_to_dec_patt_signed8((long)reqConfiguration.ulPTRPri, ucRowLong);
      for(i=0; i<4; i++)
      {
              ucRow2[i] = ucRowLong[i];
              ucRow3[i] = ucRowLong[i+4];
      }
      CreatePage(CONFIG_DISP, 4, PTR1_patts, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    case 5:                                    //page5 for PTR secondary
    {
      unsigned char ucRowLong[8];
      unsigned int i;
      convert_to_dec_patt_signed8((long)reqConfiguration.ulPTRSec, ucRowLong);
      for(i=0; i<4; i++)
      {
              ucRow2[i] = ucRowLong[i];
              ucRow3[i] = ucRowLong[i+4];
      }
      CreatePage(CONFIG_DISP, 5, PTR2_patts, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    case 6:                                    //page 6 for scroll
    {
      if(reqConfiguration.uiScrollON==1)
      {
      convert_to_dec_patt_signed2(reqConfiguration.uiScrollTime, ucRow2);
      ucRow2[2] = 0x00;
      ucRow2[3] = 0x00;
      CreatePage(CONFIG_DISP, 6, ScrlT_patts, ON_patts, ucRow2, ucSingleLED);
      }
      else
      {
      CreatePage(CONFIG_DISP, 6, ScrlT_patts, OF_patts, Blank_patts, ucSingleLED);
      }
      break;
    }
    case 7:                                      // page7 for mod ID
    {
       convert_to_hex_patt(reqConfiguration.uiNetworkID, ucRow2);
      ucRow2[2] = 0x00;
      ucRow2[3] = 0x00;
      CreatePage(CONFIG_DISP, 7, NwkId_patts, ucRow2, Blank_patts, ucSingleLED);
      break;
    }

    /*
     case 8:                                    //page 8 for baud rate
    {
      if(reqConfiguration.uiBaudRate == 1)        //4800
      {
       unsigned char temp[4] = {CHAR_FOUR, (CHAR_EIGHT| 0x80),CHAR_BLANK, CHAR_BLANK};
         CreatePage(CONFIG_DISP, 8, br_patts, temp, Blank_patts, Blank_patts);
      }
      else if(reqConfiguration.uiBaudRate == 2)      //9600
      {
       unsigned char temp[4] = {CHAR_BLANK, CHAR_BLANK,CHAR_NINE, (CHAR_SIX| 0x80)};
        CreatePage(CONFIG_DISP, 8, br_patts, temp, Blank_patts, Blank_patts);
      }
      else if(reqConfiguration.uiBaudRate == 3)     //19200
      {
       unsigned char temp[4]={CHAR_ONE, (CHAR_NINE| 0x80), CHAR_BLANK,CHAR_BLANK};
        CreatePage(CONFIG_DISP, 8, br_patts, Blank_patts, temp, Blank_patts);
      }
      else if(reqConfiguration.uiBaudRate == 4)    //38400
      {
        unsigned char temp[4]={CHAR_BLANK, CHAR_BLANK, CHAR_THREE,(CHAR_EIGHT| 0x80)};
        CreatePage(CONFIG_DISP, 8, br_patts, Blank_patts, temp, Blank_patts);
      }
      else
      {
         unsigned char temp[4] = {CHAR_FOUR, (CHAR_EIGHT| 0x80),CHAR_BLANK, CHAR_BLANK};
         CreatePage(CONFIG_DISP, 8, br_patts, temp, Blank_patts, Blank_patts);

      }
     break;
    }

    */
    case 8:                                    //page 8 for baud rate Shree made changes
    {
      if(reqConfiguration.uiBaudRate == 1)        //4800
      {                                                                                 // by abhijit uncommented to show the 4800 baudrate on 28/08/2015
       unsigned char temp[4] = {CHAR_FOUR, (CHAR_EIGHT| 0x80),CHAR_BLANK, CHAR_BLANK};  // uncommented to show the 4800 baudrate
         CreatePage(CONFIG_DISP, 8, br_patts, temp, Blank_patts, Blank_patts);         // uncommented to show the 4800 baudrate
      }
      else if(reqConfiguration.uiBaudRate == 2)      //9600
      {
       unsigned char temp[4] = {CHAR_NINE,(CHAR_SIX| 0x80),CHAR_BLANK,CHAR_BLANK};     //{CHAR_BLANK, CHAR_BLANK,CHAR_NINE, (CHAR_SIX| 0x80)};
        CreatePage(CONFIG_DISP, 8, br_patts, temp, Blank_patts, Blank_patts);          //CreatePage(CONFIG_DISP, 8, br_patts, temp, Blank_patts, Blank_patts);
      }
      else if(reqConfiguration.uiBaudRate == 3)     //19200
      {
       unsigned char temp[4]={CHAR_BLANK,CHAR_BLANK,CHAR_ONE, (CHAR_NINE| 0x80)};
        CreatePage(CONFIG_DISP, 8, br_patts, temp, Blank_patts, Blank_patts);     //CreatePage(CONFIG_DISP, 8, br_patts, Blank_patts, temp, Blank_patts);
      }
//////      else if(reqConfiguration.uiBaudRate == 4)    //38400
//////      {
//////        unsigned char temp[4]={CHAR_THREE,(CHAR_EIGHT| 0x80),CHAR_BLANK, CHAR_BLANK}; ///commented  by abhijit                                                                          commented
//////        CreatePage(CONFIG_DISP, 8, br_patts, Blank_patts, temp, Blank_patts);       // CreatePage(CONFIG_DISP, 8, br_patts, Blank_patts, temp, Blank_patts);
//////      }
      else
      {
         unsigned char temp[4] = {CHAR_FOUR, (CHAR_EIGHT| 0x80),CHAR_BLANK, CHAR_BLANK};
         CreatePage(CONFIG_DISP, 8, br_patts, temp, Blank_patts, Blank_patts);
      }
      Reset_Energy_flag=1;
     break;
    }
   case 9:
      if (Reset_Energy_flag==0)
            CreatePage(CONFIG_DISP, 9, reset_Energy_patts, Y_patts, Blank_patts, ucSingleLED);
      else
         CreatePage(CONFIG_DISP, 9, reset_Energy_patts, N_patts, Blank_patts, ucSingleLED);
           break;
    case 10:
        CreatePage(CONFIG_DISP, 10, Led_patts, Test_patts, Blank_patts, ucSingleLED);

      break;

    case 11 :
            CreatePage(CONFIG_DISP, 11, Exit_patts, Blank_patts, Blank_patts, Blank_patts);
         break;

    default:
    {
      break;
    }
  }
}

/*******************Data Conversion Functions***************************************************/

void convert_to_dec_patt_signed8(signed long val, unsigned char * patts)
{
    int n;
    long x;

    unsigned int uiInd;

     if (val < 0)
     {
        patts[0] = CHAR_MINUS;
        if(val < -9999999)
          val = -9999999;
        val = (val * -1);
        for (n=0; n<7; n++)
        {
             x = pow_10[6-n];
             patts[n+1] = hex_digit_patts[val/x];
             val = val%x;
        }
     }
     else
     {
        if(val > 99999999)
          val = 99999999;
        for (n=0; n<8; n++)
        {
          x = pow_10[7-n];

             uiInd = (unsigned int)(val/x);
             patts[n] = hex_digit_patts[uiInd];
          val = (int) (val%x);
        }
     }
}
void convert_to_dec_patt_signed_zero(signed int val, unsigned char * patts)
{
   int n;
   long x;
   unsigned int uiInd;
   if (val < 0)
   {
      val =0;
     for (n=0; n < 4; n++)
      {
           x = pow_10[2-n];
           uiInd = (unsigned int)(val/x);
           patts[n+1] = hex_digit_patts[uiInd];
           val = val%x;
      }
   }
   else
   {
      if(val > 9999)
        val = 9999;
      for (n=0; n<4; n++)
      {
        x = pow_10[3-n];
           uiInd = (unsigned int)(val/x);
           patts[n] = hex_digit_patts[uiInd];
        val = (int) (val%x);
      }
   }
}
void convert_to_dec_patt_signed_pf(signed int val, unsigned char * patts)
{
   int n;
   long x;
   unsigned int uiInd;
   if (val < 0)
   {
      patts[0] = CHAR_MINUS;
     // if(val < -999)
     //   val = -999;
      val = (val * -1);
 // val =0;
     for (n=0; n < 3; n++)
      {
           x = pow_10[2-n];
           uiInd = (unsigned int)(val/x);
           patts[n+1] = hex_digit_patts[uiInd];
           val = val%x;
      }
   }
   else
   {
      if(val > 9999)
        val = 9999;
      for (n=0; n<4; n++)
      {
        x = pow_10[3-n];
           uiInd = (unsigned int)(val/x);
           patts[n] = hex_digit_patts[uiInd];
        val = (int) (val%x);
      }
   }
}
void convert_to_dec_patt_signed(signed int val, unsigned char * patts)
{
   int n;
   long x;
   unsigned int uiInd;
   if (val < 0)
   {
      patts[0] = CHAR_MINUS;
     // if(val < -999)
     //   val = -999;
      val = (val * -1);
 // val =0;
     for (n=0; n < 3; n++)
      {
           x = pow_10[2-n];
           uiInd = (unsigned int)(val/x);
           patts[n+1] = hex_digit_patts[uiInd];
           val = val%x;
      }
   }
   else
   {
     if(val == 0){
       patts[0] = hex_digit_patts[0];
       patts[1] = hex_digit_patts[0];
       patts[2] = hex_digit_patts[0];
       patts[3] = hex_digit_patts[0];
     }
     else{
      if(val > 9999)
        val = 9999;
      for (n=0; n<4; n++)
      {
        x = pow_10[3-n];
           uiInd = (unsigned int)(val/x);
           patts[n] = hex_digit_patts[uiInd];
        val = (int) (val%x);
      }
     }
   }
}
void convert_to_dec_patt_signed2(signed int val, unsigned char * patts)
{
   int n;
   long x;
   unsigned int uiInd;

      for (n=0; n<2; n++)
      {
        x = pow_10[1-n];
        uiInd = (unsigned int)(val/x);
        patts[n] = hex_digit_patts[uiInd];
        val = (int) (val%x);
      }
}
void convert_to_dec_patt_signed3(signed int val, unsigned char * patts)
{
   int n;
   long x;
   unsigned int uiInd;
      if(val > 999)
        val = 999;
      for (n=0; n<3; n++)
      {
        x = pow_10[2-n];
           uiInd = (unsigned int)(val/x);
           patts[n] = hex_digit_patts[uiInd];
        val = (int) (val%x);
      }
}
void convert_to_hex_patt(int val, unsigned char *patts)
{
   int n;
   for(n=0; n<2; n++)
   {
      patts[1-n] = hex_digit_patts[val & 0x0f];

      val = val >> 4;
   }
}
int Floatpatt4_pf(float fInVlaue, unsigned char * ucIndex)
{
	char ConvertBuff[50];
        float flPower = 1.0;
	unsigned char i, ucMulVal = 0;
	*ucIndex = 0;
        if(fInVlaue < 0)
        {
          if(fnabs(fInVlaue) < 0.01)
            fInVlaue=0;
        }
	sprintf(ConvertBuff,"%f",fInVlaue);
	for(i=0; (ConvertBuff[i] != '.') && (i < strlen(ConvertBuff)); i++)
	{
		*ucIndex =  *ucIndex + 1;
	}
        if(fInVlaue >= 0)
        {
          for( ; i < 4; i++)
          {
                  ucMulVal++;
                  flPower *= 10;
          }
        }
        else if(fInVlaue < 0)
        {
          if ( fInVlaue > -100)
          {
            for( ; i < 3; i++)
            {
                    ucMulVal++;
                    flPower *= 10;
            }
              flPower *= 10;
          }
          else
          {
            for( ; i < 3; i++)
            {
                    ucMulVal++;
                    flPower *= 10;
            }
          }
        }
	fInVlaue = (fInVlaue) * (flPower);

  	*ucIndex = *ucIndex - 1;

	return((int)fInVlaue);
}
int Floatpatt4(float fInVlaue, unsigned char * ucIndex)
{
	char ConvertBuff[50];
        float flPower = 1.0;
	unsigned char i, ucMulVal = 0;
	*ucIndex = 0;
        if(fInVlaue < 0)
        {
          if(fnabs(fInVlaue) < 0.01)
            fInVlaue=0;
        }
	sprintf(ConvertBuff,"%f",fInVlaue);
	for(i=0; (ConvertBuff[i] != '.') && (i < strlen(ConvertBuff)); i++)
	{
		*ucIndex =  *ucIndex + 1;
	}
        if(fInVlaue >= 0)
        {
          for( ; i < 4; i++)
          {
                  ucMulVal++;
                  flPower *= 10;
          }
        }
        else if(fInVlaue < 0)
        {
          if ( fInVlaue > -100)
          {
            for( ; i < 3; i++)
            {
                    ucMulVal++;
                    flPower *= 10;
            }
              flPower *= 10;
          }
          else if( (fInVlaue < -100) && (fInVlaue > -1000))
          {
            for( ; i < 4; i++)
            {
                    ucMulVal++;
                    flPower *= 10;
            }

          }
        /*  else
          {
            flPower *= 0.10;
            *ucIndex = *ucIndex - 3;
          }*/
        }
	fInVlaue = (fInVlaue) * (flPower);

  	*ucIndex = *ucIndex - 1;

	return((int)fInVlaue);
}

long Floatpatt8(float fInVlaue, unsigned char * ucIndex)
{
	char ConvertBuff[40];
        float flPower = 1.0;
	unsigned char i, ucMulVal = 0;
	*ucIndex = 0;
	sprintf(ConvertBuff,"%f",fInVlaue);
	for(i=0; (ConvertBuff[i] != '.') && (i < strlen(ConvertBuff)); i++)
	{
		*ucIndex = *ucIndex + 1;
	}
        if(fInVlaue >= 0)
        {
          for( ; i < 8; i++)
          {
                  ucMulVal++;
                  flPower *= 10;
          }
        }
        else if(fInVlaue < 0)
        {
          if ( fInVlaue > -1000000)
          {
            for( ; i < 7; i++)
            {
                  ucMulVal++;
                  flPower *= 10;
            }
            flPower *= 10;
          }
          else
          {
            for( ; i < 7; i++)
            {
                    ucMulVal++;
                    flPower *= 10;
            }
          }
        }

        fInVlaue = (fInVlaue) * (flPower);
	*ucIndex = *ucIndex - 1;
return((long)fInVlaue);
}

/****************Para disply****************************/
void CreatePagePattParamDisp(unsigned char ucPageID) //IANET: 24/04/2024
{
  ucSingleLED[0] = 0x00;
  ucSingleLED[1] = 0x00;
  ucSingleLED[2] = 0x00;
  ucSingleLED[3] = 0x00;
  
   if(fnabs(reqMeasurepara.flKWTot) <= 9999000000)
      {
          if(fnabs(reqMeasurepara.flKWTot) > 1000000)
          {
                  uiPrintval = Floatpatt4((reqMeasurepara.flKWTot/1000000), &ucIndex);
                  ucSingleLED[2] |= 0x01;
          }
          else if(fnabs(reqMeasurepara.flKWTot) > 1000)
          {
                  uiPrintval = Floatpatt4((reqMeasurepara.flKWTot/1000), &ucIndex);
                  ucSingleLED[0] |= 0x01;
          }
          else
          {
                uiPrintval = Floatpatt4(reqMeasurepara.flKWTot, &ucIndex);
          }

          convert_to_dec_patt_signed(uiPrintval, ucRow1);
           if(ucIndex==3)
            {
             ucRow1[ucIndex] = ucRow1[ucIndex] ;
            }
            else
            {
              ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
            }
      }
      else
      {
        for(i=0; i<4; i++)
        { ucRow1[i] = OF_patts[i]; }
      }
  
  
  
  //kvar 
      if(fnabs(reqMeasurepara.flVARTot) <= 9999000000)
      {
        if(fnabs(reqMeasurepara.flVARTot) > 1000000)
        {
                uiPrintval = Floatpatt4((reqMeasurepara.flVARTot/1000000), &ucIndex);
                ucSingleLED[0] |= 0x40;
        }
        else if(fnabs(reqMeasurepara.flVARTot) > 1000)
        {
                uiPrintval = Floatpatt4((reqMeasurepara.flVARTot/1000), &ucIndex);
                ucSingleLED[2] |= 0x80;
        }
        else
        {
                uiPrintval = Floatpatt4(reqMeasurepara.flVARTot, &ucIndex);
        }
        convert_to_dec_patt_signed(uiPrintval, ucRow2);
           if(ucIndex==3)
            {
             ucRow2[ucIndex] = ucRow2[ucIndex] ;
            }
            else
            {
              ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
            }
      }
      else
      {
        for(i=0; i<4; i++)
        { ucRow2[i] = OF_patts[i]; }
      }
  
            /*-------------For PFavg----------------------------------*/
          //if(pf_error==1)  // commented
          if((pf_error==1)||(reqMeasurepara.flIavg==0))    // edited by abhijit mali on 04/06/2015
         {
           for(i=0;i<4;i++)
           {
             ucRow3[i] = Err_patts[i] ;
           }
         }
         else
         {
           uiPrintval = Floatpatt4_pf(reqMeasurepara.flPFavg, &ucIndex);
           convert_to_dec_patt_signed_pf(uiPrintval, ucRow3);

           if(ucIndex==3)
            {
             ucRow3[ucIndex] = ucRow3[ucIndex] ;
            }
            else
            {
              ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
            }
        }
       for(i=0;i<100;i++);
/*
       ucSingleLED[0]  |= 0x02; //02
       ucSingleLED[1]  |= 0x00; // 00
       ucSingleLED[2]  |= 0x04; // 04
       ucSingleLED[3]  |= 0x02; // 02
*/
      
      
        for(i=0;i<100;i++);
      CreatePage(PARAM_DISP, 1, ucRow1, ucRow2, ucRow3, ucSingleLED);
  
  
}



/*
void CreatePagePattParamDisp(unsigned char ucPageID)
{
   ucPage = ucPageID;
   ucSingleLED[0] = 0x00;
   ucSingleLED[1] = 0x00;
   ucSingleLED[2] = 0x00;
   ucSingleLED[3] = 0x00;

  switch(ucPage)
  {
     //--------------PAGE1 Vavg,Iavg,PFavg----------
    case 1:
    {
      //signed int uiPrintval;
      // Need to check System configuration here observed on dtd 05 Aug 2013 
      if(reqConfiguration.uiSysConfiguration==3) // 1P1E
      {
        if(fnabs(reqMeasurepara.flVlnavg) <= 9999000000 )
        {
          if(fnabs(reqMeasurepara.flVlnavg) > 999999 )
          {
                  uiPrintval = Floatpatt4((reqMeasurepara.flVlnavg/1000000), &ucIndex);
                  ucSingleLED[2] |= 0x01;
          }
          else if(fnabs(reqMeasurepara.flVlnavg) > 9999)
          {
              uiPrintval = Floatpatt4((reqMeasurepara.flVlnavg/1000), &ucIndex);
              ucSingleLED[0] |= 0x01;
          }
          else
          {
             uiPrintval = Floatpatt4(reqMeasurepara.flVlnavg, &ucIndex);
          }
             convert_to_dec_patt_signed(uiPrintval, ucRow1);
             ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
        }
        else
         {
            for(i=0;i<4;i++)
            {
              ucRow1[i] = OF_patts[i] ;
            }
         }
      }

      else                                                                      // if (reqConfiguration.uiSysConfiguration==3) // 1P1E
      {
           //-------------For Vavg ----------------------------------
          if(fnabs(reqMeasurepara.flVllavg) <= 9999000000 )
          {
              if(fnabs(reqMeasurepara.flVllavg) > 999999 )
              {
                      uiPrintval = Floatpatt4((reqMeasurepara.flVllavg/1000000), &ucIndex);
                      ucSingleLED[2] |= 0x01;	                             //M led for Vavg
              }
              else if(fnabs(reqMeasurepara.flVllavg) > 9999)
              {
                      uiPrintval = Floatpatt4((reqMeasurepara.flVllavg/1000), &ucIndex);

                      ucSingleLED[0] |= 0x01;                                //K led for Vavg
              }
              else
              {
                    uiPrintval = Floatpatt4(reqMeasurepara.flVllavg, &ucIndex);
                    ucSingleLED[0] = 0x02;

              }
               convert_to_dec_patt_signed(uiPrintval, ucRow1);

              if(ucIndex==3)
              {
               ucRow1[ucIndex] = ucRow1[ucIndex] ;
              }
              else
              {
                ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
              }
          }
          else
           {
                  for(i=0;i<4;i++)
                  {
                    ucRow1[i] = OF_patts[i] ;
                  }
           }
        }
        for(i=0;i<100;i++);

       //-------------For Iavg----------------------------------
        if(fnabs(reqMeasurepara.flIavg) > 999999 )
        {
                uiPrintval = Floatpatt4((reqMeasurepara.flIavg/1000000), &ucIndex);
               ucSingleLED[0] |= 0x40;	             //M led
        }
        else if(fnabs(reqMeasurepara.flIavg) > 9999)
        {
                uiPrintval = Floatpatt4((reqMeasurepara.flIavg/1000), &ucIndex);
                ucSingleLED[2] |= 0x80;           //k led
        }
        else
        {
          uiPrintval = Floatpatt4(reqMeasurepara.flIavg, &ucIndex);
        }
         convert_to_dec_patt_signed_zero(uiPrintval, ucRow2);
             if(ucIndex==3)
            {
             ucRow2[ucIndex] = ucRow2[ucIndex] ;
            }
            else
            {
              ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
            }
          for(i=0;i<100;i++);
                //-------------For PFavg----------------------------------
          //if(pf_error==1)  // commented
          if((pf_error==1)||(reqMeasurepara.flIavg==0))    // edited by abhijit mali on 04/06/2015
         {
           for(i=0;i<4;i++)
           {
             ucRow3[i] = Err_patts[i] ;
           }
         }
         else
         {
           uiPrintval = Floatpatt4_pf(reqMeasurepara.flPFavg, &ucIndex);
           convert_to_dec_patt_signed_pf(uiPrintval, ucRow3);

           if(ucIndex==3)
            {
             ucRow3[ucIndex] = ucRow3[ucIndex] ;
            }
            else
            {
              ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
            }
        }
       for(i=0;i<100;i++);
       ucSingleLED[0]  |= 0x02; //02
       ucSingleLED[1]  |= 0x00; // 00
       ucSingleLED[2]  |= 0x04; // 04
       ucSingleLED[3]  |= 0x02; // 02

      for(i=0;i<100;i++);
      CreatePage(PARAM_DISP, 1, ucRow1, ucRow2, ucRow3, ucSingleLED);

      break;
    }
     //-------------PAGE2----TOTAL W,VAR,VA----------
    case 2:
    {
     // signed int uiPrintval;

      if(fnabs(reqMeasurepara.flKWTot) <= 9999000000)
      {
          if(fnabs(reqMeasurepara.flKWTot) > 1000000)
          {
                  uiPrintval = Floatpatt4((reqMeasurepara.flKWTot/1000000), &ucIndex);
                  ucSingleLED[2] |= 0x01;
          }
          else if(fnabs(reqMeasurepara.flKWTot) > 1000)
          {
                  uiPrintval = Floatpatt4((reqMeasurepara.flKWTot/1000), &ucIndex);
                  ucSingleLED[0] |= 0x01;
          }
          else
          {
                uiPrintval = Floatpatt4(reqMeasurepara.flKWTot, &ucIndex);
          }

          convert_to_dec_patt_signed(uiPrintval, ucRow1);
           if(ucIndex==3)
            {
             ucRow1[ucIndex] = ucRow1[ucIndex] ;
            }
            else
            {
              ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
            }
      }
      else
      {
        for(i=0; i<4; i++)
        { ucRow1[i] = OF_patts[i]; }
      }
      if(fnabs(reqMeasurepara.flVARTot) <= 9999000000)
      {
        if(fnabs(reqMeasurepara.flVARTot) > 1000000)
        {
                uiPrintval = Floatpatt4((reqMeasurepara.flVARTot/1000000), &ucIndex);
                ucSingleLED[0] |= 0x40;
        }
        else if(fnabs(reqMeasurepara.flVARTot) > 1000)
        {
                uiPrintval = Floatpatt4((reqMeasurepara.flVARTot/1000), &ucIndex);
                ucSingleLED[2] |= 0x80;
        }
        else
        {
                uiPrintval = Floatpatt4(reqMeasurepara.flVARTot, &ucIndex);
        }
        convert_to_dec_patt_signed(uiPrintval, ucRow2);
           if(ucIndex==3)
            {
             ucRow2[ucIndex] = ucRow2[ucIndex] ;
            }
            else
            {
              ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
            }
      }
      else
      {
        for(i=0; i<4; i++)
        { ucRow2[i] = OF_patts[i]; }
      }
      for(i=0;i<100;i++);
      if(fnabs(reqMeasurepara.flKVATot) <= 9999000000)
      {
          if(fnabs(reqMeasurepara.flKVATot) > 1000000)
          {
                  uiPrintval = Floatpatt4((reqMeasurepara.flKVATot/1000000), &ucIndex);
                  ucSingleLED[2] |= 0x20;
          }
          else if(fnabs(reqMeasurepara.flKVATot) > 1000)
          {
                  uiPrintval = Floatpatt4((reqMeasurepara.flKVATot/1000), &ucIndex);
                  ucSingleLED[0] |= 0x20;
          }
          else
          {
                  uiPrintval = Floatpatt4(reqMeasurepara.flKVATot, &ucIndex);
          }
          convert_to_dec_patt_signed(uiPrintval, ucRow3);
           if(ucIndex==3)
            {
             ucRow3[ucIndex] = ucRow3[ucIndex] ;
            }
            else
            {
              ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
            }
      }
      else
      {
        for(i=0; i<4; i++)
        { ucRow3[i] = OF_patts[i]; }
      }
      for(i=0;i<100;i++);
      ucSingleLED[0]|= 0x90;
      ucSingleLED[1]|= 0x00;
      ucSingleLED[2] |= 0x40;
      ucSingleLED[3] |= 0x00;
      CreatePage(PARAM_DISP, 2, ucRow1, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    //---------------------PAGE3---Vr,Vy,Vb-----------------------------
    case 3:
    {
    //  signed int uiPrintval;
        //-----Vrn--------
   	if(fnabs(reqMeasurepara.flVrn) <= 9999000000)
           {
              if(fnabs(reqMeasurepara.flVrn) > 999999 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flVrn/1000000), &ucIndex);
                        ucSingleLED[2] |= 0x01;	     //M1 led
                }
             else if((reqMeasurepara.flVrn) > 9999)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flVrn/1000), &ucIndex);
                 ucSingleLED[0] |= 0x01;	 //K1 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVrn, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow1);
               if(ucIndex==3)
                {
                 ucRow1[ucIndex] = ucRow1[ucIndex] ;
                }
                else
                {
                  ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
                }
           }
           else
            {
                for(i=0;i<4;i++)
                {
                  ucRow1[i] = OF_patts[i] ;
                }
            }
         //-----Vyn--------
        if(reqConfiguration.uiSysConfiguration==1)     //3p2e
        {
          for(i=0;i<4;i++)                                 //blank Vyn
                {
                  ucRow2[i] = blank_patts[i] ;
                }
        }
        else
        {
          if(fnabs(reqMeasurepara.flVyn) <= 9999000000)
          {
              if(fnabs(reqMeasurepara.flVyn) > 999999 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flVyn/1000000), &ucIndex);
                        ucSingleLED[0] |= 0x40;	//M2 led
                }
             else if((reqMeasurepara.flVyn) > 9999)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flVyn/1000), &ucIndex);
                 ucSingleLED[2] |= 0x80;	 //K2 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVyn, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow2);
                if(ucIndex==3)
                {
                 ucRow2[ucIndex] = ucRow2[ucIndex] ;
                }
                else
                {
                  ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
                }
          }
          else
          {
              for(i=0;i<4;i++)
              {
                ucRow2[i] = OF_patts[i] ;
              }
          }
        }
         //---------Vbn----------
          if(fnabs(reqMeasurepara.flVbn) <= 9999000000)
          {
              if(fnabs(reqMeasurepara.flVbn) > 999999 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flVbn/1000000), &ucIndex);
                        ucSingleLED[2] |= 0x20;	    //M3 led
                }
             else if((reqMeasurepara.flVbn) > 9999)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flVbn/1000), &ucIndex);
                 ucSingleLED[0] |= 0x20;	 //K3 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVbn, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow3);
               if(ucIndex==3)
                {
                 ucRow3[ucIndex] = ucRow3[ucIndex] ;
                }
                else
                {
                  ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
                }
          }
          else
            {
                for(i=0;i<4;i++)
                {
                  ucRow3[i] = OF_patts[i] ;
                }
            }
       if((reqConfiguration.uiSysConfiguration==0)||(reqConfiguration.uiSysConfiguration==1)||(reqConfiguration.uiSysConfiguration==2))
       {
        ucSingleLED[0] |= (0x02 | 0x04);
        ucSingleLED[1] |= 0x02;
        ucSingleLED[2]|= 0x00;
        ucSingleLED[3]|= 0x04;
       }

       if(reqConfiguration.uiSysConfiguration==3)     //3P1E
      {
         for(i=0;i<4;i++)                                 //blank Vyn
                {
                  ucRow2[i] = blank_patts[i] ;
                }

         for(i=0;i<4;i++)                              //blank Vbn
                {
                  ucRow3[i] = blank_patts[i] ;
                }

        ucSingleLED[0] |= 0x02 ;  	  //only R phase voltage pattern
        ucSingleLED[1] |= 0x02;
        ucSingleLED[2]|= 0x00;
        ucSingleLED[3]|= 0x00;
      }


    CreatePage(PARAM_DISP, 3, ucRow1, ucRow2, ucRow3, ucSingleLED);
        break;
    }

 //------------------PAGE4  for vry,vyb,vbr-------------------------

    case 4:
    {
     // signed int uiPrintval;

         //------Vry------
      //if(reqMeasurepara.flVry != 0){
          if(fnabs(reqMeasurepara.flVry) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flVry) > 999999 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flVry/1000000), &ucIndex);
                        ucSingleLED[2] |= 0x01;	      //M1 led
                }
              else if(fnabs(reqMeasurepara.flVry) > 9999)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flVry/1000), &ucIndex);
                 ucSingleLED[0] |= 0x01;	 //K1 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVry, &ucIndex);
              }
            //}
          //else{
            //uiPrintval = Floatpatt4(0, &ucIndex);
          //}

             convert_to_dec_patt_signed(uiPrintval, ucRow1);
                //if(ucIndex==3)
                //{
                 //ucRow1[ucIndex] = ucRow1[ucIndex] ;
                //}
                if(ucIndex!=3)
                {
                  ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0;i<4;i++)
              {
                ucRow1[i] = OF_patts[i] ;
              }
            }
   //------Vyb------
          if(fnabs(reqMeasurepara.flVyb) <= 9999000000 )
            {

              if(fnabs(reqMeasurepara.flVyb) > 999999 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flVyb/1000000), &ucIndex);
                        ucSingleLED[0] |= 0x40;	             //M2 led
                }
              else if(fnabs(reqMeasurepara.flVyb) > 9999)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flVyb/1000), &ucIndex);
                 ucSingleLED[2] |= 0x80;	        //K2 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVyb, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow2);
                if(ucIndex==3)
                {
                 ucRow2[ucIndex] = ucRow2[ucIndex] ;
                }
                else
                {
                  ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0;i<4;i++)
              {
                ucRow2[i] = OF_patts[i] ;
              }
            }
   //------Vbr------
            if(fnabs(reqMeasurepara.flVbr) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flVbr) > 999999 )
              {
                      uiPrintval = Floatpatt4((reqMeasurepara.flVbr/1000000), &ucIndex);
                      ucSingleLED[2] |= 0x20;	  //M3 led
              }
              else if(fnabs(reqMeasurepara.flVbr) > 9999)
              {
                uiPrintval = Floatpatt4((reqMeasurepara.flVbr/1000), &ucIndex);
                ucSingleLED[0] |= 0x20;	          //K3 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVbr, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow3);
               if(ucIndex==3)
                {
                 ucRow3[ucIndex] = ucRow3[ucIndex] ;
                }
                else
                {
                  ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0 ;i < 4; i++)
              {
                ucRow3[i] = OF_patts[i] ;
              }
           }

          if ((reqConfiguration.uiSysConfiguration==0)|| (reqConfiguration.uiSysConfiguration==2))
           {
        ucSingleLED[0] |= 0x0A;
        ucSingleLED[1] |= 0x04;
        ucSingleLED[2]|= 0x02;
        ucSingleLED[3]|= 0x00;
            }

       if (reqConfiguration.uiSysConfiguration==1)               //3P2E
       {       for(i=0;i<4;i++)                              //blank Vbr
                {
                  ucRow3[i] = blank_patts[i] ;
                }

        ucSingleLED[0]|= 0x02;  	 //only Vry and and Vyb pattern........
        ucSingleLED[1]|= 0x04;
        ucSingleLED[2]|= 0x02;
        ucSingleLED[3]|= 0x00;
       }
       CreatePage(PARAM_DISP, 4, ucRow1, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    //-------------PAGE5 for Ir,Iy,Ib----------------------------
    case 5:
    {
     // signed int uiPrintval;
        //---------------Ir---------------
            if(fnabs(reqMeasurepara.flIr) > 999999 )
            {
                    uiPrintval = Floatpatt4((reqMeasurepara.flIr/1000000), &ucIndex);
                    ucSingleLED[2] |= 0x01;
            }
            else if(fnabs(reqMeasurepara.flIr) > 9999)
            {
              uiPrintval = Floatpatt4((reqMeasurepara.flIr/1000), &ucIndex);
              ucSingleLED[0] |= 0x01;
            }
            else
            {
              uiPrintval = Floatpatt4(reqMeasurepara.flIr, &ucIndex);
            }
            convert_to_dec_patt_signed(uiPrintval, ucRow1);
           if(ucIndex==3)
            {
             ucRow1[ucIndex] = ucRow1[ucIndex] ;
            }
            else
            {
              ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
            }
  //---------------Iy---------------
          if(fnabs(reqMeasurepara.flIy) > 999999 )
            {
                    uiPrintval = Floatpatt4((reqMeasurepara.flIy/1000000), &ucIndex);
                    ucSingleLED[0] |= 0x40;
            }
            else if(fnabs(reqMeasurepara.flIy) > 9999)
            {
              uiPrintval = Floatpatt4((reqMeasurepara.flIy/1000), &ucIndex);
              ucSingleLED[2] |= 0x80;
            }
            else
            {
              uiPrintval = Floatpatt4(reqMeasurepara.flIy, &ucIndex);
            }

          convert_to_dec_patt_signed(uiPrintval, ucRow2);
           if(ucIndex==3)
            {
             ucRow2[ucIndex] = ucRow2[ucIndex] ;
            }
            else
            {
              ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
            }

  //---------------Ib---------------
          if(fnabs(reqMeasurepara.flIb) > 999999 )
            {
                    uiPrintval = Floatpatt4((reqMeasurepara.flIb/1000000), &ucIndex);
                    ucSingleLED[2] |= 0x20;
            }
            else if(fnabs(reqMeasurepara.flIb) > 9999)
            {
              uiPrintval = Floatpatt4((reqMeasurepara.flIb/1000), &ucIndex);
              ucSingleLED[0] |= 0x20;
            }
            else
            {
              uiPrintval = Floatpatt4(reqMeasurepara.flIb, &ucIndex);
            }
          convert_to_dec_patt_signed(uiPrintval, ucRow3);
           if(ucIndex==3)
            {
             ucRow3[ucIndex] = ucRow3[ucIndex] ;
            }
            else
            {
              ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
            }


         if( reqConfiguration.uiSysConfiguration==0)
         {
          ucSingleLED[0] |= 0x04;
          ucSingleLED[1] |= 0x02;
          ucSingleLED[2] |= 0x00;
          ucSingleLED[3] |= 0x06;
          }

      if( reqConfiguration.uiSysConfiguration==1)    //3P2E
      {
        for(i=0;i<4;i++)                              //blank Iy blank
        {
          ucRow2[i] = blank_patts[i] ;
        }
        ucSingleLED[0] |= 0x00;             //pattern for Iy and Ib
        ucSingleLED[1] |= 0x02;
        ucSingleLED[2] |= 0x00;
        ucSingleLED[3] |= 0x06;

      }
      else if((reqConfiguration.uiSysConfiguration==2)||( reqConfiguration.uiSysConfiguration==3))    //3P1E or 1P1E
      {

              for(i=0;i<4;i++)                              //blank Iy blank
                {
                  ucRow2[i] = blank_patts[i] ;
                }
                for(i=0;i<4;i++)                              //blank Ib blank
                {
                  ucRow3[i] = blank_patts[i] ;
                }


      ucSingleLED[0] |= 0x00;             //pattern for Ir aonly
      ucSingleLED[1] |= 0x02;
      ucSingleLED[2] |= 0x00;
      ucSingleLED[3] |= 0x02;
       }

       CreatePage(PARAM_DISP, 5, ucRow1, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    //-----------PAGE 6 Wr,Wy,Wb---------------------------------
    case 6:
    {
    //  signed int uiPrintval;
         //--------------Wr-------------------
            if(fnabs(reqMeasurepara.flKWr) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flKWr) > 1000000 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flKWr/1000000), &ucIndex);
                        ucSingleLED[2] |= 0x01;	      //M1 led
                }
              else if(fnabs(reqMeasurepara.flKWr) > 1000)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flKWr/1000), &ucIndex);
                 ucSingleLED[0] |= 0x01;	 //K1 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flKWr, &ucIndex);
              }
             convert_to_dec_patt_signed(uiPrintval, ucRow1);
             if(ucIndex==3)
            {
             ucRow1[ucIndex] = ucRow1[ucIndex] ;
            }
            else
            {
              ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
            }
            }
            else
            {
              for(i=0;i<4;i++)
              {
                ucRow1[i] = OF_patts[i] ;
              }
            }
            //--------------Wy-------------------
          if(fnabs(reqMeasurepara.flKWy) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flKWy) > 1000000 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flKWy/1000000), &ucIndex);
                        ucSingleLED[0] |= 0x40;	             //M2 led
                }
              else if(fnabs(reqMeasurepara.flKWy) > 1000)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flKWy/1000), &ucIndex);
                 ucSingleLED[2] |= 0x80;	        //K2 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flKWy, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow2);
                if(ucIndex==3)
                {
                 ucRow2[ucIndex] = ucRow2[ucIndex] ;
                }
                else
                {
                  ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0;i<4;i++)
              {
                ucRow2[i] = OF_patts[i] ;
              }
            }
             //--------------Wb-------------------
            if(fnabs(reqMeasurepara.flKWb) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flKWb) > 1000000 )
              {
                      uiPrintval = Floatpatt4((reqMeasurepara.flKWb/1000000), &ucIndex);
                      ucSingleLED[2] |= 0x20;	  //M3 led
              }
              else if(fnabs(reqMeasurepara.flKWb) > 1000)
              {
                uiPrintval = Floatpatt4((reqMeasurepara.flKWb/1000), &ucIndex);
                ucSingleLED[0] |= 0x20;	          //K3 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flKWb, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow3);
                if(ucIndex==3)
                {
                 ucRow3[ucIndex] = ucRow3[ucIndex] ;
                }
                else
                {
                  ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0 ;i < 4; i++)
              {
                ucRow3[i] = OF_patts[i] ;
              }
            }
         if(reqConfiguration.uiSysConfiguration==0)
         {
          ucSingleLED[0]|= 0x84;
          ucSingleLED[1]|= 0x02;
          ucSingleLED[2]|= 0x00;
          ucSingleLED[3]|= 0x04;
         }
     if(reqConfiguration.uiSysConfiguration==1)
     {
                for(i=0;i<4;i++)                              //blank Wy blank
                {
                  ucRow2[i] = blank_patts[i] ;
                }
      ucSingleLED[0]|= 0x80;
      ucSingleLED[1]|= 0x02;
      ucSingleLED[2]|= 0x00;
      ucSingleLED[3]|= 0x04;
     }
     else if ((reqConfiguration.uiSysConfiguration==2)||(reqConfiguration.uiSysConfiguration==3))   //3P1E or 1P1E
     {
              for(i=0;i<4;i++)                              //blankWy blank
                {
                  ucRow2[i] = blank_patts[i] ;
                }
                for(i=0;i<4;i++)                              //blank Wb blank
                {
                  ucRow3[i] = blank_patts[i] ;
                }
      ucSingleLED[0]|= 0x80;
      ucSingleLED[1]|= 0x02;
      ucSingleLED[2]|= 0x00;
      ucSingleLED[3]|= 0x00;
     }

       CreatePage(PARAM_DISP, 6, ucRow1, ucRow2, ucRow3, ucSingleLED);
      break;
      }
    /-------------PAGE 7 Varr,Vary,Varb--------------------------
    case 7:
    {
     // signed int uiPrintval;
           if(fnabs(reqMeasurepara.flVARr) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flVARr) >1000000 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flVARr/1000000), &ucIndex);
                        ucSingleLED[2] |= 0x01;	      //M1 led
                }
              else if(fnabs(reqMeasurepara.flVARr) > 1000)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flVARr/1000), &ucIndex);
                 ucSingleLED[0] |= 0x01;	 //K1 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVARr, &ucIndex);
              }
             convert_to_dec_patt_signed(uiPrintval, ucRow1);
              if(ucIndex==3)
                {
                 ucRow1[ucIndex] = ucRow1[ucIndex] ;
                }
                else
                {
                  ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0;i<4;i++)
              {
                ucRow1[i] = OF_patts[i] ;
              }
            }
          if(fnabs(reqMeasurepara.flVARy) <= 9999000000 )
            {

              if(fnabs(reqMeasurepara.flVARy) > 1000000)
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flVARy/1000000), &ucIndex);
                        ucSingleLED[0] |= 0x40;	             //M2 led
                }
              else if(fnabs(reqMeasurepara.flVARy) > 1000)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flVARy/1000), &ucIndex);
                 ucSingleLED[2] |= 0x80;	        //K2 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVARy, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow2);
               if(ucIndex==3)
                {
                 ucRow2[ucIndex] = ucRow2[ucIndex] ;
                }
                else
                {
                  ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0;i<4;i++)
              {
                ucRow2[i] = OF_patts[i] ;
              }
            }
            if(fnabs(reqMeasurepara.flVARb) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flVARb) > 1000000 )
              {
                      uiPrintval = Floatpatt4((reqMeasurepara.flVARb/1000000), &ucIndex);
                      ucSingleLED[2] |= 0x20;	  //M3 led
              }
              else if(fnabs(reqMeasurepara.flVARb) > 1000)
              {
                uiPrintval = Floatpatt4((reqMeasurepara.flVARb/1000), &ucIndex);
                ucSingleLED[0] |= 0x20;	          //K3 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flVARb, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow3);
                if(ucIndex==3)
                {
                 ucRow3[ucIndex] = ucRow3[ucIndex] ;
                }
                else
                {
                  ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0 ;i < 4; i++)
              {
                ucRow3[i] = OF_patts[i] ;
              }
            }

       if(reqConfiguration.uiSysConfiguration==0)
         {
      ucSingleLED[0]|=0x04;
      ucSingleLED[1]|=0x02;
      ucSingleLED[2]|=0x40;
      ucSingleLED[3]|=0x04;
         }

     if(reqConfiguration.uiSysConfiguration==1)
     {
              for(i=0;i<4;i++)                              //blank VARy blank
                {
                  ucRow2[i] = blank_patts[i] ;
                }
      ucSingleLED[0]|=0x00;//0x04;//0x00;  	Shree made this changes
      ucSingleLED[1]|=0x02;
      ucSingleLED[2]|=0x40;
      ucSingleLED[3]|=0x04;
     }
     else if ((reqConfiguration.uiSysConfiguration==2)||(reqConfiguration.uiSysConfiguration==3))   //3P1E or 1P1E
     {
                for(i=0;i<4;i++)                              //blank VARy blank
                {
                  ucRow2[i] = blank_patts[i] ;
                }
                for(i=0;i<4;i++)                              //blank VARb blank
                {
                  ucRow3[i] = blank_patts[i] ;
                }
      ucSingleLED[0]|=0x00;
      ucSingleLED[1]|=0x02;
      ucSingleLED[2]|=0x40;
      ucSingleLED[3]|=0x00;
     }
       CreatePage(PARAM_DISP, 7,ucRow1, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    //-------------PAGE 8 VAr,VAy,VAb--------------------------
     case 8:
    {
   //   signed int uiPrintval;
           if(fnabs(reqMeasurepara.flKVAr) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flKVAr) >1000000)
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flKVAr/1000000), &ucIndex);
                        ucSingleLED[2] |= 0x01;	      //M1 led
                }
              else if(fnabs(reqMeasurepara.flKVAr) > 1000)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flKVAr/1000), &ucIndex);
                 ucSingleLED[0] |= 0x01;	 //K1 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flKVAr, &ucIndex);
              }
           convert_to_dec_patt_signed(uiPrintval, ucRow1);
            if(ucIndex==3)
            {
             ucRow1[ucIndex] = ucRow1[ucIndex] ;
            }
            else
            {
              ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
            }
          }
         else
            {
              for(i=0;i<4;i++)
              {
                ucRow1[i] = OF_patts[i] ;
              }
            }
          if(fnabs(reqMeasurepara.flKVAy) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flKVAy) > 1000000 )
                {
                        uiPrintval = Floatpatt4((reqMeasurepara.flKVAy/1000000), &ucIndex);
                        ucSingleLED[0] |= 0x40;	             //M2 led
                }
              else if(fnabs(reqMeasurepara.flKVAy) > 1000)
              {
                 uiPrintval = Floatpatt4((reqMeasurepara.flKVAy/1000), &ucIndex);
                 ucSingleLED[2] |= 0x80;	        //K2 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flKVAy, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow2);
                if(ucIndex==3)
                {
                 ucRow2[ucIndex] = ucRow2[ucIndex] ;
                }
                else
                {
                  ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
                }
            }
            else
            {
              for(i=0;i<4;i++)
              {
                ucRow2[i] = OF_patts[i] ;
              }
            }
            if(fnabs(reqMeasurepara.flKVAb) <= 9999000000 )
            {
              if(fnabs(reqMeasurepara.flKVAb) > 1000000 )
              {
                      uiPrintval = Floatpatt4((reqMeasurepara.flKVAb/1000000), &ucIndex);
                      ucSingleLED[2] |= 0x20;	  //M3 led
              }
              else if(fnabs(reqMeasurepara.flKVAb) > 1000)
              {
                uiPrintval = Floatpatt4((reqMeasurepara.flKVAb/1000), &ucIndex);
                ucSingleLED[0] |= 0x20;	          //K3 led
              }
              else
              {
                uiPrintval = Floatpatt4(reqMeasurepara.flKVAb, &ucIndex);
              }
              convert_to_dec_patt_signed(uiPrintval, ucRow3);
               if(ucIndex==3)
            {
             ucRow3[ucIndex] = ucRow3[ucIndex] ;
            }
            else
            {
              ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
            }
            }
            else
            {
              for(i=0 ;i < 4; i++)
              {
                ucRow3[i] = OF_patts[i] ;
              }
            }

     if(reqConfiguration.uiSysConfiguration==0)
         {
      ucSingleLED[0]|=0x14;
      ucSingleLED[1]|= 0x02;
      ucSingleLED[2]|= 0x00;
      ucSingleLED[3]|= 0x04;
         }

    if(reqConfiguration.uiSysConfiguration==1)
     {
                for(i=0;i<4;i++)                              //blank VAy blank
                {
                  ucRow2[i] = blank_patts[i] ;
                }
      ucSingleLED[0]|=0x10;
      ucSingleLED[1]|= 0x02;
      ucSingleLED[2]|= 0x00;
      ucSingleLED[3]|= 0x04;

     }
     else if ((reqConfiguration.uiSysConfiguration==2)||(reqConfiguration.uiSysConfiguration==3))   //3P1E or 1P1E
     {
               for(i=0;i<4;i++)                              //blank VAy blank
                {
                  ucRow2[i] = blank_patts[i] ;
                }
                for(i=0;i<4;i++)                              //blank VAb blank
                {
                  ucRow3[i] = blank_patts[i] ;
                }
       ucSingleLED[0]|=0x10;
      ucSingleLED[1]|= 0x02;
      ucSingleLED[2]|= 0x00;
      ucSingleLED[3]|= 0x00;
     }
       CreatePage(PARAM_DISP, 8, ucRow1, ucRow2, ucRow3, ucSingleLED);
      break;
    }

    /*-------------PAGE 9 Phase Angle Per Phase--------------------------
    case 9:
    {
    //  signed int uiPrintval;
      if (reqConfiguration.uiSysConfiguration==0)     // for 3P3E
      {
        /*-------------For PFr----------------------------------
        //if(pfr_error==1)      // commented
        if((pfr_error==1)||(reqMeasurepara.flIr==0))    // edited by abhijit mali on 04/06/2015
        {
          for(i=0;i<4;i++)
          {
            ucRow1[i] = Err_patts[i] ;
          }
        }
       else
        {
          uiPrintval = Floatpatt4_pf(reqMeasurepara.flPFr, &ucIndex);
          convert_to_dec_patt_signed_pf(uiPrintval, ucRow1);

           if(ucIndex==3)
            {
             ucRow1[ucIndex] = ucRow1[ucIndex] ;
            }
            else
            {
              ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
            }
        }
        for(i=0;i<100;i++);
        /*-------------For PFy----------------------------------
        //if(pfy_error==1) // commented
        if((pfy_error==1)||(reqMeasurepara.flIy==0))    // edited by abhijit mali on 04/06/2015
        {
          for(i=0;i<4;i++)
          {
            ucRow2[i] = Err_patts[i] ;
          }
        }
       else
        {
          uiPrintval = Floatpatt4_pf(reqMeasurepara.flPFy, &ucIndex);
          convert_to_dec_patt_signed_pf(uiPrintval, ucRow2);

           if(ucIndex==3)
            {
             ucRow2[ucIndex] = ucRow2[ucIndex] ;
            }
            else
            {
              ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
            }
         }
        for(i=0;i<100;i++);
      /*-------------For PFb----------------------------------
        //if(pfb_error==1)  // commented
        if((pfb_error==1)||(reqMeasurepara.flIb==0))    // edited by abhijit mali on 04/06/2015
        {
          for(i=0;i<4;i++)
          {
            ucRow3[i] = Err_patts[i] ;
          }
        }
       else
        {
          uiPrintval = Floatpatt4_pf(reqMeasurepara.flPFb, &ucIndex);
          convert_to_dec_patt_signed_pf(uiPrintval, ucRow3);

           if(ucIndex==3)
            {
             ucRow3[ucIndex] = ucRow3[ucIndex] ;
            }
            else
            {
              ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
            }
         }

        ucSingleLED[0]= 0x04; // Y LED
        ucSingleLED[1] = 0x02; // R LED
        ucSingleLED[2] = 0x04; // PF LED
        ucSingleLED[3] = 0x04; // B LED
      for(i=0;i<100;i++);

      CreatePage(PARAM_DISP, 9, ucRow1, ucRow2, ucRow3, ucSingleLED);
    }
    else if(reqConfiguration.uiSysConfiguration==1)       //3P2E
    {
      signed int uiPrintval;
        /*-------------For PFr----------------------------------
       // if(pfr_error==1)      // commented
        if((pfr_error==1)||(reqMeasurepara.flIr==0))    // edited by abhijit mali on 04/06/2015
        {
          for(i=0;i<4;i++)
          {
            ucRow1[i] = Err_patts[i] ;
          }
        }
       else
        {
          uiPrintval = Floatpatt4(reqMeasurepara.flPFr, &ucIndex);
          convert_to_dec_patt_signed(uiPrintval, ucRow1);

           if(ucIndex==3)
            {
             ucRow1[ucIndex] = ucRow1[ucIndex] ;
            }
            else
            {
              ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
            }
        }
        for(i=0;i<100;i++);
        /*-------------For PFb----------------------------------
       //if(pfb_error==1)  // commented
        if((pfb_error==1)||(reqMeasurepara.flIb==0))    // edited by abhijit mali on 04/06/2015
        {
          for(i=0;i<4;i++)
          {
            ucRow3[i] = Err_patts[i] ;
          }
        }
       else
        {
          uiPrintval = Floatpatt4(reqMeasurepara.flPFb, &ucIndex);
          convert_to_dec_patt_signed(uiPrintval, ucRow3);

           if(ucIndex==3)
            {
             ucRow3[ucIndex] = ucRow3[ucIndex] ;
            }
            else
            {
              ucRow3[ucIndex] = ucRow3[ucIndex] | 0x80;
            }
         }

        for(i=0;i<100;i++);
        ucSingleLED[0] = 0x00; // Y LED
        ucSingleLED[1] = 0x02; // R LED
        ucSingleLED[2] = 0x04; // PF LED
        ucSingleLED[3] = 0x04; // B LED
      for(i=0;i<100;i++);

       for(i=0;i<4;i++)                              //blank PFy blank
      {
         ucRow2[i] = blank_patts[i] ;
      }
      CreatePage(PARAM_DISP, 9, ucRow1, ucRow2, ucRow3, ucSingleLED);
    }
      else if ((reqConfiguration.uiSysConfiguration==2)||(reqConfiguration.uiSysConfiguration==3))   //3P1E or 1P1E
      {
                              /*-------------For PFr----------------------------------
                            //if(pfr_error==1)  // commented
                            if((pfr_error==1)||(reqMeasurepara.flIr==0))    // edited by abhijit mali on 04/06/2015
                            {
                              for(i=0;i<4;i++)
                              {
                                ucRow1[i] = Err_patts[i] ;
                              }
                            }
                           else
                            {
                              uiPrintval = Floatpatt4(reqMeasurepara.flPFr, &ucIndex);
                              convert_to_dec_patt_signed(uiPrintval, ucRow1);

                               if(ucIndex==3)
                                {
                                 ucRow1[ucIndex] = ucRow1[ucIndex] ;
                                }
                                else
                                {
                                  ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
                                }
                            }
                                     for(i=0;i<4;i++)                              //blank PFy blank
                                      {
                                        ucRow2[i] = blank_patts[i] ;
                                      }
                                      for(i=0;i<4;i++)                              //blank PFb blank
                                      {
                                        ucRow3[i] = blank_patts[i] ;
                                      }
                              ucSingleLED[0] |= 0x00; // Y LED
                              ucSingleLED[1] |= 0x02; // R LED
                              ucSingleLED[2] |= 0x04; // PF LED
                              ucSingleLED[3] |= 0x00; // B LED
                              CreatePage(PARAM_DISP, 9, ucRow1, ucRow2, ucRow3, ucSingleLED);
           }


      break;
    }
 /*-------------PAGE 10 frequency--------------------------
    case 10:
    {
     // signed int uiPrintval;
      uiPrintval = Floatpatt4(reqMeasurepara.flFreq, &ucIndex);
      convert_to_dec_patt_signed(uiPrintval, ucRow1);
      ucRow1[ucIndex] = ucRow1[ucIndex] | 0x80;
        for(i=0;i<4;i++)
        {
         ucRow2[i] = Blank_patts[i] ;
        }
        for(i=0;i<4;i++)
        {
         ucRow3[i] = Blank_patts[i] ;
        }
      ucSingleLED[0]|= 0x00;
      ucSingleLED[1]|= 0x00;
      ucSingleLED[2]|= 0x10;
      ucSingleLED[3]|= 0x00;
      CreatePage(PARAM_DISP, 10, ucRow1, ucRow2, ucRow3, ucSingleLED);
      break;
    }
    case 11:
      {
        signed int uiPrintval;
        if(KWHf >999999)
        {
           uiPrintval = Floatpatt8((KWHf/1000000), &ucIndex);
            ucSingleLED[0] |= 0x40;	             //M2 led
        }
        else if(KWHf >999)
        {    
          uiPrintval = Floatpatt8((KWHf/1000), &ucIndex);
           ucSingleLED[2] |= 0x80;	        //K2 led
        }
        else
        {
            uiPrintval = Floatpatt8((KWHf), &ucIndex);
        }   

        convert_to_dec_patt_signed8(uiPrintval, ucRowLong);
        for(i=0; i<4; i++)
        {
                ucRow2[i] =ucRowLong[i];
                ucRow3[i] =ucRowLong[i+4];
        }

       if(ucIndex < 4 )
               ucRow2[ucIndex] = ucRow2[ucIndex] | 0x80;
        else
        {
          ucRow3[ucIndex - 4] = ucRow3[ucIndex - 4] | 0x80;
        }
     if((reverse_flag==0) && (reverse_flag_r_phase==0) && (reverse_flag_y_phase==0) && (reverse_flag_b_phase==0))
     {
        for(i=0;i<4;i++)
        {
         ucRow1[i] = WH_patts[i] ;
        }
     }
     else
     {
       for(i=0;i<4;i++)
        {
         ucRow1[i] = WH_patts_re[i] ;
        }
     }
      ucSingleLED[0]|= 0x00;
      ucSingleLED[1]|= 0x00;
      ucSingleLED[2] |= 0x00;
      ucSingleLED[3] |= 0x00;
      CreatePage(PARAM_DISP, 11, ucRow1, ucRow2, ucRow3, ucSingleLED);
      break;
  }
 }
}

*/

/* ----------------------------------  AE9000 LED POSITION -------------------------------------------

  Indication	Array Val		Indication	Array Val	      Indication    Array Val

  V	ucSingleLED[0]=0x02		K	ucSingleLED[0] |= 0x01	          F	ucSingleLED[2]=0x10
  R 	ucSingleLED[1]=0x02		M	ucSingleLED[2] |= 0x01	          W	ucSingleLED[3]=0x01
  RY	ucSingleLED[2]=0x02		LD	ucSingleLED[0]=0x80	          Wh	ucSingleLED[1]=0x80
  I	ucSingleLED[3]=0x02		K	ucSingleLED[2] |= 0x80	          NI	ucSingleLED[3]=0x80
  Y	ucSingleLED[0]=0x04		M	ucSingleLED[0] |= 0x40	          VAR	ucSingleLED[1]=0x40
  YB	ucSingleLED[1]=0x04		LG	ucSingleLED[2] = 0x40	          Varh	ucSingleLED[3]=0x40
  PF	ucSingleLED[2]=0x04		K	ucSingleLED[0] |= 0x20	          THD	ucSingleLED[1]=0x20
  B	ucSingleLED[3]=0x04		M	ucSingleLED[2] |= 0x20	          VA	ucSingleLED[3]=0x20
  BR	ucSingleLED[0]=0x08		Q	ucSingleLED[0] = 0x10	          Vah	ucSingleLED[1]=0x10
			                IMP	ucSingleLED[2] = 0x10	          TOT	ucSingleLED[3]=0x10
			                EXP	ucSingleLED[1]=0x08	          MD	ucSingleLED[2]=0x08

*/







