#ifndef _MD_DATA_MONITOR_H_
#define _MD_DATA_MONITOR_H_

#include "includes.h"

typedef struct sDataInfo /*设备模拟量接口类型*/
{
    void*              pvVal;    
    OS_TCB*            psTCB;
    
    int32_t            sValBuf;     
    uint16_t           usDataId;    //全局标示
    
    struct sDataInfo*  pNext;
//    struct sEvent*  pLast;    
}sDataInfo;


#endif