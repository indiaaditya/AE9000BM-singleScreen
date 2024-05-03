
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


float mR,mY,mB,mRY,mYB,mBR,mIR,mIY,mIB,cR,cY,cB,cRY,cYB,cBR,cIR,cIY,cIB,mWR,mWY,mWB,cWR,cWY,cWB,mWT,cWT,mVAT,cVAT;
float m_IR,m_IY,m_IB,c_IR,c_IY,c_IB;
extern MEASURED_PARAM  reqMeasurepara;
extern float z;
extern unsigned char Run_flag;
extern long double VR_cal,VY_cal,VB_cal,VRY_cal,VYB_cal,VBR_cal,IR_cal,IY_cal,IB_cal,Pr_cal,Py_cal,Pb_cal,Qr_cal,Qy_cal,Qb_cal,PT_3P2E_cal,QT_3P2E_cal;
float Angle_Diff_Tot, Calib_Pf,Watt_scaling_factor=1,Calib_Watt_Tot,Total_Angle,Calib_Pf_AngL,Angle_Diff_R,Angle_Diff_Y,Angle_Diff_B;
unsigned char Angle_Set;


extern float Freq_calib_factor;
unsigned char freq_calib_flag;


void Meter_Calib(void)
{

  float Low_R,Low_B,Low_Y,Low_RY,Low_YB,Low_BR,Low_IR,Low_IY,Low_IB,Low_WR,Low_WY,Low_WB,Low_WT3P2E,Low_VAT3P2E;
  float High_R,High_B,High_Y,High_RY,High_YB,High_BR,High_IR,High_IY,High_IB,High_WR,High_WY,High_WB,High_WT3P2E,High_VAT3P2E;

 //char l;
 float Act_LV,Act_LVV,Act_LW,Act_LTW;
 float Act_LI,Act_HI;
 float Act_HV,Act_HVV,Act_HW,Act_HTW;

 freq_calib_flag=1;
 
 FreQfeaD();
 
 freq_calib_flag=2;
 
 FreQfeaD();
 
 freq_calib_flag=3; 
 
 FreQfeaD();
 freq_calib_flag=0; 
 
 
 SendChar('Y');
 
 flot_rec_var(&Act_HV);
 flot_rec_var(&Act_HI);
 Run_flag=1;
 FreQfeaD();
 Read_ADC();
 freq_calib_flag=0;

  High_R=(float)(VR_cal);
  High_Y=(float)(VY_cal);
  High_B=(float)(VB_cal);

  High_RY=(float)(VRY_cal);
  High_YB=(float)(VYB_cal);
  High_BR=(float)(VBR_cal);
  High_IR=(float)(IR_cal);
  High_IY=(float)(IY_cal);
  High_IB=(float)(IB_cal);

  High_WR=(float)(Pr_cal);
  High_WY=(float)(Py_cal);
  High_WB=(float)(Pb_cal);

  High_WT3P2E=(float)(PT_3P2E_cal);
  High_VAT3P2E=(float)(((VRY_cal*IR_cal)+(VYB_cal*IB_cal))*0.866025);

  SendChar('Y');

  flot_rec_var(&Act_LV);
  flot_rec_var(&Act_LI);
   
  FreQfeaD();
  Read_ADC();

  Low_R=(float)(VR_cal);
  Low_Y=(float)(VY_cal);
  Low_B=(float)(VB_cal);


  Low_RY=(float)(VRY_cal);
  Low_YB=(float)(VYB_cal);
  Low_BR=(float)(VBR_cal);
  Low_IR=(float)(IR_cal);
  Low_IY=(float)(IY_cal);
  Low_IB=(float)(IB_cal);

  Low_WR=(float)(Pr_cal);
  Low_WY=(float)(Py_cal);
  Low_WB=(float)(Pb_cal);

  Low_WT3P2E=(float)(PT_3P2E_cal);
  Low_VAT3P2E=(float)(((VRY_cal*IR_cal)+(VYB_cal*IB_cal))*0.866025);

  Act_LVV=(Act_LV* 1.732051);      // 1.732051  ???
  Act_HVV=(Act_HV* 1.732051);      // 1.732051  ???
  
  Act_LW=(Act_LV*Act_LI);
  Act_HW=(Act_HV*Act_HI);
  Act_LTW=(3.0*Act_LW);
  Act_HTW=(3.0*Act_HW);


  mR=((Act_HV-Act_LV)/(High_R-Low_R));
  cR= (Act_HV-(mR*High_R));

  mY=((Act_HV-Act_LV)/(High_Y-Low_Y));
  cY= (Act_HV-(mY*High_Y));

  mB=((Act_HV-Act_LV)/(High_B-Low_B));
  cB= (Act_HV-(mB*High_B));

  mRY=((Act_HVV-Act_LVV)/(High_RY-Low_RY));
  cRY= (Act_HVV-(mRY*High_RY));

  mYB=((Act_HVV-Act_LVV)/(High_YB-Low_YB));
  cYB= (Act_HVV-(mYB*High_YB));

  mBR=( (Act_HVV-Act_LVV)/(High_BR-Low_BR));
  cBR= (Act_HVV-(mBR*High_BR));

  mIR=((Act_HI-Act_LI)/(High_IR-Low_IR));
  cIR= (Act_HI-(mIR*High_IR));

  mIY=((Act_HI-Act_LI)/(High_IY-Low_IY));
  cIY= (Act_HI-(mIY*High_IY));

   mIB=((Act_HI-Act_LI)/(High_IB-Low_IB));
   cIB= (Act_HI-(mIB*High_IB));

     /**** watt m and c ****/
   mWR=((Act_HW-Act_LW)/(High_WR-Low_WR));
   cWR= (Act_HW-(mWR*High_WR));

   mWY=((Act_HW-Act_LW)/(High_WY-Low_WY));
   cWY= (Act_HW-(mWY*High_WY));

   mWB=((Act_HW-Act_LW)/(High_WB-Low_WB));
   cWB= (Act_HW-(mWB*High_WB));

   /******** 3p2e ************/

    mWT=( (Act_HTW-Act_LTW)/(High_WT3P2E-Low_WT3P2E));
    cWT= (Act_HTW-(mWT*High_WT3P2E));

    mVAT=((Act_HTW-Act_LTW)/(High_VAT3P2E-Low_VAT3P2E));
    cVAT= (Act_HTW-(mVAT*High_VAT3P2E));

   /******** 3p2e ************/

    SendChar('Y');
    
    High_IR=0; High_IY=0; High_IB=0;
    Low_IR=0;  Low_IY=0;  Low_IB=0;
    Act_HI=0;  Act_LI=0;
    
    flot_rec_var(&Act_HI);                   //0.8 00

    FreQfeaD();
    Read_ADC();
    
    High_IR=(float)(IR_cal);
    High_IY=(float)(IY_cal);
    High_IB=(float)(IB_cal);

    SendChar('Y');

    flot_rec_var(&Act_LI);

     FreQfeaD();
     Read_ADC();

     Low_IR=(float)(IR_cal);
     Low_IY=(float)(IY_cal);
     Low_IB=(float)(IB_cal);

     m_IR=((Act_HI-Act_LI)/(High_IR-Low_IR));
     c_IR= (Act_HI-(m_IR*High_IR));

     m_IY=((Act_HI-Act_LI)/(High_IY-Low_IY));
     c_IY= (Act_HI-(m_IY*High_IY));

     m_IB=((Act_HI-Act_LI)/(High_IB-Low_IB));
     c_IB= (Act_HI-(m_IB*High_IB));


      SendChar('Y');
      WRITE_EEPROM();
      SendChar('S');

}