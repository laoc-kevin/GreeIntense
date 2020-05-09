#ifndef _MODULAR_ROOF_H_
#define _MODULAR_ROOF_H_

#include "device.h"
#include "fan.h"
#include "compressor.h"

#define   SUP_AIR_FAN_NUM       2    //机组送风机数量
#define   COMP_NUM              4    //压缩机数量




ABS_CLASS(Unit)       /*机组抽象类*/ 
{
    EXTENDS(Device);
    
    uint16_t  usUnitID;              //机型ID
    uint16_t  usProtocolVer;         //协议版本
              
    uint8_t   ucSystemState;         //系统状态
    uint8_t   ucFuncState;           //功能设置
    uint8_t   ucRunningMode;         //运行模式
              
    uint8_t   ucSwitchCmd;           //启停命令 
    uint8_t   ucSwitchMode;          //启停模式 
    uint8_t   ucStopErrFlag;         //停系统故障标志
    
};

CLASS(ModularRoof)   /*屋顶机机组*/
{
    EXTENDS(Unit);         /*继承机组抽象类*/ 
    
    IMPLEMENTS(IDevSwitch);     //设备启停接口
    IMPLEMENTS(IDevCom);        //设备通讯接口
    IMPLEMENTS(IDevRunning);    //设备运行接口
    
    int16_t       sRetAir_T;                //回风温度
    int16_t       sSupAir_T;                //送风温度
                                            
    uint16_t      usRetAir_Vol;             //回风风量
    uint16_t      usSupAir_Vol;             //送风风量
    uint16_t      usFreAir_Vol;             //新风风量
                                            
    int16_t       sAmbientIn_T;             //室内环境温度
    int16_t       sAmbientOut_T;            //室外环境温度
                                            
    uint16_t      usAmbientIn_H;            //室内环境湿度
    uint16_t      usAmbientOut_H;           //室外环境湿度
                                            
    uint16_t      usRetAirDamper;           //回风阀开度设定
    uint16_t      usFreAirDamper;           //新风阀开度设定
                                            
    uint16_t      usRetAirDamper_Ang;       //回风阀当前开度
    uint16_t      usFreAirDamper_Ang;       //新风阀当前开度
                                            
    uint16_t      usCO2_PPM;                //CO2平均浓度
    uint16_t      usSysDischarge_T;         //系统排气温度
                  
    SupAirFan*   psSupAirFanList[SUP_AIR_FAN_NUM];   //机组送风机列表
    Compressor*  psCompList     [COMP_NUM];          //压缩机列表
    
    sMBMasterInfo*       psMBMasterInfo;      //所属通讯主栈
    sMBSlaveDevCommData  sDevCommData;        //本设备通讯数据表
    sMBSlaveDev          sMBSlaveDev;         //本通讯设备
    
    void (*init)(ModularRoof* pt, sMBMasterInfo* psMBMasterInfo);
};



#endif