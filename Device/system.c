#include "bms.h"
#include "system.h"
#include "md_event.h"
#include "md_modbus.h"
#include "md_timer.h"

/*************************************************************
*                         系统                               *
**************************************************************/
#define SYSTEM_POLL_TASK_PRIO    9

static System* psSystem = NULL;

/*系统排风机配置信息*/
sFanInfo ExAirFanSet[EX_AIR_FAN_NUM] = { {CONSTANT_FREQ, 0, 0, 0, 0, 1},
                                         {CONSTANT_FREQ, 0, 0, 0, 0, 2},
                                         {CONSTANT_FREQ, 0, 0, 0, 0, 3},
                                       };



/*系统开启*/
void vSystem_SwitchOpen(System* pt)
{
    System* pThis = (System*)pt;
    
    if(pThis->eSystemMode)
    {
        
    }
    
    
    
    
    
    
    pThis->ucSwitchCmd = ON; 
}

/*系统关闭*/
void vSystem_SwitchClose(System* pt)
{
    System* pThis = (System*)pt;
    pThis->ucSwitchCmd = OFF; 
}

/*设定系统模式*/
void vSystem_SetRunningMode(System* pt, eRunningMode eRunMode)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    
    if(pThis->eRunningMode != eRunMode)
    {
        pThis->eRunningMode = eRunMode;  
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n]; 
            pModularRoof->eRunningMode = pThis->eRunningMode;
        }
    }
}

/*/调整系统模式*/
void vSystem_AdjustRunningMode(void* p_arg)
{
    System* pThis = (System*)p_arg;
    
    //若t2(默认5分钟)内温度没有达到目标温度t(ng1)±1.5℃
    if(pThis->sAmbientIn_T > pThis->sTargetTemp + 15)  
    {
        if(pThis->eRunningMode == RUN_MODE_FAN)
        {
            vSystem_SetRunningMode(pThis, RUN_MODE_WET);    //切换为湿膜降温模式
        }
        else if(pThis->eRunningMode == RUN_MODE_WET)
        {
            vSystem_SetRunningMode(pThis, RUN_MODE_COOL);    //切换为降温模式
        }
    }
}


/*切换系统模式*/
void vSystem_ChangeRunningMode(System* pt)
{
    System* pThis = (System*)pt;
    
    /*注：室外干球温度: t(wg)   室外湿球温度: t(ws)      室内目标干球温度: t(ng1)   室内实际干球温度: t(ng2)
          养殖鸡数量：n         运行当天目标新风量:G
    */

     //制冷工况:  室内干球温度t(ng2)＞舍内温度目标要求温度t(ng1)，开启制冷工况
    if(pThis->sAmbientIn_T > pThis->sTargetTemp)  
    {
        //A. 舍内温度目标要求t(ng1)≥鸡生适宜长温度（默认25℃）
        if(pThis->sTargetTemp >= pThis->sGrowUpTemp) 
        {
            //(1)室外干球温度t(wg)≤模式调节温度（默认23℃）
            if(pThis->sAmbientOut_T <= pThis->sAdjustModeTemp)  
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_FAN);    //开启送风模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_1 * 60, vSystem_AdjustRunningMode, pThis);
            }
            
            //(2)室外干球温度t(wg)＞模式调节温度（默认23℃） 且t（ws）+3 <= t（ng1）-(3.6×n×1.7×6×0.5)/（G×1.2×2）
            if( (pThis->sAmbientOut_T > pThis->sAdjustModeTemp) &&  
                (pThis->sAmbientOut_Ts + 30) <= pThis->sTargetTemp-(76.5f * pThis->usChickenNum) / pThis->usFreAirSet_Vol )   
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_WET);    //开启湿膜降温模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_2 * 60, vSystem_AdjustRunningMode, pThis);
            }
            
            //(3)室外干球温度t(wg)＞模式调节温度（默认23℃），且t（ws）+3＞t（ng1）-(3.6×n×1.7×6×0.5)/（G×1.2×2）
            if( (pThis->sAmbientOut_T > pThis->sAdjustModeTemp) &&  
                (pThis->sAmbientOut_Ts + 30) > pThis->sTargetTemp-(76.5f * pThis->usChickenNum) / pThis->usFreAirSet_Vol )   
            {
                vSystem_SetRunningMode(pThis, RUN_MODE_COOL);    //开启降温模式
                (void)sTimerRegist(TIMER_ONE_SHOT, pThis->ucModeChangeTime_3 * 60, vSystem_AdjustRunningMode, pThis);
            }
        } 

        //B. 舍内温度目标要求t(ng1)≥鸡生适宜长温度（默认25℃）
        if(pThis->sTargetTemp < pThis->sGrowUpTemp) 
        {
            
            
        }  
    }
    
    
    //制热工况:  室内干球温度t(ng2) <= 舍内温度目标要求温度t(ng1)，开启制热工况
    if(pThis->sAmbientIn_T <= pThis->sTargetTemp)     
    {
        pThis->eRunningMode = RUN_MODE_HEAT;    
    }  
}




