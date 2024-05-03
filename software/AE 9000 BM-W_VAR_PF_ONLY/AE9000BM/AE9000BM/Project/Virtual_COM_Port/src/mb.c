#include "misc.h"
#include "stm32l1xx.h"
/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "misc.h"
#include <intrinsics.h>
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
#include "mbcrc.h"
#include "mbport.h"
#include "mbport.h"
#include "mbrtu.h"
#include "USART1_Function.h"
#include "stm32l1xx_conf.h"
#include "System_Configuration.h"
#include "struct.h"
#include "MyFun.h"
//***************************defination*****************************
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */



/* ----------------------- Static variables ---------------------------------*/
static UCHAR      ucMBAddress;   //=2;
static eMBMode    eMBCurrentMode;
extern BOOL            bTXEnabled;
extern BOOL            bRXEnabled;
extern CONFIGURATION_PARAM reqConfiguration;


static volatile eMBSndState eSndState;
static volatile eMBRcvState eRcvState;
volatile UCHAR  ucRTUBuf[MB_SER_PDU_SIZE_MAX];

//unsigned char ucRTUBuf[MB_SER_PDU_SIZE_MAX];

static volatile UCHAR *pucSndBufferCur;
static volatile USHORT usSndBufferCount;

static volatile USHORT usRcvBufferPos;


static peMBFrameSend peMBFrameSendCur;
static pvMBFrameStart pvMBFrameStartCur;
static pvMBFrameStop pvMBFrameStopCur;
static peMBFrameReceive peMBFrameReceiveCur;
static pvMBFrameClose pvMBFrameCloseCur;



static eMBEventType eQueuedEvent;
static BOOL     xEventInQueue;


static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS,eMBFuncWriteMultipleHoldingRegister}
};
//*****************************************
unsigned char fTxFrame = 0;
unsigned char uiTxFrameDelay=0;

static ULONG    ulNesting;
static __istate_t xOldState;

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */


/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL( *pxMBFrameCBByteReceived ) ( void );
BOOL( *pxMBFrameCBTransmitterEmpty ) ( void );
BOOL( *pxMBPortCBTimerExpired ) ( void );

BOOL( *pxMBFrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBFrameCBTransmitFSMCur ) ( void );




unsigned char Function_code;
extern unsigned mod_id;

/*****************main mb.c**************************/
void ucMBInit_condition()
 {
  eMBErrorCode    eStatus = MB_ENOERR;                                        // This is not used anywhere. It returns estatus;
  eStatus = MB_EINVAL;                                                        // Invalid Argument
  eMBRTUStart();                                                              // RTU start
  eMBState = STATE_ENABLED;                                                   // used anywhere Enable Modbus
 }

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBInit( eMBMode eMode, UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    /* check preconditions */
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN ) || ( ucSlaveAddress > MB_ADDRESS_MAX ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
       ucMBAddress=mod_id; //Changes Shree
       ucMBAddress = ucSlaveAddress;

        switch ( eMode )
        {

        case MB_RTU:
            pvMBFrameStartCur = eMBRTUStart;
            pvMBFrameStopCur = eMBRTUStop;
            peMBFrameSendCur = eMBRTUSend;
           // peMBFrameReceiveCur = eMBRTUReceive;
            pvMBFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBPortClose : NULL;
            pxMBFrameCBByteReceived = xMBRTUReceiveFSM;
            pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;
            pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;

            ucMBAddress=mod_id; //Changes Shree
            eStatus = eMBRTUInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;


        default:
            eStatus = MB_EINVAL;
        }

        if( eStatus == MB_ENOERR )
        {
            if( !xMBPortEventInit(  ) )
            {
                /* port dependent event module initalization failed. */
                eStatus = MB_EPORTERR;
            }
            else
            {
                eMBCurrentMode = eMode;
                eMBState = STATE_DISABLED;
            }
        }
    }
    return eStatus;
}

