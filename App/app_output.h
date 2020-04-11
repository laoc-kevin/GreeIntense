#ifndef APP_OUTPUT_H
#define APP_OUTPUT_H
#include "includes.h"
#include "app_io.h"

#define DAC7760_REG_OUT      0x551000//
#define DAC7760_REG_CFG      0x570000 //VOUT and IOUT terminals are tied together.
#define DAC7760_REG_DATA     0x010000 //

#define	USE_SUPER_IO_1		1
#define	USE_SUPER_IO_2		1

#define AO_NUM 8
#define DO_NUM 13

#define SET_VALUE_DAC7760_CLR1			AppSetDAC7760IO(&DAC1CLR)
#define	CLR_VALUE_DAC7760_CLR1			AppClrDAC7760IO(&DAC1CLR)
#define	SET_VALUE_DAC7760_LATCH1		AppSetDAC7760IO(&DAC1LATCH)
#define	CLR_VALUE_DAC7760_LATCH1		AppClrDAC7760IO(&DAC1LATCH)

#define SET_VALUE_DAC7760_CLR2			AppSetDAC7760IO(&DAC2CLR)
#define	CLR_VALUE_DAC7760_CLR2			AppClrDAC7760IO(&DAC2CLR)
#define	SET_VALUE_DAC7760_LATCH2		AppSetDAC7760IO(&DAC2LATCH)
#define	CLR_VALUE_DAC7760_LATCH2		AppClrDAC7760IO(&DAC2LATCH)	

typedef enum
{
	Off = 0,
	On =1
}CtrlEn;

typedef enum
{
	Vout0To5 = 0,
	Vout0To10,
	VoutSub5To5,
	VoutSub10To10,
	OutDisable,
	Iout4To20,
	Iout0To20,
	Iout0To24 ,
}AOType;		//输出量程

typedef struct        
{
    int16_t        Max;
	int16_t        Min;
	void*        OutputValue;     //电流值       
}sAOData; 


extern uint8_t *pDigitalOutput[DO_NUM];
extern sAOData pAnalogOutputData[AO_NUM]; 
		
void AppDAC7760Init(void);
void AppDigitalOutputInit(void);
void AppPWM1Init(void);

void AppDigitalOutputConfig( uint8_t channel, uint8_t* value );
void AppAnalogOutputConfig( uint8_t channel, int16_t min, int16_t max, void* value );

void AppDigitalOutputCtrl( uint8_t Channel, CtrlEn eCtrl);

void AppSetAORealValue(uint8_t channel, int16_t min, int16_t max, int16_t realData);
void AppGetAORealValue(uint8_t channel, int16_t min, int16_t max, int16_t realData);
void AppSetAORange(uint8_t channel, int16_t ucMin, int16_t ucMax);

void AppSetDAC7760OutputuA(uint8_t ucSuperIO, uint16_t uA);
void AppSetPWMRealValue(uint8_t pwmChannel, int16_t min, int16_t max, int16_t realData);
void AppSetDAC7760RealValue(uint8_t ucSuperIO, int16_t min, int16_t max, int16_t realData);

void AppDigitalOutputToggle( uint8_t Channel );
void AppOutputSetTask(void *p_arg);
#endif
