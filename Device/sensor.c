#include "sensor.h"

/*************************************************************
*                         传感器                             *
**************************************************************/
void vSensor_Init(Sensor* pt, sMBMasterInfo* psMBMasterInfo)
{
    IDevCom* pThis = SUPER_PTR(pt, IDevCom);  
    pt->psMBMasterInfo = psMBMasterInfo; //所属通讯主栈
    
    pThis->initDevCommData(pThis);       //初始化设备通讯数据表   
    pThis->registDev(pThis);             //向通讯主栈中注册设备
}

void vSensor_RegistDev(IDevCom* pt)
{
    Sensor* pThis = SUB_PTR(pt, IDevCom, Sensor);
    (void)xMBMasterRegistDev(pThis->psMBMasterInfo, &pThis->sMBSlaveDev);
}

ABS_CTOR(Sensor)  //传感器抽象类构造函数
    SUPER_CTOR(Device);
    FUNCTION_SETTING(init, vSensor_Init);
    FUNCTION_SETTING(IDevCom.registDev, vSensor_RegistDev);
END_CTOR


/*************************************************************
*                         CO2传感器                          *
**************************************************************/

/*通讯映射函数*/
USHORT usCO2Sensor_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID,  USHORT usAddr)
{

    
    
    
}

/*通讯数据表初始化*/
void vCO2Sensor_InitDevCommData(IDevCom* pt)
{
    Sensor* pThis = SUB_PTR(pt, IDevCom, Sensor);
    
    
    
    pThis->sDevCommData.psMBDevDataMapIndex = usCO2Sensor_DevDataMapIndex;    //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData);
}

CTOR(CO2Sensor)   //CO2传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(Sensor.IDevCom.initDevCommData, vCO2Sensor_InitDevCommData);
END_CTOR


/*************************************************************
*                         温湿度传感器                       *
**************************************************************/

/*通讯映射函数*/
USHORT usTempHumiSensor_DevDataMapIndex(eDataType eDataType, UCHAR ucProtocolID,  USHORT usAddr)
{

    
    
}

/*通讯数据表初始化*/
void vTempHumiSensor_InitDevCommData(IDevCom* pt)
{
    Sensor* pThis = SUB_PTR(pt, IDevCom, Sensor);
    
    
    
    
    
    
    pThis->sDevCommData.psMBDevDataMapIndex = usTempHumiSensor_DevDataMapIndex;    //绑定映射函数
    pThis->sMBSlaveDev.psDevDataInfo = &(pThis->sDevCommData);
}

CTOR(TempHumiSensor)   //温湿度传感器构造函数
    SUPER_CTOR(Sensor);
    FUNCTION_SETTING(Sensor.IDevCom.initDevCommData, vTempHumiSensor_InitDevCommData);
END_CTOR



