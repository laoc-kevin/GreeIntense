#ifndef _MD_MODBUS_H_
#define _MD_MODBUS_H_

#include "mb.h"
#include "mb_m.h"
#include "mb_m.h"
#include "app_config.h"

void vModbusMasterInit(OS_PRIO ucHeartPrio, OS_PRIO ucPollPrio, OS_PRIO ucScanPrio);
sMBMasterInfo* psMBGetMasterInfo(void);
                             
void vModbusSlaveInit(OS_PRIO prio);
sMBSlaveInfo*  psMBGetSlaveInfo(void);

BOOL vModbusSlaveTCPInit(OS_PRIO prio, OS_PRIO pollprio);
sMBSlaveInfo* psMBGetTCPSlaveInfo(void);    

void  vModbusMasterTCPInit(OS_PRIO ucPollPrio, OS_PRIO ucScanReadPrio, OS_PRIO ucScanWritePrio);
sMBMasterInfo*  psMBGetTCPMasterInfo(void);

BOOL xMasterRegistSlaveDev(sMBMasterInfo* psMBMasterInfo, sMBSlaveDev* psMBNewDev);
void vRegistSlaveCommData(sMBSlaveInfo* psMBSlaveInfo, sMBSlaveCommData *psCommData);
#endif
