#ifndef _MODULAR_ROOF_H_
#define _MODULAR_ROOF_H_

#include "device.h"
#include "fan.h"
#include "compressor.h"

#define   SUP_AIR_FAN_NUM             2    //机组送风机数量
#define   COMP_NUM                    4    //压缩机数量

#define   MODULAR_ROOF_REG_HOLD_NUM   30    //机组通讯数据表保持寄存器数量
#define   MODULAR_ROOF_BIT_COIL_NUM   50    //机组通讯数据表线圈数量

typedef enum   /*系统状态*/
{
    STATE_CLOSE   = 0,        //关机 
    STATE_COOL    = 1,        //制冷
    STATE_FAN     = 2,        //送风
    STATE_HEAT    = 3,        //制热
    STATE_DEFROST = 4,        //化霜
}eSystemState;


CLASS(ModularRoof)   /*屋顶机机组*/
{
    EXTENDS(Device);         /*继承设备抽象类*/ 
    
    IMPLEMENTS(IDevSwitch);     //设备启停接口
    IMPLEMENTS(IDevRunning);    //设备运行接口
    
    uint16_t      usUnitID;                 //机型ID
    uint16_t      usProtocolVer;            //协议版本
                                            
    eSystemState  eSystemState;             //系统状态
    eRunningMode  eRunningMode;             //运行模式设定
                                            
    eCtrlEn       eSwitchCmd;               //启停命令 
    uint8_t       ucSwitchMode;             //启停模式
       
    BOOL          xStopErrFlag;             //停整机故障标志
    
    int16_t       sRetAir_T;                //回风温度
    int16_t       sSupAir_T;                //送风温度  
    
    uint16_t      usRetAir_Vol;             //回风风量
    uint16_t      usSupAir_Vol;             //送风风量
    uint16_t      usFreAir_Vol;             //新风风量
                                       
//    uint16_t      usRetAirDamper;         //回风阀开度设定
//    uint16_t      usFreAirDamper;         //新风阀开度设定                                           
    uint16_t      usRetAirDamper_Ang;       //回风阀当前开度
    uint16_t      usFreAirDamper_Ang;       //新风阀当前开度
      
    uint16_t      usCoolTempSet;            //制冷温度值设定
    uint16_t      usHeatTempSet;            //制热温度值设定
    uint16_t      usFreAirSet_Vol;          //目标新风风量设定

    int16_t       sAmbientIn_T;             //室内环境温度
    int16_t       sAmbientOut_T;            //室外环境温度                                       
    uint16_t      usAmbientIn_H;            //室内环境湿度
    uint16_t      usAmbientOut_H;           //室外环境湿度

    uint16_t      usCO2PPM;                 //CO2平均浓度
    uint16_t      usCO2AdjustThr_V;         //CO2浓度调节阈值
    uint16_t      usCO2AdjustDeviat;        //CO2浓度调节偏差
    
    BOOL          xCO2SenErr;               //CO2传感器故障(群控下发)
    BOOL          xTempSenInErr;            //室内温度传感器故障(群控下发)
    BOOL          xHumiSenInErr;            //室内湿度传感器故障(群控下发)
    BOOL          xTempSenOutErr;           //室外温度传感器故障
    BOOL          xHumiSenOutErr;           //室外湿度传感器故障
    BOOL          xFreAirSenErr;            //新风量传感器故障
    
    uint16_t      usHumidityMax;            //设定湿度max
    uint16_t      usHumidityMin;            //设定湿度min
    
//    SupAirFan*     psSupAirFanList[SUP_AIR_FAN_NUM]; //机组送风机列表
//    Compressor*    psCompList     [COMP_NUM];        //压缩机列表
    
    sMBMasterInfo*       psMBMasterInfo;             //所属通讯主栈
    sMBSlaveDevCommData  sDevCommData;               //本设备通讯数据表
    sMBSlaveDev          sMBSlaveDev;                //本通讯设备
    
    OS_SEM         sModularRoofValChange;            //变量变化事件
    
    sMasterRegHoldData   sModularRoof_RegHoldBuf[MODULAR_ROOF_REG_HOLD_NUM];  //保持寄存器数据域
    sMasterBitCoilData   sModularRoof_BitCoilBuf[MODULAR_ROOF_BIT_COIL_NUM];  //线圈数据域

    void (*init)(ModularRoof* pt, sMBMasterInfo* psMBMasterInfo);
};



#endif