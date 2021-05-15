#ifndef _APP_LWIP_H_
#define _APP_LWIP_H_

#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwIP.h"
#include <stdint.h>
#include <string.h>
#include <os.h>
#include "my_rtt_printf.h"

#define TCP_SERVER_RX_BUFSIZE	2000		
#define TCP_SERVER_PORT			8080	
#define LWIP_SEND_DATA			0X80	

#define LWIP_STK_SIZE	256

#define  LWIP_TASK_START_PRIO                6
#define  LWIP_TASK_END_PRIO                  12

#define LWIP_TASK_MAX	  (LWIP_TASK_END_PRIO - LWIP_TASK_START_PRIO + 1)	
#define LWIP_START_PRIO	  LWIP_TASK_START_PRIO	

typedef struct TcpServerConnParam
{
	OS_TCB *Conn_Task_TCB;
	struct netconn *newconn;
}sTcpServerConnParam;

extern uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	
extern uint8_t tcp_server_flag;		

void  AppRecycleTask(void *arg);
void  AppTcpServerTask(void *arg);
void  AppLwipCreateTask (void *arg);

#endif