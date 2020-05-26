#ifndef MD_WATCHDOG_H
#define MD_WATCHDOG_H
#include "includes.h"

void vWatchDogInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size);

#endif
