#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "dtu.h"
#include "device.h"
#include "sensor.h"
#include "modularRoof.h"

#define EX_AIR_FAN_NUM       3            //排风机数量
#define MODULAR_ROOF_NUM     2            //屋顶机数量 

#define CO2_SEN_NUM          8            //CO2传感器数量
#define TEMP_HUMI_SEN_OUT_NUM   2        //室外温湿度传感器数量
#define TEMP_HUMI_SEN_IN_NUM    2        //室内温湿度传感器数量

typedef struct   /*系统信息*/
{
    sMBMasterInfo*  psMBMasterInfo;      //通讯主栈
    sMBSlaveInfo*   psMBSlaveInfo;       //系统从栈

    OS_PRIO         ucPrio;;             //内部任务优先级   

}sSystemInfo;



CLASS(System)   /*系统*/
{
    EXTENDS(Unit);         //继承机组抽象类*

    uint16_t         usRetAirSet_Vol;     //回风风量设定
    uint16_t         usFreAirSet_Vol;     //新风风量设定
                                          
    uint16_t         usCO2AdjustThr_V;    //CO2浓度调节阈值
    uint16_t         usCO2AdjustDeviat;   //CO2浓度调节偏差
                                          
    uint16_t         usHumidityMax;       //设定湿度max
    uint16_t         usHumidityMin;       //设定湿度min
                                          
    int16_t          sAmbientIn_T;        //室内环境温度
    uint16_t         usAmbientIn_H;       //室内环境湿度
                                          
    uint8_t          ucCO2SenErr;         //CO2传感器故障
    uint8_t          ucTempHumiSenErr;    //温湿度传感器故障
    
    DTU*             psDTU;               //DTU模块        
    
    ExAirFan*        psExAirFanList[EX_AIR_FAN_NUM];     //排风机列表
    CO2Sensor*       psCO2SenList  [CO2_SEN_NUM];        //CO2传感器列表
    
    ModularRoof*     psModularRoofList[MODULAR_ROOF_NUM];   //屋顶机列表
    
    TempHumiSensor*  psTempHumiSenOutList[TEMP_HUMI_SEN_OUT_NUM];   //室外温湿度传感器列表
    TempHumiSensor*  psTempHumiSenInList[TEMP_HUMI_SEN_OUT_NUM];    //室内温湿度传感器列表
    
    sMBMasterInfo*   psMBMasterInfo;   //通讯主栈
    sTaskInfo        sTaskInfo;        //设备内部任务信息 
    
    void   (*init)(System* pt);

};

System* System_Core();    //获取全局唯一对象，单例设计模式

#endif