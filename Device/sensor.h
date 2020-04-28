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
    IMPLEMENTS(IDevCom);      //设备通讯接口
    
    eSensorType  eType;                  //传感器类型
    sMBSlaveDevDataInfo* psDevDataInfo;  //通讯数据表
    sMBSlaveDevInfo*     psDevInfo;      //通讯设备信息
    
    void (*init)(Sensor* pt, const sMBMasterInfo* psMBMasterInfo);
};

CLASS(CO2Sensor)          /*CO2传感器*/  
{
    EXTENDS(Sensor);
    
    uint16_t     usMaxPPM;        //量程上限 = 实际值*10
    uint16_t     usMinPPM;        //量程下限 = 实际值*10
    uint16_t     usCO2_PPM;       //实际值    
};

CLASS(TempHumiSensor)          /*温湿度传感器*/  
{
    EXTENDS(Sensor);
    
    uint16_t     usMaxTemp;        //量程上限 = 实际值*10
    uint16_t     usMinTemp;        //量程下限 = 实际值*10
    uint16_t     usTemp;           //实际值  
    
    uint16_t     usMaxHumi;        //量程上限 = 实际值*10
    uint16_t     usMinHumi;        //量程下限 = 实际值*10
    uint16_t     usHumi;           //实际值  

    SIGNAL(D){uint16_t A;} G;
    
};


#endif