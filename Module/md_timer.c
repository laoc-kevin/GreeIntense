#include "md_timer.h"

/**************************************************************
*@brief 定时器注册
***************************************************************/
BOOL xTimerRegist(OS_TMR *p_tmr, uint16_t usDlyTime_s, uint32_t ulPeriod_s, OS_OPT opt, 
                  OS_TMR_CALLBACK_PTR p_callback, void *p_callback_arg, uint8_t xExcuteCallback)
{
    OS_ERR err     = OS_ERR_NONE;
    
    OS_TICK dly    = usDlyTime_s * TMR_TICK_PER_SECOND;
    OS_TICK period = ulPeriod_s * TMR_TICK_PER_SECOND;
    
    OS_STATE  sTmrState = OSTmrStateGet(p_tmr, &err);
    if(sTmrState == OS_TMR_STATE_UNUSED)
    {
        OSTmrCreate(p_tmr, "Tmr", dly, period, opt, p_callback, (void*)p_callback_arg, &err);
    }
    else
    {
        if(sTmrState == OS_TMR_STATE_RUNNING)
        {
            OSTmrStop(p_tmr, OS_OPT_TMR_NONE, NULL, &err);
        }
        p_tmr->Dly = dly;
        p_tmr->Period = period;
        p_tmr->Opt = opt;
        p_tmr->CallbackPtr = p_callback;
        p_tmr->CallbackPtrArg = p_callback_arg; 
    }
    if(err == OS_ERR_NONE)
    {
        if(xExcuteCallback && p_callback != NULL)  //是否首先执行回调函数
        {
            p_callback(p_tmr, p_callback_arg);
        }
        OSTmrStart(p_tmr, &err);
    }
    return err == OS_ERR_NONE;
}

/**************************************************************
*@brief 定时器剩余计时
***************************************************************/
uint16_t usGetTmrElapsedTime(OS_TMR *p_tmr)
{
    OS_ERR err     = OS_ERR_NONE;
    return  OSTmrRemainGet(p_tmr, &err) / TMR_TICK_PER_SECOND;
}

/**************************************************************
*@brief 定时器状态
***************************************************************/
OS_STATE usGetTmrState(OS_TMR *p_tmr)
{
    OS_ERR err     = OS_ERR_NONE;

    OS_STATE  sTmrState = OSTmrStateGet(p_tmr, &err);
    return  sTmrState; 
}
