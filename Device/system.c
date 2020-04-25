#include "system.h"

/*****************************屋顶机机组系统****************************/
BOOL xSystem_init(void* pt, const sMBMasterInfo* psMBMasterInfo, uint8_t ucModularRoofNum, 
                  uint8_t ucExAirFanNum, uint8_t ucCO2SenNum, uint8_t ucTempHumiSenNum)
{
    uint8_t n;
    
    System* pThis = (System*)pt;
    
    pThis->ucModularRoofNum = ucModularRoofNum;
    pThis->ucExAirFanNum    = ucExAirFanNum;
    
    pThis->psModularRoofList = (ModularRoof**)calloc(ucModularRoofNum, sizeof(ModularRoof*));    //存储的是对象指针
    pThis->psExAirFanList    = (ExAirFan**)   calloc(ucExAirFanNum, sizeof(ExAirFan*));
    
    pThis->psCO2SenList      = (CO2Sensor**)calloc(ucCO2SenNum, sizeof(CO2Sensor*));    //存储的是对象指针
    pThis->psTempHumiSenList = (TempHumiSensor**)calloc(ucTempHumiSenNum, sizeof(TempHumiSensor*));
    
    if( (pThis->psModularRoofList == NULL) || (pThis->psExAirFanList == NULL) || 
        (pThis->psCO2SenList == NULL) || (pThis->psTempHumiSenList == NULL) )
    {
         return FALSE;
    }
    pThis->IDevCom.initCommDevData(pThis);                 //初始化设备通讯数据表   
    pThis->IDevCom.registCommDev(pThis, psMBMasterInfo);   //向通讯主栈中注册设备
   
    return TRUE;
}

void vSystem_registCommDev(void* pt, const sMBMasterInfo* psMBMasterInfo)
{
    System* pThis = (System*)pt;
    
    if(pThis->psDevDataInfo != NULL)
    {
         pThis->psDevInfo = psMBMasterRegistDev(psMBMasterInfo, pThis->psDevDataInfo);
    }
}

void vSystem_initCommDevData(void* pt)
{
    
}

void vSystem_registDev(void* pt, void* pvDev, uint8_t ucDevNum, const char* pcDevType)
{
    System* pThis = (System*)pt;
    
    if(strcmp(pcDevType, "ModularRoof") == 0)
    {
        ModularRoof* psDev = (ModularRoof*)pvDev;
        if( (ucDevNum <= pThis->ucModularRoofNum) && (ucDevNum > 0) )
        {
             pThis->psModularRoofList[ucDevNum] = psDev;
        }
    }
    else if(strcmp(pcDevType, "ExAirFan") == 0)
    {
        ExAirFan* psDev = (ExAirFan*)pvDev;
        if( (ucDevNum <= pThis->ucExAirFanNum) && (ucDevNum > 0) )
        {
             pThis->psExAirFanList[ucDevNum] = psDev;
        }
    }
    else if(strcmp(pcDevType, "CO2Sensor") == 0)
    {
        CO2Sensor* psDev = (CO2Sensor*)pvDev;
        if( (ucDevNum <= pThis->ucCO2SenNum) && (ucDevNum > 0) )
        {
             pThis->psCO2SenList[ucDevNum] = psDev;
        }
    }
    else if(strcmp(pcDevType, "TempHumiSensor") == 0)
    {
        TempHumiSensor* psDev = (TempHumiSensor*)pvDev;
        if( (ucDevNum <= pThis->ucTempHumiSenNum) && (ucDevNum > 0) )
        {
             pThis->psTempHumiSenList[ucDevNum] = psDev;
        }
    }
}



CTOR(System)   //系统构造函数
    SUPER_CTOR(AirUnit);
    FUNCTION_SETTING(init, xSystem_init);
    FUNCTION_SETTING(IDevCom.registCommDev, vSystem_registCommDev);
    FUNCTION_SETTING(IDevCom.initCommDevData, vSystem_initCommDevData);
END_CTOR