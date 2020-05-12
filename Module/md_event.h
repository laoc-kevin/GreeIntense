#ifndef _MD_EVENT_H_
#define _MD_EVENT_H_

#include "includes.h"

//事件绑定  
#define CONNECT(psSem, psTCB) \
       vEventRegist((OS_SEM*)psSem, (OS_TCB*)psTCB);


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

void vEventRegist(OS_SEM* psSem, OS_TCB* psTCB);

OS_TCB* psEventGetTCB(void);

#endif