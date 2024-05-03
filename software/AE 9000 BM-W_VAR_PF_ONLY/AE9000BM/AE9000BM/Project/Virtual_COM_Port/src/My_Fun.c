  /******************************************************************************/

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
#include "math.h"
#include "EXT_MEM.h"

#define KW_POSITIVE_SET 0.8
#define KW_NEGATIVE_SET -0.8
#define VAR_POSITIVE_SET 0.8
#define VAR_NEGATIVE_SET -0.8

#define KW_TOT_POSITIVE_SET 2.5
#define KW_TOT_NEGATIVE_SET -2.5
#define VAR_TOT_POSITIVE_SET 2.5
#define VAR_TOT_NEGATIVE_SET -2.5




void Calculation_Power();

float z;
float FVA_R,FVA_Y,FVA_B,FW_R,FW_Y,FW_B;
float VoltFact,CurrFact,PowrFact,Sum_Watt_EM,Sum_Var_EM;

unsigned int pf_error;
signed   long int VA_R,VA_Y,VA_B,W_R,W_Y,W_B;
unsigned long int No_ip;

unsigned char R_quadrant,Y_quadrant,B_quadrant;
unsigned char pfr_error,pfy_error,pfb_error;
unsigned char EndOfTransfer,adc_count;
unsigned char Tot_quadrant;

extern unsigned char key_pressed;
extern unsigned char Key_Read1(void);

extern float Angle_Diff_Tot, Calib_Pf,Watt_scaling_factor,Calib_Watt_Tot,Angle_Diff_R,Angle_Diff_Y,Angle_Diff_B;
extern float mR,mY,mB,mRY,mYB,mBR,mIR,mIY,mIB,cR,cY,cB,cRY,cYB,cBR,cIR,cIY,cIB,mWR,mWY,mWB,cWR,cWY,cWB,mWT,cWT,mVAT,cVAT,Sum_Var,Sum_Watt;
extern float Energy_watt_tot,Energy_var_tot;
extern float m_IR,m_IY,m_IB,c_IR,c_IY,c_IB;

extern long double VR_cal,VY_cal,VB_cal,VRY_cal,VYB_cal,VBR_cal,IR_cal,IY_cal,IB_cal,Pr_cal,Py_cal,Pb_cal,Qr_cal,Qy_cal,Qb_cal,PT_3P2E_cal,QT_3P2E_cal;

extern signed int IntPFr,IntPFy,IntPFb;
extern unsigned char Angle_Set;
extern unsigned char freq_comp,Run_flag,memory_location_flag;

extern MEASURED_PARAM  reqMeasurepara;
extern CONFIGURATION_PARAM reqConfiguration;
extern UnCal reqUncal;
extern NVIC_InitTypeDef NVIC_InitStructure;
extern EXTI_InitTypeDef   EXTI_InitStructure;

extern float Freq_calib_factor,factor;
extern unsigned char reverse_flag;
/**************MODBUS*****************/
extern float VR_MB,VY_MB,VB_MB,VLNavg_MB,KWHf;
extern float VRY_MB,VYB_MB,VBR_MB,VLLavg_MB;
extern float IR_MB,IY_MB,IB_MB,Iavg_MB;
extern float PFR_MB,PFY_MB,PFB_MB,PFavg_MB,Frq_MB;
extern float KWr_MB,KWy_MB,KWb_MB,WTot_MB;
extern float VARr_MB,VARy_MB,VARb_MB,VARTot_MB;
extern float KVAr_MB,KVAy_MB,KVAb_MB,KVATot_MB;
extern float KWHf_MB;
unsigned char reverse_flag_r_phase,reverse_flag_y_phase,reverse_flag_b_phase;
unsigned int DelMe = 0;
float fUncompKWr  = 0;
float fUncompKWy  = 0;
float fUncompKWb  = 0;

/************************************/
/***************************************************************************************************************************
Function     :  READ_CONFG
Parameter    :  null
Description  :   This function is used to read configuration parameters from EEprom
***************************************************************************************************************************/

void READ_CONFG(void)
{
  reqConfiguration.uiSysConfiguration   = (unsigned int) FnReadEprom(0x080800A0);
 // reqConfiguration.ulCTRPri             = (unsigned int) FnReadEprom(0x080800B0);
 // reqConfiguration.ulCTRSec             = (unsigned int) FnReadEprom(0x080800C0);
  //reqConfiguration.ulPTRPri             = (unsigned int) FnReadEprom(0x080800D0);
 // reqConfiguration.ulPTRSec             = (unsigned int) FnReadEprom(0x080800E0);
  reqConfiguration.ulCTRPri             = (float) FnReadEprom(0x080800B0);
  reqConfiguration.ulCTRSec             = (float) FnReadEprom(0x080800C0);
  reqConfiguration.ulPTRPri             = (float) FnReadEprom(0x080800D0);
  reqConfiguration.ulPTRSec             = (float) FnReadEprom(0x080800E0);
  reqConfiguration.uiScrollTime         = (unsigned int) FnReadEprom(0x080800F0);
  reqConfiguration.uiScrollON           = (unsigned int) FnReadEprom(0x08080100);
  reqConfiguration.uiNetworkID          = (unsigned int) FnReadEprom(0x08080110);
  reqConfiguration.uiBaudRate           = (unsigned int) FnReadEprom(0x08080120);
}

/***************************************************************************************************************************
Function     :  WRITE_CONFG
Parameter    :  null
Description  :  This function is used to write configuration parameters on EEprom
***************************************************************************************************************************/

void WRITE_CONFG(void)
{

  FnEraseEpromWord(0x080800A0,2);
  FnWriteEpromWord(0x080800A0, (float)(reqConfiguration.uiSysConfiguration));
  FnEraseEpromWord(0x080800B0,2);
  FnWriteEpromWord(0x080800B0, (float)(reqConfiguration.ulCTRPri));
  FnEraseEpromWord(0x080800C0,2);
  FnWriteEpromWord(0x080800C0, (float)(reqConfiguration.ulCTRSec));
  FnEraseEpromWord(0x080800D0,2);
  FnWriteEpromWord(0x080800D0, (float)(reqConfiguration.ulPTRPri));
  FnEraseEpromWord(0x080800E0,2);
  FnWriteEpromWord(0x080800E0, (float)(reqConfiguration.ulPTRSec));
  FnEraseEpromWord(0x080800F0,2);
  FnWriteEpromWord(0x080800F0,(float) (reqConfiguration.uiScrollTime));
  FnEraseEpromWord(0x08080100,2);
  FnWriteEpromWord(0x08080100, (float)(reqConfiguration.uiScrollON));
  FnEraseEpromWord(0x08080110,2);
  FnWriteEpromWord(0x08080110, (float)( reqConfiguration.uiNetworkID));
  FnEraseEpromWord(0x08080120,2);
  FnWriteEpromWord(0x08080120, (float)(reqConfiguration.uiBaudRate));

}

/***************************************************************************************************************************
Function     :  ConF_FUN
Parameter    :  null
Description  :  This function is used to
***************************************************************************************************************************/

