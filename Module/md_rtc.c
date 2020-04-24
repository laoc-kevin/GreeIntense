#include "lpc_rtc.h"
#include "md_rtc.h"

RTC_TIME_Type CurrTime;

/*RTC初始化*/
void vRTCInit(void)
{
	// Init RTC module
	RTC_Init(LPC_RTC);
	
	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);
	//RTC_CalibCounterCmd(LPC_RTC, DISABLE);
	
	/* Set the CIIR for second counter interrupt*/
	RTC_CntIncrIntConfig(LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);
	//NVIC_EnableIRQ(RTC_IRQn);
	BSP_IntEn(BSP_INT_ID_RTC);
	
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, 0);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, 0);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, 11);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_YEAR, 2017);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 1);
}

/* 设置RTC时间 */
void vRTCSetTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec, uint8_t dow)
{
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_YEAR, year);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, month);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, day);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, hour);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, min);
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, sec);
	/* 触摸屏系统时钟周日为7，LPC4078 RTC时钟周日值为0，需要转化 */
	if (dow != 7)
	{
		RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFWEEK, dow);
	}
	else
	{
		RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFWEEK, 0);
	}
}

/* 获取RTC当前时间 */
void vRTCGetTime(void)
{
	RTC_GetFullTime(LPC_RTC, &CurrTime);
}

/* RTC中断 */
void RTC_IRQHandler(void);
void RTC_IRQHandler(void)
{
	if (RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE))
	{
		RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
	}
}