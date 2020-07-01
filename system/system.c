#include "bms.h"
#include "system.h"
#include "systemctrl.h"

#include "md_event.h"
#include "md_modbus.h"
#include "md_timer.h"

#include "stdlib.h"
/*************************************************************
*                         系统                               *
**************************************************************/
#define SYSTEM_ALARM_DO          13       //系统声光报警DO接口

#define TMR_TICK_PER_SECOND      OS_CFG_TMR_TASK_RATE_HZ
#define RUNNING_TIME_OUT_S       1
#define SYSTEM_POLL_INTERVAL_S   10

#define MODE_CHANGE_TIME         1
#define MODE_CHANGE_PERIOD       3
#define MODE_ADJUST_TEMP         15

#define HANDLE(p_arg1, p_arg2) if( (void*)psMsg->pvArg == (void*)(&p_arg1) )\
         {p_arg2; myprintf("**********************HANDLE*********************\n");goto begin;}

System*  psSystem = NULL;
System   SystemCore;
        
OS_PRIO       SysEventPollTaskPrio    = 4;
CPU_STK_SIZE  SysEventPollTaskStkSize = 128;
                       
OS_TCB*       psSysEventPollTaskTCB = NULL;
CPU_STK*      psSysEventPollTaskStk = NULL;

/*系统排风机配置信息*/
const sFanInfo ExAirFanVariate = {VARIABLE_FREQ, MIN_FAN_FREQ, MAX_FAN_FREQ, 1, 1, 1, 1, 11};

const sFanInfo ExAirFanSet[EX_AIR_FAN_NUM] = { {CONSTANT_FREQ, 0, 0, 0, 0, 1, 1, 5},
                                               {CONSTANT_FREQ, 0, 0, 0, 0, 2, 2, 6},
                                               {CONSTANT_FREQ, 0, 0, 0, 0, 3, 3, 7},
                                               {CONSTANT_FREQ, 0, 0, 0, 0, 4, 4, 8},
                                             };
/*系统排风机类型切换*/
void vSystem_ChangeExAirFanType(System* pt, eExAirFanType eExAirFanType)
{
    System*   pThis     = (System*)pt; 
    ExAirFan* pExAirFan = pThis->psExAirFanList[0];
    pThis->eExAirFanType = eExAirFanType;
    
    if(eExAirFanType == TYPE_CONSTANT_VARIABLE)     //定频 + 变频
    {
        pExAirFan->changeFreqType(pExAirFan, &ExAirFanVariate);
        pThis->pExAirFanVariate = pExAirFan; 
    }
    else   //定频
    {
        pExAirFan->changeFreqType(pExAirFan, &ExAirFanSet[0]);
        pThis->pExAirFanVariate = NULL;
    }  
#if DEBUG_ENABLE > 0        
    myprintf("vSystem_ChangeExAirFanType  eExAirFanType %d  usRunningFreq %d\n",  
             pThis->eExAirFanType, pThis->pExAirFanVariate->usRunningFreq);
#endif           
}

/*系统周期轮询*/
void vSystem_PollTask(void *p_arg)
{
    OS_ERR err = OS_ERR_NONE;
    psSystem = System_Core();
    
    while (DEF_TRUE)
    {
        (void)OSTimeDlyHMSM(0, 0, SYSTEM_POLL_INTERVAL_S, 0, OS_OPT_TIME_HMSM_STRICT, &err);
        if(psSystem->eSystemState != STATE_CLOSED)
        {
//            myprintf("********************vSystem_PollTask***********************\n");
            vSystem_ChangeUnitRunningMode(psSystem);
        }            
    }           
}