/*设定系统目标温度值*/
void vSystem_SetTemp(System* pt, int16_t sTargetTemp)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->sTargetTemp = sTargetTemp;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->sTargetTemp = pThis->sTargetTemp;
    }
}

/*设定系统目标新风量*/
void vSystem_SetFreAir(System* pt, uint16_t usFreAirSet_Vol)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->usFreAirSet_Vol = usFreAirSet_Vol;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->usFreAirSet_Vol = pThis->usFreAirSet_Vol / MODULAR_ROOF_NUM;
    }
}

;
/*设定系统湿度阈值*/
void vSystem_SetHumidity(System* pt, uint16_t usHumidityMin, uint16_t usHumidityMax)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->usHumidityMin = usHumidityMin;
    pThis->usHumidityMax = usHumidityMax;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->usHumidityMin = pThis->usHumidityMin;
        pModularRoof->usHumidityMax = pThis->usHumidityMax;
    }
}


/*设定系统目标CO2浓度值*/
void vSystem_SetCO2PPM(System* pt, uint16_t usCO2PPMSet)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->usCO2PPMSet = usCO2PPMSet;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->usCO2PPM = pThis->usCO2PPMSet;
    }
}

/*设定系统CO2浓度偏差*/
void vSystem_SetCO2AdjustDeviat(System* pt, uint16_t usCO2AdjustDeviat)
{
    uint8_t  n = 0; 
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    pThis->usCO2AdjustDeviat = usCO2AdjustDeviat;
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n]; 
        pModularRoof->usCO2AdjustDeviat = pThis->usCO2AdjustDeviat;
    }
}

/*系统CO2浓度变化*/
void vSystem_CO2PPM(System* pt)
{
    uint8_t  n = 0; 
    uint16_t usTotalCO2PPM = 0;   
    
    System* pThis = (System*)pt;
    
    ModularRoof* pModularRoof = NULL;
    CO2Sensor*   pCO2Sensor   = NULL;
    
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)pThis->psCO2SenList[n];     
        usTotalCO2PPM = usTotalCO2PPM + pCO2Sensor->usAvgCO2PPM; 
    }
    if( (usTotalCO2PPM / CO2_SEN_NUM) != pThis->usCO2PPM)
    {
        pThis->usCO2PPM = usTotalCO2PPM / CO2_SEN_NUM;  //CO2平均浓度
    }
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->usCO2PPM = pThis->usCO2PPM;
    }
}

