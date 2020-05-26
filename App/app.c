
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

#include "lpc_clkpwr.h"
#include "my_rtt_printf.h"

#include "app_config.h"

#include "md_led.h"
#include "md_input.h"
#include "md_segment.h"
#include "md_modbus.h"
#include "md_output.h"
#include "md_event.h"
#include "md_rtc.h"
#include "md_watchdog.h"

#include "system.h"

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
    while(DEF_ON){;}                                            /* Should Never Get Here.                               */
}


/**********************************************************************************************************
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
**********************************************************************************************************/
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

    while (DEF_TRUE)                                            /* Task body, always written as an infinite loop.       */
    {                                         
        OSTimeDlyHMSM(0u, 0u, 15u, 0u, OS_OPT_TIME_HMSM_STRICT, &err);
        SysStartFlag = 0;
        for(i = 0; i < 17; i++)
        {
        	OSTimeDlyHMSM(0u, 0u, 0u, 100u, OS_OPT_TIME_HMSM_STRICT, &err);
        }
    }
}

#if TASK_STACK_WATCH_EN > 0

void  AppTaskStackWatch (void *p_arg)
{
    CPU_SR_ALLOC();
    OS_ERR err;
    CPU_STK_SIZE free,used;
  
    while(DEF_TRUE)
    {
        OS_CRITICAL_ENTER();  
          
        OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &err);
           
        //OSTaskStkChk (&Data_Process_TCB,&free,&used,&err);
        //myprintf("Data_Process  used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
        
        OS_CRITICAL_EXIT(); //退出临界区	
     }
}

void  AppTaskStackWatchInit(OS_TCB *p_tcb, OS_PRIO prio, CPU_STK *p_stk_base, CPU_STK_SIZE stk_size)
{
    OSTaskCreate( &p_tcb, "TaskStackWatch", AppTaskStackWatch, 0, prio, p_stk_base, stk_size/10, stk_size,
                   0,0,0, (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &err );
                
//    OS_TaskSuspend((OS_TCB*)&AppTaskStartTCB,&err);	
}

#endif

/************************任务配置信息***************************/

#if INPUT_RECEIVE_TASK_EN  >0                     //IO输入数据接收功能
    OS_TCB      InputReceiveTaskTCB;
    CPU_STK     InputReceiveTaskStk[INPUT_RECEIVE_TASK_STK_SIZE];
#endif

#if SEGMENT_TASK_EN >0                            //数码管显示功能
    OS_TCB      SegmentTaskTCB;
    CPU_STK     SegmentTaskStk[SEGMENT_TASK_STK_SIZE];
#endif

#if EEPROM_DATA_TASK_EN >0                        //eeprom参数记忆功能
    OS_TCB      EEPROMDataTaskTCB;
    CPU_STK     EEPROMDataTaskStk[SEGMENT_TASK_STK_SIZE];
#endif

#if SYSTEM_EVENT_TASK_EN >0                       //系统事件功能
    OS_TCB      SystemEventTaskTCB;
    CPU_STK     SystemEventTaskStk[SYSTEM_EVENT_TASK_STK_SIZE];
#endif

#if SYSTEM_MONITOR_TASK_EN >0                    //系统数据监控功能
    OS_TCB      SystemMonitorTaskTCB;
    CPU_STK     SystemMonitorTaskStk[SYSTEM_MONITOR_TASK_STK_SIZE];
#endif

#if SYSTEM_MAIN_CTRL_TASK_EN  >0                 //系统逻辑控制功能
    OS_TCB      SystemMainCtrlTCB;		
    CPU_STK     SystemMainCtrlSTK [SYSTEM_MAIN_CTRL_TASK_STK_SIZE];
#endif	

#if WATCHDOG_FEED_TASK_EN >0                     //喂狗功能
    OS_TCB      WatchDogFeedTaskTCB;
    CPU_STK     WatchDogFeedTaskStk[WATCHDOG_FEED_TASK_STK_SIZE];  
#endif

#if TASK_STACK_WATCH_TASK_EN > 0                 //内存监控功能
    OS_TCB      TaskStackWatchTCB;		
    CPU_STK     TaskStackWatchStk[TASK_STACK_WATCH_TASK_STK_SIZE];
#endif



/*********************************************************************************************************
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
*********************************************************************************************************/
static  void  AppTaskCreate (void)
{
	OS_ERR err = OS_ERR_NONE;
	CPU_SR_ALLOC();

	vRTCInit();
    
	OS_CRITICAL_ENTER();

#if OUTPUT_SET_TASK_EN > 0     //IO输出数据接收功能 
    vOutputInit();   
#endif
    
#if INPUT_RECEIVE_TASK_EN > 0   //IO输入数据接收功能
    vInputInit(&InputReceiveTaskTCB, INPUT_RECEIVE_TASK_PRIO, InputReceiveTaskStk, INPUT_RECEIVE_TASK_STK_SIZE); 
#endif

#if SEGMENT_TASK_EN > 0         //数码管显示功能  
    vSegmentInit(&SegmentTaskTCB, SEGMENT_TASK_PRIO, SegmentTaskStk, SEGMENT_TASK_STK_SIZE);
#endif

#if SYSTEM_EVENT_TASK_EN >0      //系统事件功能
    vEventInit(&SystemEventTaskTCB, SYSTEM_EVENT_TASK_PRIO, SystemEventTaskStk, SYSTEM_EVENT_TASK_STK_SIZE);
#endif

#if SYSTEM_MONITOR_TASK_EN >0    //系统数据监控功能
    vEventInit(&SystemMonitorTaskTCB, SYSTEM_MONITOR_TASK_PRIO, SystemMonitorTaskStk, SYSTEM_MONITOR_TASK_STK_SIZE);
#endif

#if MB_SLAVE_POLL_TASK_EN > 0    //Modbus RS485 从栈功能    
    vModbusSlaveInit(MB_SLAVE_POLL_TASK_PRIO);
#endif

#if MB_MASTER_POLL_TASK_EN > 0   //Modbus RS485 主栈功能
    vModbusMasterInit(MB_MASTER_POLL_TASK_PRIO, MB_MASTER_SCAN_TASK_PRIO)
#endif

#if SYSTEM_MAIN_CTRL_TASK_EN > 0 //系统控制功能 
    vSystemInit(SYSTEM_MAIN_CTRL_TASK_PRIO); 
#endif	

#if WATCHDOG_FEED_TASK_EN >0     //喂狗功能
    vWatchDogInit(&WatchDogFeedTaskTCB, WATCHDOG_FEED_TASK_PRIO, WatchDogFeedTaskStk, WATCHDOG_FEED_TASK_STK_SIZE);
#endif

#if TASK_STACK_WATCH_EN > 0      //内存监控功能
    AppTaskStackWatchInit(&TaskStackWatchTCB, TASK_STACK_WATCH_TASK_PRIO, TaskStackWatchStk, TASK_STACK_WATCH_TASK_STK_SIZE)
#endif

	OS_CRITICAL_EXIT();              
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




