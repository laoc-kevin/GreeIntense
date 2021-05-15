#include "lwIP.h"
#include "md_event.h"
#include "md_lwip.h"

/******************************************************************
*@brief 创建lwip任务函数							
******************************************************************/
void vLwipCreateTask(void *p_arg)
{
    OS_ERR err = OS_ERR_NONE;
	Init_lwIP();
    OSSemPost(&Lwip_Sem, OS_OPT_POST_ALL, &err);
}

/**********************************************************************
 * @brief  LWIP协议栈初始化
 *********************************************************************/
void vLwipInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
   OS_ERR err = OS_ERR_NONE;
   OSSemCreate(&Lwip_Sem, "Lwip_Sem", 0, &err);      //从栈事件消息量 
   (void)eTaskCreate(p_tcb, vLwipCreateTask, NULL, prio, p_stk_base, stk_size);
}