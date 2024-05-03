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

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
extern unsigned int FinalDisp[];
/* Private define ------------------------------------------------------------*/
#define DATA_EEPROM_START_ADDR     0x08080000
#define DATA_EEPROM_END_ADDR       0x08080007 //0x080803FF
#define DATA_EEPROM_PAGE_SIZE      0x8
#define DATA_32                    12345678
#define FAST_DATA_32               0x55667799

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus DataMemoryProgramStatus = PASSED;
uint32_t NbrOfPage = 0, Address = 0;


/**********************************************************************************************/
        /* Union Name : split (For Type Casting)
           This union is used for for Typecasting purpose
           Float to Character & Character to Float */
/**********************************************************************************************/

union splitNcombineBytes
{
    unsigned char ucTemp[4];
    float flTemp;
    unsigned long ulTemp;
    unsigned int uiTemp;
    int iTemp;
    long lTemp;
}split;

//-----------------------------------------------------------------------------------------



/**********************************************************************************************/
 /*  Function: FnWriteEpromWord
  This Function Made for writing Word (4-byte) into specific address of EEPROM.
  By passing it 32-bi address & value to be written (4-byte)*/
/**********************************************************************************************/


void FnWriteEpromWord(uint32_t Address_temp2,float fValue1)
{
 unsigned char iWrite;
 uint32_t Address_curr;
  Address_curr=Address_temp2;
  split.flTemp=0.00000;
  split.flTemp=fValue1;

  /* Unlock the FLASH PECR register and Data EEPROM memory */
  DATA_EEPROM_Unlock();

  /* Erase 4 byte before writing */
  //FnEraseEpromWord(Address_curr,4);

  for (iWrite=0; iWrite<4; iWrite++)
   {
   
     FLASHStatus = DATA_EEPROM_ProgramByte(Address_curr, split.ucTemp[iWrite]);//DATA_EEPROM_ProgramWord(Address, DATA_32);

      if(FLASHStatus == FLASH_COMPLETE)
      {
        Address_curr = Address_curr + 1;
      }
      else
      {
        FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                             | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
      }
   }

  Address_curr = Address_temp2;    //DATA_EEPROM_START_ADDR;

  /* Check the correctness of written data */
  for (iWrite=0; iWrite<4; iWrite++)
   {
      if(*(__IO uint8_t*)Address_curr != split.ucTemp[iWrite])
      {
        DataMemoryProgramStatus = FAILED;
      }
      Address_curr = Address_curr + 1;

   }

   split.flTemp=0.00000;
}





/**********************************************************************************************/
 /*  Function: FnWriteEpromByte
  This Function Made for writing Character (1-byte) into specific address of EEPROM.
  By passing it 32-bi address & Character value to be written (1-byte)*/
/**********************************************************************************************/


void FnWriteEpromByte(uint32_t Address_temp2,unsigned char charValue1)
{
    uint32_t Address_curr;
    Address_curr=Address_temp2;
    split.ucTemp[0]=0;
    split.ucTemp[0]=charValue1;

    /* Unlock the FLASH PECR register and Data EEPROM memory */
    DATA_EEPROM_Unlock();

    /* Erase 4 byte before writing */
   // FnEraseEpromWord(Address_curr,4);

    FLASHStatus = DATA_EEPROM_ProgramByte(Address_curr, split.ucTemp[0]);//DATA_EEPROM_ProgramWord(Address, DATA_32);
    if(FLASHStatus == FLASH_COMPLETE)
    {
        Address_curr = Address_curr + 1;
    }
    else
    {
        FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                       | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
    }

    Address_curr = Address_temp2;    //DATA_EEPROM_START_ADDR;

    /* Check the correctness of written data */

    if(*(__IO uint8_t*)Address_curr != split.ucTemp[0])
    {
        DataMemoryProgramStatus = FAILED;
    }

    split.ucTemp[0]=0;

}




/**********************************************************************************************/
 /*  Function: FnEraseEpromWord
  This Function Made for Erasing (writing 0x00) Word (4-byte) location specific no. of
  address of EEPROM.By passing it 32-bi address & no. of Word to be erased (4-byte each)*/
/**********************************************************************************************/


void FnEraseEpromWord(uint32_t Address_temp1,unsigned char NoOfWords)
{
    unsigned char icount;
    /* Unlock the FLASH PECR register and Data EEPROM memory */
    //SendChar('S');
    DATA_EEPROM_Unlock();
    //SendChar('T');
    /* Clear all pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
    | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);	
    //SendChar('U');
    NbrOfPage = NoOfWords;

    /* Erase the Data EEPROM Memory pages by Word (32-bit) */
    for(icount = 0; icount < NbrOfPage; icount++)
    {
      //SendChar('V');
      /* Erase Word (4 byte location)  */
      FLASHStatus = DATA_EEPROM_EraseWord(Address_temp1+(4*icount));
    }

    //SendChar('W');
    for(icount=0; icount < (NoOfWords); icount++) //(icount=0; icount < (NoOfWords*4); icount++)
    {
      //SendChar('W');
      if(*(__IO uint32_t*)Address_temp1 != 0x0)
      {
        //SendChar('Y');
        DataMemoryProgramStatus = FAILED;
      }
      Address_temp1 = Address_temp1 + 1;
    }

}




/**********************************************************************************************/
 /*  Function: FnReadEprom (Returns Float Value)
  This Function Made for Reading float (Word 4-byte) from specific address of EEPROM.
  By passing it 32-bi address to be read & it returns  float Value(4-byte)*/
/**********************************************************************************************/


float FnReadEprom(uint32_t Address_temp)
{
      unsigned char iRead;

      /* Unlock the FLASH PECR register and Data EEPROM memory */
      DATA_EEPROM_Unlock();

      for (iRead=0; iRead < 4; iRead++)
      {
          split.ucTemp[iRead] = *(__IO uint8_t*)Address_temp;
          Address_temp = Address_temp + 1;
      }

      return(split.flTemp);

}





/**********************************************************************************************/
 /*  Function: FnReadEprom (Returns Character Value)
  This Function Made for Reading Character(1-byte) from specific address of EEPROM.
  By passing it 32-bi address to be read & it returns  Character Value(1-byte)*/
/**********************************************************************************************/


unsigned char FnReadEpromByte(uint32_t Address_temp)
{

  /* Unlock the FLASH PECR register and Data EEPROM memory */
  DATA_EEPROM_Unlock();
  split.ucTemp[0] = *(__IO uint8_t*)Address_temp;
  return(split.ucTemp[0]);

}






//**************************************************************************************************
/*------------------------------------------------------------------------------------------------*/