#include "bms.h"
#include "system.h"
#include "systemctrl.h"
#include "md_event.h"
#include "md_modbus.h"
#include "md_timer.h"

/*************************************************************
*                         系统                               *
**************************************************************/
#define SYSTEM_POLL_TASK_PRIO    9       //系统内部循环任务优先级
#define SYSTEM_ALARM_DO          5       //系统声光报警DO接口

#define HANDLE(p_arg1, p_arg2) if((USHORT*)psMsg->pvArg == (USHORT*)(&p_arg1)) {p_arg2;}

static System* psSystem = NULL;

/*系统排风机配置信息*/
sFanInfo ExAirFanSet[EX_AIR_FAN_NUM] = { {VARIABLE_FREQ, 25, 50, 1, 1, 1},
                                         {CONSTANT_FREQ, 0,   0, 0, 0, 2},
                                         {CONSTANT_FREQ, 0,   0, 0, 0, 3},
                                         {CONSTANT_FREQ, 0,   0, 0, 0, 4},
                                       };
                                                                       
/*系统内部消息轮询*/
void vSystem_PollTask(void *p_arg)
{
    CPU_TS            ts = 0;
    OS_MSG_SIZE  msgSize = 0;
    OS_ERR           err = OS_ERR_NONE;
    
    BMS* psBMS = BMS_Core();
    psSystem = System_Core();
    
    while(DEF_TRUE)
	{
        sMsg* psMsg = (sMsg*)OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, &ts, &err);
        
        HANDLE(psBMS->System.eSystemMode,      vSystem_ChangeSystemMode(psSystem, psBMS->System.eSystemMode))  
        HANDLE(psBMS->System.eRunningMode,     vSystem_SetRunningMode(psSystem, psBMS->System.eRunningMode)) 
        
        HANDLE(psBMS->System.sTempSet,         vSystem_SetTemp(psSystem, psBMS->System.sTempSet))
        HANDLE(psBMS->System.usFreAirSet_Vol,  vSystem_SetFreAir(psSystem, psBMS->System.usFreAirSet_Vol))
        
        HANDLE(psBMS->System.usHumidityMin, vSystem_SetHumidity(psSystem, psBMS->System.usHumidityMin, psBMS->System.usHumidityMax))
        HANDLE(psBMS->System.usHumidityMax, vSystem_SetHumidity(psSystem, psBMS->System.usHumidityMin, psBMS->System.usHumidityMax))
        
        HANDLE(psBMS->System.usCO2PPMSet,       vSystem_SetCO2PPM(psSystem, psBMS->System.usCO2PPMSet))
        HANDLE(psBMS->System.usCO2AdjustDeviat, vSystem_SetCO2AdjustDeviat(psSystem, psBMS->System.usCO2AdjustDeviat))
        
        
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
void vSystem_Init(System* pt)
{
    uint8_t n;
    System* pThis = (System*)pt;
    
    ModularRoof*    pModularRoof    = NULL;
    ExAirFan*       pExAirFan       = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    CO2Sensor*      pCO2Sensor      = NULL;
    DTU*            psDTU           = NULL;
    
    vModbusInit();                                  //Modbus初始化
    vSystem_RegistAlarmIO(pThis, SYSTEM_ALARM_DO);  //注册报警接口
    
    pThis->eSystemMode      = MODE_CLOSE;
    pThis->psMBMasterInfo   = psMBGetMasterInfo();
    pThis->sTaskInfo.ucPrio = SYSTEM_POLL_TASK_PRIO;
  
    /*********************DTU模块*************************/
    psDTU = DTU_new(psDTU);
    psDTU->init(psDTU, pThis->psMBMasterInfo);
    
    /*********************主机*************************/
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = (ModularRoof*)ModularRoof_new();
        pModularRoof->init(pModularRoof, pThis->psMBMasterInfo); //初始化
        
        pThis->psModularRoofList[n] = pModularRoof;
    }
    
    /*********************排风风机*************************/
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = (ExAirFan*)ExAirFan_new();  //实例化对象
        pExAirFan->init(pExAirFan, &ExAirFanSet[n]);
        
        if(pExAirFan->eFanFreqType == VARIABLE_FREQ)  
        {
            pThis->pExAirFanVariate = pExAirFan;
        }
        pThis->psExAirFanList[n] = pExAirFan;        
    }
    
    /***********************CO2传感器***********************/
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)CO2Sensor_new();     //实例化对象
        pCO2Sensor->Sensor.init( SUPER_PTR(pCO2Sensor, Sensor),  pThis->psMBMasterInfo); //向上转型，由子类转为父类
        
        pThis->psCO2SenList[n] = pCO2Sensor;
    }
    
    /***********************室外温湿度传感器***********************/
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor),  pThis->psMBMasterInfo);
        
        pThis->psTempHumiSenOutList[n] = pTempHumiSensor; 
    }
    
    /***********************室内温湿度传感器***********************/
    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor),  pThis->psMBMasterInfo);
        
        pThis->psTempHumiSenInList[n] = pTempHumiSensor; 
    }
    
    /***********************消息绑定***********************/
    CONNECT( &(BMS_Core()->sBMSValChange), &pThis->sTaskInfo.sTCB);  //绑定BMS变量变化事件
    
    xSystem_CreatePollTask(pThis); 
}

CTOR(System)   //系统构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init, vSystem_Init);
END_CTOR


System* System_Core()
{
    if(psSystem == NULL)
    {
        psSystem = (System*)System_new();
        psSystem->init(psSystem);
    }
    return psSystem;
}