/*系统内部消息轮询*/
void vSystem_EventPollTask(void *p_arg)
{
    uint8_t n;
    
    OS_MSG_SIZE  msgSize = 0;
    OS_ERR           err = OS_ERR_NONE;
    
    System* pThis = (System*)p_arg;
    
    ModularRoof*    pModularRoof    = NULL;
    ExAirFan*       pExAirFan       = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    CO2Sensor*      pCO2Sensor      = NULL;
    sEventMsg*      psMsg           = NULL;
    BMS*            psBMS           = NULL;
    
    while(xEEPROMDataIsReady() == FALSE)
    {
        OSTimeDlyHMSM(0, 0, 1, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
    psSystem = System_Core();
    psBMS    = BMS_Core();
    
    BMS_InitDefaultData(psBMS);
    
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan =  pThis->psExAirFanList[n];
        pExAirFan->Device.usRunTime_H = pExAirFan->Device.ulRunTime_S/3600;
        
#if DEBUG_ENABLE > 0 
        myprintf("pExAirFan %d ulRunTime_S %ld  usRunTime_H %d \n", n, pExAirFan->Device.ulRunTime_S, pExAirFan->Device.usRunTime_H);
#endif
    }
   
    while(DEF_TRUE)
	{ 
begin:       
        psMsg = (sEventMsg*)OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, NULL, &err);
        if(psMsg == NULL || err != OS_ERR_NONE)
        {
            continue;
        }
        /***********************BMS事件响应***********************/
        HANDLE(psBMS->eSystemMode,  vSystem_ChangeSystemMode(psSystem, psBMS->eSystemMode))  
        HANDLE(psBMS->eRunningMode, vSystem_SetUnitRunningMode(psSystem, psBMS->eRunningMode)) 
        HANDLE(psBMS->xAlarmClean,  vSystem_CleanAlarm(psSystem, &psBMS->xAlarmClean)) 
        HANDLE(psBMS->xAlarmEnable, vSystem_AlarmEnable(psSystem, psBMS->xAlarmEnable)) 
        
        HANDLE(psBMS->usTempSet,         vSystem_SetTemp(psSystem, psBMS->usTempSet))
        HANDLE(psBMS->usFreAirSet_Vol_H, vSystem_SetFreAir(psSystem, psBMS->usFreAirSet_Vol_H, psBMS->usFreAirSet_Vol_L))
        HANDLE(psBMS->usFreAirSet_Vol_L, vSystem_SetFreAir(psSystem, psBMS->usFreAirSet_Vol_H, psBMS->usFreAirSet_Vol_L))
        
        HANDLE(psBMS->ucExAirCoolRatio, vSystem_ExAirRatio(psSystem, psBMS->ucExAirCoolRatio, psBMS->ucExAirHeatRatio))
        HANDLE(psBMS->ucExAirHeatRatio, vSystem_ExAirRatio(psSystem, psBMS->ucExAirCoolRatio, psBMS->ucExAirHeatRatio))
        
        HANDLE(psBMS->usHumidityMin, vSystem_SetHumidity(psSystem, psBMS->usHumidityMin, psBMS->usHumidityMax))
        HANDLE(psBMS->usHumidityMax, vSystem_SetHumidity(psSystem, psBMS->usHumidityMin, psBMS->usHumidityMax))
        
        HANDLE(psBMS->usCO2AdjustThr_V,  vSystem_SetCO2AdjustThr_V(psSystem, psBMS->usCO2AdjustThr_V))
        HANDLE(psBMS->usCO2AdjustDeviat, vSystem_SetCO2AdjustDeviat(psSystem, psBMS->usCO2AdjustDeviat))
        
        HANDLE(psBMS->usExAirFanFreq,    vSystem_AdjustExAirFanFreq(psSystem, psBMS->usExAirFanFreq))
        HANDLE(psBMS->usExAirFanMinFreq, vSystem_SetExAirFanFreqRange(psSystem, psBMS->usExAirFanMinFreq, psBMS->usExAirFanMaxFreq))
        HANDLE(psBMS->usExAirFanMaxFreq, vSystem_SetExAirFanFreqRange(psSystem, psBMS->usExAirFanMinFreq, psBMS->usExAirFanMaxFreq))
        
        HANDLE(psBMS->eExAirFanType, vSystem_ChangeExAirFanType(psSystem, psBMS->eExAirFanType))
        
        HANDLE(psBMS->usExAirFanRated_Vol_H, vSystem_SetExAirFanRated(psSystem, psBMS->usExAirFanRated_Vol_H, psBMS->usExAirFanRated_Vol_L))                                                                     
        HANDLE(psBMS->usExAirFanRated_Vol_L, vSystem_SetExAirFanRated(psSystem, psBMS->usExAirFanRated_Vol_H, psBMS->usExAirFanRated_Vol_L))

        /***********************主机事件响应***********************/
        for(n=0; n < MODULAR_ROOF_NUM; n++)
        {
            pModularRoof = pThis->psModularRoofList[n]; 
            
            HANDLE(pModularRoof->Device.eRunningState, vSystem_DeviceRunningState(psSystem))
            HANDLE(pModularRoof->eRunningMode,         vSystem_DeviceRunningState(psSystem))
            
            HANDLE(pModularRoof->sSupAir_T,    vSystem_UnitSupAirTemp(psSystem, pModularRoof))
            HANDLE(pModularRoof->usFreAir_Vol, vSystem_UnitFreAir(psSystem))            
 
            HANDLE(pModularRoof->xStopErrFlag, vSystem_UnitErr(psSystem))
            HANDLE(pModularRoof->xCommErr,     vSystem_UnitErr(psSystem))
            
            HANDLE(pModularRoof->sAmbientInSelf_T,  vSystem_UnitTempHumiIn(psSystem))
            HANDLE(pModularRoof->usAmbientInSelf_H, vSystem_UnitTempHumiIn(psSystem))
            
            HANDLE(pModularRoof->sAmbientOutSelf_T,  vSystem_UnitTempHumiOut(psSystem))
            HANDLE(pModularRoof->usAmbientOutSelf_H, vSystem_UnitTempHumiOut(psSystem))
            
            HANDLE(pModularRoof->usCO2PPMSelf, vSystem_UnitCO2PPM(psSystem))
        }

        /***********************排风机事件响应***********************/
        for(n=0; n < EX_AIR_FAN_NUM; n++)  
        {
            pExAirFan =  pThis->psExAirFanList[n];
            
            HANDLE(pExAirFan->xExAirFanErr,         vSystem_ExAirFanErr(psSystem))
            HANDLE(pExAirFan->Device.eRunningState, vSystem_DeviceRunningState(psSystem))
        }

        /***********************CO2传感器事件响应***********************/
        for(n=0; n < CO2_SEN_NUM; n++)
        {
            pCO2Sensor = (CO2Sensor*)pThis->psCO2SenList[n];
            
            HANDLE(pCO2Sensor->usAvgCO2PPM, vSystem_CO2PPM(psSystem)) 
            HANDLE(pCO2Sensor->xCO2SenErr,  vSystem_CO2SensorErr(psSystem))
        }
        
        /***********************室外温湿度传感器事件响应***********************/
        for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
        {
            pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenOutList[n];
            
            HANDLE(pTempHumiSensor->sAvgTemp,    vSystem_TempHumiOut(psSystem)) 
            HANDLE(pTempHumiSensor->xTempSenErr, vSystem_TempHumiOutErr(psSystem))
            
            HANDLE(pTempHumiSensor->usAvgHumi,   vSystem_TempHumiOut(psSystem)) 
            HANDLE(pTempHumiSensor->xHumiSenErr, vSystem_TempHumiOutErr(psSystem))
        }
        
        /***********************室内温湿度传感器事件响应***********************/
        for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
        {
            pTempHumiSensor = (TempHumiSensor*)pThis->psTempHumiSenInList[n];  

            HANDLE(pTempHumiSensor->sAvgTemp,    vSystem_TempHumiIn(psSystem))                 
            HANDLE(pTempHumiSensor->usAvgHumi,   vSystem_TempHumiIn(psSystem))
            HANDLE(pTempHumiSensor->xTempSenErr, vSystem_TempHumiInErr(psSystem))            
            HANDLE(pTempHumiSensor->xHumiSenErr, vSystem_TempHumiInErr(psSystem))
        }
        myprintf("******************************OSTaskOver******************************\n");
    }
}

