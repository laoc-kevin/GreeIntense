#ifndef _MD_LWIP_H_
#define _MD_LWIP_H_
#include <os.h>

static OS_SEM Lwip_Sem;

void vLwipInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size);
#endif