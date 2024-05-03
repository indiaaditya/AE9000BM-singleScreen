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




extern unsigned char ucDataLogBuffer[200];
extern MEASURED_PARAM  reqMeasurepara;
union splitNcombine4Bytes
{
    unsigned char ucTemp[4];
    float flTemp;
    unsigned long ulTemp;
    unsigned int uiTemp;
    int iTemp;
    long lTemp;
};

void  fnStoredDataLoggParam(void)
{
    unsigned char ucIndex;
    union splitNcombine4Bytes split;

    ucIndex=1;

    split.flTemp =reqMeasurepara.flVry;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp =reqMeasurepara.flVyb;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp = reqMeasurepara.flVbr;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp = reqMeasurepara.flVllavg;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];



    split.flTemp = reqMeasurepara.flVrn;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


      split.flTemp = reqMeasurepara.flVbn;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp = reqMeasurepara.flVlnavg;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];



    split.flTemp = reqMeasurepara.flIr;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp = reqMeasurepara.flIy;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp = reqMeasurepara.flIb;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];



    split.flTemp = reqMeasurepara.flIavg;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp = reqMeasurepara.flFreq;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


       split.flTemp=reqMeasurepara.flKWr;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp=reqMeasurepara.flKWy;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp=reqMeasurepara.flKWb;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp=reqMeasurepara.flKWTot;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp=reqMeasurepara.flVARr;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];



    split.flTemp=reqMeasurepara.flVARy;

    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];



    split.flTemp=reqMeasurepara.flVARb;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];



    split.flTemp=reqMeasurepara.flVARTot;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];



    split.flTemp=reqMeasurepara.flKVAr;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp=reqMeasurepara.flKVAy;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp=reqMeasurepara.flKVAb;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];



    split.flTemp=reqMeasurepara.flKVATot;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];


    split.flTemp=reqMeasurepara.flPFavg;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];
   /* 4 *25  = 100 */
   
    /* Changes to add per phase PF */
    split.flTemp=reqMeasurepara.flPFr;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];
    
    split.flTemp=reqMeasurepara.flPFy;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];
    
    split.flTemp=reqMeasurepara.flPFb;
    ucDataLogBuffer[ucIndex++]=split.ucTemp[1];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[0];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[3];
    ucDataLogBuffer[ucIndex++]=split.ucTemp[2];
    
}











