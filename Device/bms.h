#ifndef _BMS_H_
#define _BMS_H_

#include "mb.h"
#include "device.h"
#include "md_monitor.h"

#define   EX_AIR_FAN_NUM     4
#define   BMS_REG_HOLD_NUM   200    //BMS通讯数据表保持寄存器数量
#define   BMS_BIT_COIL_NUM   200    //BMS通讯数据表线圈数量

CLASS(BMS)
{
    eSystemMode       eSystemMode;              //系统模式
    eRunningMode      eRunningMode;             //运行模式 
    
    uint16_t          usTempSet;                //目标温度值设定 
    uint16_t          usHumidityMax;            //设定湿度max
    uint16_t          usHumidityMin;            //设定湿度min
    
    uint16_t          usFreAirSet_Vol_H;        //系统目标新风风量设定(高位)
    uint16_t          usFreAirSet_Vol_L;        //系统目标新风风量设定(低位)

    uint16_t          usCO2AdjustThr_V;         //CO2浓度调节阈值
    uint16_t          usCO2AdjustDeviat;        //CO2浓度调节偏差
    
    uint8_t           ucExAirCoolRatio;         //制冷排风百分比
    uint8_t           ucExAirHeatRatio;         //制热排风百分比
    
    eExAirFanType     eExAirFanType;            //排风机类型(0: 全变频  1：变频+定频)
    uint16_t          usExAirFanFreq;           //排风机频率设定    
    uint16_t          usExAirFanMinFreq;        //排风机最小频率
    uint16_t          usExAirFanMaxFreq;        //排风机最大频率
    
    uint16_t          usExAirFanRated_Vol_H;    //排风机额定风量(高位)
    uint16_t          usExAirFanRated_Vol_L;    //排风机额定风量(低位)
    
    uint16_t          usTotalFreAir_Vol_H;      //系统新风风量(高位) 
    uint16_t          usTotalFreAir_Vol_L;      //系统新风风量(高位)
    
    uint16_t          usUnitTotalEnergy_H;      //机组总耗电量(高位) 
    uint16_t          usUnitTotalEnergy_L;      //机组总耗电量(高位)
    
    uint16_t          usExAirFanTotalEnergy_H;  //排风机总耗电量(高位) 
    uint16_t          usExAirFanTotalEnergy_L;  //排风机总耗电量(高位)
    
    uint16_t          usUnitPower;              //机组耗电功率
    uint16_t          usExAirFanPower;          //排风机耗电功率
    
    eCtrlEn           eExAirFanCmdList[EX_AIR_FAN_NUM];
    
    BOOL              xAlarmClean;              //声光报警清除
    BOOL              xAlarmEnable;             //声光报警使能
    
    sMBSlaveInfo*     psBMSInfo;          //从栈接口
    sMBSlaveCommData  sBMSCommData;       //系统从栈通讯数据表
    
    sMBSlaveRegData   sBMS_RegHoldBuf[BMS_REG_HOLD_NUM];  //保持寄存器数据域
    sMBSlaveBitData   sBMS_BitCoilBuf[BMS_BIT_COIL_NUM];  //线圈数据域
    
    OS_SEM            sValChange; 
    
    void   (*init)(BMS* pt);
};

BMS* BMS_Core(void);   //获取全局唯一对象，单例设计模式
void BMS_InitDefaultData(BMS* pt);
    
#endif