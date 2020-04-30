#ifndef _MD_DATA_MONITOR_H_
#define _MD_DATA_MONITOR_H_

#include "includes.h"

typedef struct sMonitorInfo /*设备模拟量接口类型*/
{
    void*      pvVal;    
    OS_SEM*    psSem;
               
    int32_t    sDataBuf;     
    uint16_t   usDataId;    //全局标示
    
    struct sMonitorInfo*  pNext;
}sMonitorInfo;


#endif