/*系统室外温湿度变化*/
void vSystem_TempHumiOut(System* pt)
{
    uint8_t  n = 0; 
    int16_t  sTotalTemp = 0;
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenOutList[n];     
        sTotalTemp  = sTotalTemp + pTempHumiSensor->sAvgTemp;
        usTotalHumi = usTotalHumi + pTempHumiSensor->usAvgHumi;      
    }
    if( (sTotalTemp / TEMP_HUMI_SEN_OUT_NUM) != pThis->sAmbientOut_T)
    {
        pThis->sAmbientOut_T = sTotalTemp / TEMP_HUMI_SEN_OUT_NUM;  //室外平均环境温度
    }
     if( (usTotalHumi / TEMP_HUMI_SEN_OUT_NUM) != pThis->usAmbientOut_H)
    {
        pThis->usAmbientOut_H = usTotalHumi / TEMP_HUMI_SEN_OUT_NUM;  //室外平均环境湿度
    }
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->sAmbientOut_T  = pThis->sAmbientOut_T;
        pModularRoof->usAmbientOut_H = pThis->usAmbientOut_H;
    }
}

/*系统室内温湿度变化*/
void vSystem_TempHumiIn(System* pt)
{
    uint8_t  n = 0; 
    int16_t  sTotalTemp = 0;
    uint16_t usTotalHumi = 0;  
    
    System* pThis = (System*)pt;
    
    ModularRoof*       pModularRoof = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    
    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenInList[n];     
        sTotalTemp  = sTotalTemp + pTempHumiSensor->sAvgTemp;
        usTotalHumi = usTotalHumi + pTempHumiSensor->usAvgHumi;      
    }
    if( (sTotalTemp / TEMP_HUMI_SEN_IN_NUM) != pThis->sAmbientIn_T)
    {
        pThis->sAmbientIn_T = sTotalTemp / TEMP_HUMI_SEN_OUT_NUM;  //室内平均环境温度
    }
     if( (usTotalHumi / TEMP_HUMI_SEN_OUT_NUM) != pThis->usAmbientIn_H)
    {
        pThis->usAmbientIn_H = usTotalHumi / TEMP_HUMI_SEN_OUT_NUM;  //室内平均环境湿度
    }
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        pModularRoof->sAmbientIn_T  = pThis->sAmbientIn_T;
        pModularRoof->usAmbientIn_H = pThis->usAmbientIn_H;
    }
}

/*系统内部消息轮询*/
void vSystem_PollTask(void *p_arg)
{
    CPU_TS            ts = 0;
    OS_MSG_SIZE  msgSize = 0;

    OS_ERR           err = OS_ERR_NONE;
    
    BMS* psBMS = BMS_Core();
    
    while(DEF_TRUE)
	{
        sMsg* psMsg = (sMsg*)OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, &ts, &err);
        if( (USHORT*)psMsg->pvArg == (USHORT*)(&psBMS->System.sAmbientIn_T) )  //查看是哪个变量发生变化
        {
            
            
        }
      
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
    
    vModbusInit();
    
    pThis->psMBMasterInfo   = psMBGetMasterInfo();
    pThis->sTaskInfo.ucPrio = SYSTEM_POLL_TASK_PRIO;
  
    psDTU = DTU_new(psDTU);
    psDTU->init(psDTU, pThis->psMBMasterInfo);
    
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = (ModularRoof*)ModularRoof_new();
        pModularRoof->init(pModularRoof, pThis->psMBMasterInfo); //初始化
        
        pThis->psModularRoofList[n] = pModularRoof;
    }
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = (ExAirFan*)ExAirFan_new();  //实例化对象
        pExAirFan->init(pExAirFan, &ExAirFanSet[n]);
        
        pThis->psExAirFanList[n] = pExAirFan;  
    }
    
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)CO2Sensor_new();     //实例化对象
        pCO2Sensor->Sensor.init( SUPER_PTR(pCO2Sensor, Sensor),  pThis->psMBMasterInfo); //向上转型，由子类转为父类
        
        pThis->psCO2SenList[n] = pCO2Sensor;
    }
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor),  pThis->psMBMasterInfo);
        
        pThis->psTempHumiSenOutList[n] = pTempHumiSensor; 
    }
    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor),  pThis->psMBMasterInfo);
        
        pThis->psTempHumiSenInList[n] = pTempHumiSensor; 
    }
    
    
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