/*创建系统内部消息轮询任务*/
BOOL xSystem_CreatePollTask(System* pt)
{
    OS_ERR    err = OS_ERR_NONE;
    System* pThis = (System*)pt;

    err = eTaskCreate(psSysEventPollTaskTCB, vSystem_EventPollTask, pThis, SysEventPollTaskPrio, 
                      psSysEventPollTaskStk, SysEventPollTaskStkSize);   
    return (err == OS_ERR_NONE);              
}

void vSystem_RuntimeTmrCallback(void * p_tmr, void * p_arg)
{
    uint8_t n;
    ModularRoof*    pModularRoof    = NULL;
    ExAirFan*       pExAirFan       = NULL;
    
     System* pThis = (System*)p_arg;
    
    /*********************主机运行时间*************************/
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = pThis->psModularRoofList[n];
        if(pModularRoof->Device.eRunningState == STATE_RUN && pModularRoof->Device.ulRunTime_S < UINT32_MAX)
        {
            pModularRoof->Device.ulRunTime_S++;
            pModularRoof->Device.usRunTime_H = pModularRoof->Device.ulRunTime_S / 3600;
        }
        if(pModularRoof->Device.ulRunTime_S == UINT32_MAX)
        {
            pModularRoof->Device.ulRunTime_S = 0;
        }
    }
    /*********************排风风机运行时间*************************/
    for(n=0; n < EX_AIR_FAN_NUM; n++)  
    {
        pExAirFan = pThis->psExAirFanList[n];
        if(pExAirFan->Device.eRunningState == STATE_RUN && pExAirFan->Device.ulRunTime_S < UINT32_MAX)
        {
            pExAirFan->Device.ulRunTime_S++;
            pExAirFan->Device.usRunTime_H = pExAirFan->Device.ulRunTime_S / 3600;
        }
        if(pExAirFan->Device.ulRunTime_S == UINT32_MAX)
        {
            pExAirFan->Device.ulRunTime_S = 0;
        }        
    }
}
    
