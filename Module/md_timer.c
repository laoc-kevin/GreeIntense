#include "md_timer.h"

#define MD_TIMER_USED 			(LPC_TIM2)

#define MAX_TIMER_NUM   50

#define TIMER_NONE -1

sTimerEntity Timers[MAX_TIMER_NUM] = {{TIMER_FREE, NULL, 0, 0},};

void TIM2_Configuration(void)
{
	TIM_TIMERCFG_Type TIM_ConfigStruct = {TIM_PRESCALE_USVAL, {0,0,0}, 500};
	TIM_MATCHCFG_Type TIM_MatchConfigStruct = {0, TRUE, TRUE, FALSE, TIM_EXTMATCH_NOTHING, {0,0,0}, 1};

	TIM_Init(MD_TIMER_USED, TIM_TIMER_MODE, &TIM_ConfigStruct);
	TIM_ConfigMatch(MD_TIMER_USED, &TIM_MatchConfigStruct);

//	/* preemption = 1, sub-priority = 1 */
//	NVIC_SetPriority(CNAOPEN_TIMER_INTR_USED, ((0x01<<3)|0x01));

	/* Enable interrupt for timer 2 */
	//NVIC_EnableIRQ(CNAOPEN_TIMER_INTR_USED);
	BSP_IntEn(BSP_INT_ID_TIMER2);

	// To start timer
	TIM_Cmd(MD_TIMER_USED, ENABLE);
}

void vTimerInit(void)
{
	TIM2_Configuration();
}

void vTimerDispatch(void)
{
    TIMER_HANDLE  usTimerIndex = 0;
    sTimerEntity* psTimer      = NULL;
    
    for(usTimerIndex=0; usTimerIndex < MAX_TIMER_NUM; usTimerIndex++)
    {
        psTimer = &Timers[usTimerIndex];
        psTimer->usRemainTime--;
        
        if(psTimer->usRemainTime == 0)
        {
            if(psTimer->pTimerCallback != NULL)
            {
                psTimer->pTimerCallback(psTimer->p_arg);
            }
            
            if(psTimer->eTimerType == TIMER_PERIODIC)
            {
                psTimer->usRemainTime = psTimer->usTrigTime;
            }
            else if(psTimer->eTimerType == TIMER_ONE_SHOT)
            {
                psTimer->eTimerState  = TIMER_FREE;
            } 
        }
        
    }
}

TIMER_HANDLE sTimerRegist(eTimerType eTimerType, uint16_t usTrigTime, pTimerCallback_t callback, void* p_arg)
{
    TIMER_HANDLE  usTimerIndex = 0;
    sTimerEntity* psTimer      = NULL;
    
    for(usTimerIndex=0; usTimerIndex < MAX_TIMER_NUM; usTimerIndex++)
    {
        psTimer = &Timers[usTimerIndex];
        if( (callback != NULL) && (psTimer->eTimerState == TIMER_FREE) )
        {
            psTimer->eTimerState  = TIMER_ARMED;
            psTimer->eTimerType   = eTimerType;
            psTimer->usTrigTime   = usTrigTime;
            psTimer->usRemainTime = usTrigTime;
            psTimer->pTimerCallback = callback;
            
            return usTimerIndex;
        }
    }
    return TIMER_NONE; 
}

void vTimerRemove(TIMER_HANDLE usTimerIndex)
{
    sTimerEntity* psTimer = &Timers[usTimerIndex];;
    psTimer->eTimerState = TIMER_FREE;
}


void TIMER3_IRQHandler(void)
{
	if(TIM_GetIntStatus(MD_TIMER_USED, TIM_MR0_INT) != RESET)
	{
		TIM_ClearIntPending(MD_TIMER_USED, TIM_MR0_INT);
		vTimerDispatch();
	}
}
