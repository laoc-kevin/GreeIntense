#ifndef _MD_TIMER_H_
#define _MD_TIMER_H_

#include "includes.h"
#include "LPC407x_8x_177x_8x.h"
#include "lpc_timer.h"
#include "lpc_types.h"

#define TIMER_HANDLE uint16_t

BOOL xTimerRegist(OS_TMR *p_tmr, uint32_t ulDlyTime_s, uint32_t ulPeriod_s, OS_OPT opt, 
                  OS_TMR_CALLBACK_PTR p_callback, void *p_callback_arg, uint8_t xExcuteCallback);
                      
uint16_t usGetTmrElapsedTime(OS_TMR *p_tmr);
OS_STATE usGetTmrState(OS_TMR *p_tmr);

#endif