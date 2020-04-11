#ifndef _USER_MB_MAP_
#define _USER_MB_MAP_

#include "mbframe.h"
#include "mb.h"

eMBErrorCode eMBScanRegInMap( USHORT usAddress, sMBRegData ** pvRegInValue);

eMBErrorCode eMBScanRegHoldMap( USHORT usRegHoldAddr, sMBRegData** pvRegHoldValue);

eMBErrorCode eMBScanCoilsMap( USHORT usCoilAddr, sMBBitData** pvCoilValue);

eMBErrorCode eMBScanDiscreteMap( USHORT usDiscreteAddr, sMBBitData** pvDiscreteValue);

eMBErrorCode eMBScanCPNMap( USHORT usCpnName, sValCPNData ** pvCPNValue);

void eMBSlaveTableInit( void );

#endif