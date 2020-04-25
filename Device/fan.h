#ifndef _FAN_H_
#define _FAN_H_

#include "device.h"



ABS_CLASS(Fan)          /*风机抽象类*/ 
{
    EXTENDS(Device);
    eFreqType  eFanFreqType;      //风机频率类型
  
    void (*init)(void* pt, eFreqType eFanFreqType);
};

CLASS(SupAirFan)          /*送风风机*/ 
{
    EXTENDS(Fan);
};

CLASS(ExAirFan)           /*排风风机*/ 
{
    EXTENDS(Fan);
    
    IMPLEMENTS(IDevFreq);       //设备频率接口
    IMPLEMENTS(IDevSwitch);     //设备启停接口
    
    uint16_t  usRunningFreq;    //运行频率
    uint16_t  usMaxFreq;        //频率上限 = 实际值*10
    uint16_t  usMinFreq;        //频率下限 = 实际值*10
    
    sAnalog_IO   sFreq_AO;      //频率输出AO   
    sAnalog_IO   sFreq_AI;      //频率输入AI
    sDigital_IO  sSwitch_DO;    //启停DO

};


#endif