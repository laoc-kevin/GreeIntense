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
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "cc.h"
#include "os.h"
#include <string.h>

#define SYS_STK_SIZE	  256

#define MAX_QUEUES        10	// the number of mailboxes
#define MAX_QUEUE_ENTRIES 20	// the max size of each mailbox	

/* HANDLE is used for sys_sem_t but we won't include windows.h */
struct _sys_sem {
  void *sem;
};

/** struct of LwIP mailbox */
typedef OS_SEM     sys_sem_t; // type of semiphores
typedef OS_MUTEX   sys_mutex_t; // type of mutex
typedef OS_Q       sys_mbox_t; // type of mailboxes
typedef CPU_INT08U sys_thread_t; // type of id of the new thread

#define SYS_SEM_NULL NULL
//#define sys_sem_valid(sema) (((sema) != NULL) && ((sema) != NULL)  && ((sema) != (void*)-1))
//#define sys_sem_set_invalid(sema) ((sema) = NULL)
//#define sys_sem_set_invalid(sema, err) (OSSemDel(sema, OS_OPT_DEL_ALWAYS, &err))

/* let sys.h use binary semaphores for mutexes */
#define LWIP_COMPAT_MUTEX 1

#ifndef MAX_QUEUE_ENTRIES
#define MAX_QUEUE_ENTRIES 100
#endif

struct lwip_mbox {
  u32_t* pMsg;
  OS_Q*  pQ;
};

//typedef struct lwip_mbox sys_mbox_t;
#define SYS_MBOX_NULL NULL
//#define sys_mbox_valid(mbox) ((mbox != NULL) && ((mbox) != NULL)  && ((mbox) != (void*)-1))
//#define sys_mbox_set_invalid(mbox) ((mbox) = NULL)

/* DWORD (thread id) is used for sys_thread_t but we won't include windows.h */
//typedef u32_t sys_thread_t;

int sys_mbox_valid(sys_mbox_t *mbox);
void sys_mbox_set_invalid(sys_mbox_t *mbox);

#endif /* __ARCH_SYS_ARCH_H__ */





