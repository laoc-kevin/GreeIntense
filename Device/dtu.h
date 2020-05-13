#ifndef _DTU_H
#define _DTU_H

#include "mbconfig.h"

#ifdef MB_MASTER_DTU_ENABLED     //GPRS模块功能支持

#include "device.h"
#include "mb_m.h"

CLASS(DTU)   /*DTU模块*/
{
    USHORT*             psDTUInitCmd;
    USHORT*             psDTUInitedCmd;
                        
    OS_TMR              DTUTimerTimeout;    
    UCHAR               ucDTUInited;
                        
    sMBMasterInfo*      psMBMasterInfo;   //通讯主栈
    
    sMBSlaveDev         sDevDTU247;
    sMBSlaveDev         sDevDTU200;   
    
    sMBSlaveDevCommData sDevDataDTU247;    //DTU247数据域
    sMBSlaveDevCommData sDevDataDTU200;    //DTU200数据域
    
    BOOL (*init)(DTU* pt, sMBMasterInfo* psMBMasterInfo);
    
    void (*scanDev)(void* pt);
};


#endif

#endif