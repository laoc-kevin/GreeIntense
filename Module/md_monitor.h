#ifndef _MD_MONITOR_H_
#define _MD_MONITOR_H_

#include "includes.h"

#define MONITOR(pvVal, psSem) \
       vMonitorRegist((void*)pvVal, (OS_SEM*)psSem);

typedef struct sMonitorInfo /*设备模拟量接口类型*/
{
    void*      pvVal;    
    OS_SEM*    psSem;
               
    int32_t    sDataBuf;     
    uint16_t   usDataId;    //全局标示
    
    struct sMonitorInfo*  pNext;
    struct sMonitorInfo*  pLast;    
}sMonitorInfo;

void vMonitorRegist(void* pvVal, OS_SEM* psSem);
 
void vMonitorInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size);
#endif