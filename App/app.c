
/*
*********************************************************************************************************
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdio.h>
#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>
#include  <os_app_hooks.h>
#include  <app_cfg.h>
#include  <bsp.h>
#include  <bsp_int.h>

#include "my_rtt_printf.h"
#include "lpc_clkpwr.h"

#include "app_led.h"
#include "app_config.h"
#include "app_input.h"
#include "app_segment.h"
#include "app_modbus.h"
#include "app_output.h"
#include "app_rtc.h"
#include "app_watchdog.h"

#include "mb.h"


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

uint8_t SysStartFlag = 1;
                                                                /* --------------- APPLICATION GLOBALS ---------------- */
static  OS_TCB       AppTaskStartTCB;
static  CPU_STK      AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart (void  *p_arg);
static  void  AppTaskCreate(void);
static  void  AppObjCreate (void);
static  void  AppTaskStackWatch (void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
* Notes       : 1) STM32F4xx HAL library initialization:
*                      a) Configures the Flash prefetch, intruction and data caches.
*                      b) Configures the Systick to generate an interrupt. However, the function ,
*                         HAL_InitTick(), that initializes the Systick has been overwritten since Micrium's
*                         RTOS has `its own Systick initialization and it is recommended to initialize the
*                         Systick after multitasking has started.
*
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR   err = OS_ERR_NONE;
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err = CPU_ERR_NONE;
#endif

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)"LPC4088",
                (CPU_ERR  *)&cpu_err);
#endif

    BSP_IntDisAll();                                            /* Disable all Interrupts.                              */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate(&AppTaskStartTCB,                              /* Create the start task                                */
                  "App Task Start",
                  AppTaskStart,
                  0u,
                  APP_CFG_TASK_START_PRIO,
                 &AppTaskStartStk[0u],
                  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                  APP_CFG_TASK_START_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cnts = 0;
    OS_ERR      err = OS_ERR_NONE;
	uint8_t i = 0;
 
    BSP_PostInit();                                             /* Initialize BSP functions                             */

    cnts = BSP_CPU_ClkGet() / OSCfg_TickRate_Hz;
    OS_CPU_SysTickInit(cnts);
	
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    (void)CPU_IntDisMeasMaxCurReset();
#endif

    AppObjCreate();                                             /* Create Applicaiton kernel objects                    */

    AppTaskCreate();                                            /* Create Application tasks                             */

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0u, 0u, 15u, 0u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
        SysStartFlag = 0;
    		for(i = 0; i < 17; i++)
    		{
    			OSTimeDlyHMSM(0u, 0u, 0u, 100u,
                          OS_OPT_TIME_HMSM_STRICT,
                          &err);
    		}
    }
}

//任务配置信息
//系统控制主任务

#define  	APP_OUTPUT_SET_TASK_PRIO		12
#define     APP_OUTPUT_SET_TASK_STK_SIZE     128
OS_TCB      AppOutputSetTaskTCB;
CPU_STK     AppOutputSetTaskStk[APP_OUTPUT_SET_TASK_STK_SIZE];

#define  	APP_INPUT_RECEIVE_TASK_PRIO		13
#define     APP_INPUT_RECEIVE_TASK_STK_SIZE     128
OS_TCB      AppInputReceiveTaskTCB;
CPU_STK     AppInputReceiveTaskStk[APP_INPUT_RECEIVE_TASK_STK_SIZE];

#define  	APP_SEGMENT_TASK_PRIO  			14
#define     APP_SEGMENT_TASK_STK_SIZE         	128
OS_TCB      AppSegmentTaskTCB;
CPU_STK     AppSegmentTaskStk[APP_SEGMENT_TASK_STK_SIZE];

#define  	APP_MB_SLAVE_POLL_TASK_PRIO  	16
#define     APP_MB_SLAVE_POLL_TASK_STK_SIZE   160
OS_TCB      AppMbSlavePollTaskTCB;
CPU_STK     AppMbSlavePollTaskStk[APP_MB_SLAVE_POLL_TASK_STK_SIZE];
CPU_STK     AppMbSlavePollTaskFPStk[APP_MB_SLAVE_POLL_TASK_STK_SIZE];

#define  	APP_MB_MASTER_POLL_TASK_PRIO  	17
#define     APP_MB_MASTER_POLL_TASK_STK_SIZE    160
OS_TCB      AppMbMasterPollTaskTCB;
CPU_STK     AppMbMasterPollTaskStk[APP_MB_MASTER_POLL_TASK_STK_SIZE];

#define  	APP_MB_SCAN_SLAVE_TASK_PRIO  	18
#define     APP_MB_SCAN_SLAVE_TASK_STK_SIZE    160
OS_TCB      AppMbScanSlaveTaskTCB;
CPU_STK     AppMbScanSlaveTaskStk[APP_MB_SCAN_SLAVE_TASK_STK_SIZE];

#define  	APP_WATCHDOG_FEED_TASK_PRIO  	23
#define     APP_WATCHDOG_FEED_TASK_STK_SIZE    128
OS_TCB      AppWatchDogFeedTaskTCB;
CPU_STK     AppWatchDogFeedTaskStk[APP_WATCHDOG_FEED_TASK_STK_SIZE];

#define  AppTaskStackWatch_PRIO            24 
#define  AppTaskStackWatch_STK_SIZE       100 
OS_TCB  AppTaskStackWatch_TCB;		
CPU_STK AppTaskStackWatch_STK [AppTaskStackWatch_STK_SIZE];

/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create Application Tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	
	OS_ERR err = OS_ERR_NONE;
	//初始化
	
	CPU_SR_ALLOC();

	AppRTCInit();

	OS_CRITICAL_ENTER();

    //IO输入数据接收任务
	OSTaskCreate(&AppOutputSetTaskTCB,
                  "AppOutputSetTask",
                  AppOutputSetTask,
                  0u,
                  APP_OUTPUT_SET_TASK_PRIO,
                  &AppOutputSetTaskStk[0u],
                  APP_OUTPUT_SET_TASK_STK_SIZE / 10u,
                  APP_OUTPUT_SET_TASK_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
                 &err);


	//IO输入数据接收任务
	OSTaskCreate(&AppInputReceiveTaskTCB,
                  "AppInputReceiveTask",
                  AppInputReceiveTask,
                  0u,
                  APP_INPUT_RECEIVE_TASK_PRIO,
                 &AppInputReceiveTaskStk[0u],
                  APP_INPUT_RECEIVE_TASK_STK_SIZE / 10u,
                  APP_INPUT_RECEIVE_TASK_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
                 &err);
				 
	//数码管显示任务
	OSTaskCreate(&AppSegmentTaskTCB,
                  "AppSegmentTask",
                  AppSegmentTask,
                  0u,
                  APP_SEGMENT_TASK_PRIO,
                 &AppSegmentTaskStk[0u],
                  APP_SEGMENT_TASK_STK_SIZE / 10u,
                  APP_SEGMENT_TASK_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
                 &err);
				 
#if WATCHDOG_ENABLE
	//喂狗任务
	OSTaskCreate(&AppWatchDogFeedTaskTCB,
                  "AppWatchDogFeedTask",
                  AppWatchDogFeedTask,
                  0u,
                  APP_WATCHDOG_FEED_TASK_PRIO,
                 &AppWatchDogFeedTaskStk[0u],
                  APP_WATCHDOG_FEED_TASK_STK_SIZE / 10u,
                  APP_WATCHDOG_FEED_TASK_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
                 &err);
#endif

#if TASK_STACK_WATCH_ENABLE
 OSTaskCreate( (OS_TCB     *)&AppTaskStackWatch_TCB,
                (CPU_CHAR   *)"AppTaskStackWatch",
                (OS_TASK_PTR ) AppTaskStackWatch,
                (void       *) 0,
                (OS_PRIO     ) AppTaskStackWatch_PRIO,
                (CPU_STK    *)&AppTaskStackWatch_STK[0],
                (CPU_STK_SIZE) AppTaskStackWatch_STK_SIZE/10,/*栈溢出临界值我设置在栈大小的90%处*/
                (CPU_STK_SIZE) AppTaskStackWatch_STK_SIZE,
                (OS_MSG_QTY  ) 0,
                (OS_TICK     ) 0,
                (void       *) 0,
                (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                (OS_ERR     *) &err);
                
    OS_TaskSuspend((OS_TCB*)&AppTaskStartTCB,&err);				
	OS_CRITICAL_EXIT();

#endif
                
}
/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create Application Kernel Objects.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	
}

void  AppTaskStackWatch (void *p_arg)
{
  CPU_SR_ALLOC();
  OS_ERR err;
  CPU_STK_SIZE free,used;
  
  while(DEF_TRUE)
  {
	OS_CRITICAL_ENTER();  
	  
    OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &err);

//	OSTaskStkChk (&Data_Process_TCB,&free,&used,&err);
//	myprintf("Data_Process  used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
	
	OS_CRITICAL_EXIT(); //退出临界区	
   }
}

