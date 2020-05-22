#ifndef _SYSTEM_CTRL_H_
#define _SYSTEM_CTRL_H_

#include "system.h"

/*********************系统*************************/
void vSystem_SetAlarm(System* pt);
void vSystem_DelAlarmRequst(System* pt);
void vSystem_RegistAlarmIO(System* pt, uint8_t ucSwitch_DO);

void vSystem_ChangeSystemMode(System* pt, eSystemMode eSystemMode);

void vSystem_SetTemp(System* pt, int16_t sTempSet);
void vSystem_SetFreAir(System* pt, uint16_t usFreAirSet_Vol);
void vSystem_SetHumidity(System* pt, uint16_t usHumidityMin, uint16_t usHumidityMax);

void vSystem_SetCO2PPM(System* pt, uint16_t usCO2PPMSet);
void vSystem_SetCO2AdjustDeviat(System* pt, uint16_t usCO2AdjustDeviat);

/*********************主机*************************/
void vSystem_OpenUnits(System* pt);
void vSystem_CloseUnits(System* pt);

void vSystem_ChangeRunningMode(System* pt);
void vSystem_SetRunningMode(System* pt, eRunningMode eRunMode);

void vSystem_SupAirTemp(System* pt);
void vSystem_UnitErr(System* pt);
void vSystem_FreAir(System* pt);

/*********************排风风机*************************/
void vSystem_CloseExAirFans(System* pt);
void vSystem_SetExAirFanFreqRange(System* pt, uint16_t usMinFreq, uint16_t usMaxFreq);
void vSystem_ExAirSet_Vol(System* pt);
void vSystem_AdjustExAirFanFreq(void* p_tmr, void* p_arg);

/*********************传感器*************************/
void vSystem_CO2SensorErr(System* pt);
void vSystem_CO2PPM(System* pt);

void vSystem_TempHumiOut(System* pt);
void vSystem_TempHumiOutErr(System* pt);

void vSystem_TempHumiIn(System* pt);
void vSystem_TempHumiInErr(System* pt);
#endif