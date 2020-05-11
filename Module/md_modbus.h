#ifndef _MD_MODBUS_H_
#define _MD_MODBUS_H_

#include "mb.h"
#include "mb_m.h"

void vModbusInit(void);

sMBMasterInfo* psGetMBMasterInfo(void);
sMBSlaveInfo*  psGetMBSlaveInfo(void);

#endif
