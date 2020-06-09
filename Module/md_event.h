#ifndef _MD_EVENT_H_
#define _MD_EVENT_H_

#include "includes.h"

//事件绑定  
#define CONNECT(psSem, psTCB) \
       vEventRegist((OS_SEM*)psSem, (OS_TCB*)psTCB);


typedef struct sEvent /*事件类型*/
{
    OS_SEM*         psSem;     
    OS_TCB*         psTCB; 
    struct sEvent*  pNext;
    struct sEvent*  pLast;    
}sEvent;

typedef struct  /*消息结构封装*/
{
    OS_SEM*        psSem; 
    void*          pvArg;     
}sEventMsg;


void vEventRegist(OS_SEM* psSem, OS_TCB* psTCB);
void vEventInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size);

OS_ERR eTaskCreate(OS_TCB *p_tcb, OS_TASK_PTR p_task, void *p_arg, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size);
OS_TCB* psGetEventTCB(void);

#endif