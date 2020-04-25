#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "device.h"

typedef enum   /*传感器类型*/
{
   TYPE_IO = 0,     //IO传输
   TYPE_COM = 1     //通讯
}eSensorType;


ABS_CLASS(Sensor)          /*传感器*/ 
{
    EXTENDS(Device);
    eSensorType  eType;        //传感器类型
    uint16_t     usMax;        //量程上限 = 实际值*10
    uint16_t     usMin;        //量程下限 = 实际值*10
    int32_t      lValue;       //实际值
};

CLASS(SensorCO2)          /*CO2传感器*/  
{
    EXTENDS(Sensor);
    IMPLEMENTS(IDevCom);      //设备通讯接口
    
    sMBSlaveDevDataInfo* psDevDataInfo;  //通讯数据表
    sMBSlaveDevInfo*     psDevInfo;      //通讯设备信息
    
    void (*init)(void* pt, const sMBMasterInfo* psMBMasterInfo);
    
};


#endif