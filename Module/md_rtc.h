#ifndef APP_RTC_H
#define APP_RTC_H
#include "includes.h"
#include "lpc_rtc.h"

extern RTC_TIME_Type CurrTime;

void vRTCInit(void);
void vRTCSetTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec, uint8_t dow);
void vRTCGetTime(void);

#endif
