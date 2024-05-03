
#include <intrinsics.h>
#include "stm32l1xx.h"
#include "target.h"
#include "Struct.h"
//#include "stm32l152d_eval_spi_ee.h"

#include "EXT_MEM.h"
#include "port.h"
#include "mbcrc.h"
#include <math.h>
#include <stdio.h>
#include <string.h>


#define WREN 	   0x06//write enble
#define WRDI 	   0x04//write disable
#define RDSR  	   0x05//read sts regi
#define WRSR  	   0x01//write sts regi
#define READ	   0x03//Read Data Bytes 
#define WRITE	   0x02//Write Data Bytes 
///////////////////////////////////////////////////////////////////////////////////////////////
//extern float flPrvReal,flPrvApp,flPrvReac;
extern unsigned char memory_location_flag;
extern unsigned long write_cycle;
unsigned long memory_flag;
union splitNcombineBytes
{
    unsigned char ucTemp[4];
    float flTemp;
    unsigned long ulTemp;
    unsigned int uiTemp; 
    int iTemp; 
    long lTemp;
};

unsigned char fTestBank2=0;
MEASURED_PARAM reqMeasuredParam;			
extern float KWH,Sum_Watt,Sum_Var,KVAR,KWHf,KVARf;
//CONFIGURATION_PARAM reqConfiguration;		
//PREV_PARAM reqPreviousParam;				

void fnWrite_enable()
{  
   //unsigned char Dummy1;
   unsigned char i=0;
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
 
  SPI_I2S_SendData(SPI2,WREN);
   for (i=0;i<100;i++);
   for (i=0;i<100;i++);
    
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
}

unsigned char fnRead_Status_page(void)
{
    unsigned char Dummy1;
    unsigned char i=0;
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
    
      for (i=0;i<100;i++);
      for (i=0;i<100;i++);
     
    SPI_I2S_SendData(SPI2,0x05);     
    Dummy1=SPI_I2S_ReceiveData(SPI2);
      for (i=0;i<100;i++);
      for (i=0;i<100;i++);
    
 
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    return Dummy1;    
}

void SendDataToMemory( unsigned char *pucData,unsigned char ucLength)
{
    unsigned char ucCount;
     unsigned char i=0;
    for (ucCount = 0 ; ucCount < ucLength; ucCount++)
    {
        SPI_I2S_SendData(SPI2,*pucData);
         for (i=0;i<100;i++);
      for (i=0;i<100;i++);
       i = SPI_I2S_ReceiveData(SPI2); // i is the dummy byte. of no use. //061222
        for (i=0;i<100;i++);
      for (i=0;i<100;i++);
            
        pucData++;
        
    }
}

/*****************************************************************************
** Function name:   fnWriteDataToMemory
**
** Descriptions:    Write a parameter in memory  as per group 
**
** parameters:      
                lAddr---->Addr where to write a parameter
                ucGroup---->which data wants to write
*****************************************************************************/
void fnWriteDataToMemory(long lAddr,unsigned char ucGroup )
{
   //unsigned char i=0;
   volatile char read_status=0;
   //int iCounter;  
   union splitNcombineBytes split;
   unsigned char ucWriteData[10];
   unsigned char ucIndex=0;
   //unsigned char ucCount;
   //unsigned char Dummy;
   fnWrite_enable();

    while((fnRead_Status_page() & 0x01)==0x01);
   
    ucWriteData[ucIndex++]=WRITE;
    
    split.lTemp=lAddr;

    ucWriteData[ucIndex++]=split.ucTemp[2];
    ucWriteData[ucIndex++]=split.ucTemp[1];
    ucWriteData[ucIndex++]=split.ucTemp[0];
  

    switch(ucGroup)
    {	
         case 0:
               split.flTemp = KWHf;
             
                ucWriteData[ucIndex++]=split.ucTemp[3];
                ucWriteData[ucIndex++]=split.ucTemp[2];
                ucWriteData[ucIndex++]=split.ucTemp[1];
                ucWriteData[ucIndex++]=split.ucTemp[0];
                break;
        
        case 1:
                 split.ulTemp =  write_cycle;
             
                 ucWriteData[ucIndex++]=split.ucTemp[3];
                 ucWriteData[ucIndex++]=split.ucTemp[2];
                 ucWriteData[ucIndex++]=split.ucTemp[1];
                 ucWriteData[ucIndex++]=split.ucTemp[0];
                 break;
       
       case 2:
                 split.ulTemp = (long) memory_location_flag;
             
                 ucWriteData[ucIndex++]=split.ucTemp[3];
                 ucWriteData[ucIndex++]=split.ucTemp[2];
                 ucWriteData[ucIndex++]=split.ucTemp[1];
                 ucWriteData[ucIndex++]=split.ucTemp[0];
                 
            
                  break;
      
   }       
    
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
    SendDataToMemory(ucWriteData,ucIndex);
   
    while((fnRead_Status_page() & 0x01)==0x01);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
       
}


/*****************************************************************************
** Function name  :   fnReadFromNVMemory
** Descriptions   :   read a data stotred in memory as addr
** parameters     :
		    lAddr   Address of memory form where data want to retrive
                    ucGroup   which group of data want to read
                            
** return         : None 
****************************************************************************/

/*##
Data which user want to retrive after power off on is divide into groups 
These groups are given struct.h file.  
##*/
void fnReadFromNVMemory(unsigned long lAddr,unsigned char ucGroup) 
{   
    //unsigned char i=0;
    unsigned char Dummy/*,rec_data*/;
    /*int iCounter;*/
    union splitNcombineBytes split;
    /*unsigned int iCounter1; */
    /*long Active_energy;*/
    /*unsigned char ucWriteData[10];*/
    /*unsigned char ucIndex=0;*/
    
    split.ulTemp=lAddr;
    
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
    
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == SET)
    {
        Dummy = SPI2->DR;	
    }
       
    SPI_I2S_SendData(SPI2,READ);
    Dummy = SPI_I2S_ReceiveData(SPI2);

    SPI_I2S_SendData(SPI2,split.ucTemp[2]);
    Dummy = SPI_I2S_ReceiveData(SPI2);

    SPI_I2S_SendData(SPI2,split.ucTemp[1]);
    Dummy = SPI_I2S_ReceiveData(SPI2);                       

    SPI_I2S_SendData(SPI2,split.ucTemp[0]);
    Dummy = SPI_I2S_ReceiveData(SPI2);
	
    Dummy=0xAA;
    switch(ucGroup)
    {
      case 0:
           SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[0]= SPI_I2S_ReceiveData(SPI2); 
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[3]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[2]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[1]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[0]= SPI_I2S_ReceiveData(SPI2);
    
            KWHf=split.flTemp; 
    
            break;
            
       case 1:
             SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[0]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[3]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[2]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[1]= SPI_I2S_ReceiveData(SPI2);
             SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[0]= SPI_I2S_ReceiveData(SPI2);
    
            write_cycle= split.ulTemp; 
       
              break;   
              
      case 2:      
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[0]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[3]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[2]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[1]= SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,Dummy);
            split.ucTemp[0]= SPI_I2S_ReceiveData(SPI2);
    
            memory_flag= split.ulTemp; 
            memory_location_flag = memory_flag;
            break;   
    }
     GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 

}
    

   