eMBErrorCode
eMBRegisterCB( UCHAR ucFunctionCode, pxMBFunctionHandler pxHandler )
{
    int             i;
    eMBErrorCode    eStatus;

    if( ( 0 < ucFunctionCode ) && ( ucFunctionCode <= 127 ) )
    {
      //  ENTER_CRITICAL_SECTION(  );
        if( pxHandler != NULL )
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( ( xFuncHandlers[i].pxHandler == NULL ) ||
                    ( xFuncHandlers[i].pxHandler == pxHandler ) )
                {
                    xFuncHandlers[i].ucFunctionCode = ucFunctionCode;
                    xFuncHandlers[i].pxHandler = pxHandler;
                    break;
                }
            }
            eStatus = ( i != MB_FUNC_HANDLERS_MAX ) ? MB_ENOERR : MB_ENORES;
        }
        else
        {
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {
                    xFuncHandlers[i].ucFunctionCode = 0;
                    xFuncHandlers[i].pxHandler = NULL;
                    break;
                }
            }
            /* Remove can't fail. */
            eStatus = MB_ENOERR;
        }
      //  EXIT_CRITICAL_SECTION(  );
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}


eMBErrorCode
eMBClose( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        if( pvMBFrameCloseCur != NULL )
        {
            pvMBFrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBEnable( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBFrameStartCur(  );
        eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBDisable( void )
{
    eMBErrorCode    eStatus;

    if( eMBState == STATE_ENABLED )
    {
        pvMBFrameStopCur(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBPoll( void )
{
    static UCHAR   *ucMBFrame;
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
    static USHORT   usLength;
    static eMBException eException;

    int             i;
    eMBErrorCode    eStatus = MB_ENOERR;
    eMBEventType    eEvent;
    /* Check if the protocol stack is ready. */
    if( eMBState != STATE_ENABLED )
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBPortEventGet( &eEvent ) == TRUE )
    {
        switch ( eEvent )
        {
        case EV_READY:
            break;

        case EV_FRAME_RECEIVED:
             //eStatus = peMBFrameReceiveCur( &ucRcvAddress, &ucMBFrame, &usLength );

                eStatus = eMBRTUReceive( &ucRcvAddress, &ucMBFrame, &usLength );    //here we get length= 5 bytes








            if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                ucMBAddress=mod_id;//Changes Shree
                if( ( ucRcvAddress ==ucMBAddress ) || ( ucRcvAddress == MB_ADDRESS_BROADCAST ) )   //ucMBAddress
                {
                    ( void )xMBPortEventPost( EV_EXECUTE );

                }
            }
            break;

        case EV_EXECUTE:


              // SendChar('A');

            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];


            Function_code=ucFunctionCode;
            eException = MB_EX_ILLEGAL_FUNCTION;
            for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
            {
                /* No more function handlers registered. Abort. */
                if( xFuncHandlers[i].ucFunctionCode == 0 )
                {
                    break;
                }
                else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
                {

                     //SendChar('A');
                    eException = xFuncHandlers[i].pxHandler( ucMBFrame, &usLength );

                    //    SendChar('E');


                    break;
                }
            }

            /* If the request was not sent to the broadcast address we
             * return a reply. */
            if( ucRcvAddress != MB_ADDRESS_BROADCAST )
            {
                if( eException != MB_EX_NONE )
                {
                    /* An exception occured. Build an error frame. */
                    usLength = 0;
                    ucMBFrame[usLength++] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR );
                    ucMBFrame[usLength++] = eException;
                }

                //eStatus = peMBFrameSendCur( ucMBAddress, ucMBFrame, usLength );
                   // SendChar('F');
                   // char_trans(usLength);
                 ucMBAddress=mod_id; //Changes Shree
                 eStatus = eMBRTUSend( ucMBAddress, ucMBFrame, usLength );



            }
            break;

        case EV_FRAME_SENT:
            break;
        }
    }
    return MB_ENOERR;
}

//********************RTU.c********************//















/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBRTUInit( UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    ULONG           usTimerT35_50us;

    ( void )ucSlaveAddress;
   // ENTER_CRITICAL_SECTION(  );

    /* Modbus RTU uses 8 Databits. */
    if( xMBPortSerialInit( ucPort, ulBaudRate, 8, eParity ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }
    else
    {
        /* If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if( ulBaudRate > 19200 )
        {
            usTimerT35_50us = 35;       /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
            usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
        }
        if( xMBPortTimersInit( ( USHORT ) usTimerT35_50us ) != TRUE )
        {
            eStatus = MB_EPORTERR;
        }
    }
   // EXIT_CRITICAL_SECTION(  );

    return eStatus;
}

void
eMBRTUStart( void )
{

    /* Initially the receiver is in the state STATE_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    eRcvState = STATE_RX_INIT;   // Receive state
    vMBPortSerialEnable( TRUE, FALSE ); // USART2 as Receiver
    vMBPortTimersEnable();    // Enable Timer T3.5 Character
}

void
eMBRTUStop( void )
{
   // ENTER_CRITICAL_SECTION(  );
    vMBPortSerialEnable( FALSE, FALSE );
    vMBPortTimersDisable(  );
   // EXIT_CRITICAL_SECTION(  );
}

eMBErrorCode
eMBRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    //BOOL            xFrameReceived = FALSE;
    eMBErrorCode    eStatus = MB_ENOERR;

   // ENTER_CRITICAL_SECTION(  );
    //assert( usRcvBufferPos < MB_SER_PDU_SIZE_MAX );
   // SendChar('D');
    /* Length and CRC check */

  //  char_trans(usRcvBufferPos);
    if( ( usRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( ( UCHAR * ) ucRTUBuf, usRcvBufferPos ) == 0 ) )
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = ucRTUBuf[MB_SER_PDU_ADDR_OFF];

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        *pusLength = ( USHORT )( usRcvBufferPos - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC );

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = ( UCHAR * ) & ucRTUBuf[MB_SER_PDU_PDU_OFF];
        //xFrameReceived = TRUE;

         //SendChar('E');

    }
    else
    {
        eStatus = MB_EIO;
    }

   // EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBRTUSend( UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;

  //  ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
   if( eRcvState == STATE_RX_IDLE )
   {


         vMBPortSerialEnable(FALSE,FALSE);
        /* First byte before the Modbus-PDU is the slave address. */
        pucSndBufferCur = ( UCHAR * ) pucFrame - 1;
        usSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usSndBufferCount += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) pucSndBufferCur, usSndBufferCount );
        ucRTUBuf[usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        ucRTUBuf[usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        /* Activate the transmitter. */
       eSndState = STATE_TX_XMIT;



      // vMBPortSerialEnable( FALSE, TRUE );
        //GPIO_SetBits(GPIOA, GPIO_Pin_1);
       // USART2->DR = 0x00;
        // GPIO_ResetBits(GPIOA, GPIO_Pin_1);


     fTxFrame =1;
    uiTxFrameDelay=0;




   }
   else
    {
       eStatus = MB_EIO;
    }
  //  EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

BOOL


xMBRTUReceiveFSM( void )
{
           BOOL            xTaskNeedSwitch = FALSE;
           UCHAR           ucByte;
          // UCHAR           Data;

    /* Always read the character. */
  //  Data=USART2->SR;
   // ucByte=USART_ReceiveData(USART2);
       ucByte=USART2->DR;
   // SendChar('A');
    // char_trans( ucByte);

    switch ( eRcvState )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_RX_INIT:

      vMBPortTimersEnable();

        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_RX_ERROR:
       vMBPortTimersEnable();
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE.
         */
    case STATE_RX_IDLE:
        usRcvBufferPos = 0;
      // SendChar('A');
        ucRTUBuf[usRcvBufferPos++] = ucByte;
        //SendChar(' ');
       // char_trans(ucRTUBuf[(usRcvBufferPos)-1]);
        //SendChar(' ');
        eRcvState = STATE_RX_RCV;

        /* Enable t3.5 timers. */
        vMBPortTimersEnable();
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if( usRcvBufferPos < 8)        //MB_SER_PDU_SIZE_MAX
        {

          //  SendChar('B');
            ucRTUBuf[usRcvBufferPos++] = ucByte;
           // char_trans(ucRTUBuf[(usRcvBufferPos)-1]);
           // SendChar(' ');
           // SendChar('C');
           // char_trans(usRcvBufferPos);
           // SendChar(' ');


        }


          if(usRcvBufferPos>=8)
            {
            // char_trans(usRcvBufferPos);
           vMBPortTimersEnable();


           }



     //  else
      //  {

       //    char_trans(usRcvBufferPos);
        //  vMBPortTimersEnable();


      //     eRcvState = STATE_RX_ERROR;
      // }
      //vMBPortTimersEnable( );
     break;
    }


        //SendChar('B');
    return xTaskNeedSwitch;
}

BOOL
xMBRTUTransmitFSM( void )
{
    BOOL            xNeedPoll = FALSE;
    // unsigned int  i;


   // assert( eRcvState == STATE_RX_IDLE );



    switch ( eSndState )
    {



        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_TX_IDLE:
        /* enable receiver/disable transmitter. */

         GPIO_ResetBits(GPIOA, GPIO_Pin_1);
         vMBPortSerialEnable( TRUE, FALSE );
        break;

    case STATE_TX_XMIT:
        /* check if we are finished. */


        if( usSndBufferCount != 0 )
        {

              //SendChar('D');
           // xMBPortSerialPutByte( ( CHAR )*pucSndBufferCur );
             USART_SendData( USART2,( CHAR )*pucSndBufferCur );    //**************doubt of how usart send data *********************
            pucSndBufferCur++;  /* next byte in sendbuffer. */
            usSndBufferCount--;
        }
        else
        {


             xNeedPoll = xMBPortEventPost( EV_FRAME_SENT );
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */

           //  for(i=0;i<8;i++)
           //  {
           //   ucRTUBuf[i]=0;
           //  }

           vMBPortSerialEnable(FALSE, FALSE );
            USART2->DR=0x00;

            vMBPortSerialEnable( TRUE, FALSE );
             eSndState = STATE_TX_IDLE;
          while(!(USART_GetFlagStatus(USART2, USART_FLAG_TC))) ;

           //  for (i=0 ;i<50; i++) ;
          //   GPIO_ResetBits(GPIOA, GPIO_Pin_1);


          //  GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_RESET);
           GPIO_ResetBits(GPIOA, GPIO_Pin_1);



        }
        break;
    }

    return xNeedPoll;
}

BOOL

xMBRTUTimerT35Expired( void )
{
    BOOL            xNeedPoll = FALSE;

    switch ( eRcvState  )
    {
        /* Timer t35 expired. Startup phase is finished. */
    case STATE_RX_INIT:
        xNeedPoll = xMBPortEventPost( EV_READY );

        break;

        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_RX_RCV:
        xNeedPoll = xMBPortEventPost( EV_FRAME_RECEIVED );



        break;

        /* An error occured while receiving the frame. */
    case STATE_RX_ERROR:
        break;

        /* Function called in an illegal state. */
    default:
        assert( ( eRcvState == STATE_RX_INIT ) ||
                ( eRcvState == STATE_RX_RCV ) || ( eRcvState == STATE_RX_ERROR ) );
    }

    vMBPortTimersDisable();
    eRcvState = STATE_RX_IDLE;
   // SendChar('C');
    return xNeedPoll;
}




//**************POTR.c***********************//


/* ----------------------- Variables ----------------------------------------*/


/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
    xEventInQueue = FALSE;
    return TRUE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xEventInQueue )
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}




/* ----------------------- Defines ------------------------------------------*/
#define USART0_ENABLED          ( 1 )
#define USART0_IDX              ( 0 )

#define USART1_ENABLED          ( 1 )
#define USART1_IDX              ( USART0_IDX + USART0_ENABLED * 1 )

#define USART_IDX_LAST          ( USART1_IDX )

#define USART_INVALID_PORT      ( 0xFF )
#define USART_NOT_RE_IDX        ( 3 )
#define USART_DE_IDX            ( 4 )

/* ----------------------- Static variables ---------------------------------*/



void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
    {
        bRXEnabled = TRUE;
        USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    }
    else
    {
       bRXEnabled = FALSE;
       USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
    }

    if( xTxEnable )
    {
       bTXEnabled = TRUE;
       USART_ITConfig(USART2,USART_IT_TXE ,ENABLE);
    }
    else
    {
       bTXEnabled = FALSE;
       USART_ITConfig(USART2,USART_IT_TXE ,DISABLE);
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  USART_InitTypeDef    USART_InitStructure;
      /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Configure USART Tx as alternate function push-pull */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART2);

  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

  /* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 9600 baud
        - Word Length = 8 Bits
        - Two Stop Bit
        - Odd parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
       if(reqConfiguration.uiBaudRate==1)
       {
           USART_InitStructure.USART_BaudRate = 4800;
       }
       else if(reqConfiguration.uiBaudRate==2)
       {
         USART_InitStructure.USART_BaudRate = 9600;
       }
      else if(reqConfiguration.uiBaudRate==3)
       {
         USART_InitStructure.USART_BaudRate = 19200;
       }
      else if(reqConfiguration.uiBaudRate==4)
       {
         USART_InitStructure.USART_BaudRate = 38400;
       }
       else
       {
         USART_InitStructure.USART_BaudRate = 9600;
       }

            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
            USART_InitStructure.USART_StopBits = USART_StopBits_1;//USART_StopBits_1;
            USART_InitStructure.USART_Parity =USART_Parity_No;// USART_Parity_Odd  ;//;// ;
            USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

            /* USART configuration */
            USART_Init(USART2, &USART_InitStructure);

  /* Enable Receive Interrupt */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  /* Enable USART */

   USART_Cmd(USART2, ENABLE);
  /* --------------------------NVIC Configuration -------------------------------*/
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

 return TRUE;

 /* Original code
 GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  USART_InitTypeDef    USART_InitStructure;
      // Enable GPIO clock
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  // Enable UART clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  // Configure USART Tx as alternate function push-pull

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Configure USART Rx as alternate function push-pull
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Connect PXx to USARTx_Tx
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART2);

  // Connect PXx to USARTx_Rx
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

  // USARTx configuration ------------------------------------------------------
  // USARTx configured as follow:
  //        - BaudRate = 9600 baud
  //        - Word Length = 8 Bits
  //        - Two Stop Bit
  //        - Odd parity
  //        - Hardware flow control disabled (RTS and CTS signals)
  //        - Receive and transmit enabled

       if(reqConfiguration.uiBaudRate==1)
       {
           USART_InitStructure.USART_BaudRate = 4800;
       }
       else if(reqConfiguration.uiBaudRate==2)
       {
         USART_InitStructure.USART_BaudRate = 9600;
       }
      else if(reqConfiguration.uiBaudRate==3)
       {
         USART_InitStructure.USART_BaudRate = 19200;
       }
      else if(reqConfiguration.uiBaudRate==4)
       {
         USART_InitStructure.USART_BaudRate = 38400;
       }


            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
            USART_InitStructure.USART_StopBits = USART_StopBits_1;//USART_StopBits_1;
            USART_InitStructure.USART_Parity =USART_Parity_No;// USART_Parity_Odd  ;//;// ;
            USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

            // USART configuration
            USART_Init(USART2, &USART_InitStructure);

  // Enable Receive Interrupt
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  // Enable USART

   USART_Cmd(USART2, ENABLE);
  // --------------------------NVIC Configuration -------------------------------
  // Enable the TIM2 gloabal Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

 return TRUE;

 */
}

