#ifndef _USER_MB_MAP_H
#define _USER_MB_MAP_H

#include "mb.h"
#include "mbdict.h"

eMBErrorCode 
eMBSlaveRegInMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usAddress, sMBSlaveRegData** pvRegInValue);

eMBErrorCode 
eMBSlaveRegHoldMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usRegHoldAddr, sMBSlaveRegData** pvRegHoldValue);

eMBErrorCode 
eMBSlaveCoilsMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usCoilAddr, sMBSlaveBitData** pvCoilValue);

eMBErrorCode 
eMBSlaveDiscreteMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usDiscreteAddr, sMBSlaveBitData** pvDiscreteValue);

eMBErrorCode 
eMBSlaveCPNMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usCPNName, sMBSlaveCPNData** pvCPNValue);

void eMBSlaveSlaveTableInit( void );

#endif