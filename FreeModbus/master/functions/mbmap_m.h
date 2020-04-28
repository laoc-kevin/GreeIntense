#ifndef _USER_MB_MAP_M_H
#define _USER_MB_MAP_M_H

#include "mbframe.h"
#include "mb_m.h"

eMBMasterReqErrCode eMBMasterRegInMap(const sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                      USHORT usRegAddr, sMasterRegInData ** pvRegInValue);

eMBMasterReqErrCode eMBMasterRegHoldingMap(const sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                           USHORT usRegAddr, sMasterRegHoldData ** pvRegHoldValue);

eMBMasterReqErrCode eMBMasterCoilMap(const sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                     USHORT usCoilAddr, sMasterBitCoilData ** pvCoilValue);

eMBMasterReqErrCode eMBMasterDiscreteMap(const sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                         USHORT usDiscreteAddr, sMasterBitDiscData ** pvDiscreteValue);

void eMBMasterTableInit(const sMBMasterInfo* psMBMasterInfo);

#endif