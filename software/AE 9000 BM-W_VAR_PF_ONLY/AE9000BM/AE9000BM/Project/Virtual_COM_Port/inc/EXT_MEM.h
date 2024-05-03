#ifndef _EXT_MEM_H
#define _EXT_MEM_H
#include "port.h"
//#include "stm32f10x_usart.h"
void SPI1_Init(void); 
void SPI2_Init(void); 
void fnAssign_data();
unsigned int fnGetPageNum(long adr);
void fnWrite_enable();
void fnWriteDataToMemory(long lAddr,unsigned char Write_Data );
void fnReadFromNVMemory(unsigned long lAddr,unsigned char ucGroup)  ;
//void fnWriteDataLoggToMemory(unsigned char ucNumWriteBytes);
void fnWriteDataLoggToMemory(unsigned char ucNumWriteBytes , long lADD , unsigned char ucType);
void fnStoredDataParamToMemory();
unsigned char fnReadLogDataFromNVMemory(long lAddr,unsigned char ucLength);

void fnFormInterruptionsMsg();
void fnWriteInterruption(unsigned char ucNumWriteBytes);
void fnStoredInterruptionMsg();
unsigned char fnReadInterruptionFromNVMemory(long lAddr,unsigned char ucLength);

extern void fnReadCmdFromModdbus(long lAdd,unsigned char ucLength,unsigned char *pucReadData);
extern void fnWriteCmdFromModdbus(long lAdd,unsigned char ucLength,unsigned char *pucReadData);

extern void update_value(void);
extern unsigned char fnRead_Status_page(void);
void fnDataLogParam(void);
float Rounding( float  flValue);
void  fnStoredDataLoggParam(void);


//float fwrite_cycle;
#endif                    