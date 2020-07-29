#ifndef _SYSTEM_CTRL_H_
#define _SYSTEM_CTRL_H_

#include "system.h"

/*********************系统*************************/
void vSystem_SetAlarm(System* pt);
void vSystem_DelAlarmRequst(System* pt);
void vSystem_AlarmEnable(System* pt, BOOL xAlarmEnable);
void vSystem_CleanAlarm(System* pt, BOOL* pxAlarmClean);
void vSystem_RegistAlarmIO(System* pt, uint8_t ucSwitch_DO);

void vSystem_DeviceRunningState(System* pt);
void vSystem_ChangeSystemMode(System* pt, eSystemMode eSystemMode);

void vSystem_SetTemp(System* pt, uint16_t usTempSet);
void vSystem_SetFreAir(System* pt, uint16_t usFreAirSet_Vol_H, uint16_t usFreAirSet_Vol_L);
void vSystem_SetHumidity(System* pt, uint16_t usHumidityMin, uint16_t usHumidityMax);

void vSystem_SetCO2AdjustThr_V(System* pt, uint16_t usCO2PPMSet);
void vSystem_SetCO2AdjustDeviat(System* pt, uint16_t usCO2AdjustDeviat);

void vSystem_SetExAirFanRated(System* pt, uint16_t usExAirFanRated_Vol_H, uint16_t usExAirFanRated_Vol_L);  

/*********************主机*************************/
void vSystem_OpenUnits(System* pt);
void vSystem_CloseUnits(System* pt);

void vSystem_AdjustUnitRunningMode(System* pt);
void vSystem_ChangeUnitRunningMode(System* pt);
void vSystem_SetUnitRunningMode(System* pt, eRunningMode eRunMode);

void vSystem_UnitSupAirTemp(System* pt, ModularRoof* pModularRoof);
void vSystem_UnitErr(System* pt);

void vSystem_UnitTempHumiOut(System* pt);
void vSystem_UnitTempHumiIn(System* pt);

void vSystem_UnitCO2PPM(System* pt);
void vSystem_UnitFreAir(System* pt);

/*********************排风风机*************************/
void vSystem_CloseExAirFans(System* pt);

void vSystem_SetExAirFanCtrlPeriod(System* pt, uint16_t usExAirFanCtrlPeriod);
void vSystem_SetExAirFanFreqRange(System* pt, uint16_t usMinFreq, uint16_t usMaxFreq);
void vSystem_ExAirSet_Vol(System* pt);
void vSystem_ExAirRatio(System* pt, uint8_t ucExAirCoolRatio, uint8_t ucExAirHeatRatio);

void vSystem_ExAirRequest_Vol(System* pt);
void vSystem_AdjustExAirFanFreq(System* pt, uint16_t usFreq);
void vSystem_ExAirFanCtrl(System* pt);
void vSystem_ExAirFanRemoteChange(System* pt);
    
void vSystem_ExAirFanErr(System* pt);
void vSystem_ExAirFanErrClean(System* pt);
/*********************传感器*************************/
void vSystem_CO2SensorErr(System* pt);
void vSystem_CO2PPM(System* pt);

void vSystem_TempHumiOut(System* pt);
void vSystem_TempHumiOutErr(System* pt);

void vSystem_TempHumiIn(System* pt);
void vSystem_TempHumiInErr(System* pt);
#endif