/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/*  Porting by Michael Vysotsky <michaelvy@hotmail.com> August 2011   */

#define SYS_ARCH_GLOBALS

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "includes.h"
#include "arch/sys_arch.h"

/*----------------------------------------------------------------------------*/
/*                      DEFINITIONS                                           */
/*----------------------------------------------------------------------------*/
#define LWIP_ARCH_TICK_PER_MS       1000/OS_CFG_TICK_RATE_HZ

/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/


//当消息指针为空时,指向一个常量pvNullPointer所指向的值.
//在UCOS中如果OSQPost()中的msg==NULL会返回一条OS_ERR_POST_NULL
//错误,而在lwip中会调用sys_mbox_post(mbox,NULL)发送一条空消息,我们
//在本函数中把NULL变成一个常量指针0Xffffffff
const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;
 
OS_TCB  SYS_TASK_TCB;
CPU_STK SYS_TASK_STK[SYS_STK_SIZE];

//说明：创建一个消息邮箱
//参数：*mbox:消息邮箱；size:邮箱大小
//返回值:0,创建成功；-1,创建失败

err_t sys_mbox_new( sys_mbox_t *mbox, int size)
{
  OS_ERR ucErr;
      
  OSQCreate(mbox, "LWIP quiue", size, &ucErr); 
  LWIP_ASSERT("OSQCreate ", ucErr == OS_ERR_NONE);
  
  if( ucErr == OS_ERR_NONE)
  { 
    return 0; 
  }
  return -1;
}

//说明：释放并删除一个消息邮箱
//参数：*mbox，要删除的消息邮箱
//返回值：无
void sys_mbox_free(sys_mbox_t * mbox)
{
    OS_ERR     ucErr;
    LWIP_ASSERT( "sys_mbox_free ", mbox != SYS_MBOX_NULL );      
        
    OSQFlush(mbox,& ucErr);
    
    OSQDel(mbox, OS_OPT_DEL_ALWAYS, &ucErr);
    LWIP_ASSERT( "OSQDel ", ucErr == OS_ERR_NONE );
}


//说明：向消息邮箱中发送一条消息(必须发送成功)
//参数：*mbox:消息邮箱；*msg:要发送的消息
//返回值：无
void sys_mbox_post(sys_mbox_t *mbox, void *msg, u16_t uMsgSize)
{
  OS_ERR     ucErr;
  CPU_INT08U  i=0; 
  if( msg == NULL ) 
  {
	   msg = (void*)&pvNullPointer;
  }
  /* try 10 times */
  while(i < 10)
  {
    OSQPost(mbox, msg, uMsgSize,OS_OPT_POST_ALL, &ucErr);
    //debug("sys_mbox_post mbox %d msg %d uMsgSize %d ucErr %d\n", mbox, msg, uMsgSize, ucErr); 
    if(ucErr == OS_ERR_NONE)
	{
		break;
	}
    i++;
    OSTimeDly(5,OS_OPT_TIME_DLY,&ucErr);
  }
  LWIP_ASSERT( "sys_mbox_post error!\n", i !=10 );  
}


//说明：尝试向一个消息邮箱发送消息
//		此函数相对于sys_mbox_post函数只发送一次消息，
//		发送失败后不会尝试第二次发送
//参数：*mbox:消息邮箱；*msg:要发送的消息
//返回值:ERR_OK,发送成功；ERR_MEM,发送失败
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg, u16_t uMsgSize)
{
  OS_ERR ucErr = OS_ERR_NONE;
  if(msg == NULL)
  {
	  msg = (void*)&pvNullPointer; 
  }   
  OSQPost(mbox, msg, uMsgSize, OS_OPT_POST_ALL, &ucErr);     
  if(ucErr != OS_ERR_NONE)
  { 
      return ERR_MEM;
  }
  //debug("sys_mbox_trypost mbox %d msg %p uMsgSize %d ucErr %d\n", mbox, msg, uMsgSize, ucErr); 
  return ERR_OK;
}

//说明：等待邮箱中的消息
//参数：*mbox:消息邮箱；*msg:消息；
//		timeout:超时时间，如果timeout为0的话,就一直等待
//返回值:当timeout不为0时如果成功的话就返回等待的时间，
//		失败的话就返回超时SYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{ 
  OS_ERR	ucErr = OS_ERR_NONE;
  OS_MSG_SIZE   msg_size = 0;
    
  uint32_t tick = LWIP_ARCH_TICK_PER_MS;  
  CPU_TS ucos_timeout = 0;  
  CPU_TS in_timeout = timeout/tick;
	
  if(timeout && in_timeout == 0)
  {
      in_timeout = 1;
  }
  *msg = OSQPend(mbox, in_timeout, OS_OPT_PEND_BLOCKING, &msg_size, &ucos_timeout, &ucErr);
  
  //debug("sys_arch_mbox_fetch mbox %d  msg %d  in_timeout %d  ucErr %d ucos_timeout %d\n", mbox, *msg, in_timeout, ucErr, ucos_timeout);
  if(ucErr == OS_ERR_TIMEOUT )
  {
	  ucos_timeout = SYS_ARCH_TIMEOUT;
  }
  if(ucErr != OS_ERR_NONE)
  {
      *msg = NULL;
  }
  return ucos_timeout; 
}

//说明：尝试获取消息
//参数：*mbox:消息邮箱；*msg:消息
//返回值:等待消息所用的时间/SYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	return sys_arch_mbox_fetch(mbox,msg,1);//尝试获取一个消息
}

//说明：检查一个消息邮箱是否有效
//参数：*mbox:消息邮箱
//返回值:1,有效； 0,无效
int sys_mbox_valid(sys_mbox_t *mbox)
{
  if(mbox->NamePtr)
  {
	  return (strcmp(mbox->NamePtr,"?Q"))? 1:0;
  }	    
  else
  {
	  return 0;
  }  
}

