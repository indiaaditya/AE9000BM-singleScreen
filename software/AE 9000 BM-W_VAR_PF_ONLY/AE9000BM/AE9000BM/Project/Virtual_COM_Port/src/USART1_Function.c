#include "stm32l1xx.h"
#include "USART1_Function.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>


char flaot2ascii[23];
char flaot2ascii2[23];
char flaot2ascii3[23];
unsigned char flot_rec_1,flot_rec_2[2],flot_rec_3[3],flot_rec_4[4],flot_rec_5[5],flot_rec_6[6],
flot_rec_7[7],flot_rec_8[8],flot_rec_9[9],flot_rec_10[10],flot_rec_11[11],flot_rec_12[12],
flot_rec_14[14];
extern unsigned char str5[3],str6[3];
extern void Delay(__IO uint32_t);

/**********************************************************************************************/
 /*  Function: DispFloat
  This Function Made for Displaying/Sending Float value on Hyper Terminal or to VB software
with + sign before & at the end - sign Example : +123.456789-
  By passing it float vaule to be displayed or sent.  */
/**********************************************************************************************/

void DispFloat(float temp3)
 {
   //float tempDisp=0.0000;
   unsigned char j=1;

   sprintf(flaot2ascii,"%12.12f",temp3);
   sprintf(flaot2ascii,"%12.12f",temp3);
   //SendChar(0x00);
   SendChar('+');
   USART1->DR=flaot2ascii[0];
   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
   for(j=1;j<(strlen(flaot2ascii));j++)
   {
     USART1->DR=flaot2ascii[j];
     while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      {}
   }	
   SendChar('-');
 	
}



/*****************************************************************************************************/
void DispFlot(float temp4)
 {
   //float tempDisp1=0.0000;
   unsigned char j=1;

   Delay(0x03);
   //sprintf(flaot2ascii2,"%12.12f",tempDisp1);
   sprintf(flaot2ascii2,"%12.12f",temp4);
   sprintf(flaot2ascii2,"%12.12f",temp4);
   Delay(0x03);
   //SendChar(0x00);
   SendChar('+');
   USART1->DR=flaot2ascii2[0];
   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
   for(j=1;j<(strlen(flaot2ascii2));j++)
   {
     USART1->DR=flaot2ascii2[j];
     while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      {}
   }	
   SendChar('-');
 	
}

/**********************************************************************************************/
//----------------------------- Display  Float without Sign -----------------------------------
/***********************************************************************************************/

void DispFloatWo(float temp_float)
 {
   unsigned char h = 1;
   sprintf(flaot2ascii3,"%5.5f",temp_float);
   sprintf(flaot2ascii3,"%5.5f",temp_float);

   for(h=0; h <(strlen(flaot2ascii3)); h++)
   {
     USART1->DR=flaot2ascii3[h];
     while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      {}
    Delay(5);    //parijat
   }	

   SendChar(';');
  // SendChar((strlen(flaot2ascii3)+0x30));
  // SendChar(':');
 }




/**********************************************************************************************/
 /*  Function: flot_rec_var
  This Function Made for Receiving Float value 1 to 12 digit (14 including + &- Sign) on Hyper Terminal or to VB software
with + sign before & at the end - sign Example : +123.456789-
  By passing it float vaule to be displayed or sent.  */
/**********************************************************************************************/