void ConF_FUN(void)
{
  float TempValue=0;

  flot_rec_var(&TempValue);                                                      // (&reqConfiguration.uiSysConfiguration);
  FnEraseEpromWord(0x080800A0,2);
  FnWriteEpromWord(0x080800A0, TempValue);                                       // (float)(reqConfiguration.uiSysConfiguration));
  SendChar('Y');

  /* CTR Prim */
  flot_rec_var(&TempValue);                                                      // (&reqConfiguration.ulCTRPri);
  FnEraseEpromWord(0x080800B0,2);
  FnWriteEpromWord(0x080800B0, TempValue);                                       // (float)(reqConfiguration.ulCTRPri));
  SendChar('Y');

  /* CTR Sec */
  flot_rec_var(&TempValue);                                                      //  flot_rec_var(&reqConfiguration.ulCTRSec);
  FnEraseEpromWord(0x080800C0,2);
  FnWriteEpromWord(0x080800C0, TempValue);                                       // (float)(reqConfiguration.ulCTRSec));
  SendChar('Y');

  /* PTR Prim */
  flot_rec_var(&TempValue);                                                     // (reqConfiguration.ulPTRPri));
  FnEraseEpromWord(0x080800D0,2);
  FnWriteEpromWord(0x080800D0, TempValue);                                      // (float)(reqConfiguration.ulPTRPri));
  SendChar('Y');

  /* PTR Sec */
  flot_rec_var(&TempValue);                                                     // (reqConfiguration.ulPTRSec));
  FnEraseEpromWord(0x080800E0,2);
  FnWriteEpromWord(0x080800E0, TempValue);                                      // (float)(reqConfiguration.ulPTRSec));
  SendChar('Y');

  /* Scroll Time */
  flot_rec_var(&TempValue);                                                     // (reqConfiguration.uiScrollTime));
  FnEraseEpromWord(0x080800F0,2);
  FnWriteEpromWord(0x080800F0, TempValue);                                      // (float) (reqConfiguration.uiScrollTime));
  SendChar('Y');


  flot_rec_var(&TempValue);                                                     // (reqConfiguration.uiScrollON));
  FnEraseEpromWord(0x08080100,2);
  FnWriteEpromWord(0x08080100, TempValue);                                      // (float)(reqConfiguration.uiScrollON));
  SendChar('Y');

  flot_rec_var(&TempValue);                                                     // (reqConfiguration.uiNetworkID));
  FnEraseEpromWord(0x08080110,2);
  FnWriteEpromWord(0x08080110, TempValue);                                      // (float)( reqConfiguration.uiNetworkID));
  SendChar('Y');

  flot_rec_var(&TempValue);                                                     // (reqConfiguration.uiBaudRate));
  FnEraseEpromWord(0x08080120,2);
  FnWriteEpromWord(0x08080120, TempValue);                                      // (float)(reqConfiguration.uiBaudRate));
  SendChar('Y');
}

/***************************************************************************************************************************
Function     :  READ_EEPROM
Parameter    :  null
Description  :  This function is used to read calibration values from EEprom
***************************************************************************************************************************/

void READ_EEPROM(void)
{
  mR = FnReadEprom(0x08080000);
  cR = FnReadEprom(0x08080004);
  mY = FnReadEprom(0x08080008);
  cY = FnReadEprom(0x0808000C);
  mB = FnReadEprom(0x08080010);
  cB = FnReadEprom(0x08080014);
  mRY = FnReadEprom(0x08080019);
  cRY = FnReadEprom(0x0808001D);
  mYB = FnReadEprom(0x08080021);
  cYB = FnReadEprom(0x08080025);
  mBR = FnReadEprom(0x0808002A);
  cBR = FnReadEprom(0x0808002F);
  mIR = FnReadEprom(0x08080033);
  cIR = FnReadEprom(0x08080037);
  mIY = FnReadEprom(0x0808003B);
  cIY = FnReadEprom(0x0808003F);
  mIB = FnReadEprom(0x08080044);
  cIB = FnReadEprom(0x08080048);
  mWR = FnReadEprom(0x0808004C);
  cWR = FnReadEprom(0x08080050);
  mWY = FnReadEprom(0x08080054);
  cWY = FnReadEprom(0x08080058);
  mWB = FnReadEprom(0x0808005C);
  cWB = FnReadEprom(0x08080060);
  mWT = FnReadEprom(0x08080064);   //for reactive power
  cWT = FnReadEprom(0x08080068);
  mVAT = FnReadEprom(0x0808006C);
  cVAT = FnReadEprom(0x08080070);

  m_IR = FnReadEprom(0x08080080);
  c_IR = FnReadEprom(0x08080084);
  m_IY = FnReadEprom(0x0808008C);
  c_IY = FnReadEprom(0x08080090);
  m_IB = FnReadEprom(0x08080094);
  c_IB = FnReadEprom(0x0808009C);

  Freq_calib_factor = FnReadEprom(0x080800B4);
  factor = FnReadEprom(0x080800AC);

  if(Freq_calib_factor==0)
    Freq_calib_factor =1;
  if(factor==0)
    factor=1;

  fnReadFromNVMemory(0x00060,2);    //read memory flag
  if(memory_location_flag==0)
  {
     fnReadFromNVMemory(0x00010,0);    // read KWH
     Sum_Watt =KWHf;
     fnReadFromNVMemory(0x00020,1);     // read write cycle
  }
  else
  {
    fnReadFromNVMemory(0x00030,0);
    Sum_Watt =KWHf;
    fnReadFromNVMemory(0x00040,1);
  }
   //  Sum_Watt =KWHf;
   Sum_Watt = Sum_Watt *3600;


//  Sum_Watt_EM  = FnReadEprom(0x08080160);
//  Sum_Watt     = Sum_Watt_EM *3600;
//  Sum_Var_EM   = FnReadEprom(0x08080170);
//  Sum_Var      = Sum_Var_EM*3600;

}
/***************************************************************************************************************************
Function     :  WRITE_EEPROM
Parameter    :  null
Description  :  This function is used to write calibration values on EEprom
***************************************************************************************************************************/

void WRITE_EEPROM(void)
{
    FnEraseEpromWord(0x08080000,30);
    FnWriteEpromWord(0x08080000, mR);
    FnWriteEpromWord(0x08080004, cR);
    FnWriteEpromWord(0x08080008, mY);
    FnWriteEpromWord(0x0808000C, cY);
    FnWriteEpromWord(0x08080010, mB);
    FnWriteEpromWord(0x08080014, cB);
    FnWriteEpromWord(0x08080019, mRY);
    FnWriteEpromWord(0x0808001D, cRY);
    FnWriteEpromWord(0x08080021, mYB);
    FnWriteEpromWord(0x08080025, cYB);
    FnWriteEpromWord(0x0808002A, mBR);
    FnWriteEpromWord(0x0808002F, cBR);
    FnWriteEpromWord(0x08080033, mIR);
    FnWriteEpromWord(0x08080037, cIR);
    FnWriteEpromWord(0x0808003B, mIY);
    FnWriteEpromWord(0x0808003F, cIY);
    FnWriteEpromWord(0x08080044, mIB);
    FnWriteEpromWord(0x08080048, cIB);
    FnWriteEpromWord(0x0808004C, mWR);
    FnWriteEpromWord(0x08080050, cWR);
    FnWriteEpromWord(0x08080054, mWY);
    FnWriteEpromWord(0x08080058, cWY);
    FnWriteEpromWord(0x0808005C, mWB);
    FnWriteEpromWord(0x08080060, cWB);
    FnWriteEpromWord(0x08080064, mWT);
    FnWriteEpromWord(0x08080068, cWT);
    FnWriteEpromWord(0x0808006C, mVAT);
    FnWriteEpromWord(0x08080070, cVAT);

    FnWriteEpromWord(0x08080080, m_IR);
    FnWriteEpromWord(0x08080084, c_IR);
    FnWriteEpromWord(0x0808008C, m_IY);
    FnWriteEpromWord(0x08080090, c_IY);
    FnWriteEpromWord(0x08080094, m_IB);
    FnWriteEpromWord(0x0808009C, c_IB);

     FnWriteEpromWord(0x080800B4, Freq_calib_factor);
     FnWriteEpromWord(0x080800AC, factor);

//    FnWriteEpromWord(0x08080074, Angle_Diff_Tot);
//    FnWriteEpromWord(0x08080078, Calib_Pf);
//    FnWriteEpromWord(0x0808007C, Watt_scaling_factor);
//    FnWriteEpromWord(0x08080080, Angle_Diff_R);
//    FnWriteEpromWord(0x08080084, Angle_Diff_Y);
//    FnWriteEpromWord(0x0808008C, Angle_Diff_B);
}
/***************************************************************************************************************************
Function     :  FreQfeaD
Parameter    :  null
Description  :  This function is used to enable external interrupt in which frequency is calculated and that value
                is collected in this function  this function will wait for atleast 390ms to check for frequency calculation
                complete flag.
***************************************************************************************************************************/

