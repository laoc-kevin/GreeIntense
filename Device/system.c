#include "system.h"
#include "md_event.h"

/*************************************************************
*                         系统                               *
**************************************************************/
/*系统通讯数据表初始化*/
void vSystem_initCommDevData(IDevCom* pt)
{
    System* pThis = SUB_PTR(pt, IDevCom, System);
}

/*向通讯主栈中注册设备*/
void vSystem_registCommDev(IDevCom* pt)
{
    System* pThis = SUB_PTR(pt, IDevCom, System);
    
    if(pThis->psDevDataInfo != NULL)
    {
         (void)xMBMasterRegistDev(pThis->psMBMasterInfo, &pThis->sMBDev);
    }
}

/*系统初始化*/
BOOL xSystem_init(System* pt, sSystemInfo* psSystem)
{
    uint8_t n;
    
    System* pThis = (System*)pt;
    
    pThis->psMBMasterInfo = psSystem->psMBMasterInfo;
    
    pThis->ucModularRoofNum = psSystem->ucModularRoofNum;
    pThis->ucExAirFanNum    = psSystem->ucExAirFanNum;
    
    pThis->psModularRoofList = (ModularRoof**)calloc(psSystem->ucModularRoofNum, sizeof(ModularRoof*));    //存储的是对象指针
    pThis->psExAirFanList    = (ExAirFan**)   calloc(psSystem->ucExAirFanNum, sizeof(ExAirFan*));
    
    pThis->psCO2SenList      = (CO2Sensor**)calloc(psSystem->ucCO2SenNum, sizeof(CO2Sensor*));    //存储的是对象指针
    pThis->psTempHumiSenList = (TempHumiSensor**)calloc(psSystem->ucTempHumiSenNum, sizeof(TempHumiSensor*));
    
    if( (pThis->psModularRoofList == NULL) || (pThis->psExAirFanList == NULL) || 
        (pThis->psCO2SenList == NULL) || (pThis->psTempHumiSenList == NULL) )
    {
         return FALSE;
    }
    vSystem_initCommDevData(&pThis->IDevCom);                            //初始化设备通讯数据表   
    vSystem_registCommDev(&pThis->IDevCom);   //向通讯主栈中注册设备
   
    return TRUE;
}

/*系统开启*/
void vSystem_switchOpen(IDevSwitch* pt)
{
    System* pThis = SUB_PTR(pt, IDevSwitch, System);
    pThis->AirUnit.ucSwitchCmd = ON; 
}

/*系统关闭*/
void vSystem_switchClose(IDevSwitch* pt)
{
     System* pThis = SUB_PTR(pt, IDevSwitch, System);
    pThis->AirUnit.ucSwitchCmd = OFF; 
}

/*向系统注册所控设备*/
void vSystem_registDev(System* pt, void* pvDev, uint8_t ucDevNum, const char* pcDevType)
{
    System* pThis = (System*)pt;
    
    if(strcmp(pcDevType, "ModularRoof") == 0)
    {
        ModularRoof* psDev = (ModularRoof*)pvDev;
        if( (pThis->psModularRoofList != NULL) && (ucDevNum <= pThis->ucModularRoofNum) && (ucDevNum > 0) )
        {
             pThis->psModularRoofList[ucDevNum] = psDev;
        }
    }
    else if(strcmp(pcDevType, "ExAirFan") == 0)
    {
        ExAirFan* psDev = (ExAirFan*)pvDev;
        if( (pThis->psExAirFanList != NULL) && (ucDevNum <= pThis->ucExAirFanNum) && (ucDevNum > 0) )
        {
             pThis->psExAirFanList[ucDevNum] = psDev;
        }
    }
    else if(strcmp(pcDevType, "CO2Sensor") == 0)
    {
        CO2Sensor* psDev = (CO2Sensor*)pvDev;
        if( (pThis->psCO2SenList != NULL) &&(ucDevNum <= pThis->ucCO2SenNum) && (ucDevNum > 0) )
        {
             pThis->psCO2SenList[ucDevNum] = psDev;
        }
    }
    else if(strcmp(pcDevType, "TempHumiSensor") == 0)
    {
        TempHumiSensor* psDev = (TempHumiSensor*)pvDev;
        if( (pThis->psTempHumiSenList != NULL) &&(ucDevNum <= pThis->ucTempHumiSenNum) && (ucDevNum > 0) )
        {
             pThis->psTempHumiSenList[ucDevNum] = psDev;
        }
    }
}

/*系统内部消息轮询*/
void vSystem_pollTask(void *p_arg)
{
    CPU_TS            ts = 0;
    OS_MSG_SIZE  msgSize = 0;
    OS_ERR           err = OS_ERR_NONE;
    
    while(DEF_TRUE)
	{
        sMsg* psMsg = (sMsg*)OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, &ts, &err);
//        switch(psMsg-> == )
       
    }
}

/*创建系统内部消息轮询任务*/
BOOL xSystem_createPollTask(System* pt, OS_PRIO ucPrio, CPU_STK_SIZE ucStackSize)
{
    OS_ERR    err = OS_ERR_NONE;
    System* pThis = (System*)pt;
    
    sDevTaskInfo*  psTaskInfo = (sDevTaskInfo*)calloc(1, sizeof(sDevTaskInfo));
    
    pThis->psTaskInfo = (sDevTaskInfo*)calloc(1, sizeof(sDevTaskInfo));
    
    if(psTaskInfo != NULL)
    {
        psTaskInfo->prio       = ucPrio;
        psTaskInfo->stk_size   = ucStackSize;
        psTaskInfo->p_stk_base = (CPU_STK*)calloc(ucStackSize, sizeof(CPU_STK));
      
        pThis->psTaskInfo = psTaskInfo;
    }
    else
    {
        return FALSE;
    }
    OSTaskCreate(&psTaskInfo->p_tcb,
              "vSystem_pollTask",
              vSystem_pollTask,
              (void*)0,
              psTaskInfo->prio,
              psTaskInfo->p_stk_base ,
              psTaskInfo->stk_size / 10u,
              psTaskInfo->stk_size,
              0u,
              0u,
              0u,
             (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR ),
             &err);
    return (err == OS_ERR_NONE);              
}

CTOR(System)   //系统构造函数
    SUPER_CTOR(AirUnit);
    FUNCTION_SETTING(init, xSystem_init);

    FUNCTION_SETTING(IDevSwitch.switchOpen, vSystem_switchOpen);
    FUNCTION_SETTING(IDevSwitch.switchClose, vSystem_switchClose);

    FUNCTION_SETTING(IDevCom.registCommDev, vSystem_registCommDev);
    FUNCTION_SETTING(IDevCom.initCommDevData, vSystem_initCommDevData);


END_CTOR