void flot_rec_var (float * flot_data_rec)
{
      static unsigned char flag_sign=0,pt_array=0; //i_cnt,temp_zero,l_f=0, //float trytp;
      double rec_data_f;
      flag_sign=0;
      pt_array=0;


      while(flag_sign!=2)

      {
          while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
          USART_ClearFlag(USART1,USART_FLAG_RXNE);
          flot_rec_14[pt_array]=USART1->DR;

          if(((flot_rec_14[pt_array]==43)||(flot_rec_14[pt_array]==45)))
          {
                flag_sign=flag_sign+1;
          }	
          pt_array=pt_array+1;					
      }

      pt_array=pt_array-2;	

      if(pt_array==1)
      {

        flot_rec_1=flot_rec_14[1];
        rec_data_f=((flot_rec_1) - 0x30);//rec_data_f=atof(&flot_rec_1);
        *flot_data_rec = (float)rec_data_f;

      }
      else if(pt_array==2)
      {
        flot_rec_2[0]=flot_rec_14[1];
        flot_rec_2[1]=flot_rec_14[2];
        rec_data_f=(float)strtod(flot_rec_2, (char **)NULL);//atof(flot_rec_2);
        *flot_data_rec = (float)rec_data_f;
      }
      else if(pt_array==3)
      {
        flot_rec_3[0]=flot_rec_14[1];
        flot_rec_3[1]=flot_rec_14[2];
        flot_rec_3[2]=flot_rec_14[3];
        rec_data_f=(float)strtod(flot_rec_3, (char **)NULL);//atof(flot_rec_3);
        *flot_data_rec = (float)rec_data_f;
      }
      else if(pt_array==4)
      {
        flot_rec_4[0]=flot_rec_14[1];
        flot_rec_4[1]=flot_rec_14[2];
        flot_rec_4[2]=flot_rec_14[3];
        flot_rec_4[3]=flot_rec_14[4];
        rec_data_f=(float)strtod(flot_rec_4, (char **)NULL);//atof(flot_rec_4);
        *flot_data_rec = (float)rec_data_f;

      }
      else if(pt_array==5)
      {
        flot_rec_5[0]=flot_rec_14[1];
        flot_rec_5[1]=flot_rec_14[2];
        flot_rec_5[2]=flot_rec_14[3];
        flot_rec_5[3]=flot_rec_14[4];
        flot_rec_5[4]=flot_rec_14[5];
        rec_data_f=(float)strtod(flot_rec_5, (char **)NULL);//atof(flot_rec_5);
        *flot_data_rec = (float)rec_data_f;

      }
      else if(pt_array==6)
      {
        flot_rec_6[0]=flot_rec_14[1];
        flot_rec_6[1]=flot_rec_14[2];
        flot_rec_6[2]=flot_rec_14[3];
        flot_rec_6[3]=flot_rec_14[4];
        flot_rec_6[4]=flot_rec_14[5];
        flot_rec_6[5]=flot_rec_14[6];
        rec_data_f=(float)strtod(flot_rec_6, (char **)NULL);//atof(flot_rec_6);
        *flot_data_rec = (float)rec_data_f;
      }
      else if(pt_array==7)
      {
        flot_rec_7[0]=flot_rec_14[1];
        flot_rec_7[1]=flot_rec_14[2];
        flot_rec_7[2]=flot_rec_14[3];
        flot_rec_7[3]=flot_rec_14[4];
        flot_rec_7[4]=flot_rec_14[5];
        flot_rec_7[5]=flot_rec_14[6];
        flot_rec_7[6]=flot_rec_14[7];
        rec_data_f=(float)strtod(flot_rec_7, (char **)NULL);//atof(flot_rec_7);
        *flot_data_rec = (float)rec_data_f;
      }
      else if(pt_array==8)
      {
        flot_rec_8[0]=flot_rec_14[1];
        flot_rec_8[1]=flot_rec_14[2];
        flot_rec_8[2]=flot_rec_14[3];
        flot_rec_8[3]=flot_rec_14[4];
        flot_rec_8[4]=flot_rec_14[5];
        flot_rec_8[5]=flot_rec_14[6];
        flot_rec_8[6]=flot_rec_14[7];
        flot_rec_8[7]=flot_rec_14[8];
        rec_data_f=(float)strtod(flot_rec_8, (char **)NULL);//atof(flot_rec_8);
        *flot_data_rec = (float)rec_data_f;
      }
      else if(pt_array==9)
      {
        flot_rec_9[0]=flot_rec_14[1];
        flot_rec_9[1]=flot_rec_14[2];
        flot_rec_9[2]=flot_rec_14[3];
        flot_rec_9[3]=flot_rec_14[4];
        flot_rec_9[4]=flot_rec_14[5];
        flot_rec_9[5]=flot_rec_14[6];
        flot_rec_9[6]=flot_rec_14[7];
        flot_rec_9[7]=flot_rec_14[8];
        flot_rec_9[8]=flot_rec_14[9];
        rec_data_f=(float)strtod(flot_rec_9, (char **)NULL);//atof(flot_rec_9);
        *flot_data_rec = (float)rec_data_f;
      }
      else if(pt_array==10)
      {
        flot_rec_10[0]=flot_rec_14[1];
        flot_rec_10[1]=flot_rec_14[2];
        flot_rec_10[2]=flot_rec_14[3];
        flot_rec_10[3]=flot_rec_14[4];
        flot_rec_10[4]=flot_rec_14[5];
        flot_rec_10[5]=flot_rec_14[6];
        flot_rec_10[6]=flot_rec_14[7];
        flot_rec_10[7]=flot_rec_14[8];
        flot_rec_10[8]=flot_rec_14[9];
        flot_rec_10[9]=flot_rec_14[10];
        rec_data_f=(float)strtod(flot_rec_10, (char **)NULL);//atof(flot_rec_10);
        *flot_data_rec = (float)rec_data_f;
      }
      else if(pt_array==11)
      {
        flot_rec_11[0]=flot_rec_14[1];
        flot_rec_11[1]=flot_rec_14[2];
        flot_rec_11[2]=flot_rec_14[3];
        flot_rec_11[3]=flot_rec_14[4];
        flot_rec_11[4]=flot_rec_14[5];
        flot_rec_11[5]=flot_rec_14[6];
        flot_rec_11[6]=flot_rec_14[7];
        flot_rec_11[7]=flot_rec_14[8];
        flot_rec_11[8]=flot_rec_14[9];
        flot_rec_11[9]=flot_rec_14[10];
        flot_rec_11[10]=flot_rec_14[11];
        rec_data_f=(float)strtod(flot_rec_11, (char **)NULL);//atof(flot_rec_11);
        *flot_data_rec = (float)rec_data_f;

      }
      else if(pt_array==12)
      {
        flot_rec_12[0]=flot_rec_14[1];
        flot_rec_12[1]=flot_rec_14[2];
        flot_rec_12[2]=flot_rec_14[3];
        flot_rec_12[3]=flot_rec_14[4];
        flot_rec_12[4]=flot_rec_14[5];
        flot_rec_12[5]=flot_rec_14[6];
        flot_rec_12[6]=flot_rec_14[7];
        flot_rec_12[7]=flot_rec_14[8];
        flot_rec_12[8]=flot_rec_14[9];
        flot_rec_12[9]=flot_rec_14[10];
        flot_rec_12[10]=flot_rec_14[11];
        flot_rec_12[11]=flot_rec_14[12];
        rec_data_f=(float)strtod(flot_rec_12, (char **)NULL);//atof(flot_rec_12);
        *flot_data_rec = (float)rec_data_f;
      }
                                                  //trytp = rec_data_f;
   /*   if(flag_sign==2)
      {
        SendYack();//DispString(str5,3); //disp_volt(trytp); for Testing
      }
      else
      {
        SendNack();//DispString(str6,3);
      }*/
      return;
 }



