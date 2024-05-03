#include "stm32l1xx.h"
#include "USART1_Function.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>


union point
{
  double val;
  char val1[8];
};

union x
{
  float FNUM;
  unsigned char float_byte[4];
};
union x FconvertC;
extern float **ptr_par;
extern unsigned char Para_Max,Para_Strt;
void Uart_Frame_Received(void);
extern void sendchars(unsigned char *ch,unsigned short len);
extern float *s;
/////extern unsigned char Usart_buf[10];
///unsigned char arr_MbSend[35];
unsigned char Baud_Rate,Parity_bit,Stp_bits,Parity_B,Stop_B;
unsigned int device_id=1;
unsigned short crc=0x00,CRC1=0x00;
unsigned char length=0x00;
//arr2[30],arr3[30];
unsigned int  my_device_id=1;
extern unsigned char buf_in;
extern unsigned char query_complete_flag;
extern unsigned char *ptr_Trans, *ptr_rec;
/****************************************************************************
 Name: Crc16Modbus

 Description: Calculates the CRC-16 for Modbus

 Parameters: Crc - Crc calculated till current position.
             Data - Current 1 byte data

 Output: Crc - Crc including current position
*****************************************************************************/

unsigned short Crc16Modbus(unsigned short Crc, char Data)
{
  			unsigned char temp;

			Crc = Crc ^ Data;
 			for(temp=0; temp<8; temp++)
			{
    		if (Crc & 0x0001)
      		Crc = (Crc >> 1) ^ 0xA001;  // Polynomial used by MODBUS
    		else
      		Crc = (Crc >> 1);           // Right shifting algorithm
			}
			return Crc;
}


/****************************************************************************
 Name: crc_cal

 Description: Calculates the CRC-16 for Modbus

 Parameters: Crc - Crc calculated till current position.
             Data - Current 1 byte data

 Output: Crc - Crc including current position
*****************************************************************************/

unsigned short crc_cal(unsigned char *data,unsigned char len)
{
    unsigned short crc;
    unsigned char i;

    crc = 0xFFFF; // Initial state

    for(i=0; i<len; i++)
	{
		crc = Crc16Modbus(crc, data[i]);
	}
    return crc;
}

/****************************************************************************
 Functio     : Uart_Frame_Received
 Description : when data on Modbus received this function will be evoked
 Parameters  : Null
*****************************************************************************/

