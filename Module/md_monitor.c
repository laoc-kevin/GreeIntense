#include "md_monitor.h"
#include "md_event.h"

#define MONITOR_DATA_MAX_NUM        50     //最大可监控点位数，根据实际情况调整
#define MONITOR_POLL_INTERVAL_MS    10     //

sMonitorInfo* MonitorList = NULL;
sMonitorInfo  MonitorBuf[MONITOR_DATA_MAX_NUM];

uint16_t MonitorID = 0;

/**************************************************************
*@brief 数据监控注册
***************************************************************/
void vMonitorRegist(void* pvVal, OS_SEM* psSem)
{
    sMonitorInfo* psMonitorInfo = NULL;
    
    if(MonitorID >= MONITOR_DATA_MAX_NUM)
    {
        return ;
    }
    psMonitorInfo = &MonitorBuf[MonitorID];
    MonitorID++;
    
    psMonitorInfo->pvVal    = pvVal;
    psMonitorInfo->psSem    = psSem;
    psMonitorInfo->usDataId = MonitorID + 1;   //全局标示
    psMonitorInfo->sDataBuf = *(int32_t*)pvVal;
    psMonitorInfo->pNext    = NULL;
 
    if(MonitorList == NULL)
    {
        MonitorList = psMonitorInfo;
    }
    else
    {
        MonitorList->pLast->pNext = psMonitorInfo;
    }
    MonitorList->pLast = psMonitorInfo;
}

/**************************************************************
*@brief 数据监控轮询
***************************************************************/
void vMonitorPollTask(void *p_arg)
{
    CPU_TS  ts = 0;
    OS_ERR err = OS_ERR_NONE;
    
    sMsg*            psMsg  = NULL;
    sMonitorInfo* psMonitor = NULL;
    
    OS_TCB* psEventTCB = psGetEventTCB();
    
    while(DEF_TRUE)
	{
        (void)OSTimeDlyHMSM(0, 0, 0, MONITOR_POLL_INTERVAL_MS, OS_OPT_TIME_HMSM_STRICT, &err);	
        for(psMonitor = MonitorList; psMonitor != NULL; psMonitor = psMonitor->pNext)
        {
            if(psMonitor->sDataBuf != *(int32_t*)psMonitor->pvVal)
            {
                psMsg->psSem = psMonitor->psSem;
                psMsg->pvArg = psMonitor->pvVal;
                
                (void)OSTaskQPost(psEventTCB, (void*)psMsg, sizeof(sMsg), OS_OPT_POST_ALL, &err);  //转发到特定的Task
                psMonitor->sDataBuf = *(int32_t*)psMonitor->pvVal;                
            }
        }
    }
}

void vMonitorInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
    (void)eTaskCreate(p_tcb, vMonitorPollTask, NULL, prio, p_stk_base, stk_size);
}
