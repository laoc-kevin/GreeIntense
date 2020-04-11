#ifndef _USER_MB_SCAN_M_H
#define _USER_MB_SCAN_M_H

#include "port.h"
#include "mb_m.h"

#define MB_MASTER_WAITING_DELAY          50    //主栈等待响应时间
#define MB_WRITE_SLAVE_DELAY_MS          30     //主栈写操作间隔
#define MB_SCAN_SLAVE_DELAY_MS           20     //主栈扫描从设备
#define MB_WRITE_RETRY_TIMES             1      //写入失败重复次数

typedef struct
{
   volatile USHORT RegValue;
   volatile USHORT*  pRegPreValue;
}sRegHoldPara;

eMBMasterReqErrCode eMBMasterScanReadInputRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

eMBMasterReqErrCode eMBMasterScanReadHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

eMBMasterReqErrCode eMBMasterScanWriteHoldingRegister( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue );

eMBMasterReqErrCode eMBMasterScanReadCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr);

eMBMasterReqErrCode eMBMasterScanWriteCoils( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bCheckPreValue);

eMBMasterReqErrCode eMBMasterScanReadDiscreteInputs( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr );

eMBMasterReqErrCode eMBMasterSynchronizeDevice( sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, UCHAR bSysnRegHold, UCHAR bSysnRegIn, 
                                                UCHAR bSysnCoil, UCHAR bSysnDiscrete);

#endif