//说明：设置一个消息邮箱为无效
//参数：*mbox:消息邮箱
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  if(sys_mbox_valid(mbox))
  {
	  sys_mbox_free(mbox);
  }   
}

/********************信号量相关函数*************************/

//说明：创建一个信号量
//参数；*sem:创建的信号量；count:信号量值
//返回值:ERR_OK,创建OK； ERR_MEM,创建失败
err_t sys_sem_new(sys_sem_t * sem, u8_t count)
{  
  OS_ERR ucErr = OS_ERR_NONE;
  OSSemCreate (sem, "LWIP Sem", count, &ucErr);
  if(ucErr != OS_ERR_NONE )
  {
      LWIP_ASSERT("OSSemCreate ",ucErr == OS_ERR_NONE );
      return -1;    
  }
  return 0;
}

//说明：等待一个信号量
//参数：*sem:要等待的信号量；timeout:超时时间
//返回值:当timeout不为0时如果成功的话就返回等待的时间，
//		失败的话，就返回超时SYS_ARCH_TIMEOUT
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{ 
  OS_ERR ucErr = OS_ERR_NONE;
  uint32_t tick = LWIP_ARCH_TICK_PER_MS;
  CPU_TS ucos_timeout = 0;
  CPU_TS in_timeout = timeout / tick;
    
  //debug("sys_arch_sem_wait timeout %d in_timeout %d tick %d\n", timeout, in_timeout, tick);  	  
  if(timeout && in_timeout == 0)
  {
      in_timeout = 1;  
  }
  OSSemPend(sem, in_timeout, OS_OPT_PEND_BLOCKING, &ucos_timeout, &ucErr);
  
    /*  only when timeout! */
  if(ucErr == OS_ERR_TIMEOUT)
  {
	  ucos_timeout = SYS_ARCH_TIMEOUT;
  }      	
  return ucos_timeout;
}

//说明：发送一个信号量
//参数：sem，信号量指针
void sys_sem_signal(sys_sem_t *sem)
{
  OS_ERR ucErr = OS_ERR_NONE;  
  OSSemPost(sem,OS_OPT_POST_ALL,&ucErr);
  LWIP_ASSERT("OSSemPost ",ucErr == OS_ERR_NONE );  
}

//说明：释放并删除一个信号量
//参数：sem:信号量指针
void sys_sem_free(sys_sem_t *sem)
{
    OS_ERR ucErr = OS_ERR_NONE;  
    OSSemDel(sem, OS_OPT_DEL_ALWAYS, &ucErr );
    LWIP_ASSERT( "OSSemDel ", ucErr == OS_ERR_NONE );
}

//说明：查询一个信号量的状态,无效或有效
//参数：sem:信号量指针
//返回值:1,有效； 0,无效
int sys_sem_valid(sys_sem_t *sem)
{
  if(sem->NamePtr)
    return (strcmp(sem->NamePtr,"?SEM"))? 1:0;
  else
    return 0;
}
//说明：设置一个信号量无效
//参数：sem，信号量指针
//返回值：无
void sys_sem_set_invalid(sys_sem_t *sem)
{
  if(sys_sem_valid(sem))
    sys_sem_free(sem);
}

//说明：arch初始化
void sys_init(void)
{
  OS_ERR ucErr;
//  memset(LwIP_task_priopity_stask, 0, sizeof(LwIP_task_priopity_stask));
//	
//  /* init mem used by sys_mbox_t, use ucosII functions */
//  OSMemCreate(&StackMem, "LWIP TASK STK", (void*)LwIP_Task_Stk, LWIP_TASK_MAX, LWIP_STK_SIZE * sizeof(CPU_STK), &ucErr);
	
  //LWIP_ASSERT( "sys_init: failed OSMemCreate STK", ucErr == OS_ERR_NONE );
}
 
//说明：创建一个新进程
//参数：*name:进程名称；thred:进程任务函数；*arg:进程任务函数的参数
//		stacksize:进程任务的堆栈大小
//		prio:进程任务的优先级
//返回值：

int tsk_prio;
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    int i;
    OS_ERR      ucErr;
	CPU_STK * task_stk;
	CPU_INT08U  ubPrio = prio;
	
    if(stacksize > SYS_STK_SIZE || !stacksize) 
	{
	    stacksize = SYS_STK_SIZE;
	}
    OSTaskCreate(&SYS_TASK_TCB,
                 (CPU_CHAR  *)name,
                 (OS_TASK_PTR)thread, 
                 (void      *)0,
                 (OS_PRIO    )prio,
                 (CPU_STK   *)&SYS_TASK_STK[0],
                 (CPU_STK_SIZE)stacksize/10,
                 (CPU_STK_SIZE)stacksize,
                 (OS_MSG_QTY )0,
                 (OS_TICK    )0,
                 (void      *)0,
                 (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR    *)&ucErr);  
						
    return ubPrio;
}


//说明：lwip延时函数
//ms:要延时的ms数
//void sys_msleep(u32_t ms)
//{
//  OS_ERR      ucErr;  
//  OSTimeDly(ms,OS_OPT_TIME_DLY,&ucErr);  
//}

//说明：获取系统时间,LWIP1.4.1增加的函数
//返回值:当前系统时间(单位:毫秒)
//u32_t sys_now(void)
//{
//	u32_t ucos_time, lwip_time;
//	ucos_time=OSTimeGet();	//获取当前系统时间 得到的是UCSO的节拍数
//	lwip_time=(ucos_time*1000/OS_TICKS_PER_SEC+1);//将节拍数转换为LWIP的时间MS
//	return lwip_time; 		//返回lwip_time;
//}















































