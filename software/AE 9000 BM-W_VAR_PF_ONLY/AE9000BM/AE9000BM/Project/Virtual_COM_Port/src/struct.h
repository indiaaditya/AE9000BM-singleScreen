


/*Define address for parameter stored after energy reser 36 byte required*/

#define PERV_ADD                0x100//(RTU_STATUS+8)      //(4 byte for offset)     
#define INTG_Wh                 (PERV_ADD+(0x04*0x00))
#define PREV_IMP_VAh            (PERV_ADD+(0x04*0x01))
#define PREVInd_IMP_VARh        (PERV_ADD+(0x04*0x02))
#define PREV_EXP_Wh             (PERV_ADD+(0x04*0x03))
#define PREV_EXP_VAh            (PERV_ADD+(0x04*0x04))
#define PREVInd_EXP_VARh        (PERV_ADD+(0x04*0x05))
#define PREV_TOL_Wh     	(PERV_ADD+(0x04*0x06))
#define PREV_TOL_VAh            (PERV_ADD+(0x04*0x07))
#define PREVInd_TOL_VARh        (PERV_ADD+(0x04*0x08))//0x120


#define CHAR_ZERO     0x3F
#define CHAR_ONE      0x30
#define CHAR_TWO      0x6D
#define CHAR_THREE    0x79
#define CHAR_FOUR     0x72
#define CHAR_FIVE     0x5B
#define CHAR_SIX      0x5F
#define CHAR_SEVEN    0x31
#define CHAR_EIGHT    0x7F
#define CHAR_NINE     0x7B
#define CHAR_A        0x77
#define CHAR_b        0x5E
#define CHAR_C        0x0F
#define CHAR_c        0x4C
#define CHAR_d        0x7C
#define CHAR_E        0x4F
#define CHAR_F        0x47
#define CHAR_G        0x1F
#define CHAR_h        0x56
#define CHAR_I        0x06
#define CHAR_L        0x0E
#define CHAR_M1       0x27
#define CHAR_M2       0x33
#define CHAR_n        0x54
#define CHAR_O        0x3F
#define CHAR_o        0x5C
#define CHAR_P        0x67
#define CHAR_r        0x44
#define CHAR_S        0x5B
#define CHAR_t        0x4E
#define CHAR_u        0x1C
#define CHAR_U        0x3E
#define CHAR_y        0x7A
#define CHAR_BLANK    0x00
#define CHAR_MINUS    0x40
#define CHAR_H       0x76


#define PARAM_DISP      1
#define CONFIG_DISP     2
#define TEST_DISP       3
#define INIT_DISP 8

#define SET 	  1
#define CLEAR 	  0




static unsigned char const ConF_patts[4] = {CHAR_C, CHAR_o, CHAR_n, CHAR_F};
static unsigned char const CTR1_patts[4] = {CHAR_C, CHAR_t, CHAR_P, CHAR_r};
static unsigned char const CTR2_patts[4] = {CHAR_C, CHAR_t, CHAR_S, CHAR_E};
static unsigned char const PTR1_patts[4] = {CHAR_P, CHAR_t, CHAR_P, CHAR_r};
static unsigned char const PTR2_patts[4] = {CHAR_P, CHAR_t, CHAR_S, CHAR_E};

static unsigned char const sel_ConF_patts[4][4]={{CHAR_THREE, CHAR_P, CHAR_THREE, CHAR_E},{CHAR_THREE, CHAR_P, CHAR_TWO, CHAR_E},{CHAR_THREE, CHAR_P, CHAR_ONE, CHAR_E},{CHAR_ONE, CHAR_P, CHAR_ONE, CHAR_E}};
static unsigned char const C3P3E_patts[4] = {CHAR_THREE, CHAR_P, CHAR_THREE, CHAR_E};
static unsigned char const C3P2E_patts[4] = {CHAR_THREE, CHAR_P, CHAR_TWO, CHAR_E};
static unsigned char const C3P1E_patts[4] = {CHAR_THREE, CHAR_P, CHAR_ONE, CHAR_E};
static unsigned char const C1P1E_patts[4] = {CHAR_ONE, CHAR_P, CHAR_ONE, CHAR_E};
static unsigned char const ResEner_patts[4] = {CHAR_r, CHAR_E, CHAR_S, CHAR_E};
static unsigned char const Enr_patts[4] = {CHAR_E, CHAR_n, CHAR_G,CHAR_BLANK};

