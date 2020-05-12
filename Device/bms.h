#ifndef _BMS_H_
#define _BMS_H_

#include "mb.h"
#include "device.h"
#include "md_monitor.h"

#define   BMS_REG_HOLD_NUM   30    //机组通讯数据表保持寄存器数量
#define   BMS_BIT_COIL_NUM   50    //机组通讯数据表线圈数量




CLASS(BMS)
{
    sMBSlaveInfo*     psBMSInfo;          //从栈接口
    
    sMBSlaveCommData  sBMSCommData;      //系统从栈通讯数据表
    
    sMBSlaveRegData   sBMS_RegHoldBuf[BMS_REG_HOLD_NUM];  //保持寄存器数据域
    sMBSlaveBitData   sBMS_BitCoilBuf[BMS_BIT_COIL_NUM];  //线圈数据域
    
    OS_SEM            sMBValChangeSem; 
    
    void   (*init)(BMS* pt, sMBSlaveInfo* psBMSInfo, void* pData);
};

#endif