/*系统运行时间初始化*/
void vSystem_InitRuntimeTmr(System* pt)
{
    System* pThis = (System*)pt; 
    (void)xTimerRegist(&pThis->sRuntimeTmr, 0, RUNNING_TIME_OUT_S, OS_OPT_TMR_PERIODIC, vSystem_RuntimeTmrCallback, pThis, FALSE);                       
}
 
/*系统EEPROM数据注册*/
void vSystem_RegistEEPROMData(System* pt)
{
    System* pThis = (System*)pt;
    
    EEPROM_DATA(TYPE_UINT_8, pThis->ucExAirCoolRatio)
    EEPROM_DATA(TYPE_UINT_8, pThis->ucExAirHeatRatio)
    EEPROM_DATA(TYPE_UINT_8, pThis->eExAirFanType)
    EEPROM_DATA(TYPE_UINT_8, pThis->xAlarmEnable)
    
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangeTime_1)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangeTime_2)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangeTime_3)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangeTime_4)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangeTime_5)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangeTime_6)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangeTime_7)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangeTime_8)
    
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangePeriod_1)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangePeriod_2)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangePeriod_3)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangePeriod_4)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangePeriod_5)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeChangePeriod_6)
    
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeAdjustTemp_0)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeAdjustTemp_1)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeAdjustTemp_2)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeAdjustTemp_3)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeAdjustTemp_4)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeAdjustTemp_5)
    EEPROM_DATA(TYPE_UINT_16, pThis->usModeAdjustTemp_6)
    
    EEPROM_DATA(TYPE_UINT_16, pThis->usTempSet)
    EEPROM_DATA(TYPE_UINT_16, pThis->usEnergyTemp)
    EEPROM_DATA(TYPE_UINT_16, pThis->usTempDeviat)
    EEPROM_DATA(TYPE_UINT_16, pThis->usSupAirMax_T)
    
    EEPROM_DATA(TYPE_UINT_16, pThis->usCO2AdjustThr_V)
    EEPROM_DATA(TYPE_UINT_16, pThis->usCO2AdjustDeviat)
    EEPROM_DATA(TYPE_UINT_16, pThis->usCO2PPMAlarm)
    EEPROM_DATA(TYPE_UINT_16, pThis->ulFreAirSet_Vol)
    
    EEPROM_DATA(TYPE_UINT_16, pThis->usHumidityMax)
    EEPROM_DATA(TYPE_UINT_16, pThis->usHumidityMin)
    
    EEPROM_DATA(TYPE_UINT_16, pThis->usExAirFanFreq)
    EEPROM_DATA(TYPE_UINT_16, pThis->usExAirFanMinFreq)
    EEPROM_DATA(TYPE_UINT_16, pThis->usExAirFanMaxFreq)
    EEPROM_DATA(TYPE_UINT_16, pThis->usExAirFanRunTimeLeast)
    EEPROM_DATA(TYPE_UINT_16, pThis->usExAirFanCtrlPeriod)
   
    EEPROM_DATA(TYPE_INT_16, pThis->sChickenGrowDays)
    
    EEPROM_DATA(TYPE_UINT_32, pThis->ulExAirFanRated_Vol)
}

