
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

#define BUFF_SIZE 5200

extern MEASURED_PARAM  reqMeasurepara;
unsigned int FindSqrt(unsigned long );
long double VR_cal,VY_cal,VB_cal,VRY_cal,VYB_cal,VBR_cal,IR_cal,IY_cal,IB_cal,Pr_cal,Py_cal,Pb_cal,Qr_cal,Qy_cal,Qb_cal,PT_3P2E_cal,QT_3P2E_cal;


//long double
long double  temp_R,temp_Y,temp_B,temp_RY,temp_YB,temp_BR,temp_IR,temp_IY,temp_IB;
long double temp_Pr,temp_Pr_3P2E,temp_Py,temp_Pb,temp_Pb_3P2E,temp_Qr,temp_Qr_3P2E,temp_Qy,temp_Qb,temp_Qb_3P2E,temp_PT_3P2E,temp_QT_3P2E,temp_Pr_3P2E,temp_Pb_3P2E,temp_Qr_3P2E,temp_Qb_3P2E;
long double ADC_VR,ADC_VY,ADC_VB,ADC_VRY,ADC_VYB,ADC_VBR,ADC_IR,ADC_IY,ADC_IB;
long double VR_AVG,VY_AVG,VB_AVG,VRY_AVG,VYB_AVG,VBR_AVG,IR_AVG,IY_AVG,IB_AVG,Pr_AVG,Pr_AVG_3P2E,Py_AVG,Pb_AVG,Pb_AVG_3P2E,Qr_AVG,Qr_AVG_3P2E,Qy_AVG,Qb_AVG,Qb_AVG_3P2E,PT_3P2E_AVG,PT_3P2E,QT_3P2E_AVG,QT_3P2E;
long double VR_T,VY_T,VB_T,VRY_T,VYB_T,VBR_T,IR_T,IY_T,IB_T,Pr_T,Py_T,Pb_T,Qr_T,Qy_T,Qb_T,PT_3P2E_T,QT_3P2E_T,Pr_T_3P2E,Pb_T_3P2E,Qr_T_3P2E,Qb_T_3P2E;

unsigned char key_pressed=0;
extern unsigned char Key_Read1(void);

 unsigned char init=0,p, channel_change=0;
 unsigned char Str=1;
 float AVG_VR[8],AVG_VY[8],AVG_VB[8],AVG_VRY[8],AVG_VYB[8],AVG_VBR[8],AVG_IR[8],AVG_IY[8],AVG_IB[8];
 float AVG_Pr[8],AVG_Py[8],AVG_Pb[8],AVG_PT[8],AVG_Qr[8],AVG_Qy[8],AVG_Qb[8],AVG_QT[8];
 float  fAvG_VR=0.0,fAvG_VY=0.0,fAvG_VB=0.0,fAvG_VRY=0.0,fAvG_VYB=0.0,fAvG_VBR=0.0,fAvG_IR=0.0,fAvG_IY=0.0,fAvG_IB=0.0,fAvG_Pr=0.0,fAvG_Py=0.0,fAvG_Pb=0.0,fAvG_PT=0.0,fAvG_Qr=0.0,fAvG_Qy=0.0,fAvG_Qb=0.0,fAvG_QT=0.0;

// Shree made changes
extern CONFIGURATION_PARAM reqConfiguration;

extern unsigned char adc_count;
extern unsigned char Run_flag;
extern uint16_t Buffer[];

//#define PHCAL 7
//#define PHYCAL 7
//#define PHBCAL 7

//#define V_90_samp 1080
//#define I_90_samp 1024

unsigned char PHCAL =0;

