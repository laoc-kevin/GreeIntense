#ifndef _USER_MB_SCAN_M_H
#define _USER_MB_SCAN_M_H

#include "port.h"
#include "mb_m.h"

BOOL xMBMasterCreateScanSlaveDevTask(sMBMasterInfo* psMBMasterInfo);

void vMBMasterScanSlaveDev(sMBMasterInfo* psMBMasterInfo, sMBSlaveDev* psMBSlaveDev);

void vMBMasterScanReadSlaveDev(sMBMasterInfo* psMBMasterInfo, UCHAR ucSlaveAddr);
void vMBMasterScanWriteSlaveDev(sMBMasterInfo* psMBMasterInfo, UCHAR ucSlaveAddr, UCHAR bCheckPreValue); 

eMBMasterReqErrCode eMBMasterScanReadInputRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

eMBMasterReqErrCode eMBMasterScanReadHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );
eMBMasterReqErrCode eMBMasterScanWriteHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue );

eMBMasterReqErrCode eMBMasterScanReadCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr);
eMBMasterReqErrCode eMBMasterScanWriteCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue);

eMBMasterReqErrCode eMBMasterScanReadDiscreteInputs( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

//eMBMasterReqErrCode eMBMasterSynchronizeDevice( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bSysnRegHold, UCHAR bSysnRegIn, 
//                                                UCHAR bSysnCoil, UCHAR bSysnDiscrete);



#endif