static unsigned char const ScrlT_patts[4] = {CHAR_S, CHAR_c, (CHAR_r), (CHAR_ONE | 0x80) };
static unsigned char const NwkId_patts[4] = {(CHAR_n ),CHAR_BLANK, CHAR_I, CHAR_d };

static unsigned char const Blank_patts[4] = {CHAR_BLANK, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};
static unsigned char const Num1_patts[4] = {CHAR_ONE, CHAR_TWO, CHAR_THREE, CHAR_FOUR};
static unsigned char const Num2_patts[4] = {CHAR_FIVE, CHAR_SIX, CHAR_SEVEN, CHAR_EIGHT};
static unsigned char const Dig2_patts[4] = {CHAR_EIGHT, CHAR_EIGHT, CHAR_BLANK, CHAR_BLANK};
static unsigned char const y_patts[4] = {CHAR_y, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};
static unsigned char const n_patts[4] = {CHAR_n, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};
static unsigned char const All_GLOW[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static unsigned char const blank_patts[4] = {CHAR_BLANK, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};
static unsigned char const zero_patts[4] = {CHAR_ZERO, CHAR_ZERO, CHAR_ZERO, CHAR_ZERO};
static unsigned char const InIt_patts[4] = {CHAR_I, CHAR_n, CHAR_I, (CHAR_t | 0x80)};

static unsigned char const Err_patts[4] = {CHAR_E, CHAR_r, CHAR_r, CHAR_BLANK};
static unsigned char const OL_patts[4] = {CHAR_O, CHAR_L, CHAR_BLANK, CHAR_BLANK};
static unsigned char const ON_patts[4] = {CHAR_O, CHAR_n, CHAR_BLANK, CHAR_BLANK};
static unsigned char const OF_patts[4] = {CHAR_O, CHAR_F, CHAR_F, CHAR_BLANK};
static unsigned char const AEL_patts[4] = {CHAR_A, CHAR_E, CHAR_BLANK, CHAR_BLANK};
static unsigned char const PLM_patts[4] = {CHAR_P, CHAR_L, CHAR_M1, CHAR_M2};
static unsigned char const Dev_patts[4] = {CHAR_BLANK, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};//IANET:24042024
static unsigned char const Pass_patts[4] = {CHAR_P, CHAR_A, CHAR_S, CHAR_S};
static unsigned char const Fail_patts[4] = {CHAR_F, CHAR_A, CHAR_I, CHAR_L};
static unsigned char const U_patts[4] = {CHAR_U, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};
static unsigned char const C_patts[4] = {CHAR_C, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};
static unsigned char const br_patts[4] = {(CHAR_b | 0x80), (CHAR_r| 0x80), CHAR_BLANK, CHAR_BLANK};//9600
//static unsigned char const br9600_patts[4] = {CHAR_FOUR, (CHAR_EIGHT| 0x80),CHAR_NINE, (CHAR_SIX| 0x80)};//9600
//static unsigned char const br1900_patts[4] = {CHAR_ONE, (CHAR_NINE| 0x80), CHAR_THREE,(CHAR_EIGHT| 0x80)};//4800
static unsigned char const Led_patts[4] = {CHAR_L, CHAR_E, CHAR_d, CHAR_BLANK};
static unsigned char const Test_patts[4] = {CHAR_t, CHAR_E, CHAR_S, CHAR_t};
static unsigned char const Exit_patts[4] = {CHAR_E, CHAR_H, CHAR_I, CHAR_t};
static unsigned char const u_patts[4] = {CHAR_u, CHAR_MINUS, CHAR_BLANK, CHAR_BLANK};
static unsigned char const met_patts[4] = {CHAR_M1, CHAR_M2, CHAR_E, CHAR_t};

static unsigned char const WH_patts[4] = {0x1E, 0x3C, CHAR_h, CHAR_BLANK};
static unsigned char const WH_patts_re[4] = {0x1E, 0x3C, CHAR_h, CHAR_MINUS};

static unsigned char const Y_patts[4] = {CHAR_y, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};
static unsigned char const N_patts[4] = {CHAR_n, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};
static unsigned char const reset_Energy_patts[4] = {CHAR_r, CHAR_E, CHAR_n, CHAR_G};

static unsigned char const AE_patts[4] = {CHAR_A, CHAR_E, CHAR_BLANK, CHAR_BLANK};
//static unsigned char const AE9000_patts[4] = {CHAR_NINE, CHAR_NINE, CHAR_ZERO, CHAR_ZERO};
static unsigned char const BM_patts[4] = {CHAR_BLANK, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK};//IANET:24042024
/*----------------Configuration Para Structure------------------------------------*/
  
typedef struct
{
  unsigned int uiSysConfiguration;      /*---System Config--------*/
//  unsigned int ulCTRPri;                /*---CT primary--------*/   //adi
//  unsigned int ulCTRSec;                /*---CT secondary--------*/ //adi
   float ulCTRPri;                /*---CT primary--------*/           //  adi032017
   float  ulCTRSec;                /*---CT secondary--------*/      //     adi032017
  // unsigned int ulPTRPri;                /*---PT primary--------*/   // adi
      float ulPTRPri;                                                 //  adi032017
 // unsigned int ulPTRSec;                /*---PT secondary--------*/ //  adi
     float  ulPTRSec;                                                //adi032017
  unsigned int uiScrollTime;            /*---scroll time--------*/
  unsigned int uiScrollON;              /*---scroll on/off--------*/
  unsigned int uiNetworkID;             /*---Mod ID--------*/
  unsigned int uiBaudRate;               /*---Baud Rate--------*/
}CONFIGURATION_PARAM;

/*----------------Measured Para Structure------------------------------------*/
typedef struct
{
 /* Voltage line-to-line per phase */
  float flVry;
  float flVyb;
  float flVbr;

  /*Voltage line-to-neutral per phase*/
  float flVrn;
  float flVyn;
  float flVbn;
  /*Voltage line-to-neutral Average*/
  float flVllavg;
  float flVlnavg;

  /*Current Per phase*/
  float flIr;
  float flIy;
  float flIb;
  /*Current Averaget*/
  float flIavg;
  /*Power factor Per phase*/
  /* Changes to add per phase PF */
  float flPFr;
  float flPFy;
  float flPFb;
  
  /*Power Factor Average*/
  float flPFavg;
  /*Frequency*/
  float flFreq;
  /*Active power per phase and total*/
  float flKWr;
  float flKWy;
  float flKWb;
  float flKWTot;
  /*Reactive power per phase & Total*/
  float flVARr;
  float flVARy;
  float flVARb;
  float flVARTot;
  /*Apparent power per phase & Total*/
  float flKVAr;
  float flKVAy;
  float flKVAb;
  float flKVATot;
  float AngL_R;
  float AngL_Y;
  float AngL_B;

  float AngL;

}MEASURED_PARAM;


/*----------------Uncalibrated value Structure------------------------------------*/
typedef struct
{

  /* Voltage line-to-line per phase */
  float Vry;
  float Vyb;
  float Vbr;
  /*Voltage line-to-line Average*/
  float Vllavg;
  /*Voltage line-to-neutral per phase*/
  float Vrn;
  float Vyn;
  float Vbn;
  /*Voltage line-to-neutral Average*/
  float Vlnavg;
  /*Current Per phase*/
  float Ir;
  float Iy;
  float Ib;
  /*Current Averaget*/
  float Iavg;
  /*Power Factor Average*/
  float PFavg;
  /*Frequency*/
  float Freq;
  /*Active power per phase and total*/
  float KWr;
  float KWy;
  float KWb;
  float KWTot;
  /*Reactive power per phase & Total*/
  float VARr;
  float VARy;
  float VARb;
  float VARTot;
  /*Apparent power per phase & Total*/
  float KVAr;
  float KVAy;
  float KVAb;
  float KVATot;
}UnCal;