//void Vrms_CAL(unsigned short m,unsigned char phase, signed long int *v, signed long int *v90, unsigned long int *sqR)
//{
//    *v   = (signed long int)((Buffer[((8*m)+ phase)+(PHCAL*8)])   - Buffer[((8*m)+3)+(PHCAL*8)]);
//    *v90 = (signed long int)((Buffer[((8*m)+ phase)+(V_90_samp)]) - Buffer[((8*m)+3)+(V_90_samp)]);
//    *sqR = (unsigned long int)((signed long int)(*v)*(signed long int)(*v));
//}
//
//void Irms_CAL(unsigned short m,unsigned char phase, signed long int *i, signed long int *i90, unsigned long int *sqR)
//{
//    *i   = (signed long int)  ((Buffer[((8*m)+phase)])           -  Buffer[((8*m)+3)]);
//    *i90 = (signed long int)  ((Buffer[((8*m)+phase)+I_90_samp]) -  Buffer[((8*m)+3)+I_90_samp]);
//    *sqR = (unsigned long int)((signed long int)  (*i)           *  (signed long int)(*i));
//
//}
/***************************************************************************************************************************
Function     :  Para_Store
Parameter    :  null
Description  :  This function is used to calculate VRMS values of 3 phase voltage and current and also for calculation of
                watt , var. also key sensing is also executed.
***************************************************************************************************************************/
void Para_Store(void)
{
  unsigned int m,rp,pr;
  signed int subR;
  long double Pr,Py,Pb,Qr,Qy,Qb;             //Pr_3P2E,Pb_3P2E,Qr_3P2E,Qb_3P2E;
  long double  Vry_samp=0.0,Vyb_samp=0.0,Vbr_samp=0.0,sumR,sumY,sumB,sumRY,sumYB,sumBR,sumIR,sumIY,sumIB;
  signed long int Vr_samp,Vy_samp,Vb_samp,Ir_samp,Iy_samp,Ib_samp;
  signed long int Ir90_samp,Iy90_samp,Ib90_samp,Vr90_samp=0.0,Vy90_samp=0.0,Vb90_samp=0.0;
  unsigned long int sqR;
  unsigned short V_90_samp;
  int lclIndex1, lclIndex2;
  float temp123;
  sumR=0.0;sumY=0.0;sumB=0.0;sumRY=0.0;sumYB=0.0;sumBR=0.0;sumIR=0.0;sumIY=0.0;sumIB=0.0;
  Pr=0.0;Py=0.0;Pb=0.0;PT_3P2E=0.0; Qr=0.0;Qy=0.0;Qb=0.0,QT_3P2E=0.0;

   if(reqMeasurepara.flFreq > 46.5 && reqMeasurepara.flFreq <55.5)
   {
     PHCAL=7;V_90_samp=1080;
   }
  else if(reqMeasurepara.flFreq < 46.6)
  {
    PHCAL=6;V_90_samp=1072;
  }
    rp=1;
    for(pr=0;pr<5200;pr++)
    {
      if(pr!=0)
      {
        if((pr%8)==0)
        {
           rp++;
        }
      }

      temp123=(float)(3103.0/((float)(Buffer[(7*rp)+(rp-1)])));
      Buffer[pr]=(unsigned int)((Buffer[pr])*temp123);
    }

    subR=0;sqR=0;
    for(m=1;m<513;m++)
    {
        /***************************** R phase **********************************/
        lclIndex1 = ((8*m))+(PHCAL*8);
        lclIndex2 = ((8*m)+3)+(PHCAL*8);
        if((lclIndex1 >= 0) && (lclIndex2 >= 0) &&  (lclIndex1 < BUFF_SIZE) && ((lclIndex1 < BUFF_SIZE))){
          subR=(signed long int)((Buffer[lclIndex1])-Buffer[lclIndex2]);
          Vr_samp=((signed long int)(subR));
        }
        
        lclIndex1 = ((8*m))+(V_90_samp);
        lclIndex2 = ((8*m)+3)+(V_90_samp);
        if((lclIndex1 >= 0) && (lclIndex2 >= 0) &&  (lclIndex1 < BUFF_SIZE) && ((lclIndex1 < BUFF_SIZE))){
          subR=(signed long int)((Buffer[lclIndex1])-Buffer[lclIndex2]);
          Vr90_samp=(subR);
        }
        
        sqR=(unsigned long int)((signed long int)(Vr_samp)*(signed long int)(Vr_samp));
        sumR=(long double)(sumR+sqR);

        /****************************** Y phase **********************************/
         subR=0;sqR=0;
         subR=(signed long int)((signed int)(Buffer[((8*m)+1)+(PHCAL*8)])-Buffer[((8*m)+3)+(PHCAL)*8]);
         Vy_samp=((signed long int)(subR));
        lclIndex1 = ((8*m)+1)+(V_90_samp);
        lclIndex2 = ((8*m)+3)+V_90_samp;
        if((lclIndex1 >= 0) && (lclIndex2 >= 0) &&  (lclIndex1 < BUFF_SIZE) && ((lclIndex1 < BUFF_SIZE))){
         subR=(signed long int)((Buffer[lclIndex1])-Buffer[lclIndex2]);
         Vy90_samp=(subR);
        }
         sqR=(unsigned long int)((signed long int)(Vy_samp)*(signed long int)(Vy_samp));
         sumY=(long double)(sumY+sqR);


         /****************************** B phase **********************************/
         subR=0;sqR=0;
         subR=(signed long int)((signed int)(Buffer[((8*m)+2)+(PHCAL*8)])-Buffer[((8*m)+3)+(PHCAL*8)]);
         Vb_samp=((signed long int)(subR));
         lclIndex1 = ((8*m)+2)+(V_90_samp);
         lclIndex2 = ((8*m)+3)+(V_90_samp); 
         if((lclIndex1 >= 0) && (lclIndex2 >= 0) &&  (lclIndex1 < BUFF_SIZE) && ((lclIndex1 < BUFF_SIZE))){
          subR=(signed long int)((Buffer[lclIndex1])-Buffer[lclIndex2]);
          Vb90_samp=subR;
         }
         sqR=(unsigned long int)((signed long int)(Vb_samp)*(signed long int)(Vb_samp));
         sumB=(long double)(sumB+sqR);


         /****************************** RY phase **********************************/
         sqR=0;
         Vry_samp=(long double)(Vr_samp-Vy_samp);
         sqR=(unsigned long int)((Vry_samp)*(Vry_samp));
         sumRY=(long double)(sumRY+sqR);

         /**************YB phase**********************************/

         sqR=0;
         Vyb_samp=(long double)(Vb_samp-Vy_samp);
         sqR=(unsigned long int)((Vyb_samp)*(Vyb_samp));
         sumYB=(long double)(sumYB+sqR);

        /**************BR phase**********************************/

        sqR=0;
        Vbr_samp=(long double)(Vb_samp-Vr_samp);
        sqR=(unsigned long int)((Vbr_samp)*(Vbr_samp));
        sumBR=(long double)(sumBR+sqR);

       /**************IR **********************************/
       subR=0;sqR=0;
       subR=(signed long int)((Buffer[((8*m)+4)])-Buffer[((8*m)+3)]);
       Ir_samp=((signed long int)(subR));

       subR=(signed long int)((Buffer[((8*m)+4)+1024])-Buffer[((8*m)+3)+1024]);//1000
       Ir90_samp=subR;

       sqR=(unsigned long int)((signed long int)(Ir_samp)*(signed long int)(Ir_samp));
       sumIR=(long double)(sumIR+sqR);

       /**************IY **********************************/
       subR=0;sqR=0;
       subR=(signed long int)((Buffer[((8*m)+5)])-Buffer[((8*m)+3)]);
       Iy_samp=((signed long int)(subR));

       subR=(signed long int)((Buffer[((8*m)+5)+1024])-Buffer[((8*m)+3)+1024]);//1000
       Iy90_samp=subR;

       sqR=(unsigned long int)((signed long int)(Iy_samp)*(signed long int)(Iy_samp));
       sumIY=(long double)(sumIY+sqR);
       /**************IB **********************************/

       subR=0;sqR=0;
       subR=(signed long int)((Buffer[((8*m)+6)])-Buffer[((8*m)+3)]);
       Ib_samp=((signed long int)(subR));

       subR=(signed long int)((Buffer[((8*m)+6)+1024])-Buffer[((8*m)+3)+1024]);//1000
       Ib90_samp=subR;

       sqR=(unsigned long int)((signed long int)(Ib_samp)*(signed long int)(Ib_samp));
       sumIB=(long double)(sumIB+sqR);

     if(reqConfiguration.uiSysConfiguration==0)
     {
     /********************** Active Power calculation**********************************/
        Pr=(long double)((long double)(Vr_samp*Ir_samp)+Pr);
        Py=(long double)((long double)(Vy_samp*Iy_samp)+Py);
        Pb=(long double)((long double)(Vb_samp*Ib_samp)+Pb);
    /********************** Reactive Power calculation**********************************/

        Qr=(long double)((long double)(Vr90_samp*Ir_samp)+Qr);
        Qy=(long double)((long double)(Vy90_samp*Iy_samp)+Qy);
        Qb=(long double)((long double)(Vb90_samp*Ib_samp)+Qb);
     /********************** Reactive Power 3P2E calculation**********************************/
     }
     else if (reqConfiguration.uiSysConfiguration==1)
     {
     /********************** Active Power calculation**********************************/
       Pr=(long double)((long double)(Vry_samp*Ir_samp)+Pr);
       Pb=(long double)((long double)(Vyb_samp*Ib_samp)+Pb);

     /********************** Reactive Power calculation**********************************/
       Qr=(long double)((long double)(Vry_samp*Ir90_samp)+Qr);
       Qb=(long double)((long double)(Vyb_samp*Ib90_samp)+Qb);

     /********************** Reactive Power 3P21E 1P1E calculation**********************************/
   }

   else if ((reqConfiguration.uiSysConfiguration==2) || (reqConfiguration.uiSysConfiguration==3))
   {
        Pr=(long double)((long double)(Vr_samp   * Ir_samp  )+ Pr);
        Qr=(long double)((long double)(Vr90_samp * Ir_samp  )+ Qr);
   }
      if((m%20)==0) //shree maede changes here 20 to 10
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
      }

    // Shree made changes
   if(reqConfiguration.uiSysConfiguration==0) //3P3E
   {
      temp_Pr=(Pr/512.0);
      temp_Py=(Py/512.0);
      temp_Pb=(Pb/512.0);

      Pr_AVG    = (Pr_AVG+temp_Pr);
      Py_AVG    = (Py_AVG+temp_Py);
      Pb_AVG    = (Pb_AVG+temp_Pb);

      temp_Qr=((-1)*Qr/512.0);
      temp_Qy=((-1)*Qy/512.0);
      temp_Qb=((-1)*Qb/512.0);

      Qr_AVG=(Qr_AVG+temp_Qr);
      Qy_AVG=(Qy_AVG+temp_Qy);
      Qb_AVG=(Qb_AVG+temp_Qb);

      temp_PT_3P2E=((PT_3P2E/512.0));                                         // *(0.866025)); // sin60 = 0.866025
      PT_3P2E_AVG=(temp_PT_3P2E+PT_3P2E_AVG);

      temp_QT_3P2E=((QT_3P2E/512.0)*(0.866025));                              // sin60 = 0.866025
      QT_3P2E_AVG=(temp_QT_3P2E+QT_3P2E_AVG);

   }
   else if(reqConfiguration.uiSysConfiguration==1)                            // 3P2E
   {

      temp_Pr=(Pr/512.0);
      temp_Pb=(Pb/512.0);

      Pr_AVG    = (Pr_AVG+temp_Pr);
      Pb_AVG    = (Pb_AVG+temp_Pb);

      temp_Qr=(Qr/512.0);
      temp_Qb=(Qb/512.0);

      Qr_AVG=(Qr_AVG+temp_Qr);
      Qb_AVG=(Qb_AVG+temp_Qb);

      temp_PT_3P2E=((PT_3P2E/512.0)*(0.866025)); // sin60 = 0.866025
      PT_3P2E_AVG=(temp_PT_3P2E+PT_3P2E_AVG);

      temp_QT_3P2E=((QT_3P2E/512.0)*(0.866025)); // sin60 = 0.866025
      QT_3P2E_AVG=(temp_QT_3P2E+QT_3P2E_AVG);

   }
   else if((reqConfiguration.uiSysConfiguration==2)||(reqConfiguration.uiSysConfiguration==3)) //3P1E & 1P1E
   {
      temp_Pr = (Pr/512.0);
      Pr_AVG  = (Pr_AVG+temp_Pr);
      temp_Qr = ((-1)*Qr/512.0);
      Qr_AVG  = (Qr_AVG + temp_Qr);
   }

      temp_R=(long double)((sumR)/512.0);                            //(long double)  (unsigned long)
      temp_Y=(long double)((sumY)/512.0);
      temp_B=(long double)((sumB)/512.0);
      temp_RY=(long double)((sumRY)/512.0);
      temp_YB=(long double)((sumYB)/512.0);
      temp_BR=(long double)((sumBR)/512.0);
      temp_IR=(long double)((sumIR)/512.0);
      temp_IY=(long double)((sumIY)/512.0);
      temp_IB=(long double)((sumIB)/512.0);
      ////GPIO_ResetBits(GPIOC, GPIO_Pin_9);

      ADC_VR=(long double)(sqrt(temp_R));
      ADC_VY=(long double)(sqrt(temp_Y));
      ADC_VB=(long double)(sqrt(temp_B));
      ADC_VRY=(long double)(sqrt(temp_RY));
      ADC_VYB=(long double)(sqrt(temp_YB));
      ADC_VBR=(long double)(sqrt(temp_BR));
      ADC_IR=(long double)(sqrt(temp_IR));
      ADC_IY=(long double)(sqrt(temp_IY));
      ADC_IB=(long double)(sqrt(temp_IB));

      VR_AVG=ADC_VR+VR_AVG;
      VY_AVG=VY_AVG+ADC_VY;
      VB_AVG=VB_AVG+ADC_VB;
      VRY_AVG=VRY_AVG+ADC_VRY;
      VYB_AVG=VYB_AVG+ADC_VYB;
      VBR_AVG=VBR_AVG+ADC_VBR;
      IR_AVG=IR_AVG+ADC_IR;
      IY_AVG=IY_AVG+ADC_IY;
      IB_AVG=IB_AVG+ADC_IB;


    if(adc_count==11)
    {
      VR_T = VR_AVG/12.0;
      VY_T = VY_AVG/12.0;
      VB_T = VB_AVG/12.0;

      VRY_T = VRY_AVG/12.0;
      VYB_T = VYB_AVG/12.0;
      VBR_T = VBR_AVG/12.0;

      IR_T = IR_AVG/12.0;
      IY_T = IY_AVG/12.0;
      IB_T = IB_AVG/12.0;

     if(reqConfiguration.uiSysConfiguration==0) //3P3E
     {
       Pr_T=Pr_AVG/12.0;
       Py_T=Py_AVG/12.0;
       Pb_T=Pb_AVG/12.0;

       Qr_T=Qr_AVG/12.0;
       Qy_T=Qy_AVG/12.0;
       Qb_T=Qb_AVG/12.0;

       PT_3P2E_T=(PT_3P2E_AVG/12.0);
       QT_3P2E_T=(QT_3P2E_AVG/12.0);
     }
     else if(reqConfiguration.uiSysConfiguration==1) //3P2E
     {
       Pr_T=Pr_AVG/12.0;
       Pb_T=Pb_AVG/12.0;

       Qr_T=Qr_AVG/12.0;
       Qb_T=Qb_AVG/12.0;

       PT_3P2E_T=(PT_3P2E_AVG/12.0);
       QT_3P2E_T=(QT_3P2E_AVG/12.0);
     }
     else if((reqConfiguration.uiSysConfiguration==2)||(reqConfiguration.uiSysConfiguration==3)) //3P1E & 1P1E
     {
       Pr_T=Pr_AVG/12.0;
       Qr_T=Qr_AVG/12.0;
     }
     slide();
     VR_AVG=0.0;VY_AVG=0.0;VB_AVG=0.0;VRY_AVG=0.0;VYB_AVG=0.0;VBR_AVG=0.0;IR_AVG=0.0;IY_AVG=0.0;IB_AVG=0.0;
     Pr_AVG=0.0;Py_AVG=0.0;Pb_AVG=0.0;PT_3P2E_AVG=0.0;Qr_AVG=0.0;Qy_AVG=0.0;Qb_AVG=0.0,QT_3P2E_AVG=0.0;
  }

 }