/**********************************************************************************************/
 /*  Function: SendChar
  This Function Made for Transfering Single Character on Hyper Terminal or to VB software
  By passing it Character vaule/ ASCII to be displayed or sent.  */
/**********************************************************************************************/

void SendChar(char ch)
{
    USART1->DR = (ch & 0x1FF);
    while (!(USART1->SR & USART_FLAG_TXE));
}




/**********************************************************************************************/
 /*  Function: DispString
  This Function Made for Transfering String on Hyper Terminal or to VB software
  By passing it String array pointer and number of character to be displayed or sent.  */
/**********************************************************************************************/

void DispString (unsigned char *string, unsigned char x)
{
    unsigned int i;

    for (i=0; i<x; i++)
    {
        USART1->DR=*string;
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE)== RESET);
        USART_ClearFlag(USART1,USART_FLAG_TXE);
        string++;
    }
}



/**********************************************************************************************/
 /*  Function: DispDeci (Conversion to Decimal &  Display it)
  This Function Made for Displaying / Transfering Decimal Value (0 to 255) on Hyper Terminal or
  to VB software, By passing it Hex/ASCII Value to be displayed or sent.  */
/**********************************************************************************************/


 void DispDeci(unsigned char temp_char)
 {
      unsigned char test[3],lk;
      /* Converting Hex to Decimal*/
      test[2]=temp_char%10;
      temp_char=temp_char/10;
      test[1]=temp_char%10;
      temp_char=temp_char/10;
      test[0]=temp_char;

      USART1->DR=0x09;
      while(!(USART1->SR & USART_FLAG_TXE));

      for(lk=0;lk<3;lk++)
      {
          USART1->DR=test[lk]+48;
          while(!(USART1->SR & USART_FLAG_TXE));		
      }
 }



