#ifndef _USER_MB_SCAN_M_H
#define _USER_MB_SCAN_M_H

#include "port.h"
#include "mb_m.h"



BOOL xMBMasterCreateScanSlaveDevTask(sMBMasterInfo* psMBMasterInfo);

void vMBMasterScanSlaveDev(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev);

void vMBMasterScanReadSlaveDev(sMBMasterInfo* psMBMasterInfo, UCHAR iSlaveAddr);
void vMBMasterScanWriteSlaveDev(sMBMasterInfo* psMBMasterInfo, UCHAR iSlaveAddr, UCHAR bCheckPreValue); 

eMBMasterReqErrCode eMBMasterScanReadInputRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

eMBMasterReqErrCode eMBMasterScanReadHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );
eMBMasterReqErrCode eMBMasterScanWriteHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue );

eMBMasterReqErrCode eMBMasterScanReadCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr);
eMBMasterReqErrCode eMBMasterScanWriteCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue);

eMBMasterReqErrCode eMBMasterScanReadDiscreteInputs( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

//eMBMasterReqErrCode eMBMasterSynchronizeDevice( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bSysnRegHold, UCHAR bSysnRegIn, 
//                                                UCHAR bSysnCoil, UCHAR bSysnDiscrete);



#endif