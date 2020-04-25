#include "sensor.h"

/*****************************传感器抽象类****************************/
void vSensor_init(void* pt, const sMBMasterInfo* psMBMasterInfo)
{
    Sensor* pThis = (Sensor*)pt;
    
    pThis->IDevCom.initCommDevData(pThis);                 //初始化设备通讯数据表   
    pThis->IDevCom.registCommDev(pThis, psMBMasterInfo);   //向通讯主栈中注册设备
}

void vSensor_registCommDev(void* pt, const sMBMasterInfo* psMBMasterInfo)
{
    Sensor* pThis = (Sensor*)pt;
    
    if(pThis->psDevDataInfo != NULL)
    {
         pThis->psDevInfo = psMBMasterRegistDev(psMBMasterInfo, pThis->psDevDataInfo);
    }
}

ABS_CTOR(Sensor)  //传感器抽象类构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init, vSensor_init);
    FUNCTION_SETTING(IDevCom.registCommDev, vSensor_registCommDev);
END_CTOR


/*****************************CO2传感器****************************/
void vCO2Sensor_initCommDevData(void* pt)
{
    
}

CTOR(CO2Sensor)   //CO2传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(Sensor.IDevCom.initCommDevData, vCO2Sensor_initCommDevData);
END_CTOR



/*****************************温湿度传感器****************************/
void vTempHumiSensor_initCommDevData(void* pt)
{
    
}

CTOR(TempHumiSensor)   //CO2传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(Sensor.IDevCom.initCommDevData, vTempHumiSensor_initCommDevData);
END_CTOR



