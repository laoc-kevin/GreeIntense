#ifndef _APP_INPUT_H_
#define _APP_INPUT_H_
#include "includes.h"

#define GET_QUICK_TEST		(GPIO_ReadValue(1)>>25 & 1)

extern uint8_t ControllerID;               /* 控制器ID 通过拨码设置*/

typedef struct        /** Struct for creating entries in the communictaion profile */
{
    int16_t         Max;
	int16_t         Min;
	void*        InputValue;     //实际值       
}sAIData; 

void AppDigitalInputConfig( uint8_t channel, uint8_t* value );
void AppAnalogInputConfig( uint8_t channel, int16_t min, int16_t max, void* value );

int16_t AppInputConvertToReal(int16_t min, int16_t max, int16_t uA);

int16_t AppGetAIuA(uint8_t channel);
int16_t AppGetAIRealValue(uint8_t channel, int16_t ucMin, int16_t ucMax);

uint8_t AppGetDIBit(uint8_t DACNum, uint8_t bit);
uint8_t AppGetDIRealValue(uint8_t channel);

uint8_t AppGetSaInput(void);

void AppSetAIRange(uint8_t channel, int16_t ucMin, int16_t ucMax);
void AppInputReceiveTask(void *p_arg);

#endif
