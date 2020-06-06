#ifndef _MD_MODBUS_H_
#define _MD_MODBUS_H_

#include "mb.h"
#include "mb_m.h"

void vModbusSlaveInit(OS_PRIO prio);
void vModbusMasterInit(OS_PRIO ucHeartPrio, OS_PRIO ucPollPrio, OS_PRIO ucScanPrio);
    
sMBMasterInfo* psMBGetMasterInfo(void);
sMBSlaveInfo*  psMBGetSlaveInfo(void);

#endif
