#ifndef _MD_EVENT_H_
#define _MD_EVENT_H_

#include "includes.h"

typedef struct sEvent /*设备模拟量接口类型*/
{
    OS_SEM*         psSem;     
    OS_TCB*         psTCB; 
    struct sEvent*  pNext;
    struct sEvent*  pLast;    
}sEvent;

typedef struct  /*设备模拟量接口类型*/
{
    OS_SEM*        psSem; 
    void*          pvArg;     
}sMsg;


OS_TCB* psEventGetTCB(void);

#endif