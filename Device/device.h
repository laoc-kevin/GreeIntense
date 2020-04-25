#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "lw_oopc.h"
#include "stdint.h"

#include "mb_m.h"
#include "md_output.h"
#include "md_input.h"

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;


typedef enum   /*频率类型*/
{
   VARIABLE_FREQ = 0,     //变频
   CONSTANT_FREQ = 1      //定频
}eFreqType;

typedef struct  /*设备模拟量接口类型*/
{
    int32_t      lMax;            //量程最大值 = 实际量程最大值*10
	int32_t      lMin;            //量程最小值 = 实际量程最小值*10
    uint8_t      ucChannel;       //通道
}sAnalog_IO;

typedef struct  /*设备模拟量接口类型*/
{
    uint8_t      ucChannel;       //通道
}sDigital_IO;



INTERFACE(IDevSwitch)    /*设备启停接口*/
{                                                         
    void     (*switchOpen)(void* pt);        //开启
    void    (*switchClose)(void* pt);        //关闭
    
    void   (*registSwitch_IO)(void* pt, uint8_t ucSwitch_DO);                    //注册启停接口
    void  (*registSwitch_IOs)(void* pt, uint8_t ucOpen_DO, uint8_t ucClose_DO);  //注册开启和关闭接口，针对双接口
};

INTERFACE(IDevRunning)    /*设备运行接口*/
{                                                         
    void    (*registRunState_IO)(void* pt, uint8_t ucRunState_DO);                  //注册运行状态接口
};

INTERFACE(IDevFreq)      /*设备频率接口*/
{
    void         (*setFreq)(void* pt, uint16_t usFreq);                              //设置频率
    void    (*setFreqRange)(void* pt, uint16_t usMinFreq, uint16_t usMaxFreq);       //设置频率上下限

    void   (*registFreq_IO)(void* pt, uint8_t ucFreq_AO, uint8_t ucFreq_AI, uint16_t usMinFreq, uint16_t usMaxFreq); //注册频率接口
};

INTERFACE(IDevCom)      /*设备通讯接口*/
{
    void   (*registCommDev)(void* pt, const sMBMasterInfo* psMBMasterInfo);          //向通讯主栈中注册设备
    void   (*initCommDevData)(void* pt);                                             //初始化设备通讯数据表
};


ABS_CLASS(Device)        /*设备抽象类*/
{
    uint16_t usRunTime;         //运行时间
    uint8_t  ucRunningFlag;     //运行状态标志
    uint8_t  ucErrFlag;         //总故障标志
    uint8_t  ucAlarmFlag;       //总报警标志
};

#endif