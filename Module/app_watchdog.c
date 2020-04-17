
#include "lpc_wwdt.h"
#include "app_watchdog.h"
#include "app_led.h"

#define WDT_TIMEOUT 	10000000

void AppWatchDogInit(void)
{
	(void)WWDT_Init(WDT_TIMEOUT);
	WWDT_Enable(ENABLE);
	WWDT_SetMode(WWDT_RESET_MODE, ENABLE);
	(void)WWDT_Start(WDT_TIMEOUT);
}
/**
*@brief 喂狗任务，如果超时则复位芯片
*		每秒钟喂一次，超时5秒则复位
*		超时设定WDT_TIMEOUT
*/
void AppWatchDogFeedTask(void * p_arg)
{
	OS_ERR err = OS_ERR_NONE;

#if WATCHDOG_ENABLE
	AppWatchDogInit();
#endif	
	
	while(DEF_TRUE)
	{
		AppLedOn( &LedMCU );
		OSTimeDlyHMSM( 0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err );
		AppLedOff( &LedMCU );
	    OSTimeDlyHMSM( 0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err );
		WWDT_Feed();
	}
}

