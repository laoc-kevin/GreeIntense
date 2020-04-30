#ifndef _COMM_H_
#define _COMM_H_

#include "mb.h"
#include "device.h"
#include "md_monitor.h"

typedef struct   /*风机设备信息*/
{
    sUART_Def* psMasterUart;
    eMBMode    eMode;
    CHAR*      pcMBPortName;           
    USHORT     usMaxAddr; 
    USHORT     usMinAddr;          
    OS_PRIO    usPrio; 
    BOOL       bDTUEnable;
}sCommMasterInfo;

typedef struct   /*风机设备信息*/
{
    sUART_Def* psSlaveUart;
    eMBMode    eMode;
    OS_PRIO    usPrio;  
    CHAR*      pcMBPortName;  
}sCommSlaveInfo;

CLASS(Comm)
{
    sMBMasterInfo     sMBMasterInfo;
    sMBSlaveInfo      sMBSlaveInfo;
    sMBSlaveDataInfo  sSlaveCurData;

    sMonitorInfo*     pMonitorList;
    
    OS_SEM            sMBValChangeSem; 
    
    void (*init)(Comm* pt, sCommMasterInfo* psMasterInfo, sCommSlaveInfo* psSlaveInfo);
};

#endif