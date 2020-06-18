#ifndef _MD_OUTPUT_H
#define _MD_OUTPUT_H

#include "includes.h"
#include "md_io.h"

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
}eAOType;		//输出量程

typedef struct        
{
    int32_t      lMax;            //量程最大值 = 实际量程最大值*10
	int32_t      lMin;            //量程最小值 = 实际量程最小值*10
	int16_t      s10_uA;          //输出电流值  单位10uA电流值       
    int32_t      lAOVal;          //根据量程转化为实际输出值      
}sAOData;  

typedef struct        
{
	eCtrlEn      eDOVal;         //当前实际值       
}sDOData; 

void vOutputInit(void);
void vAnalogOutputRegist(uint8_t ucChannel, int32_t lMin, int32_t lMax);

void vDigitalOutputCtrl( uint8_t ucChannel, eCtrlEn eCtrl);
void vDigitalOutputDataToggle(uint8_t ucChannel);

void vAnalogOutputSetRealVal(uint8_t ucChannel, uint32_t ulRealData);
void vAnalogOutputSetRange(uint8_t ucChannel, int32_t lMin, int32_t lMax);

void vOutputInit(void);
#endif
