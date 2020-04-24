#ifndef _MD_INPUT_H_
#define _MD_INPUT_H_

#include "includes.h"

#define GET_QUICK_TEST		(GPIO_ReadValue(1)>>25 & 1)

typedef struct        /**AI数据结构**/
{
    int32_t         lMax;          //量程最大值 = 实际量程最大值*10
	int32_t         lMin;          //量程最小值 = 实际量程最小值*10
    int16_t         s10_uA;        //输入电流值  单位10uA
	void*           pvAIVal;       //根据量程转化为实际值       
}sAIData; 

typedef struct        /**DI数据结构**/
{
	void*           pvDIVal;       //当前实际值       
}sDIData; 

void     vDigitalInputRegister(uint8_t ucChannel, uint8_t* pvVal);
void     vAnalogInputRegister(uint8_t ucChannel, int32_t lMin, int32_t lMax, void* pvVal);
         
void     vAnalogInputSetRange(uint8_t ucChannel, int32_t lMin, int32_t lMax);

uint32_t ulAnalogInputGetRealVal(uint8_t ucChannel);
uint8_t  ucDigitalInputGetRealVal(uint8_t ucChannel);

uint8_t  ucGetControllerID(void);
uint8_t  ucGetSaInput(void);

void     vInputReceiveTask(void *p_arg);

#endif
