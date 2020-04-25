#include "md_event.h"

sEvent* SysEventList = NULL;

void vEventRegist(void* pvVal, OS_TCB* psTCB, uint8_t ucEventNum)
{
    sEvent* psEvent = calloc(1, sizeof(sEvent));
    
    if(psEvent != NULL)
    {
        psEvent->pvVal      = pvVal;
        psEvent->psTCB      = psTCB;
        psEvent->ucEventNum = ucEventNum;
        psEvent->pNext      = NULL;
    }
    if(SysEventList == NULL)
    {
        SysEventList = psEvent;
    }
    else
    {
        SysEventList->pLast->pNext = psEvent;
    }
    SysEventList->pLast = psEvent;
}

void vEventPollTask(void)
{
    OS_ERR      err = OS_ERR_NONE;
    sEvent* psEvent = NULL;
    sMsg*   psMsg   = NULL;
    
    for(psEvent = SysEventList; psEvent != NULL; psEvent = psEvent->pNext)
    {
        if( *(int32_t*)(psEvent->pvVal) != psEvent->lValBuf )
        {
            psMsg->pvVal      = psEvent->pvVal;
            psMsg->ucEventNum = psEvent->ucEventNum;
            
            (void)OSTaskQPost(psEvent->psTCB, (void*)psMsg, sizeof(sMsg), OS_OPT_POST_ALL, &err);	
        }
    }
}