/***************************************************************************************************************************
Function     :  slide
Parameter    :  null
Description  :  This function is used to calculate VRMS values of 3 phase voltage and current and also for calculation of
                watt , var. also key sensing is also executed.
***************************************************************************************************************************/
void slide(void)
{


   err(VR_cal,VR_T,AVG_VR);    err(VY_cal,VY_T,AVG_VY);     err(VB_cal,VB_T,AVG_VB);

   err(VRY_cal,VRY_T,AVG_VRY); err(VYB_cal,VYB_T,AVG_VYB);  err(VBR_cal,VBR_T,AVG_VBR);

   err(IR_cal,IR_T,AVG_IR);    err(IY_cal,IY_T,AVG_IY);     err(IB_cal,IB_T,AVG_IB);

   err(Pr_cal,Pr_T,AVG_Pr);    err(Py_cal,Py_T,AVG_Py);     err(Pb_cal,Pb_T,AVG_Pb);   err(PT_3P2E_cal,PT_3P2E_T,AVG_PT);
   err(Qr_cal,Qr_T,AVG_Qr);    err(Qy_cal,Qy_T,AVG_Qy);     err(Qb_cal,Qb_T,AVG_Qb);   err(QT_3P2E_cal,QT_3P2E_T,AVG_QT);

   if(init==0)
    {
     init=1;
     channel_change=0;

      for(p=1;p<7;p++)
     {
      AVG_VR[p]=VR_T;
      AVG_VY[p]=VY_T;
      AVG_VB[p]=VB_T;

      AVG_VRY[p]=VRY_T;
      AVG_VYB[p]=VYB_T;
      AVG_VBR[p]=VBR_T;

      AVG_IR[p]=IR_T;
      AVG_IY[p]=IY_T;
      AVG_IB[p]=IB_T;


      AVG_Pr[p]=Pr_T;
      AVG_Py[p]=Py_T;
      AVG_Pb[p]=Pb_T;

      // AVG_Pr[p]=Pr_T;
      // AVG_Pb[p]=Pb_T;
     // AVG_PT[p]=PT_3P2E_T;

      AVG_Qr[p]=Qr_T;
      AVG_Qy[p]=Qy_T;
      AVG_Qb[p]=Qb_T;

      // AVG_Qr[p]=Qr_T;
      // AVG_Qb[p]=Qb_T;
     // AVG_QT[p]=QT_3P2E_T;

     }

    }
    else
    {

      AVG_VR[Str]=VR_T;
      AVG_VY[Str]=VY_T;
      AVG_VB[Str]=VB_T;

      AVG_VRY[Str]=VRY_T;
      AVG_VYB[Str]=VYB_T;
      AVG_VBR[Str]=VBR_T;

      AVG_IR[Str]=IR_T;
      AVG_IY[Str]=IY_T;
      AVG_IB[Str]=IB_T;

    //  if(reqConfiguration.uiSysConfiguration==1)   //3P2E
     // {
      // AVG_Pr[Str]=Pr_T_3P2E;
      //   AVG_Pb[Str]=Pb_T_3P2E;
     // }
    //  else
    //  {
        AVG_Pr[Str]=Pr_T;
        AVG_Py[Str]=Py_T;
        AVG_Pb[Str]=Pb_T;
     // }

   //   AVG_PT[Str]=PT_3P2E_T;

//      if(reqConfiguration.uiSysConfiguration==1)   //3P2E
//      {
//      AVG_Qr[Str]=Qr_T_3P2E;
//      AVG_Qb[Str]=Qb_T_3P2E;
//      }
//      else
//      {
      AVG_Qr[Str]=Qr_T;
      AVG_Qy[Str]=Qy_T;
      AVG_Qb[Str]=Qb_T;
//      }
//      AVG_QT[Str]=QT_3P2E_T;
     Str++;
     if(Str==6)
     {
       Str=1;
     }

    }
    fAvG_VR=0.0;    fAvG_VY=0.0;     fAvG_VB=0.0;   fAvG_VRY=0.0;   fAvG_VYB=0.0;   fAvG_VBR=0.0;  fAvG_IR=0.0;   fAvG_IY=0.0;   fAvG_IB=0.0;  fAvG_Pr=0.0;
    fAvG_Py=0.0;    fAvG_Pb=0.0;     fAvG_PT=0.0;   fAvG_Qr=0.0;    fAvG_Qy=0.0;    fAvG_Qb=0.0;   fAvG_QT=0.0;
   for(p=1;p<6;p++)
   {
     fAvG_VR= AVG_VR[p]+fAvG_VR;
     fAvG_VY= AVG_VY[p]+fAvG_VY;
     fAvG_VB= AVG_VB[p]+fAvG_VB;

     fAvG_VRY= AVG_VRY[p]+fAvG_VRY;
     fAvG_VYB= AVG_VYB[p]+fAvG_VYB;
     fAvG_VBR= AVG_VBR[p]+fAvG_VBR;

     fAvG_IR= AVG_IR[p]+fAvG_IR;
     fAvG_IY= AVG_IY[p]+fAvG_IY;
     fAvG_IB= AVG_IB[p]+fAvG_IB;

     fAvG_Pr= AVG_Pr[p]+fAvG_Pr;
     fAvG_Py= AVG_Py[p]+fAvG_Py;
     fAvG_Pb= AVG_Pb[p]+fAvG_Pb;
     fAvG_PT= AVG_PT[p]+fAvG_PT;

     fAvG_Qr= AVG_Qr[p]+fAvG_Qr;
     fAvG_Qy= AVG_Qy[p]+fAvG_Qy;
     fAvG_Qb= AVG_Qb[p]+fAvG_Qb;
     fAvG_QT= AVG_QT[p]+fAvG_QT;
   }

   fAvG_VR  = fAvG_VR/5.0;   fAvG_VY  =fAvG_VY/5.0;    fAvG_VB  = fAvG_VB/5.0;
   fAvG_VRY = fAvG_VRY/5.0;  fAvG_VYB =fAvG_VYB/5.0;   fAvG_VBR = fAvG_VBR/5.0;

   fAvG_IR =fAvG_IR/5.0;  fAvG_IY =fAvG_IY/5.0;  fAvG_IB =fAvG_IB/5.0;

   fAvG_Pr =fAvG_Pr/5.0;  fAvG_Py =fAvG_Py/5.0;  fAvG_Pb =fAvG_Pb/5.0;  fAvG_PT =fAvG_PT/5.0;
   fAvG_Qr =fAvG_Qr/5.0;  fAvG_Qy =fAvG_Qy/5.0;  fAvG_Qb =fAvG_Qb/5.0;  fAvG_QT =fAvG_QT/5.0;


   fAvG_VR =((AVG_VR[6] *5.0) + fAvG_VR )/6.0;   fAvG_VY =((AVG_VY[6] *5.0) + fAvG_VY )/6.0;   fAvG_VB =((AVG_VB[6] * 5.0) + fAvG_VB )/6.0;
   fAvG_VRY=((AVG_VRY[6]*5.0) + fAvG_VRY)/6.0;   fAvG_VYB=((AVG_VYB[6]*5.0) + fAvG_VYB)/6.0;   fAvG_VBR=((AVG_VBR[6]* 5.0) + fAvG_VBR)/6.0;
   fAvG_IR =((AVG_IR[6] *5.0) + fAvG_IR )/6.0;   fAvG_IY =((AVG_IY[6] *5.0) + fAvG_IY )/6.0;   fAvG_IB =((AVG_IB[6] * 5.0) + fAvG_IB )/6.0;
   fAvG_Pr =((AVG_Pr[6] *5.0) + fAvG_Pr )/6.0;   fAvG_Py =((AVG_Py[6] *5.0) + fAvG_Py )/6.0;   fAvG_Pb =((AVG_Pb[6] * 5.0) + fAvG_Pb )/6.0;
   fAvG_PT=((AVG_PT[6] * 5.0) + fAvG_PT )/6.0;
   fAvG_Qr =((AVG_Qr[6] *5.0) + fAvG_Qr )/6.0;   fAvG_Qy =((AVG_Qy[6] *5.0) + fAvG_Qy )/6.0;   fAvG_Qb =((AVG_Qb[6] * 5.0) + fAvG_Qb )/6.0;
   fAvG_QT=((AVG_QT[6] * 5.0) + fAvG_QT) /6.0;

   AVG_VR[6] =fAvG_VR;    AVG_VY[6]=fAvG_VY;      AVG_VB[6]=fAvG_VB;
   AVG_VRY[6]=fAvG_VRY;   AVG_VYB[6]=fAvG_VYB;    AVG_VBR[6]=fAvG_VBR;
   AVG_IR[6] =fAvG_IR;    AVG_IY[6]=fAvG_IY;      AVG_IB[6]=fAvG_IB;
   AVG_Pr[6] =fAvG_Pr;    AVG_Py[6]=fAvG_Py;      AVG_Pb[6]=fAvG_Pb;  AVG_PT[6]=fAvG_PT;
   AVG_Qr[6] =fAvG_Qr;    AVG_Qy[6]=fAvG_Qy;      AVG_Qb[6]=fAvG_Qb;  AVG_QT[6]=fAvG_QT;

  VR_cal=fAvG_VR;         VY_cal=fAvG_VY;         VB_cal=fAvG_VB;
  VRY_cal=fAvG_VRY;       VYB_cal=fAvG_VYB;       VBR_cal=fAvG_VBR;

  IR_cal=fAvG_IR;         IY_cal=fAvG_IY;         IB_cal=fAvG_IB;
  Pr_cal=fAvG_Pr;         Py_cal=fAvG_Py;         Pb_cal=fAvG_Pb;
  Qr_cal=fAvG_Qr;         Qy_cal=fAvG_Qy;         Qb_cal=fAvG_Qb;

// PT_3P2E_cal=fAvG_PT;
 //QT_3P2E_cal=fAvG_QT;

}
/***************************************************************************************************************************
Function     :  err
Parameter    :  pre, *ary
Description  :  This function is used to calculate VRMS values of 3 phase voltage and current and also for calculation of
                watt , var. also key sensing is also executed.
***************************************************************************************************************************/
void err(float pre,float curr,float *ary)
{
 float Hi_Val,Lo_Val;
 unsigned char s;
  ary++;
  Hi_Val=(pre*0.01)+pre;
  Lo_Val=(pre-(pre*0.01));
  if((curr<Lo_Val)||(curr>Hi_Val))
  {
    for(s=0;s<6;s++)
    {
        *ary=(float)(curr);
         ary++;
    }
  }
}
/***************************************************************************************************************************
Function     :  FindSqrt
Parameter    :  SqrtInputVal
Description  :  This function is used to calculate VRMS values of 3 phase voltage and current and also for calculation of
                watt , var. also key sensing is also executed.
***************************************************************************************************************************/

unsigned int FindSqrt(unsigned long SqrtInputVal)
{
  unsigned char BitCntr = 14;
  unsigned long zval=0,yval=0,tempval;

    yval=1<<BitCntr;
    while(yval)
    {
        zval=zval^yval;
        tempval=zval*zval;
        if(tempval>SqrtInputVal)
        {
            zval=zval^yval;
        }
        yval>>=1;
    }
  return((unsigned int)zval);
}

/*************************** END OF FILE ***************************************/
