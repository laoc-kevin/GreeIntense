#include "sensor.h"

/*************************************************************
*                         传感器                             *
**************************************************************/
void vSensor_init(Sensor* pt, const sMBMasterInfo* psMBMasterInfo)
{
    IDevCom* pThis = SUPER_PTR(pt, IDevCom);
    
    pThis->initCommDevData(pThis);               //初始化设备通讯数据表   
    pThis->registCommDev(pThis, psMBMasterInfo); //向通讯主栈中注册设备
}

void vSensor_registCommDev(IDevCom* pt, const sMBMasterInfo* psMBMasterInfo)
{
    Sensor* pThis = SUB_PTR(pt, IDevCom, Sensor);
    
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


/*************************************************************
*                         CO2传感器                          *
**************************************************************/
void vCO2Sensor_initCommDevData(IDevCom* pt)
{
   Sensor* pThis = SUB_PTR(pt, IDevCom, Sensor);
}

CTOR(CO2Sensor)   //CO2传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(Sensor.IDevCom.initCommDevData, vCO2Sensor_initCommDevData);
END_CTOR


/*************************************************************
*                         温湿度传感器                       *
**************************************************************/
void vTempHumiSensor_initCommDevData(IDevCom* pt)
{
   Sensor* pThis = SUB_PTR(pt, IDevCom, Sensor);
}

CTOR(TempHumiSensor)   //温湿度传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(Sensor.IDevCom.initCommDevData, vTempHumiSensor_initCommDevData);
END_CTOR



