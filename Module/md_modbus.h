#ifndef _MD_MODBUS_H_
#define _MD_MODBUS_H_

#include "mb.h"
#include "mb_m.h"

void vModbusSlaveInit(OS_PRIO prio);

sMBMasterInfo* psMBGetMasterInfo(void);
sMBSlaveInfo*  psMBGetSlaveInfo(void);

#endif