void
vMBPortSerialClose( void )
{

}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{


  return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{


  return TRUE;
}






/* ----------------------- System includes ----------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/


#define MB_TIMER_DEBUG                      ( 0 )
#define MB_TIMER_PRESCALER                  ( 128UL )
#define MB_TIMER_TICKS                      ( BOARD_MCK / MB_TIMER_PRESCALER )
#define MB_50US_TICKS                       ( 20000UL )

#define TCX                                 ( TC0 )
#define TCXIRQ                              ( TC0_IRQn )
#define TCCHANNEL                           ( 0 )
#define TCX_IRQHANDLER                      TC0_IrqHandler

#define TC_CMRX_WAVE                        ( 0x1 << 15 )
#define TC_CMRX_TCCLKS_TIMER_DIV4_CLOCK     ( 0x3 << 0 )
#define TC_CMRX_CPCSTOP                     ( 0x1 << 6 )
#define TC_CMRX_WAVESEL_UP_RC               ( 0x2 << 13 )

#define TC_IERX_CPCS                        ( 0x1 << 4 )
#define TC_IERX_CPAS                        ( 0x1 << 2 )
#define TC_SRX_CPAS                         ( 0x1 << 2 )
#if MB_TIMER_DEBUG == 1
#define TIMER_PIN { 1 << 6, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT }
#endif

#ifndef SYSTICK_COUNTFLAG
/* missing in CMSIS */
#define SYSTICK_COUNTFLAG                   ( 16 )
#endif
/* ----------------------- Static variables ---------------------------------*/
#if MB_TIMER_DEBUG == 1
const static Pin xTimerDebugPins[] = { TIMER_PIN };
#endif

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);


  TIM_TimeBaseStructure.TIM_Prescaler =2;        // 1;//
  TIM_TimeBaseStructure.TIM_Period =46720;      // 50222; //46720;//54424;//15215;// 40215; //52224;//54424;//52224;//50215;//40215;//46720;//55252;//65520;//55525;//     //;//46720;////       46720;    //720;//55525;//
  /* Delay workingfor BaudRate of 9600, 14400, 19200, 38400 */
  TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);



  TIM_ITConfig(TIM10,TIM_IT_Update, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_Cmd(TIM10, ENABLE);

 return TRUE;
}

