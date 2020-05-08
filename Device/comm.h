#ifndef _COMM_H_
#define _COMM_H_

#include "mb.h"
#include "device.h"
#include "system.h"
#include "md_monitor.h"

typedef struct   /*系统通讯配置信息*/
{
    sMBMasterInfo*  psMBMasterInfo;      //通讯主栈
    sMBSlaveInfo*   psMBSlaveInfo;       //系统从栈
    
    System*         psSystem;            //系统

}sCommInfo;


CLASS(Comm)
{
    sMBMasterInfo*   psMBMasterInfo;

    sMBSlaveInfo*    psMBSlaveInfo;       //系统从栈
    sMBSlaveCommData sSlaveCommData;      //系统从栈通讯数据表
    
    System*          psSystem;            //系统
    
    OS_SEM           sMBValChangeSem; 
    
    void   (*init)(Comm* pt, sCommInfo* psComm);

};

#endif