void  FreQfeaD(void)
{
 // SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);
  unsigned char ZCD_Vr=0;

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);

  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI15_10 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  No_ip=1;
  while(freq_comp==0)
  {
    if(No_ip==400)//400
    {
      freq_comp=1;
      //reqMeasurepara.flFreq=0.0;
      ZCD_Vr=1;
    }
  }
  freq_comp=0;
  if(ZCD_Vr==1)
  {
    //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);

   NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//1
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    No_ip=1;
    while(freq_comp==0)
    {
      if(No_ip==400)//400
      {
        freq_comp=1;
        reqMeasurepara.flFreq=0.0;
        ZCD_Vr=2;
      }
    }
   //ZCD_Vr=0;
   freq_comp=0;

  // SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);
   EXTI_InitStructure.EXTI_Line = EXTI_Line9;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising;
   EXTI_InitStructure.EXTI_LineCmd = DISABLE;
   EXTI_Init(&EXTI_InitStructure);
  }
//  if(ZCD_Vr==2)
//  {
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//2
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//1
//    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
//    NVIC_Init(&NVIC_InitStructure);
//  }
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//1
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/***************************************************************************************************************************
Function     :  RUN_FUN
Parameter    :  null
Description  :
***************************************************************************************************************************/

void RUN_FUN()
{
  unsigned char ucRstVarCntr = 0;
  if(reqMeasurepara.flVrn!=0)
    FreQfeaD();
  else{reqMeasurepara.flFreq=0;}
    if(reqMeasurepara.flFreq==0.0)
    {
      //IANET: 21/12/2018
        reqMeasurepara.flVrn=0.0;    reqMeasurepara.flVyn=0.0;    reqMeasurepara.flVbn=0.0;    reqMeasurepara.flVry=0.0;reqMeasurepara.flVyb=0.0;reqMeasurepara.flVbr=0.0;
        reqMeasurepara.flIr=0.0;     reqMeasurepara.flIy=0.0;     reqMeasurepara.flIb=0.0;
        reqMeasurepara.flKWr=0.0;    reqMeasurepara.flKWy=0.0;    reqMeasurepara.flKWb=0.0;
        reqMeasurepara.flVARr=0.0;   reqMeasurepara.flVARy=0.0;   reqMeasurepara.flVARb=0.0;
        reqMeasurepara.flKVAr=0.0;   reqMeasurepara.flKVAy=0.0;   reqMeasurepara.flKVAb=0.0;
        reqMeasurepara.flKWTot=0.0;  reqMeasurepara.flVARTot=0.0; reqMeasurepara.flKVATot=0.0;
        reqMeasurepara.flVllavg=0.0; reqMeasurepara.flIavg=0.0;   pf_error=1;                   reqMeasurepara.AngL=0.0;
        reqMeasurepara.flVlnavg=0.0; Energy_watt_tot =0.0;        Energy_var_tot=0.0;
        reverse_flag=0;  reverse_flag_r_phase=0;  reverse_flag_y_phase=0;  reverse_flag_b_phase=0;
          
      /* Changes to add per phase PF */
        pfr_error=1;pfy_error=1;pfb_error=1;     //IANET: 21/12/2018

        key_pressed=Key_Read1();

        if(key_pressed)
        {
          if(key_pressed==1)
          {
            key_pressed=0;
            Select_Key();
          }
          else if(key_pressed==2)
          {
            key_pressed=0;
            Enter_Key();
          }
         while(Run_flag==0)
         {
           key_pressed=Key_Read1();
           if(key_pressed)
           {
             if(key_pressed==1)
             {
               key_pressed=0;
               Select_Key();
             }
             else if(key_pressed==2)
             {
               key_pressed=0;
               Enter_Key();
             }
           }
         }
        }
    }
   // else    //IANET: 21/12/2018
    //{
        Read_ADC();
        pfr_error=0;pfy_error=0;pfb_error=0;pf_error=0;

       /************************************* VOLTAGE   ************************************************/

        reqMeasurepara.flVrn=(float)((VR_cal*mR)+cR);
        reqMeasurepara.flVyn=(float)((VY_cal*mY)+cY);
        reqMeasurepara.flVbn=(float)((VB_cal*mB)+cB);

        reqMeasurepara.flVry=(float)((VRY_cal*mRY)+cRY);
        reqMeasurepara.flVyb=(float)((VYB_cal*mYB)+cYB);
        reqMeasurepara.flVbr=(float)((VBR_cal*mBR)+cBR);

        if(reqMeasurepara.flVrn<10){reqMeasurepara.flVrn=0;}
        if(reqMeasurepara.flVyn<10){reqMeasurepara.flVyn=0;}
        if(reqMeasurepara.flVbn<10){reqMeasurepara.flVbn=0;}
        if(reqMeasurepara.flVry<10){reqMeasurepara.flVry=0;}
        if(reqMeasurepara.flVyb<10){reqMeasurepara.flVyb=0;}
        if(reqMeasurepara.flVbr<10){reqMeasurepara.flVbr=0;}


       /************************************** CURRENT  ****************************************************/

         reqMeasurepara.flIr=(float)((IR_cal*mIR)+cIR);
         reqMeasurepara.flIy=(float)((IY_cal*mIY)+cIY);
         reqMeasurepara.flIb=(float)((IB_cal*mIB)+cIB);

	 if(reqMeasurepara.flIr < 0.9499)
              reqMeasurepara.flIr=(float)((IR_cal*m_IR)+c_IR);
         if(reqMeasurepara.flIy < 0.9499)
                  reqMeasurepara.flIy=(float)((IY_cal*m_IY)+c_IY);
         if(reqMeasurepara.flIb < 0.9499)
            reqMeasurepara.flIb=(float)((IB_cal*m_IB)+c_IB);

        /************************************* ACTIVE POWER   ************************************************/
           if(reqConfiguration.uiSysConfiguration==1)
           {
               reqMeasurepara.flKWr= (long double)((Pr_cal*mWR)+cWR);
               fUncompKWr = reqMeasurepara.flKWr;
                if((reqMeasurepara.flKWr > KW_NEGATIVE_SET)&&(reqMeasurepara.flKWr < KW_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flKWr =0.0;
              }
               reqMeasurepara.flKWb= (long double)((Pb_cal*mWB)+cWB);
               fUncompKWb = reqMeasurepara.flKWb;
                if((reqMeasurepara.flKWb > KW_NEGATIVE_SET)&&(reqMeasurepara.flKWb < KW_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flKWb =0.0;
              }

           }

           else if(reqConfiguration.uiSysConfiguration==0)
           {
               reqMeasurepara.flKWr= (long double)((Pr_cal*mWR)+cWR);
               fUncompKWr = reqMeasurepara.flKWr;
             if((reqMeasurepara.flKWr > KW_NEGATIVE_SET)&&(reqMeasurepara.flKWr < KW_POSITIVE_SET))     // 90degree active 10W offset
             {
               reqMeasurepara.flKWr =0.0;
              }

              reqMeasurepara.flKWy= (long double)((Py_cal*mWY)+cWY);
              fUncompKWy  = reqMeasurepara.flKWy;

               if((reqMeasurepara.flKWy > KW_NEGATIVE_SET)&&(reqMeasurepara.flKWy < KW_POSITIVE_SET))     // 90degree active 10W offset
             {
              reqMeasurepara.flKWy =0.0;
              }

             reqMeasurepara.flKWb= (long double)((Pb_cal*mWB)+cWB);
             fUncompKWb= reqMeasurepara.flKWb;
              if((reqMeasurepara.flKWb > KW_NEGATIVE_SET)&&(reqMeasurepara.flKWb < KW_POSITIVE_SET))     // 90degree active 10W offset
             {
               reqMeasurepara.flKWb =0.0;
             }

           }
           else if(reqConfiguration.uiSysConfiguration==2)
           {
               reqMeasurepara.flKWr = (long double)((Pr_cal*mWR)+cWR);
               fUncompKWr = reqMeasurepara.flKWr;
                if((reqMeasurepara.flKWr > KW_NEGATIVE_SET)&&(reqMeasurepara.flKWr < KW_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flKWr =0.0;
              }
           }
           else if(reqConfiguration.uiSysConfiguration==3)
           {
               reqMeasurepara.flKWr= (long double)((Pr_cal*mWR)+cWR);
               fUncompKWr = reqMeasurepara.flKWr;
                if((reqMeasurepara.flKWr > KW_NEGATIVE_SET)&&(reqMeasurepara.flKWr < KW_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flKWr =0.0;
              }
           }

        /************************************* REACTIVE POWER  ************************************************/
           if(reqConfiguration.uiSysConfiguration==1)
           {
             reqMeasurepara.flVARr= (long double)((Qr_cal*mWR)+cWR);
              if((reqMeasurepara.flVARr > VAR_NEGATIVE_SET)&&(reqMeasurepara.flVARr < VAR_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flVARr =0.0;
              }

             reqMeasurepara.flVARb= (long double)((Qb_cal*mWB)+cWB);
             if((reqMeasurepara.flVARb > VAR_NEGATIVE_SET)&&(reqMeasurepara.flVARb < VAR_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flVARb =0.0;
              }
            }
         else if(reqConfiguration.uiSysConfiguration==0)
           {
             reqMeasurepara.flVARr= (long double)((Qr_cal*mWR)+cWR);
        if((reqMeasurepara.flVARr > VAR_NEGATIVE_SET)&&(reqMeasurepara.flVARr < VAR_POSITIVE_SET))     // 90degree active 10W offset
         {
             reqMeasurepara.flVARr =0.0;
               }
            reqMeasurepara.flVARy= (long double)((Qy_cal*mWY)+cWY);
             if((reqMeasurepara.flVARy > VAR_NEGATIVE_SET)&&(reqMeasurepara.flVARy < VAR_POSITIVE_SET))     // 90degree active 10W offset
           {
              reqMeasurepara.flVARy =0.0;
              }
           reqMeasurepara.flVARb= (long double)((Qb_cal*mWB)+cWB);
            if((reqMeasurepara.flVARb > VAR_NEGATIVE_SET)&&(reqMeasurepara.flVARb < VAR_POSITIVE_SET))     // 90degree active 10W offset
           {
              reqMeasurepara.flVARb =0.0;
            }
           }
            else if(reqConfiguration.uiSysConfiguration==2)
           {
             reqMeasurepara.flVARr= (long double)((Qr_cal*mWR)+cWR);
             if((reqMeasurepara.flVARr > VAR_NEGATIVE_SET)&&(reqMeasurepara.flVARr < VAR_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flVARr =0.0;
              }
           }
           else if(reqConfiguration.uiSysConfiguration==3)
           {
             reqMeasurepara.flVARr= (long double)((Qr_cal*mWR)+cWR);
             if((reqMeasurepara.flVARr > VAR_NEGATIVE_SET)&&(reqMeasurepara.flVARr < VAR_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flVARr =0.0;
              }
           }
         /************************************* APPERENT POWER   ************************************************/

         if(reqConfiguration.uiSysConfiguration==1)                                                      //3P2E
         {
              reqMeasurepara.flKVAr= (long double)(sqrt(((fUncompKWr)*(fUncompKWr))+(( reqMeasurepara.flVARr)*( reqMeasurepara.flVARr))));
              reqMeasurepara.flKVAb= (long double)(sqrt(((reqMeasurepara.flKWb)*(reqMeasurepara.flKWb))+(( reqMeasurepara.flVARb)*( reqMeasurepara.flVARb))));
         }
         else                                                                                                     
         {                                                                                                   
              reqMeasurepara.flKVAr=(long double)(sqrt(((fUncompKWr)*(fUncompKWr))+((reqMeasurepara.flVARr)*( reqMeasurepara.flVARr))));
              reqMeasurepara.flKVAy=(long double)(sqrt(((fUncompKWy)*(fUncompKWy))+(( reqMeasurepara.flVARy)*( reqMeasurepara.flVARy))));
              reqMeasurepara.flKVAb=(long double)(sqrt(((fUncompKWb)*(fUncompKWb))+(( reqMeasurepara.flVARb)*( reqMeasurepara.flVARb))));
         }
                                                                                                       //3P3E /3P1E /1P1E
         /************************************* FOR CURRENT LOWER SIDE   ************************************************/
         if(reqConfiguration.uiSysConfiguration==0)  // 3 Phase 3 Element
         {

             //if( reqMeasurepara.flIr<0.190)
           if( reqMeasurepara.flIr<0.050)
         {
              if(reqMeasurepara.flVrn!=0)           
                 reqMeasurepara.flIr=(float)((reqMeasurepara.flKVAr)/(reqMeasurepara.flVrn));
             else /*if(reqMeasurepara.flIr<=0.010)*/  
                 reqMeasurepara.flIr=0.0;          
            }
             if(reqMeasurepara.flIr<0.010) //|| (reqMeasurepara.flVrn < 10))
                reqMeasurepara.flIr=0.0;

             //if(reqMeasurepara.flIy<0.190)
             if(reqMeasurepara.flIy<0.050)
             {
               if(reqMeasurepara.flVyn!=0)           
                 reqMeasurepara.flIy=(float)((reqMeasurepara.flKVAy)/(reqMeasurepara.flVyn));
               else /*if(reqMeasurepara.flIy<=0.010)*/  
                 reqMeasurepara.flIy=0.0;          
             }
             if(reqMeasurepara.flIy<0.010) //|| (reqMeasurepara.flVyn < 10))
                 reqMeasurepara.flIy=0.0;

             //if((reqMeasurepara.flIb<0.190))
             if((reqMeasurepara.flIb<0.050))
             {
               if(reqMeasurepara.flVbn!=0)           
                 reqMeasurepara.flIb=(float)((reqMeasurepara.flKVAb)/(reqMeasurepara.flVbn));
               else /*if(reqMeasurepara.flIb<=0.010)*/  
                 reqMeasurepara.flIb=0.0;          
             }
             if(reqMeasurepara.flIb<0.010) //|| (reqMeasurepara.flVbn < 10))
                 reqMeasurepara.flIb=0.0;
         }
          else if(reqConfiguration.uiSysConfiguration==1)    // 3 Phase 2 Element
          {

           if( reqMeasurepara.flIr<0.050)
         {
              if(reqMeasurepara.flVrn!=0)           
                 reqMeasurepara.flIr=(float)((reqMeasurepara.flKVAr)/(reqMeasurepara.flVrn));
             else /*if(reqMeasurepara.flIr<=0.010)*/  
                 reqMeasurepara.flIr=0.0;          
            }
             if(reqMeasurepara.flIr<0.010) //|| (reqMeasurepara.flVrn < 10))
                reqMeasurepara.flIr=0.0;

                if((reqMeasurepara.flIb<0.050))
             {
               if(reqMeasurepara.flVbn!=0)           
                 reqMeasurepara.flIb=(float)((reqMeasurepara.flKVAb)/(reqMeasurepara.flVbn));
               else /*if(reqMeasurepara.flIb<=0.010)*/  
                 reqMeasurepara.flIb=0.0;          
             }
             if(reqMeasurepara.flIb<0.010) //|| (reqMeasurepara.flVbn < 10))
                 reqMeasurepara.flIb=0.0;
          }
          else if((reqConfiguration.uiSysConfiguration==2)||(reqConfiguration.uiSysConfiguration==3))   // 3 Phase 1 Element and 1 Phase
          {
              if( reqMeasurepara.flIr<0.050)
         {
              if(reqMeasurepara.flVrn!=0)           
                 reqMeasurepara.flIr=(float)((reqMeasurepara.flKVAr)/(reqMeasurepara.flVrn));
             else /*if(reqMeasurepara.flIr<=0.010)*/  
                 reqMeasurepara.flIr=0.0;          
            }
             if(reqMeasurepara.flIr<0.010) //|| (reqMeasurepara.flVrn < 10))
                reqMeasurepara.flIr=0.0;
          }
         /******************* Power offset when current input not given**************/

         if(reqMeasurepara.flIr==0)                                                               // if R phase current 0 All powers of R phase made 0 (to nullify offset in power)
        {
          reqMeasurepara.flKWr  =0.0;
          reqMeasurepara.flVARr =0.0;
          reqMeasurepara.flKVAr =0.0;
        }
        if(reqMeasurepara.flIy==0)                                                                // if Y phase current 0 All powers of Y phase made 0 (to nullify offset in power)
        {
          reqMeasurepara.flKWy  =0.0;
          reqMeasurepara.flKVAy =0.0;
          reqMeasurepara.flVARy =0.0;
        }
        if(reqMeasurepara.flIb==0)                                                                // if B phase current 0 All powers of B phase made 0 (to nullify offset in power)
        {
          reqMeasurepara.flKWb  =0.0;
          reqMeasurepara.flKVAb =0.0;
          reqMeasurepara.flVARb =0.0;
        }
        //********************************** LOCK FOR 0 CURRENT OFFSET ********************************************************/

         if((reqMeasurepara.flIr > 0.100))
         {
            if((reqMeasurepara.flKWr <0.5)&&(reqMeasurepara.flKWr >-0.5)){reqMeasurepara.flKWr =0.0;}
            if((reqMeasurepara.flVARr<0.5)&&(reqMeasurepara.flVARr>-0.5)){reqMeasurepara.flVARr=0.0;}
            if((reqMeasurepara.flKVAr<0.5)&&(reqMeasurepara.flKVAr>-0.5)){reqMeasurepara.flKVAr=0.0;}
         }
         if(( reqMeasurepara.flIy > 0.100))
         {
            if((reqMeasurepara.flKWy <0.5)&&(reqMeasurepara.flKWy >-0.5)){reqMeasurepara.flKWy =0.0;}
            if((reqMeasurepara.flVARy<0.5)&&(reqMeasurepara.flVARy>-0.5)){reqMeasurepara.flVARy=0.0;}
            if((reqMeasurepara.flKVAy<0.5)&&(reqMeasurepara.flKVAy>-0.5)){reqMeasurepara.flKVAy=0.0;}
         }
         if(( reqMeasurepara.flIb > 0.100))
         {
            if((reqMeasurepara.flKWb <0.5)&&(reqMeasurepara.flKWb >-0.5)){reqMeasurepara.flKWb =0.0;}
            if((reqMeasurepara.flVARb<0.5)&&(reqMeasurepara.flVARb>-0.5)){reqMeasurepara.flVARb=0.0;}
            if((reqMeasurepara.flKVAb<0.5)&&(reqMeasurepara.flKVAb>-0.5)){reqMeasurepara.flKVAb=0.0;}
         }

        //******************************** Average Voltage and current *************************************

         if(reqConfiguration.uiSysConfiguration==0)                                                     //3P3E
         {
           reqMeasurepara.flVllavg =(float)(( reqMeasurepara.flVry + reqMeasurepara.flVyb + reqMeasurepara.flVbr)/3.0);
           reqMeasurepara.flVlnavg =(float)(( reqMeasurepara.flVrn + reqMeasurepara.flVyn + reqMeasurepara.flVbn)/3.0);
           reqMeasurepara.flIavg   =(float)(( reqMeasurepara.flIr  + reqMeasurepara.flIy  + reqMeasurepara.flIb)/3.0);
         }
         else if (reqConfiguration.uiSysConfiguration==1)                                               //3P2E
         {
           reqMeasurepara.flVllavg = (float)(( reqMeasurepara.flVry + reqMeasurepara.flVyb)/2.0);
           reqMeasurepara.flVlnavg = 0.0;
           reqMeasurepara.flIavg   =(float)(( reqMeasurepara.flIr  + reqMeasurepara.flIb)/2.0);
         }
         else if (reqConfiguration.uiSysConfiguration==2)                                               //3P1E
         {
           reqMeasurepara.flVllavg = (float)(( reqMeasurepara.flVry + reqMeasurepara.flVyb + reqMeasurepara.flVbr)/3.0);
           reqMeasurepara.flVlnavg = (float)(( reqMeasurepara.flVrn + reqMeasurepara.flVyn + reqMeasurepara.flVbn)/3.0);
           reqMeasurepara.flIavg   = (float)(( reqMeasurepara.flIr));
         }
         else if (reqConfiguration.uiSysConfiguration==3)                                                //1P1E
         {
           reqMeasurepara.flVllavg = 0.0;
           reqMeasurepara.flVlnavg = (float)( reqMeasurepara.flVrn);
           reqMeasurepara.flIavg   = (float)( (reqMeasurepara.flIr));
         }
        //*************************************   TOTAL POWER   ****************************************************************/

         if(reqConfiguration.uiSysConfiguration==0)                                                                     //3P3E
         {
            if(reqMeasurepara.flKWr <0)
         {
           reverse_flag_r_phase=1;
         }
         else
         {
           reverse_flag_r_phase=0;
         }

          if(reqMeasurepara.flKWy <0)
         {
           reverse_flag_y_phase=1;
         }
         else
         {
           reverse_flag_y_phase=0;
         }
          if(reqMeasurepara.flKWb <0)
         {
           reverse_flag_b_phase=1;
         }
         else
         {
           reverse_flag_b_phase=0;
         }
              reqMeasurepara.flKWTot  = ((reqMeasurepara.flKWr  + reqMeasurepara.flKWy  + reqMeasurepara.flKWb));
               if((reqMeasurepara.flKWTot > KW_TOT_NEGATIVE_SET)&&(reqMeasurepara.flKWTot < KW_TOT_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flKWTot =0.0;
              }

              reqMeasurepara.flVARTot = ((reqMeasurepara.flVARr + reqMeasurepara.flVARy + reqMeasurepara.flVARb));
              if((reqMeasurepara.flVARTot > VAR_TOT_NEGATIVE_SET)&&(reqMeasurepara.flVARTot < VAR_TOT_POSITIVE_SET))     // 0 degree reactive 10 var offset
              {
               reqMeasurepara.flVARTot =0.0;
              }
              reqMeasurepara.flKVATot = (sqrt(((reqMeasurepara.flKWTot) * (reqMeasurepara.flKWTot))\
                                         + (( reqMeasurepara.flVARTot)  * ( reqMeasurepara.flVARTot))));
         }
         else if(reqConfiguration.uiSysConfiguration==1)                                                                //  3P2E
         {
                           if(reqMeasurepara.flKWr <0)
                           {
                             reverse_flag_r_phase=1;
                           }
                           else
                           {
                             reverse_flag_r_phase=0;
                           }
                            if(reqMeasurepara.flKWb <0)
                           {
                             reverse_flag_b_phase=1;
                           }
                           else
                           {
                             reverse_flag_b_phase=0;
                           }

                           reverse_flag_y_phase = 0;

              reqMeasurepara.flKWTot  = ((reqMeasurepara.flKWr  + reqMeasurepara.flKWb));
                if((reqMeasurepara.flKWTot > KW_TOT_NEGATIVE_SET)&&(reqMeasurepara.flKWTot < KW_TOT_POSITIVE_SET))     // 90degree active 10W offset
              {
                reqMeasurepara.flKWTot =0.0;
              }
              reqMeasurepara.flVARTot = ((reqMeasurepara.flVARr + reqMeasurepara.flVARb));
                if((reqMeasurepara.flVARTot > VAR_TOT_NEGATIVE_SET)&&(reqMeasurepara.flVARTot < VAR_TOT_POSITIVE_SET))     // 0 degree reactive 10 var offset
                  {
                reqMeasurepara.flVARTot =0.0;
              }
              reqMeasurepara.flKVATot = (sqrt(((reqMeasurepara.flKWTot) * (reqMeasurepara.flKWTot))\
                                        +   (( reqMeasurepara.flVARTot) * ( reqMeasurepara.flVARTot))));
         }
         else if (reqConfiguration.uiSysConfiguration==2)                                                               //3P1E
         {
                        if(reqMeasurepara.flKWr <0)
                           {
                             reverse_flag_r_phase=1;
                           }
                           else
                           {
                             reverse_flag_r_phase=0;
                           }

                        reverse_flag_y_phase = 0;
                        reverse_flag_b_phase = 0;

              reqMeasurepara.flKWTot  = (float)(reqMeasurepara.flKWr);
            if((reqMeasurepara.flKWTot >-2.8)&&(reqMeasurepara.flKWTot <2.8))     // 90degree active 10W offset
              {
                reqMeasurepara.flKWTot =0.0;
               }
              reqMeasurepara.flVARTot = (float)(reqMeasurepara.flVARr);
            if((reqMeasurepara.flVARTot >-2.8)&&(reqMeasurepara.flVARTot <2.8))     // 0 degree reactive 10 var offset
              {
                reqMeasurepara.flVARTot =0.0;
               }
              reqMeasurepara.flKVATot = (float)(reqMeasurepara.flKVAr);
         }
         else if(reqConfiguration.uiSysConfiguration==3) //1P1E
         {
                          if(reqMeasurepara.flKWr <0)
                           {
                             reverse_flag_r_phase=1;
                           }
                           else
                           {
                             reverse_flag_r_phase=0;
                           }

                            reverse_flag_y_phase = 0;
                            reverse_flag_b_phase = 0;

              reqMeasurepara.flKWTot  = (reqMeasurepara.flKWr);
           
              reqMeasurepara.flVARTot = (reqMeasurepara.flVARr);
        
              reqMeasurepara.flKVATot = (sqrt(((reqMeasurepara.flKWTot) * (reqMeasurepara.flKWTot))\
                                         +  (( reqMeasurepara.flVARTot) * ( reqMeasurepara.flVARTot))));
                  reqMeasurepara.flKWTot  = (float)(reqMeasurepara.flKWr);
              if((reqMeasurepara.flKWTot >-2.0)&&(reqMeasurepara.flKWTot <2.0))     // 90degree active 10W offset
              {
                reqMeasurepara.flKWTot =0.0;
               }
              reqMeasurepara.flVARTot = (float)(reqMeasurepara.flVARr);
               if((reqMeasurepara.flVARTot >-2.0)&&(reqMeasurepara.flVARTot <2.0))     // 0 degree reactive 10 var offset
              {
                reqMeasurepara.flVARTot =0.0;
               }
         }


         /********************************* R,Y & B PHASE POWER FACTOR  *********************************************/


         if(reqMeasurepara.flKWTot<0)
        {
          reverse_flag=1;
          Energy_watt_tot = reqMeasurepara.flKWTot*(-1);
        }

        else
        {
          reverse_flag=0;
          Energy_watt_tot = reqMeasurepara.flKWTot;
        }
//////////////////////////////////////////////////////////////////////////////////////////////
         reqMeasurepara.flPFr=(float)((reqMeasurepara.flKWr)/(reqMeasurepara.flKVAr));// R phase PF calculation

         z= (float)(acos(reqMeasurepara.flPFr));                                                             // R phase Phase angle calculation in radian
         reqMeasurepara.AngL_R=(float)((180.0/3.141593)*z);                                                  // R phase Phase angle radian to degree conversion

      
  ////////////////////////////////////////////////////////////////////////////////
       ///angle lock//
         if((reqMeasurepara.AngL_R>355.0)||(reqMeasurepara.AngL_R <8.0)||((reqMeasurepara.AngL_R>175.0)&&(reqMeasurepara.AngL_R <185.0)))
          {
           reqMeasurepara.flVARr=0.0;
          }
          if(((reqMeasurepara.AngL_R >88.0)&&(reqMeasurepara.AngL_R <92.0))||((reqMeasurepara.AngL_R >265.0)&&(reqMeasurepara.AngL_R <275.0)))
          {
           reqMeasurepara.flKWr=0.0;
          }

/////////////////////////////////////////////////////////////////////////////////////////
         reqMeasurepara.flPFy=(float)((reqMeasurepara.flKWy)/(reqMeasurepara.flKVAy));                       // Y phase PF calculation
       

         z= (float)(acos(reqMeasurepara.flPFy));                                                             // Y phase Phase angle calculation in radian
         reqMeasurepara.AngL_Y=(float)((180.0/3.141593)*z);                                                  // Y phase Phase angle radian to degree conversion
///////////////////////////////////////////////////////////////////////////////////////////
         if((reqMeasurepara.AngL_Y>355.0)||(reqMeasurepara.AngL_Y <8.0)||((reqMeasurepara.AngL_Y>175.0)&&(reqMeasurepara.AngL_Y <185.0)))
          {
           reqMeasurepara.flVARy=0.0;
          }
          if(((reqMeasurepara.AngL_Y >88.0)&&(reqMeasurepara.AngL_Y <92.0))||((reqMeasurepara.AngL_Y >265.0)&&(reqMeasurepara.AngL_Y <275.0)))
          {
           reqMeasurepara.flKWy=0.0;
          }
         ////////////////////////////////////////////////////////////////////////////////
         reqMeasurepara.flPFb=(float)((reqMeasurepara.flKWb)/(reqMeasurepara.flKVAb));                       // B phase PF calculation
        
         z= (float)(acos(reqMeasurepara.flPFb));                                                             // B phase Phase angle calculation in radian
         reqMeasurepara.AngL_B=(float)((180.0/3.141593)*z);                                                  // B phase Phase angle radian to degree conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////
         if((reqMeasurepara.AngL_B>355.0)||(reqMeasurepara.AngL_B <8.0)||((reqMeasurepara.AngL_B>175.0)&&(reqMeasurepara.AngL_B <185.0)))
          {
           reqMeasurepara.flVARb=0.0;
          }
          if(((reqMeasurepara.AngL_B >88.0)&&(reqMeasurepara.AngL_B <92.0))||((reqMeasurepara.AngL_B >265.0)&&(reqMeasurepara.AngL_B <275.0)))
          {
           reqMeasurepara.flKWb=0.0;
          }

        /************************************* SYSTEM POWER FACTOR   ************************************************/
         if(reqConfiguration.uiSysConfiguration==0)         {                                                  // 3P3E
           reqMeasurepara.flPFavg=(float)((reqMeasurepara.flKWTot)/(reqMeasurepara.flKVATot));             // Total power factor of system in 3P3E
           //IANET:091122
           if((reqMeasurepara.AngL_R>355.0)||(reqMeasurepara.AngL_R <8.0)||((reqMeasurepara.AngL_R>175.0)&&(reqMeasurepara.AngL_R <185.0))){
             ucRstVarCntr++;
           }
           if((reqMeasurepara.AngL_Y>355.0)||(reqMeasurepara.AngL_Y <8.0)||((reqMeasurepara.AngL_Y>175.0)&&(reqMeasurepara.AngL_Y <185.0))){
             ucRstVarCntr++;
           }
           if((reqMeasurepara.AngL_B>355.0)||(reqMeasurepara.AngL_B <8.0)||((reqMeasurepara.AngL_B>175.0)&&(reqMeasurepara.AngL_B <185.0))){
             ucRstVarCntr++;
           }
           if(ucRstVarCntr == 3)
             reqMeasurepara.flVARTot = 0.0;
           ucRstVarCntr = 0; 
           if(((reqMeasurepara.AngL_R >88.0)&&(reqMeasurepara.AngL_R <92.0))||((reqMeasurepara.AngL_R >265.0)&&(reqMeasurepara.AngL_R <275.0)))
             ucRstVarCntr++;
           if(((reqMeasurepara.AngL_Y >88.0)&&(reqMeasurepara.AngL_Y <92.0))||((reqMeasurepara.AngL_Y >265.0)&&(reqMeasurepara.AngL_Y <275.0)))
             ucRstVarCntr++;
           if(((reqMeasurepara.AngL_B >88.0)&&(reqMeasurepara.AngL_B <92.0))||((reqMeasurepara.AngL_B >265.0)&&(reqMeasurepara.AngL_B <275.0)))
             ucRstVarCntr++;
         
          if(ucRstVarCntr == 3)
            reqMeasurepara.flKWTot=0.0;
          ucRstVarCntr = 0;
         }        
         else if(reqConfiguration.uiSysConfiguration==1){                                                      // 3P2E
          reqMeasurepara.flPFavg=(float)((reqMeasurepara.flKWTot)/(reqMeasurepara.flKVATot));              // Total power factor of system in 3P2E
          //IANET: 091122
          ucRstVarCntr = 0; 
          if(((reqMeasurepara.AngL_R>25.0)&&(reqMeasurepara.AngL_R <35.0))||((reqMeasurepara.AngL_R>145.0)&&(reqMeasurepara.AngL_R <155.0)))
            ucRstVarCntr++;
          if(((reqMeasurepara.AngL_B>25.0)&&(reqMeasurepara.AngL_B <35.0))||((reqMeasurepara.AngL_B>145.0)&&(reqMeasurepara.AngL_B <155.0)))
            ucRstVarCntr++;
          if(ucRstVarCntr == 2)
            reqMeasurepara.flVARTot = 0.0;
          ucRstVarCntr = 0;
          
          if(((reqMeasurepara.AngL_R >118.0)&&(reqMeasurepara.AngL_R <122.0))||((reqMeasurepara.AngL_R >298.0)&&(reqMeasurepara.AngL_R <302.0)))    
            ucRstVarCntr++;
          if(((reqMeasurepara.AngL_B >58.0)&&(reqMeasurepara.AngL_B <62.0))||((reqMeasurepara.AngL_B >238.0)&&(reqMeasurepara.AngL_B <242.0)))
            ucRstVarCntr++;
          if(ucRstVarCntr == 2)
           reqMeasurepara.flKWTot=0.0;
          ucRstVarCntr = 0;
         }
         else if(reqConfiguration.uiSysConfiguration==2){                                                      // 3P1E
          reqMeasurepara.flPFavg=(float)((reqMeasurepara.flKWTot)/(reqMeasurepara.flKVATot));             // Total power factor of system in 3P1E
          if((reqMeasurepara.AngL_R > 355.0)||(reqMeasurepara.AngL_R < 8.0)||((reqMeasurepara.AngL_R > 175.0)&&(reqMeasurepara.AngL_R < 185.0)))
           reqMeasurepara.flVARTot = 0.0;
          if(((reqMeasurepara.AngL_R > 88.0)&&(reqMeasurepara.AngL_R < 92.0))||((reqMeasurepara.AngL_R > 265.0)&&(reqMeasurepara.AngL_R < 275.0)))
            reqMeasurepara.flKWTot = 0.0;          
         }
         else if(reqConfiguration.uiSysConfiguration==3){                                                      // 1P1E
          reqMeasurepara.flPFavg=(float)((reqMeasurepara.flKWTot)/(reqMeasurepara.flKVATot));             // Total power factor of system in 1P1E
          if((reqMeasurepara.AngL_R > 355.0)||(reqMeasurepara.AngL_R < 8.0)||((reqMeasurepara.AngL_R > 175.0)&&(reqMeasurepara.AngL_R < 185.0)))
           reqMeasurepara.flVARTot = 0.0;
          if(((reqMeasurepara.AngL_R > 88.0)&&(reqMeasurepara.AngL_R < 92.0))||((reqMeasurepara.AngL_R > 265.0)&&(reqMeasurepara.AngL_R < 275.0)))
            reqMeasurepara.flKWTot = 0.0;
         }
    
       /*************************************  PHASE ANGLE   ************************************************/

        z= (float)(acos(reqMeasurepara.flPFavg));                                                           //  TOTAL Phase angle calculation in radian
        reqMeasurepara.AngL=(float)((180.0/3.141593)*z);                                                    //  TOTAL Phase angle radian to degree conversion

        if((reqMeasurepara.flVARTot < 0.0))                                                                 // if reactive power is negative
        {
           reqMeasurepara.AngL_R =(360.0-reqMeasurepara.AngL_R);
           reqMeasurepara.AngL_Y =(360.0-reqMeasurepara.AngL_Y);
           reqMeasurepara.AngL_B =(360.0-reqMeasurepara.AngL_B);
           reqMeasurepara.AngL   =(360.0-reqMeasurepara.AngL);
        }
      //}//IANET: 21/12/2018
      ParaFactUp();
}
float asb=0;


/***************************************************************************************************************************
Function     :  MB_PARA
Parameter    :  null
Description  :
***************************************************************************************************************************/
void MB_PARA(void)
{
  VR_MB = reqMeasurepara.flVrn;
  VY_MB = reqMeasurepara.flVyn;
  VB_MB = reqMeasurepara.flVbn;
  VLNavg_MB = reqMeasurepara.flVlnavg; //(reqMeasurepara.flVrn + reqMeasurepara.flVyn + reqMeasurepara.flVbn)/3;

  VRY_MB = reqMeasurepara.flVry;
  VYB_MB = reqMeasurepara.flVyb;
  VBR_MB = reqMeasurepara.flVbr;
  VLLavg_MB = reqMeasurepara.flVllavg; // (reqMeasurepara.flVry+reqMeasurepara.flVyb+reqMeasurepara.flVbr)/3;

  IR_MB = reqMeasurepara.flIr;
  IY_MB = reqMeasurepara.flIy;
  IB_MB = reqMeasurepara.flIb;
  Iavg_MB = reqMeasurepara.flIavg; //(reqMeasurepara.flIr+reqMeasurepara.flIy+reqMeasurepara.flIb)/3;

  Frq_MB = reqMeasurepara.flFreq;
  PFR_MB = reqMeasurepara.flPFr;
  PFY_MB = reqMeasurepara.flPFy;
  PFB_MB = reqMeasurepara.flPFb;
  PFavg_MB = reqMeasurepara.flPFavg;

  KWr_MB = reqMeasurepara.flKWr;
  KWy_MB = reqMeasurepara.flKWy;
  KWb_MB = reqMeasurepara.flKWb;
  WTot_MB = reqMeasurepara.flKWTot;

  VARr_MB = reqMeasurepara.flVARr;
  VARy_MB = reqMeasurepara.flVARy;
  VARb_MB = reqMeasurepara.flVARb;
  VARTot_MB =reqMeasurepara.flVARTot;

  KVAr_MB = reqMeasurepara.flKVAr;
  KVAy_MB = reqMeasurepara.flKVAy;
  KVAb_MB = reqMeasurepara.flKVAb;
  KVATot_MB = reqMeasurepara.flKVATot;

  KWHf_MB = KWHf;

}






/***************************************************************************************************************************
Function     :  Read_ADC
Parameter    :  null
Description  :  This function is used to read adc values from 3 phase syatem i.e Vr,Vy,Vb ,Ir,Iy & Ib
***************************************************************************************************************************/

void Read_ADC(void)
{
  unsigned int x=0;
  float flclfreq;
  if(reqMeasurepara.flFreq == 0)                   //21/12/2018
        flclfreq = 50;
  else
        flclfreq = reqMeasurepara.flFreq;

    asb=((1.0/(512.0*flclfreq))*1000000.0);
    x = (unsigned int)((asb *1000) / 125);
    RCC->APB1RSTR |=(1<<0);
    TIM2_Configuration(x);
    while(adc_count<12)
    {
      TIM_Cmd(TIM2,ENABLE);                                    //  vMBPortTimersDisable();
      DMA_Cmd(DMA1_Channel1, ENABLE);
      while(EndOfTransfer==0);
      EndOfTransfer=0;
      TIM_Cmd(TIM2,DISABLE);
      //  DMA_Cmd(DMA1_Channel1, DISABLE);
      Para_Store();
      adc_count++;
    }
  adc_count=0;
}
/***************************************************************************************************************************
Function     :  Disp_FUN
Parameter    :  null
Description  :  This function is used to transmit all paramewters to the USART
***************************************************************************************************************************/
void Disp_FUN(void)
{
  DispFloatWo(reqMeasurepara.flVrn);
  DispFloatWo(reqMeasurepara.flVyn);
  DispFloatWo(reqMeasurepara.flVbn);
  DispFloatWo(reqMeasurepara.flVry);
  DispFloatWo(reqMeasurepara.flVyb);
  DispFloatWo(reqMeasurepara.flVbr);
  DispFloatWo(reqMeasurepara.flIr);
  DispFloatWo(reqMeasurepara.flIy);
  DispFloatWo(reqMeasurepara.flIb);
  DispFloatWo(reqMeasurepara.flKWr);
  DispFloatWo(reqMeasurepara.flKWy);
  DispFloatWo(reqMeasurepara.flKWb);
  DispFloatWo(reqMeasurepara.flKWTot);
  DispFloatWo(reqMeasurepara.flVARr);
  DispFloatWo(reqMeasurepara.flVARy);
  DispFloatWo(reqMeasurepara.flVARb);
  DispFloatWo(reqMeasurepara.flVARTot);
  DispFloatWo(reqMeasurepara.flKVAr);
  DispFloatWo(reqMeasurepara.flKVAy);
  DispFloatWo(reqMeasurepara.flKVAb);
  DispFloatWo(reqMeasurepara.flKVATot);
  DispFloatWo(reqMeasurepara.flFreq);
  DispFloatWo(reqMeasurepara.AngL);
  DispFloatWo(reqMeasurepara.flPFavg);
  DispFloatWo(reqMeasurepara.flPFr);
  DispFloatWo(reqMeasurepara.flPFy);
  DispFloatWo(reqMeasurepara.flPFb);
  SendChar('Y');
  SendChar(';');
}
/***************************************************************************************************************************
Function     :  Factor_Update
Parameter    :  null
Description  : This function used to calculate the current and voltage primary to secondary ratio factor
***************************************************************************************************************************/
 void Factor_Update(void)
 {
    VoltFact = (float)((float )reqConfiguration.ulPTRPri/(float ) reqConfiguration.ulPTRSec);                          // voltage primary to secondary ration
    CurrFact = (float)((float )reqConfiguration.ulCTRPri/(float )reqConfiguration.ulCTRSec);                           // current primary to secondary ration
    PowrFact = (float)(VoltFact*CurrFact);                                                             // multiplication factor
 }

/***************************************************************************************************************************
Function     :  ParaFactUp
Parameter    :  null
Description  :
***************************************************************************************************************************/
void ParaFactUp(void)
{
    reqMeasurepara.flVrn    = VoltFact * reqMeasurepara.flVrn;
    reqMeasurepara.flVyn    = VoltFact * reqMeasurepara.flVyn;
    reqMeasurepara.flVbn    = VoltFact * reqMeasurepara.flVbn;
    reqMeasurepara.flVry    = VoltFact * reqMeasurepara.flVry;
    reqMeasurepara.flVyb    = VoltFact * reqMeasurepara.flVyb;
    reqMeasurepara.flVbr    = VoltFact * reqMeasurepara.flVbr;
    reqMeasurepara.flIr     = CurrFact * reqMeasurepara.flIr;
    reqMeasurepara.flIy     = CurrFact * reqMeasurepara.flIy;
    reqMeasurepara.flIb     = CurrFact * reqMeasurepara.flIb;
    reqMeasurepara.flKWr    = PowrFact * reqMeasurepara.flKWr;
    reqMeasurepara.flKWy    = PowrFact * reqMeasurepara.flKWy;
    reqMeasurepara.flKWb    = PowrFact * reqMeasurepara.flKWb;
    reqMeasurepara.flVARr   = PowrFact * reqMeasurepara.flVARr;
    reqMeasurepara.flVARy   = PowrFact * reqMeasurepara.flVARy;
    reqMeasurepara.flVARb   = PowrFact * reqMeasurepara.flVARb;
    reqMeasurepara.flKVAr   = PowrFact * reqMeasurepara.flKVAr;
    reqMeasurepara.flKVAy   = PowrFact * reqMeasurepara.flKVAy;
    reqMeasurepara.flKVAb   = PowrFact * reqMeasurepara.flKVAb;
    reqMeasurepara.flKWTot  = PowrFact * reqMeasurepara.flKWTot;
    reqMeasurepara.flVARTot = PowrFact * reqMeasurepara.flVARTot;
    reqMeasurepara.flKVATot = PowrFact * reqMeasurepara.flKVATot;
    reqMeasurepara.flVllavg = VoltFact * reqMeasurepara.flVllavg;
    reqMeasurepara.flIavg   = CurrFact * reqMeasurepara.flIavg;
    reqMeasurepara.flVlnavg = VoltFact * reqMeasurepara.flVlnavg;
}
/************************************************* END OF THIS FILE **************************************************************************/