/**********************************************************************************************/
/* Function: CheckACK (Checking Acknowledge "+r-" from PC)
  This Function Made for Receiving acknowlement from PC / VB software "+r-"
  and it returns character 1 if coreectly received othervise return 0.  */
/**********************************************************************************************/

char CheckACK(void)
{
      unsigned char n, temp_ack[3];
      for(n=0;n<3;n++)
      {
          while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
          USART_ClearFlag(USART1,USART_FLAG_RXNE);
          temp_ack[n]= USART1->DR;
      }

      if (temp_ack[0] == '+')				  // str5[0]
      {
          if (temp_ack[1] == 'r')
          { 	
              if (temp_ack[2] == '-')
              {
                  return (1);   		
              }
              else
              {
                  return (0);
              }
          }
      }
  return('N');
}



/**********************************************************************************************/
/* Function: usart_rec_n_sign
  This Function Made for Receiving String/Character from PC / VB software eliminating '+' & '-'
  sign By passing it array pointer & number of character to be received. */
/**********************************************************************************************/


void usart_rec_n_sign(unsigned char* rx_data2_sign, unsigned char m_sign)
{
      unsigned char o_sign,check_sign=0,usart_rec_temp;
      m_sign=m_sign+2;
      check_sign=0;
      for(o_sign=0;o_sign<m_sign;o_sign++)
      {
          while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
          USART_ClearFlag(USART1,USART_FLAG_RXNE);
          usart_rec_temp=USART1->DR;
          if((usart_rec_temp!=43)&&(usart_rec_temp!=45))
          {
              *rx_data2_sign= usart_rec_temp;
              if(m_sign!=3)
              {
                  rx_data2_sign++;
              }
          }
          else if((usart_rec_temp==43)||(usart_rec_temp==45))
          {
              check_sign=check_sign+1;
          }
      }
      if(check_sign==2)
      {
//         if(*(rx_data2_sign+1)!='N')
//         {
            SendYack();//DispString(str5,3);
//         }
      }
      else
      {
          SendNack();//DispString(str6,3);
      }
      return;
} 		




/**********************************************************************************************/
/* Function: usart_rec_n
  This Function Made for Receiving String/Character from PC / VB software
  By passing it array pointer & value of number of character to be received. */
/**********************************************************************************************/

void usart_rec_n(unsigned char* rx_data2, unsigned char m)
{
    unsigned char o;
    for(o=0;o<m;o++)
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
        USART_ClearFlag(USART1,USART_FLAG_RXNE);
        *rx_data2= USART1->DR;
        rx_data2++;
    }
    return;
}



/**********************************************************************************************/
/* Function: SendYack
  This Function Made for Transfering +Y- String to PC / VB software
 */
/**********************************************************************************************/


void SendYack(void)
{
 USART1->DR='+';
 while(!(USART1->SR & USART_FLAG_TXE));
 USART1->DR='Y';
 while(!(USART1->SR & USART_FLAG_TXE));
 USART1->DR='-';
 while(!(USART1->SR & USART_FLAG_TXE));
 return;
}


/**********************************************************************************************/
/* Function: SendNack
  This Function Made for Transfering +Y- String to PC / VB software
 */
/**********************************************************************************************/

void SendNack(void)
{
 USART1->DR='+';
 while(!(USART1->SR & USART_FLAG_TXE));
 USART1->DR='N';
 while(!(USART1->SR & USART_FLAG_TXE));
 USART1->DR='-';
 while(!(USART1->SR & USART_FLAG_TXE));
 return;
}



/**********************************************************************************************/
/* Function: uart_rec
  This Function Made for Receiving 4 byte from PC / VB software
 */
/**********************************************************************************************/

void  uart_rec(unsigned char* rx_data1)
 {
   unsigned char m;
   for(m=0;m<4;m++)
  {
	
	 //SendChar('O');
     while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
	 //	SendChar('K');
     USART_ClearFlag(USART1,USART_FLAG_RXNE);
	 *rx_data1= USART1->DR;
	  rx_data1++;

  }
  return;
}