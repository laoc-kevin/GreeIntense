#ifndef _USER_MB_MAP_M_H
#define _USER_MB_MAP_M_H

#include "mbframe.h"
#include "mb_m.h"

eMBMasterReqErrCode eMBMasterRegInMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                      USHORT usRegAddr, sMasterRegInData ** pvRegInValue);

eMBMasterReqErrCode eMBMasterRegHoldingMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                           USHORT usRegAddr, sMasterRegHoldData ** pvRegHoldValue);

eMBMasterReqErrCode eMBMasterCoilMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                     USHORT usCoilAddr, sMasterBitCoilData ** pvCoilValue);

eMBMasterReqErrCode eMBMasterDiscreteMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                         USHORT usDiscreteAddr, sMasterBitDiscData ** pvDiscreteValue);


void eMBMasterDevRegHoldDataInit(sMasterRegHoldData* pData, USHORT usAddr, UCHAR ucDataType, USHORT usPreVal,
                              LONG lMinVal, LONG lMaxVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue);

void eMBMasterDevRegInDataInit(sMasterRegInData* pData, USHORT usAddr, UCHAR ucDataType, LONG lMinVal, 
                            LONG lMaxVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue);   
                              
                              
void eMBMasterDevCoilDataInit(sMasterBitCoilData* pData, USHORT usAddr, UCHAR ucPreVal, UCHAR ucAccessMode, void* pvValue);

void eMBMasterDevDiscDataInit(sMasterBitDiscData* pData, USHORT usAddr, UCHAR ucAccessMode, void* pvValue);

void eMBMasterDevTestCmdDaInit(sMBTestDevCmd* pCmd, USHORT usValue, eMasterCmdMode eCmdMode, UCHAR ucAddr); 

void eMBMasterDevDataTableInit(sMBDevDataTable* pDataTable, void* pvDataBuf, USHORT usStartAddr, USHORT usEndAddr, USHORT usDataCount);  
#endif