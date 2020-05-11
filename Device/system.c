#include "system.h"
#include "md_event.h"

/*************************************************************
*                         系统                               *
**************************************************************/

/*系统排风机配置信息*/
sFanInfo ExAirFanSet[EX_AIR_FAN_NUM] = { {CONSTANT_FREQ, 0, 0, 0, 0, 1},
                                         {CONSTANT_FREQ, 0, 0, 0, 0, 2},
                                         {CONSTANT_FREQ, 0, 0, 0, 0, 3},
                                       };



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
    
    ModularRoof*    pModularRoof    = NULL;
    ExAirFan*       pExAirFan       = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    CO2Sensor*      pCO2Sensor      = NULL;
    
    System* pThis = (System*)pt; 
    
    pThis->psMBMasterInfo = psSystemInfo->psMBMasterInfo;
    pThis->sTaskInfo.ucPrio = psSystemInfo->ucPrio;
      
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = (ExAirFan*)ExAirFan_new();  //实例化对象
        pExAirFan->init(pExAirFan, &ExAirFanSet[n]);
        
        pThis->psExAirFanList[n] = pExAirFan;  
    }
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = (ModularRoof*)ModularRoof_new();
        pModularRoof->init(pModularRoof, psSystemInfo->psMBMasterInfo); //初始化
        
        pThis->psModularRoofList[n] = pModularRoof;
    }
    
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)CO2Sensor_new();     //实例化对象
        pCO2Sensor->Sensor.init( SUPER_PTR(pCO2Sensor, Sensor), psSystemInfo->psMBMasterInfo); //向上转型，由子类转为父类
        
        pThis->psCO2SenList[n] = pCO2Sensor;
    }
    for(n=0; n < TEMP_HUMI_SEN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor), psSystemInfo->psMBMasterInfo);
        
        pThis->psTempHumiSenList[n] = pTempHumiSensor;
         
    }
    xSystem_CreatePollTask(pThis); 
}

CTOR(System)   //系统构造函数
    SUPER_CTOR(Unit);
    FUNCTION_SETTING(init, vSystem_Init);
END_CTOR