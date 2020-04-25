#ifndef _MD_EVENT_H_
#define _MD_EVENT_H_

#include "includes.h"

typedef struct sEvent /*设备模拟量接口类型*/
{
    void*           pvVal;     
    OS_TCB*         psTCB; 
    int32_t         lValBuf;
    uint8_t         ucEventNum;    
    struct sEvent*  pNext;
    struct sEvent*  pLast;    
}sEvent;

typedef struct  /*设备模拟量接口类型*/
{
    void*          pvVal;     
    uint8_t        ucEventNum;
}sMsg;


#endif