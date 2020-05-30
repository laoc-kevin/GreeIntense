#ifndef _METER_H_
#define _METER_H_

#include "device.h"

#define METER_REG_HOLD_NUM 3

CLASS(Meter)          /*电表*/ 
{
    EXTENDS(Device);
    IMPLEMENTS(IDevCom);          //设备通讯接口
    
    uint16_t   usTotalEnergy_H;  //耗电量(高位) 
    uint16_t   usTotalEnergy_L;  //耗电量(低位)
    
    uint16_t   usPower;          //耗电功率(有功功率)
    
    OS_SEM               sValChange;       //变量变化事件
    
    sMBMasterInfo*       psMBMasterInfo;   //通讯主栈
    sMBSlaveDevCommData  sDevCommData;     //通讯数据表
    sMBSlaveDev          sMBSlaveDev;      //本通讯设备
    
    sMasterRegHoldData   sSensor_RegHoldBuf[METER_REG_HOLD_NUM];  //保持寄存器数据域
    
    
    void (*init)(Meter* pt); 
};

#endif