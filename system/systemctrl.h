#ifndef _SYSTEM_CTRL_H_
#define _SYSTEM_CTRL_H_

#include "system.h"

/*********************系统*************************/
void vSystem_SetAlarm(System* pt);
void vSystem_DelAlarmRequst(System* pt);
void vSystem_RegistAlarmIO(System* pt, uint8_t ucSwitch_DO);

void vSystem_ChangeSystemMode(System* pt, eSystemMode eSystemMode);

void vSystem_SetTemp(System* pt, int16_t sTargetTemp);
void vSystem_SetFreAir(System* pt, uint16_t usFreAirSet_Vol);
void vSystem_SetHumidity(System* pt, uint16_t usHumidityMin, uint16_t usHumidityMax);

void vSystem_SetCO2PPM(System* pt, uint16_t usCO2PPMSet);
void vSystem_SetCO2AdjustDeviat(System* pt, uint16_t usCO2AdjustDeviat);


    
/*********************主机*************************/
void vSystem_OpenUnits(System* pt);
void vSystem_CloseUnits(System* pt);

void vSystem_ChangeRunningMode(System* pt);
void vSystem_SetRunningMode(System* pt, eRunningMode eRunMode);



/*********************排风风机*************************/
void vSystem_CloseExAirFans(System* pt);


#endif