/*系统数据默认值初始化*/
void vSystem_InitDefaultData(System* pt)
{
    System* pThis = (System*)pt;
    
    DATA_INIT(pThis->usUnitID,        0x302A)
    DATA_INIT(pThis->usProtocolVer,       10)
    DATA_INIT(pThis->eSystemMode,    MODE_CLOSE)
    DATA_INIT(pThis->eRunningMode, RUN_MODE_COOL)

    DATA_INIT(pThis->sChickenGrowDays,  -2)
    DATA_INIT(pThis->usEnergyTemp,     250)
    DATA_INIT(pThis->usTempDeviat,       5)
    DATA_INIT(pThis->usSupAirMax_T,    450)
    DATA_INIT(pThis->usTempSet,        260)
    
    DATA_INIT(pThis->usHumidityMin,    55)
    DATA_INIT(pThis->usHumidityMax,    65)
    
    DATA_INIT(pThis->usCO2AdjustThr_V,  2700)
    DATA_INIT(pThis->usCO2AdjustDeviat,  270)
    DATA_INIT(pThis->usCO2ExAirDeviat_1, 270)
    DATA_INIT(pThis->usCO2ExAirDeviat_2, 270)
    DATA_INIT(pThis->usCO2PPMAlarm,     3000)
    
    DATA_INIT(pThis->ulFreAirSet_Vol,  60000)
    
    DATA_INIT(pThis->eExAirFanType, TYPE_CONSTANT_VARIABLE)
    DATA_INIT(pThis->usExAirFanFreq,     MIN_FAN_FREQ)
    DATA_INIT(pThis->usExAirFanMinFreq,  MIN_FAN_FREQ)
    DATA_INIT(pThis->usExAirFanMaxFreq,  MAX_FAN_FREQ)
    DATA_INIT(pThis->ucExAirCoolRatio,    90)
    DATA_INIT(pThis->ucExAirHeatRatio,    90)
    
    DATA_INIT(pThis->usExAirFanRunTimeLeast,  300)
    DATA_INIT(pThis->usExAirFanCtrlPeriod,   1800)
    DATA_INIT(pThis->ulExAirFanRated_Vol,   36000)
    
    DATA_INIT(pThis->usModeChangeTime_1, MODE_CHANGE_TIME)
    DATA_INIT(pThis->usModeChangeTime_2, MODE_CHANGE_TIME)
    DATA_INIT(pThis->usModeChangeTime_3, MODE_CHANGE_TIME)
    DATA_INIT(pThis->usModeChangeTime_4, MODE_CHANGE_TIME)
    DATA_INIT(pThis->usModeChangeTime_5, MODE_CHANGE_TIME)
    DATA_INIT(pThis->usModeChangeTime_6, MODE_CHANGE_TIME) 
    DATA_INIT(pThis->usModeChangeTime_7, MODE_CHANGE_TIME)
    DATA_INIT(pThis->usModeChangeTime_8, MODE_CHANGE_TIME)

    DATA_INIT(pThis->usModeChangePeriod_1, MODE_CHANGE_PERIOD)
    DATA_INIT(pThis->usModeChangePeriod_2, MODE_CHANGE_PERIOD)
    DATA_INIT(pThis->usModeChangePeriod_3, MODE_CHANGE_PERIOD)
    DATA_INIT(pThis->usModeChangePeriod_4, MODE_CHANGE_PERIOD)
    DATA_INIT(pThis->usModeChangePeriod_5, MODE_CHANGE_PERIOD)
    DATA_INIT(pThis->usModeChangePeriod_6, MODE_CHANGE_PERIOD)

    DATA_INIT(pThis->usModeAdjustTemp_0, MODE_ADJUST_TEMP)
    DATA_INIT(pThis->usModeAdjustTemp_1, MODE_ADJUST_TEMP)
    DATA_INIT(pThis->usModeAdjustTemp_2, MODE_ADJUST_TEMP)
    DATA_INIT(pThis->usModeAdjustTemp_3, MODE_ADJUST_TEMP)
    DATA_INIT(pThis->usModeAdjustTemp_4, MODE_ADJUST_TEMP)
    DATA_INIT(pThis->usModeAdjustTemp_5, MODE_ADJUST_TEMP)
    DATA_INIT(pThis->usModeAdjustTemp_6, MODE_ADJUST_TEMP)
    
    vSystem_ChangeExAirFanType(pThis, pThis->eExAirFanType);
    vSystem_SetExAirFanFreqRange(pThis, pThis->usExAirFanMinFreq, pThis->usExAirFanMaxFreq);
//    myprintf("vSystem_InitDefaultData usMinFreq %d usMaxFreq %d \n", pThis->psExAirFanList[0]->usMinFreq, pThis->psExAirFanList[0]->usMaxFreq);
}

