#include "os.h"
#include "md_event.h"

#define EVENT_MAX_NUM        50     //最大可监控点位数，根据实际情况调整


static sEvent* EventList = NULL;
static sEvent  EventBuf[EVENT_MAX_NUM];

uint16_t EventID = 0;

static OS_TCB EventTCB;

/**************************************************************
*@brief 事件注册
***************************************************************/
void vEventRegist(OS_SEM* psSem, OS_TCB* psTCB)
{
    sEvent* psEvent = NULL;   
    if(EventID >= EVENT_MAX_NUM)
    {
        return;
    }
    psEvent = &EventBuf[EventID];
    EventID++;
     
    psEvent->psSem      = psSem;
    psEvent->psTCB      = psTCB;
    psEvent->pNext      = NULL;
 
    if(EventList == NULL)
    {
        EventList = psEvent;
    }
    else
    {
        EventList->pLast->pNext = psEvent;
    }
    EventList->pLast = psEvent;
}

/**************************************************************
*@brief 事件轮询
***************************************************************/
void vEventPollTask(void * p_arg)
{
    CPU_TS            ts = 0;
    OS_MSG_SIZE  msgSize = 0;
    OS_ERR           err = OS_ERR_NONE;
    
    sEvent* psEvent = NULL;
    while(DEF_TRUE)
	{
        sMsg* psMsg = (sMsg*)OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, &ts, &err);    
        for(psEvent = EventList; psEvent != NULL; psEvent = psEvent->pNext) //轮询所有注册的事件
        {
            if(psMsg->psSem == psEvent->psSem)    //判断是否为同一消息
            {
                (void)OSTaskQPost(psEvent->psTCB, (void*)psMsg, sizeof(sMsg), OS_OPT_POST_ALL, &err);  //转发到特定的Task	
            }
        }
    }
}

/**************************************************************
*@brief 事件消息泵初始化
***************************************************************/
void vEventInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
    (void)eTaskCreate(p_tcb, vEventPollTask, NULL, prio, p_stk_base, stk_size);
}

OS_ERR eTaskCreate(OS_TCB *p_tcb, OS_TASK_PTR p_task, void *p_arg, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
    OS_ERR err = OS_ERR_NONE;
    
    OSTaskCreate( p_tcb, NULL, p_task, p_arg, prio, p_stk_base, stk_size/10u, stk_size, 0u, 0u, 0u,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &err);
    return err;
}

OS_TCB* psGetEventTCB(void)
{
    return &EventTCB;
}


