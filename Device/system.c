#include "system.h"
#include "md_event.h"

/*************************************************************
*                         系统                               *
**************************************************************/


/*系统开启*/
void vSystem_SwitchOpen(System* pt)
{
    System* pThis = (System*)pt;
    pThis->Unit.ucSwitchCmd = ON; 
}

/*系统关闭*/
void vSystem_SwitchClose(System* pt)
{
    System* pThis = (System*)pt;
    pThis->Unit.ucSwitchCmd = OFF; 
}


/*系统内部消息轮询*/
void vSystem_PollTask(void *p_arg)
{
    CPU_TS            ts = 0;
    OS_MSG_SIZE  msgSize = 0;
    OS_ERR           err = OS_ERR_NONE;
    
    while(DEF_TRUE)
	{
        sMsg* psMsg = (sMsg*)OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, &ts, &err);

        
        
        
        
        
        
        
       
    }
}

/*创建系统内部消息轮询任务*/
BOOL xSystem_CreatePollTask(System* pt)
{
    OS_ERR    err = OS_ERR_NONE;
    System* pThis = (System*)pt;
    
    sTaskInfo*  psTaskInfo = &pThis->sTaskInfo;
   
    OS_PRIO             prio = psTaskInfo->ucPrio;
    OS_TCB*            p_tcb = (OS_TCB*)(&psTaskInfo->sTCB);  
    CPU_STK*      p_stk_base = (CPU_STK*)(psTaskInfo->usStk);
    CPU_STK_SIZE    stk_size = MB_DEV_POLL_TASK_STK_SIZE; 
    
    OSTaskCreate(&psTaskInfo->sTCB,
                  "vSystem_PollTask",
                  vSystem_PollTask,
                  (void*)0,
                  prio,
                  p_stk_base ,
                  stk_size / 10u,
                  stk_size,
                  0u,
                  0u,
                  0u,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
                  &err);
    return (err == OS_ERR_NONE);              
}

/*系统初始化*/
void vSystem_Init(System* pt, sSystemInfo* psSystemInfo)
{
    uint8_t n;  
    System* pThis = (System*)pt; 
    
   
    pThis->sTaskInfo.ucPrio = psSystemInfo->ucPrio;
      
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pThis->psExAirFanList[n] = (ExAirFan*)ExAirFan_new();     //实例化对象
    }
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pThis->psModularRoofList[n] = (ModularRoof*)ModularRoof_new();
    }
    
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pThis->psCO2SenList[n] = (CO2Sensor*)CO2Sensor_new();     //实例化对象
    }
    for(n=0; n < TEMP_HUMI_SEN_NUM; n++)
    {
        pThis->psTempHumiSenList[n] = (TempHumiSensor*)TempHumiSensor_new();
    }
    
    xSystem_CreatePollTask(pThis);
    
}

CTOR(System)   //系统构造函数
    SUPER_CTOR(Unit);
    FUNCTION_SETTING(init, vSystem_Init);
END_CTOR