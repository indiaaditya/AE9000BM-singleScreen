
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
extern MEASURED_PARAM  reqMeasurepara;
extern CONFIGURATION_PARAM reqConfiguration;
extern UnCal reqUncal;
extern unsigned char freq_comp,Run_flag;
extern NVIC_InitTypeDef NVIC_InitStructure;
extern EXTI_InitTypeDef   EXTI_InitStructure;
extern float z;
extern unsigned char EndOfTransfer,adc_count;
extern float Angle_Diff_Tot, Calib_Pf,Watt_scaling_factor,Calib_Watt_Tot,Angle_Diff_R,Angle_Diff_Y,Angle_Diff_B;
extern unsigned int asb,pf_error;
/* Changes to add per phase PF*/
extern unsigned char pfr_error,pfy_error,pfb_error;
float Tot_VAR_Pf,R_VAR_Pf,Y_VAR_Pf,B_VAR_Pf;
extern unsigned long int No_ip;
float R_watt,Y_watt,B_watt,R_var,Y_var,B_var,Energy_var_tot,Energy_watt_tot;
extern float VoltFact,CurrFact,PowrFact;
extern long double VR_cal,VY_cal,VB_cal,VRY_cal,VYB_cal,VBR_cal,IR_cal,IY_cal,IB_cal,Pr_cal,Py_cal,Pb_cal,Qr_cal,Qy_cal,Qb_cal,PT_3P2E_cal,QT_3P2E_cal;
extern float mR,mY,mB,mRY,mYB,mBR,mIR,mIY,mIB,cR,cY,cB,cRY,cYB,cBR,cIR,cIY,cIB,mWR,mWY,mWB,cWR,cWY,cWB,mWT,cWT,mVAT,cVAT;
float Angle_R_Phase_3P2E,Angle_B_Phase_3P2E,PF_R_Phase_3P2E,PF_B_Phase_3P2E;
unsigned int IntPFr,IntPFy,IntPFb;
extern unsigned char R_quadrant,Y_quadrant,B_quadrant;
//unsigned char Tot_quadrant;
void Calculation_Power()
{

  if(reqMeasurepara.flPFr >1)
    reqMeasurepara.flPFr=1;
  
  if(reqMeasurepara.flPFy >1)
    reqMeasurepara.flPFy = 1;
  
  if(reqMeasurepara.flPFb >1)
    reqMeasurepara.flPFb = 1;
 
 
 //-------------------------Quadrant currection--------------------
 
    if(R_quadrant!=0)
       reqMeasurepara.AngL_R=360.00- reqMeasurepara.AngL_R;
    if(Y_quadrant!=0)
       reqMeasurepara.AngL_Y=360.00- reqMeasurepara.AngL_Y;
    if(B_quadrant!=0)
       reqMeasurepara.AngL_B=360.00- reqMeasurepara.AngL_B;
    
  /*------------------R-Phase-PF--------------------------*/
  
    if((reqMeasurepara.AngL_R< 91.500 && reqMeasurepara.AngL_R>88.500)||(reqMeasurepara.AngL_R< 271.500 && reqMeasurepara.AngL_R>268.500))    // for 90 and 270
      reqMeasurepara.AngL_R=reqMeasurepara.AngL_R;
    
    else if((reqMeasurepara.AngL_R< 1.500 || reqMeasurepara.AngL_R>358.500)||(reqMeasurepara.AngL_R< 181.500 && reqMeasurepara.AngL_R>178.500))   // for 0 and 180
       reqMeasurepara.AngL_R=reqMeasurepara.AngL_R;
    else
        reqMeasurepara.AngL_R= reqMeasurepara.AngL_R + Angle_Diff_R;
   
  /*  if((reqMeasurepara.AngL_R< 94.00 && reqMeasurepara.AngL_R>83.00)||(reqMeasurepara.AngL_R< 274.0 && reqMeasurepara.AngL_R>263.000))    // for 90 and 270
    {
      IntPFr=(unsigned int)(Angle_Diff_R*100);
         Angle_Diff_R=((float)IntPFr/100);
         reqMeasurepara.AngL_R= reqMeasurepara.AngL_R - Angle_Diff_R;
     }
    
    else if((reqMeasurepara.AngL_R< 6.000 || reqMeasurepara.AngL_R>353.000)||(reqMeasurepara.AngL_R< 186.000 && reqMeasurepara.AngL_R>173.000))   // for 0 and 180
    {  
        
        IntPFr=(unsigned int)(Angle_Diff_R*10000);
         Angle_Diff_R=((float)IntPFr/10000);
         reqMeasurepara.AngL_R= reqMeasurepara.AngL_R - Angle_Diff_R;
     }*/
   
     
     reqMeasurepara.flPFr = (float)(cos(reqMeasurepara.AngL_R*(3.141593/180)));
  
  
  
  /*------------------Y-Phase-PF--------------------------*/
 
    /* if((reqMeasurepara.AngL_Y< 91.500 && reqMeasurepara.AngL_Y>88.500)||(reqMeasurepara.AngL_Y< 271.500 && reqMeasurepara.AngL_Y>268.500))    // for 90 and 270
      reqMeasurepara.AngL_Y=reqMeasurepara.AngL_Y;
    
    else if((reqMeasurepara.AngL_Y< 1.500 || reqMeasurepara.AngL_Y>358.500)||(reqMeasurepara.AngL_Y< 181.500 && reqMeasurepara.AngL_Y>178.500))   // for 0 and 180
       reqMeasurepara.AngL_Y=reqMeasurepara.AngL_Y;
      
   /* else if((reqMeasurepara.AngL_Y< 95.500 && reqMeasurepara.AngL_Y>84.500)||(reqMeasurepara.AngL_Y< 275.500 && reqMeasurepara.AngL_Y>264.500))    // for 90 and 270
    {   
         IntPFy=(unsigned int)(reqMeasurepara.flPFy);
         Angle_Diff_Y=(Angle_Diff_Y-IntPFy);
         reqMeasurepara.AngL_Y= reqMeasurepara.AngL_Y + Angle_Diff_Y;
    }
    
    else if((reqMeasurepara.AngL_Y< 5.500 || reqMeasurepara.AngL_Y>354.500)||(reqMeasurepara.AngL_Y< 185.500 && reqMeasurepara.AngL_Y>174.500))   // for 0 and 180
    {  
         IntPFy=(unsigned int)(reqMeasurepara.flPFy);
         Angle_Diff_Y=(Angle_Diff_Y-IntPFy);
         reqMeasurepara.AngL_Y= reqMeasurepara.AngL_Y + Angle_Diff_Y;
     }*/
     //  else
        reqMeasurepara.AngL_Y = reqMeasurepara.AngL_Y + Angle_Diff_Y;
     
       reqMeasurepara.flPFy = (float)(cos(reqMeasurepara.AngL_Y*(3.141593/180)));  
  
  /*------------------B-Phase-PF--------------------------*/
    
    /* if((reqMeasurepara.AngL_B< 91.500 )&& (reqMeasurepara.AngL_B>88.500))    // for 90 and 270
      reqMeasurepara.AngL_Y=reqMeasurepara.AngL_Y;
    else if((reqMeasurepara.AngL_B< 271.500) && (reqMeasurepara.AngL_B>267.500))
         reqMeasurepara.AngL_Y=reqMeasurepara.AngL_Y;
     else if((reqMeasurepara.AngL_B< 1.500 || reqMeasurepara.AngL_B>358.500))   // for 0 and 180
       reqMeasurepara.AngL_Y=reqMeasurepara.AngL_Y;
     else if((reqMeasurepara.AngL_B<=181.500) && (reqMeasurepara.AngL_B>=178.500))
           reqMeasurepara.AngL_Y=reqMeasurepara.AngL_Y;
   
    /* else if((reqMeasurepara.AngL_B< 95.500 && reqMeasurepara.AngL_B>84.500)||(reqMeasurepara.AngL_B< 275.500 && reqMeasurepara.AngL_B>264.500))    // for 90 and 270
      { 
        IntPFb=(signed int)(reqMeasurepara.flPFb);
         Angle_Diff_B=(Angle_Diff_B-IntPFb);
         reqMeasurepara.AngL_B= reqMeasurepara.AngL_B + Angle_Diff_B;
      }
    
    else if((reqMeasurepara.AngL_B< 5.500 || reqMeasurepara.AngL_B>354.500)||(reqMeasurepara.AngL_B< 185.500 && reqMeasurepara.AngL_B>174.500))   // for 0 and 180
    {  
        IntPFb=(signed int)(reqMeasurepara.flPFb);
         Angle_Diff_B=(Angle_Diff_B-IntPFb);
         reqMeasurepara.AngL_B= reqMeasurepara.AngL_B + Angle_Diff_B;
     } */
    // else
        reqMeasurepara.AngL_B = reqMeasurepara.AngL_B + Angle_Diff_B;
    
     reqMeasurepara.flPFb = (float)(cos(reqMeasurepara.AngL_B*(3.141593/180)));  
   
   
     /*-------------------R-Phase_Power------------------------*/
    if (reqConfiguration.uiSysConfiguration==1) //3P2E
    {
        if(reqMeasurepara.flIr ==0)
          {
             reqMeasurepara.flKWr =0;
             reqMeasurepara.flVARr =0;
             reqMeasurepara.flKVAr =0;
          }
          else
          {
               Angle_R_Phase_3P2E =(reqMeasurepara.AngL_R);
               PF_R_Phase_3P2E= (float)(cos(Angle_R_Phase_3P2E*(3.141593/180)));
            
              reqMeasurepara.flKWr = (float)(reqMeasurepara.flVry*reqMeasurepara.flIr*PF_R_Phase_3P2E);
              R_VAR_Pf = (float)(sin(Angle_R_Phase_3P2E*(3.141593/180)));
              reqMeasurepara.flVARr = (float)(reqMeasurepara.flVry*reqMeasurepara.flIr* R_VAR_Pf);
              reqMeasurepara.flKVAr = (float)(reqMeasurepara.flVry*reqMeasurepara.flIr);
          }
         
    }
    else
    {
       if(reqMeasurepara.flIr ==0)              // if R-PHASE CURRENT IS ZERO
          {
             reqMeasurepara.flKWr =0;
             reqMeasurepara.flVARr =0;
             reqMeasurepara.flKVAr =0;
          }
          else
          {
            reqMeasurepara.flKWr = (float)(reqMeasurepara.flVrn*reqMeasurepara.flIr*reqMeasurepara.flPFr);
            R_VAR_Pf = (float)(sin(reqMeasurepara.AngL_R*(3.141593/180)));
            reqMeasurepara.flVARr = (float)(reqMeasurepara.flVrn*reqMeasurepara.flIr* R_VAR_Pf);
            reqMeasurepara.flKVAr = (float)(reqMeasurepara.flVrn*reqMeasurepara.flIr);
          }
    }
    
    /*-------------------Y-Phase_Power------------------------*/
   
    if(reqConfiguration.uiSysConfiguration==0) //3P3E
     {
        if(reqMeasurepara.flIy ==0)              // IF Y PHASE CURRENT IS ZERO
          {
             reqMeasurepara.flKWy =0;
             reqMeasurepara.flVARy =0;
             reqMeasurepara.flKVAy =0;
          }
          else
          {
            reqMeasurepara.flKWy =(float)(reqMeasurepara.flVyn*reqMeasurepara.flIy*reqMeasurepara.flPFy);
            Y_VAR_Pf = (float)(sin(reqMeasurepara.AngL_Y*(3.141593/180)));
            reqMeasurepara.flVARy = (float)(reqMeasurepara.flVyn*reqMeasurepara.flIy* Y_VAR_Pf);
            reqMeasurepara.flKVAy = (float)(reqMeasurepara.flVyn*reqMeasurepara.flIy);
          }
    }
    else
    {
      reqMeasurepara.flKWy =0;
      reqMeasurepara.flVARy=0;
      reqMeasurepara.flKVAy=0;
          
    }
    /*-------------------B-Phase_Power------------------------*/
   
   if((reqConfiguration.uiSysConfiguration==1)||(reqConfiguration.uiSysConfiguration==0)) //3P2E and 3p3e
  {  
    if (reqConfiguration.uiSysConfiguration==1) //3P2E
      {
         if(reqMeasurepara.flIb ==0)               //if B PHASE CURRENNT IS ZERO
         {
             reqMeasurepara.flKWb =0;
             reqMeasurepara.flVARb =0;
             reqMeasurepara.flKVAb =0;
          }
          else
          {
            
                  Angle_B_Phase_3P2E  = (reqMeasurepara.AngL_B);
                   PF_B_Phase_3P2E= (float)(cos(Angle_B_Phase_3P2E*(3.141593/180)));
                   
              reqMeasurepara.flKWb =(float)(reqMeasurepara.flVyb*reqMeasurepara.flIb*PF_B_Phase_3P2E);
              B_VAR_Pf =(float)(sin(Angle_B_Phase_3P2E*(3.141593/180)));
              reqMeasurepara.flVARb =(float)(reqMeasurepara.flVyb*reqMeasurepara.flIb* B_VAR_Pf);
              reqMeasurepara.flKVAb =(float)(reqMeasurepara.flVyb*reqMeasurepara.flIb);
          }
      }
  
    else
    {
      
         if(reqMeasurepara.flIb ==0)               //if B PHASE CURRENNT IS ZERO
         {
             reqMeasurepara.flKWb =0;
             reqMeasurepara.flVARb =0;
             reqMeasurepara.flKVAb =0;
          }
          else
          {
              reqMeasurepara.flKWb =(float)(reqMeasurepara.flVbn*reqMeasurepara.flIb*reqMeasurepara.flPFb);
              B_VAR_Pf =(float)(sin(reqMeasurepara.AngL_B*(3.141593/180)));
              reqMeasurepara.flVARb =(float)(reqMeasurepara.flVbn*reqMeasurepara.flIb* B_VAR_Pf);
              reqMeasurepara.flKVAb =(float)(reqMeasurepara.flVbn*reqMeasurepara.flIb);
         }
    }
  }  
   
     
    /*------------------------Total_Power---------------------*/
    if(reqConfiguration.uiSysConfiguration==0) //3P3E
    {
     reqMeasurepara.flKWTot = ((reqMeasurepara.flKWr+reqMeasurepara.flKWy+reqMeasurepara.flKWb));
     reqMeasurepara.flVARTot = ((reqMeasurepara.flVARr+reqMeasurepara.flVARy+reqMeasurepara.flVARb));
     reqMeasurepara.flKVATot = (sqrt(((reqMeasurepara.flKWTot)*(reqMeasurepara.flKWTot))+(( reqMeasurepara.flVARTot)*( reqMeasurepara.flVARTot))));
    }
   else if(reqConfiguration.uiSysConfiguration==1) //  3P2E
    {
        reqMeasurepara.flKWTot = ((reqMeasurepara.flKWr+reqMeasurepara.flKWb));
        reqMeasurepara.flVARTot = ((reqMeasurepara.flVARr+reqMeasurepara.flVARb));
         reqMeasurepara.flKVATot = (sqrt(((reqMeasurepara.flKWTot)*(reqMeasurepara.flKWTot))+(( reqMeasurepara.flVARTot)*( reqMeasurepara.flVARTot))));
    }
    else if (reqConfiguration.uiSysConfiguration==2) //3P1E
    {
     reqMeasurepara.flKWTot=(float)(reqMeasurepara.flKWr);
     reqMeasurepara.flVARTot=(float)(reqMeasurepara.flVARr);
     reqMeasurepara.flKVATot=(float)(reqMeasurepara.flKVAr);
    }
    
    else if(reqConfiguration.uiSysConfiguration==3) //1P1E
    {
     reqMeasurepara.flKWTot = (reqMeasurepara.flKWr);
     reqMeasurepara.flVARTot = (reqMeasurepara.flVARr);
    reqMeasurepara.flKVATot = (sqrt(((reqMeasurepara.flKWTot)*(reqMeasurepara.flKWTot))+(( reqMeasurepara.flVARTot)*( reqMeasurepara.flVARTot))));
     
    }
  //------------------------AVG POWER FACTOR--------------------------
 
    reqMeasurepara.flPFavg = (reqMeasurepara.flKWTot /reqMeasurepara.flKVATot);
      z= (float)(acos(reqMeasurepara.flPFavg));
       reqMeasurepara.AngL=(float)((180.0/3.141593)*z);
       
         if((reqMeasurepara.flKWTot<0) &&(reqMeasurepara.flVARTot <0))
         Tot_quadrant=3;
       
       else if((reqMeasurepara.flKWTot>0) &&(reqMeasurepara.flVARTot <0))
         Tot_quadrant=4;
        else
          Tot_quadrant=0;
        
       if(Tot_quadrant!=0)
       reqMeasurepara.AngL=360.00- reqMeasurepara.AngL;
     
}