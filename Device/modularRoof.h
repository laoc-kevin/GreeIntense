#ifndef _MODULAR_ROOF_H_
#define _MODULAR_ROOF_H_

#include "device.h"
#include "fan.h"
#include "compressor.h"

typedef enum   /*频率类型*/
{
   HEAT_MODE = 0,     //
   COOL_MODE = 1      //定频
}eRunMode;


ABS_CLASS(AirUnit)       /*空调机组抽象类*/ 
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
    
    void (*setRunningMode)(void* pt, eRunMode eRunMode);
    
};

CLASS(ModularRoof)   /*屋顶机机组*/
{
    EXTENDS(AirUnit);         /*继承机组抽象类*/         
    
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
                  
    uint8_t       ucSupAirFanNum;           //机组送风机数量
    SupAirFan**   psSupAirFanList;          //机组送风机列表
    
    uint8_t       ucCompNum;                //压缩机数量
    Compressor**  psCompList;               //压缩机列表
    
    BOOL (*init)(void* pt, uint8_t ucSupAirFanNum, uint8_t ucCompNum);
};



#endif