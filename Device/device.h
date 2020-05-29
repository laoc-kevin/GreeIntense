#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "lw_oopc.h"
#include "stdint.h"

#include "mb.h"
#include "mb_m.h"
#include "mbmap_m.h"
#include "md_monitor.h"
#include "md_output.h"
#include "md_input.h"
#include "md_eeprom.h"

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#define DATA_INIT(arg_1, arg_2)  arg_1 = arg_2;


typedef enum   /*频率类型*/
{
   VARIABLE_FREQ = 0,     //变频
   CONSTANT_FREQ = 1      //定频
}eFreqType;

typedef enum   /*系统模式*/
{
    MODE_MANUAL    = 1,     
    MODE_AUTO      = 2,
    MODE_CLOSE     = 3,
    MODE_EMERGENCY = 4, 
}eSystemMode;

typedef enum   /*运行状态*/
{
    STATE_STOP   = 0,      //停止
    STATE_RUN    = 1,      //运行
}eRunningState;

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
    void    (*switchOpen)(IDevSwitch* pt);        //开启
    void    (*switchClose)(IDevSwitch* pt);        //关闭
    
};

INTERFACE(IDevFreq)      /*设备频率接口*/
{
    void         (*setFreq)(IDevFreq* pt, uint16_t usFreq);                              //设置频率
    void    (*setFreqRange)(IDevFreq* pt, uint16_t usMinFreq, uint16_t usMaxFreq);       //设置频率上下限

};

INTERFACE(IDevCom)      /*设备通讯接口*/
{
    void   (*initDevCommData)(IDevCom* pt); //初始化设备通讯数据表
};

ABS_CLASS(Device)        /*设备抽象类*/
{
    uint32_t        ulRunTime;       //运行时间(s)
    uint16_t        usRunTime_H;     //运行时间(h)
    
    BOOL            xErrFlag;        //总故障标志
    BOOL            xAlarmFlag;      //总报警标志
    eRunningState   eRunningState;   //运行状态标志
};

#endif