#ifndef _COMM_H_
#define _COMM_H_

#include "mb.h"
#include "device.h"

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
    sMBSlaveDataInfo* psSlaveCurData;
    
}sCommSlaveInfo;

CLASS(Comm)
{
    sMBMasterInfo*    psMBMasterInfo;
    sMBSlaveInfo*     psMBSlaveInfo;
    
    sMBSlaveDataInfo* psSlaveCurData;
    
    OS_SEM            sMBValChangeSem;  

    void (*init)(Comm* pt, sCommMasterInfo* psMasterInfo, sCommSlaveInfo* psSlaveInfo);
};

#endif