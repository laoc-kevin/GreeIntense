#include "mbmap.h"
#include "mbdict.h"

#if MB_FUNC_READ_INPUT_ENABLED > 0
/***********************************************************************************
 * @brief  输入寄存器映射
 * @param  usRegInAddr    寄存器地址
 * @param  pvRegInValue   寄存器指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveRegInMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usRegInAddr, sMBSlaveRegData** pvRegInValue)
{
	eMBErrorCode    eStatus = MB_ENOERR;
    
    sMBSlaveCommData*            psCurData  = psMBSlaveInfo->sMBCommInfo.psSlaveCurData;
	const sMBSlaveDataTable* psMBRegInTable = &psCurData->sMBRegInTable;
	
    UCHAR i = psCurData->psMBSlaveDataMapIndex(RegInputData, usRegInAddr);    //从栈字典映射函数
	
	*pvRegInValue = (sMBSlaveRegData*)psMBRegInTable->pvDataBuf + i;
    return eStatus;	
}
#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0 \
    || MB_FUNC_READ_HOLDING_ENABLED > 0 || MB_FUNC_READWRITE_HOLDING_ENABLED > 0

/***********************************************************************************
 * @brief  保持寄存器映射
 * @param  usRegHoldAddr    寄存器地址
 * @param  pvRegHoldValue   寄存器指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveRegHoldMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usRegHoldAddr, sMBSlaveRegData** pvRegHoldValue)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    
    sMBSlaveCommData*              psCurData  = psMBSlaveInfo->sMBCommInfo.psSlaveCurData;
	const sMBSlaveDataTable* psMBRegHoldTable = &psCurData->sMBRegHoldTable;

	UCHAR i = psCurData->psMBSlaveDataMapIndex(RegHoldData, usRegHoldAddr);    //从栈字典映射函数

	*pvRegHoldValue = (sMBSlaveRegData*)psMBRegHoldTable->pvDataBuf + i;
    return eStatus;
}
#endif

#if MB_FUNC_READ_COILS_ENABLED > 0 || MB_FUNC_WRITE_COIL_ENABLED > 0 || MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
/***********************************************************************************
 * @brief  线圈字典映射
 * @param  usCoilAddr    线圈地址
 * @param  pvCoilValue   线圈指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveCoilsMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usCoilAddr, sMBSlaveBitData** pvCoilValue)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    
	sMBSlaveCommData*           psCurData  = psMBSlaveInfo->sMBCommInfo.psSlaveCurData;
	const sMBSlaveDataTable* psMBCoilTable = &psCurData->sMBCoilTable;
    
	UCHAR i = psCurData->psMBSlaveDataMapIndex(CoilData, usCoilAddr);    //从栈字典映射函数
    
	*pvCoilValue = (sMBSlaveBitData*)psMBCoilTable->pvDataBuf + i ;
    return eStatus;
} 

#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0

/***********************************************************************************
 * @brief  离散量字典映射
 * @param  usCoilAddr    线圈地址
 * @param  pvCoilValue   线圈指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveDiscreteMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usDiscreteAddr, sMBSlaveBitData** pvDiscreteValue)
{
    eMBErrorCode    eStatus = MB_ENOERR;

	sMBSlaveCommData*             psCurData  = psMBSlaveInfo->sMBCommInfo.psSlaveCurData;
	const sMBSlaveDataTable* psMBDiscInTable = &psCurData->sMBDiscInTable;

	UCHAR i = psCurData->psMBSlaveDataMapIndex(DiscInData, usDiscreteAddr);    //从栈字典映射函数
    
	*pvDiscreteValue = (sMBSlaveBitData*)psMBDiscInTable->pvDataBuf + i ;
    return eStatus;
}
#endif

#if MB_SLAVE_CPN_ENABLED > 0 
/***********************************************************************************
 * @brief  CPN变量字典映射
 * @param  usCpnName    变量名称
 * @param  pvCPNValue   变量指针
 * @return eMBErrorCode 错误码
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
eMBErrorCode 
eMBSlaveCPNMap(sMBSlaveInfo* psMBSlaveInfo, USHORT usCpnName, sMBSlaveCPNData ** pvCPNValue)
{
	eMBErrorCode    eStatus = MB_ENOERR;
    
    sMBSlaveCommData*          psCurData  = psMBSlaveInfo->sMBCommInfo.psSlaveCurData;
	const sMBSlaveDataTable* psMBCPNTable = &psCurData->sMBCPNTable;

	UCHAR i = psCurData->psMBSlaveDataMapIndex(ValCPNData, usCpnName);    //从栈字典映射函数
    
	*pvCPNValue = (sMBSlaveCPNData*)psMBCPNTable->pvDataBuf + i;
	return eStatus;
}
#endif

/***********************************************************************************
 * @brief  寄存器数据表初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBSlaveRegDataInit(sMBSlaveRegData* pData, USHORT usAddr, UCHAR ucDataType, LONG lMinVal, 
                         LONG lMaxVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue)                                  
{
    pData->usAddr            = usAddr;                    
    pData->ucDataType        = ucDataType;                         
    pData->lMinVal           = lMinVal;                  
    pData->lMaxVal           = lMaxVal;                  
    pData->ucAccessMode      = ucAccessMode;        
    pData->fTransmitMultiple = fTransmitMultiple;
    pData->pvValue           = pvValue;                  	
}

/***********************************************************************************
 * @brief  线圈和离散量数据表初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBSlaveBitDataInit(sMBSlaveBitData* pData, USHORT usAddr, UCHAR ucAccessMode, void* pvValue)                                  
{
    pData->usAddr        = usAddr;                            
    pData->ucAccessMode  = ucAccessMode;        
    pData->pvValue       = pvValue;                  	
}

/***********************************************************************************
 * @brief  CPN数据表初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBSlaveCPNDataInit(sMBSlaveCPNData* pData, USHORT usAddr, UCHAR ucDataType, LONG lMinVal, 
                         LONG lMaxVal, UCHAR ucAccessMode, float fTransmitMultiple, void* pvValue)                                   
{
    pData->usAddr            = usAddr;                    
    pData->ucDataType        = ucDataType;                         
    pData->lMinVal           = lMinVal;                  
    pData->lMaxVal           = lMaxVal;                  
    pData->ucAccessMode      = ucAccessMode;        
    pData->fTransmitMultiple = fTransmitMultiple;
    pData->pvValue           = pvValue;            	
}

/***********************************************************************************
 * @brief 数据表初始化
 * @author laoc
 * @date 2019.01.22
 *************************************************************************************/
void eMBSlaveDevDataTableInit(sMBSlaveDataTable* pDataTable, void* pvDataBuf, USHORT usStartAddr, 
                              USHORT usEndAddr, USHORT usDataCount)                                  
{
    pDataTable->pvDataBuf   = pvDataBuf;        //协议数据域
    pDataTable->usStartAddr = usStartAddr;      //起始地址
    pDataTable->usEndAddr   = usEndAddr;        //末尾地址
    pDataTable->usDataCount = usDataCount;      //协议点位总数
    
}
