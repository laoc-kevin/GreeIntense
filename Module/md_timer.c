#include "md_timer.h"
#include "my_rtt_printf.h"
/**************************************************************
*@brief 定时器注册
***************************************************************/
BOOL xTimerRegist(OS_TMR *p_tmr, uint32_t ulDlyTime_s, uint32_t ulPeriod_s, OS_OPT opt, 
                  OS_TMR_CALLBACK_PTR p_callback, void *p_callback_arg, uint8_t xExcuteCallback)
{
    OS_ERR err     = OS_ERR_NONE;
    OS_TICK dly    = ulDlyTime_s * TMR_TICK_PER_SECOND;
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
            OSTmrDel(p_tmr, &err);
            OSTmrCreate(p_tmr, "Tmr", dly, period, opt, p_callback, (void*)p_callback_arg, &err);
        }
    }
    if(err == OS_ERR_NONE)
    {
        if(xExcuteCallback && p_callback != NULL)  //是否首先执行回调函数
        {
            p_callback(p_tmr, p_callback_arg);
        }
        OSTmrStart(p_tmr, &err);
    }
//    myprintf("xTimerRegist usDlyTime_s %d  ulPeriod_s %d\n", usDlyTime_s, ulPeriod_s);
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