/*系统初始化*/
void vSystem_Init(System* pt)
{
    uint8_t n, ucDevAddr;
    System* pThis = (System*)pt;
    
    ModularRoof*    pModularRoof    = NULL;
    ExAirFan*       pExAirFan       = NULL;
    TempHumiSensor* pTempHumiSensor = NULL;
    CO2Sensor*      pCO2Sensor      = NULL;
    
#if MB_MASTER_DTU_ENABLED > 0 
    DTU*            psDTU           = NULL;
#endif 
    
    pThis->psMBMasterInfo = psMBGetMasterInfo();    //主栈
    if(pThis->psMBMasterInfo == NULL)
    {
        return;
    }
    ucDevAddr =  pThis->psMBMasterInfo->sMBDevsInfo.ucSlaveDevMinAddr;
    
    vSystem_RegistAlarmIO(pThis, SYSTEM_ALARM_DO);  //注册报警接口
    
#if MB_MASTER_DTU_ENABLED  > 0   //GPRS模块功能支持    
    psDTU = DTU_new(psDTU);
    if(psDTU != NULL)
    {
        psDTU->init(psDTU, pThis->psMBMasterInfo);
    }
#endif 
    
    /*实例化各设备对象时，要特别注意防止内存溢出，要根据设备多少调整堆区大小，
      堆区大小通过修改startup_LPC407x_8x.s 文件中的Heap_Size*/
                     
    /*********************主机*************************/
    for(n=0; n < MODULAR_ROOF_NUM; n++)
    {
        pModularRoof = (ModularRoof*)ModularRoof_new();
        
        if(pModularRoof != NULL)
        {
            pModularRoof->init(pModularRoof, pThis->psMBMasterInfo, ucDevAddr++, n); //初始化
            pThis->psModularRoofList[n] = pModularRoof;
        
            CONNECT( &pModularRoof->sValChange, psSysEventPollTaskTCB);  //绑定主机变量变化事件     
        } 
    }
    /*********************排风风机*************************/
    for(n=0; n < EX_AIR_FAN_NUM; n++)
    {
        pExAirFan = (ExAirFan*)ExAirFan_new();  //实例化对象        
        if(pExAirFan != NULL)
        {
            pExAirFan->init(pExAirFan, &ExAirFanSet[n], n);
            pThis->psExAirFanList[n] = pExAirFan; 
            
            CONNECT( &pExAirFan->sValChange, psSysEventPollTaskTCB);  //绑定风机变量变化事件  
        }     
    }
    /***********************CO2传感器***********************/
    for(n=0; n < CO2_SEN_NUM; n++)
    {
        pCO2Sensor = (CO2Sensor*)CO2Sensor_new();     //实例化对象
        if(pCO2Sensor != NULL)
        {
            pCO2Sensor->Sensor.init( SUPER_PTR(pCO2Sensor, Sensor),  pThis->psMBMasterInfo, TYPE_CO2, ucDevAddr++, n); //向上转型，由子类转为父类
            pThis->psCO2SenList[n] = pCO2Sensor;
            
            CONNECT( &pCO2Sensor->Sensor.sValChange, psSysEventPollTaskTCB);  //绑定传感器变量变化事件
        }
    }
    /***********************室外温湿度传感器***********************/
    for(n=0; n < TEMP_HUMI_SEN_OUT_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        if(pTempHumiSensor != NULL)
        {
            pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor),  pThis->psMBMasterInfo, TYPE_TEMP_HUMI_OUT, ucDevAddr++, n);
            pThis->psTempHumiSenOutList[n] = pTempHumiSensor;
            
            CONNECT( &pTempHumiSensor->Sensor.sValChange, psSysEventPollTaskTCB);  //绑定传感器变量变化事件 
        }          
    }
    /***********************室内温湿度传感器***********************/
    for(n=0; n < TEMP_HUMI_SEN_IN_NUM; n++)
    {
        pTempHumiSensor = (TempHumiSensor*)TempHumiSensor_new();
        if(pTempHumiSensor != NULL)
        {
            pTempHumiSensor->Sensor.init( SUPER_PTR(pTempHumiSensor, Sensor),  pThis->psMBMasterInfo, TYPE_TEMP_HUMI_IN, ucDevAddr++, n);
            pThis->psTempHumiSenInList[n] = pTempHumiSensor; 
            
            CONNECT( &pTempHumiSensor->Sensor.sValChange, psSysEventPollTaskTCB);  //绑定传感器变量变化事件    
        }
    } 
