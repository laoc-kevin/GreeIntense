#ifndef _FAN_H_
#define _FAN_H_

#include "device.h"

typedef enum   /*系统状态*/
{
    MODE_REAL_TIME   = 0,        //实时新风量 
    MODE_TARGET      = 1,        //目标新风量
}eExAirFanCtrlMode;

typedef struct   /*风机设备信息*/
{
    eFreqType  eFanFreqType;   //频率类型
    
    uint16_t   usMinFreq;      //频率下限 = 实际值*10
    uint16_t   usMaxFreq;      //频率上限 = 实际值*10

    uint8_t    ucFreq_AO;      //频率输出AO通道   
    uint8_t    ucFreq_AI;      //频率输入AI通道
    uint8_t    ucSwitch_DO;    //启停DO通道 
    
}sFanInfo;


CLASS(SupAirFan)          /*送风风机*/ 
{
    EXTENDS(Device);
};

CLASS(ExAirFan)           /*排风风机*/ 
{
    EXTENDS(Device);
    
    IMPLEMENTS(IDevFreq);       //设备频率接口
    IMPLEMENTS(IDevSwitch);     //设备启停接口
    
    eFreqType  eFanFreqType;    //风机频率类型
    
    uint16_t  usRunningFreq;    //运行频率
    uint16_t  usMaxFreq;        //频率上限 = 实际值*10
    uint16_t  usMinFreq;        //频率下限 = 实际值*10
    
    sAnalog_IO   sFreq_AO;      //频率输出AO   
    sAnalog_IO   sFreq_AI;      //频率输入AI
    sDigital_IO  sSwitch_DO;    //启停DO
    
    eCtrlEn      eSwitchCmd;    //启停命令
    uint32_t     ulRated_Vol;   //额定风量

    void (*init)(ExAirFan* pt, sFanInfo* psFan); 
};


#endif
