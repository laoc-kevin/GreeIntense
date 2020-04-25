#include "sensor.h"

ABS_CTOR(Sensor)  //传感器抽象类构造函数
    SUPER_CTOR(Device);
END_CTOR



void vSensorCO2_init(void* pt, const sMBMasterInfo* psMBMasterInfo)
{
    SensorCO2* pThis = (SensorCO2*)pt;
    
    pThis->IDevCom.initCommDevData(pThis);                 //初始化设备通讯数据表   
    pThis->IDevCom.registCommDev(pThis, psMBMasterInfo);   //向通讯主栈中注册设备
}

void vSensorCO2_registCommDev(void* pt, const sMBMasterInfo* psMBMasterInfo)
{
    SensorCO2* pThis = (SensorCO2*)pt;
    
    if(pThis->psDevDataInfo != NULL)
    {
         pThis->psDevInfo = psMBMasterRegistDev(psMBMasterInfo, pThis->psDevDataInfo);
    }
}

void vSensorCO2_initCommDevData(void* pt)
{
    
}

CTOR(SensorCO2)   //CO2传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(init, vSensorCO2_init);
    FUNCTION_SETTING(IDevCom.registCommDev, vSensorCO2_registCommDev);
    FUNCTION_SETTING(IDevCom.initCommDevData, vSensorCO2_initCommDevData);
END_CTOR
