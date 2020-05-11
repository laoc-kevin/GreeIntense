#ifndef _USER_MB_MAP_M_H
#define _USER_MB_MAP_M_H

#include "mbframe.h"
#include "mb_m.h"

//主栈从设备字典申请 
#ifndef MASTER_DEV_DICT_ALLOC
#define MASTER_DEV_DICT_ALLOC

#define PBUF_INDEX_ALLOC() \
        void*             pvDataBuf   = NULL; \
        sMBDevDataTable*  psDataTable = NULL;  \
        uint16_t          usIndex     = 0;
        
//开始数据表申请 
#define BEGIN_DATA_BUF(BUF, TABLE) \
        usIndex = 0; \
        pvDataBuf   = (void*)BUF; \
        psDataTable = (sMBDevDataTable*)TABLE;
  
//保持寄存器数据申请  
#define REG_HOLD_DATA(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
        vMBMasterDevRegHoldDataInit((sMasterRegHoldData*)pvDataBuf + usIndex, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
        usIndex++;
        
//输入寄存器数据申请  
#define REG_IN_DATA(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
        vMBMasterDevRegInDataInit((sMasterRegInData*)pvDataBuf + usIndex, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
        usIndex++;
        
//线圈数据申请  
#define COIL_BIT_DATA(arg1, arg2, arg3, arg4) \
        vMBMasterDevCoilDataInit((sMasterBitCoilData*)pvDataBuf + usIndex, arg1, arg2, arg3, arg4); \
        usIndex++;
        
//离散量数据申请  
#define DISC_BIT_DATA(arg1, arg2, arg3) \
        vMBMasterDevDiscDataInit((sMasterBitCoilData*)pvDataBuf + usIndex, arg1, arg2, arg3); \
        usIndex++;
        
//结束数据表申请  
#define END_DATA_BUF(usStartAddr, usEndAddr)\
        vMBMasterDevDataTableInit(psDataTable, &pvDataBuf, usStartAddr, \
                                  usEndAddr, usIndex);     \
        usIndex = 0; 
 
//测试命令初始化申请  
#define TEST_CMD_INIT(pCmd, arg1, arg2, arg3) \
        vMBMasterDevTestCmdInit((sMBTestDevCmd*)pCmd, arg1, arg2, arg3);    

#endif


eMBMasterReqErrCode eMBMasterRegInMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                      USHORT usRegAddr, sMasterRegInData ** pvRegInValue);

eMBMasterReqErrCode eMBMasterRegHoldingMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                           USHORT usRegAddr, sMasterRegHoldData ** pvRegHoldValue);

eMBMasterReqErrCode eMBMasterCoilMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                     USHORT usCoilAddr, sMasterBitCoilData ** pvCoilValue);

eMBMasterReqErrCode eMBMasterDiscreteMap(sMBMasterInfo* psMBMasterInfo, UCHAR ucSndAddr, 
                                         USHORT usDiscreteAddr, sMasterBitDiscData ** pvDiscreteValue);


void vMBMasterDevRegHoldDataInit(sMasterRegHoldData* pData, USHORT usAddr, UCHAR ucDataType, USHORT usPreVal,
                              LONG lMinVal, LONG lMaxVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue);

void vMBMasterDevRegInDataInit(sMasterRegInData* pData, USHORT usAddr, UCHAR ucDataType, LONG lMinVal, 
                            LONG lMaxVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue);   
                              
                              
void vMBMasterDevCoilDataInit(sMasterBitCoilData* pData, USHORT usAddr, UCHAR ucPreVal, UCHAR ucAccessMode, void* pvValue);

void vMBMasterDevDiscDataInit(sMasterBitDiscData* pData, USHORT usAddr, UCHAR ucAccessMode, void* pvValue);

void vMBMasterDevTestCmdInit(sMBTestDevCmd* pCmd, USHORT usValue, eMasterCmdMode eCmdMode, UCHAR ucAddr); 

void vMBMasterDevTableInit(sMBDevDataTable* pDataTable, void* pvDataBuf, USHORT usStartAddr, USHORT usEndAddr, USHORT usDataCount); 

#endif