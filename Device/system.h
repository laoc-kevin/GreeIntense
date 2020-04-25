
#include "device.h"
#include "sensor.h"
#include "modularRoof.h"

CLASS(System)   /*系统*/
{
    EXTENDS(AirUnit);         //继承机组抽象类*
    IMPLEMENTS(IDevSwitch);   //设备启停接口
    IMPLEMENTS(IDevCom);      //设备通讯接口
    
    uint16_t             usRetAirSet_Vol;     //回风风量设定
    uint16_t             usFreAirSet_Vol;     //新风风量设定
                                              
    uint16_t             usCO2AdjustThr_V;    //CO2浓度调节阈值
    uint16_t             usCO2AdjustDeviat;   //CO2浓度调节偏差
                                              
    uint16_t             usHumidityMax;       //设定湿度max
    uint16_t             usHumidityMin;       //设定湿度min
                                              
    int16_t              sAmbientIn_T;        //室内环境温度
    uint16_t             usAmbientIn_H;       //室内环境湿度
                                              
    uint8_t              ucCO2SenErr;         //CO2传感器故障
    uint8_t              ucTempHumiSenErr;    //温湿度传感器故障
         
    uint8_t              ucExAirFanNum;       //排风机数量
    ExAirFan**           psExAirFanList;      //排风机列表
   

    uint8_t              ucModularRoofNum;    //屋顶机数量
    ModularRoof**        psModularRoofList;   //屋顶机列表
    
    uint8_t              ucCO2SenNum;         //CO2传感器数量
    CO2Sensor**          psCO2SenList;        //CO2传感器列表
    
    uint8_t              ucTempHumiSenNum;    //温湿度传感器数量
    TempHumiSensor**     psTempHumiSenList;   //温湿度传感器列表
    
    sMBSlaveDevDataInfo* psDevDataInfo;       //通讯数据表
    sMBSlaveDevInfo*     psDevInfo;           //通讯设备信息
    
    BOOL   (*init)(void* pt, const sMBMasterInfo* psMBMasterInfo, uint8_t ucModularRoofNum, 
                  uint8_t ucExAirFanNum, uint8_t ucCO2SenNum, uint8_t ucTempHumiSenNum);
    
    void (*registDev)(void* pt, void* pvDev, uint8_t ucDevNum, const char* pcDevType);
       
};
