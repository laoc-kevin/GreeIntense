#ifndef _USER_MB_SCAN_M_H
#define _USER_MB_SCAN_M_H

#include "port.h"
#include "mb_m.h"

#define MB_SCAN_SLAVE_DELAY_MS                  20    //主栈扫描从设备

#define MB_SCAN_READ_SLAVE_INTERVAL_MS          20
#define MB_SCAN_WRITE_SLAVE_INTERVAL_MS         20

void vMBScanSlaveDev(sMBMasterInfo* psMBMasterInfo, sMBSlaveDevInfo* psMBSlaveDev);

void vMBScanReadSlaveDev(sMBMasterInfo* psMBMasterInfo, UCHAR iSlaveAddr);
void vMBScanWriteSlaveDev(sMBMasterInfo* psMBMasterInfo, UCHAR iSlaveAddr, UCHAR bCheckPreValue); 

eMBMasterReqErrCode eMBMasterScanReadInputRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

eMBMasterReqErrCode eMBMasterScanReadHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );
eMBMasterReqErrCode eMBMasterScanWriteHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue );

eMBMasterReqErrCode eMBMasterScanReadCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr);
eMBMasterReqErrCode eMBMasterScanWriteCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue);

eMBMasterReqErrCode eMBMasterScanReadDiscreteInputs( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

//eMBMasterReqErrCode eMBMasterSynchronizeDevice( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bSysnRegHold, UCHAR bSysnRegIn, 
//                                                UCHAR bSysnCoil, UCHAR bSysnDiscrete);

#endif