void
vMBPortTimerClose( void )
{




}

void vMBPortTimersEnable()
{
   TIM_Cmd(TIM10, ENABLE);                 //enable the timer for modbus port
}

void vMBPortTimersDisable(  )
{
   TIM_Cmd(TIM10,DISABLE);                //disable the timer for modbus port
}

void vMBPortTimersDelay( USHORT usTimeOutMS )
{

}

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortEnterCritical( void )
{
    __istate_t      xCurState;
    xCurState = __get_interrupt_state(  );
    __disable_interrupt(  );
    if( ulNesting == 0 )
    {
        xOldState = xCurState;
    }
    ulNesting++;
}

void
vMBPortExitCritical( void )
{
    ulNesting--;
    if( 0 == ulNesting )
    {
        __set_interrupt_state( xOldState );
    }
}

void
vMBPortClose( void )
{
    extern void vMBPortSerialClose( void );
    extern void vMBPortTimerClose( void );
    vMBPortSerialClose(  );
    vMBPortTimerClose(  );
}



//****************CRC.c********************//

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

static const UCHAR aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const UCHAR aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

USHORT
usMBCRC16( UCHAR * pucFrame, USHORT usLen )
{
    UCHAR           ucCRCHi = 0xFF;
    UCHAR           ucCRCLo = 0xFF;
    int             iIndex;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( UCHAR )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( USHORT )( ucCRCHi << 8 | ucCRCLo );
}


