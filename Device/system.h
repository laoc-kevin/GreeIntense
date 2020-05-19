#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "dtu.h"
#include "device.h"
#include "sensor.h"
#include "modularRoof.h"

#include "md_timer.h"

#define EX_AIR_FAN_NUM       4            //排风机数量
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
    EXTENDS(Device);         /*继承设备抽象类*/ 

    eSystemMode      eSystemMode;              //系统模式
                                               
    eSystemState     eSystemState;             //系统状态
    eRunningMode     eRunningMode;             //运行模式设定
                                               
    uint8_t          ucSwitchCmd;              //启停命令 
    uint8_t          ucSwitchMode;             //启停模式 
    uint8_t          ucStopErrFlag;            //停系统故障标志
    
    uint16_t         usChickenNum;             //小鸡数量
    uint8_t          ucChickenGrowDays;        //鸡生长周期天数
    
    uint8_t          ucModeChangeTime_1;       //模式切换时间t1(min)
    uint8_t          ucModeChangeTime_2;       //模式切换时间t2(min)
    uint8_t          ucModeChangeTime_3;       //模式切换时间t3(min)
    uint8_t          ucModeChangeTime_4;       //模式切换时间t4(min)
    uint8_t          ucModeChangeTime_5;       //模式切换时间t5(min)
    uint8_t          ucModeChangeTime_6;       //模式切换时间t6(min)
                                               
    int16_t          sTargetTemp;              //目标温度值设定
    int16_t          sGrowUpTemp;              //小鸡适宜生长温度值
    int16_t          sAdjustModeTemp;          //模式调节温度
                                               
    uint16_t         usCO2PPM;                 //CO2平均浓度
    uint16_t         usCO2PPMSet;              //CO2平均浓度设定
    uint16_t         usCO2AdjustThr_V;         //CO2浓度调节阈值
    uint16_t         usCO2AdjustDeviat;        //CO2浓度调节偏差
    uint16_t         usCO2PPMDeviat;           //CO2浓度偏差
    
    uint16_t         usTargetFreAir_Vol;       //目标新风风量    
    uint16_t         usExAirSet_Vol;           //系统排风风量设定 
    uint16_t         usFreAirSet_Vol;          //系统新风风量设定
                                                                                          
    uint16_t         usHumidityMax;            //设定湿度max
    uint16_t         usHumidityMin;            //设定湿度min
                                               
    int16_t          sAmbientIn_T;             //室内环境干球温度
    int16_t          sAmbientOut_T;            //室外环境干球温度                                        
    int16_t          sAmbientIn_Ts;            //室内环境湿球温度
    int16_t          sAmbientOut_Ts;           //室外环境湿球温度                                           
    uint16_t         usAmbientIn_H;            //室内环境湿度
    uint16_t         usAmbientOut_H;           //室外环境湿度
    
    uint8_t          ucAmbientInDeviat_T;      //室内环境温度偏差
    uint8_t          ucAmbientOutDeviat_T;     //室外环境温度偏差
    uint8_t          ucAmbientInDeviat_H;      //室内环境湿度偏差
    uint8_t          ucAmbientOutDeviat_H;     //室外环境湿度偏差

    uint16_t         usExAirFanMinFreq;        //排风机最小频率
    uint16_t         usExAirFanMaxFreq;        //排风机最大频率
    uint32_t         ulExAirFanRated_Vol;      //排风机额定风量
    
    uint16_t         usExAirFanFreqAdjustTime; //排风机频率调节时间
    uint16_t         usExAirFanPreventTime;    //排风机防频繁调节时间
    uint16_t         usExAirFanTestTime;       //风量检测稳定时间
    
    uint16_t         usExAirFanCtrlTime;       //排风机控制周期 
    uint8_t          ucExAirRatio_1;           //排风百分比
    
    TIMER_HANDLE     sExAirFanFreqAdjustTmr;  //排风机频率调节时间定时器
    TIMER_HANDLE     sExAirFanPreventTmr;     //排风机防频繁调节时间定时器
    TIMER_HANDLE     sExAirFanTestTmr;        //风量检测稳定时间定时器
    
    uint8_t          ucExAirFanRequstNum;      //排风机需求个数
    
    eExAirFanCtrlMode eExAirFanCtrlMode;       //排风机控制模式
                                               
    uint8_t          ucAlarmEnable;            //声光报警使能                                                                        
    uint8_t          ucCO2SenErr;              //CO2传感器故障
    uint8_t          ucTempHumiSenErr;         //温湿度传感器故障
                                               
    DTU*             psDTU;                    //DTU模块        
    
    ExAirFan*        pExAirFanVariate;                   //变频风机
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