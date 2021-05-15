#include "app_lwip.h"

#include "tcp.h"
#include "sys_arch.h"

uint8_t   tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	
uint8_t   tcp_server_flag, iPrioIndex;

sTcpServerConnParam   sConnParam[LWIP_TASK_MAX];

static OS_MEM StackMem;   
static OS_SEM Lwip_Sem;

#pragma pack(4)
CPU_STK       LwIP_Task_Stk[LWIP_TASK_MAX][LWIP_STK_SIZE];
#pragma pack(4)

CPU_INT08U    LwIP_task_priopity_stask[LWIP_TASK_MAX];
OS_TCB        LwIP_task_TCB[LWIP_TASK_MAX]; 
OS_Q          RecycleSQ;

static void AppTcpServerCreateConn(void *arg);
static void AppTcpServerConnThread(void *arg);

void  AppLwipCreateTask (void *arg)
{
	OS_ERR err;
	Init_lwIP();
    
	//OSSemPost(&Lwip_Sem, OS_OPT_POST_ALL, &err);
	
	//OSTaskDel(NULL, &err);
}

void AppTcpServerTask(void *arg)
{
	uint16_t data_len, n;
	struct pbuf *q;
	err_t recv_err;
	OS_ERR error;
	uint8_t remot_addr[4];
	struct netconn *conn, *newconn;
	static ip_addr_t ipaddr;
	static u16_t 		port;
	
	struct netbuf *recvbuf;
    void *dataptr;
	
    
    
	CPU_SR_ALLOC();	
	
	LWIP_UNUSED_ARG(arg);

    Init_lwIP();
    
	OSSemPend (&Lwip_Sem, 0, OS_OPT_PEND_BLOCKING, 0, &error);
	OSQCreate(&RecycleSQ, "Recycle quiue", LWIP_TASK_MAX, &error); 
	
//	OSMemCreate(&StackMem, "LWIP TASK STK", (void*)LwIP_Task_Stk, LWIP_TASK_MAX, LWIP_STK_SIZE * sizeof(CPU_STK), &error);
	
	conn = netconn_new(NETCONN_TCP);  
	conn->recv_timeout = 0;
	netconn_bind(conn, IP_ADDR_ANY,TCP_SERVER_PORT);  
	netconn_listen(conn);  

    iPrioIndex = 0;
	while (1) 
	{
		OSTimeDlyHMSM( 0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &error ); 
		for( n = iPrioIndex; n < LWIP_TASK_MAX; n++)
		{
			if(LwIP_task_priopity_stask[n] == 0)
			{
				iPrioIndex = n;
				break;
			}
		}
		if( n == LWIP_TASK_MAX)
		{
			for(n = 0; n < iPrioIndex; n++)
			{
				if(LwIP_task_priopity_stask[n] == 0)
			    {
					iPrioIndex = n;
			    	break;
			    }
			}
			if(n == iPrioIndex)
			{
				myprintf( "sys_thread_new: there is no free priority\n" );
			    continue;
			}
		}
		myprintf("netconn_accept\n");
		recv_err = netconn_accept(conn,&newconn);  

		if (recv_err == ERR_OK)    
		{ 
			newconn->recv_timeout = 3000;
			netconn_getaddr(newconn,&ipaddr,&port,0); 
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
			myprintf("master:%d.%d.%d.%d  port:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			
			if(newconn!=NULL)
			{						
                AppTcpServerCreateConn((void*)newconn);
				
				myprintf("AppTcpServerCreateConn %d\n",iPrioIndex);
			}
		}
	}	
}

void AppTcpServerCreateConn(void *arg)
{
	uint16_t    i, ubPrio, stacksize;
    OS_ERR      ucErr;
	CPU_STK *   task_stk;
	
    ubPrio = LWIP_TASK_START_PRIO + iPrioIndex;
	
    if(stacksize > LWIP_STK_SIZE || !stacksize || stacksize < OS_CFG_STK_SIZE_MIN) 
	{
	    stacksize = LWIP_STK_SIZE;
	}
//	
////    task_stk = OSMemGet( &StackMem, &ucErr );
//	
	task_stk = &LwIP_Task_Stk[iPrioIndex][0];
//	
//    if(ucErr != OS_ERR_NONE)
//	{
//        myprintf( "sys_thread_new: impossible to get a stack\n" );
//        return;
//    } 
	
	sConnParam[iPrioIndex].Conn_Task_TCB = (OS_TCB *)&LwIP_task_TCB[iPrioIndex];
	sConnParam[iPrioIndex].newconn  = (struct netconn *)arg;
	
    OSTaskCreate(&LwIP_task_TCB[iPrioIndex],
                 (CPU_CHAR  *)"AppTcpServerConnThread",
                 (OS_TASK_PTR)AppTcpServerConnThread, 
                 (void      *)&sConnParam[iPrioIndex],
                 (OS_PRIO    )ubPrio,
                 (CPU_STK   *)&task_stk[0],
                 (CPU_STK_SIZE)stacksize/10,
                 (CPU_STK_SIZE)stacksize,
                 (OS_MSG_QTY )0,
                 (OS_TICK    )0,
                 (void      *)0,
                 (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR    *)&ucErr);			 
	myprintf("AppTcpServerConnThread  %d  ubPrio %d  stacksize %d\n", ucErr, ubPrio, stacksize);

    if(ucErr == OS_ERR_NONE)
	{
		LwIP_task_priopity_stask[iPrioIndex] = 1;
	}		
}

void AppTcpServerConnThread(void *arg)
{
	uint8_t remot_addr[4];
	
	uint16_t data_len, iPrio, port;
	ip_addr_t ipaddr;
	err_t recv_err;
	OS_ERR error;

	void *dataptr;
	struct netconn *newconn;
	struct netbuf *recvbuf;

	sTcpServerConnParam*  psConnParam = (sTcpServerConnParam*)arg;
	
	newconn = (struct netconn *)psConnParam->newconn;

//	newconn = sConnParam.newconn;
	
	iPrio = psConnParam->Conn_Task_TCB ->Prio;
	
//	newconn = (struct netconn *)arg;
	
    myprintf("iPrioIndex %d \n",  iPrio);	
	
	if (newconn != NULL)    
	{ 
		recv_err = netconn_getaddr(newconn,&ipaddr,&port,0); 
		
		remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
		remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
		remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
		remot_addr[0] = (uint8_t)(ipaddr.addr);	
		
		myprintf("master:%d.%d.%d.%d  port:%d \n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
	}
	
	while(newconn != NULL)
	{
	    recv_err = netconn_recv(newconn, &recvbuf);
		
	    if( (recv_err == ERR_OK) && (recvbuf != NULL) )  	
	    {	
            recv_err = netbuf_data(recvbuf, &dataptr, &data_len);
	     
			if( (recv_err == ERR_OK) && (recvbuf != NULL))
			{
				if(data_len == 0)
                { 
					netbuf_delete(recvbuf);
	    	    	netconn_close(newconn);
	    	        netconn_delete(newconn);
			    
	    	        myprintf("master:%d.%d.%d.%d  port:%d  close\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3], port);     
			    	break;
	    	    }
//	    	    memset(tcp_server_recvbuf, 0, TCP_SERVER_RX_BUFSIZE); 			
//                memcpy(tcp_server_recvbuf, (uint8_t*)dataptr, data_len);				
 	                 
//	    	    recv_err = netconn_write(newconn ,tcp_server_recvbuf,strlen((char*)tcp_server_recvbuf),NETCONN_COPY);

                recv_err = netconn_write(newconn, (uint8_t *)dataptr, data_len, NETCONN_COPY);				
	            myprintf("%s \n", (uint8_t *)dataptr); 
			    
	    	    data_len=0;  
	    	    netbuf_delete(recvbuf);
			}
	    }
	    else if( (recv_err == ERR_CLSD) || ((recvbuf == NULL) && (recv_err != ERR_TIMEOUT)) ) 
	    {
	    	myprintf(" recv_err  %d   newconn->state %d\n", recv_err, newconn->state );
	    	netconn_close(newconn);
	    	netconn_delete(newconn);
	    	myprintf("master:%d.%d.%d.%d  port:%d  close\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
	    	break;
	    }
	}
	
	if( (iPrio >= LWIP_TASK_START_PRIO) && (iPrio < LWIP_TASK_START_PRIO + LWIP_TASK_MAX) )
	{
		LwIP_task_priopity_stask[iPrio-LWIP_TASK_START_PRIO] = 0;
//		OSMemPut( &StackMem, psConnParam->Conn_Task_TCB->StkPtr ,&err);
	
	    myprintf("OSMemPut %d \n",  iPrio);
	}
	
	
	OSQPost(&RecycleSQ, psConnParam, sizeof(sTcpServerConnParam), OS_OPT_POST_ALL, &error);
	//OSTaskDel(NULL, &error);
}

void  AppRecycleTask(void *arg)
{
	OS_ERR err;
	uint16_t iPrio;
	CPU_TS ts;
    OS_MSG_SIZE   msg_size;
    void  *       msg;
	sTcpServerConnParam*  psConnParam;
	
	msg  = OSQPend (&RecycleSQ, 0, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &err);
	
	OSTimeDlyHMSM( 0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &err ); 
	
	psConnParam = (sTcpServerConnParam*)msg;
	
	iPrio = psConnParam->Conn_Task_TCB->Prio;
	
	 myprintf("OSMemPut %d \n",  iPrio);
	
	if( (iPrio >= LWIP_TASK_START_PRIO) && (iPrio < LWIP_TASK_START_PRIO + LWIP_TASK_MAX) )
	{
		LwIP_task_priopity_stask[iPrio-LWIP_TASK_START_PRIO] = 0;
//		OSMemPut( &StackMem, psConnParam->Conn_Task_TCB->StkPtr ,&err);
	
	    myprintf("OSMemPut %d \n",  psConnParam->Conn_Task_TCB->Prio);
	}
	
   
	
}

