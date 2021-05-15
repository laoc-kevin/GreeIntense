#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "dtu.h"
#include "device.h"
#include "sensor.h"
#include "modularRoof.h"
#include "meter.h"
#include "bms.h"
#include "md_timer.h"

#define EX_AIR_FAN_NUM          4        //排风机数量
#define MODULAR_ROOF_NUM        2        //屋顶机数量 
                                
#define CO2_SEN_NUM             3        //CO2传感器数量
#define TEMP_HUMI_SEN_OUT_NUM   1        //室外温湿度传感器数量
#define TEMP_HUMI_SEN_IN_NUM    12       //室内温湿度传感器数量

CLASS(System)   /*系统*/
{
    EXTENDS(Device);         /*继承设备抽象类*/ 

    uint16_t          usUnitID;                 //机型ID
    uint16_t          usProtocolVer;            //协议版本
    
    eSystemMode       eSystemMode;              //系统模式
    eSystemState      eSystemState;             //系统状态
    eRunningMode      eRunningMode;             //运行模式                                            
    
    
    int16_t           sChickenGrowDays;         //鸡生长周期天数
                      
    uint16_t          usModeChangeTime_1;       //模式切换时间t1(min)
    uint16_t          usModeChangeTime_2;       //模式切换时间t2(min)
    uint16_t          usModeChangeTime_3;       //模式切换时间t3(min)
    uint16_t          usModeChangeTime_4;       //模式切换时间t4(min)
    uint16_t          usModeChangeTime_5;       //模式切换时间t5(min)
    uint16_t          usModeChangeTime_6;       //模式切换时间t6(min)
    uint16_t          usModeChangeTime_7;       //模式切换时间t7(min)
    uint16_t          usModeChangeTime_8;       //模式切换时间t8(min)
     
    uint16_t          usModeChangePeriod_1;     //模式切换间隔时间t1(min)
    uint16_t          usModeChangePeriod_2;     //模式切换间隔时间t2(min)
    uint16_t          usModeChangePeriod_3;     //模式切换间隔时间t3(min)
    uint16_t          usModeChangePeriod_4;     //模式切换间隔时间t4(min)
    uint16_t          usModeChangePeriod_5;     //模式切换间隔时间t5(min)
    uint16_t          usModeChangePeriod_6;     //模式切换间隔时间t6(min)
    
    uint16_t          usModeAdjustTemp_0;       //模式调节温度T0
    uint16_t          usModeAdjustTemp_1;       //模式调节温度T1
    uint16_t          usModeAdjustTemp_2;       //模式调节温度T2
    uint16_t          usModeAdjustTemp_3;       //模式调节温度T3
    uint16_t          usModeAdjustTemp_4;       //模式调节温度T4
    uint16_t          usModeAdjustTemp_5;       //模式调节温度T5
    uint16_t          usModeAdjustTemp_6;       //模式调节温度T6

    uint16_t          usTempSet;                //目标温度值设定
    uint16_t          usEnergyTemp;             //节能温度
    uint16_t          usTempDeviat;             //温度偏差
    uint16_t          usSupAirMax_T;            //送风最大温度  
    
    uint16_t          usCO2PPM;                 //CO2平均浓度
    uint16_t          usCO2AdjustThr_V;         //CO2浓度调节阈值
    uint16_t          usCO2AdjustDeviat;        //CO2浓度调节偏差
    
    uint16_t          usCO2ExAirDeviat_1;       //CO2浓度排风控制偏差1
    uint16_t          usCO2ExAirDeviat_2;       //CO2浓度排风控制偏差2
    uint16_t          usCO2PPMAlarm;            //CO2浓度报警值
     
    uint32_t          ulExAirRequest_Vol;       //系统排风需求量
    uint32_t          ulFreAirSet_Vol;          //系统目标新风风量设定
    uint32_t          ulTotalFreAir_Vol;        //系统新风风量 
    uint32_t          ulExAirFanRated_Vol;      //排风机额定风量
    
    uint16_t          usHumidityMax;            //设定湿度max
    uint16_t          usHumidityMin;            //设定湿度min
                                                
    int16_t           sAmbientIn_T;             //室内环境干球温度
    int16_t           sAmbientOut_T;            //室外环境干球温度                                        
    int16_t           sAmbientIn_Ts;            //室内环境湿球温度
    int16_t           sAmbientOut_Ts;           //室外环境湿球温度                                           
    uint16_t          usAmbientIn_H;            //室内环境湿度
    uint16_t          usAmbientOut_H;           //室外环境湿度
    
    uint16_t          usExAirFanRunTimeLeast;   //排风机最小运行时间  
    uint16_t          usExAirFanFreq;           //排风机频率设定    
    uint16_t          usExAirFanMinFreq;        //排风机最小频率
    uint16_t          usExAirFanMaxFreq;        //排风机最大频率
     
    uint32_t          ulExAirFanRequestTime;    //排风机运行需求时间    
    uint16_t          usExAirFanCtrlPeriod;     //排风机控制周期
    
    uint8_t           ucExAirCoolRatio;         //制冷排风百分比
    uint8_t           ucExAirHeatRatio;         //制热排风百分比
    uint8_t           ucConstantFanRequestNum;  //定频排风机需求个数
    uint8_t           ucConstantFanOpenNum;     //定频排风机常开个数
    BOOL              xVariableFanRequest;      //变频排风机需求

    eExAirFanType     eExAirFanType;            //排风机类型(0: 全变频  1：变频+定频)
    
    OS_TMR            sSysPollTmr;              //系统周期轮询定时器 
    OS_TMR            sExAirFanCtrlTmr;         //排风机控制周期定时器
    OS_TMR            sExAirFanRequestTimeTmr;  //排风机运行需求时间定时器
    
    OS_TMR            sModeChangeTmr_1;         //模式切换时间t1(min)定时器
    OS_TMR            sModeChangeTmr_2;         //模式切换时间t2(min)定时器
    OS_TMR            sModeChangeTmr_3;         //模式切换时间t3(min)定时器
    OS_TMR            sModeChangeTmr_4;         //模式切换时间t4(min)定时器
    OS_TMR            sModeChangeTmr_5;         //模式切换时间t5(min)定时器
    OS_TMR            sModeChangeTmr_6;         //模式切换时间t6(min)定时器
    OS_TMR            sModeChangeTmr_7;         //模式切换时间t7(min)定时器
    OS_TMR            sModeChangeTmr_8;         //模式切换时间t8(min)定时器
    
    OS_TMR            sModeChangePeriodTmr_1;   //模式切换间隔t1(min)定时器
    OS_TMR            sModeChangePeriodTmr_2;   //模式切换间隔t2(min)定时器
    OS_TMR            sModeChangePeriodTmr_3;   //模式切换间隔t3(min)定时器
    OS_TMR            sModeChangePeriodTmr_4;   //模式切换间隔t4(min)定时器
    OS_TMR            sModeChangePeriodTmr_5;   //模式切换间隔t5(min)定时器
    OS_TMR            sModeChangePeriodTmr_6;   //模式切换间隔t6(min)定时器
   
    BOOL              xCompFirstRun;            //压缩机首次开启
    BOOL              xUnitErrFlag;             //机组总故障标志
    BOOL              xExFanErrFlag;            //排风机总故障标志
    BOOL              xAlarmEnable;             //声光报警使能
    BOOL              xAlarmClean;              //声光报警清除
    BOOL              xCO2SenErr;               //CO2传感器故障
    
    BOOL              xTempHumiSenOutErr;       //室外温湿度传感器故障
    BOOL              xTempHumiSenInErr;        //室内温湿度传感器故障
    
    BOOL              xTempSenOutErr;           //室外温度传感器故障
    BOOL              xTempSenInErr;            //室内温度传感器故障
    
    BOOL              xHumiSenOutErr;           //室外湿度传感器故障
    BOOL              xHumiSenInErr;            //室内湿度传感器故障
    
    BOOL              xExAirFanErrClean;        //排风机控制故障清除
    
    sDigital_IO       sAlarm_DO;                //声光报警DO
    
#if MB_MASTER_DTU_ENABLED > 0     
    DTU*              psDTU;                    //DTU模块
#endif
    BMS*              psBMS;                    //BMS接口  
      
    Meter*            pUnitMeter;               //机组电表  
    Meter*            pExAirFanMeter;           //排风机电表  
    
    ExAirFan*         pExAirFanCostant;                              //定频风机    
    ExAirFan*         pExAirFanVariate;                              //变频风机
    ExAirFan*         psExAirFanList[EX_AIR_FAN_NUM];                //排风机列表
    CO2Sensor*        psCO2SenList  [CO2_SEN_NUM];                   //CO2传感器列表
                                                                     
    ModularRoof*      psModularRoofList[MODULAR_ROOF_NUM];           //屋顶机列表                 
    TempHumiSensor*   psTempHumiSenOutList[TEMP_HUMI_SEN_OUT_NUM];   //室外温湿度传感器列表
    TempHumiSensor*   psTempHumiSenInList[TEMP_HUMI_SEN_IN_NUM];     //室内温湿度传感器列表
                      
    sMBMasterInfo*    psMBMasterInfo;   //通讯主栈
    sMBSlaveInfo*     psMBSlaveInfo;    //通讯从栈

    void   (*init)(System* pt);
};

void vSystemInit(OS_TCB* psEventPollTaskTCB, OS_PRIO ucEventPollTaskPrio, CPU_STK* psEventPollTaskStkBase, CPU_STK_SIZE usEventPollTaskStkSize,
                 OS_TCB* psPollTaskTCB, OS_PRIO ucPollTaskPrio, CPU_STK* psPollTaskStkBase, CPU_STK_SIZE usPollTaskStkSize);

System* System_Core();    //获取全局唯一对象，单例设计模式

#endif