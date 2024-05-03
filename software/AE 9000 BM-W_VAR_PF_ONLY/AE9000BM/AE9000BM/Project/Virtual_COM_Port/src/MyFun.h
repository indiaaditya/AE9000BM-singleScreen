void convert_to_dec_patt_signed8(signed long val, unsigned char * patts);
void convert_to_dec_patt_signed(signed int val, unsigned char * patts);
void convert_to_dec_patt_signed2(signed int val, unsigned char * patts);
void convert_to_dec_patt_signed3(signed int val, unsigned char * patts);
void convert_to_hex_patt(int val, unsigned char *patts);
void MountDisplay(unsigned int * DisplayScreen, unsigned int uiBlinkChar);
int Floatpatt4(float,unsigned char *);
long Floatpatt8(float , unsigned char *);
inline float fnabs(float);

void CreatePage(unsigned char ucPageType, unsigned char , unsigned char const * , unsigned char const * , unsigned char const * , unsigned char const * );
void Display(unsigned int *);
void CreatePagePattConfigDisp(unsigned char);
void CreatePagePattParamDisp(unsigned char );

void Enter_Key(void);
void Select_Key(void);
void Config_Fun(void);
void ED_Config(void);
void ED_PTP_Fun(void);
void ED_PTS_Fun(void);
void ED_CTP_Fun(void);
void ED_CTS_Fun(void);
void ED_Scroll_Fun(void);
void ED_NI_Fun(void);
void ED_Baud_Fun(void);
void Test_Fun(void);
void Key_Read(void);
void delay(unsigned int);
void Blink_Config(void);
void READ_EEPROM(void);
void WRITE_EEPROM(void);
void  FreQfeaD(void);
void RUN_FUN(void);
void Read_ADC(void);
void Para_Store(void);
void slide(void);
void err(float,float,float *);
void Meter_Calib(void);
void READ_CONFG(void);
void WRITE_CONFG(void);
void Disp_FUN(void);
void Delay(__IO uint32_t );
void FnWriteEpromWord(uint32_t ,float);
void FnEraseEpromWord(uint32_t ,unsigned char);
float FnReadEprom(uint32_t );
void FnWriteEpromByte(uint32_t ,unsigned char);
unsigned char FnReadEpromByte(uint32_t);
void fnStoredDataLoggParam(void);
void Factor_Update(void);
void ParaFactUp(void);
//MODBUS 
void MB_PARA(void);
void TIM6_Configuration(void);