void Uart_Frame_Received(void)
{
  
  unsigned int ret_CRC=0,strt_addr=0,no_point=0,P_start=0;
  unsigned char incr=0;
  // unsigned char y,k,j;
  CRC1=0;
  // long int val=9999999999;
 // union point value;
  crc   = crc_cal((&(*ptr_rec)),6);	//CALCULATE CRC OF DATA RECEIVED VIA MODBUS
  CRC1 |= *(ptr_rec+7);
  CRC1  = CRC1 << 8;
  CRC1 |= *(ptr_rec+6);						   					//CRC TAKEN FROM MODBUS
  if(crc==CRC1)							   					//COMPARE CALCULATED CRC AND RECEIVED CRC
  {
    if(device_id==(*(ptr_rec+0)))
    {
      *(ptr_Trans+0)=device_id;
      if(*(ptr_rec+1) == 0x03)						 					// 0X03 MODBUS FUNCTION CODE FOR READ OPERATION OF MODBUS
      {
        *(ptr_Trans+1)=0x03;
        P_start=(*(ptr_rec+3))/2;
        strt_addr = ((((unsigned int)(*(ptr_rec+2)))<<8) | ((unsigned int)(*(ptr_rec+3))));
        no_point = ((((unsigned int)(*(ptr_rec+4)))<<8) | ((unsigned int)(*(ptr_rec+5))));
        if((strt_addr<=58)&&(((*(ptr_rec+3))%2)==0)&&(((no_point)+ strt_addr)<=60)&&(no_point%2==0))
        {
          *(ptr_Trans+2)=no_point*2;         // double the number of points 
          incr=3;
          while(no_point)
          {
            FconvertC.FNUM=**(ptr_par+P_start);
            *(ptr_Trans+(incr++)) = FconvertC.float_byte[1];
            *(ptr_Trans+(incr++)) = FconvertC.float_byte[0];
            *(ptr_Trans+(incr++)) = FconvertC.float_byte[3];
            *(ptr_Trans+(incr++)) = FconvertC.float_byte[2];
            no_point-=2;
            ++P_start;
       // --no_point;
          }
          ret_CRC=crc_cal(&(*ptr_Trans),incr);
          *(ptr_Trans+incr)=(unsigned char)(ret_CRC);
          *(ptr_Trans+(incr+1))=(unsigned char)(ret_CRC >> 8);
          
//          buf_in=incr+1;
//          query_complete_flag=1;
//          GPIO_SetBits(GPIOB,GPIO_Pin_2);
//          USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
//          USART_ITConfig(USART3, USART_IT_TC, ENABLE);
//          USART_SendData(USART3, arr_MbSend[0]);
        }
        
    
        else
        {
          *(ptr_Trans+1)=(*(ptr_rec+1))|0x80;
          *(ptr_Trans+2)=0x02;
          incr=3;
          ret_CRC=crc_cal(&(*ptr_Trans),incr);
          *(ptr_Trans+incr)=(unsigned char)(ret_CRC);
          *(ptr_Trans+(incr+1))=(unsigned char)(ret_CRC >>8);
//          buf_in=incr+1;
//          query_complete_flag=1;
//          GPIO_SetBits(GPIOB,GPIO_Pin_2);
//          USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
//          USART_ITConfig(USART3, USART_IT_TC, ENABLE);
//          USART_SendData(USART3, arr_MbSend[0]);
        }
      }
      else
      {
         *(ptr_Trans+1)=(*(ptr_rec+1))|0x80;
          *(ptr_Trans+1)=0x01;
          incr=3;
          ret_CRC=crc_cal(&(*ptr_Trans),incr);
          *(ptr_Trans+incr)=(unsigned char)(ret_CRC);
          *(ptr_Trans+(incr+1))=(unsigned char)(ret_CRC >>8);
//        buf_in=incr+1;
//        query_complete_flag=1;
//        GPIO_SetBits(GPIOB,GPIO_Pin_2);
//        USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
//        USART_ITConfig(USART3, USART_IT_TC, ENABLE);
//        USART_SendData(USART3, arr_MbSend[0]);
      }
      buf_in=incr+1;
      query_complete_flag=1;
      GPIO_SetBits(GPIOA,GPIO_Pin_1);
     // USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
      
      USART_SendData(USART2, *(ptr_Trans+0));
      USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    }
  }
//////        if((strt_addr<=80)&&(((*(ptr_rec+3))%2)==0)&&(((*(ptr_rec+3))+ no_point)<=84)&&(no_point%2==0))
//////        {
//////          
//////          if((strt_addr<33)&&(strt_addr%2==0))
//////          {
//////            fpoints=(strt_addr-32);
//////            dpoints=(no_point-(2*fpoints));
//////             
//////          }
//////          else if(strt_addr%4==0)
//////          {
//////             
//////          }
//////          else
//////          {
//////            //invalid starting address
//////          }
//////        } 
          
          
}
////////          if((Usart_buf[2]==0x00) && (Usart_buf[3] ==0x00))				   	// read 	registers from 00 to 13
////////        {
////////          length = Usart_buf[5]*2;								   	//calculate length of data to be send on modbus
////////          value.val=(double)*s;
////////        // value.val1[7],value.val1[6],value.val1[5],value.val1[4],
////////        sprintf(arr3,"%c%c%c%c%c%c%c%c",value.val1[7],value.val1[6],value.val1[5],value.val1[4],value.val1[3],value.val1[2],value.val1[1],value.val1[0]);
////////        sprintf(arr2,"%c%c%c",0x01,0x03,length);
////////        for(y=0,k=0,j=3;k<length;++y,++k,++j)
////////        {
////////          arr2[j]=arr3[y];
////////        }
////////        crc=crc_cal(arr2,j);
////////        arr2[j++]= crc & 0xff;
////////        arr2[j++]=(crc & 0xff00)>>8;
////////        sendchars(arr2,j);
////////      }
////////      else
////////      {
////////        Usart_buf[1] |=0x80;
////////        sprintf(arr3,"%c%c%c",0x01,Usart_buf[1],0x02);
////////        crc=crc_cal(arr3,3);
////////        sprintf(arr3,"%c%c%c%c%c",0x01,Usart_buf[1],0x02,crc&0xff,(crc&0xff00)>>8);
////////        sendchars(arr3,5);
////////      }
////////    }
////////         else if(Usart_buf[1]==0x06)											// 0X06 MODBUS FUNCTION CODE FOR WRITE OPERATION OF MODBUS
////////         {
////////          if((Usart_buf[2]==0x00) && (Usart_buf[3]==0x00))		   					// 0X00 ADDRESS TO SET LOGIN TIME TO WRITE LOG ON EEPROM
////////	  {
////////           if(Usart_buf[4]<0x3C && Usart_buf[5]<0x3C)
////////            {
////////
////////
////////	   }
////////	   else
////////	   {
////////	     Usart_buf[1] |=0x80;
////////	     sprintf(arr3,"%c%c%c",0x01,Usart_buf[1],0x03);
////////	     crc=crc_cal(arr3,3);
////////	     sprintf(arr3,"%c%c%c%c%c",0x01,Usart_buf[1],0x03,crc&0xff,(crc&0xff00)>>8);
////////	     sendchars(arr3,5);
////////	   }
////////          }
////////	  else if((Usart_buf[2]==0x00) && (Usart_buf[3]==0x01))		 				// 0X01 ADDRESS TO SET HOUR AND MINUTE OF RTC
////////	  {
////////	    if(Usart_buf[4]<0x3C && Usart_buf[5]<0x3C)
////////	    {
////////		  sendchars(Usart_buf,8);
////////	    }
////////	    else
////////	    {
////////		  Usart_buf[1] |=0x80;
////////		  sprintf(arr3,"%c%c%c",0x01,Usart_buf[1],0x03);
////////		  crc=crc_cal(arr3,3);
////////		  sprintf(arr3,"%c%c%c%c%c",0x01,Usart_buf[1],0x03,crc&0xff,(crc&0xff00)>>8);
////////		  sendchars(arr3,5);
////////		}
////////	   }
////////     else if ((Usart_buf[2]==0x00) && Usart_buf[3]==0x02)					// 0X02 ADDRESS TO SET SECOND OF RTC
////////	 {
////////	   if(Usart_buf[4] < 0x3C && Usart_buf[5]==0)
////////		{
////////		  sendchars(Usart_buf,8);
////////		}
////////		else
////////	 	{
////////	          Usart_buf[1] |=0x80;
////////		  sprintf(arr3,"%c%c%c",0x01,Usart_buf[1],0x03);
////////		  crc=crc_cal(arr3,3);
////////		  sprintf(arr3,"%c%c%c%c%c",0x01,Usart_buf[1],0x03,crc&0xff,(crc&0xff00)>>8);
////////		  sendchars(arr3,5);
////////		}
////////	  }
////////	 else if((Usart_buf[2]==0x00) && (Usart_buf[3]==0x03))					 // 0X03 ADDRESS TO SET DATE AND MONTH
////////	 {
////////		 if(Usart_buf[4] && Usart_buf[5])
////////		 {
////////		  if((Usart_buf[4]<=0x1f) && (Usart_buf[5]<=0x0C))
////////	         {
////////			 sendchars(Usart_buf,8);
////////	         }
////////		}
////////		else
////////         	{
////////		   Usart_buf[1] |=0x80;
////////		   sprintf(arr3,"%c%c%c",0x01,Usart_buf[1],0x03);
////////		   crc=crc_cal(arr3,3);
////////		   sprintf(arr3,"%c%c%c%c%c",0x01,Usart_buf[1],0x03,crc&0xff,(crc&0xff00)>>8);
////////	           sendchars(arr3,5);
////////	    }
////////	  }
////////	  else
////////	  {
////////	      Usart_buf[1] |=0x80;
////////	      sprintf(arr3,"%c%c%c",0x01,Usart_buf[1],0x02);
////////	      crc=crc_cal(arr3,3);
////////              sprintf(arr3,"%c%c%c%c%c",0x01,Usart_buf[1],0x02,crc&0xff,(crc&0xff00)>>8);
////////	      sendchars(arr3,5);
////////          }
////////         }
////////     }
////////   CRC1=0x00;
////////   crc=0x00;
////////}
extern unsigned char buf_in;
extern unsigned char buf_out;
extern unsigned char Tx_buf[256];
extern unsigned char transmit;

void delay4(unsigned int z);
void sendchars(unsigned char *ch,unsigned short len)
{

	while(len)
	{
	   Tx_buf[buf_in]=*ch;
		buf_in++;
		ch++;
		len--;
        if(transmit && len==0)
	{
		  GPIOB->BSRRL |=(1<<2);					// Modbus direction bit set to transmit
		  delay4(2);
		   transmit=0;
		   USART3->CR1 |= USART_FLAG_TXE;
        }
   }
}

void delay4(unsigned int z)
{
	unsigned char x;
	unsigned int y;
	for(y=0;y<z;y++)
	{
	  for(x=0;x<35;x++)
	  {}
	}
}