#include "os.h"
#include "md_event.h"

static sEvent* SysEventList = NULL;

void vEventRegist(OS_SEM* psSem, OS_TCB* psTCB)
{
    sEvent* psEvent = calloc(1, sizeof(sEvent));
    if(psEvent != NULL)
    {
        psEvent->psSem      = psSem;
        psEvent->psTCB      = psTCB;
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
    CPU_TS            ts = 0;
    OS_MSG_SIZE  msgSize = 0;
    OS_ERR           err = OS_ERR_NONE;
    
    sEvent* psEvent = NULL;
    while(DEF_TRUE)
	{
        sMsg* psMsg = (sMsg*)OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, &ts, &err);
        
        for(psEvent = SysEventList; psEvent != NULL; psEvent = psEvent->pNext) //轮询所有注册的事件
        {
            if( psMsg->psSem == psEvent->psSem ) //判断是否为同一消息
            {
                (void)OSTaskQPost(psEvent->psTCB, (void*)psMsg, sizeof(sMsg), OS_OPT_POST_ALL, &err);  //转发到特定的Task	
            }
        }
    }
}


