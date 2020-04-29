#include "md_dataMonitor.h"

sDataInfo* SysDataList;

uint16_t usDataMonitor(void* pvVal, OS_TCB* psTCB)
{
    sDataInfo* psDataInfo = calloc(1, sizeof(sDataInfo));
    sDataInfo* psData = NULL;
    
    if(psDataInfo != NULL)
    {
        psDataInfo->pvVal      = pvVal;
        psDataInfo->psTCB      = psTCB;
        psDataInfo->pNext      = NULL;
    }
    if(SysDataList == NULL)
    {
        psDataInfo->usDataId = 1;   //全局标示
        SysDataList = psDataInfo;
    }
    else
    {
        for(psData = SysDataList; psData != NULL; psData = psData->pNext)
        {
            if(psData->pNext == NULL)
            {
                psDataInfo->usDataId = psData->usDataId++;
                psData->pNext = psDataInfo;
                break;
            }
        }
    }
    return psDataInfo->usDataId;
}

void vDataPollTask(void)
{
    CPU_TS  ts = 0;
    OS_ERR err = OS_ERR_NONE;
    
    while(DEF_TRUE)
	{
        sDataInfo* psData = NULL;
        for(psData = SysDataList; psData != NULL; psData = psData->pNext)
        {
            if(psData->sValBuf != *(int32_t*)psData->pvVal)
            {
                (void)OSTaskQPost(psData->psTCB, (void*)psData, sizeof(sDataInfo), OS_OPT_POST_ALL, &err);  //转发到特定的Task
                psData->sValBuf = *(int32_t*)psData->pvVal;                
            }
        }
    }
}

