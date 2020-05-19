#ifndef _MD_TIMER_H_
#define _MD_TIMER_H_

#include "includes.h"
#include "LPC407x_8x_177x_8x.h"
#include "lpc_timer.h"
#include "lpc_types.h"

#define TIMER_HANDLE uint16_t

typedef enum   /*定时器状态*/
{
    TIMER_FREE   =  0,
    TIMER_ARMED  =  1,
}eTimerState;

typedef enum   /*系统状态*/
{
    TIMER_ONE_SHOT = 0,
    TIMER_PERIODIC = 1,
}eTimerType;

typedef void (*pTimerCallback_t)(void* p_arg);


typedef struct {
    
	eTimerState       eTimerState;
    eTimerType        eTimerType;
 
    uint16_t          usTrigTime;
    uint16_t          usRemainTime;
    void*             p_arg;
    
    pTimerCallback_t  pTimerCallback;
    
}sTimerEntity;

TIMER_HANDLE sTimerRegist(eTimerType eTimerType, uint16_t usTrigTime, pTimerCallback_t callback, void* p_arg);

void vTimerRemove(TIMER_HANDLE usTimerIndex);

int16_t sTimerGetElapsedTime(TIMER_HANDLE usTimerIndex);

#endif