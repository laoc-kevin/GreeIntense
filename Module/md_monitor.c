#include "md_monitor.h"
#include "md_event.h"
#include "my_rtt_printf.h"

#include "system.h"

#define MONITOR_DATA_MAX_NUM        200     //最大可监控点位数，根据实际情况调整
#define MONITOR_POLL_INTERVAL_S     1     //

sMonitorInfo* MonitorList = NULL;
sMonitorInfo  MonitorBuf[MONITOR_DATA_MAX_NUM];

uint16_t MonitorID = 0;

/**************************************************************
*@brief 数据监控注册
***************************************************************/
void vMonitorRegist(void* pvVal, uint8_t ucDataType, OS_SEM* psSem)
{
    uint8_t  ucValue;
    uint16_t usValue;
    
    int8_t   cValue;
    int16_t  sValue;
    
    sMonitorInfo* psMonitor = NULL; 
    if(MonitorID >= MONITOR_DATA_MAX_NUM)
    {
        myprintf("vMonitorRegist MonitorID %d  \n", MonitorID);  
        return ;
    }
    psMonitor = &MonitorBuf[MonitorID];
    MonitorID++;
    
    psMonitor->pvVal      = pvVal;
    psMonitor->psSem      = psSem;
    psMonitor->ucDataType = ucDataType;
    psMonitor->usDataId   = MonitorID + 1;   //全局标示
    psMonitor->pNext      = NULL;
    
    if(ucDataType == uint8)
    {
        ucValue = *(uint8_t*)pvVal;
        psMonitor->usDataVal = (uint16_t)ucValue;
    }
    else if(ucDataType == uint16)
    {
        usValue = *(uint16_t*)pvVal;
        psMonitor->usDataVal = (uint16_t)usValue;
    }
    else if(ucDataType == int8)
    {
        cValue = *(int8_t*)pvVal;
        psMonitor->usDataVal = (uint16_t)cValue;
    }
    else if(ucDataType == int16)
    {
        sValue = *(int16_t*)pvVal;
        psMonitor->usDataVal = (uint16_t)sValue;
    }
    if(MonitorList == NULL)
    {
        MonitorList = psMonitor;
    }
    else
    {
        MonitorList->pLast->pNext = psMonitor;
    }
//    myprintf("vMonitorRegist %d \n", psMonitor->usDataVal); 
    MonitorList->pLast = psMonitor;
}

/**************************************************************
*@brief 数据监控轮询
***************************************************************/
void vMonitorPollTask(void *p_arg)
{
    sEventMsg sEventMsg;
    
    uint8_t  ucValue;
    int8_t   cValue;
    uint16_t usValue;
    int16_t  sValue;

    uint16_t usDataValue = 0;
    
    CPU_TS  ts = 0;
    OS_ERR err = OS_ERR_NONE;
    sMonitorInfo* psMonitor = NULL;
    
    OS_TCB* psEventTCB = psGetEventTCB();
    
     System* psSystem = System_Core();
    
    
    while(DEF_TRUE)
	{
        (void)OSTimeDlyHMSM(0, 0, MONITOR_POLL_INTERVAL_S, 0, OS_OPT_TIME_HMSM_STRICT, &err);
        for(psMonitor = MonitorList; psMonitor != NULL; psMonitor = psMonitor->pNext)
        {
//            usDataValue = psMonitor->usDataVal;      
            if(psMonitor->ucDataType == uint8)
            {
                ucValue = *(uint8_t*)psMonitor->pvVal;
                usDataValue = (uint16_t)ucValue;
            }
            else if(psMonitor->ucDataType == uint16)
            {
                usValue = *(uint16_t*)psMonitor->pvVal;
                usDataValue = (uint16_t)usValue;                
            }
            else if(psMonitor->ucDataType == int8)
            {
                cValue = *(int16_t*)psMonitor->pvVal;
                usDataValue = (uint16_t)cValue;
            }
            else if(psMonitor->ucDataType == int16)
            {
                sValue = *(int16_t*)psMonitor->pvVal;
                usDataValue = (uint16_t)sValue;
            }
            
            if(psMonitor->usDataVal != usDataValue)
            {
                sEventMsg.psSem = psMonitor->psSem;
                sEventMsg.pvArg = psMonitor->pvVal;
                psMonitor->usDataVal = usDataValue;     
                
//                if(&psSystem->psTempHumiSenInList[11]->sAvgTemp == psMonitor->pvVal)
//                {
//                    myprintf("vMonitorPollTask  sEventMsg.pvArg %d  sAvgTemp %d\n", sEventMsg.pvArg, &psSystem->psTempHumiSenInList[11]->sAvgTemp);  
//                }
//                (void)OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);
                (void)OSTaskQPost(psEventTCB, (void*)&sEventMsg, sizeof(sEventMsg), OS_OPT_POST_FIFO, &err);  //转发到特定的Task            
                              
            }
        }
    }
}

/**************************************************************
*@brief 数据监控初始化
***************************************************************/
void vMonitorInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
    (void)eTaskCreate(p_tcb, vMonitorPollTask, NULL, prio, p_stk_base, stk_size);
}
