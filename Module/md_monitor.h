#ifndef _MD_MONITOR_H_
#define _MD_MONITOR_H_

#include "includes.h"

#define MONITOR(pvVal, ucDataType, psSem) \
       vMonitorRegist((void*)pvVal, ucDataType, (OS_SEM*)psSem);

#define single          0x00
#define boolean         0x01
#define int8            0x02
#define int16           0x03
#define int32           0x04
#define uint8           0x05
#define uint16          0x06
#define uint32          0x07

typedef struct sMonitorInfo /*设备模拟量接口类型*/
{
    void*      pvVal;    
    OS_SEM*    psSem;
    
    uint8_t    ucDataType;  //数据类型    
    uint16_t   usDataVal;   //数据值  
    uint16_t   usDataId;    //全局标示
    
    struct sMonitorInfo*  pNext;
    struct sMonitorInfo*  pLast;    
}sMonitorInfo;

void vMonitorRegist(void* pvVal, uint8_t ucDataType, OS_SEM* psSem);
 
void vMonitorInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size);
#endif