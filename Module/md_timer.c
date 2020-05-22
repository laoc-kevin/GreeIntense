#include "md_timer.h"

//#define MAX_TIMER_NUM  10

//OS_TMR TimerPool[MAX_TIMER_NUM];

//uint8_t TimerIndex = 0;

BOOL xTimerRegist(OS_TMR *p_tmr, uint16_t usDlyTime_s, uint16_t usPeriod_s, OS_OPT opt, 
                  OS_TMR_CALLBACK_PTR p_callback, void *p_callback_arg)
{
    OS_ERR err     = OS_ERR_NONE;
    
    OS_TICK dly    = usDlyTime_s * TMR_TICK_PER_SECOND;
    OS_TICK period = usPeriod_s * TMR_TICK_PER_SECOND;
    
    OS_STATE  sTmrState = OSTmrStateGet(p_tmr, &err);
    
    if(sTmrState == OS_TMR_STATE_UNUSED)
    {
        OSTmrCreate(p_tmr, "Tmr", dly, period, opt, p_callback, (void*)p_callback_arg, &err);
    }
    if( (sTmrState == OS_TMR_STATE_STOPPED) || (sTmrState == OS_TMR_STATE_COMPLETED) )
    {
        OSTmrStart(p_tmr, &err);
    }
    return err == OS_ERR_NONE;
}

uint16_t usGetTmrElapsedTime(OS_TMR *p_tmr)
{
    OS_ERR err     = OS_ERR_NONE;
    return  OSTmrRemainGet(p_tmr, &err) / TMR_TICK_PER_SECOND;
}