//    /*********************电表*************************/
//    pThis->pUnitMeter     = (Meter*)Meter_new();     //机组电表  
//    pThis->pExAirFanMeter = (Meter*)Meter_new();     //排风机电表 

//    pThis->pUnitMeter->init(pThis->pUnitMeter, pThis->psMBMasterInfo, ucDevAddr++);
//    pThis->pExAirFanMeter->init(pThis->pExAirFanMeter, pThis->psMBMasterInfo, ucDevAddr++);    
    
    (void)xSystem_CreatePollTask(pThis);
    CONNECT( &(BMS_Core()->sValChange), psSysEventPollTaskTCB);  // 绑定BMS变量变化事件
    
    vSystem_InitDefaultData(pThis);
    vSystem_RegistEEPROMData(pThis);
    
    vSystem_InitRuntimeTmr(pThis);
    pThis->psBMS = (BMS*)BMS_Core();
    
    myprintf("vSystem_Init \n");   
}

CTOR(System)   //系统构造函数
    SUPER_CTOR(Device);
END_CTOR


void vSystemInit(OS_TCB* psEventPollTaskTCB, OS_PRIO ucEventPollTaskPrio, CPU_STK* psEventPollTaskStkBase, CPU_STK_SIZE usEventPollTaskStkSize)
{
    SysEventPollTaskPrio    = ucEventPollTaskPrio;
    SysEventPollTaskStkSize = usEventPollTaskStkSize;
    
    psSysEventPollTaskTCB = psEventPollTaskTCB;
    psSysEventPollTaskStk = psEventPollTaskStkBase;
    
    System_Core();
}

System* System_Core()
{
    if(psSystem == NULL)
    { 
        psSystem = (System*)&SystemCore;
        if(psSystem != NULL)
        {
            vSystem_Init(psSystem);
        }  
    }
    return psSystem;
}


