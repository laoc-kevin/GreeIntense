
#include "lpc_wwdt.h"
#include "md_watchdog.h"
#include "md_led.h"
#include "md_event.h"

#define WDT_TIMEOUT 	10000000   //10s


/**
*@brief 喂狗任务，如果超时则复位芯片
*		每秒钟喂一次，超时5秒则复位
*		超时设定WDT_TIMEOUT
*/
void vWatchDogFeedTask(void * p_arg)
{
	OS_ERR err = OS_ERR_NONE;
    
	while(DEF_TRUE)
	{
		vLedOn(&LedMCU);
		OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
        
		vLedOff(&LedMCU);
	    OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
		WWDT_Feed();
	}
}

void vWatchDogInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
	(void)WWDT_Init(WDT_TIMEOUT);
	WWDT_Enable(ENABLE);
	WWDT_SetMode(WWDT_RESET_MODE, ENABLE);
	(void)WWDT_Start(WDT_TIMEOUT);
    
    (void)eTaskCreate(p_tcb, vWatchDogFeedTask, NULL, prio, p_stk_base, stk_size);
}

